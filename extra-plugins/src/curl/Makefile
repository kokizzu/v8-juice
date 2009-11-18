CURL_PREFIX := $(HOME)
PLUGIN_NAME := libcurl
PLUGIN_OBJECTS := CurlJS.o
PLUGIN_LDADD := -L$(CURL_PREFIX)/lib -lcurl
CPPFLAGS += -I$(CURL_PREFIX)/include
include ../plugin_template.make
