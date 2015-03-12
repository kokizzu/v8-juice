

See also: [JSPDO\_API](JSPDO_API.md), [JSPDO\_HowTo](JSPDO_HowTo.md)

# About JSPDO #

JSPDO is a database access abstraction API, modelled heavily off of the [PHP PDO](http://php.net/manual/en/book.pdo.php) API. The underlying database abstraction library is called [cpdo](http://fossil.wanderinghorse.net/repos/cpdo/) and is implemented in C. (cpdo was originally written in the hopes of supporting this very library.) JSPDO basically provides a v8 wrapper around cpdo, and some of the JSPDO documentation may refer the reader to the related [cpdo documentation](http://fossil.wanderinghorse.net/wikis/cpdo/). JSPDO is not a "thin wrapper", though - it extends the native C/C++ APIs with several useful features and hides some details which can become rather tedious in the native API. It's really, _really_ easy to use.

Requirements:

  * libv8, of course
  * [V8Convert](V8Convert.md) (it's a header-only library, so there are no major installation hassles).
  * The [V8Convert\_ByteArray](V8Convert_ByteArray.md) code (only two files).
  * At least one of sqlite3, sqlite4, or MySQLv5.
  * Some patience - the build tree is currently only designed to build directly from [the original source tree](http://code.google.com/p/v8-juice/source/browse/#svn/convert/addons/jspdo) packaged together with [V8Convert](V8Convert.md). i hope to modularize it someday to make it easy to drop in to arbitrary `v8::convert`-using clients.

See [the README file](http://code.google.com/p/v8-juice/source/browse/convert/addons/jspdo/README.txt) for much more detail about the requirements and building the sources outside of their native source tree.

Supported Drivers:
  * sqlite3
  * [sqlite4](http://sqlite.org/src4) (added 20120630), but sqlite4 itself is still pre-release and very much beta, subject still to much change.
  * MySQLv5

See the [cpdo home page](http://fossil.wanderinghorse.net/repos/cpdo/) for more info than you could possibly want to know about it. JSPDO's source tree includes a complete copy of the cpdo library - it does not need to be installed separately.

## Notable TODOs ##

The more significant ones:

  * The underlying C lib is missing a couple of features which might be needed for advanced use, but it's functional enough for everything i've needed to do so far.
  * Refactor the build process to make it easily droppable into arbitrary client trees.

## Plugging JSPDO into your Application ##

See [the README file](http://code.google.com/p/v8-juice/source/browse/convert/addons/jspdo/README.txt) for details.


# Important Details to Keep in Mind #

## #1: Lifetime Issues ##

Database connections and the objects they create have very strict rules about when they must be cleaned up. The JSPDO class has a method called `close()` to release all resources owned by the database connection. The `Statement` handle class (returned by `JSPDO.prepare()`) has a `finalize()` method which _must_ be called _before_ the parent `JSPDO` instance is `close()`ed, or Undefined Behaviour will ensue. _Do not_ rely on garbage collection to free up database-related handles. Doing so will lead the Grief in the form of Undefined Behaviour.

## #2: Error Reporting/Handling ##

The vast majority of the functions in this API report errors via exceptions. Exceptions, for all their merits, make it easy to write code which does not properly finalize query/result set data. Get in the habit of using a `try/finally` block, as shown in [the examples](JSPDO_HowTo.md), to ensure that statement handles are properly cleaned up. If a single statement handle is closed _after_ its database connection is closed, the v8 engine may very well crash as the statement handle steps on an invalidated database handle. (We have since added some plumbing to avoid these crashes, but do not get lazy about this - clean up or Undefined Behaviour will eventually smack you in the face.)

## #3: Beware of Binary Data! ##

JavaScript has no APIs for working with binary data, and storing binary data in JS strings results in undefined behaviour. To try to avoid such undefined behaviour, this API treats `BLOB` fields not as strings, but as [ByteArray](V8Convert_ByteArray.md) objects. When fetching `BLOB` fields the client get have a ByteArray object and not a string, and the two are not 100% API-compatible (i.e. client code must behave slightly differently for them, even if they contain valid string data). There are examples of using binary data on the [JSPDO HowTo page](JSPDO_HowTo.md) (search for "ByteArray").

## #4: Data Types ##

This code only explicitly accounts for numeric, string, and "blob" (generic binary) data, and there are some notable limitations regarding numbers. v8's `Integer` type only supports 32 bits. Thus this library tries to use `double` for integers of greater than 32 bits. It is conceivable that some floating-point numeric precision will be lost, depending on the underlying db driver and the exact field definitions.

When binding JS values to database fields for purposes of inserting data, this code tries its best to do the right thing, but it is possible that a specific db driver, or slightly different database table definition, will behave differently than another when it comes to numeric precision, maximum field length, and similar details. It is recommended that databases used with this code avoid constructs like 1-byte integer fields and similar things which may cause unexpected failures at runtime (depending on the driver). e.g. if the underlying native driver returns an error because we inserted the value 500 to a 1-byte integer field, then that will show up as a JS-side exception via this interface.


# How To ... #

See [JSPDO\_HowTo](JSPDO_HowTo.md) and [JSPDO\_API](JSPDO_API.md).

# Caveats and Gotchas #

## Column Names might be Unpredictable! (Use "AS") ##

Database drivers do not always guaranty what the name of a given column will be. e.g. when using sqlite the names are not guaranteed to match how they are named in the query unless the client uses an `AS` clause, e.g. `SELECT a as a, b as b from t...`.

## Opening Multiple Statement Concurrently (Don't do it!) ##

Drivers are not required to allow more than one open result set for a given db connection (MySQL, for example, does not allow it). Thus query code should be structured so that it does not need to open multiple queries at one time.

## Multi-Threading (Don't do it!) ##

It is, in general, not legal (or not portable) for more than one thread to use the same connection (`JSPDO` instance) or statement/result handle (`JSPDO.Statement` instance), whether concurrently or not. If you use v8 in multiple threads, be sure that each thread has its own connection instance and does not share statements handles with another instance or thread.