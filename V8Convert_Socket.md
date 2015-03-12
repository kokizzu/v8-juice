

# Overview #

This page gives an overview of a `Socket` class binding implemented using [V8Convert](V8Convert.md). It "should" be relatively easy to drop in to arbitrary client apps.

This API was released on 20110619 and is still (as of this writing) quite experimental.

Minor achtung: this API technically includes TCP, UDP/datagram, raw socket, and Unix filesystem sockets/pipes, but it is untested with anything but "stream" sockets (type=`SOCK_STREAM`) with a family of `AF_INET` or `AF_UNIX`.

Another achtung: i won't claim that this is production-level code. i'm not a sockets guru and there are very likely some undiscovered bugs.

## Getting the Source ##

This code requires [V8Convert](V8Convert.md) (which is header-only). The sources are here:

> http://code.google.com/p/v8-juice/source/browse/convert/addons/

This module needs both the `socket` and `bytearray` directories. If you are building it directly from a checked-out copy of the main source, running `make` will also build a sample/test shell which includes the bindings.

## Adding the Bindings to your App ##

Grab the above source code and add the `cpp` files to your project (optionally compiling them as a separate static library or DLL).

```
#include "path/to/socket.hpp"
...
cvv8::JSSocket::SetupBindings( yourV8Object );
```

The bindings will be added to the given object, which is normally the v8 context's global object.

# Socket Class #

| **Method** | **Description** |
|:-----------|:----------------|
| `new Socket( [int family=AF_INET [, int type=SOCK_STREAM [, int protocol=0]]] )` | Constructor. TODO: add a ctor taking a configuration Object |
| `void close()` | Closes the socket and frees all resources. Further access to native methods or properties on this object will leads to JS-side exceptions. For `AF_UNIX` _server_ sockets (for which `bind()` has succeeded), this also removes the socket's pipe file from the filesystem.  |
| `string toString()` | Returns a debugging-style string. |
| `int bind( string hostNameOrAddressOrUnixPipeName, int port )` | "Binds" the socket to a given host-or-pipe/port combination. This is required for server sockets, not client sockets (which use `connect()`). Throws on error. |
| `int connect( string hostNameOrAddressOrUnixPipeName, int port )` | Connects to the given host-or-pipe/port combination. Throws on error. Server sockets should use the `bind()`/`listen()`/`accept()` combination instead. |
| `mixed read(unsigned int length[, bool binary=false])` | Reads up to `length` bytes of data from the socket. Returns a `string` if `binary` is false, else a `ByteArray` instance. |
| `int write(string|ByteArray data [,int length=data.length])` | Writes the given data to the socket. Returns the number of bytes written. |
| `int setTimeout( unsigned int seconds[, unsigned int microseconds=0] )` | Sets the i/o timeout for read/write operations. |
| `int setTimeoutMs( unsigned int ms )` | Sets the timeout in milliseconds. |
| `int listen([int backlog])` | Tells the socket to enter listening mode. Returns 0 on success, throws on error. See [man 2 listen](http://linux.die.net/man/2/listen) for details. The default backlog size is unspecified (more than 1 and less than 10). |
| `Socket accept()` | Listens for a connection, blocking until one arrives or the socket timeout expires. The socket must have been `bind()`ed and `listen()` must have succeeded before this is called. Returns null if a timeout was reached, a new `Socket` instance on a successful connection, and throws for any other condition. |
| **Property** | **Description** |
| `Array[string address,int port] peerInfo` | Only valid after a connection is established. |
| `bool timeoutReached` | If a `read()` or `write()` call fails due to a timeout then this will be `true`. The value is re-set on each call to `read()` or `write()`. |

| **Class-level Functions** | **Description** |
|:--------------------------|:----------------|
| `int getProtoByName( string protocolName )` | Returns the integer value for a given protocol name (e.g. `tcp`, `udp`, or `ntp`) |
| `string nameToAddress( string hostname )` | Resolves a host name to an address. |
| `string addressToName( string address )` | Resolves an address to a host name. |


The `Socket` constructor also has the following numeric properties, all of which correspond directly to same-named C-side constants: `AF_INET`, `AF_INET6`, `AF_UNIX`, `AF_UNSPEC`, `SOCK_DGRAM`, `SOCK_STREAM`, `SOCK_SEQPACKET`, `SOCK_RAW`.

# ByteArray Class #

The [ByteArray class](V8Convert_ByteArray.md) is available to this API via the name `Socket.ByteArray`.

# Code Examples #

Please see the various `.js` files at:

> http://code.google.com/p/v8-juice/source/browse/convert/addons/socket