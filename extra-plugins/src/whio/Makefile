########################################################################
# set WHIO_USE_WHEFS to 0 for platforms where the whefs bits won't
# build.
WHIO_USE_WHEFS := 1

CPPFLAGS += -DWHIO_PLUGIN_ENABLE_WHEFS=$(WHIO_USE_WHEFS)

PLUGIN_NAME := whio

PLUGIN_OBJECTS := v8-whio.o
ifeq (1,$(WHIO_USE_WHEFS))
  PLUGIN_OBJECTS += v8-whefs.o whefs_amalgamation.o
  whefs_amalgamation.o: CFLAGS+=-std=c99 -fPIC
else
  PLUGIN_OBJECTS += whio_amalgamation.o
  whio_amalgamation.o: CFLAGS+=-std=c99 -fPIC
endif

ifeq (1,1)
    CPPFLAGS += -DWHDBG_CONFIG_ENABLE=0
    CPPFLAGS += -DWHIO_ENABLE_ZLIB=1
    PLUGIN_LDADD := -lz
endif
include ../plugin_template.make
