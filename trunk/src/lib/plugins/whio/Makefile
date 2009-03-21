include toc2.make
PLUGIN_NAME := whio
PLUGIN_OBJECTS := v8-whio.o whio_amalgamation.o
ifeq (1,1)
CPPFLAGS += -DWHIO_ENABLE_ZLIB=1
PLUGIN_LDADD := -lz
endif
include ../plugin_template.make
