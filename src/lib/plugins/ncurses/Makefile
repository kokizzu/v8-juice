include toc2.make
PLUGIN_NAME := ncurses
PLUGIN_OBJECTS := $(addsuffix .o,\
	nccolors \
	ncmode \
	ncstream \
	v8-ncurses \
	)
PLUGIN_LDADD := -lncurses -lpanel
include ../plugin_template.make
