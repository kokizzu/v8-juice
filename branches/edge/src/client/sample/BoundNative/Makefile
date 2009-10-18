#package.subdirs := js
default: all
include toc2.make

package.dist_files += $(wildcard *.cpp *.hpp *.js *.cc *.h)
SOURCES = demo.cc

c-bins.list := BoundNative

BoundNative.BIN.OBJECTS := demo.o MyNative.o
#$(error x=$(V8_LDFLAGS))
BoundNative.BIN.LDADD = $(LIBV8JUICE_CLIENT_INTREE_LDADD) $(V8_LDFLAGS)
#$(error BoundNative.BIN.LDADD=$(BoundNative.BIN.LDADD))
# package.install.bins := $(BoundNative.BIN)
include $(toc2.dirs.makefiles)/c-bins.make
all: $(BoundNative.BIN)

#package.install.man1 = BoundNative.1
#subdirs: subdir-js
#all: subdirs
