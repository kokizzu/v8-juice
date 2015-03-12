**Achtung #1:** This is very beta. Seems to work for me. YMMV. As far as i'm aware, this is the world's first JavaScriptable embedded filesystem, so you _just know_ it's gotta have a few bugs.

**Achtung #2:** these docs are for the current version in the subversion
source tree (i document here as changes are made). If you are using an "older" version,
i recommend [searching through the subversion repo](http://code.google.com/p/v8-juice/source/browse/wiki/PluginWhefs.wiki) for a version of this page which is
closer to the version you're working with (follow that link, and there are options to browse all versions).


# The whefs embedded filesystem plugin #

The whefs plugin is actually an extension of the [whio plugin](PluginWhio.md), and is loaded
with it. It builds off of the i/o device API to act as a wrapper for the
[whefs embedded filesystem library](http://code.google.com/p/whefs),
which is (not coincidentally) based off of the same C library as the i/o plugin is.




## About ##
whefs gets its name from the library it is based on:

http://code.google.com/p/whefs

That library is provided with this plugin, and need not be installed separately.

This plugins provides the following features:

  * Class `whefs.WhEfs` ecapsulates access to a virtual/embedded filesystem, as described in detail on the whefs home page.
  * whefs works in conjunction with the `whio.IODevice` API in two ways. First, it can use any back-end storage supported by `IODevice`. Secondly, the "pseudofiles" in the embedded filesystem are accessed using the `IODevice` interface. This property inherently makes it capable of hosting embedded filesystems within embedded filesystems (not that there's any good reason to do so).

## Source code ##

This plugin's source code is currently part of the v8-juice "extra plugins" source tree,
and can be found here:

http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/whio

See the file `v8-whefs.cc` for the particulars.

_Much_ more information about [whefs](http://code.google.com/p/whefs)
can be found on [its home page](http://code.google.com/p/whefs).

# Important Caveats #

(Never been tested on Windows. Might work. Might not. Same for Mac.)

The [whefs home page](http://code.google.com/p/whefs) goes into great detail
about the cans and cannots of whefs. _Please_ browse that site to get an understanding
of what whefs can and cannot do.

Here's a very brief summary:

Firstly, the whefs library is intended for small use cases, not large
amounts of storage. Keep that in mind.

Secondly, whefs currently as only the most utterly basic support for multi-process
concurrency, and no support whatsoever for multi-thread concurrency (which _will_
cause corruption). See this page page for more information:

http://code.google.com/p/whefs/wiki/WhefsConcurrency

The concurrency limitations rule out certain otherwise very
interesting possible uses of whefs (e.g. as read/write a data store
for server-side JS, possibly storing client-generated data from
multiple clients).

Thirdly, do not try to store binary data in, or retrieve binary data from, a whefs
via JavaScript (doing so via the native API is okay). The problem is in how v8 stores string data - if it sees any character above ASCII 127 then it assumes the data is well-formed UC16 and internally transforms it. This will change the size of the data and
cause Grief.

Lastly, you can get more out of whefs by making use of the whefs-specific tools which come with the whefs source tree:

http://code.google.com/p/whefs/wiki/WhefsTools

For example, they allow one to easily create EFS containers and import/export files into/out of them.

# C++ usage #

When using the plugins API, you don't need to do anything C++-side to
use this add-on - simply load it as described on [the plugins page](Plugins.md).

This plugin is actually a part of the whio plugin, and is loaded when that
plugin is loaded.

When loaded, this plugin will create an object called `whefs` inside the target object
to which it is added (normally the global object). That `whefs` object holds the rest of the classes and functions for this plugin.

The native API has huge amounts of API documentation which might be interesting
to users of this API: [whefs\_amalgamation.h](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/whio/whefs_amalgamation.h). But all of the "less technical" info one needs for using it in JS is documented below. For more details than you'd ever want to know, see the native API documentation

# JS usage #

A brief example:

```
load_plugin('v8-juice-whio');
var fname = "test.whefs";
var fs = new whefs.WhEfs(fname,true); // open up an existing whefs container
var fi = fs.openPseudofile("first.file",true); // open (or create) a file
fi.truncate(0);
fi.write("Hi, world!\n");
fi.close(); // close the file
fs.close(); // close the whefs container
```

The majority of the functionality is provided by the [IODevice class](PluginWhio.md). The so-called _pseudofiles_ (files inside a whefs
container (also known as an _EFS_ or _embedded filesystem_)) use the
exact same i/o interface as `whio.IODevice`, and inherit all usage
conventions from that interface. The most significant of these is that
the `close()` member must be called when the client is done with the
object, to ensure that all handles are closed and memory is
released. Closing a device handle after the EFS instance has been
closed, or allowing the object's destruction to depend on the garbage
collector, will lead to undefined results.

See below for the complete class and function list.

# Error reporting #

See the [whio plugin](PluginWhio.md) for details on how errors are
reported. In addition to that information, the list of integer
error codes used by whefs is:

| **Symbolic name**, as a member of `whefs.rc` | **Description** |
|:---------------------------------------------|:----------------|
| `OK` | The generic success value. Always 0. |
| `ArgError` | Signals some form of error in argument handling (wrong type, missing arguments, etc.) |
| `IOError` | Signals a real I/O error. |
| `AllocError` | Signals an out-of-memory error. |
| `InternalError` | Signals an error internal to libwhefs. |
| `RangeError` | Signals that some argument is out of range or would cause an overflow/underflow. |
| `AccessError` | Signals that access was denied for the operation, e.g. trying to open a read-only file in write mode. |
| `BadMagicError` | This may be triggered when attempting to open an EFS file which is not formatted as an EFS. |
| `ConsistencyError` | A data consistency error in the EFS. The FS should not be used any more if this error occurs. |
| `NYIError` | User has called a function which is "Not Yet Implemented." |
| `UnsupportedError` | Signals that this device type does not support the requested operation. |
| `FSFull` | Signifies that the EFS seems to be full (out of inodes or blocks). |
| `TypeError` | Signals that an argument is of an incorrect type. |
| `SizeTError` | Used by a few i/o operations which fail miserably. Always equal to -1. |

All of them have unspecified values which are unique within that
list. The only entries with predefined values are `OK`, which is
always 0, and `SizeTError`, which is always equal to -1. Interestingly
enough, its _exact_ native value of -1 may depends on the
compile-time size of the native `whio_size_t` type.

While `whio` and `whefs` do share many of the same error code _names_,
there is no guaranty that the _values_ will be the same for the same
error code name. e.g. `whio.rc.AccessError` might be (and actually
is) different than `whefs.rc.AccessError`.

# JS API for whefs #

## WhEfs Class ##

This class is is the only one provided by this plugin. It encapsulates
access to a virtual/embedded filesystem, as described in detail on the
whefs home page. All other features are inherited from the `IODevice` class.


| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| `WhEfs` | `new WhEfs(String filename,bool writeMode)` | Tries to read the given file as a whefs filesystem. Throws on error. If `writeMode` is false, access to the EFS will be read-only. Prefer this constructor over the next form...|
| `WhEfs` | `new WhEfs(IODevice)` | Tries to read the given device as a whefs filesystem. Throws on error. If the device is read-only, no write operations can succeed on the EFS. The device _must not_ be manipulated by any means from outside the WhEfs instance. Doing so likely corrupt it or cause it to fail with a consistency error. The device must also out-live the EFS, and must not be closed before the EFS is closed. |
| `void` | `close()` | Closes the EFS. All pseudofiles/devices opened from this object must have been closed by the time this is called. |
| `int` | `dumpToFile(string filename)` | Dumps the EFS's binary image to the given file. Results are undefined if it is told to overwrite itself. |
| `Array` | `ls(string pattern)` | Returns a list (or null) of all pseudofiles matching the given glob-like pattern. Most common glob patterns are accepted, but only one pattern per call. e.g. `print(fs.ls('*.js'))`. |
| `IODevice` | `openPseudofile(string filename, bool writeMode)` | Opens up a pseudofile within the EFS and returns it as a `whio.IODevice`. If `writeMode` is true then the file is created if it does not exist. Throws on error. The returned IODevice must `close()`ed before the EFS is closed. |
| `int` | `size()` | Returns the size of the underlying pseudofilesystem. |
| `int` | `unlink(string filename)` | Unlinks (deletes) the given pseudofile name. It will fail if the given file is currently opened. |
| **Type** | **Properties** (read-only) | **Notes** |
| `string` | `fileName` | Only for `WhEfs` objects created via a filename argument. Changing it has no effect on the object. |


TODOs:

  * `ls()` should be able to take more than one pattern. It should also accept a callback function to check for a match. We unfortunately have no C++ client-side access to the JS RegExp, so we can't easily support that.
  * Think about how we might integrate [PathFinder](PathFinderClass.md) with whefs, such that PathFinder would be able to search through an EFS.

## whefs shared/static functions ##

| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| `WhEfs` | `whefs.mkfs(string filename [,Object options])` | Creates a new filesystem (destroying any existing file with the same name). This is covered in more detail below. |
| **Type** | **Properties** (read-only) | **Notes** |
| `string` | `fsFormatVersion` | Version string describing the supported whefs file format (which depends on several compile-time options of whefs). Any given binary version of whefs is configured with certain parameters which determine core properties of its file format, and different configurations are not compatible. |
| `string` | `homePage` | The home page URL of whefs. |

### mkfs in more detail ###

The `whefs.mkfs()` function works exactly like the
[whefs-mkfs tool](http://code.google.com/p/whefs/wiki/WhefsMkfs),
creating a so-called "container file" for a virtual filesystem. The
second parameter to the function is an Object which may have these
optional properties, along with sensible (or required) ranges for their values:

  * `inodeCount` = the number of inodes. Must be at least two. More than 1000-2000 is probably not suitable for the current whefs engine.
  * `blockCount` = the number of data blocks. Must be at least inodeCount, but may be greater (up to 64k).
  * `blockSize` = the size of each data block. 4k-32k is normally good.
  * `filenameLength` = maximum length of filenames. Default is 64 and there is a hard-coded limit explained [on the whefs site](http://code.google.com/p/whefs/wiki/WhefsTweakingEFS).

Each parameter is described on the [whefs-mkfs page](http://code.google.com/p/whefs/wiki/WhefsMkfs),
and in even more detail on [the whefs-tweaks page](http://code.google.com/p/whefs/wiki/WhefsTweakingEFS).

If the options object is null or any given parameter is missing then some
arbitrarily chosen defaults are used, and they are almost certainly
not suitable for your particular case (see `whefs_amalgamation.h`,
`whefs_mkfs()` for the full details). Each parameter has certain valid ranges
(again, see the API docs) and `mkfs()` will fail if those ranges are
not met.

As a special service to our dear users, `mkfs()` supports the
pseudofilename `":memory:"`, which means to build the EFS in memory. When
the returned `WhEfs` object is closed (or garbage collected), the
filesystem will be lost. The in-memory FS can be copied to disk using
`dumpToFile()`.

# Some things to keep in mind #

  * See the Caveats section, above, for important notes, especially about concurrency.
  * _Do not_ use a whefs instance from multiple threads (e.g. via [setTimeout()](BindableFunctions.md))! Doing so _will_ eventually corrupt a read/write EFS or produce incorrect output on a read-only EFS.
  * An EFS container cannot be resized dynamically. To resize it, the data must be exported, the container re-created, and the data re-imported.
  * The original whefs source tree comes with [tools several tools for manipulating whefs filesystems](http://code.google.com/p/whefs/wiki/WhefsTools).
  * There are _lots_ more docs available via the whefs source code (API docs) and [the whefs home page](http://code.google.com/p/whefs).
  * whefs doesn't support true directories. Pseudofiles can be named `/path/to/file.txt`, but they're all currently stored in one big directory. This makes searching by name (e.g. using `ls()` or `openPseudofile()`) relatively slow.
  * whefs' engine is not scalable (in terms of performance) to large data stores. Keep them small (a few hundred inodes or so) or suffer dearly from the underlying linear algorithms.
  * whefs is fairly minimalistic - do not expect all the features of a normal filesystem. It does only what i need it to do.
  * If you're running in a server environment, sharing a whefs filesystem across client processes (not threads!), be sure to keep the EFS open for only as long as needed, to avoid locking other clients out of it.

# Tips and Tricks #

## GZipping/Unzipping EFS Content ##

You can use the `whio.InStream` methods `gzipTo()` and `ungzipTo()` to (de)compress an arbitrary stream from/to the EFS.

For example, let's compress a copy of `/etc/hosts` into an EFS:

```
// Assume fs is an opened WhEfs object

var istr = new whio.InStream("/etc/hosts");
var fz = fs.openPseudofile("/etc/hosts.gz",true);
fz.truncate(0); // in case it already had content.
var ostr = new whio.OutStream(fz);
istr.gzipTo(ostr);
istr.close();
ostr.close();
fz.close();

// To decompress it:

fz = fs.openPseudofile("/etc/hosts.gz",false);
istr = new whio.InStream(fz);
ostr = new whio.OutStream('/dev/stdout',true);
istr.gunzipTo(ostr);
istr.close();
ostr.close();
fz.close();
```

And on my system the EFS file looks like:

```
stephan@jareth:~/cvs/v8-juice/trunk/src/lib/plugins/whio$ whefs-ls -b -v memory.whefs 
whefs-ls [No EFS opened]: Verbose mode activated.
whefs-ls [memory.whefs]: Opening EFS [memory.whefs]
List of used blocks per file:

[/etc/hosts.gz]:	1 = 1 block(s), 211 of 8192 bytes used

Totals: 211 byte(s) in 1 blocks
```