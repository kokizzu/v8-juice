#ifndef code_google_com_p_v8_juice_plugin_CONFIG_HPP_INCLUDED
#define code_google_com_p_v8_juice_plugin_CONFIG_HPP_INCLUDED 1

// Configuration options specific to the v8::juice::plugin layer.

#define v8_juice_CONFIG_ENABLE_PLUGINS 1

#if v8_juice_CONFIG_ENABLE_PLUGINS
#if defined(WIN32)
#  define v8_juice_plugin_CONFIG_PLUGINS_PATH std::string(".")
#  define v8_juice_plugin_CONFIG_HAVE_LIBLTDL 0
#  define v8_juice_plugin_CONFIG_HAVE_LIBDL 0
#  define v8_juice_plugin_CONFIG_DLL_EXTENSIONS std::string(".dll")
#  define v8_juice_plugin_CONFIG_PATH_SEPARATOR std::string(":")
#else
#  define v8_juice_plugin_CONFIG_PLUGINS_PATH std::string(".:/home/stephan/lib/v8/juice/20091016:/home/stephan/lib/v8/juice")
#  define v8_juice_plugin_CONFIG_HAVE_LIBLTDL 0
#  define v8_juice_plugin_CONFIG_HAVE_LIBDL 1
#  define v8_juice_plugin_CONFIG_DLL_EXTENSIONS std::string(".so:.dynlib")
#  define v8_juice_plugin_CONFIG_PATH_SEPARATOR std::string(":")
#endif
#else
// No plugins:
#  define v8_juice_plugin_CONFIG_PLUGINS_PATH std::string()
#  define v8_juice_plugin_CONFIG_HAVE_LIBLTDL 0
#  define v8_juice_plugin_CONFIG_HAVE_LIBDL 0
#  define v8_juice_plugin_CONFIG_DLL_EXTENSIONS std::string()
#  define v8_juice_plugin_CONFIG_PATH_SEPARATOR std::string(":")
#endif

#endif // code_google_com_p_v8_juice_plugin_CONFIG_HPP_INCLUDED
