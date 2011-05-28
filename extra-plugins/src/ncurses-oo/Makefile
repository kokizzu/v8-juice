#!/usr/bin/make -f
########################################################################
# A standalone makefile for ncutil which does not rely on a specific
# build tree.
#
# The ncutil "should" build using only this makefile, assuming
# you have ncurses++ installed in your system paths.
#
# Requires GNU make and might only work with gcc. Dunno.
########################################################################
PLUGIN_NAME := ncurses-oo
PLUGIN_OBJECTS := $(addsuffix .o, \
	ncpalette \
	nckey \
	ncmode \
	ncpad \
	ncpanel \
	ncutil \
	ncwidgets \
	ncwindow \
	v8-nc \
	)
PLUGIN_LDADD := -lpanel -lncurses
include ../plugin_template.make

#PALETTE_SH = createCursesPalette.sh
#PALETTE_CPP = ncpalette.cpp ncpalette.hpp
#CLEAN_FILES += $(PALETTE_CPP)
#$(PALETTE_CPP): $(PALETTE_SH)
#	bash ./$(PALETTE_SH)
#ncpalette.o: ncpalette.cpp ncpalette.hpp

CURSES_LDADD = -lncurses -lpanel
#ncdemo_OBJECTS = $(OBJECTS) ncdemo.o
#ncdemo: $(ncdemo_OBJECTS)
#	$(CC) -o $@ $(ncdemo_OBJECTS) \
#	$(CURSES_LDADD) -lstdc++
#CLEAN_FILES += $(ncdemo_OBJECTS) ncdemo
#include basics.make
#DIST_FILES := $(sort $(DIST_FILES) basics.make sigslot.make $(SOURCES) $(HEADERS) $(PALETTE_SH))
# ^^^ sort removes dupes, because we get dupes by including sigslot.make
#all: ncdemo
