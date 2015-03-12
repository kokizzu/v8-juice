**[V8Convert](V8Convert.md) users:** please see [V8Convert\_Whio](V8Convert_Whio.md) instead. That's a newer/improved incarnation of this API.

**Achtung #1:** these docs are for the current version in the subversion
source tree (i document here as changes are made). If you are using an "older" version,
i recommend [searching through the subversion repo](http://code.google.com/p/v8-juice/source/browse/wiki/PluginWhio.wiki) for a version of this page which is
closer to the version you're working with (follow that link, and there are options to browse all versions).

**Achtung #2:** It has [come to my attention](http://groups.google.com/group/v8-users/browse_thread/thread/1621beb71d65e6fa/2883be6ddd10cddd) that using this API to read and write non-ASCII data may (and eventually will) produce undefined results (due to how v8 internally stores and translates strings).
**Do not try to read/write binary data via the JS interface! You Have Been Warned.**


# The whio I/O plugin #

The whio plugin adds object-oriented i/o support to v8 JavaScript applications.




## About ##
whio gets its name from the i/o library it is based on:

http://fossil.wanderinghorse.net/repos/whio/

That library is provided with this plugin, and need not be installed separately.

This plugins provides the following features:

  * Classes `whio.InStream` and `whio.OutStream`, which model sequential-access devices. These are your general-purposes i/o objects and are suitable for most work even though they are limited to either reading or writing.
  * Class `whio.IODevice` controls access to an underlying random-acces device (e.g. a file or in-memory buffer). The back-end supports, in principal, a wide variety of data stores.


# C++ usage #

When using the plugins API, you don't need to do anything C++-side to
use this addon - simply load it as described on [the plugins page](Plugins.md).

When this plugin is registered it will create an object called `whio` inside the target object
into which it is added (normally the global object). That `whio` object holds the rest of the
classes and functions for this plugin.

The native whio API has huge amounts of API documentation which might be interesting
to users of this API:
[whio\_amalgamation.h](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/whio/whio_amalgamation.h)

But all of the "less technical" info one needs for using it in JS is documented below. For the _real_
guts (e.g. "what are the behavioural differences for FILE- and memory-based i/o devices?") see
the API documentation

# JS usage #

```
load_plugin('v8-juice-whio');
var dev = new whio.IODevice("/path/to/file", true); // true= Write-mode
dev.truncate(0);
dev.write("Hi, world!");
...
dev.close();
```

See below for the complete class and function list.

## TODOs ##

  * Either overload read/write() to take an Array of integers or add readBytes()/writeBytes(Array) to allow the safe reading/writing of non-ASCII data.
  * Maybe create a ByteArray class, which is just an in-memory IODevice. An implementation exists but it's real value is in doubt, so it's not published yet.
  * Maybe add `writeBytes(Array-of-int)` and `Array readBytes(int)` so that we can safely pass around non-ascii data.

# Error reporting #

The i/o classes are based on an abstract C API. That API uses numeric
error codes or null values to indicate errors, and this library
normally passed on errors from the C API directly back to the
caller. The value zero is normally the success value and non-zero is
some device-dependent error value (this convention is derived from
common C I/O interfaces).  Errors which happen at the JS level, as
opposed to the C level, are reported via exceptions. All of the
constructors will throw if they cannot open their underlying device or
stream.

For example, a call to `read(20)` which can only read 13 bytes will
tell us so by returning the number 13, but calling `read()` with no
arugments would trigger an exception because there is no sensible
default value to pass on to the C API.

Where integer error codes are used, they have the following symbolic
equivalents:

| **Symbolic name**, as a member of `whio.rc` | **Description** |
|:--------------------------------------------|:----------------|
| `OK` | The generic success value. Always 0. |
| `ArgError` | Signals some form of error in argument handling (wrong type, missing arguments, etc.) |
| `IOError` | Signals a real I/O error. |
| `AllocError` | Signals an out-of-memory error. |
| `InternalError` | Signals an error internal to libwhio. |
| `RangeError` | Signals that some argument is out of range or would (or did) cause an overflow/underflow. |
| `AccessError` | Signals that access was denied for the operation, e.g. trying to open a read-only file in write mode. |
| `ConsistencyError` | A data consistency error. Not used by the core library, but might be used by certain i/o device implementations. |
| `NYIError` | User has called a function which is "Not Yet Implemented." |
| `UnsupportedError` | Signals that this device type does not support the requested operation. |
| `TypeError` | Signals that an argument is of an incorrect type. e.g. passing a non-subdevice to `IODevice.rebound()`. |
| `SizeTError` | Always equal to -1. |

All of them have unspecified values which are unique within that
list. The only entries with predefined values are `OK`, which is
always 0, and `SizeTError`, which is always equal to -1. Interestingly
enough, its _exact_ native value of -1 may depends on the
compile-time size of the native `whio_size_t` type.

Errors which happen outside the scope of the underlying i/o library,
but within the realm of JS-specific usage, are reported by throwing
exceptions. For example, when passing invalid arguments to a
constructor, an exception is the only way to report the error.


# JS API for the whio classes #

## The whio Object ##

The whio object contains all classes and other symbols in this API. The non-class symbols
include the error codes listed above and:

| **Type** | **Properties** (read-only) | **Notes** |
|:---------|:---------------------------|:----------|
| `string` | `homePage` | The home page URL of whio. |


## IOBase Class ##

This class is abstract - it cannot be instantiated directly. However, it does specify several functions of the i/o interface used by its subclasses, and it can be used with `instanceof` to ensure that a given object has the most basic i/o operations.

The API for this class includes any routines which are common amongst the `InStream`, `OutStream`, and `IODevice` classes, but those classes sometimes provide their own implementations for these functions.

| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| `string` | `read(int length)` | Tries to read up to `length` bytes from the input source. Returns the data as a string, and the length of that string will equal the `length` parameter unless EOF is reached or a read error occurs. |
| `int` | `write(string data [,int length=data.length])` | Attempts to write `length` bytes of data. Returns the number of bytes actually written, which will be less than `length` on error |
| `void` | `close()` | Closes the stream. Do not use the object after calling this. |
| `int` | `flush()` | May (or may not) flush any device-specific cache. Returns 0 on success. |
| `bool` | `isGood()`  | Returns true if the object is in a usable state (i.e. there have been no i/o errors). |
| `int` | `ioMode()` | (Versions 20090701 and newer.) Returns 0 if the object is read-only, a positive value if it is writeable, and a negative value if the object does not know (proxy objects sometimes have no way of confirming the i/o mode) |
| **Type** | **Properties** (read-only) | **Notes** |
| `bool` | `canWrite` and `canRead`| Specify whether the object is capable of reading or writing. These are static - set when the device is opened and never changed, so don't use them for error checking. |

## InStream Class ##

InStream represents a read-only stream. In addition to the interface defined by IOBase, it has:

| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| `InStream` | `new InStream(string file)` | Opens the given file in read mode. |
| `InStream` | `new InStream(IODevice)` | Creates a new stream which acts as a proxy for the given device, which must outlive the new stream. The current cursor position of the IODevice is always used as the current reading point for the stream. |
| `int` | `gzipTo(OutStream[,int level])` | Consumes all input from this object and compresses it to the given output stream using gzip compression. Returns `whio.rc.OK` on success. The optional `level` argument specifies the compression level (0 for no compression to 9 for best compression).  |
| `int` | `gunzipTo(OutStream)` | Consumes all input from this object and decompresses it to the given output stream using gzip decompression. Returns `whio.rc.OK` on success. |
| **Type** | **Properties** (read-only) | **Notes** |
| `IODevice` | `ioDevice` | Only for objects created with the `InStream(IODevice)` constructor, this points to the stream's pointed-to device. |
| `string` | `fileName` | Only for objects which are associated with a file. |
| `bool` | `canWrite` | Always false for InStreams. |
| `bool` | `canRead` | Always true for InStreams. |

## OutStream Class ##

OutStream represents a write-only stream. In additions to the interface defined by IOBase, it has:

| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| `OutStream` | `new OutStream(string file[, bool truncate=false])` | Opens the given file in write-only mode, truncating it to 0 bytes if truncate is true, otherwise it is appended to |
| `OutStream` | `new OutStream(IODevice)` | As for `InStream(IODevice)`. If the device is not writable, no write operations on the new object will succeed.|
| **Type** | **Properties** (read-only) | **Notes** |
| `IODevice` | `ioDevice` | Only for objects created with the `OutStream(IODevice)` constructor, this points to the stream's pointed-to device. |
| `string` | `fileName` | Only for objects which are associated with a file. |
| `bool` | `canWrite` | Always true for OutStreams. |
| `bool` | `canRead` | Always false for OutStreams. |


## IODevice Class ##

IODevice represents a random-access i/o device, with read-only or
read/write access. When we say "device", we're normally referring to
a file, but the concept of i/o device is more abstract than that, and
can essentially mean any data source or destination to which we have
random access (the ability to access any point in the data at any
time).

In addition to the interface defined by IOBase, it has:

| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| `IODevice` | `new IODevice(string file,bool writeMode)` | Opens the given file for read and (if writeMode is true) write |
| `IODevice` | `new IODevice(":memory:" [, int startingSize=0])` | Opens a read/write i/o device which uses RAM as its storage. It starts out at the given size and grows on demand. |
| `IODevice` | `new IODevice(IODevice,int lower,int upper)`| Creates a new IODevice which acts as a proxy for a subset of the original device's range. This is described in more detail below. The parent device must outlive the new device. |
| `int` | `error()` | Returns a platform-specific non-0 value if the device has an error state. `isGood()` is equivalent to `(0 == dev.error())`.|
| `int` | `clearError()` | Tries to clear any error state and EOF marker. Returns 0 on success. |
| `bool` | `eof()` | Returns true if the device is at (or past) its EOF. |
| `int` | `tell()` | Returns the current position within the underlying device. |
| `int` | `seek(int offset[, int whence=IOBase.SEEK_SET])` | Sets the current position to offset, relative to whence, which must be one of `whio.IOBase.SEEK_SET`, `whio.IOBase.SEET_CUR`, or `whio.IOBase.SEEK_END`. Returns the new position. |
| `int` | `flush()` | Tries to flush any existing write data, if applicable. Some device types might do other handling here. Returns 0 on success. |
| `int` | `truncate(int length)` | Truncates the device to the given length. Returns 0 on success. |
| `int` | `size()` | Returns the current size of the device. |
| `int` | `rewind()`| Rewinds the internal device pointer. Returns 0 on success. |
| **Type** | **Properties** (read-only) | **Notes** |
| `IODevice` | `ioDevice` | Only for devices created with the `IODevice(IODevice,int,int)` constructor, this points to the subdevice's pointed-to device. |
| `string` | `fileName` | Only for objects which are associated with a file. |
| `bool` | `canWrite` | Value depends on how the device was opened. It is not always possible for the stream opener to know if a given stream supports writing (e.g. "/dev/stdin"), and this value may be set incorrectly for those cases. Writing to such a stream will cause the normal write failure, nothing catastrophic. |
| `bool` | `canRead` | Value depends on how the device was opened. For files this value is always true, even if the file being opened cannot support reading (e.g. stdout). That is because the open() routines don't have enough information to know that the device cannot be read (or written to, in the case of stdin). In a case like that, reading (or writing) will fail in the normal manner. |


### SEEK\_SET and friends ###

`whio.IODevice.seek()` is semantically identical to the C function `fseek()`, and requires a second argument with one of these values:

  * `whio.IOBase.SEEK_SET` means to move to the given absolute position.
  * `whio.IOBase.SEEK_CUR` means to move relative to the current position.
  * `whio.IOBase.SEEK_END` means to move relative to the end of the device.

they are analogous to the C-standard constants of the same names, and are described in the `man` pages for `fseek()`.

### Subdevices ###

The IODevice class has a constructor with the signature `new IODevice(IODevice dev,int lower, int upper)` which used to create "subdevices". A subdevice is a proxy which wraps up a certain range of bytes inside of another IODevice object. This is covered in gross detail in the source file [whio\_amalgamation.h](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/whio/whio_amalgamation.h) (search for "subdev\_create"), and here's a short demonstration:

Here's one explanation of what subdevices are for:

Consider an IODevice pointing to 1000 bytes of storage (somewhere - we don't care where). We can create a subdevice which fences off some amount of that storage. The subdevice is a full-fledged i/o device, but it cannot read or write outside of the bounds it is assigned to.

For example:

```
var origin = new whio.IODevice(":memory:");
origin.write("012345679"); // 10 bytes long
var subdev = new whio.IODevice(origin, 4, 7 ); // fence bytes [4,7)
subdev.write("abcdefghij"); // will only write first 3 bytes
origin.rewind();
print(origin.read(10));
subdev.close();
origin.close();
```

The output of that script is:
```
0123abc79
```

Note that the upper bound is equivalent to EOF, or "one past the end", and is not accessible to the subdevice.

Subdevices have some obscure uses, but probably not many which JS
applications can make use of. They were originally designed to
partition off access to various internal areas of
[an embedded filesystem library](PluginWhefs.md).

Subdevices get an extra function compared to other IODevices:

| `int` | `rebound( int lower, int upper )` | Changes the subdevice's lower and upper bounds, relative to the parent. Returns 0 on success. |
|:------|:----------------------------------|:----------------------------------------------------------------------------------------------|

And a new property:

| `IODevice` | `ioDevice` | This points to the subdevice's pointed-to device. |
|:-----------|:-----------|:--------------------------------------------------|

# Embedded/Virtual Filesystems via whio #

This plugin comes with an optional component, [whefs](PluginWhefs.md), which provides a virtual/embedded filesystem for JavaScript applications. The default build process includes whefs, but it can be disabled for platforms where it won't compile (it is untested on anything other than Unix/Posix platforms). whefs is described [on its own page](PluginWhefs.md).

# Tips and tricks #


## stdin and stdout ##

On Unix platforms you can open standard input and output like this:

```
var stdout = new whio.OutStream("/dev/stdout");
var stdin = new whio.InStream("/dev/stdin");
stdout.write("hi, world!\n");
```

Note, however, that you shouldn't mix such usage with, e.g. `print()`, or other routines which read/write stdin/stdout, as that may garble your output (depending on the buffering used by the underlying interface, e.g. `std::cout` vs. `printf()`). Calling `close()` on such devices will _not_ close the stdin/stdout associated with the application.

## Use in-memory IODevices as buffers ##

You don't need files to use IODevices:

```
var dev = new whio.IODevice(":memory:");
```

Creates an i/o device which uses RAM as storage, growing as necessary. It works just like a file-based i/o device, and can be used to buffer arbitrary data in memory. To specify a starting size of the device, pass it as the optional second parameter. Multiple in-memory devices can be created, but each has its own memory despite having the same name of `":memory:"`.

There is a caveat, however: the memory used for the buffer is not reported to the v8 engine, which means that it can grow to larger than any limit which has been imposed on v8. Adding this awareness to v8 would actually require a significant performance overhead to all write operations, due to the generic nature of the underlying i/o device API (the JS wrapper doesn't actually know, after construction, that it's an in-memory device, and we'd have to query it on each write or truncate operation in order to figure out if it is). The memory associated with an in-memory device can be freed by calling the `truncate()` member (which might or might not actually free any memory, depending on the magnitude of the change in size, but this implementation explicitly frees the memory when `truncate(0)` is called).

While it may not be immediately obvious, this is one easy way to
buffer large amounts of text for later output, and should be much more
efficient than using string concatenation to build up very large
strings. (See the big ugly warning at the top of this page regarding
writing binary data this way!)

If you want to ensure sequential, write-only access to a buffer until
you are ready to send it, simply wrap up the IODevice in an OutStream
using `new OutStream(myDevice)`.

## Using gzip with streams ##

The `InStream` class has minimal support for gzip compression and
decompression. It does not support incremental de/compression, but
does support compressing a whole input stream at once.  While the JS
API does not allow us to safely pass binary around, this support
happens at a lower level, transferring bytes directly between the two
native-level streams.

Here's an example:

```
function tryGzip()
{
    var fname = "test.js";
    var outname = fname + ".gz";
    var ist = new whio.InStream(fname);
    var ost = new whio.OutStream(outname);
    var rc = ist.gzipTo(ost);
    print("gzip rc =",rc,'outfile =', outname);
    ist.close();
    ost.close();
    if( whio.rc.OK != rc ) throw new Error("Gzip failed with code "+rc);
    ist = new whio.InStream(outname);
    outname = outname + '.check';
    ost = new whio.OutStream(outname);
    rc = ist.gunzipTo(ost);
    print("gunzip rc =",rc,'outfile =', outname);
    ist.close();
    ost.close();
    if( whio.rc.OK != rc ) throw new Error("Gunzip failed with code "+rc);
}
tryGzip();
```

That might output:

```
gzip rc = 0 outfile = test.js.gz
gunzip rc = 0 outfile = test.js.gz.check
```

And the list of files it used or created:

```
stephan@jareth:~/cvs/v8-juice/trunk/src/lib/plugins/whio$ l test.js*
-rw-r--r-- 1 stephan stephan 5922 2009-03-21 14:16 test.js
-rw-r--r-- 1 stephan stephan 1860 2009-03-21 14:19 test.js.gz
-rw-r--r-- 1 stephan stephan 5922 2009-03-21 14:19 test.js.gz.check
stephan@jareth:~/cvs/v8-juice/trunk/src/lib/plugins/whio$ cmp test.js test.js.gz.check; echo $?
0
```

## Use streams as device proxies ##

Many i/o algorithms only need sequential access to a stream, and thus
accept `InStream` or `OutStream` arguments. If you have an `IODevice`
you can easily wrap a stream around it by using `new InStream(myDevice)`
or `new OutStream(myDevice)`, as appropriate. When
finished, the stream can be closed, leaving the device intact. Note
that all read/write operations performed on such a stream will use the
device's current cursor position for their read/write. Thus, if the
device is manipulated while streams are using it, the effects may be
unpredictable (unless planned very carefully). Also be aware that an
output stream might not actually write its data until it is flushed or
closed, so do not rely on the contents of a wrapped i/o device to be
accurate until the stream proxy has been closed or explicitly flushed.

## Reimplementing `print()` ##

It is trivial to re-implement the `print()` function so that it writes to an output stream of your choice. As a simple example:

```
function print()
{
   print.stream.write( Array.prototype.slice.apply( arguments, [0] ).join(" ") + "\n" );
   print.stream.flush();
}
print.stream = new whio.OutStream('/dev/stdout'); // or whatever
```

Now any calls to `print()` will use the proxy output stream.

## Don't forget to flush ##

The i/o API is ignorant of any underlying storage mechanism (if any) and may inherit certain behaviours which differ from implementation to implementation. The most notable case is _flushing_. To "flush" a device means "write any pending data," and is necessary because some device types buffer some amount of output to improve overall performance.

When writing to an in-memory buffer, the device's i/o happens at the exact moment it is requested. On the other side, a write to a file may be cached somewhere (e.g. the C `FILE` object may do buffering of its own). Such a cache will not be written until `flush()` is called on the device (or the device is closed, in which case it is automatically flushed). The case where this is most visible is when using an `OutStream` to write to stdout. When doing so, it may be necessary to call `flush()` on the device before any output is written to the console. If the output need not appear immediately (e.g. it's not intended for interactive use) then explicit flushing is not necessary.

It is never harmful to call flush (except on a closed device, in which case the function will throw an exception), but over-flushing may lead to performance problems for some devices. If you need to be certain that a certain bit of output is written to its final destination, call `flush()`. If you don't need to be certain (that is, you don't mind it internally buffering the data for a bit) then calling `flush()` is not necessary. The only use case i've personally experienced where it is necessary is when writing to stdout, so that the output becomes available immediately.