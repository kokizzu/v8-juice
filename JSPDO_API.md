

See also: [JSPDO](JSPDO.md), [JSPDO\_HowTo](JSPDO_HowTo.md)

# JS API Overview #

This page provides an overview of the [JSPDO](JSPDO.md) JS API. See the [JSPDO\_HowTo](JSPDO_HowTo.md) page for examples of using it.

Unless noted otherwise, the API reports any errors via exceptions. Some types of errors (e.g. trying to convert a string to an integer) will not trigger explicit errors but may result in confusion on the user's part (e.g. converting a string to a number will always result in the value zero).

# JSPDO Class #

This class is synonymous with a database connection. Its main responsibilities are establishing a connection and doling out "statement" objects via its `prepare()` method.

| **Method** | **Description** |
|:-----------|:----------------|
| `JSPDO(dsn[,userName,password])` | Connects to the database described in the given DSN string. See [the cpdo wiki for details](http://fossil.wanderinghorse.net/wikis/cpdo/?page=DSN). |
| `void exec(string|Object)` | Executes simple SQL string statements (those not returning values) or, if passed an Object, allows executing a callback for each returned row. Described in detail below. |
| `Statement prepare(string sql[, objectOrArray bindData])` | "Prepares" SQL code for execution. This is the only legal way to construct Statement objects. If `bindData` is provided then `bind(bindData)` is called to bind any parameters. |
| `Statement prepare(Object opt)` | Equivalent to `prepare(opt.sql,opt.bind)`. |
| `void close()` | Closes the db connection and frees any C-level resources allocated for the connection, _except_ for Statements, which _must_ be `finalize()`d _before_ their db is closed. |
| `int lastInsertId()` | Returns the "most recent" insertion ID for this connection, but this might not be the ID one may expect (triggers can cause IDs to be updated in ways opaque to the client application). |
| `void begin()` | Starts a transaction. Nested transactions are not supported. |
| `void commit()` | Commits the current opened transaction. |
| `void rollback()` | Rolls back the current transaction. |
| `Array fetchAll(object)` | Executes a query and returns all result records. Described in detail below. |
| `mixed selectValue(string|Object)` | Selects a single value from the database. Described below. |
| `string quote(string value)` | Returns the SQL-quoted form of a string. |
| `string qualify(string identifier)` | Returns the SQL-quoted form of a table or column name. |

Instance properties (read-only!):

| `string driverName` | The name of the underlying driver ("sqlite3" or "mysql5"). |
|:--------------------|:-----------------------------------------------------------|
| `int errorCode` | The most recent error code (driver-specific). May be reset by the next call into the API. The code 0 is universally understood as success and a non-0 value is driver-dependent. |
| `string errorText` | The most recent error string (driver-specific). May be reset by the next call into the API. |
| `string dsn` | The DSN string passed to the constructor. |


Class-level properties (read-only!) (properties of the constructor function):

| `Array driverList` | A list of the names of all available database drivers. |
|:-------------------|:-------------------------------------------------------|
| `Object columnTypes` | Mapping of symbolic names to integer values for use with the `columnType()` member of the statement class. |
| `String readFileContents(name[,bool binary=false])` | Tries to read the entire contents of the given file into memory and return it. If the the 2nd argument is false then it treats the data as a string, else it treats it as a `JSPDO.ByteArray`. Intended for SQL, JSON, and the like. Results are undefined if the 2nd parameter is false and the file contains non-UTF8 data. |

## `JSPDO.exec()` ##

`exec()` can be used to execute a "simple" query (an SQL string with no bound parameter and no return data) or a prepared query with bound parameters. It can optionally call a callback function on each iteration of a result set. It is kind of a Swiss Army Knife of query functionality, and is used as the basis for several other functions, e.g. `fetchAll()` and `selectValue()`.

In its simplest form it is used like this:

```
db.exec("SQL goes here");
```

However, it can also be used to bind values and iterate over result sets:

```
db.exec({
  sql:"SELECT a,b,c FROM mytbl WHERE ID=?",
  bind:[32],
  mode:'array',
  callbackData:{rows:0},
  each:function(row,data,st){
      ++data.rows;
      print(row.join('\t'));
  }
});
```

The options supported by the function are:

| `mixed sql` | The SQL (string) code to run. It may also be a Statement object and `exec()` will, by default, finalize that statement regardless of success of failure. See the `finalizeStatement` option for how to keep ownership of a passed-in statement. |
|:------------|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `bool finalizeStatement` | Default=true. If `sql` is-a Statement object and this value is true (the default) then the statement will be finalized by `exec()`, regardless of success or failure. If it is `false` (_not_ another "false value" like `undefined`, `0`, or `null`!) then `sql` will not finalized by `exec()` (i.e. it is still owned by the caller). |
| `mixed bind` | An object or array of data to bind to the statement before execution. If it is a function then `f(statement,bindData)` is called (one time!), where `bindData` is the value of this object's `bindData` member. |
| `any bindData` | Optional data passed to `bind` if it is a function. If `bind` is _not_ a function then this data not used. |
| `function each(row,data,st)` | Called for each result set row. Described below. |
| `string mode` | If == "array", `each()` is passed an Array as the 1st parameter. "object" passes an Object as the 1st parameter. Anything else causes the statement object itself to be passed. See examples below. |
| `any callbackData` | Optional data passed as the 2nd argument to `each()`. |
| `int limit` | For `SELECT`-like queries, this option can be used to limit the number of values fetched. The default is 0, meaning no limit. This does not modify the SQL, but stops looping after this many rows have been fetched. |

Note that the `each()` function is passed the statement handle as its final parameter. This is _only_ so that clients can fetch the column names if they need to. _Do not_ call `step()` or `bind()` on the handle from the `each()` callback - doing so will lead to unpredictable results and/or errors.

### The `finalizeStatement` Option ###


By default, if the `optObj.sql` parameter passed to  `exec(optObj)` is a `Statement` object then `exec()` assumes ownership of it, meaning that it will `finalize()` the statement before it returns to the caller (regardless of whether `exec()` succeeds or not). If you want to keep ownership of such a statement object, be sure to set `finalizeStatement` to a false value _other than_ `undefined` (which is the JS-default value for unset properties, and `exec()` interprets this as `true` for this special case). The reason for why `exec()` takes ownership by default is basically ease of use. It is much easier to manage statement lifetimes this way from client code, but provides a way for the client to keep the statement intact for the 1 in 200 use cases which might need it.

Example:

```
var st = db.prepare("SELECT ...");
try {
    db.exec({ sql:st, finalizeStatement: false ... });
    ... do something else (what???) with st (it is still alive but has been fully traversed) ...
}
finally {
   // st is still valid here, and must be finalized by the client
   st.finalize();
}
```


### The `each()` Callback ###

The `each()` callback function option used by `exec()` is called for each result set row fetched by `exec()`. If `each()` is not a function (or not set) then `exec()` `step()`s the statement only one time and the result of that call is returned (this is generally only useful for `INSERTS` and such).

The callback is called with 3 arguments, `(row, callbackData, statement)`, as described below...

The exact type of the first parameter depends on the `mode` property:

  * `mode` == `"object"`: the first parameter will be the result of a `stepObject()` call.
  * `mode` == `"array"`: the first parameter will be the result of a `stepArray()` call.
  * Any other `mode` value will cause the statement object itself to be passed as the first parameter.

The 2nd argument is the value of the `exec()` options object's `callbackData` member, or `undefined` if `callbackData` is not set.

The 3rd argument is the statement itself. This is provided primarily so that clients can get the column names and whatnot, but the statement _must not_ be `step()`ed or `finalize()`d from the callback. The majority of callbacks won't need this parameter.

If `each()` returns a literal `false` (as opposed to an arbitrary false-like value like `0`, `null`, or `undefined`) then looping stops without an error. To report an error `each()` should throw an exception.

## `JSPDO.fetchAll()` ##

`fetchAll()` is an `exec({...})` wrapper which returns _all_ matching rows of a select-style query in the following structure:

```
{ columns:[ column names... ], rows:[ records... ] }
```

It requires a single Object argument with the following properties: `sql`, `finalizeStatement`, `bind`, `bindData`, `mode`. All of those have the same semantics as they do for `exec()` except that `mode` must be one of the strings (`object`,`array`) and defaults to `array`.

Each record in the `columns` array corresponds to a result set column name. Their order is guaranteed to be the same as in the query but their names are _not_ guaranteed (at the native driver level) to be the same as given in the query _unless_ the client uses an `AS` specifier in the SQL (e.g. `SELECT A AS A, B AS B...`).

Each record in the `rows` array will be either an Object or Array, depending on the `mode` option.

**Minor Achtung:**: in 'object' mode, the keys (property names) are _not_  guaranteed to be in the same order as in the original query (the ordering depends on internal details of v8's Object class implementation). In practice, they appear to be the same as their insertion order, but it's not _guaranteed_. When converting such Object-based row data to JSON for use by downstream client code, it is very possible that the key/value pairs of any given result object get re-ordered by down-stream JSON libraries. It is common to use string-keyed maps to store JSON object, and those often use lexically sorted keys. That is the real reason why the `columns` property is provided in the return results - because their ordering is always guaranteed. This is useful when clients _need_ to know the query-side order of the fields (e.g. for printing them in tab format, so that the field data match up with the field headers).

Example:

```
var res = db.fetchAll({
    sql:"SELECT a AS a,b AS b,c AS c FROM mytbl",
    mode:"object" // default == "array"
});
print(JSON.stringify(res));
```

## `JSPDO.selectValue()` ##

`selectValue()` can be used to fetch a single value from a database. It is a thin wrapper around `exec()` and supports the same arguments `exec()` supports, with the following differences:

  * The (`each`, `callbackData`, `mode`, `limit`) options are ignored.
  * If the `column` option is set then that result column (0-based index) is used for fetching the return value. The default is to use column 0 of the result set.

The return value will be `undefined` if no results matched the query, else it will be a value pulled from the first result row (subsequent rows are not traversed).

Examples:

```
var count = db.selectValue("SELECT COUNT(*) FROM T");
var otherCount = db.selectValue({sql:"SELECT A, B, COUNT(*) FROM T", column:2});

var hasObject42 = db.selectValue({
    sql: db.prepare("SELECT COUNT(*) FROM T WHERE ID=?"),
    bind:[42]
});
/** ^^^ Note that exec() takes ownership of the prepared statement by default, and finalize()s
    it before exec() returns. If you want to keep ownership, set the finalizeStatement option
    in the selectValue() call to false.
*/
```

# JSPDO.Statement #

This class represents both prepared SQL statements and result-set data. This merging of functionality was inspired by PHP's PDO (which i happen to quite like).

These objects are _only_ to be created by calling `JSPDO.prepare()`, and not via a constructor call. The constructor is visible as `JSPDO.Statement`, but it is only in the public API so that clients may use it for `instanceof` checks. It should not be called by client code and the constructor's arguments are not publicly documented.

| **Method** | **Description** |
|:-----------|:----------------|
| `void finalize()` | Frees all resources associated with the statement. All statements _must_ be finalized after use or the db handle might get in an unusable state. |
| `Statement bind(index[,value])` | Binds a value (default=NULL) to the given _1-based_ parameter index or name (in the form `:paramName`). The _type_ of the given value determines how the value is bound (e.g. as an integer, double, string, or BLOB ([ByteArray](V8Convert_ByteArray.md)). Returns this object. |
| `Statement bind(Array)` | Binds each value in the given array to the next subsequent bindable parameter. e.g. `bind([7,null,11])` binds parameters 1 to 3 with the given values. Returns this object. |
| `Statement bind(Object)` | Binds each key/value pair in the given object, treating the object's keys as bound parameter names. e.g. `bind({':p1',':p2':...})`. Returns this object. |
| `Statement bind(Statement)` | Binds all columns from the `Statement` object argument to the columns in the current statement. This can be used, e.g. to copy results from one table into another. TODO: accept array of param column numbers/names to copy, instead of copying all. Returns this object. |
| `bool step()` | "Steps" one row through a result set or executes a non-result query. Returns false if it is at the end of the set (or the query had no data to return, like an `INSERT`) or true if it successfully fetched a row. |
| `Array stepArray()` | Like `step()` but returns all of the columns from the fetched row as an array of values. It returns `null` at the end of the result set. |
| `Object stepObject()` | Like `step()` but returns all of the columns from the fetched row as an Object, using the column names as the object keys. It returns `null` at the end of the result set. |
| `mixed get(index)` | Returns a value from a fetched row, using the given _0-based_ column index. (The inconsistency is not mine - that's just how SQL APIs work!) Described in more detail below. |
| `string columnName(index)` | The name of the 0-based result set column. |
| `int columnType(index)` | The cpdo-defined type ID for row data from the given 0-based column index. (Do not rely on stable values between releases!) |
| `void reset()` | Re-sets a statement so that it can be re-executed. Call this after each iteration when changing the values of bound parameters in a loop. |
| `int paramIndex(string)` | Returns the 1-based index of the given named bound parameter, or 0 if no such parameter is found. When using MySQL (which doesn't natively support named parameters), please read [the fine manual](http://fossil.wanderinghorse.net/wikis/cpdo/?page=cpdo_mysql5) regarding potential caveats. |
| `string paramName(index)` | Returns the bound parameter name at the given 1-based index, or null if the index is out of bounds (it does not throw on out-of-bounds like most other functions). |

Instance properties (read-only!):

| `int errorCode` | The most recent error code (driver-specific). May be cleared by the next call into the API. The code 0 is universally understood as success and a non-0 value is driver-dependent. |
|:----------------|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `string errorText` | The most recent error string (driver-specific). May be cleared by the next call into the API. |
| `int paramCount` | The number of bound parameters in a prepared statement. |
| `int columnCount` | The number of columns in the current result set. |
| `string sql` | The SQL string passed to the constructor. |
| `Array columnNames` | The names of the columns, for `SELECT`-like queries, else null. Equivalent to the function `columnName(int)`. |
| ~~Array columnTypes~~ | ~~Mapping of symbolic names to integer values for use with the `columnType()` member of the statement class.~~ (Removed 20110620 because of a semantic incompatibility with `columnType()`.) |
| `Array paramNames` | The names of all bound column parameters, if any, else null. _Note that_ the array indexes are 0-based whereas the `bind()` API uses 1-based indexes! |


The majority of the functions report errors via exceptions. Because of that, statement-handling code _really really really should always_  be wrapped in a `try/finally` block, like this:

```
var st;
try {
    st = mydb.prepare(...);
    ... use st ...
}
finally { if(st) st.finalize(); }
```

Note that the `catch` part is optional - if it's not there then the behaviour is almost as if we had written `catch(e){throw e;}`, except that the stack trace stays intact. The important part is that the `finally` block is always executed, regardless of whether or not we catch the exception.

We could rewrite the above example a bit more briefly:


```
var st = mydb.prepare(...);
try {
    ... use st ...
}
finally { st.finalize(); }
```

There _may_ a subtle difference, however, depending on the code's context. `mydb.prepare()` might throw. In the second example, we would not catch that exception, but instead propagate it up the stack chain. Because of that, the `finally` block doesn't need to check if `st` was set. However, in some code constructs blindly propagating that exception might lead to resource leaks stemming from _other_ native objects which have specific cleanup requirements.

## `Statement.bind()` ##

`bind()` associates values with specific placeholder columns in a statement. It is only useful for statements which contain value placeholders, e.g. `SELECT ... WHERE ID=?` (the question mark is a placeholder) or `SELECT ... WHERE ID=:id` (`:id` is a named placeholder).

It can be called several different ways:

  * `bind(integer index [, value = null])` binds the given value to the given 1-based index.
  * `bind( Array )` is basically a loop over `bind()`, binding each subsequent argument to the next value in the array. Note that while `bind()` uses 1-based indexes, the passed-in array uses 0-based indexes.
  * `bind( Object )` binds parameters by _name_, as opposed to index, using key/value pairs from the given object. Note that named parameter support is an optional feature of the underlying db drivers, and may not be supported/activated for a given driver. (That said, the current drivers both support this.)

Be aware that `bind()` uses _1-based index_, whereas `get()` uses 0-based indexes. Don't ask me why - i don't know - ask the people who created SQL.

The JS type of a bound value determines how it is bound at the database level:

  * JS `null` or `undefined` are bound as SQL `NULL`.
  * JS numbers are bound to one of the lower-level numeric types supported by the db layer (possibly truncating them if the db uses small-precision fields!).
  * JS strings are bound as strings, of course. (Though sometimes it is useful to use BLOB fields for strings.)
  * [ByteArray](V8Convert_ByteArray.md) objects are bound as BLOB fields.
  * Other types will (or should) cause `bind()` to throw an exception.

The underlying driver _might_ fail when binding types to columns declared to be a different type, or it might perform a conversion or store the data as-is (regardless of the field's declared type). sqlite3 can store any value in any column, and ignores the declared types. MySQL, on the other hand, _might_ report an error (causing an exception in the JS code) or _might_ convert, truncate, or other change the value. e.g. trying to stuff an invalid timestamp string into a `DATETIME` field will likely result in no error but the db will store a "zeroed out" timestamp value.

## `Statement.get()` ##

The `get()` function returns the value for a specific _0-based_ column index (note that `bind()` uses 1-based indexes - this discrepancy is a mysterious part of SQL history).

This function _must never, ever_ be called unless `step()` (or one of its variants) has returned a non-false value (the exact type depends on the `step()` variant). Violating that leads to undefined behaviour.

The type of the value it returns depends on the database-driver-reported typed:

  * All driver-level integer/float/double types are converted to JS numbers.
  * BLOB fields are returned as [ByteArray](V8Convert_ByteArray.md) objects.
  * String fields are of course returned as strings.
  * Columns with `NULL` values will be returned as JS `null`, regardless of the declared field type of the column.

# ByteArray Class #

The [ByteArray class](V8Convert_ByteArray.md) is available to this API via the name `JSPDO.ByteArray`.


# How to... #

This sections provides a brief overview of how to do certain things with the API. Please see the [JSPDO\_HowTo](JSPDO_HowTo.md) page for more details.

## Step through result sets... ##

The `Statement.step()` function is used to iterate through result sets. It returns true if the statement object now contains the data for the next row, false at the end of the result set, and thrown on error.

```
var st = mydb.prepare("SELECT ...");
try {
  while( st.step() ) {
    ... use st.get() and friends to get the data ...
  }
}
finally { st.finalize(); }
```

Note that some functions and properties, namely `get()`, are only valid in the context of a successful call to `step()`. If called before `step()` has succeeded, those API members will likely throw an exception if used.

It is often more convenient to fetch all columns from a result set row at once. We can fetch them as an array of values:

```
var list;
while( (list = st.stepArray()) ) { ... }
```

Or a set of key/value pairs:

```
var obj;
while( (obj = st.stepObject()) ) { ... }
```

## Get the logical type of a column ##

```
var t = myStatement.columnType(zero_based_index);
```

The return value will map to one of the values stored in `JSPDO.columnTypes`, which is a static set of string/integer pairs containing the logical type names and their C-level values. _Do not_ rely on the values being stable between releases - use only the symbolic names (the keys of the `JSPDO.columnTypes` object).

The full list of type symbol names is: `ERROR`, `NULL`, `INT8`, `INT16`, `INT32`, `INT64`, `FLOAT`, `DOUBLE`, `STRING`, `BLOB`, `CUSTOM`

Note that because v8's C++ API only supports integer values of 32 bits or less, JSPDO treats values greater than 32 bits as doubles. This _might_, depending on the db driver and schema-level settings, cause errors when trying to insert, e.g., a 33-bit value in a numeric db field declared as being 4 bytes (32 bits). JS itself does not really differentiate between integers and doubles - they're both classified as "numbers" in JS.

Note that drivers may report different types for the same column in different rows of the same result set. This is because:

  * This is how they report that a value is an SQL `NULL`.
  * sqlite3 allows any type to be stored in any column, regardless of the schema-declared types.

The MySQL driver, because it is "strongly typed", will always return the schema-declared type (or _something approximating it_!) _except_ in the case of SQL `NULL` values. It reports "is it NULL?" via a column type of `JSPDO.columnTypes.NULL`.

## Find out if a column is `NULL` ##

The simplest way:

```
var v = myStatement.get(zero_based_index);
if( null === v ) { ... it is an SQL NULL ... }
```

It can also be determined by looking at the driver-reported type of the column:

```
var t = myStatement.columnType(zero_based_index);
if( JSPDO.columnTypes.NULL === t ) {
   ... the value is SQL NULL ...
}
```