

# v8-juice Plugins #

v8-juice provides an API for loading DLLs with the intent of adding functionality to a running JS engine.

List of current plugins:

  * An object-oriented [ncurses wrapper](PluginNCurses.md).
  * A minimalistic [GNU Readline/BSD Editline wrapper](PluginReadline.md)
  * An event-driven [XML parser](PluginExpatParser.md)
  * A [libcurl wrapper](PluginCurl.md).
  * [sqlite3 bindings](PluginSQLite3.md)
  * [supplemental string functions](PluginStringFuncs.md) (e.g. for escaping and quoting strings).
  * [i/o device and stream classes](PluginWhio.md), with an optional [embedded filesystem component](PluginWhefs.md).
  * [The PosixFILE plugin](PluginPosixFILE.md) provides an i/o API almost identical to the Posix `FILE` API for C.

The plugins live in
[the extra-plugins source repository](http://code.google.com/p/v8-juice/source/browse/extra-plugins), and can be downloaded from there. See the
`README` file at the top of the source tree for build and installation
instructions.

If your copy of juice is configured without plugins, the API for loading
plugins will still be there (for consistency and binary compatibility), but
the functions won't do anything useful.

# Building the plugins #

First off, be warned: the plugin model used by v8-juice _cannot_ work with static libraries. The plugin model relies on code being executed during the static initialization phase (when an app/dll is opened), and that phase doesn't happen for symbols in static libraries. Plugins _must_ be built as shared libraries.

**On Linux platforms**, applications using v8-juice plugins must be linked with the `-export-dynamic` (a.k.a. `-rdynamic`) linker option. If they are not, the plugins will not be able to find the v8-juice symbols they need in order to register themselves with the client application. **On Windows platforms**, the "keep unreferenced data" compilation option _must_ be enabled or plugin registrations cannot work (for the same reason statically linking them doesn't work). In MSVC this option is called `/OPT:NOREF`. When building with MSVC, be sure to turn _off_ precompiled headers, as they have been shown to somehow interfere with the plugin model.

There are two options for building the plugins, both of which require that v8-juice is installed and that its headers can be found.

For scons: edit the `SConstruct` file to tell it where your v8/v8-juice live. Then simply run `scons` from the top of the build tree. That will build all of the plugins. Simply copy them to `prefix/lib/v8/juice`, where `prefix` is explained in the build options, below.

The `make` build requires that v8-juice be installed and that `libv8-juice-config` be in the `$PATH` (it is installed via the normal v8-juice installation process). If those conditions are met one can do:

```
~> cd src/pluginName
~> make
~> make install
```

It will install to `$(libv8-juice-config --prefix)/lib/v8/juice/VERSION`, where `VERSION` is the version of v8-juice it is compiled against. This directory is in the default v8-juice plugins search path.

# C++-side #

To be able to load new plugins from JS code, we first have to add a small amount of C++ code.

First off, we need to add a plugin-loading function to our global JS object:

```
#include <v8/juice/plugin.h>
...
// when initializing the JS global object, add this function:
gl->Set(v8::String::New("loadPlugin"),
        v8::FunctionTemplate::New(v8::juice::plugin::LoadPlugin)->GetFunction() );
```

That will create the loader function (name it whatever you like) in the specified object (it need not be the global object).

That's all you need to do to enable the plugins support.

On Linux systems (and probably other Unices), your application must be linked
with the `-export-dynamic` (also known as `-rdynamic`) flag, or DLLs may not
be able to load properly (they won't be able to see the native v8::juice::plugin API provided by the application/v8-juice!).

Now, in the code which you want to link as a DLL you need to add a few small
bits of code. First, we will assume you have a function with this signature:

```
v8::Handle<v8::Value> SetupMyAddon( v8::Handle<v8::Object> target );
```

That exact signature is not technically required, but it is a convenient
convention and it is in fact required by the macro-based plugin initialization
approach shown below.

That routine should add the plugin's functionality, treating the given target
object as the logical global object (whether it is or is not the real global
object is up to the caller). On success it should return a non-exception.

Now we set up a trigger which will call that init routine whenever the DLL is
opened by adding the following macro somewhere in our implementation file (it
doesn't matter where, as long as it's got file- or namespace-level scope):

```
V8_JUICE_PLUGIN_STATIC_INIT(SetupMyAddon);
```

Now, whenever `loadPlugin("MyPlugin")` opens the DLL, the initialization code
will be run automatically and it can set up its JS state. Optionally, we can
pass an Object as a second argument to `loadPlugin()` and that object will be
used as the logical global object. Multiple plugins may live inside a single
DLL, and the above macro needs to be called one time per plugin initialization
function. Opening a plugin multiple times will not trigger the initialization
more than once (because it happens during the static initialization phase
of the DLL opening process).

The function name passed to the macro may not be namespace-qualified because its name is used as part of a generated variable's name (to allow multiple registrations per source file).

Note that there is nothing magical about the `V8_JUICE_PLUGIN_STATIC_INIT` macro. Any code which is functionally equivalent to what it generates will do.

# JS-side #

```
loadPlugin("MyPlugin");
// or:
var p2 = {};
loadPlugin("MyOtherPlugin",p2); // will pass p2 as the target object
```

`loadPlugin()` will throw on error and the exception will explain the problem, which is normally one of:

  * No plugin could be found in the current plugins search path.
  * A plugin was found but opening it failed (e.g. missing DLL symbols are sometimes problematic, and may indicate version incompatibility).
  * The DLL initialization code threw a JS exception.

# The plugins search path #

From native code, you can use the `v8::juice::plugin::PluginsPath()` function
to get access to a shared `v8::juice::PathFinder` object which is used to do
the actual searches for plugins. The plugins path is accessible via JS code if
it has been explicitly added using `v8::juice::SetupPathFinderClass()`. Once it is added to the JS engine, the plugins path is accessible via the shared JS object
`PathFinder.shared.plugins` (it uses the same native instance returned by the
`PluginsPath()` function).

For example:

```
print(PathFinder.shared.plugins.find("MyPlugin"));
// ^^^ searches for MyPlugin.(dll|so) but does not open it
```


See [PathFinderClass](PathFinderClass.md) for the complete JS API docs for the PathFinder class.

The default plugins search path is:
```
.
PREFIX/lib/v8/juice/JUICE_VERSION
PREFIX/lib/v8/juice
```

Where `PREFIX` is the installation prefix (see [BuildingTheCode](BuildingTheCode.md)) and
`JUICE_VERSION` is the version of the library (set during the configuration
process).


# DLLs are _never_ unloaded #

Even if the DLL init code throws an exception, DLLs are _never_ unloaded. It cannot be safely done. Period. Don't argue with me on this. i'm right, for once, so just let me be right.

Here's a pasted-in email which goes into a bit more detail about my fanatical beliefs on this topic:

```
On Thu, Mar 5, 2009 at 6:15 PM, Slide xxxxxxxxx wrote:
> I was reading your article "Classloading in C++: Bringing classloading
> into the 21st century" and was wondering if your classloader scheme
> also supports unloading of shared library and unregistering the class
> from the loader.

Hi, Alex!

That's a good question: my basic philosphical view is that unloading
is NEVER safe in the generic sense. Merely opening a DLL can trigger
abitrary code via static initialization, and that code can allocate
resources which may not get cleaned up when the DLL is unloaded (they
may also load other DLLs and then not close them (and they can't do so
safely because they can't know who's using the resources in that
DLL)). Doing even the most basic cleanup requires doing some symbol
lookup in the DLL to find the dtor function, cast it to the type you
want, and call it (which technically has undefined results if you cast
to a function pointer derived from a dlsym() `void *`, though it's
likely to work on most platforms). Even with such a system in place,
it's not generically impossible to rule out that the DLL has allocated
resources inside the main application which would no longer be valid
if the DLL is closed. When we consider that a DLL can itself open
other DLLs, and those DLLs may feed data or offer services to the main
application, the problem gets worse.

So, the short answer is, i've never added unload support. The first
variant i worked on did, and our app crashed continually when we
unloaded DLLs, so we quickly removed it. After thinking about it for
several years, i'm 100% convinced that no generic framework can make
any safety guarantees for an unload, and even the most rigorous
approaches to DLL unload can fail if any single init routine allocates
memory outside of the DLL's address space (e.g. registered a classloading
callback function in the main app) and fails to clean it up. If any
client of that DLL managed to copy a pointer which was, unbeknownst to
that client, generated from a DLL, the client will crash when he then
references that pointer. It's just not safe.
```
(The article here's referring to is here: http://wanderinghorse.net/computing/papers/)