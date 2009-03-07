include toc2.make

package.dist_files += $(wildcard *.cpp *.cc *.h *.hpp)

HEADERS = 

OBJECTS = $(addsuffix .o,\
	nccolors \
	ncmode \
	ncstream \
	v8-ncurses \
	)

########################################################################
# Build DLL...
c-dlls.list := v8-juice-ncurses
v8-juice-ncurses.DLL.VERSION := $(V8JUICE_VERSION_DOTTED)
v8-juice-ncurses.DLL.OBJECTS := $(OBJECTS)
v8-juice-ncurses.DLL.LDADD := -lncurses -lpanel
include $(toc2.dirs.makefiles)/c-dlls.make
all: $(v8-juice-ncurses.DLL)
# end DLL rules
########################################################################
