#if !defined(CODE_GOOGLE_COM_P_V8_V8_P3_SQLITE3_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_V8_P3_SQLITE3_H_INCLUDED 1
/**
   Implementation code for v8 (JavaScript engine) bindings for sqlite3.

   v8: http://code.google.com/p/v8/

   sqlite3: http://sqlite.org

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: same as v8 (see below)
*/
// Copyright 2007-2008 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

struct sqlite3;
namespace v8 {
/**
   The juice namespace is for "3rd Party" add-ons written for v8.
*/
namespace juice {
/**
   The v8::juice::sq3 namespace encapsulates sqlite3 bindings for
   v8. Call v8::juice::sq3::SetupAddon() to install the sqlite3 API into
   an existing JS object (normally the global object). Functions in
   the native API marked with the V8_BINDING macro represent
   JS-side functions made available by this addon (and no native
   implementations exist using those names, so don't try to call them
   from native code).

*/
namespace sq3 {

    enum { sq3_library_version = 0x20090303 };

    /**
       Installs JS bindings for a subset of the sqlite3 C API into the
       given object. On success it returns some arbitrary value which
       "should" evaluate to true. On error it returns a false value.

       The bindings very closely mimic the sqlite3 C API, with only
       very minor differences. The complete list of bound functions
       and their docs can be found in this header file - look for the
       functions marked with the V8_BINDING macro. Undocumented
       functions work identically to the native counterparts and are
       documented at http://sqlite.org.

       In addition to the sqlite3 API functions, this add-on also
       binds the sqlite3-standard SQLITE_xxx macros defined in
       sqlite3.h (as they are often used for error handling in sqlite3
       client code). They can be used in JS code just as in C++, for example
       (JS code):

       \code
       var db = sqlite3_open(":memory:");
       var rc = sqlite3_exec( db, "create table t(a,b,c)");
       if( SQLITE_OK != rc ) { ... }
       \endcode

       Also note that these bindings expect the same cleanup
       requirements as the native API - they do not rely on JS garbage
       collection to clean up the native-side objects. For example,
       the db handle created by sqlite3_open() must eventually be
       passed to sqlite3_close(), or the library will leak the
       resources associated with that handle.


       Notably missing sqlite3 features:

       - Custom collators written in JS. There are no plans to
       implement this.

       - sqlite3 authorizer functions aren't implemented. Again, there
       are no immediate plans to implement these.

    */
    ::v8::Handle< ::v8::Value > SetupAddon( ::v8::Handle< ::v8::Object > global );

    /**
       Once a database has been opened from JS script using the
       sqlite3_open() function, a native handle can be fetched by
       passing the result of the script-side JS sqlite3_open() call to
       this function.

       BUT BEWARE:

       The returned handle is owned by the JS script which created it
       and it MUST NOT be closed from native code (which would leave
       the JS object with a dangling handle). The pointed-to object is
       valid until sqlite3_close() is called from JS code on the
       object.

       This function is provided so that clients who need features not
       provided by the JS API (or which are otherwise inconvenient to
       use from native code) can use the complete sqlite3 API on the
       handle.  Examples including adding a custom authorizer function
       or custom collating sequences, neither of which are supported
       by the bound API.
    */
    sqlite3 * GetNativeDBHandle( Handle<Value> const & h );

    /** \def V8_BINDING

       Functions declared with V8_BINDING are not really
       implemented - this is a marker to show which sqlite3 functions
       have JS bindings. Any undocumented entries work as described in
       the sqlite3 C API (see http://sqlite.org). Documented entries
       normally have slightly different semantics than in the native
       API, due to differences in argument handling in C and JS. Every
       effort has been made to make the JS API as close to the
       native as possible.

       Most of the functions handle errors the same as their C
       counterparts, but some throw exceptions. The guideline is this:
       Errors reported by the sqlite3 API will always be reported back
       using the sqlite3 error code. For JS-side errors, for example
       incompatible argument count or types, the functions may throw a
       JS-side exception or translate the error to some sqlite3 error
       code (most will throw, however).
    */
#define V8_BINDING

    /**
       Usage:

       var dbh = sqlite3_open( filename );

       Returns a database handle on success and null on error. It is an opaque
       type and cannot be used or inspected directly by client code.

       The client MUST call sqlite3_close(dbh) to clean up the handle when he is
       done using it, or a resource leak is certain.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_open( ::v8::Arguments const & );

    /**
       Closes a database handle and frees its resources.

       JS usage:

       var rc = sqlite3_close( dbh );
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_close( ::v8::Arguments const & );

    /**
       Compiles an an SQLstatement.

       Usage:

       var sth = sqlite3_prepare( dbh, sql );

       On success it returns an sqlite3_stmt handle (on opaque type
       which must not be modified by the caller). That handle is
       used as an argument to various functions, like sqlite3_step(),
       sqlite3_reset(), and sqlite3_bind().

       This uses the native sqlite3_prepare_v2(), if available, which
       is syntactically compatible with sqlite3_compare() but is ever
       so slightly different.

       The returned handle, if not null/undefined, MUST eventually be passed
       to sqlite3_finalize() to free its resources.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_prepare( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_reset( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_clear_bindings( ::v8::Arguments const & );
    /**
       Usage:

       var rc = sqlite3_finalize( statement_handle );
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_finalize( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_step( ::v8::Arguments const & );
    /**
       Gets the last error message for a database.

       JS Usage:

       var string = sqlite3_errmsg(dbh);

       Reminder: sqlite3 returns the literal "not an error" for
       non-errors, instead of returning null or an empty
       string. Strange and annoying, yet unfortunately true.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_errmsg( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_bind_null( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_bind_int( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_bind_double( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_bind_int64( ::v8::Arguments const & );

    /**
       js_eval() is an SQL function which gets installed into any DB
       opened via this addon's API. It executes arbitrary JS code and
       returns the final result to sqlite3 as a string. It is used like
       this (SQL code):

       \code
       select js_eval('some_function()');
       \endcode

       The JS code is evaluated in the context of the object from which
       sqlite3_open() was called (normally the global object).

       js_eval() takes any number of arguments, all of which must be
       convertible to a string for evaluation.

       The result of the final eval is converted to a string and given
       back to sqlite3. If a statement in the eval throws an exception
       then the exception's toString text is used as the error string
       (passed to sqlite3_result_error()) and no further arguments to
       that call of js_eval() are processed. i would prefer to pass
       the exception back to the caller of js_eval(), but i haven't
       found a way to do it. The problem is that the actual
       implementation is called from sqlite3's engine whenever
       js_eval() is called from sql code, at which point we haven't
       got a direct line back to the caller (and it might have been
       called from outside of the JS engine).
    */
    V8_BINDING ::v8::Handle< ::v8::Value > js_eval( ::v8::Arguments const & );

    /**
       Usage:

       var rc = sqlite3_bind_text( statement, index, string [, length=string.length] );

       A length argument of -1 means to use string.length as the maximum input
       length.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_bind_text( ::v8::Arguments const & );

    /**
       Not sqlite3-standard. Usage:

       var rc = sqlite3_bind( statement, index, INT | DOUBLE | STRING | NULL | VOID );

       It calls one of sqlite3_bind_{int,int64,double,text,null}(), depending on
       the type of the 3rd argument.

       When passing column indexes, remember that bind indexes start
       at 1, not 0!

       Returns the result of an underlying sqlite3_bind_xxx()
       call, so it returns SQLITE_OK on success and something else
       on error.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_bind( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_bind_parameter_index( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_bind_parameter_name( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_bind_parameter_count( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_column_bytes( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_column_int( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_column_int64( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_column_double( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_column_type( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_column_name( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_column_decltype( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_column_count( ::v8::Arguments const & );
    /**
       Usage:
       var str = sqlite3_column_text( sth, index );

       void is returned on error.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_column_text( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_busy_timeout( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_changes( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_get_autocommit( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_libversion( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_errcode( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_expired( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_last_insert_rowid( ::v8::Arguments const & );

    /**
       Determines whether the given string is a complete (parseable) SQL
       statement.

       Usage:

       var rc = sqlite3_complete(sql);

       Retrurn true on false on success or failure and returns void if
       sql is not convertible to a string.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_complete( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_data_count( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_interrupt( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_extended_result_codes( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_sleep( ::v8::Arguments const & );
    /**
       Non-standard. Returns true if argv[0] is one of SQLITE_OK,
       SQLITE_ROW, or SQLITE_DONE, else false. (Each of those
       values means "success" in different contexts, and we
       sometimes don't care which of them we get back.)
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_is_okay( ::v8::Arguments const & );
    /**
       Non-standard. Runs a query which is expected to return a single
       integer value.

       JS usage:

       var myInt = sqlite3_select_int( dbhandle, sql );
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_select_int( ::v8::Arguments const & );
    /**
       Same as sqlite3_select_int() except that it returns the
       value as a double.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_select_double( ::v8::Arguments const & );
    /**
       Same as sqlite3_select_int() except that it returns the
       value as an int64.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_select_int64( ::v8::Arguments const & );
    /**
       Same as sqlite3_select_int() except that it returns the
       value as a string.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_select_text( ::v8::Arguments const & );

    /**
       This works just like the native sqlite3_create_function()
       routine and has the following JS signature:

       \code
       int sqlite3_create_function(
          opaque dbh, // DB handle
          string funcName, // Function name (currently only UTF8)
          int argCoung, // expected argument count, or -1 for any number
          int encoding, // currently only supports the value SQLITE_UTF8
          mixed userData, // arbitrary data to pass to the callback(s)
          Function callback, // signature: f( sqlite3_context,int,Array-of-sqlite3_value )
          OPTIONAL Function callbackStep = null, // signature: f( sqlite3_context,int,Array-of-sqlite3_value )
          OPTIONAL Function callbackFinal = null // signature: f( sqlite3_context )
          );
       \endcode

       The semantics of the various callback functions are as
       described in the native API. If a callback throws a JS
       exception then it is caught by the internal callback handler,
       converted to a string, and sqlite3_result_error() is passed
       that string value. (We apparently have no way to get the
       exception back to the caller of the SQL code from this point in
       the call chain.)

       Limitations:

       - The native sqlite3 API allows one to register the same
       function name multiple times with different argument
       counts. This implementation doesn't - registering the same name
       multiple times may have unpredictable effects. (FIXME?)
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_create_function( ::v8::Arguments const & );

    /**
       Executes arbitrary SQL code, optionally sending results to a callback
       function.

       Usage:

       var rc = sqlite3_exec( dbh, sql [, callbackFunction [, callbackData]] );

       For each row in the result set, callbackFunction is passed a row of data.
       The callback must have this signature:

       int function( mixed callbackData, Array rowArray, Array colArray );

       The colArray holds the column names, rowArray holds the values. callbackData
       is the same as the 4th argument passed to sqlite3_exec(), which defaults to
       undefined.

       Exceptions thrown in the callback are caught by the internal
       handler and then propagated back to the caller. The callback
       should return SQLITE_OK to signal success or some other value
       on error (in which case the sqlite3_exec() aborts the query),
       but a callback may optionally throw to signal an error (which
       gets reported to sqlite3 as SQLITE_ERROR).
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_exec( ::v8::Arguments const & );


    /**
       Non-standard. Escapes a string argument using SQL escaping
       (doubles all "'" characters).

       Try not to use this for creating SQL dynamically - that is subject
       to injection attacks and whatnot. Instead, prefer the
       sqlite3_bind() API, which is not subject to such mallice.

       @see sqlite3_quote_sql()
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_escape_sql( ::v8::Arguments const & );

    /**
       Like sqlite3_escape_sql(), but also surrounds the resulting
       string in quotes.

       @see sqlite3_escape_sql()
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_quote_sql( ::v8::Arguments const & );

    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_value_int( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_value_int64( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_value_double( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_value_text( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_value_type( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_result_int( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_result_int64( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_result_double( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_result_null( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_result_value( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_result_text( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_result_error( ::v8::Arguments const & );

    /**
       Similar to sqlite3_get_table(), but it returns the results in a different form.

       JS Usage:

       Mixed sqlite3_select_array( db, String sql )
       
       On error, an Integer value is returned (the same as the underlying call to
       sqlite3_get_table()).

       On success, an Object is returned with this structure:

       {cols:[column names], rows[row data]}

       The result will have cols.length columns and rows.length
       rows. Each entry in the rows array is an Array with cols.length
       entries.

       For example:

       \code
       var rc = sqlite3_select_array( mydb, "select a,b,c from t" );
       \endcode

        The resulting object would, on success, be structured like:

        \code
        {cols:['a','b','c'],
        rows:[
          [ ... values for row 1 ... ],
          [ ... ]
          [ ... values for row N ... ],
        ]}
        \endcode


        Here is an example suitable for copy/paste:

        \code
    var ar = sqlite3_select_array( my.db, "select * from t order by rowid desc limit 7" );
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
    }
        \endcode
    */
    V8_BINDING ::v8::Handle< ::v8::Value > sqlite3_select_array( ::v8::Arguments const & );

#undef V8_BINDING

}}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_V8_P3_SQLITE3_H_INCLUDED */
