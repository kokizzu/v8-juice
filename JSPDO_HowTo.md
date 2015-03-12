

See also: [JSPDO](JSPDO.md), [JSPDO\_API](JSPDO_API.md)

This page gives an overview of how to do just about everything there is to do with the [JSPDO](JSPDO.md) API.

# Building and Installing #

## Unix-like Setups ##

First, [check out](http://code.google.com/p/v8-juice/source/checkout) the `convert` directory:

> http://code.google.com/p/v8-juice/source/browse/#svn/convert

Then see [README.txt](http://code.google.com/p/v8-juice/source/browse/convert/addons/jspdo/README.txt) (after check-out: `convert/addons/jspdo/README.txt`).

## Adding to Your Project ##

See the [the JSPDO README](http://code.google.com/p/v8-juice/source/browse/convert/addons/jspdo/README.txt).


# Managing Connections #

## Connecting ##

To connect to a database we need a so-called DSN string and possibly user credentials. See [the cpdo wiki](http://fossil.wanderinghorse.net/wikis/cpdo/?page=DSN) for the full description of the DSN string format and details regarding to the various driver-specific configuration options.

Examples:

```
// For sqlite3:
var db;
db = new JSPDO("sqlite3:/path/to/db.sqlite3;openmode=rwc");
// For an in-memory database use: "sqlite3::memory:"

// For sqlite4:
db = new JSPDO("sqlite4:/path/to/db.sqlite4");

// For MySQLv5:
db = new JSPDO("mysql5:dbname=abc;host=localhost",
                   "user", "password");
```

ACHTUNG: as of 20120630, sqlite4 itself is still very much beta and up for change. See the [cpdo\_sqlite4](http://fossil.wanderinghorse.net/wikis/cpdo/?page=cpdo_sqlite4) page for the known caveats, bugs, and limitations regarding the sqlite4 support.

## Disconnecting ##

It is imperative that client code always close a db handle when he is done with it, as opposed to relying on the garbage collector (which may literally never be called).

To do that:

```
db.close();
```


After calling `close()`, any further calls to member functions will cause a JS-side exception to be triggered.

It is extremely important that _all_ objects returned from `db.prepare()` be destroyed (by calling their `finalize()` method) _before_ the db instance is destroyed). Violating this can lead to a crash when a dead object is stepped on (or another object re-allocated at the same address is mis-used!). Thus clients _must not_ leave their destruction to the whims of the garbage collector. See below for how to easily ensure that such objects are cleaned up at the right time.

# Queries #

## Running simple Queries ##

"Simple" SQL, without any runtime-bound arguments and/or returning no results, can most easily be run like this:

```
db.exec("SQL GOES HERE");
```

The JS API does not have routines for escaping SQL data. To avoid escaping problems altogether, and help avoid potential SQL injection attacks, always use prepared statements (see the next section). i so strongly believe in the value of prepared statements that i currently have no plans to ever add escaping-related functions to the JS API.

## Preparing SQL Statements ##

"Non-simple" SQL, e.g. statements requiring values acquired at runtime or returning results to the user, requires the use of so-called "prepared statements", which are created like this:

```
var st = db.prepare("SQL GOES HERE");
```

That returns an object of the class `JSPDO.Statement` (a bit of a misnomer, since it is also holds result set data). It throws an exception on error, in which case `db.errorCode` and `db.errorText` _hopefully_ contain some useful information about the error.

For `SELECT`-like queries: after preparation, the `st.columnCount` property and the `st.columnName()` and `st.columnType()` functions can be used to fetch information about a given column in the result set.

## Executing Prepared Statements ##

`prepare()` compiles the SQL for use with the database but does not execute it. To run the SQL:

```
st.step();
```

That will execute the code and, in the case of queries returning data, populate `st` with the results from the first row of the result set.

If the SQL is a `SELECT` statement then we need to loop over it:

```
while( st.step() ) {
   ... we got a row ...
   ... use st.get() to fetch the data ...
}
```

`step()` returns true if a row was fetched, false at the end of a result set or non-fetching query (e.g. `INSERT`), and throws an exception on error. On error its `errorCode` and `errorText` properties "should" give an indication of the problem.

The functions `stepArray()` and `stepObject()` can be used to step/read the row data in a single operation.

The next sections show how to use a prepared statement for various types of queries.

## Binding Parameters ##

The API supports executing "plain SQL" and prepared statements, but it does not have APIs for escaping SQL. This is an intentional design decision intended to avoid the can of worms which comes along with escaping. Using prepared statements is superior in every way to executing "plain" SQL and doing the value escaping in the client code.

Statement objects created from SQL containing bound parameters will have a numeric `paramCount` property telling the user how many parameters the query has for binding purposes.

We can bind parameters to queries before executing them, as demonstrated below...

```
st = db.prepare("SELECT ... WHERE a=? AND b=?");

// The following approaches are equivalent:

// Bind as array:
st.bind(["v8-juice","JSPDO"]);

// Two binds by index: (remember: binding uses 1-based parameter indexes!)
st.bind(1,"v8-juice");
st.bind(2,"JSPDO");
```

Or the same query but using named parameters:

```
st = db.prepare("SELECT ... WHERE a=:p1 AND b=:p2");

// The following approaches are equivalent:

// Bind as object:
st.bind({":p1":"v8-juice",":p2":"JSPDO"});
// Is equivalent to:
st.bind({p1:"v8-juice",p2:"JSPDO"}); // note the different property names

// Is equivalent to using two binds by index:
st.bind(':p1',"v8-juice");
st.bind(':p2',"JSPDO");

// The following _might_ work, depending on the driver:
st.bind(1,"v8-juice");
st.bind(2,"JSPDO");
// For portable behaviour, Do Not mix named and unnamed parameter binding!
```

**Achtung #1:** though `bind(Object)` allows object keys to be missing the `:` prefix character, `bind(string,value)` _does not_. e.g. `bind({p1:...})` and `bind({":p1":...})` are equivalent, but `bind('p1',...)` will not work (use `bind(':p1',...)` instead).

**Achtung #2:** named parameters support is an _optional_ feature for the underlying db drivers. Both of the current drivers support them, however (it can optionally be _disabled_ [in the MySQL driver](http://fossil.wanderinghorse.net/wikis/cpdo/?page=cpdo_mysql5)).

To get the index of a given parameter name:

```
var index = st.paramIndex(":paramName");
```

That will return 0 if the parameter is not found (bind parameters are 1-based), as opposed to throwing an exception.

## Fetching Column Names and Type Info ##

The column names of `SELECT`-like queries can be fetched using `columnName(index)`, which takes a 0-based index and throws if the index is out of bounds. Alternately, the `columnNames` property (an array) can be used to get them. Bound parameter names can be fetched via `paramName(index)` (which throws on out-of bounds) or the `paramNames` array. The `column/paramNames` properties will have "a false value" (not an empty array) if the statement has no fetchable columns resp. named bound parameters.

The db-declared type of a given column (as interpreted by the db abstraction layer) can be fetched via `columnType(index)`, which also takes a 0-based index and throws if the index is out of bounds. The integer returned by that function can be used in conjunction with `JSPDO.columnTypes` to figure out what type of data the field holds:

```
var ct = JSPDO.columnTypes;
switch( st.columnType(0) ) {
    case ct.STRING: ...; break;
    case ct.NULL: ...; break;
    ...
}
```

That said, knowing a field's db-declared type is probably seldom useful at the level of JS code.

The entries in `JSPDO.columnTypes` are: `ERROR`, `NULL`, `INT8`, `INT16`, `INT32`, `INT64`, `FLOAT`, `DOUBLE`, `STRING`, `BLOB`, `CUSTOM`

Some things to keep in the back of your mind about JS/DB data type conversions:

  * v8's integer type only supports 32 bits, so doubles are used for numeric values larger than 32 bits.
  * This API optimistically assumes that string fields contain UTF-8-encoded string data and will pass the data to the `v8::String::New()` constructor. If the data is indeed not UTF-8, this will lead to undefined results.
  * BLOB fields are returned as [ByteArray](V8Convert_ByteArray.md) objects. _If_ they contain legal UTF-8 data then `ByteArray.stringValue()` can be used to convert it to a string.
  * Depending on the database, binding a value of a different type to a field might or might not produce an error. e.g. sqlite3 will (normally) bind anything to any column regardless of their declared types whereas MySQL won't bind a string to a numeric column (whether or not it returns an error or sets the value to 0 is up to the driver!).

## Fetching Row Data ##

`step()` is used to fetch row data and `get()` is used to get individual values from a row. It is _absolutely illegal_ to call `get()` unless `step()` returns true. `step()` does not actually return a row - it modifies the statement object's internal SQL cursor to point to the data for the next row. `get()` can then be used to fetch the individual fields from that row.

`get()` takes a _0-based_ column index and returns the JS value for that column (insofar as possible - it will throw if it can't figure out what to do).

```
var st = db.prepare("SELECT ...");
var i, row;
try {
  var colCount = st.columnCount;
  // ^^^^ This is more efficient than accessing (the native)
  // st.columnCount in our loop.
  while( row = st.step() ) {
    for( i = 0; i < colCount; ++i ) {
      print('Column #'+i+' type='+st.columnType(i)+': '+st.get(i));
    }
  }
}
finally { st.finalize(); }
```

It is often simpler (but arguably less efficient) to use one of the other `step()` variants:

```
var row;
while( row = st.stepArray() ) print(row.join('\t'));
// Or:
while( row = st.stepObject() ) print(JSON.stringify(row));
```

Using `stepArray()` _might_ not be notably less efficient than a manually loop (this is not entirely clear), but `stepObject()` is almost certainly less efficient, in large part because it has to copy the column names for each iteration.

If memory usage is of no concern and your data sets are not too large, you can use `fetchAll()` to load all results for a query in a single call:

```
var res = db.fetchAll({sql:"SELECT ..."}); // it also supports other options
print(JSON.stringify(res));
```

## Example: INSERT ##

```
var st;
try {
    st = db.prepare("INSERT INTO mytbl(a,b,c) VALUES(?,?,?)");

    // Bind some values:
    st.bind(1, "value of a");
    st.bind(2, 32);
    st.bind(3, null); // same as: st.bind(3)
    // Alternately: st.bind(["value of a",32,null]);

    // Now execute the query:
    st.step();
}
finally {
    if( st ) st.finalize(); // this is very important!!!
}
```

Note that when binding parameters databases use _1-based_ indexes, whereas when _fetching_ data (via the `get()` method), _0-based_ indexes are used. i didn't create these conventions - i just follow them.

You can also bind parameters by name, as demonstrated here:

```
var st;
try {
   st = db.prepare("INSERT INTO mytbl(a,b,c) VALUES(:pA, :pB, :pC)");
   st.bind(":pA", "hi, world");
   // Bind several named parameters at once by using an object:
   st.bind({":pB":42, ":pC": "..."});
   st.step();
}
finally {
  if( st ) st.finalize();
}
```

It is not recommended that one mix name- and index-based parameters because the abstraction API may have no control over how the underlying driver may (or may not) handle such uses.

### Fetching the Most Recent Insertion ID ###

After inserting a row it is often desirable to find out its ID. _If_ a database table is set up with an auto-incrementing id field (its name is irrelevant, but we will call it `id` here). To fetch the "last insert ID":

```
var id = db.lastInsertId();
// or: db.lastInsertId("tableOrSequenceHint")
```

The second form is not used by the current drivers, but the API supports it in case future drivers do.

**Achtung:** when using server-side on-insert triggers in a db table and those triggers perform an insert as a side-effect of the client-performed insert, the ID returned by `lastInsertId()` might not be the ID of the record the client inserted! It might instead by the ID of the record inserted by the trigger (which is probably in a different table!).

## Example: SELECT ##

```
var st;
try {
    st = db.prepare("SELECT a as a, b as b from mytbl");
    // st is now a Statement object.

    // You can also bind parameters here if you need to,
    // as demonstrated elsewhere.

    // Display a header line with the column names:    
    var separator = '\t';
    print( sp.columnNames.join(separator) );


    // Now execute the query, stepping over each row in the
    // result set:

    // Now step through each row and print its columns out:
    var cols = [];
    var colCount = sp.columnCount;
    // ^^^ accessing sp.columnCount requires a native-level function
    // call, so i recommend copying it if you want to loop over it.
    while( st.step() ) {
        cols.length = 0;
        for( i = 0; i < colCount; ++i ) {
            cols.push( st.get(i) );
        }
        print(cols.join(separator));
    }
}
finally {
    if( st ) st.finalize(); // this is very important!!!
}
```

**Achtung:** if you do _not_ use an `AS` specifier for each column name, the database drivers _might_ return a string different than the one used in the `SELECT` code (this is driver-dependent). If you need specific column names, always use an `AS` clause, e.g. `SELECT A AS A, B AS B...`.

## Cleaning Up ##

_Always_ call `finalize()` when you are done with statement/result set object:

```
st.finalize();
```

After calling `finalize()`, calling any further calls to member functions will cause a JS-side exception to be triggered.

As mentioned above, it is always best to put statement operations in a `try/finally` block:

```
var st;
try {
    st = db.prepare(...);
    ...
}
finally { if(st) st.finalize(); }
```

You may of course include an explicit `catch` block if you like (if you do not, the exception is propagated but the statement is still cleaned up).

# Binary Data (ByteArray) #

This API can bind and fetch binary data stored in BLOB fields. Since JS has no native binary data support, this requires another level of indirection, the [ByteArray](V8Convert_ByteArray.md) class.

To bind a binary value:

```
var ba = new JSPDO.ByteArray();
... populate ba ...
st.bind(index, ba);
ba.destroy(); // not strictly necessary, but frees memory immediately.
```

`ByteArray`s can of course be used in the other `bind()` overloads, e.g. `st.bind([myByteArray,...])`.

In general it is also legal (at least semantically) to bind a string value to a BLOB field, but whether or not this will work depends on the underlying db driver.


When fetching column data (via `get()` or one of the `stepXXX()` variants), non-`NULL` values which come from fields declared to be of type `BLOB` (or similar, depending on the driver's SQL dialect) will be returned to the caller as `ByteArray` objects. Fetching BLOB data is just like fetching any other value:

```
var ba = st.get(index);
if( ba instanceof JSPDO.ByteArray ) {
  ... it came from a BLOB field and has a non-NULL value ...
  ba.destroy(); // not strictly necessary, but frees memory immediately.
}
```

Note that a 0-byte BLOB value is returned as `null`, not as an empty `ByteArray`.

If, and only if, a `ByteArray` contains a UTF-8 value is it legal to convert the contents to a string:

```
var str = ba.stringValue();
```

Note that `ByteArray.toString()` does _not_ convert the object to a JS string - it creates a string similar to one one returned by `Object.toString()` (`[object ByteArray...]`). It was decided that performing a string conversion here is too risky because it's easy to accidentally do from client code and can have undefined results (e.g. a crash) if the data is indeed not valid string content. Thus we require the call to `stringValue()` if the client wants to convert the data to a string.

> Historical note of no real consequence: `stringValue` was originally a non-function property, but the conversion to `v8::String` had to be done on every access, so i switched it to a function to "force" clients to get/reuse a reference to the string. The previous behaviour could cause weird side-effects when used in seemingly intuitive (but semantically wrong) ways.


## De/Compressing Field Data ##

The [ByteArray](V8Convert_ByteArray.md) class used for BLOB field data supports gzip de/compression, and we can take advantage of that to de/compress large db data (e.g. large JSON documents). Here is an example:

(The `assert()` function used below comes from external test-related utility code.)

```
function testGzippedJSON(db)
{
    db.exec("DROP TABLE IF EXISTS ziptest");
    db.exec("CREATE TABLE ziptest(name VARCHAR(50) NOT NULL, json BLOB DEFAULT NULL)");
    var theObj = { // an object to store in the DB as compressed JSON
        a:"Äaöoüu",
        b:42,
        db:db.toJSON(),
        list:[]
    };
    // Add some filler so that the data can actually compress a bit...
    var i;
    for( i = 1; i <= 10; ++i ) { theObj.list.push("Line #"+i+"\n"); };
    var json = JSON.stringify(theObj,0,4);
    // Stuff it into the database...
    var st;    
    try {
        print("Inserting compressed data....");
        st = db.prepare("INSERT INTO ziptest (name,json) VALUES (?,?)");
        st.bind(1,"bogo");
        var bac, ba = new JSPDO.ByteArray( json );
        bac = ba.gzip();
        ba.destroy();
        st.bind(2,bac);
        bac.destroy();
        st.step();
        print("Inserted compressed data.");
        st.finalize();
        st = null;
        // Now read it back and compare it ot the original...
        db.exec({
            sql:"SELECT name as name,json as json FROM ziptest",
            each:function(st) {
                var ba = st.get(1);
                assert(ba instanceof JSPDO.ByteArray,'Field is-a ByteArray');
                print("Got compressed field data: "+ba);
                var buc = ba.gunzip();
                ba.destroy();
                var sv = buc.stringValue();
                print("Uncompressed JSON data: "+buc+" Length: "+
                    buc.length+" bytes/"+sv.length+' characters:\n'+sv);
                buc.destroy();
                assert( sv === json, "Decompressed JSON matches original" );
            }
        });
    }
    finally {
        if(st) st.finalize();
    }
}
```

On my machine that produces output similar to:

```
Inserting compressed data....
Inserted compressed data.
Assertion OK: Field is-a ByteArray
Got compressed field data: [object ByteArray@0x215d240, length=180]
Uncompressed JSON data: [object ByteArray@0x2161270, length=379] Length: 379 bytes/376 characters:
{
    "a": "Äaöoüu",
    "b": 42,
    "db": {
        "dsn": "sqlite3::memory:",
        "id": "[cpdo::driver@0x211d750, driver=sqlite3]"
    },
...
}
Assertion OK: Decompressed JSON matches original
```