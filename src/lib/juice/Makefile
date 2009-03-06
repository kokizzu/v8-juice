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