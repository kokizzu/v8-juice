#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

/**
   Implementation code for v8 (JavaScript engine) bindings for sqlite3.

   v8: http://code.google.com/p/v8/

   sqlite3: http://sqlite.org

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: same as v8 (see below)

   Pedantic license note: much of this code was ported from the
   SpiderApe JS/sqlite3 bindings (http://SpiderApe.sf.net). Though
   that code is GPL, i wrote it, and am re-licensing this copy to
   conform to v8 conventions.
*/
// Copyright 2007-2009 the V8 project authors. All rights reserved.
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

#include <vector>

#include <iostream> /* only for debuggering */
#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

#include <sqlite3.h>

/**
   sqlite3 ver 3.3.9 introduced two new variants of
   sqlite3_prepare() which are call-compatible with the old
   ones but have different names and are supposedly improved.
   We'll macro those in place here:
*/
#if SQLITE_VERSION_NUMBER >= 3003009
	#define SQLITE3_PREPARE ::sqlite3_prepare_v2
	#define SQLITE3_PREPARE16 ::sqlite3_prepare16_v2
#else
	#define SQLITE3_PREPARE ::sqlite3_prepare
	#define SQLITE3_PREPARE16 ::sqlite3_prepare16
#endif

#include <sstream>

#include <v8.h>
#include <v8/juice/convert.h>
#include <v8/juice/plugin.h>

/**
   Convenience macro for marking wrapper functions.
*/
#define JS_WRAPPER(FN) static ::v8::Handle< ::v8::Value > FN( const ::v8::Arguments & argv )

namespace v8 {
    namespace convert {
	/**
	   Custom JS/C++ cast operators...
	 */
	namespace Detail
	{
	    template <>
	    struct to_js_f<sqlite3_int64>
	    {
		ValueHandle operator()( sqlite3_int64 v ) const
		{
		    return Number::New( v );
		}
	    };

	    template <>
	    struct to_native_f<sqlite3_int64>
	    {
		typedef sqlite3_int64 result_type;
		result_type operator()( ::v8::bind::BindKeyType cx, ValueHandle const & h ) const
		{
		    return h->IsNumber()
			? h->IntegerValue()
			: 0;
		}
		result_type operator()( ValueHandle const & h ) const
		{
		    return this->operator()( 0, h );
		}
	    };
	}
    }

namespace juice {
namespace sq3 {

    using namespace ::v8;
    using namespace ::v8::bind;
    using namespace ::v8::convert;

    JS_WRAPPER(sq3_callback_noop)
    {
	return Undefined();
    }

    /**
       Info holder for JS-side functions created via
       sqlite3_create_function() and called via sqlite3_exec()
       callbacks.
    */
    struct CallbackInfo
    {
	Handle<Object> self; // "this" object for func member
	Persistent<Function> func_noop; // internal use
	Persistent<Function> func; // primary function object for sqlite3_xxx() callbacks
	Persistent<Function> func_step; // for use with sqlite3_create_function()
	Persistent<Function> func_final; // for use with sqlite3_create_function()
	Handle<Array> exec_cols; // cached column names for use with sqlite3_exec() callback
	bool cols_cached; // set to true when exec_cols is populated.
	sqlite3 * dbh; // database handle
	Handle<Value> data; // arbitrary user data, used by sqlite3_xxx() callbacks.
	Handle<Value> return_val; // holds the return value for this->func calls.
	std::string func_name; // name of the bound function (if any)
	Handle<Value> cb_exception; // when a callback throws a JS exception, we stuff it here.
	int bogo; // for internal testing

#define FUNCCALL_INIT(OB,FU) \
	self(OB),							\
	    func_noop(Persistent<Function>::New( FunctionTemplate::New(sq3_callback_noop)->GetFunction() )), \
	    func(FU),							\
	    func_step(func_noop),					\
	    func_final(func_noop),					\
	    exec_cols(),						\
	    cols_cached(false),						\
	    dbh(0),							\
	    data(),							\
	    return_val(),						\
	    func_name(),						\
	    cb_exception(),						\
	    bogo(3)


	CallbackInfo() : FUNCCALL_INIT(,func_noop)
	{
	}

	CallbackInfo(Handle<Object> self, Persistent<Function> func)
	    : FUNCCALL_INIT(self,func)
	{
	}

#undef FUNCCALL_INIT

	~CallbackInfo()
	{
	    if(0) CERR << "~CallbackInfo()"
		       << " this->func.IsEmpty() ?= " << this->func.IsEmpty()
		       << " this->func->IsFunction() ?= " << this->func->IsFunction()
		       << " this->func = " << CastFromJS<std::string>( this->func )
		       <<'\n';
	    if(0)
	    { // causes crashes :(
		this->func.Dispose();
		this->func_step.Dispose();
		this->func_final.Dispose();
		this->func_noop.Dispose();
	    }
	}

    };

    /**
       Internal type for binding sqlite3-related info to JS. It
       encapsulates a single db opened via sqlite3_open().
    */
    class DBInfo
    {
    public:
	/**
	   Database handle. This type owns handles associated
	   with it.
	*/
	sqlite3 * dbh;
	/**
	   The object this handle is associated with (normally
	   the global object).
	*/
	Handle<Object> jsobj;
	//! encoding: SQLITE_UTF8 or SQLITE_UTF16
	int encoding;
	/**
	   For use by sqlite3_create_function() and friends.
	*/
	typedef std::map<std::string,CallbackInfo> FuncMapType;
	FuncMapType userFuncs;

	typedef std::map<sqlite3_stmt const *,DBInfo *> StmtMap;
	static StmtMap stmt;

	/**
	   Not necessary - can be removed. We use v8::External to pass
	   DBInfo objects around, instead of a full-fledged Object.
	*/
	static Persistent<ObjectTemplate> js_prototype;
	/**
	   A context for use with BindNative() and friends.
	*/
	static const void * bind_context;
	
	/**
	   Creates a new
	*/
	DBInfo() : dbh(0),
		   jsobj(),
		   encoding(SQLITE_UTF8),
		   userFuncs()
	{
	    ::v8::bind::BindNative( DBInfo::bind_context, this, this );
	}
	/**
	   Unbinds this object from the JS/native bindings. Does not
	   (cannot?) tell JS to clean up its references, though.
	*/
	~DBInfo()
	{
	    ::v8::bind::UnbindNative( DBInfo::bind_context, this, this );
	    if( dbh )
	    {
		//CERR << "Closing sqlite3 handle @"<<dbh<<'\n';
		sqlite3_close( dbh );
		dbh = 0;
	    }
	}

	/**
	   Returns the DBInfo object (if any) bound to the given JS value.
	*/
	static DBInfo * GetNative( Handle<Value> const id )
	{
	    return ::v8::bind::GetBoundNative<DBInfo>( DBInfo::bind_context, id );
	}

	/**
	   Gets the sqlite3_stmt (if any) associated with given JS value.
	*/
	static sqlite3_stmt * GetStatement( Handle<Value> const id )
	{
	    return ::v8::bind::GetBoundNative<sqlite3_stmt>( DBInfo::bind_context, id );
	}

	/**
	   sqlite3_prepare() should call this to register the
	   statement it creates.
	*/
	void AddStatement( sqlite3_stmt * st )
	{
	    ::v8::bind::BindNative( DBInfo::bind_context, st, st );
	    DBInfo::stmt[st] = this;
	}

	/**
	   sqlite3_finalize() should call this to deregister its
	   statement. This routine does NOT call sqlite3_finalize()
	   on the statement.
	*/
	void RemoveStatement( sqlite3_stmt * st )
	{
	    DBInfo::stmt.erase(st);
	    ::v8::bind::UnbindNative( DBInfo::bind_context, st, st );
	    return;
	}

	/**
	   Returns the DBInfo (if any) from which st was created.
	*/
	static DBInfo * DBForStatement( sqlite3_stmt const * st )
	{
	    StmtMap::iterator it = DBInfo::stmt.find(st);
	    if( DBInfo::stmt.end() == it ) return 0;
	    return (*it).second;
	}

    };
    Persistent<ObjectTemplate> DBInfo::js_prototype;
    DBInfo::StmtMap DBInfo::stmt;
    void const * DBInfo::bind_context = &DBInfo::js_prototype;

    sqlite3 * GetNativeDBHandle( Handle<Value> const & h )
    {
	DBInfo * db = DBInfo::GetNative( h );
	return db ? db->dbh : 0;
    }

#define ASSERTARGS(FUNCNAME,COND) if(!(COND)) return ThrowException(String::New(# FUNCNAME "(): assertion failed: " # COND))

#define DB_ARG(HND) DBInfo * db = DBInfo::GetNative( HND );			\
	if( ! db ) return ThrowException(String::New("Argument is not a sqlite3 handle!"))

#define STMT_ARG(HND) sqlite3_stmt * stmt = DBInfo::GetStatement( HND )
#define STMT_ARG_OR(HND,RV) STMT_ARG(HND);	\
	if( ! stmt ) return RV
#define STMT_ARG_THROW(HND) STMT_ARG_OR(HND,ThrowException(String::New("Argument is not a sqlite3_stmt handle!")))

#define SQCX_ARG(HND) sqlite3_context * sqcx = GetBoundNative<sqlite3_context>( DBInfo::bind_context, HND )
#define SQCX_ARG_OR(HND,RV) SQCX_ARG(HND);	\
	if( ! sqcx ) return RV
#define SQCX_ARG_THROW(HND) SQCX_ARG_OR(HND,ThrowException(String::New("Argument is not a sqlite3_context handle!")))

#define SQVAL_ARG(HND) sqlite3_value * sqval = GetBoundNative<sqlite3_value>( DBInfo::bind_context, HND )
#define SQVAL_ARG_OR(HND,RV) SQVAL_ARG(HND);	\
	if( ! sqval ) return RV
#define SQVAL_ARG_THROW(HND) SQVAL_ARG_OR(HND,ThrowException(String::New("Argument is not a sqlite3_value handle!")))


    /**
       Performs SQL-style escaping, doubling all apostrophes,
       of the given string. Returns the number of escapes
       made.
    */
    static size_t sql_escape_string( std::string & ins )
    {
	std::string::size_type pos;
	const std::string to_esc("'");
	pos = ins.find_first_of( to_esc );
	if( std::string::npos == pos ) return 0;
	const std::string esc("'");
	size_t reps = 0;
	while( pos != std::string::npos )
	{
	    ins.insert( pos, esc );
	    ++reps;
	    pos = ins.find_first_of( to_esc, pos + esc.size() + 1 );
	}
	return reps;
    }

    /**
       Escapes argv[0] as SQL.

       JS Usage:

       string = sqlite3_escape_sql("a string");

       TODO?:

       Array sqlite3_escape_sql(Array)

    */
    JS_WRAPPER(sq3_escape_sql)
    {
	ASSERTARGS(sqlite3_escape_sql,(1==argv.Length()));
	std::string s( CastFromJS<std::string>( argv[0] ) );
	if( 0 == sql_escape_string( s ) )
	{
	    return argv[0];
	}
	else
	{
	    return CastToJS( s );
	}
    }

    JS_WRAPPER(sq3_quote_sql)
    {
	ASSERTARGS(sqlite3_quote_sql,(1==argv.Length()));
	std::string s( CastFromJS<std::string>( argv[0] ) );
	if( ! s.empty() )
	{
	    sql_escape_string( s );
	}
	s.insert(0,1,'\'');
	s.push_back('\'');
	return CastToJS( s );
    }


    /**
       Callback for sqlite3_create_function(). It evaluates each argument
       as a JavaScript string, in the context of the object associated
       with the context's DBInfo "parent" object.
    */
    static void sql_callback_js_eval(
				     sqlite3_context *context,
				     int argc,
				     sqlite3_value **argv )
    {
	if( 0 == argc ) return;
	// TODO: eval all args as JS code.
	DBInfo * db = static_cast< DBInfo * >( sqlite3_user_data( context ) );
	if( ! db )
	{
	    std::string str("Internal consistency error: sql_callback_js_eval(): (DBInfo *) is null.");
	    //CERR << str << '\n';
	    sqlite3_result_text( context, str.c_str(), static_cast<int>(str.size()), SQLITE_TRANSIENT );
	    return;
	}
	//HandleScope sentry;
	TryCatch tryer;
        Local<Value> rv;
	Local<Function> eval = Function::Cast( *(db->jsobj->Get(String::New("eval"))) );
    	for( int i = 0; i < argc; ++i )
	{
	    char const * cp = reinterpret_cast<char const *>( sqlite3_value_text( argv[i] ) );
	    if( ! cp || !*cp ) continue;
	    Local<Value> arg = String::New( cp, sqlite3_value_bytes( argv[i] ) );
	    rv = eval->Call( db->jsobj, 1, &arg );
	    if( rv.IsEmpty() )
	    {
		break;
	    }
	}
	//CERR << "evaled to ["<<str<<"]\n";
	if( !rv.IsEmpty() )
	{
	    std::string str( CastFromJS<std::string>(rv) );
	    sqlite3_result_text( context, str.c_str(), static_cast<int>(str.size()), SQLITE_TRANSIENT );
	}
	else
	{
	    std::string str( CastFromJS<std::string>( tryer.Exception() ) );
	    sqlite3_result_text( context, str.c_str(), static_cast<int>(str.size()), SQLITE_TRANSIENT );
	}
    }


    JS_WRAPPER(sq3_open)
    {
	ASSERTARGS(sqlite3_open,(argv.Length()==1));
	std::string name = CastFromJS<std::string>( argv[0] );
	if( name.empty() )
	{
	    return ThrowException( String::New("First argument must be a database file name!") );
	}
	sqlite3 * dbh = 0;
	int rc = sqlite3_open( name.c_str(), &dbh );
	if( SQLITE_OK != rc )
	{
	    std::ostringstream os;
	    os << "sqlite3_open(\""<<name<<"\") failed with error code "<<rc<<'!';
	    return ThrowException( String::New(os.str().c_str()) );
	}
	DBInfo * db = new DBInfo;
	db->dbh = dbh;
	db->jsobj = argv.Holder();

	if(1)
	{ // register js_eval() SQL func
	    rc = ::sqlite3_create_function( db->dbh,
					    "js_eval", // func name
					    -1, // arg count, or -1 for "any"
					    SQLITE_UTF8, // encoding
					    db, // arbitrary user data
					    sql_callback_js_eval,// void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
					    0,// void (*xStep)(sqlite3_context*,int,sqlite3_value**),
					    0 // void (*xFinal)(sqlite3_context*)
					    );
	    if( SQLITE_OK != rc )
	    {
		CERR << "Registration of js_eval() failed with rc "<<rc<<"!\n";
	    }
	}
	return External::New( db );
    }

    JS_WRAPPER(sq3_close)
    {
	ASSERTARGS(sqlite3_close,(argv.Length()==1));
	using namespace ::v8::bind;
	DB_ARG( argv[0] );
	delete db;
	return Handle<Value>();
    }

    JS_WRAPPER(sq3_prepare)
    {
	ASSERTARGS(sqlite3_prepare,(argv.Length()==2));
	using namespace ::v8::bind;
	DB_ARG( argv[0] );
	String::Utf8Value asc( argv[1] );
	char const * sql = *asc;
	if( ! sql || !*sql )
	{
	    return Null();
	}
	sqlite3_stmt * st = 0;
	char const * tail = 0;
	int rc = SQLITE3_PREPARE( db->dbh, sql, -1, &st, &tail );
	if( SQLITE_OK != rc )
	{
	    return Null();
	}
	db->AddStatement( st );
	return External::New( st );
    }


    JS_WRAPPER(sq3_reset)
    {
	ASSERTARGS(sqlite3_reset,(argv.Length()==1));
	STMT_ARG_OR(argv[0],Integer::New(SQLITE_ERROR));
	return Integer::New( sqlite3_reset( stmt ) );
    }


    JS_WRAPPER(sq3_clear_bindings)
    {
	ASSERTARGS(sqlite3_clear_bindings,(argv.Length()>=1));
	STMT_ARG_THROW(argv[0]);
	return CastToJS( ::sqlite3_clear_bindings( stmt ) );
    }

    JS_WRAPPER(sq3_finalize)
    {
	ASSERTARGS(sqlite3_finalize,(argv.Length()==1));
	STMT_ARG_OR(argv[0],Integer::New(SQLITE_ERROR));
	DBInfo * db = DBInfo::DBForStatement( stmt );
	db->RemoveStatement( stmt );
	return Integer::New( sqlite3_finalize( stmt ) );
    }

    JS_WRAPPER(sq3_step)
    {
	ASSERTARGS(sqlite3_step,(argv.Length()==1));
	STMT_ARG_OR(argv[0],Integer::New(SQLITE_ERROR));
	return Integer::New( sqlite3_step( stmt ) );
    }



    /**
       Internal callback callback for sqlite3_exec(dbh,sql,callback_func,data).
    */
    static int sq3_exec_callback(void *data, int argc, char **argv, char **azColName)
    {
	CallbackInfo * fi = static_cast<CallbackInfo *>(data);
	if( ! fi )
	{
	    // Internal consistency error...
	    CERR << "sq3c_exec_callback(): (CallbackInfo *) is null.\n";
	    return SQLITE_ERROR;
	}
	//HandleScope sentry;

	if( ! fi->cols_cached )
	{
	    //fi->exec_cols = Persistent<Array>::New( Array::New(argc) );
	    fi->cols_cached = true;
	    fi->exec_cols = Array::New(argc);
	    for( int i = 0; i < argc; ++i )
	    {
		fi->exec_cols->Set( Integer::New(i),
				    CastToJS<std::string>( azColName[i] ) );
	    }
	}

	Local<Array> rows = Array::New(argc);
	for( int i = 0; i < argc; ++i )
	{
	    rows->Set( Integer::New(i),
		       CastToJS<std::string>( argv[i] ) );
	}

	typedef std::vector< Handle<Value> > VT;
	VT vec( 3, Handle<Value>() );
	vec[0] = fi->data;
	vec[1] = rows;
	vec[2] = fi->exec_cols;
	TryCatch tryer;
	Local<Value> rv
	    = fi->func->Call( fi->self, 3, &vec[0] )
	    ;
	if( rv.IsEmpty() )
	{
	    fi->cb_exception = tryer.Exception();
	    return CastFromJS<int>( Integer::New(SQLITE_ERROR) );
	}
	return CastFromJS<int>( rv );
    }

    JS_WRAPPER(sq3_exec)
    { // sqlite3_exec( dbh, sql [, callback [, callback_data] ] )
	/** Callbacks look like:
	
	function callback( callback_data, rows, cols ){...}

	They are called one time for each row of data returned from
	the sql passed to sqlite3_exec().

	The callback args are:

	- callback_data is the same argument passed to sqlite3_exec()

	- rows is an Array of SQL result row values.

	- cols is an Array of SQL result column names. This object is
	cached between calls to the callback, so DO NOT modify it from
	the callback.

	Callbacks must return SQLITE_OK on success, or execution
	of the query will be halted.

	TODO: fix exception handling.
	*/
	const int argc = argv.Length();
	ASSERTARGS(sqlite3_exec,((argc>=2)&&(argc<5)));
	DB_ARG(argv[0]);
	const std::string Sql( CastFromJS<std::string>( argv[1] ) );
	char const * sql = Sql.c_str();
	if( 2 == argc )
	{ // exec( dbh, sql );
	    return CastToJS( ::sqlite3_exec( db->dbh, sql, 0, 0, 0 ) );
	}
	Handle<Value> fv = argv[2];
	if( ! fv->IsFunction() )
	{
	    return ThrowException(String::New("sqlite3_exec() 3rd argument is not a Function!"));
	}
	//HandleScope sentry;
	CallbackInfo fi( db->jsobj,
			 //Persistent<Function>( fv )
			 //Persistent<Function>( Function::Cast( *fv ) )
			 Function::Cast( *fv )
			 );
	fi.data = Undefined();
	if( argc > 3 )
	{
	    fi.data = argv[3];
	}
	//return ThrowException(String::New("sqlite3_exec() not yet implemented for >2 arguments"));
	int rc = ::sqlite3_exec( db->dbh, sql, sq3_exec_callback, &fi, 0 );
	if( ! fi.cb_exception.IsEmpty() )
	{
	    Handle<Value> ex = fi.cb_exception;
	    fi.cb_exception = Handle<Value>();
	    return ThrowException(ex);
	}
	return CastToJS( rc );
    }



    JS_WRAPPER(sq3_errmsg)
    {
	ASSERTARGS(sqlite3_errmsg,(argv.Length()==1));
	DB_ARG(argv[0]);
	return String::New( sqlite3_errmsg( db->dbh ) );
    }

    JS_WRAPPER(sq3_bind_null)
    {
	ASSERTARGS(sqlite3_bind_null,(argv.Length()==2));
	STMT_ARG_THROW(argv[0]);
	int ndx = ::v8::convert::CastFromJS<int>( argv[1] );
	if( 0 == ndx )
	{
	    return Integer::New( SQLITE_RANGE );
	}
	return Integer::New( sqlite3_bind_null( stmt, ndx ) );
    }

#define STMT_BIND3_IMPL(FN,Type)		      \
    JS_WRAPPER(sq3_bind_ ## FN)			      \
    {							   \
	/** SQFUNC( sth, int index, Type val ) **/			\
	ASSERTARGS(sqlite3_bind_ ## FN ,(argv.Length()==3));		\
	STMT_ARG_THROW(argv[0]);					\
	int index = ::v8::convert::CastFromJS<int>( argv[1] );		\
	Type val = ::v8::convert::CastFromJS<Type>( argv[2] );		\
	int rc = ::sqlite3_bind_ ## FN( stmt, index, val );		\
	return Integer::New(rc);					\
	}
    STMT_BIND3_IMPL(double,double);
    STMT_BIND3_IMPL(int,int);
    STMT_BIND3_IMPL(int64,sqlite3_int64);
#undef STMT_BIND3_IMPL

    JS_WRAPPER(sq3_bind_text)
    {
	/** sqlite3_bind_text( sth, int index, text [, length of text = -1]  ) **/
	const int argc = argv.Length();
	ASSERTARGS(sqlite3_bind_text,((argc==3) || (argc==4)));
	STMT_ARG_THROW(argv[0]);
	using namespace ::v8::convert;
	int ndx = CastFromJS<int>( argv[1] );
	if( 0 == ndx )
	{
	    return Integer::New( SQLITE_RANGE );
	}
	String::Utf8Value asc( argv[2] );
	int slen = (argc>3) ? CastFromJS<int>(argv[3]) : -1;
	return Integer::New( sqlite3_bind_text( stmt, ndx, *asc, slen, SQLITE_TRANSIENT ) );
    }


    JS_WRAPPER(sq3_bind)
    {
	/** sqlite3_bind( sth, int index [, mixed=void]  ) **/
	const int argc = argv.Length();
	ASSERTARGS(sqlite3_bind_text,((argc==2) || (argc==3)));
	STMT_ARG_THROW(argv[0]);
	using namespace ::v8::convert;
	int ndx = CastFromJS<int>( argv[1] );
	if( 0 == ndx )
	{
	    return Integer::New( SQLITE_RANGE );
	}
	Handle<Value> arg = argv[2];
	if( (2 == argc) || arg.IsEmpty() || arg->IsNull() || arg->IsUndefined() )
	{
	    return CastToJS( sqlite3_bind_null( stmt, ndx ) );
	}
	if( arg->IsString() )
	{
	    String::Utf8Value asc(arg);
	    return CastToJS( sqlite3_bind_text( stmt, ndx, *asc ? *asc : "", -1, SQLITE_TRANSIENT ) );
	}
	if( arg->IsInt32() )
	{
	    return CastToJS( sqlite3_bind_int( stmt, ndx, arg->Int32Value() ) );
	}
	if( arg->IsNumber() )
	{
	    return CastToJS( sqlite3_bind_double( stmt, ndx, arg->NumberValue() ) );
	}
	return ThrowException( String::New("sqlite3_bind(): 3rd argument must be of type Integer, Double, String, Null, or undefined (void)!") );
    }

    JS_WRAPPER(sq3_bind_parameter_count)
    {
	const int argc = argv.Length();
	ASSERTARGS(sqlite3_bind_parameter_name,(argc==1));
	STMT_ARG_THROW(argv[0]);
	return ::v8::convert::CastToJS( sqlite3_bind_parameter_count( stmt ) );
    }

    JS_WRAPPER(sq3_bind_parameter_index)
    {
	const int argc = argv.Length();
	ASSERTARGS(sqlite3_bind_parameter_index,(argc==2));
	STMT_ARG_THROW(argv[0]);
	String::Utf8Value asc( argv[1] );
	return ::v8::convert::CastToJS<int>( sqlite3_bind_parameter_index( stmt, *asc ) );
    }

    JS_WRAPPER(sq3_bind_parameter_name)
    {
	const int argc = argv.Length();
	ASSERTARGS(sqlite3_bind_parameter_name,(argc==2));
	STMT_ARG_THROW(argv[0]);
	int ndx = argv[1]->Int32Value();
	char const * str = sqlite3_bind_parameter_name( stmt, ndx );
	return str
	    ? String::New(str)
	    : Null();
    }

#define SQ3_COLUMN_FUNC_IMPL(FN,TYPE)				\
    JS_WRAPPER(sq3_ ## FN)					\
    {									\
	ASSERTARGS(sqlite3 ## FN,(2==argv.Length()));			\
	STMT_ARG_THROW(argv[0]);					\
	int index = ::v8::convert::CastFromJS<int>( argv[1] );		\
	return ::v8::convert::CastToJS( ::sqlite3_ ## FN( stmt, index ) ); \
    }

    SQ3_COLUMN_FUNC_IMPL(column_bytes,int);
    SQ3_COLUMN_FUNC_IMPL(column_int,int);
    SQ3_COLUMN_FUNC_IMPL(column_int64,sqlite3_int64);
    SQ3_COLUMN_FUNC_IMPL(column_double,double);
    SQ3_COLUMN_FUNC_IMPL(column_type,int);
    SQ3_COLUMN_FUNC_IMPL(column_name,char const *);
    SQ3_COLUMN_FUNC_IMPL(column_decltype,char const *);
#undef SQ3_COLUMN_FUNC_IMPL


    JS_WRAPPER(sq3_column_count)
    {
	ASSERTARGS(sqlite3_column_count,(1==argv.Length()));
	STMT_ARG_THROW(argv[0]);
	return CastToJS( ::sqlite3_column_count( stmt ) );
    }
    
    JS_WRAPPER(sq3_column_text)
    {
	ASSERTARGS(sqlite3_column_text,(2==argv.Length()));
	STMT_ARG_THROW(argv[0]);
	int index = CastFromJS<int>( argv[1] );
	const unsigned char * rc = ::sqlite3_column_text( stmt, index );
	return CastToJS( reinterpret_cast<char const *>( rc ) );
    }

    JS_WRAPPER(sq3_busy_timeout)
    {
	ASSERTARGS(sqlite3_busy_timeout,(2==argv.Length()));
	DB_ARG(argv[0]);
	int ms = CastFromJS<int>( argv[1] );
	return CastToJS( ::sqlite3_busy_timeout( db->dbh, ms ) );
    }
    
    JS_WRAPPER(sq3_changes)
    {
	ASSERTARGS(sqlite3_changes,(argv.Length()>=1));
	DB_ARG( argv[0] );
	return CastToJS( ::sqlite3_changes( db->dbh ) );
    }

    JS_WRAPPER(sq3_get_autocommit)
    {
	ASSERTARGS(sqlite3_get_autocommit,(argv.Length()>=1));
	DB_ARG(argv[0]);
	return CastToJS( ::sqlite3_get_autocommit( db->dbh ) );
    }
    
    JS_WRAPPER(sq3_libversion)
    {
	//ASSERTARGS(sqlite3_libversion,(0==argv.Length()));
	return CastToJS( ::sqlite3_libversion() );
    }

    JS_WRAPPER(sq3_errcode)
    {
	ASSERTARGS(sqlite3_errcode,(argv.Length()>=1));
	DB_ARG(argv[0]);
	return CastToJS( ::sqlite3_errcode( db->dbh ) );
    }


    JS_WRAPPER(sq3_expired)
    {
	ASSERTARGS(sqlite3_expired,(1==argv.Length()));
	STMT_ARG_THROW(argv[0]);
	return CastToJS( ::sqlite3_expired( stmt ) );
    }
    
    JS_WRAPPER(sq3_last_insert_rowid)
    {
	ASSERTARGS(sqlite3_last_insert_rowid,(1==argv.Length()));
	DB_ARG( argv[0] );
	return CastToJS( ::sqlite3_last_insert_rowid( db->dbh ) );
    }


    JS_WRAPPER(sq3_complete)
    {
	ASSERTARGS(sqlite3_complete,(1==argv.Length()));
	std::string sql = CastFromJS<std::string>( argv[0] );
	Handle<Value> rv = Undefined();
	if( ! sql.empty() )
	{
	    rv = CastToJS( ::sqlite3_complete( sql.c_str() ) );
	}
	return rv;
    }


    JS_WRAPPER(sq3_interrupt)
    {
	ASSERTARGS(sqlite3_interrupt,(1==argv.Length()));
	DB_ARG(argv[0]);
	::sqlite3_interrupt( db->dbh );
	return Undefined();
    }
    
    JS_WRAPPER(sq3_data_count)
    {
	ASSERTARGS(sqlite3_data_count,(1==argv.Length()));
	STMT_ARG_THROW(argv[0]);
	return CastToJS( ::sqlite3_data_count( stmt ) );
    }

    JS_WRAPPER(sq3_extended_result_codes)
    {
	ASSERTARGS(sqlite3_extended_result_codes,(2==argv.Length()));
	DB_ARG(argv[0]);
	int aBool = argv[1]->BooleanValue();
	return CastToJS( ::sqlite3_extended_result_codes( db->dbh, aBool ) );
    }

    JS_WRAPPER(sq3_sleep)
    {
	ASSERTARGS(sqlite3_sleep,(1==argv.Length()));
	int ms = CastFromJS<int>( argv[0] );
	return CastToJS( ::sqlite3_sleep( ms ) );
    }

    /**
       JS Usage:

       if( sqlite3_is_okay( resultCode ) ) ...

       Returns true if resultCode is one of SQLITE_OK, SQLITE_DONE,
       or SQLITE_ROW (which are all "okay" in certain contexts).
    */
    JS_WRAPPER(sq3_is_okay)
    {
	ASSERTARGS(sqlite3_is_okay,(1==argv.Length()));
	int x = CastFromJS<int>( argv[0] );
	return Boolean::New( (x == SQLITE_OK) || (x == SQLITE_DONE) || (x == SQLITE_ROW) );
    }

    static bool rc_is_okay( int x )
    {
	return (x == SQLITE_OK) || (x == SQLITE_DONE) || (x == SQLITE_ROW);
    }

    /**
       Creates a wrapper for sqlite3_exec()/sqlite3_column_TYPE().
       Valid TYPE values: int, double, text, int64
     */
#define SELECT_T(TYPE)				\
    JS_WRAPPER(sq3_select_ ## TYPE)			\
    {							\
	ASSERTARGS(sqlite3_select_ ## TYPE,(2==argv.Length()));	\
	DB_ARG(argv[0]);						\
	const std::string Sql = CastFromJS<std::string>( argv[1] );	\
	char const * sql = Sql.c_str();					\
	if( ! sql )							\
	{								\
	    return ThrowException( String::New( "sqlite3_select_" # TYPE "(): conversion of second arg to SQL string failed." ) ); \
	}								\
	sqlite3_stmt * sth = 0;						\
	char const * tail = 0;						\
	int rc = sqlite3_prepare( db->dbh, sql, -1, &sth, &tail );	\
	if( SQLITE_OK != rc )						\
	{								\
	    std::ostringstream os;					\
	    os << "sqlite3_select_" << # TYPE << ": sqlite3_prepare(dbh,[SQL="<<sql<<"]) failed with code "<<rc<<'!'; \
	    return ThrowException( String::New(os.str().c_str()) );	\
	}								\
	rc = sqlite3_step( sth );					\
	Handle<Value> rv = Undefined();					\
	if( rc_is_okay( rc ) )						\
	{								\
	    rv = CastToJS( sqlite3_column_ ## TYPE( sth, 0 ) );		\
	}								\
	sqlite3_finalize( sth );					\
	return rv;							\
    }

    SELECT_T(int)
    SELECT_T(double)
    SELECT_T(int64)
#undef SELECT_T

    JS_WRAPPER(sq3_select_text)
    {
	ASSERTARGS(sqlite3_select_text,(2==argv.Length()));
	DB_ARG(argv[0]);
	const std::string Sql = CastFromJS<std::string>( argv[1] );
	char const * sql = Sql.c_str();
	if( ! sql )
	{
	    return ThrowException( String::New( "sqlite3_select_text(): conversion of second arg to SQL string failed." ) );
	}
	sqlite3_stmt * sth = 0;
	char const * tail = 0;
	int rc = sqlite3_prepare( db->dbh, sql, -1, &sth, &tail );
	if( SQLITE_OK != rc )
	{
	    std::ostringstream os;
	    os << "sqlite3_select_text(): sqlite3_prepare(dbh,[SQL="<<sql<<"]) failed with code "<<rc<<'!';
	    return ThrowException( String::New(os.str().c_str()) );
	}
	rc = sqlite3_step( sth );
	Handle<Value> rv = Undefined();
	if( rc_is_okay( rc ) )
	{
	    rv = CastToJS( reinterpret_cast<char const *>(sqlite3_column_text( sth, 0 )) );
	}
	sqlite3_finalize( sth );
	return rv;
    }


    /**
       Internal helper to forward SQL functions to JS-side
       implementations.
       
       This is the base implementation for the
       sqlite_func*_forwarder() functions.
       
       funcid must be one of:
       
       0 == xFunc, the "base" function.
       
       1 == xStep, the "step" function.
       
       2 == xFinal, the "final" function.
       
       This function is embarassingly long and has to do a
       significant amount of bookkeeping to pass on data from
       sqlite3 to JS-space.
    */
    static void sqlite_func_forwarder_impl(
					   const int funcid, // 0 == xFunc, 1 == xStep, 2 == xFinal
					   sqlite3_context *context,
					   const int argc,
					   ::sqlite3_value **argv )
    {
	CallbackInfo * fi = 0;
	fi = static_cast<CallbackInfo *>( sqlite3_user_data( context ) );
	if( ! fi )
	{
	    return;// ThrowException( String::New("sqlite_func_forwarder_impl() internal error: called without bound native data.") );
	}
	fi->return_val = Undefined();
	if(0)
	{
	    CERR << "function type id: " << funcid << ", fi=@"<<fi<<'\n';
	    CERR << "fi->bogo: " << fi->bogo << '\n';
	    CERR << "fi->func_name: " << fi->func_name
		 << ", fi->func->IsFunction()="<<fi->func->IsFunction()
		// << ", fi->func toString="<<CastFromJS<std::string>(fi->func)
		 <<'\n';
	}
	Handle<Function> thefunc;
	if( 0 == funcid ) thefunc = fi->func;
	else if( 1 == funcid ) thefunc = fi->func_step;
	else if( 2 == funcid ) thefunc = fi->func_final;
	else
	{
	    if(1)
	    {
		std::ostringstream os;
		os << "sqlite3_func_forwarder_impl("<<funcid<<",context,"<<argc<<",array) was passed a bad 1st argument.";
		fi->cb_exception = Exception::Error( String::New(os.str().c_str()) );
		CERR << os.str() << '\n';
		//return ThrowException(String::New(os.str().c_str()));
	    }
	    return;
	}
	if( !thefunc->IsFunction() )
	{
	    if(1)
	    {
		std::ostringstream os;
		os << "sqlite3_func_forwarder_impl("<<funcid<<",context,"<<argc<<",array) could not determine which function to run!"
		     << " func="<<CastFromJS<std::string>(thefunc)<<'\n';
		fi->cb_exception = Exception::Error( String::New(os.str().c_str()) );
		CERR << os.str() << '\n';
		//return ThrowException(String::New(os.str().c_str()));
	    }
	    return;
	}
	//HandleScope sentry;
	/*****
	      We need to bind the sqlite3_context and each
	      sqlite3_value to JS space only for the duration
	      of this function. Unfortunately, we can't copy
	      scoped_binder objects, yet we need them to live
	      in an STL container. Thus binding the
	      sqlite3_values is significantly more work than
	      the sqlite3_context objects
	*/
	//jsval sqcxjv = ape::bind::get_next_resource_id<sqlite3_context>( fi->context );
	//ape::bind::scoped_binder<sqlite3_context> scx_sentry( fi->context, sqcxjv, *context );
	Handle<External> sqcxjv = External::New( context );
	ScopedBinder<sqlite3_context> scx_sentry( DBInfo::bind_context, context, context );

	typedef Handle<Value> HV;
	typedef std::vector< HV > VecT;
	const int vlen = argc;
	VecT valvec(vlen, HV());
	for( int i = 0; i < vlen; ++i )
	{
	    sqlite3_value * val = argv[i];
	    valvec[i] = External::New( val );
	    BindNative( DBInfo::bind_context, val, val );
	}
	//#define CLEANUP for( int i = 0; i < argc; ++i ) ape::bind::unbind_native< ::sqlite3_value >( fi->context, valvec[i], argv[i] );
	//#define CLEANUP for( int i = 0; i < argc; ++i ) unbind_impl( fi->context, argv[i] );

	Local<Array> array;
	if( 2 != funcid )
	{
	    if( vlen )
	    {
		//CERR << "Going to create an array of "<<vlen<<" items. Vector has space for "<<valvec.size()<<'\n';
		array = Array::New( vlen );
		for( int i = 0; i < vlen; ++i )
		{
		    //bool rc =
		    array->Set( Integer::New( static_cast<int>(i) ), valvec[i] );
		    //CERR << "argv["<<i<<"] == "<<CastFromJS<std::string>(valvec[i])<<'\n';
		}
	    }
	}
	else
	{
	    array = Array::New(0);
	}
	const int realArgC = ( (2 == funcid) ? 2 : 3 );
	VecT realargs(realArgC,Null());
	int argpos = 0;
	realargs[argpos++] = sqcxjv;
	realargs[argpos++] = fi->data;
	if( funcid != 2 )
	{
	    realargs[argpos++] = array;
	}
	//CERR << "Calling JS-side "<<fi->func_name<<"() with "<<realArgC<<" arguments...\n";
	assert( ! thefunc.IsEmpty() && thefunc->IsFunction() );
	TryCatch tryer;
	try
	{
	    fi->return_val = thefunc->Call( fi->self, realArgC, &realargs[0] );
	}
	catch(...)
	{}
	for( int i = 0; i < argc; ++i )
	{
	    sqlite3_value * val = argv[i];
	    UnbindNative<sqlite3_value>( DBInfo::bind_context, val, val );
	}
	if( fi->return_val.IsEmpty() )
	{
	    fi->cb_exception = tryer.Exception();
	    std::string msg( CastFromJS<std::string>( fi->cb_exception ) );
	    sqlite3_result_error( context, msg.c_str(), static_cast<int>( msg.size() ) );
	}
    }

    /**
       Internal helper to forward functions to JS-side implementations.
    */
    static void sqlite_func_forwarder(
				      sqlite3_context *context,
				      int argc,
				      sqlite3_value **argv )
    {
	sqlite_func_forwarder_impl( 0, context, argc, argv );
    }

    /**
       Internal helper to forward functions to JS-side implementations.
    */
    static void sqlite_func_step_forwarder(sqlite3_context* context, int argc, sqlite3_value ** argv)
    {
	sqlite_func_forwarder_impl( 1, context, argc, argv );
    }
    /**
       Internal helper to forward functions to JS-side implementations.
    */
    static void sqlite_func_finalize_forwarder(sqlite3_context* context)
    {
	sqlite_func_forwarder_impl( 2, context, 0, 0 );
    }


    JS_WRAPPER(sq3_create_function)
    {
// int sqlite3_create_function(
//[0]   sqlite3 *,
//[1]   [const char * | void const *] zFunctionName, 
//[2]   int nArg,
//[3]   int eTextRep,
//[4]   void *pUserData,
//[5]   void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
//[6]   void (*xStep)(sqlite3_context*,int,sqlite3_value**),
//[7]   void (*xFinal)(sqlite3_context*)
// );
// JS call:
// sqlite3_create_function( dbh, funcname, nArg, encoding, userdata, xFunc = 0 [, xStep = 0, [xFinal = 0]])
	const int argc = argv.Length();
	ASSERTARGS(sqlite3_create_function,((argc>5) && (argc<9)));
	DB_ARG(argv[0]);
	std::string fname = CastFromJS<std::string>( argv[1] );
	if( fname.empty() )
	{
	    return ThrowException(String::New("SQL function name may not be empty!"));
	}
	char const * fnameC = fname.c_str();
	int nArg = CastFromJS<int>( argv[2] );
	int enc = CastFromJS<int>( argv[3] );
	enc = SQLITE_UTF8; // only supported value for the moment.

	//HandleScope sentry;

	CallbackInfo & fi = db->userFuncs[fname];// = CallbackInfo();
	int rc = 0;
	{ // new scope for testing some scoping issues...
	    //HandleScope sentry;
	    fi.bogo = 171311 + nArg;
	    fi.self = argv.Holder();
	    fi.func_name = fname;
	    fi.data = argv[4];

	    void (*xFunc)(sqlite3_context*,int,sqlite3_value**) = 0; //sqlite_func_forwarder
	    void (*xStep)(sqlite3_context*,int,sqlite3_value**) = 0; //sqlite_func_step_forwarder
	    void (*xFinal)(sqlite3_context*) = 0; //sqlite_func_finalize_forwarder

#define BAIL(N) return ThrowException(String::New("Argument #" # N " is not a Function!"))
	    Handle<Value> arg( argv[5] );
	    if( arg->IsFunction() )
	    {
		fi.func =
		    //Handle<Function>( Function::Cast( *arg ) );
		    Persistent<Function>::New( Handle<Function>( Function::Cast( *arg ) ) );
		//Function::Cast( *arg );
		//Persistent<Function>( FunctionTemplate::New( >( Function::Cast( *arg ) ) );
		//CERR << "setting fi.func: " <<CastFromJS<std::string>(fi.func)<<"\n";
		xFunc = sqlite_func_forwarder;
		assert( fi.func->IsFunction() );
		db->jsobj->Set( String::New("reftest"), fi.func );
	    }
	    else BAIL(5);

	    if( argc > 6 )
	    {
		arg = argv[6];
		if( arg->IsFunction() )
		{
		    //CERR << "setting fi.func_step\n";
		    fi.func_step = Persistent<Function>::New( Handle<Function>(Function::Cast( *arg )) );
		    xStep = sqlite_func_step_forwarder;
		}
		else BAIL(6);
	    }
	    if( argc > 7 )
	    {
		arg = argv[7];
		if( arg->IsFunction() )
		{
		    //CERR << "setting fi.func_final\n";
		    fi.func_final = Persistent<Function>::New( Handle<Function>( Function::Cast( *arg ) ) );
		    xFinal = sqlite_func_finalize_forwarder;
		}
		else BAIL(7);
	    }
#undef BAIL
	    
	    rc = ::sqlite3_create_function( db->dbh,
					    fnameC, // func name
					    nArg, // arg count, or -1 for "any"
					    enc, // encoding
					    &fi, // arbitrary user data
					    xFunc,// void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
					    xStep,// void (*xStep)(sqlite3_context*,int,sqlite3_value**),
					    xFinal // void (*xFinal)(sqlite3_context*)
					    );
	    if(0)
	    {
		CERR << "sqlite3_create_function("
		     << "dbh="<<(void const *)db->dbh
		     << ", \"" << fi.func_name<<"\""
		     << ", " << nArg
		     << ", " << enc
		     << ", userData @" << &fi
		     <<", func @" << (void const *)xFunc
		     <<", step_func @" << (void const *)xStep
		     <<", final_func @" << (void const *)xFinal
		     <<"), rc="<<rc <<'\n'
		     << "fi.bogo:"<<fi.bogo<<", fi.func: " <<CastFromJS<std::string>(fi.func)
		     <<'\n';

	    }
	}
	if(0)
	{ /**
	     Trying to narrow down exactly where/why fi.func is losing its value before the callback
	     is triggered. If i call it here using sqlite3_exec() it works as expected. But if its
	     not called until after this func returns, fi.func loses its Function value somewhere.
	  */
	    std::ostringstream sql;
	    //sql << "select "<<fi.func_name<<"(7,11,13);";
	    sql << "select "<<fi.func_name<<"(9,11,17,23,27);";
	    sqlite3_exec( db->dbh, sql.str().c_str(), 0, 0, 0 );
	}
	return CastToJS( rc );
    }

    JS_WRAPPER(sq3_value_int)
    {
	ASSERTARGS(sqlite3_value_int,(1==argv.Length()));
	SQVAL_ARG_THROW(argv[0]);
	return CastToJS( ::sqlite3_value_int( sqval ) );
    }

    JS_WRAPPER(sq3_value_int64)
    {
	ASSERTARGS(sqlite3_value_int64,(1==argv.Length()));
	SQVAL_ARG_THROW(argv[0]);
	return CastToJS( ::sqlite3_value_int64( sqval ) );
    }


    JS_WRAPPER(sq3_value_double)
    {
	ASSERTARGS(sqlite3_value_double,(1==argv.Length()));
	SQVAL_ARG_THROW(argv[0]);
	return CastToJS( ::sqlite3_value_double( sqval ) );
    }
    
    JS_WRAPPER(sq3_value_text)
    {
	ASSERTARGS(sqlite3_value_text,(1==argv.Length()));
	SQVAL_ARG_THROW(argv[0]);
	const unsigned char * str = ::sqlite3_value_text( sqval );
	return str
	    ? CastToJS<std::string>( reinterpret_cast<char const *>( str ) )
	    : Handle<Value>( ::v8::Null() );
    }

    JS_WRAPPER(sq3_result_int)
    {
	ASSERTARGS(sqlite3_result_int,(2==argv.Length()));
	SQCX_ARG_THROW(argv[0]);
	::sqlite3_result_int( sqcx, CastFromJS<int>( argv[1] ) );
	return Undefined();
    }

    JS_WRAPPER(sq3_result_int64)
    {
	ASSERTARGS(sqlite3_result_int64,(2==argv.Length()));
	SQCX_ARG_THROW(argv[0]);
	::sqlite3_result_int64( sqcx, CastFromJS<sqlite3_int64>( argv[1] ) );
	return Undefined();
    }

    JS_WRAPPER(sq3_result_double)
    {
	ASSERTARGS(sqlite3_result_double,(2==argv.Length()));
	SQCX_ARG_THROW(argv[0]);
	::sqlite3_result_double( sqcx, CastFromJS<double>( argv[1] ) );
	return Undefined();
    }


    JS_WRAPPER(sq3_result_value)
    {
	ASSERTARGS(sqlite3_result_value,(2==argv.Length()));
	SQCX_ARG_THROW(argv[0]);
	SQVAL_ARG_THROW(argv[1]);
	::sqlite3_result_value( sqcx, sqval );
	return Undefined();
    }

    JS_WRAPPER(sq3_result_null)
    {
	ASSERTARGS(sqlite3_result_null,(1==argv.Length()));
	SQCX_ARG_THROW(argv[0]);
	::sqlite3_result_null( sqcx );
	return Undefined();
    }

    JS_WRAPPER(sq3_result_error)
    {
	ASSERTARGS(sqlite3_result_error,(2==argv.Length()));
	SQCX_ARG_THROW(argv[0]);
	std::string msg = CastFromJS<std::string>( argv[1] );
	::sqlite3_result_error( sqcx, msg.c_str(), static_cast<int>(msg.size()) );
	return Undefined();
    }

    JS_WRAPPER(sq3_result_text)
    {
	ASSERTARGS(sqlite3_result_text,(2==argv.Length()));
	SQCX_ARG_THROW(argv[0]);
	std::string msg = CastFromJS<std::string>( argv[1] );
	::sqlite3_result_text( sqcx, msg.c_str(), static_cast<int>(msg.size()), SQLITE_TRANSIENT );
	return Undefined();
    }

    JS_WRAPPER(sq3_select_array)
    {
	ASSERTARGS(sqlite3_select_array,(2==argv.Length()));
	DB_ARG(argv[0]);
	std::string sql = CastFromJS<std::string>( argv[1] );
	int nRows = 0;
	int nCols = 0;
	char ** table = 0;
	//char * errmsg = 0; // consider throwing (with this error string) on error
	int rc = ::sqlite3_get_table( db->dbh, sql.c_str(), &table, &nRows, &nCols, 0 );
	if( SQLITE_OK != rc )
	{
	    return Integer::New(rc);
	}
	Handle<Object> outer( Object::New() );
	Handle<Array> cols( Array::New(nCols) );
	Handle<Array> rows( Array::New(nRows) );
#define STR String::New
	outer->Set(STR("cols"), cols );
	outer->Set(STR("rows"), rows );
	for( int c = 0; c < nCols; ++c )
	{
	    cols->Set(Integer::New(c),STR(table[c]));
	}
	for( int r = 0; r < nRows; ++r )
	{
	    Handle<Array> ra( Array::New(nCols) );
	    rows->Set(Integer::New(r),ra);
	    int colOff = nCols + (r * nCols);
	    for( int c = 0; c < nCols; ++c, ++colOff )
	    {
		ra->Set(Integer::New(c),StdStringToJS(table[colOff]));
	    }
	}
#undef STR
	sqlite3_free_table(table);
	return outer;

    }



#if 0 // boilerplate code for copy/replace:
	JS_WRAPPER(sq3_XXX)
	{
	    //ASSERTARGS(sqlite3_XXX,(2==argv.Length()));
	    //DB_ARG(argv[0]);
	    //STMT_ARG_THROW(argv[0]);
	    //return CastToJS( ::sqlite3_XXX( db->dbh, stmt ) );
	    return ThrowException(String::New("Native function not implemented!"));
	}
#endif

#undef ASSERTARGS
#undef DB_ARG
#undef STMT_ARG
#undef STMT_ARG_OR
#undef STMT_ARG_THROW
#undef JS_WRAPPER

    Handle<Value> SetupAddon( Handle<Object> gl )
    {
#if 0
	if( DBInfo::js_prototype.IsEmpty() )
	{
	    Handle<ObjectTemplate> wtf = ObjectTemplate::New();
	    DBInfo::js_prototype = Persistent<ObjectTemplate>::New( wtf );
	}
#endif

#define ADDFUNC(SUF) gl->Set(String::New("sqlite3_" # SUF), FunctionTemplate::New(sq3_ ## SUF)->GetFunction() )
	ADDFUNC(open);
	ADDFUNC(close);
	ADDFUNC(prepare);
	ADDFUNC(reset);
	ADDFUNC(clear_bindings);
	ADDFUNC(finalize);
	ADDFUNC(step);
	ADDFUNC(errmsg);
	ADDFUNC(bind_null);
	ADDFUNC(bind_int);
	ADDFUNC(bind_double);
	ADDFUNC(bind_int64);
	ADDFUNC(bind_text);
	ADDFUNC(bind);
	ADDFUNC(bind_parameter_index);
	ADDFUNC(bind_parameter_name);
	ADDFUNC(bind_parameter_count);
	ADDFUNC(column_bytes);
	ADDFUNC(column_int);
	ADDFUNC(column_int64);
	ADDFUNC(column_double);
	ADDFUNC(column_type);
	ADDFUNC(column_name);
	ADDFUNC(column_decltype);
	ADDFUNC(column_count);
	ADDFUNC(column_text);
	ADDFUNC(busy_timeout);
	ADDFUNC(changes);
	ADDFUNC(get_autocommit);
	ADDFUNC(libversion);
	ADDFUNC(errcode);
	ADDFUNC(expired);
	ADDFUNC(last_insert_rowid);
	ADDFUNC(complete);
	ADDFUNC(data_count);
	ADDFUNC(interrupt);
	ADDFUNC(extended_result_codes);
	ADDFUNC(sleep);
	ADDFUNC(is_okay);
	ADDFUNC(select_int);
	ADDFUNC(select_double);
	ADDFUNC(select_int64);
	ADDFUNC(select_text);
	ADDFUNC(create_function);
	ADDFUNC(exec);
	ADDFUNC(escape_sql);
	ADDFUNC(quote_sql);
	ADDFUNC(value_int);
	ADDFUNC(value_int64);
	ADDFUNC(value_double);
	ADDFUNC(value_text);
	ADDFUNC(result_int);
	ADDFUNC(result_int64);
	ADDFUNC(result_double);
	ADDFUNC(result_null);
	ADDFUNC(result_value);
	ADDFUNC(result_text);
	ADDFUNC(result_error);

	ADDFUNC(select_array);

#undef ADDFUNC

#define SET_MAC(N) gl->Set(String::New(# N), Integer::New(N), ::v8::ReadOnly)
	SET_MAC(SQLITE_OK);
	SET_MAC(SQLITE_ERROR);
	SET_MAC(SQLITE_INTERNAL);
	SET_MAC(SQLITE_PERM);
	SET_MAC(SQLITE_ABORT);
	SET_MAC(SQLITE_BUSY);
	SET_MAC(SQLITE_LOCKED);
	SET_MAC(SQLITE_NOMEM);
	SET_MAC(SQLITE_READONLY);
	SET_MAC(SQLITE_INTERRUPT);
	SET_MAC(SQLITE_IOERR);
	SET_MAC(SQLITE_CORRUPT);
	SET_MAC(SQLITE_NOTFOUND);
	SET_MAC(SQLITE_FULL);
	SET_MAC(SQLITE_CANTOPEN);
	SET_MAC(SQLITE_PROTOCOL);
	SET_MAC(SQLITE_EMPTY);
	SET_MAC(SQLITE_SCHEMA);
	SET_MAC(SQLITE_TOOBIG);
	SET_MAC(SQLITE_CONSTRAINT);
	SET_MAC(SQLITE_MISMATCH);
	SET_MAC(SQLITE_MISUSE);
	SET_MAC(SQLITE_NOLFS);
	SET_MAC(SQLITE_AUTH);
	SET_MAC(SQLITE_ROW);
	SET_MAC(SQLITE_DONE);

	SET_MAC(SQLITE_UTF8);
	SET_MAC(SQLITE_UTF16);
	SET_MAC(SQLITE_UTF16BE);
	SET_MAC(SQLITE_UTF16LE);
	SET_MAC(SQLITE_ANY);
	SET_MAC(SQLITE_INTEGER);
	SET_MAC(SQLITE_FLOAT);
	SET_MAC(SQLITE_TEXT);
	SET_MAC(SQLITE_BLOB);
	SET_MAC(SQLITE_NULL);

#ifdef SQLITE_IOERR_READ
	// enable extended codes...
	SET_MAC(SQLITE_IOERR_READ);
	SET_MAC(SQLITE_IOERR_SHORT_READ);
	SET_MAC(SQLITE_IOERR_WRITE);
	SET_MAC(SQLITE_IOERR_FSYNC);
	SET_MAC(SQLITE_IOERR_DIR_FSYNC);
	SET_MAC(SQLITE_IOERR_TRUNCATE);
	SET_MAC(SQLITE_IOERR_FSTAT);
	SET_MAC(SQLITE_IOERR_UNLOCK);
	SET_MAC(SQLITE_IOERR_RDLOCK);
#endif // SQLITE_IOERR_READ

#undef SET_MAC
#define SET_MAC(N) gl->Set(String::New(# N), External::New((void *)N), ::v8::ReadOnly)
	SET_MAC(SQLITE_TRANSIENT);
	SET_MAC(SQLITE_STATIC);

#undef SET_MAC

	return gl->Get(String::New("sqlite3_open"));
	//return Boolean::New(true);
    }

#undef JS_WRAPPER


    namespace {
	static void InitPlugin()
	{
	    using v8::juice::plugin::LoadPluginScope;
	    LoadPluginScope * sc = LoadPluginScope::Current();
	    if( ! sc ) return; // not called by LoadModule()
	    sc->SetReturnValue( SetupAddon( sc->Target() ) );
	}
	static bool dll_initializer = (InitPlugin(),true);
    }

}}} /* namespaces */
