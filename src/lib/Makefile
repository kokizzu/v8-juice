#!/usr/bin/make -f

include toc2.make

########################################################################
# Reminder: set package.subdirs so that cleanup rules will work:
package.subdirs = juice plugins
#subdir-s11n: subdir-plugin
#subdir-io: subdir-s11n
#subdir-proxy: subdir-s11n
#subdir-lite: subdir-io subdir-proxy
#subdir-addons subdir-unit subdir-client subdir-nodeutil: subdir-lite
subdirs: $(addprefix subdir-,$(package.subdirs))
all: subdirs
subdir-plugins: |subdir-juice
#package.dist_files += $(wildcard in.*) test.cpp
# Makefile.for.generated.src.tree

