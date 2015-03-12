(This is now part of [the stringfuncs plugin](PluginStringFuncs.md).)

20090325: sprintf was copied back into the core lib because it'll be useful from several plugins.

# `sprintf()` for JavaScript #

The files `jssprintf.{h,cc}` contain an implementation of sprintf() for JS. It is independent of the rest of the library - those two files can be plugged in to arbitrary v8-based source trees.

To include it in your C++ application, see the instructions in the header file. From JS, it is used just like printf(), but instead of printing the result somewhere it returns the result as a string. For example:

```
print("Short demonstration of sprintf():");
var obj = {hi:32};
print(sprintf("hi, %s!","world"));
print(sprintf("hi, %s!", obj));
print(sprintf("%d %05d", 42, 42));
print(sprintf("%d %10d %10c %c %15f", 42, 42, 'a', 97, 42.24));
```

That outputs:

```
Short demonstration of sprintf():
hi, world!
hi, [object Object]!
42 00042
42         42          a a       42.240000
```

The function supports a few extension formatting options, such as `%h` to escape HTML, `%q` and `%Q` for SQL escaping, and `%t/%T` for URL encoding/decoding.

It might be interesting to know that the native implementation code was derived from:
http://fossil.wanderinghorse.net/repos/whprintf/