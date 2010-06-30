# Automatically generated by configure - do not modify
CONFIG_QEMU_PREFIX="/usr/gnemul/qemu-sparc"
TARGET_ARCH=sparc
TARGET_SPARC=y
TARGET_ARCH2=sparc
TARGET_BASE_ARCH=sparc
TARGET_ABI_DIR=sparc
TARGET_PHYS_ADDR_BITS=64
TARGET_WORDS_BIGENDIAN=y
CONFIG_SOFTMMU=y
LIBS+=-lutil -lbluetooth   -lncurses -lbrlapi  -lvdeplug -lsasl2 -lgnutls   -L/usr/lib -lSDL -lX11 
HWLIB=../libhw64/libqemuhw64.a
CONFIG_SOFTFLOAT=y
CONFIG_I386_DIS=y
CONFIG_SPARC_DIS=y
LDFLAGS+=
QEMU_CFLAGS+=-I$(SRC_PATH)/fpu -I$(SRC_PATH)/tcg -I$(SRC_PATH)/tcg/$(ARCH) 
