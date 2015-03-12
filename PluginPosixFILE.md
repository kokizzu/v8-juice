**See also:** [Plugins](Plugins.md)



# Introduction #

(Added in commit [r1112](https://code.google.com/p/v8-juice/source/detail?r=1112) (20100318).)

The `PosixFILE` plugin started out as a test/demonstration of the [ClassWrap](ClassWrap.md) API, but it turns out it's useful in its own right. It provides an API nearly identical to the POSIX-standard `FILE` API, and it provides both C-like and OO-style interfaces (which can be used together). This plugin is different from [the whio plugin](PluginWhio.md): whio provides a more abstract i/o interface, but this one deals only with the POSIX-defined APIs for `FILE` handles.

**ACHTUNG: DO NOT USE THIS API WITH BINARY DATA!** Binary data _cannot_ be safely passed between the v8 engine and JS code. The engine will, if it sees any non-ASCII characters, assume the data is UC16 and will convert it accordingly. This is not my bug, but a trait of the v8 engine.

## Loading from JS ##

Install the plugin by following the instructions on the [Plugins](Plugins.md) page.

Then simply do:

```
loadPlugin('v8-juice-PosixFILE');
```

## Loading from C++ ##

The class can be added to a JS engine by linking in
[the relevant code](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/PosixFILE)
into your app and calling `PosixFILE::SetupBindings()`, or it can be dynamically loaded using `v8::juice::plugin::LoadPlugin()`.

# C-style API #

The C-style API functions are as close to the real C API as JS calling conventions allow for. All of the following functions and properties are members of an object named `posix` (that object is created by the plugin loading process if it does not already exist).

| **Return Type** | **Signature** | **Notes** |
|:----------------|:--------------|:----------|
| `FILE` | `fopen(string filename, string mode)` | See `man fopen`. |
| `bool` | `fclose(FILE)` | Closes the file and destroys the handle object. |
| `void` | `clearerr(FILE)` | Clears EOF and error state flags. |
| `int` | `fdatasync(int)` | See `man fdatasync`. |
| `int` | `feof(FILE)` | Returns 0 if _not_ at EOF, else non-0 (a `true` value). |
| `int` | `ferror(FILE)` | Returns 0 if the file is in an OK state, else non-0. |
| `int` | `fileno(FILE)` | Returns the file descriptor number for the file. |
| `int` | `flush(FILE)` | Flushes the file handle. |
| `int` | `ftell(FILE)` | Returns the current position of the file cursor. |
| `string` | `fread( int sizeEach, int count, FILE )` | Reads up to `count` blocks, each of `sizeEach` bytes, and returns the result as a string. |
| `string` | `fread( int n, FILE )` | Reads up to `n` bytes from the file and returns it as a string. |
| `int` | `fseek(FILE,int offset,int whence)` | See `man fseek`. |
| `int` | `fsync(FILE | int)` | See `man fsync` |
| `int` | `ftruncate(FILE | int, int size)` | Truncates a file (or file descriptor ID) to the given size. |
| `int` | `fwrite( string src, int sizeEach, int n, FILE )` | Writes `n` objects, each of `sizeEach` bytes, from src. Returns the number of objects (not bytes) written. |
| `int` | `fwrite( string src, int n, FILE )` | Writes up to n bytes from src to the file. Returns the number of bytes written. |
| `int` | `fwrite( string src, FILE )` | Writes src to the file. Returns the number of bytes written. |
| `void` | `rewind(FILE)` | Resets the file's cursor to the beginning of the file. |
| `int` | `unlink( string )` | Removes the given filename from the system. Returns 0 on success. |
| **Properties (read-only!)** | **Type** | **Notes** |
| `SEEK_SET` | `int` | Used with `fseek()`. |
| `SEEK_CUR` | `int` | Used with `fseek()`. |
| `SEEK_END` | `int` | Used with `fseek()`. |


# OO API #

The OO API is very similar to, and can be used in conjunction with, the C-style API. The functions all work as described above, and the only differences are:

  * Calling member functions does not require one to pass the file object as a parameter.
  * The member functions do not have the "f" prefix in their names.

Like the C-style API, the `FILE` class is a member of the `posix` object.

The members are:

| **Return Type** | **Signature** | **Notes** |
|:----------------|:--------------|:----------|
| `FILE` | `new posix.FILE(string filename, string mode)` | Eqivalent to `fopen()`. |
| `bool` | `close()` | Equivalent to `fclose()` |
| `void` | `clearerr()` |  |
| `int` | `datasync()` | See `man fdatasync`. |
| `int` | `eof()` |  |
| `int` | `error()` | Returns 0 if the file is in an OK state, else non-0. |
| `int` | `fileno()` | Returns the file descriptor number for the file. |
| `int` | `flush()` | Flushes the file handle. |
| `int` | `tell()` | Returns the current position of the file cursor. |
| `string` | `read( int sizeEach, int count )` | Equivalent to `fread()` |
| `string` | `read( int n )` |  Equivalent to `fread(n,FILE)` |
| `int` | `seek(int offset[,int whence=posix.SEEK_SET])` |  Equivalent to `fseek()` |
| `int` | `sync()` |  Equivalent to `fsync()` |
| `int` | `truncate(int size)` |  Equivalent to `ftruncate()` |
| `int` | `write( string src, int sizeEach, int n )` |  Equivalent to `fwrite()` |
| `int` | `write( string src, int n )` | Equivalent to `fwrite()` |
| `int` | `write( string src, PosixFILE )` | Equivalent to `fwrite()` |
| `void` | `rewind()` | Equivalent to `frewind()` |
| `void` | `size()` | Returns the file's current size, in bytes. |
| `int` | `unlink()` | Equivalent to `unlink(thisObject.name)` Removes the underlying file but does not close the object - it can still be used for i/o. |
| `string` | `toString()` | More of a debugging string than anything else. |
| **Properties (read-only!)** | **Type** | **Notes** |
| `string` | `name` | The name passed to the ctor or `fopen()` |
| `string` | `mode` | The mode string passed to the ctor or `fopen()` |
| `int` | `errno` | Corresponds to the C-global `errno`, which is set by many of the i/o routines on error. The values are system-specific, and code other than this class might cause the value to change. |

# Example #

Here's a sample script which uses this plugin:

```
if( ! ('posix' in this) || (!posix.FILE) )
{
    loadPlugin("v8-juice-PosixFILE");
}

var fi = { name:"my.file", mode:"w+" };
var ns = posix;
var f =
    ns.fopen(fi.name,fi.mode)
    //new ns.FILE(fi.name,fi.mode) // same thing
    ;
print("f =",f);
f.write("This is ["+f.name+"]["+f.mode+"].\n");
f.write("Hello, world!\n");
ns.fwrite("Bye, world!\n",f);
print( "tell =",f.tell());
var pos = f.seek( 3, ns.SEEK_SET );
print( "tell =",f.tell());
print("feof() =",ns.feof(f));
pos = f.seek( 0, ns.FILE.SEEK_END );
print("feof() =",ns.feof(f));
ns.rewind(f);
print( "ftell =",ns.ftell(f));
pos = ns.fseek( f, -1, ns.SEEK_SET );
print( "ftell =",ns.ftell(f));
print("errno =",f.errno);
pos = ns.fseek( f, 7, ns.SEEK_SET );
print("errno =",f.errno);
var c = ns.fread(4,f);
print("read() =","["+c+"]");
ns.fsync(f.fileno());
ns.fflush(f);
print("file size =",f.size());
ns.fclose(f); // or: f.close()
posix.unlink(fi.name);
print("Done!");
```

The output:

```
stephan@jareth:~/cvs/v8-juice/extra-plugins/src/PosixFILE$ v8-juice-shell test.js
f = [object FILE@0x8aaebb8 name=my.file,  mode=w+]
tell = 49
tell = 3
feof() = 0
feof() = 0
ftell = 0
ftell = 0
errno = 22
errno = 22
read() = [ [my]
file size = 49
Done!

```