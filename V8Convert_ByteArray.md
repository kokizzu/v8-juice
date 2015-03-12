

# ByteArray Class #

This class is used by the [Socket class](V8Convert_Socket.md) and [JSPDO](JSPDO.md) to allow the moderately efficient usage of binary data with JavaScript. It is basically an Array-like class which is capable of holding arbitrary data (string or binary) read from or being written to a socket.

It is visible to client code via `ByteArray`, but it might not be global scope, depending on the code which binds this class (e.g. it might be `Socket.ByteArray` and/or `JSPDO.ByteArray`). Regardless of which API(s) make this class visible, they all share the same underlying implementation, meaning that e.g. `Socket.ByteArray` and `JSPDO.ByteArray` are actually the same class (just references via different names).

`ByteArray` implements the index-of operator like arrays to, but each value at a given index is a single byte (a small integer).

Note that this class _does not_ implement the [CommonJS ByteArray interface](http://www.commonjs.org/specs/), though it is incidentally superficially similar.

## Source Code ##

http://code.google.com/p/v8-juice/source/browse/#svn%2Fconvert%2Faddons%2Fbytearray

## API Overview ##

|**Function** | **Description** |
|:------------|:----------------|
|`new ByteArray([int length=0])` | Creates a new byte array holding the given number of bytes (all with the value 0). |
|`new ByteArray(string data [, int length=0])` | Creates a new byte array from the given string. |
|`new ByteArray(ByteArray data)` | Creates a new deep copy of the given byte array. |
| `void destroy()` | Destroys all of the underlying native data. Further operations on this object will cause an exception to be thrown. This is optional, but can be used to free up memory before the garbage collector would normally do so. |
| `int index-operator[int index]` | Returns the byte value at the given index. When changing values, an out-of-range index leads to an exception. Set `length` to explicitly expand a `ByteArray`. TODO: add `autoExpand` option. |
| `void append(Number|String|ByteArray)` | Appends either a single byte or a set of string/ByteArray bytes to this buffer. |
| `ByteArray gzip()` | Gzip-compresses this object's bytes into a new `ByteArray`. |
| `ByteArray gunzip()` | Gunzip-decompresses this object's bytes into a new `ByteArray`. |
| `string stringValue()` | Converts the data to a JS string. Results are undefined if the data is not UTF8-encoded string data. This is a function instead of a property because it has to perform the conversion to a string each time it is accessed. |
|**Property** | **Description** |
| `int length` | The length of the object's data, in bytes. May be modified. When growing, new bytes have the value `0`. |
| `bool isGzipped` | Is true if this object _appears to_ contain gzip-compressed data, else false. It throws if set by client code. |

Class-level members:

| bool `zlibEnabled` | True if this library was compiled with zlib support, else false. |
|:-------------------|:-----------------------------------------------------------------|