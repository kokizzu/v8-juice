

# PathFinder Class #

The `v8::juice::PathFinder` class is responsible for doing all file searching
which v8-juice does. This currently means looking for plugins and external scripts, but could just as easily mean finding client-side documents or image files to process. This class has been used for a number of years in other projects, and has proven to be very useful.

The class can be added to a JS engine by calling `v8::juice::SetupPathFinderClass()`, declared in [PathFinder.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/PathFinder.h).

# JS API #

**ACHTUNG: the API changed significantly on 2009 Oct 16 ([r722](https://code.google.com/p/v8-juice/source/detail?r=722)),** to take advantage of the new feature allowing us to bind JS properties to native getter/setter functions. All of the getter functions are now prefixed with "get", and the older names now refer to read/write properties which behave identically to the getter/setter functions.

| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| `PathFinder` | `new PathFinder( [string path=''], [string extensions=''], [string delimiter=':'])` | Creates a new PathFinder with the given search path, extensions, and delimiter. |
| `String` | `getPathString()` | Search path as a delimited string. |
| `int` | `setPathString( String )`  | Sets the search path. May be delimited. e.g. `setPathString("/lib:/usr/lib")`|
| `Array` | `getPathArray()` | Search path as an array. |
| `int` | `setPathArray( Array )`  | Sets search path to the given list of dirs. |
| `String` | `getPathSeparator()` | The delimiter used to split/join path lists. |
| `void` | `setPathSeparator( character )`  | Sets path delimiter used to split/join paths. Functions denoted as using a "delimited list" use this delimiter. |
| `Array` | `getExtensionsArray()` | List of search extensions, as an array. |
| `int` | `setExtensionsArray( Array )`  | Sets list of search extensions. |
| `String` | `getExtensionsString()` | List of search extensions as a delimited string. |
| `int` | `setExtensionsString( String )`  | Set list of search extensions as a delimited string. e.g. `setExtensionsString(".exe:.bat:.com")` |
| `void` | `addPathString( String )`  | Adds a dir (or delimited list) to the search path. e.g. `addPathString("/usr/local/bin:/home/me/bin")` |
| `void` | `addExtensionString( String )`  | Adds an extension (or delimited list) to the extensions search list. |
| `string` | `baseName(string)` | (Added 20091030) Returns the part of the given filename after the last directory separator. Also a class ("static") method. |
| `void` | `clearCache()` | Clears the internal file lookup cache which is used by `find()`. |
| `String` | `find( String basename )`  | Searches for the given file. It will search through all combinations of path+name+extension to try to find the file. |
| `bool` | `isEmpty()` | Returns true only if this object has no path/search information. |
| `bool` | `isAccessible(string)` | Returns true if the given filename is accessible (readable). This is both a static and per-instance function. |
| **Type** | **Properties** (read-write) | **Notes** |
| COPY OF `Array-of-strings` | `pathArray` | See `get/setPathArray()` |
| COPY OF `Array-of-strings` | `extensionsArray` | See `get/setExtensionsArray()` |
| `string` | `extensionsString` | See `get/setExtensionsString()` |
| `string` | `pathSeparator` | See `get/setPathSeparator()` |
| `string` | `pathString` | See `get/setPathString()` |
| **Type** | **Properties** (read-only) | **Notes** |
| `string` | `dirSeparator` (read-only) | The platform-specific character used to separate directory names in a path. This is `\` on Windows and `/` everywhere else. |

**ACHTUNG:** Note that because the values in a PathFinder object are internally native types, it is not possible to directly change them via JS objects. e.g. `obj.pathArray.push('/home')` _will have no effect_, as the object returned from `pathArray` is necessarily a _copy_ of the native data. Thus holding references to them will not work as expected.

The various properties and functions with the suffixes `Array` and `String` (e.g. `pathArray` and `pathString`) can be used like so:

```
obj.pathString = "/usr/local/bin:/usr/bin";
// Is equivalent to:
obj.setPathString( "/usr/local/bin:/usr/bin" );
// Is equivalent to:
obj.pathArray = ["/usr/local/bin", "/usr/bin"];
// Is equivalent to:
obj.setPathArray( '/usr/local/bin', '/usr/bin' );
```

And setting one property will change the value of the other (but see the caveat above about not being able to directly change these properties!).

# Shared PathFinder Instances #

When the class is added to an object, the setup routine also creates a couple
shared PathFinder objects, accessible via the shared JS object
`PathFinder.shared`. The list of shared objects is:

  * `PathFinder.shared.plugins` = the same object as `v8::juice::plugin::PluginsPath()`.
  * `PathFinder.shared.include` = the same object as `v8::juice::ScriptsPath()`.

In both cases, JS and native code refer to the same underlying shared instances. Thus changes made to the object via the native or JS APIs will affect both native and JS-side usage of those object.

# Examples #

```
print(PathFinder.shared.plugins.find("MyPlugin"));
// ^^^ searches for MyPlugin.(dll|so) but does not open it

var binsearch = new PathFinder("/usr/bin:/bin");
print(binsearch.find('ls')); // prints: "/bin/ls"
print(binsearch.find('gzip')); // prints: "/usr/bin/gzip"

var libsearch = new PathFinder("/lib:/usr/lib", ".so:.a");
print(libsearch.find("libz")); // prints "/usr/lib/libz.so"
```

# TODOs #

  * The ctor should allow arrays or strings for the various arguments.
  * We really should do away with the delimited strings approach and go all-array, as we're just asking for problems on Windows platforms with ":"-delimited paths. i don't use Windows, so i'm not gonna worry about that too much.