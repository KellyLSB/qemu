#ifndef _IPXE_HYPERV_H
#define _IPXE_HYPERV_H

/** @file
 *
 * Hyper-V interface
 *
 */

FILE_LICENCE ( GPL2_OR_LATER_OR_UBDL );

#include <stdint.h>
#include <ipxe/io.h>

/** Hyper-V interface identification */
#define HV_INTERFACE_ID 0x31237648 /* "Hv#1" */

/** Guest OS identity for iPXE
 *
 * This field comprises:
 *
 * Bit  63    : set to 1 to indicate an open source OS
 * Bits 62:56 : OS Type
 * Bits 55:48 : OS ID
 * Bits 47:16 : Version
 * Bits 15:0  : Build number
 *
 * There appears to be no central registry for the "OS Type".  The
 * specification states that "Linux is 0x100", and the FreeBSD source
 * states that "FreeBSD is 0x200".  Both of these statements are
 * actually referring to the combined "OS Type" and "OS ID" field.
 *
 * We choose to use 0x98ae: this is generated by setting bit 63 (to
 * indicate an open source OS) and setting the OS Type+ID equal to the
 * PnP vendor ID used in romprefix.S.  No version information or build
 * number is included.
 */
#define HV_GUEST_OS_ID_IPXE ( ( 1ULL << 63 ) | ( 0x18aeULL << 48 ) )

/** Enable hypercall page */
#define HV_HYPERCALL_ENABLE 0x00000001UL

/** Enable SynIC */
#define HV_SCONTROL_ENABLE 0x00000001UL

/** Enable SynIC event flags */
#define HV_SIEFP_ENABLE 0x00000001UL

/** Enable SynIC messages */
#define HV_SIMP_ENABLE 0x00000001UL

/** Perform implicit EOI upon synthetic interrupt delivery */
#define HV_SINT_AUTO_EOI 0x00020000UL

/** Mask synthetic interrupt */
#define HV_SINT_MASKED 0x00010000UL

/** Synthetic interrupt vector */
#define HV_SINT_VECTOR(x) ( (x) << 0 )

/** Synthetic interrupt vector mask */
#define HV_SINT_VECTOR_MASK HV_SINT_VECTOR ( 0xff )

/** Post message */
#define HV_POST_MESSAGE 0x005c

/** A posted message
 *
 * This is the input parameter list for the HvPostMessage hypercall.
 */
struct hv_post_message {
	/** Connection ID */
	uint32_t id;
	/** Padding */
	uint32_t reserved;
	/** Type */
	uint32_t type;
	/** Length of message */
	uint32_t len;
	/** Message */
	uint8_t data[240];
} __attribute__ (( packed ));

/** A received message
 *
 * This is the HV_MESSAGE structure from the Hypervisor Top-Level
 * Functional Specification.  The field order given in the
 * documentation is incorrect.
 */
struct hv_message {
	/** Type */
	uint32_t type;
	/** Length of message */
	uint8_t len;
	/** Flags */
	uint8_t flags;
	/** Padding */
	uint16_t reserved;
	/** Origin */
	uint64_t origin;
	/** Message */
	uint8_t data[240];
} __attribute__ (( packed ));

/** Signal event */
#define HV_SIGNAL_EVENT 0x005d

/** A signalled event */
struct hv_signal_event {
	/** Connection ID */
	uint32_t id;
	/** Flag number */
	uint16_t flag;
	/** Reserved */
	uint16_t reserved;
} __attribute__ (( packed ));

/** A received event */
struct hv_event {
	/** Event flags */
	uint8_t flags[256];
} __attribute__ (( packed ));

/** A monitor trigger group
 *
 * This is the HV_MONITOR_TRIGGER_GROUP structure from the Hypervisor
 * Top-Level Functional Specification.
 */
struct hv_monitor_trigger {
	/** Pending events */
	uint32_t pending;
	/** Armed events */
	uint32_t armed;
} __attribute__ (( packed ));

/** A monitor parameter set
 *
 * This is the HV_MONITOR_PARAMETER structure from the Hypervisor
 * Top-Level Functional Specification.
 */
struct hv_monitor_parameter {
	/** Connection ID */
	uint32_t id;
	/** Flag number */
	uint16_t flag;
	/** Reserved */
	uint16_t reserved;
} __attribute__ (( packed ));

/** A monitor page
 *
 * This is the HV_MONITOR_PAGE structure from the Hypervisor Top-Level
 * Functional Specification.
 */
struct hv_monitor {
	/** Flags */
	uint32_t flags;
	/** Reserved */
	uint8_t reserved_a[4];
	/** Trigger groups */
	struct hv_monitor_trigger trigger[4];
	/** Reserved */
	uint8_t reserved_b[536];
	/** Latencies */
	uint16 latency[4][32];
	/** Reserved */
	uint8_t reserved_c[256];
	/** Parameters */
	struct hv_monitor_parameter param[4][32];
	/** Reserved */
	uint8_t reserved_d[1984];
} __attribute__ (( packed ));

/** A synthetic interrupt controller */
struct hv_synic {
	/** Message page */
	struct hv_message *message;
	/** Event flag page */
	struct hv_event *event;
};

/** A message buffer */
union hv_message_buffer {
	/** Posted message */
	struct hv_post_message posted;
	/** Received message */
	struct hv_message received;
	/** Signalled event */
	struct hv_signal_event signalled;
};

/** A Hyper-V hypervisor */
struct hv_hypervisor {
	/** Hypercall page */
	void *hypercall;
	/** Synthetic interrupt controller (SynIC) */
	struct hv_synic synic;
	/** Message buffer */
	union hv_message_buffer *message;
	/** Virtual machine bus */
	struct vmbus *vmbus;
};

#include <bits/hyperv.h>

/**
 * Calculate the number of pages covering an address range
 *
 * @v data		Start of data
 * @v len		Length of data (must be non-zero)
 * @ret pfn_count	Number of pages covered
 */
static inline unsigned int hv_pfn_count ( physaddr_t data, size_t len ) {
	unsigned int first_pfn = ( data / PAGE_SIZE );
	unsigned int last_pfn = ( ( data + len - 1 ) / PAGE_SIZE );

	return ( last_pfn - first_pfn + 1 );
}

extern __attribute__ (( sentinel )) int
hv_alloc_pages ( struct hv_hypervisor *hv, ... );
extern __attribute__ (( sentinel )) void
hv_free_pages ( struct hv_hypervisor *hv, ... );
extern void hv_enable_sint ( struct hv_hypervisor *hv, unsigned int sintx );
extern void hv_disable_sint ( struct hv_hypervisor *hv, unsigned int sintx );
extern int hv_post_message ( struct hv_hypervisor *hv, unsigned int id,
			     unsigned int type, const void *data, size_t len );
extern int hv_wait_for_message ( struct hv_hypervisor *hv, unsigned int sintx );
extern int hv_signal_event ( struct hv_hypervisor *hv, unsigned int id,
			     unsigned int flag );

#endif /* _IPXE_HYPERV_H */
