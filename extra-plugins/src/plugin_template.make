#!/do/not/make
# To be included by plugin makefiles
########################################################################
# Before including this file define:
#  PLUGIN_NAME=the base plugin name (e.g. myplugin)
#  PLUGIN_OBJ=list of .o files for the plugin
#  PLUGIN_LDADD=optional list of extra linker arguments
########################################################################
ifeq (,$(PLUGIN_NAME))
$(error PLUGIN_NAME must be defined before including this file!)
endif

ifeq (,$(PLUGIN_OBJECTS))
$(error PLUGIN_OBJECTS must be defined before including this file!)
endif

PACKAGE.NAME = libv8-plugin-$(PLUGIN_NAME)
PACKAGE.VERSION := $(shell date +%Y%m%d)
ShakeNMake.DOXYGEN.GENERATE_LATEX := NO
ShakeNMake.DOXYGEN.USE_DOT := 0
include ../shake-n-make.make

jconfig := $(call ShakeNMake.CALL.FIND_BIN,libv8-juice-config)
ifeq (,$(jconfig))
$(error libv8-juice-config was not found in the PATH!)
endif

CXXFLAGS += -g3 -Wall
CPPFLAGS += $(shell $(jconfig) --includes)
JUICE_VERSION := $(shell $(jconfig) --version)

#package.dist_files += $(wildcard *.cpp *.cc *.h *.hpp *.c)
#package.install.dlls.dest := $(package.install.package_dlls.dest)# override so plugins get installed here
########################################################################
# Build DLL...
PLUGIN_DLL_NAME := v8-juice-$(PLUGIN_NAME)
c-dlls.list := $(PLUGIN_DLL_NAME)
#$(PLUGIN_DLL_NAME).DLL.VERSION := $(V8JUICE_VERSION_DOTTED)
$(PLUGIN_DLL_NAME).DLL.OBJECTS := $(PLUGIN_OBJECTS)
$(PLUGIN_DLL_NAME).DLL.LDADD ?= $(PLUGIN_LDADD)

$(PLUGIN_DLL_NAME).DLL.LDFLAGS := $(PLUGIN_LDADD)
$(call ShakeNMake.CALL.RULES.DLLS,$(PLUGIN_DLL_NAME))
all: $($(PLUGIN_DLL_NAME).DLL)

INSTALL.PLUGIN := $($(PLUGIN_DLL_NAME).DLL)
INSTALL.PLUGIN.DEST := $(shell $(jconfig) --prefix)/lib/v8/juice/$(JUICE_VERSION)
$(call ShakeNMake.CALL.RULES.INSTALL,PLUGIN)

CLEAN_FILES += $(wildcard *~)
# end DLL rules
########################################################################
