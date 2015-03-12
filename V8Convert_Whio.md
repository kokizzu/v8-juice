

# Introduction #

This [V8Convert](V8Convert.md) add-on implements a variety of i/o facilities by wrapping [libwhio](http://fossil.wanderinghorse.net/repos/whio/) (specifically, it wraps most of [the whio C++ wrapper](http://whiki.wanderinghorse.net/wikis/whio/?page=WhioCPlusPlus)). It provides streaming- and random access to a variety of storage types.

## Downloading ##

The sources are available here:

  * http://code.google.com/p/v8-juice/source/browse/#svn/convert/addons/whio

The included build files are for platforms hosting GNU Make and gcc. It requires [V8Convert](V8Convert.md). It is unknown whether the underlying libwhio code will compile out of the box on non-Unix platforms.

## Plugging In the Bindings ##

Include the file `cvv8-whio.hpp`, build/link in the various CPP files, and call `cvv8::io::SetupBindings()`.


## The Golden Rule: Closing Handles ##

All streams and i/o-devices are wrappers around underlying native resources. When client code is done with a given device it must call `close()` on it to close the underlying handle(s) and free any native resources. For some specific uses, lifetimes _must_ be managed properly by the client _or Undefined Behaviour ensues_. Examples include "subdevices", which are not allowed to live longer than their parent device, and EPFS Pseudofiles, which must be closed before their containing EPFS is closed.

## Error Reporting ##

The vast majority of errors are reported via exceptions. Because of this:

  1. Clients may need to take extra care to `try`/`catch` and `close()` streams/devices.
  1. Error checking is reduced dramatically compared to using the underlying C API.

Relatively few stream/device functions do _not_ throw on error.

## Notes on Ownership and Handle Sharing ##

Each high-level i/o object is a thin wrapper around a C-level object which does (or proxies) the real work. Many of the stream/device constructors have a boolean parameter which specifies whether or not they "take ownership" of the passed-in stream/device. In such cases, "taking ownership" means the following:

  * On success (and _only_ on success), the C-level handle is removed from the source stream/device. On error ownership is never transferred unless explicitly documented otherwise (such cases are rare, and i can't think of an example in this code).
  * The stream/device being transferred is then freed _immediately_ using the JS engine, which will end up calling its native destructor. By the time that happens, the destructing wrapper has been disassociated from the C-level handle, so the only resources being freed are the few bytes of the wrapper object.

This might be clearer after a demonstration:

```
var dev = new whio.IODev("/dev/null",whio.iomode.WRITE);
var str = new whio.OutStream(dev,true/* <--- HERE*/);
// At THIS point dev is NO LONGER VALID. Its underlying
// C-level handle has been transferred to the str object
// and [the equivalent of] dev.close() has been called.
```

The reason for this slightly non-intuitive behaviour (the immediate destruction of the "source" device) is because it is the only safe option we have. If we allowed, e.g., the above `dev` object to keep ahold of its handle then the lifetime of the underlying handle would be impossible to track/manage properly (`dev` might be closed before `str` is finished using it, and `str` has no way of knowing that and will step on an invalid (or re-allocated!) pointer). If, on the other hand, we simply transferred the C-level handle and _did not_ do (the equivalent of) `dev.close()` then the client would be forced to call `dev.close()` after transferring ownership (failing to do so would introduce a leak until GC kicked in), which would be even more non-intuitive than the current approach.

So, the important thing to remember here is that when a constructor (or function) "takes ownership" of a stream or `IODev` object, the object being transferred is actually destroyed "immediately" (as part of the operation which takes ownership). Yes, this is slightly non-intuitive, but it keeps ownership of the pointers manageable.


# whio Object #

All APIs in this add-on are added to the `whio` object which, in turn, gets added to the client's global object (whichever object he passes to `SetupBindings()`).

| **Member** | **Description** |
|:-----------|:----------------|
| Object `iomode` | See below. |
| bool `zlibEnabled` | True if the library was compiled with zlib support, else false. |


## whio.ByteArray ##

The `whio.ByteArray` class is identical to the class described on the [V8Convert\_ByteArray](V8Convert_ByteArray.md) page.

## I/O Mode Flags ##

The `whio.iomode` object contains a mapping of names to integers corresponding to the C-level `whio_iomodes` enum entries. The keys are the enum item names minus the `WHIO_MODE_` prefix. The entire list is:

| **Entry** | Description. |
|:----------|:-------------|
| `INVALID` | Invalid value (numeric 0). |
| `UNKNOWN` | Indicates the device does not (yet) know its i/o mode. Happens sometimes when proxying. |
| `READ` | Indicates read mode. |
| `WRITE` | Indicates write mode. |
| `RO` | Alias for (`READ|FLAG_ONLY`) |
| `RW` | Alias for (`READ|WRITE`) |
| `WO` | Alias for (`WRITE|FLAG_ONLY`) |
| `RWC` | Alias for (`READ|WRITE|FLAG_CREATE`) |
| `FLAG_ONLY` | Marks "only" modes, like read-only and write-only. |
| `FLAG_CREATE` | Means to create the file if it does not yet exist. |
| `FLAG_EXCL` | Means to fail to open if the file already exists. |
| `FLAG_FAIL_IF_EXISTS` | Alias for `FLAG_EXCL` |
| `FLAG_TRUNCATE` | Means to truncate the file on opening. |
| `FLAG_APPEND` | Means to append new output to the end of the file. |

These flags are typically only honored by routines which open file-based devices, and not all options may be supported by a given device-opening routine.


# Stream Classes #

`whio.StreamBase` is the base (abstract) class of the `InStream` and `OutStream` classes. Each instance is a thin wrapper around a [whio\_stream](http://whiki.wanderinghorse.net/wikis/whio/?page=whio_stream) object (the base C-level stream abstraction class).

This class is abstract and cannot be instantiated by client code - use the `InStream` and `OutStream` classes to create new instances. This type _can_ be used with the `instanceof` operator.

It provides the following base functionality:

| **Member** | **Description** |
|:-----------|:----------------|
| `void close()` | Closes the stream and frees all native resources. |
| `int iomode()` | Returns a mask of `whio.iomode` values. |
| `bool isGood()` | Returns true if the stream is "good" (has no error state). Note that most routines throw on error, so this is rarely useful. |

## InStream ##

This general-purpose sequential-input class subclasses `whio.StreamBase` and provides the following additional functionality:

| **Constructor** | **Description** |
|:----------------|:----------------|
| `InStream(string fileName)` | Opens the given file for read-only access. |
| `InStream(IODev dev, bool takeOwnership)` | Wraps the given `IODev`, which must be input-capable. If `takeOwnership` is true then `dev`'s handle is given to the new object and `dev` is destroyed immediately (it is just a thin wrapper around the handle). If it is false then `dev` _must_ outlive the new stream. |
| **Member** | **Description** |
| `mixed read(int len, bool binary)` | Reads `len` bytes and returns the results as a `ByteArray` if `binary` is true, else as a String. Results are undefined if `binary` is false and the read-in data is _not_ UTF8. As a special case, it returns `null` for 0-byte reads. |
| `void gzipTo(OutStream|IODev dest [, int level=3])` | If the library is built with zlib support then this compresses all contents of this stream to the `dest` stream. Throws on error. |
| `void gunzipTo(OutStream|IODev dest)` | If the library is built with zlib support then this decompresses all contents of this stream to the `dest` stream. Throws on error. |
| `void copyTo(OutStream|IODev dest)` | Copies the entire contents of this stream to the given destination. |

### Example: De/Compressing ###

Here's a simple example which compresses a file into a memory buffer and then
decompresses it to stdout.

```
function testGZip() {
    // Compress a file to an in-memory buffer:
    var buf = new whio.MemoryIODev(1024*4) /* in-memory device */;
    var src = new whio.InStream("/etc/hosts");
    src.gzipTo( buf );
    src.close();
    print("Compressed size = "+buf.size());

    // Decompress the buffer to an OutStream:
    buf.seek(0, whio.SEEK_SET); // move to the start of the data
    src = new whio.InStream(buf, true /*==take ownership of buf's underlying handle*/);
    dest = new whio.OutStream("/dev/stdout",false);
    src.gunzipTo(dest);
    src.close() /* also frees the memory buffer */;
    dest.close();
}
```

The API does not support incremental de/compression, only whole-stream de/compression. The input stream is de/compressed in chunks, not fully buffered unless we explicitly output it to a buffer as in the example above.

## OutStream ##

This general-purpose sequential-output class subclasses `whio.StreamBase` and provides the following additional functionality:

| **Constructor** | **Description** |
|:----------------|:----------------|
| `OutStream(string fileName, bool truncate)` | Opens the given file for write-only access, either truncating it or appending to it. |
| `OutStream(IODev dev, bool takeOwnership)` | Wraps the given `IODev`, which must be output-capable. If `takeOwnership` is true then `dev`'s handle is given to the new object and `dev` is destroyed immediately (it is just a thin wrapper around the handle). If it is false then `dev` _must_ outlive the new stream. |
| **Member** | **Description** |
| `int write(string|ByteArray [,int len=arg1.length])` | Writes the given string or `ByteArray`. Returns the number of bytes written (which may differ from the length, in characters, of an input string). |
| `void flush()` | Flushes any pending output. |

# Random-Access Devices #

`whio.IODev` is the base class/interface for random-access devices. The base implementation wraps  various types of devices, e.g. files, memory buffers, and "subdevices". Each instance is a thin wrapper around a [whio\_dev](http://whiki.wanderinghorse.net/wikis/whio/?page=whio_dev) object (the base C-level i/o device abstraction class).

It provides the following base functionality:

| **Constructor** | **Description** |
|:----------------|:----------------|
| `IODev(string name,string mode)` | Opens a file using an `fopen()`-style mode string. |
| `IODev(string name,int imode[,int unixPermisions])` | Opens a file using a `whio.iomode`-style mode flags and optional explicit unix-style permissions. |
| **Member** | **Description** |
| `void clearError()` | Clears any error state. |
| `void close()` | Closes the stream and frees all native resources. |
| `bool eof()` | Returns true if the device is at EOF. |
| `int error()` | Returns device-specific error code, or 0 if everything is okay. |
| `void flush()` | Flushes the device. Only useful when writing. |
| `int iomode()` | Returns a mask of `whio.iomode` values. |
| `mixed read(int len, bool binary)` | Reads `len` bytes and returns it as a `ByteArray` (if `binary`==`true`) or a String.  As a special case, it returns `null` for 0-byte reads. |
| `int seek(int pos, int whence)` | Analog to `fseek(3)`. The last argument must be one of `whio.SEEK_SET`, `whio.SEEK_CUR`, or `whio.SEEK_END`. Returns the new position. |
| `int size()` | Returns the current on-storage size of the device. May be smaller than the internal buffer allocated for in-memory devices. |
| `int tell()` | Reports the current position of the cursor within the device. |
| `void truncate(int newSize)` | Truncates the device. Subdevices do not support truncation. |
| `int write(String|ByteArray [, int len=arg1.length])` | Writes out a string or `ByteArray`. Returns the number of bytes written (which may differ from the _character_ length of a String parameter). |

## General Notes ##

  * Because `In/OutStream` can proxy `IODev`, you can use the stream APIs to read/write from/to `IODev` instances. Just be careful with the cursor positioning when reading/writing from _both_ the device and stream APIs on the same storage.

## In-memory Devices ##

`whio.MemoryIODev` is a `IODev` subclass with the following additional members:

| **Constructor** | **Description** |
|:----------------|:----------------|
| `MemoryIODev(uint size[, float expFactor=possibly 1.5])` | Creates an in-memory device with the given initial capacity and the given expand-on-grow factor. See below. |
| **Member** | **Description** |
| `int bufferSize()` | Returns the allocated size of the internal memory buffer. |

Expansion: If the `expFactor` argument to the constructor is (`<1.0`) then the buffer will not be able to dynamically grow beyond its initial size, otherwise it will grow by (times `expFactor`) each time it expands. If `expFactor` is exactly 1.0 then expansion always tries allocate only as much as needed (==more (re)allocs, and probably overall slower, but a tighter fit).

Note that while a non-expanding buffer will fail if it is asked to write past its boundaries, the `truncate()` operation can be used to resize even a non-(dynamically-)expanding memory buffer. When shrinking, the internal buffer is not resized unless it is reduced by some value related to the expansion factor. For expandable buffers, truncating to a size of 0 explicitly frees the internal buffer, but it can be reallocated via future `write()` or `truncate()` operations. _Non-expandable_ buffers do not free their memory when shrinking because their rules would later prohibit them from expanding back to (up to) their original size.

## Subdevices ##

`whio.Subdevice` is a `IODev` subclass with the following additional members:

| **Constructor** | **Description** |
|:----------------|:----------------|
| `Subdevice(IODev parent, uint low, uint high)` | Creates a "subdevice" which only has access to the given byte range in the parent device. The parent _must_ outlive this object. |
| **Member** | **Description** |
| `int rebound(uint low,uint high)` | Re-sets the boundaries of the subdevice to a new range within its current device. |
| `int rebound(IODev dev, uint low,uint high)` | Re-parents the subdevice to a new parent and re-sets the boundaries to the given range. |

Subdevices are used for partitioning off an area of a parent device (which _must_ outlive the subdevice). A subdevice can only read/write from/to the given byte range. It has its own coordinate space, however. e.g. a subdevice wrapping the range 100-200 treats byte 0 as the 100th byte of the parent, byte 1 and the 101st, etc.

Minor achtung: Subdevices do not support the `truncate()` operation (it always fails).


# EPFS: Embedded Pseudo-FileSystem #

This API wraps most of the [whio\_epfs API](http://whiki.wanderinghorse.net/wikis/whio/?page=whio_epfs). Please see that page for _lots_ of information about it. This API represents, to the best of my knowledge, the world's _second_ embedded filesystem API for JavaScript. The [first-ever implementation](PluginWhio.md) is an immediate genetic forefather of this code.

`whio.EPFS` has the following public API:

| **Constructor** | **Description** |
|:----------------|:----------------|
| `EPFS(string fileName, bool writeMode)` | Opens an existing EFS file. |
| `EPFS(IODev storage, bool takeOwnership)` | Opens pre-existing EFS storage. If `takeOwnership` is true then the new object takes over ownership of the underlying storage handle and destroys the `storage` object (which is just a thin wrapper). |
| `EPFS(string fileName, bool allowOverwrite, Object opt)` | Formats a file as EFS storage space. See below for more info. Use the file name `:memory:` for an in-memory EFS (but all contents will be lost when this object is destroyed). |
| `EPFS(IODev storage, bool takeOwnership, Object opt)` | Formats an opened device as EFS storage. If `takeOwnership` is true then the new object takes over ownership of the underlying storage handle and destroys the `storage` object (which is just a thin wrapper). |
| **Member** | **Description** |
| `void close()` | Closes the EFS and frees all native resources. ALL pseudofiles `open()`d by this device _MUST_ be `close()`d _BEFORE_ this object is `close()`d. |
| `void foreachInode(function callback [, any clientData=undefined])` | For each _used_ inode in the EFS, `callback(inodeData,clientData)` is called. See below. |
| `bool hasNamer()` | Returns true if this EFS has a namer installed. |
| `void installNamer(string)` | Installs a "namer" implementation in the EFS. See below. |
| `void removeNamer()` | Removes any installed namer (throws if there is no namer). |
| `bool isRW()` | Returns true if the EFS is operating in read/write mode. |
| `int inodeId(string fname)` | Returns the inode ID for the given name _if_ a namer is installed. |
| `void label(string)` | Sets the EPFS label. The property `label.maxLength` Specifies the maximum legal length (in bytes) of a label string (this is a compile-time constant and cannot be changed without invaliding EFS files created using other values). |
| `string label()` | Gets the EPFS label. |
| `void name(int inodeID, string name)` | Sets the given inode ID's name. Use a `null` name to un-set it. A given namer implementation may impose certain restrictions on name patterns and/or lengths. |
| `string name(int inodeID)` | Returns the name for the given inode. |
| `int size()` | Returns the on-storage size of the EFS container. |
| `void unlink(int|string)` | "Unlinks" (removes) the given inode. Only works if the inode is _not_ currently opened. |
| `EPFS.PseudoFile open(int|string, int iomode)` | Opens the given inode. The second parameter must be `whio.iomode` flags. To create a new file, pass 0 (or an unused name) as the first argument and `whio.iomode.RWC` as the second. |

Opening an EFS can fail for several reasons:

  * The storage being read was not formatted as an EFS, or was formatted with an incompatible version.
  * Read/write mode was requested but the storage is read-only.
  * Out of drive space or memory.
  * The given combination of fs options is invalid (see `whio_epfs_mkfs()` and friends for the legal ranges). e.g. trying to create an EFS with an inode count of 0 or a block size of 10 bytes will fail. Likewise, really large values (a block size of `2^30`) might fail in various ways.
  * Ad nauseum.

## Achtung ##

  * _NEVER EVER EVER_ allow multi-threaded access to an EPFS. Doing so _will_ eventually corrupt the EFS state.
  * Likewise, never "interweave" EPFS instance usage across `setTimeout()` or similar.
  * Only access the EFS from the thread it was opened in and never open the same EFS container multiple times inside one process. The instances can't see each other and will eventually step on one another's toes.
  * If enabled at compile-time (is on by default on non-Windows platforms), the EPFS [locks EFS container files](http://whiki.wanderinghorse.net/wikis/whio/?page=whio_epfs_locking) when it is opened, and keeps the lock for the life of the EFS instance.
  * Use at your own risk. A crash caused by another thread, a poorly-timed `SIGINT`, or similar, while an EFS is writing could lead to EFS container corruption.
  * Inode and block IDs are 16 bits, any passing numbers larger than `2^16` as EFS option values to the formatting ctors will likely result in numeric overflow/overwrap and unexpected results. There are more details about this in the "Sizes, Ranges, ..." section of this page.

## Creating a new EFS ##

Creating a new EFS container is generically known as `mkfs`ing (or "formatting") it. Two ctors are provided which format storage for use with EFS:

```
// Core properties of the EFS:
var fsOpt = {
  inodeCount: 512, // max number of entries in the EFS
  blockSize:1024 * 16, // size (bytes) of each p-file data block
  maxBlocks:0 // max number of p-file data block (0=unbounded)
};

// Format a file:
var fs = new whio.EPFS("/my/file.epfs",true, fsOpt);
// The second argument specifies whether or not overwriting an existing file
// is allowed. It will fail if the value is false and the file already exists.

// Format an arbitrary IODev:
var dev = new IODev("/my/file.epfs", whio.iomode.RWC | whio.iomode.TRUNCATE);
var fs = new whio.EPFS(dev,true, fsOpt);
// If the second parameter is true then the new EPFS takes over the underlying
// whio_dev handle and destroys the dev object (which is just a thin wrapper
// around the native handle). If it is false then dev MUST outlive fs and
// MUST NOT be close()d until after fs is close()d.
```

If whio is built with file locking support, the formatting will fail without storage-side side-effects if the EFS is locked by another process. It is semantically illegal to open the same EPFS multiple times from the same process, and doing so will lead to corruption (if multiple writers are involved) or confusing/mis-synced inode state (if there is one writer and N readers).

## Adding Inode Name Support ##

By default an EPFS does not have support for naming inodes (they are referenced only by their IDs). If you want to be able to assign names to the files in your EFS you must install a "[namer](http://whiki.wanderinghorse.net/wikis/whio/?page=whio_epfs_namer)" (ideally immediately after formatting the EFS). "Namer" is an abstract interface which EPFS uses to delegate the naming of inodes to 3rd-party code. The namer implementation is separated from the core library because different inode-to-name mapping mechanisms have wildly varying storage, memory, and performance trade-offs, and this allows us to experiment with different models without touching the core library.

To install a namer:

```
fs.installNamer("ht"); // persistent hashtable-based namer
// Or:
fs.installNamer("array"); // non-persistent memory-based namer
```

(See the next subsection for a description of each namer.)

When opening the EPFS later on, its namer (if any) will be automatically loaded - it only needs to be installed one time, ideally right after formatting the storage.

There is currently no way to remove or replace a namer once it is installed (aside from re-creating the EFS and copying over the p-file contents).

Note that a given namer implementation might not support all optional operations provided by that interface. Namers _may_ provide both inode-id-to-name and name-to-inode-id mappings, but are not required to perform both (the "ht" namer does).

Each namer may impose limitations on the length and name patterns of inode names.

To remove a namer:

```
fs.removeNamer();
```

### Supported Namer Implementations ###

The "namer" interface abstracts away the details of inode-to-name mappings from the EPFS core API because various implementations of such mappings have widely varying storage, memory, and performance trade-offs.

The library currently supports the namers described below. The names shown here are the ones which may be passed to `EPFS.installNamer()`.

**The `ht` namer** stores inode-to-name mappings in a hashtable which is itself stored in the EPFS. Has constant memory costs and amortized O(1) insert/search/remove costs. Insertions _sometimes_ (depending on usage patterns) have an associated O(N) cost, where N is some function of the number of _free_ blocks in the storage which must be considered for re-allocation. The "ht" namer imposes no name length limits but reserves names matching `<([NUMBER])>` for its internal use.

**The `array` namer** is a _non-persistent_ namer, meaning that the names set in a given EPFS session are not stored to disk and _are lost_ when the EPFS instance is closed. This is intended mainly as (A) a proof-of-concept namer and (B) as an alternative namer for in-memory EPFS instances (those created using the special filename `:memory:`). It has O(N) memory costs (N=inode count times average name length), O(N) search performance for name-to-inode-id operations, but effectively O(1) for inode-id-to-name operations. This namer imposes no specific name length limits.

For example:

```
var fs = new EPFS(":memory:", true,
                  {inodeCount:20, blockSize:2000, maxBlocks:0} );
fs.installNamer("array");
```


## Opening an Existing EFS ##

This looks a lot like _creating_ an EFS, but has one fewer parameter for the constructor.

```
var fs = new whio.EPFS("/my/file.epfs",true);
// The 2nd parameter means whether or not to open in Write mode (false=read-only)

// Or fs-open a pre-opened device:

var fs = new whio.EPFS(myDevice,true);
// See the 3-arg ctor for the meaning of the 2nd parameter.
```

Opening will fail (with an exception) if the given storage was not previously formatted with a compatible version of `whio_epfs`.

## `EPFS.foreachInode()` ##

For each _used_ inode in the EFS, this function calls a callback:

```
function myCallback(inode, list) { list.push(inode); }
var results = [];
fs.foreachInode( myCallback, results );
```

which is equivalent to:

```
var results = [];
function myCallback(inode) { results.push(inode); }
fs.foreachInode( myCallback );
```

In the context of the callback, `this` is the EPFS object. If `callback()` returns a non-0 value then that value is returned to the EFS API, which will cause looping to stop and the error code to be returned to the `foreachInode()` caller. (FIXME: allow the callback to throw to report an error. i don't currently know how to properly catch this from the level (C) where it would happen.)

The inode object passed to the callback is a simple collection of key/value pairs:

| `id` | Inode ID. |
|:-----|:----------|
| `mtime` | Modification time, in Unix Epoch format. |
| `size` | Size, in bytes. |
| `clientFlags` | Client-defined flags (uint32). |
| `name` | Only if the EFS has a namer installed. |

Notes:

  * This is a _subset_ of the actual inode data (plus the optional name), eliding parts which are not expected to be useful in JS code (mainly low-level info which does not make much sense without other data only accessible via the native API).
  * Changes made to the EFS from within the foreach loop, may or may not be visible to further iterations within that same loop. e.g. when adding a new file within the loop the new file might or might not appear in a future iteration of the loop. Ideally clients _should not_ modify the EFS while for-each looping.
  * This loop does not take into account any unflushed info held by opened p-files.
  * If a namer is installed, one of the inodes is almost certainly the p-file used to store the inode/name mappings. If you see a p-file named something like `<([whio_epfs_namer_ht.whio_ht])>`, that's what you're seeing.

Please see the native `whio_epfs_foreach_inode()` for more information.

## Pseudofiles ##

"Pseudofiles" (a.k.a. p-files) are the "files" which live within an EFS container. Sometimes the term inode and p-file are used interchangeably, as a p-file is really just a handle to an opened inode in the EFS storage (equivalent to an opened file).

**ACHTUNG:** is it critical that all p-files opened from a given EFS be `close()`d _BEFORE_ their containing EFS is `close()`d, or Undefined Behaviour ensues.

Instances of `whio.EPFS.PseudoFile` are _not_ created via constructor calls - they are created _only_ using `EPFS.open()`. `PseudoFile` is a full-fledged `IODev` subclass and adds the following methods:

| **Member** | **Description** |
|:-----------|:----------------|
| `int touch([Date|uint32=currentTime])` | Sets the device's modification timestamp to the given Unix Epoch time. Returns the new (touched) time. |
| `uint32 mtime` | Get or set the modification time as a `uint32` Unix Epoch timestamp. Note that the JS `Date` class uses (Epoch + milliseconds) time. |
| `uint32 clientFlags` | Gets or sets the "client flags" (32-bit unsigned integer) associated with the p-file. |
| `uint inodeId` | Gets the EPFS inode ID of the p-file. It is guaranteed to be non-0. Setting this causes an exception. |
| `string name` | Gets or sets the p-file's name. If the parent EFS has no namer then accessing this property will throw an exception. (FIXME: only throw when _setting_ in that case, else return `null`.)|

### Important Notes ###

  * Is it _critical_ that all p-files opened from a given EFS be `close()`d _BEFORE_ their containing EFS is `close()`d, or Undefined Behaviour ensues.
  * `EPFS.unlink()` will fail for any inode which is currently opened.
  * It is legal to have a given inode (p-file) opened multiple times, but the client app must carefully synchronize such use if he wants to avoid "interweaving" data written via multiple handles.
  * _NEVER_ use a p-file handle from any thread except the one which created it. Violating this _will_ eventually lead to EFS corruption.
  * Do not use p-files from `setTimeout()` (or similar), as this could potentially cause "interweaving" of certain non-atomic write operations (the same problem as with multi-threading), which _will_ lead to corruption at some point.

### Creating and Opening Pseudofiles ###

To "create" a new p-file from the next available (unused) inode, we ask the EFS to open inode #0 (which is the "not an inode" inode ID):

```
var p = fs.open( 0, whio.iomode.RWC );
```

That allocates the next-available inode.

To open a specific inode ID we replace the number 0 with the inode ID we want to use:

```
var inodeId = 7;
var p = fs.open( 7, whio.iomode.RWC );
```

Achtung: if the EPFS has a namer installed, it is likely (we'll call it 100%) that the namer has used one of the inodes for itself. There is no generic way to know which inode (if any) a namer reserves for its own use. Be careful not to delete/modify the namer's inode.

To open an "existing" (used) inode, but only if it is already in use:

```
var inodeId = 7;
var p = fs.open( 7, whio.iomode.RW );
```

That will fail if inode 7 is not already "used" because the 2nd argument to `open()` is missing the `whio.iomode.FLAG_CREATE` flag.

To open by name (only works if a namer is installed in the EFS):

```
var p = fs.open("the_name", ... );
```

You can use various combinations of the `whio.iomode` flags to allow/disallow the creation of new p-files, to control read-only vs. read-write, or to truncate them upon opening.

Note that only p-files can only be opened in read-write mode if the EFS itself is opened in read-write mode.

### Generic P-file Tips ###

  * ALWAYS close ALL p-files _BEFORE_ the underlying EPFS is closed. If you do not, you risk Undefined Behaviour.
  * P-files are full-fledged i/o devices, and can be used with, e.g. the `InStream` and `OutStream` wrappers.
  * If an EPFS is opened in read-only mode, p-files can only be opened in read-only mode.
  * A given inode can be opened multiple times (up to 255 times). Each opened copy has its own cursor and read/write mode, but shares the same underlying inode metadata.
  * When writing large amounts of data to a p-file, if you know the size in advance, use `truncate()` to re-size the p-file. Errors during writing are most often caused by running out of space in the EPFS, so truncating in advance will get the majority of the error conditions out of the way up front, instead of hitting them in the middle of writing. There is no API for determining how much space the EPFS itself can still hold (and calculating that would be computationally very expensive).
  * To clear a p-file's name, set it to `null`.

## General EPFS Tips ##

  * ALWAYS close ALL p-files _BEFORE_ the underlying EPFS is closed. If you do not, you risk Undefined Behaviour.
  * EPFS has no concept of directories, but instead supports a single flat namespace. Depending on the namer implementation, names may or may not have restrictions on how they can be named. The "ht" namer allows essentially any names, including those which contain what appear to be path parts, e.g. `/foo/bar.txt`.
  * The inode count of an EFS is fixed when the EFS container storage is formatted, and cannot be changed later.
  * EPFS does not require that inodes have names. An application is free to pick its own inode IDs and use those for opening inodes (provided they are within range of the EFS' inode count).
  * Opening p-files by their inode ID is significantly faster than opening them by name.
  * If using a namer, try to avoid using very small data block sizes (e.g. 4kb), to avoid fragmenting the namer's storage (which hurts performance on name-get/set operations).
  * If you know what your data looks like in advance, you can size the inode count and data block size for a optimum fit (or nearly so). As a rule, having larger data blocks means using less memory at run-time, as we have to manager fewer blocks per inode. However, having larger data blocks means more wasted storage for small p-files.
  * The EPFS "mkfs" constructor taking a filename accepts the special name `:memory:` to mean an in-memory filesystem. The down-side to this is that the EFS and all of its contents will be destroyed when the EPFS instance is closed.

# Sizes, Ranges, and Memory Requirements #

## Core library ##

The core library is compiled with 32-bit i/o ranges by default (C type=`whio_size_t`). Since v8 does not directly support 64-bit values, it is not recommended to compile whio for 64-bit i/o (with `WHIO_SIZE_T_BITS=64`) in the context of this binding API.

Memory usage is minimal. All of the stream/device implementations are thin wrappers around lower-level functionality (e.g. `FILE` handles or raw memory operations). The only one which needs (potentially) significant memory is the `MemoryIODev` class (in-memory devices, which can theoretically grow as large as your system's allocator will allow, to within the size limit of `whio_size_t`). All of the stream/device handle types have a smaller (by far!) `sizeof()` than `sizeof(FILE)` (=216 on my 64-bit box and 148 on my 32-bit machine, and `fopen()` allocates twice that amount).

## EPFS ##

EPFS is compiled, by default, with 16-bit inode and data block IDs (C type=`whio_epfs_id_t`). 16-bit inode/block IDs "limit" the EFS to `2^16-1` (64k) inodes/blocks (theoretically, though the real limit is likely a few ticks less than that), but data blocks have 32-bit size ranges, so this 16-bit limit is not expected to ever be a problem.

Library versions compiled with different "bitness" values cannot read each others' EFS containers, and thus it is not recommended that the default values be changed except for specialized embedding cases (as opposed to generic use). While libwhio itself can be compiled for 16-bit or even 8-bit modes, EPFS cannot work in 8-bit mode and would be severely constrained in 16-bit mode (with a maximum storage size of 64kb!).

While the algorithms used by EPFS should all scale relatively well to high inode/block counts, the library is not really intended to be used for holding more than a couple hundred files. i.e. complaints that it slows down or fails in weird ways when adding 30k files to it won't be taken seriously (but i would nonetheless like to hear about them). That said, it _theoretically_ can scale to any inode/block count ranges within its numeric limits without noticeable slow-downs. The most significant i/o operations, e.g. de/allocating blocks and inodes and translating p-file cursor positions to storage-side positions, are amortized O(1), with O(N) components when allocating _new_ blocks (N=block size). The "namer" interface does not make specific performance guarantees (e.g. for search-by-name), but the only current implementation is [a hash-table](http://whiki.wanderinghorse.net/wikis/whio/?page=whio_ht) with O(1) average-case search/insert/remove performance.

Memory usage:

The underlying C bits, not counting the few bytes for the C++ wrapper, generally need _less than 1kb_ of dynamically-allocated memory for typical use cases (only a couple of p-files opened at a time). i.e. an EPFS container only needs about 2-3 times the memory of an `fopen()`ed `FILE` handle (which allocates over 400 bytes on my machine). Each _opened_ inode needs additional memory to manage its mutable state and its block chain (_somewhere around_ 48 bytes + 8 bytes per data block owned by the inode). When an inode is opened multiple times, most of that memory is shared amongst all opened copies. Only _opened_ inodes with _really long_ block chains use any significant amount of memory (potentially even growing larger than a massive _100 bytes_! ;).

The point of all that is - whio, _especially_ EPFS, is heavily optimized for a _very small_ memory footprint (smaller than many JSON strings and other client-side constructs). The JS side of the bindings _certainly_ takes up more memory than the lower-level bits do (but how much the v8 side requires is essentially unknowable, depending on implementation details of v8).


# Notes about the C++/v8 Binding Implementation #

This API is a mixture of 3rd-party code (libwhio, with a core implemented in C and C++ wrapper on top of that) and libv8 (C++ and JS), with [V8Convert](V8Convert.md) sitting between them to convert the types and function calls between C/C++ and JS. All of the whio-provided classes described on this page come "directly" from libwhio, requiring _no_ additional level of JS-related subclassing or redirection in order to bind them to JS (as if often the case when binding classes). The "no extra layer of indirection" implementation is made possible (and easy to do) by the [V8Convert](V8Convert.md) core supported the [V8Convert\_ClassCreator](V8Convert_ClassCreator.md) class-binding mechanism. A relatively small handful of function bindings require a layer of hand-written functions because the C++ and JS world-views are incompatible here or because of intentional semantic differences in the JS/C++ interfaces. The notable examples are `(size_t read(void*,size_t))` and `(size_t write(void const *,size_t))`, which cannot be generically represented 1-to-1 in JS. We also wrote some constructor shims in order to implement some special-case ownership semantics which could not be automated. The vast majority of the function/member bindings required no hand-written shims, just (hand-written) instantiations of various [V8Convert](V8Convert.md)-provided templates (mostly from [V8Convert\_XTo](V8Convert_XTo.md) and [V8Convert\_ClassCreator](V8Convert_ClassCreator.md)).

The implementation files can be found here:

  * [cvv8-whio.hpp](http://code.google.com/p/v8-juice/source/browse/convert/addons/whio/cvv8-whio.hpp)
  * [cvv8-whio.cpp](http://code.google.com/p/v8-juice/source/browse/convert/addons/whio/cvv8-whio.cpp)

As a brief example, here's the main initialization code for the `MemoryIODev` class bindings:

```
void ClassCreator_SetupBindings<whio::MemoryIODev>::Initialize(
     v8::Handle<v8::Object> const & dest )
{
    typedef whio::MemoryIODev IOD;
    typedef ClassCreator<IOD> CC;
    CC & cc(CC::Instance());
    if( cc.IsSealed() )
    {
        cc.AddClassTo(TypeName<IOD>::Value, dest);
        return;
    }
    cc.Inherit<whio::IODev>();
    cc
        ("bufferSize",
         InCaCatcher_std< MethodTo<InCa, IOD, whio_size_t (), &IOD::bufferSize> >::Call )
        ("toString",
         io::toString_generic<IOD> )
        ;
    cc.AddClassTo(TypeName<IOD>::Value, dest);
}
```