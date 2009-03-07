#!/usr/bin/make -f

include toc2.make

#package.subdirs := lib
package.subdirs_FIRST += lib
package.subdirs_SECOND = client
package.subdirs = $(package.subdirs_FIRST) $(package.subdirs_SECOND)
subdirs: $(addprefix subdir-,$(package.subdirs))
all: subdirs
#$(package.subdirs_SECOND): |$(package.subdirs_FIRST)
subdir-client: |subdir-lib

#package.dist_files += $(wildcard in.*) test.cpp
# Makefile.for.generated.src.tree

