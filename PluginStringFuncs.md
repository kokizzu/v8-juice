This plugin provides the following JS-side functions:

  * `escape_js_string()` escapes a JS string.
  * `quote_js_string()` works like `escape_js_string()` but also adds quotes surrounding the string.
  * <tt><a href='PluginSprintf.md'>snprintf()</a></tt>, for formatted output in classical `printf()` style.

Reminder to self: port in the glob() and like() string matching functions from [the whefs tree](http://fossil.wanderinghorse.net/repos/whefs).