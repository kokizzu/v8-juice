include toc2.make

package.dist_files += $(wildcard *.cpp *.cc *.h *.hpp)

HEADERS = 

OBJECTS = $(addsuffix .o,\
	PathFinder \
	PathFinder-js \
	v8-cleanup \
	v8-convert \
	v8-plugin \
	)
#package.dist_files += $(wildcard *.at) $(HEADERS)
#package.distclean_files += s11n_config.hpp
#HEADERS_RELPATH = include/s11n.net/s11n
#package.install.headers = $(HEADERS) s11n_config.hpp
#package.install.headers.dest = $(prefix)/$(HEADERS_RELPATH)
#symlink-files.list := $(package.install.headers)
#symlink-files.dest := $(toc2.top_srcdir)/$(HEADERS_RELPATH)
#include $(toc2.dirs.makefiles)/symlink-files.make

#$(OBJECTS): symlink-files
#%.o: symlink-files
#all: $(OBJECTS)
########################################################################
# Reminder: set package.subdirs so that cleanup rules will work.
# And make sure subdirs are built AFTER everything else.
#package.subdirs := 0x
#all: symlink-files $(OBJECTS) $(addprefix subdir-,$(package.subdirs))
all: $(OBJECTS)

########################################################################
# Build DLL...
c-dlls.list := libv8-juice
libv8-juice.DLL.VERSION := $(shell echo $(package.version) | perl -pe 's|(\d\d\d\d)(\d\d)(\d\d)|$$1.$$2.$$3|')
libv8-juice.DLL.OBJECTS = $(OBJECTS)
#libv8-juice.DLL.LDADD = $(LIBV8JUICE_LDADD) 
#ifeq (1,$(v8juice_CONFIG_ENABLE_PLUGINS))
#  libv8-juice.DLL.OBJECTS += plugin/plugin.o plugin/path_finder.o
#endif
include $(toc2.dirs.makefiles)/c-dlls.make
# end DLL rules
########################################################################

libv8-juice.LIB.OBJECTS := $(libv8-juice.DLL.OBJECTS)
$(eval $(call toc2.eval.rules.c-lib,libv8-juice))

all: $(libv8-juice.LIB)
all: $(libv8-juice.DLL)
