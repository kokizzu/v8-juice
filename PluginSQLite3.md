**ACHTUNG:** the current version (20090313+) of the C-style interface provided by this plugin is sorely broken because it is in the middle of a rewrite. Don't use it until this warning note goes away. The OO API "should" be okay (works for me).




# The sqlite3 bindings #

The sqlite3 bindings wrap about 60 sqlite3 functions for use in JavaScript, providing access to database files from script code.

All of the most significant features of sqlite3 are supported except:

  * Custom collation sequences (no plans to implement these)
  * Custom authorization functions (also no plans)
  * The BLOB-related functions are missing. There would seem to be relatively few safe (in terms of type conversion) ways to use this from JS.
  * The UTF16 functions are missing, as v8 does not directly support UTF16 (only in a roundabout manner).
  * `sqlite3_create_function()` API currently only works with normal functions (not aggregates), for reasons i don't understand. i'm hoping the rewrite will solve this.

Everything else one would "normally" need from script code is scriptable. The API provides native-side access to the DB handles created from script code, so clients can extend their script-side DBs using functionality not provided by these bindings.

## The source code ##

http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/sqlite3/

The current (20090317) code is in `v8-attempt2.cc`, and the older implementation is in `v8-sqlite3.cc`.


# The C-style API #

## JS API Docs ##

The non-OO API of this plugin has its own manual, in ODT format, available from:

http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/sqlite3/v8-juice-plugin-sqlite3.odt

For the API of the object-oriented interface, see below.

## Known problems ##

  * The `sqlite3_create_function()` API may crash when used with aggregate functions, for reasons i don't at all understand (it seems that my function handles may be disappearing again).
  * Applies to the older C-style API, but not the rewrite:_NEVER_ dereference (print, assign a property to, or even check for booleaness) any handle type given back from this library. The DB handle type is (now) an Object and can be safely manipulated, but the handles used by `sqlite3_prepare()`, `sqlite3_value_xxx()`, and `sqlite3_result_xxx()` are internally `v8::External` handles and _will_ cause a crash if they are dereferenced from script code. This might be fixed at some point.

## Example ##

Here's a rather contrived example which i use for testing out the bindings:

```
load_plugin('v8-juice-sqlite3');
var my = {db:0,stmt:0};
my.db = sqlite3_open("my.db");
print("Using sqlite3 version",sqlite3_libversion());

my.stmt = sqlite3_prepare( my.db, "create table if not exists t(a,b,c)");
print('typeof stmt ==',typeof my.stmt,', errmsg =',sqlite3_errmsg(my.db));
var rc = sqlite3_step( my.stmt );
print("step() rc ==",rc);
rc = sqlite3_finalize( my.stmt );
my.stmt = 0;
print("finalize() rc ==",rc);


if(1)
{
    my.stmt = sqlite3_prepare( my.db, "insert into t(a,b,c) values (?,?,?)" );
    var now = (new Date()).toString();
    for( var i = 0; i < 5; ++i )
    {
	sqlite3_bind( my.stmt, 1, 42 + i );
	sqlite3_bind( my.stmt, 2, 24.42 * i );
	//sqlite3_bind_text( my.stmt, 3, now+": #"+i, 10 );
	sqlite3_bind_text( my.stmt, 3, now+": #"+i );
	sqlite3_step( my.stmt );
	sqlite3_clear_bindings( my.stmt );
	sqlite3_reset( my.stmt );
    }
    sqlite3_finalize( my.stmt );
}

print("Entry count:",sqlite3_select_text(my.db,"select count(*) from t"));


if(1)
{
    function my_exec_cb(cbdata,rows,cols)
    {
	if( 1 == ++cbdata.pos )
	{
	    print("my_exec_cb()! cbdata =["+cbdata+"]",'rows =',typeof rows, rows.length,', cols =',typeof cols,cols.length);
	    print( cols.join('\t'));
	}
	print( rows.join('\t'));
	if( 0 && (cbdata.pos > 3) )
	{
	    throw new Error("Throwing from my_exec_cb()!");
	}
	return SQLITE_OK;
    }
    //rc = sqlite3_exec(my.db,"select myfunc('hi','world')",my_exec_cb);
    rc = sqlite3_exec(my.db,"select rowid,* from t order by rowid desc limit 7",my_exec_cb,{pos:0});
    print("exec() rc=",rc);
}

if( 1 )
{
    print("sqlite3_create_function() test...");
    function my_cb( sqcx, data, values )
    {
	print( "my_cb()!!! argc =",arguments.length, ", values.length =",values.length);//,"(",values.join(','),")" );
	for( var i = 0; i < values.length; ++i )
	{
	    print( "sqlite3_value_text(value #"+i+") =",sqlite3_value_text(values[i]));
	}
	sqlite3_result_value( sqcx, values[0] );
	return SQLITE_OK;
    }

    rc = sqlite3_create_function( my.db, "myfunc", -1, SQLITE_UTF8, {msg:"hi, callback"}, my_cb );
    print("create_function() rc=",rc);
    rc = sqlite3_select_int(my.db,"select myfunc(7,11,13)");
    print("myfunc() rc=",rc,', errmsg=',sqlite3_errmsg(my.db));
}

if(0)
{
    var str = "i'm stephan, who're you?";
    print("str:", str );
    sqlite3_sleep( 1000 );
    print("esc'd:", sqlite3_escape_sql( str ) );
    sqlite3_sleep( 1000 );
    print("quoted:", sqlite3_quote_sql( str ) );
    sqlite3_sleep( 1000 );
}

if(1)
{
    print("js_eval() test...");
    sqlite3_exec( my.db, "select js_eval('sqlite3_select_int(my.db,''select count(*) from t'')')");
    print( 'js_eval():',
	   sqlite3_select_text( my.db,
				"select js_eval('print(''hello, world'');''hi there'';')"
				)
	   );
    print( 'js_eval():',
	   sqlite3_select_text( my.db,
				"select js_eval('throw new Error(''testing throwing from js_eval!'')')"
				)
	   );
    print( 'js_eval():',
	   sqlite3_select_text( my.db,
				"select js_eval()"
				)
	   );
    print("js_eval() tests done.");
}

if(1)
{
    var ar = sqlite3_select_array( my.db, "select * from t order by rowid desc limit 7" );
    print('select array:',ar);
    var sep = '\t';
    if( ar instanceof Object )
    {
	for( var r = 0; r < ar.rows.length; ++r )
	{
	    if( 0 == r )
	    {
		print(ar.cols.join(sep));
	    }
	    print( ar.rows[r].join(sep) );
	}
	//print('select cols:',ar.cols);
	//print('select rows:',ar.rows);
    }
}

print("Shuting down...");
sqlite3_close( my.db );
print("Done!");
```

And that outputs:

```
stephan@jareth:~/cvs/v8-juice/extra-plugins/src/sqlite3$ v8-juice-shell test.js 
Using sqlite3 version 3.6.2
typeof stmt == object , errmsg = not an error
step() rc == 101
finalize() rc == 0
Entry count: 90
my_exec_cb()! cbdata =[[object Object]] rows = object 4 , cols = object 4
rowid	a	b	c
90	46	97.68	Sat Mar 14 2009 18:44:40 GMT+0100 (CET): #4
89	45	73.26	Sat Mar 14 2009 18:44:40 GMT+0100 (CET): #3
88	44	48.84	Sat Mar 14 2009 18:44:40 GMT+0100 (CET): #2
87	43	24.42	Sat Mar 14 2009 18:44:40 GMT+0100 (CET): #1
86	42	0	Sat Mar 14 2009 18:44:40 GMT+0100 (CET): #0
85	46	97.68	Sat Mar 14 2009 18:38:50 GMT+0100 (CET): #4
84	45	73.26	Sat Mar 14 2009 18:38:50 GMT+0100 (CET): #3
exec() rc= 0
sqlite3_create_function() test...
create_function() rc= 0
my_cb()!!! argc = 3 , values.length = 3
sqlite3_value_text(value #0) = 7
sqlite3_value_text(value #1) = 11
sqlite3_value_text(value #2) = 13
myfunc() rc= 7 , errmsg= not an error
js_eval() test...
hello, world
js_eval(): hi there
js_eval(): Error: testing throwing from js_eval!
js_eval(): 
js_eval() tests done.
select array: [object Object]
a	b	c
46	97.68	Sat Mar 14 2009 18:44:40 GMT+0100 (CET): #4
45	73.26	Sat Mar 14 2009 18:44:40 GMT+0100 (CET): #3
44	48.84	Sat Mar 14 2009 18:44:40 GMT+0100 (CET): #2
43	24.42	Sat Mar 14 2009 18:44:40 GMT+0100 (CET): #1
42	0	Sat Mar 14 2009 18:44:40 GMT+0100 (CET): #0
46	97.68	Sat Mar 14 2009 18:38:50 GMT+0100 (CET): #4
45	73.26	Sat Mar 14 2009 18:38:50 GMT+0100 (CET): #3
Shuting down...
Done!
```


# Object-oriented API #

**ACHTUNG:** this is still very much in development. These docs
represent the state of the code in subversion.

This is an overview of the JS-side API for the object-oriented sqlite3 bindings.

Summary of the classes:

  * `sqlite3.DB` is a database object.
  * `sqlite3.Statement` is a prepared SQL statement (analog to [sqlite3\_prepare\_v2()](http://sqlite.org/c3ref/prepare.html)).
  * `sqlite3.Cursor` is a helper type for  iterating over query results.

Much of the C-style API is also provided by this plugin, and where such functions take
a database or statement handle, they really want a `DB` resp. `Statement` object. In fact,
`sqlite3_open(...)` is equivalent to `new sqlite3.DB(...)`, `sqlite3_prepare(db,...)` is equivalent to
`new sqlite3.Statement(db,...)`, and `sqlite3_close(db)` is the same as `db.close()`.

TODOs:
  * Many convenience routines are missing.
  * Tie in to the non-OO API, so we can use them together. This is partially done but there's still lots to do.
  * `sqlite3_create_function()` and friends are not yet implemented in the OO interface.

## Error codes ##

Many routines in this API return integer error codes. These are always
codes from the sqlite3 API, the most important of which are
`SQLITE_OK`, `SQLITE_ROW`, and `SQLITE_DONE` (anything else is an
error code, whereas these are success codes in different contexts).

When the API encounters an error which is not reported by sqlite3 it
will normally throw an exception.  For example, passing a non-db
object to a function requiring one will cause an exception to be
thrown.

## Destruction of objects (IMPORTANT!) ##

JavaScript, like most languages with built-in garbage collection, does not
guaranty when objects will be deleted. GC-oriented languages are the natural
antithesis of native objects which rely on specific destruction semantics and
ordering. A database wrapper is one such case where GC and the underlying
objects play particularly poorly together. Because of this, it is essential
that clients clean up their own database-related objects from their scripts.
JS may eventually do it, but it may very well destroy things in an order which
is incompatible with the destruction requirements of the native objects.

To destroy objects:

The DB class has a `close()` method, and the Statement and Cursor classes each
have a `finalize()` method. Each of these ends our use of the associated
underlying native resources (e.g. the db handle or prepared statement). It is
_critical_ that they be called by the user. Relying on the non-deterministic
destruction via JS GC will eventually lead to problems, and potentially a crash.

`sqlite3_close()` will fail with an error if any statements are opened. Thus
statements must be finalized (call `st.finalize()` on it when you're done). A
Cursor object directly refers back to a specific statement and _must_ be
destroyed (call `curs.finalize()`) _before_ the associated statement is
finalized. If a statement is destroyed before the cursor is, it can cause the
cursor to try to use a non-existent resource, causing a crash. If neither the
statement nor the cursor are closed by the client then GC might eventually
destroy them in the wrong order, leading to a crash at some arbitrary (and
completely unrelated) point in your application.

**To summarize:**

  * Always call `Cursor.finalize()` to destroy a `Cursor` object _before_ the corresponding statement is finalized.
  * Always call `Statement.finalize()` to destroy a `Statement` object _before_ the corresponding database is closed.
  * Always call `DB.close()` to close a `DB` object _after_ all associated statements and cursors have been destroyed.


## sqlite3.DB class ##

The `DB` class manages a single sqlite3 database connection.

### Example ###

```
var db = new sqlite3.DB("my.db");
db.execute("create table if not exists t(a,b,c)");
...
db.close();
```

Note that sqlite3 reserves the special filename `":memory:"` to create an in-memory database. The v8 engine is not aware of such memory, and thus in-memory databases are not counted for purposes any VM-imposed memory limit.

### Function list ###

| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| `DB` | `new DB(String filename)` | Opens the given db or throws (remember that `:memory:` is a special name for in-memory databases. |
| `DB` | `sqlite3_open(String filename)` | Same as the constructor. |
| `int` | `changes()` | Returns the number of rows modified by the most recently completed SQL statement. |
| `int` | `clear()` | _BE CAREFUL!!!_ Drops all tables and views in the database! |
| `void` | `close()` | Closes the DB connection. No member functions should be called after this. |
| `int` | `sqlite3_close(DB)` | Same as `db.close()`. |
| `string` | `errorMessage()` | Returns the current error text. Remember that sqlite3 uses the literal string "not an error" when there is no error! |
| `int` | `execPragma(String pragma)` | Convenience form of `execute("pragma ...")`. Argument must be in the form of an sqlite3 pragma without the leading `pragma` word, e.g. `SYNCHRONOUS=OFF`.|
| `int` | `execute(String sql [,Function callback[,mixed callbackData])` | Executes the given SQL and returns the result of `sqlite3_exec()`. The callback will be called for each row and is passed `(callbackData,Array rowData, Array colNames)`|
| `double` | `executeDouble(String sql)` | Executes the given SQL and returns the result as a double. If compilation of the SQL fails then it throws. |
| `int` | `executeInt(String sql)` | As for `executeDouble()`, but returns the value as an integer. |
| `int` | `executeInt64(String sql)` | As for `executeDouble()`, but returns the value as a 64-bit integer. |
| `string` | `executeString(String sql)` | As for `executeDouble()`, but returns the value as a string. |
| `int` | `lastInsertID()` | Returns the last row ID assigned via an insertion operation. |
| `void` | `setBusyTimeout(int ms)` | Sets the "busy timeout" of the underlying sqlite3 engine. |
| `int` | `vacuum()` | Vacuums the database, possibly shrinking its size. |


TODOs:

| `int` | `createFunction(String name, Function func1, Function step, Function final, Mixed userData)` | As per `sqlite3_create_function(....)`. |
|:------|:---------------------------------------------------------------------------------------------|:----------------------------------------|


### `execute()` callbacks ###

The `DB.execute()` and `sqlite3_execute()` functions take an optional callback function argument.
The callback is called one time for each row of data and should have this signature:

```
function callback(Mixed callbackData,Array rowData, Array colNames);
```

The `callbackData` is an arbitrary data object which the user passes
as the final argument to `execute()`. It can be used to accumulate
results, or whatever else the caller might want to collect before the
call to `execute()` returns. The `rowData` argument is a single db
record.  The `colNames` array contains the names of the columns from
the `rowData` array.

If the callback returns any value other than `SQLITE_OK` (which is 0) then execution
of the SQL stops with an error.

Here's a simple example which simply prints out all results via the
callback. Notice how it uses the callback data object to know whether
or not it should print out the header line and to hold a separator
character.

```
function my_exec_cb(cbdata,rows,cols)
{
    if( 1 == ++cbdata.pos )
    {
        print("my_exec_cb()! cbdata =["+cbdata+"]",'rows =',typeof rows, rows.length,', cols =',typeof cols,cols.length);
        print( cols.join(cbdata.sep));
    }
    print( rows.join(cbdata.sep));
    return SQLITE_OK;
}
var udata = {pos:0,sep:'\t'};
var rc = my.db.execute("select rowid,* from t order by rowid desc limit 7",my_exec_cb,udata);
print(udata.pos,"row(s) processed.");
```

## sqlite3.Statement class ##

The `Statement` class represents a single SQL prepared statement. A prepared statement is an SQL string which has been "compiled" for use with the database engine, and they are critical for implementing all but the most utterly basic SQL queries.

### Function list ###

| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| `Statement` | `new Statement(DB [,string sql])` | Creates a new prepared statement for the given db. If no sql is given `prepare()` must be called before the object is used. If sql is given and it cannot be compiled then the ctor throws. |
| `Statement` | `sqlite3_prepare(DB,string sql)` | Same as `new Statement(db,sql)` |
| `int` | `bindDouble( int ndx, double val )` | Binds a value as a double to the given index (1-based!) in the query. |
| `int` | `bindInt(int ndx, int val)` | Binds a value as an int to the given index (1-based!) in the query. |
| `int` | `bindInt64(int ndx, int val)` | Binds a value as a 64-bit integer to the given index (1-based!) in the query. |
| `int` | `bindNull(int ndx)` | Binds NULL to the given index (1-based!) in the query. |
| `int` | `bindText(int ndx, string text [,length=text.length])` | Binds a value as a string to the given index (1-based!) in the query. If the 3rd argument is specified, only that many bytes (at most) are used. |
| `int` | `clearBindings()` | Removes all bound values. |
| `int` | `columnCount()` | Returns the number of columns in the statement. |
| `void` | `finalize()` | Destroys the statement. This must be called to ensure that the underlying statement is destroyed. Failing to do so may lead to failure in later DB operations, or even a crash later on in the app (when the statement tries to clean up out of context). |
| `int` | `sqlite3_finalize(Statement)` | Same as `statement.finalize()` |
| `Cursor` | `getCursor()` | Returns a new Cursor object for traversing a query's results. |
| `bool` | `isPrepared()` | Returns true if the last `prepare()` succeeded. |
| `int` | `prepare(string sql)` | Prepares the given SQL for execution. Unlike the constructor, if compilation of the SQL fails then it returns a non-zero error code (as specified by `sqlite3_prepare()`). It is okay to re-use a Statement object by calling `prepare()` with new SQL code. |
| `int` | `reset()` | Resets the status of the query, such that looping over the results will start back at the beginning. |

### Example ###

```
var st = new sqlite3.Statement(db,"select * from t");
var sep = '\t';
var curs = st.getCursor();
var colCount = curs.columnCount();
print( curs.columnNames().join(sep) );
while( SQLITE_ROW == curs.step() )
{
        var ar = [];
        for( var c = 0; c < colCount; ++c )
        {
            ar.push( curs.getString(c) );
        }
        print(ar.join(my.sep));
}
curs.finalize(); // IMPORTANT: not doing this may lead to a "locked" DB or even a crash during GC at shutdown.
st.finalize(); // Again, IMPORTANT.
```

The various "bind" functions can be used to attach values to query string placeholders, for example:
```
var st = new Statement(db,"insert into t values(?)");
st.bindString(1,"Hi, world");
...
st.finalize();
```

## sqlite3.Cursor class ##

The `Cursor` class is used for iterating over a result set. See the Statement example above.

### Function list ###

| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| Constructor | Call `Statement.getCursor()` to create a Cursor object. |
| `void` | `finalize()` | Destroys the cursor. _MUST_ be called _BEFORE_ the associated statement is finalized. See `Statement.finalize()` for details. |
| `int` | `columnCount()` | The number of columns in this result set. |
| `string` | `columnName(int ndx)` | The column name of the 0-based result column. (In contrast to the SQL bind interface, which uses 1-based indexes!) |
| `Array` | `columnNames()` | An array of all column names. |
| `double` | `getDouble(int ndx)` | The value of the given 0-based result column, converted to a double. If the index is out of bounds it throws. |
| `int` | `getInt(int ndx)` | Like `getDouble()`, but for integer values. |
| `int` | `getInt64(int ndx)` | Like `getDouble()`, but for 64-bit integer values. |
| `string` | `getString(int ndx)` | Like `getDouble()`, but for string values. |
| `bool` | `isNull(int ndx)` | Returns true if the given column has an SQL NULL value. |
| `int` | `reset()` | Equivalent to calling `reset()` on this object's associated Statement. |
| `int` | `step()` | Steps one result row and returns the result of [sqlite3\_step()](http://sqlite.org/c3ref/step.html). In general `SQLITE_OK`, `SQLITE_ROW`, or `SQLITE_DONE` may be success values, depending on the SQL code being executed. |

All cursors which refer to a Statement object all refer to the same underlying statement, so reseting one of them will reset the statement for all related cursors. Also note that when a cursor is finalized, the underlying statement is reset (this has a long history in the underlying C++ implementation, and is not easily changed due to the reference counting involved).