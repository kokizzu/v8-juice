include toc2.make

package.dist_files += $(wildcard *.cpp *.cc *.h *.hpp)

OBJECTS := v8-sqlite3.o

########################################################################
# Build DLL...
c-dlls.list := v8-juice-sqlite3
v8-juice-sqlite3.DLL.VERSION := $(V8JUICE_VERSION_DOTTED)
v8-juice-sqlite3.DLL.OBJECTS := $(OBJECTS)
v8-juice-sqlite3.DLL.LDADD := -lsqlite3
include $(toc2.dirs.makefiles)/c-dlls.make
all: $(v8-juice-sqlite3.DLL)
# end DLL rules
########################################################################
