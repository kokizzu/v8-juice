#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

/**
   Implementation code for v8 (JavaScript engine) bindings for sqlite3.

   v8: http://code.google.com/p/v8/

   sqlite3: http://sqlite.org

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: Public Domain

   Pedantic license note: much of this code was ported from the
   SpiderApe JS/sqlite3 bindings (http://SpiderApe.sf.net). Though
   that code is MPL, i wrote it, and am re-licensing this copy to
   be in the Public Domain.
*/

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
#include <v8/juice/cleanup.h>
#include <v8/juice/ClassBinder.h>

#include "sq3.hpp"

#define JSTR(X) ::v8::String::New(X)
#define TOSS(X) return ::v8::ThrowException(JSTR(X))
#define TOSSV(X) return ::v8::ThrowException(X)
#define PLUGIN_RTFM "RTFM: " v8_juice_HOME_PAGE "/wiki/PluginSqlite3"


class DBWrapper;
class StmtWrapper;
class CursorWrapper;
namespace v8 { namespace juice {
        template <>
        struct WeakJSClassCreatorOps< DBWrapper >;
        template <>
        struct WeakJSClassCreatorOps< StmtWrapper >;
        template <>
        struct WeakJSClassCreatorOps< CursorWrapper >;
}}
/**
   Convenience macro for marking wrapper functions.
*/
#define JS_WRAPPER(FN) static ::v8::Handle< ::v8::Value > FN( const ::v8::Arguments & argv )
using namespace v8;
using namespace v8::juice;
using namespace v8::juice::convert;

struct strings
{
    static char const * classDB;
    static char const * classStatement;
    static char const * classCursor;
};
char const * strings::classDB = "DB";
char const * strings::classStatement = "Statement";
char const * strings::classCursor = "Cursor";

/**
   Internal type for binding sqlite3-related info to JS. It
   encapsulates a single db opened via sqlite3_open().
*/
class DBWrapper// : public v8::juice::WrappedClassBase<DBWrapper>
{
public:
    typedef v8::juice::WrappedClassBase<DBWrapper> ScriptedBase;
    /**
       Database handle. This type owns handles associated
       with it.
    */
    sqlite3 * dbh;
    sq3::database proxy;
    /**
       Not necessary - can be removed. We use v8::External to pass
       DBWrapper objects around, instead of a full-fledged Object.
    */
    //static Persistent<ObjectTemplate> js_prototype;
    static Persistent<Function> js_ctor;
	
    /**
       Creates a new
    */
    DBWrapper(Handle<Object> jself, sqlite3 * d ) :
        //ScriptedBase( jself ),
        dbh(d),
        proxy()
    {
        proxy.take_handle( d );
        bind::BindNative( this, this );
    }
    /**
       Unbinds this object from the JS/native bindings. Does not
       (cannot?) tell JS to clean up its references, though.
    */
    ~DBWrapper()
    {
        bind::UnbindNative( this, this );
        //CERR << "Closing sqlite3 handle @"<<dbh<<'\n';
        // FIXME: finalize any open statements.
        dbh = 0;
    }

    typedef Handle<Value> HV;
    HV close();
    HV insertid() { return CastToJS(proxy.insertid()); }
    HV changes() { return CastToJS(proxy.changes()); }
    HV setbusytimeout( int ms ) { return CastToJS(proxy.setbusytimeout(ms)); }

    HV pragma( std::string const & );
    HV execute( Arguments const & argv );
    HV errmsg();
    HV vacuum();
    HV clear();

    static DBWrapper * GetNative( HV );
    static DBWrapper * GetNative( Local<Object> );

};
Persistent<Function> DBWrapper::js_ctor;


class StmtWrapper// : public v8::juice::WrappedClassBase<StmtWrapper>
{
public:
    static Persistent<Function> js_ctor;
    sq3::statement proxy;
    //typedef v8::juice::WrappedClassBase<StmtWrapper> ScriptedBase;
    StmtWrapper( DBWrapper * db, std::string sql )
        : proxy( db->proxy, sql )
    {
    }
    explicit StmtWrapper( DBWrapper * db )
        : proxy( db->proxy )
    {
    }

    typedef Handle<Value> HV;
    //HV finalize() { return CastToJS(proxy.finalize()); }
    HV finalize();
    HV prepare( Arguments const & );

    HV bindNull( int ndx ) { return CastToJS(proxy.bind(ndx)); }
    HV bindInt( int ndx, int val ) { return CastToJS(proxy.bind(ndx,val)); }
    HV bindInt64( int ndx, int64_t val ) { return CastToJS(proxy.bind(ndx,val)); }
    HV bindDouble( int ndx, double val ) { return CastToJS(proxy.bind(ndx,val)); }
    HV bindText( int ndx, std::string const & val, uint32_t len )
    {
        if( len > val.size() ) len = val.size();
        return CastToJS(proxy.bind(ndx,val.data(),len));
    }
    HV bindText( int ndx, std::string const & val ) { return this->bindText( ndx, val, val.size() ); }

    HV reset() { return CastToJS(proxy.reset()); }
    HV clearBindings() { return CastToJS(proxy.clear_bindings()); }
    HV columnCount() { return CastToJS(proxy.colcount()); }

    bool isPrepared() const { return proxy.is_prepared(); }

    static StmtWrapper * GetNative( HV );
    static StmtWrapper * GetNative( Local<Object> );

    HV getCursor();
};
Persistent<Function> StmtWrapper::js_ctor;

class CursorWrapper// : public v8::juice::WrappedClassBase<StmtWrapper>
{
public:
    static Persistent<Function> js_ctor;
    sq3::cursor proxy;
    StmtWrapper * stmt;
    CursorWrapper( StmtWrapper * st )
        : proxy( st->proxy ), stmt(st)
    {
    }
    ~CursorWrapper()
    {
    }

    typedef Handle<Value> HV;
    HV close() { proxy.close(); return ::v8::Undefined(); }
    HV columnCount() { return CastToJS(proxy.colcount()); }
    HV step() { return CastToJS(proxy.step()); }
    HV reset() { return CastToJS(proxy.reset()); }

    HV columnName( int index )
    {
        std::string name;
        if( SQLITE_OK != proxy.colname( index, name) )
        {TOSS("Error getting column info!");}
        return CastToJS(name);
    }

    HV isNull( int index )
    {
        bool tgt;
        if( SQLITE_OK != proxy.isnull( index, tgt) )
        {TOSS("Error getting column info!");}
        return CastToJS(tgt);
    }

    /**
       GetType must be one of: int, int64_t, double, std::string
    */
    template <typename GetType>
    HV getT( int index )
    {
        GetType tgt;
        if( SQLITE_OK != proxy.get( index, tgt) )
        {TOSS("Error getting column info!");}
        return CastToJS(tgt);
    }


//     HV bindNull( int ndx ) { return CastToJS(proxy.bind(ndx)); }
//     HV bindInt( int ndx, int val ) { return CastToJS(proxy.bind(ndx,val)); }
//     HV bindInt64( int ndx, int64_t val ) { return CastToJS(proxy.bind(ndx,val)); }
//     HV bindDouble( int ndx, double val ) { return CastToJS(proxy.bind(ndx,val)); }
//     HV bindText( int ndx, std::string const & val, uint32_t len )
//     {
//         if( len > val.size() ) len = val.size();
//         return CastToJS(proxy.bind(ndx,val.data(),len));
//     }
//     HV bindText( int ndx, std::string const & val ) { return this->bindText( ndx, val, val.size() ); }

};
Persistent<Function> CursorWrapper::js_ctor;


/**
   Base class for WeakJSClassCreatorOps<T> specializations,
   where T is one of our internal wrapper types.
*/
template <typename WrappedT, char const * & className>
struct BaseWeakOps
{
    typedef WrappedT WrappedType;
    enum {
    ExtraInternalFieldCount = 0
    };
    static char const * ClassName() { return className; }
    static void AddToCleanup( WrappedType * obj )
    {
        ::v8::juice::cleanup::AddToCleanup( obj, cleanup_callback );
    }
    static void Dtor( WrappedType * obj )
    {
        cleanup::RemoveFromCleanup( obj );
        CERR << "Destroying wrapped object @"<<obj<<'\n';
        delete obj;
    }
    static void cleanup_callback( void * obj )
    {
        Dtor( static_cast<WrappedType *>( obj ) );
    }
};

namespace v8 { namespace juice {

    using namespace convert;
    template <>
    struct WeakJSClassCreatorOps<DBWrapper> : BaseWeakOps<DBWrapper,strings::classDB>
    {
        static WrappedType * Ctor( Arguments const & argv,
                                   std::string & exceptionText )
        {
            if( argv.Length() != 1 )
            {
                exceptionText = "Constructor usage: (string filename)";
                return 0;
            }
            std::string name( JSToStdString(argv[0]) );
            sqlite3 * dbh = 0;
            int rc = sqlite3_open( name.c_str(), &dbh );
            if( SQLITE_OK != rc )
            {
                std::ostringstream os;
                os << "sqlite3_open(\""<<name<<"\") failed with error code "<<rc<<'!';
                exceptionText = os.str();
                return 0;
            }
            DBWrapper * db = new DBWrapper(argv.This(), dbh);
            //db->jsobj = argv.This();
            CERR << "Created DBWrapper object @"<<db<<'\n';
            AddToCleanup( db );
            return db;
        }
    };

    using namespace convert;
    template <>
    struct WeakJSClassCreatorOps<StmtWrapper> : BaseWeakOps<StmtWrapper,strings::classStatement>
    {
        static WrappedType * Ctor( Arguments const & argv,
                                   std::string & exceptionText )
        {
            char const argc = argv.Length();
            if( !argc || (argc>2) )
            {
                exceptionText = "Constructor usage: (DB [, string SQL])";
                return 0;
            }
            DBWrapper * db = DBWrapper::GetNative( argv[0] );
            if( ! db )
            {
                exceptionText = "First argument must be a DB object!";
                return 0;
            }
            StmtWrapper * st = 0;
            if( 2 == argc )
            {
                st = new StmtWrapper( db, JSToStdString(argv[1]) );
            }
            else
            {
                st = new StmtWrapper( db );
            }
            CERR << "Created StmtWrapper object @"<<st<<'\n';
            AddToCleanup( st );
            return st;
        }
    };

    using namespace convert;
    template <>
    struct WeakJSClassCreatorOps<CursorWrapper> : BaseWeakOps<CursorWrapper,strings::classCursor>
    {
        static WrappedType * Ctor( Arguments const & argv,
                                   std::string & exceptionText )
        {
            char const argc = argv.Length();
            if( !argc || (argc>2) )
            {
                exceptionText = "Constructor usage: (Statement)";
                return 0;
            }
            StmtWrapper * st = StmtWrapper::GetNative( argv[0] );
            if( ! st )
            {
                exceptionText = "First argument must be a Statement object!";
                return 0;
            }
            CursorWrapper * cur = new CursorWrapper( st );
            CERR << "Created CursorWrapper object @"<<st<<'\n';
            AddToCleanup( cur );
            return cur;
        }
    };

} } // namespaces
#define WEAK_CLASS_TYPE DBWrapper
#include <v8/juice/WeakJSClassCreator-CastOps.h>
#define WEAK_CLASS_TYPE StmtWrapper
#include <v8/juice/WeakJSClassCreator-CastOps.h>
#define WEAK_CLASS_TYPE CursorWrapper
#include <v8/juice/WeakJSClassCreator-CastOps.h>

using namespace ::v8;
using namespace ::v8::juice;

typedef ClassBinder<DBWrapper> DBWeakWrap;
typedef ClassBinder<StmtWrapper> StmtWeakWrap;
typedef ClassBinder<CursorWrapper> CursorWeakWrap;


Handle<Value> DBWrapper::close()
{
    DBWeakWrap::DestroyObject( DBWeakWrap::GetJSObject(this) );
    return Undefined();
}
Handle<Value> DBWrapper::execute( Arguments const & argv )
{
    //return proxy.execute( sql.c_str() );
    TOSS("Not yet implemented.");
}

DBWrapper * DBWrapper::GetNative( Handle<Value> v )
{
    return WeakJSClassCreator<DBWrapper>::GetNative(v);
}
DBWrapper * DBWrapper::GetNative( Local<Object> v )
{
    return WeakJSClassCreator<DBWrapper>::GetSelf(v);
}

Handle<Value> DBWrapper::errmsg()
{
    return CastToJS( sqlite3_errmsg(proxy.handle()) );
}
Handle<Value> DBWrapper::vacuum()
{
    return CastToJS( proxy.vacuum() );
}
Handle<Value> DBWrapper::clear()
{
    return CastToJS( proxy.clear() );
}

Handle<Value> DBWrapper::pragma( std::string const & str)
{
    return CastToJS( proxy.pragma(str.c_str()) );
}


Handle<Value> StmtWrapper::finalize()
{
    int rc = proxy.finalize();
    StmtWeakWrap::DestroyObject( StmtWeakWrap::GetJSObject(this) );
    return CastToJS( rc );
}

Handle<Value> StmtWrapper::getCursor()
{
    Handle<Value> jobj( StmtWeakWrap::GetJSObject(this) );
    if( jobj.IsEmpty() ) TOSS("Could not get JS-side object for native!");
    return CursorWrapper::js_ctor->NewInstance(1, &jobj );
}

StmtWrapper * StmtWrapper::GetNative( Handle<Value> v )
{
    return WeakJSClassCreator<StmtWrapper>::GetNative(v);
}
StmtWrapper * StmtWrapper::GetNative( Local<Object> v )
{
    return WeakJSClassCreator<StmtWrapper>::GetSelf(v);
}


#define ARGC int argc = argv.Length()
#define ASSERTARGS(COND) if(!(COND)) return ThrowException(String::New("Arguments assertion failed: " # COND))


#define DB_ARG(HND) DBWrapper * db = CastFromJS<DBWrapper>( HND );            \
    if( ! db ) return ThrowException(String::New("Argument is not a sqlite3 handle!"));

#define SETUP_DBFWD(ARG_COND) ARGC; ASSERTARGS( ARG_COND ); DB_ARG(argv[0])


JS_WRAPPER(sq3_open)
{
    ARGC; ASSERTARGS((argc==1));
    Handle<Value> arg0( argv[0] );
    TryCatch tryer;
    Local<Object> dbjo = DBWrapper::js_ctor->NewInstance(1, &arg0 );
    if( dbjo.IsEmpty() )
    {
        return ThrowException(tryer.Exception());
    }
    DBWrapper * db = DBWeakWrap::GetSelf(dbjo);
    if( ! db )
    {
        return ThrowException(String::New("Internal error: created wrapped native but could not convert JS object back to the native!"));
    }
    return dbjo;
}


JS_WRAPPER(sq3_close)
{
    SETUP_DBFWD((argc==1));
    return db->close();;
}
JS_WRAPPER(sq3_last_insert_rowid)
{
    SETUP_DBFWD((argc==1));
    return db->insertid();
}
JS_WRAPPER(sq3_errmsg)
{
    SETUP_DBFWD((argc==1));
    return db->errmsg();
}

JS_WRAPPER(sq3_changes)
{
    SETUP_DBFWD((argc==1));
    return db->changes();
}


Handle<Value> StmtWrapper::prepare( Arguments const & argv )
{
    ARGC; ASSERTARGS(1==argc);
    std::string sql( JSToStdString(argv[0]) );
    if( sql.empty() ) TOSS("SQL string is empty!");
    return CastToJS( (SQLITE_OK == proxy.prepare( sql )) ? true : false );
}


#define STMT_ARG(HND) StmtWrapper * st = CastFromJS<StmtWrapper>( HND );            \
    if( ! st ) return ThrowException(String::New("Argument is not a sqlite3_stmt handle!"));
#define SETUP_STMTFWD(ARG_COND) ARGC; ASSERTARGS( ARG_COND ); STMT_ARG(argv[0])

JS_WRAPPER(sq3_prepare)
{
    //SETUP_DBFWD(argc==2);
    ARGC; ASSERTARGS(2==argc);
    Handle<Value> args[] = { argv[0], argv[1] };
    Handle<Value> ex;
    Local<Object> jsobj;
    {
        TryCatch tryer;
        jsobj = StmtWrapper::js_ctor->NewInstance(2, args );
        if( jsobj.IsEmpty() )
        {
            ex = tryer.Exception();
        }
    }
    if( ! ex.IsEmpty() ) TOSSV(ex);
    StmtWrapper * stmt = StmtWeakWrap::GetSelf(jsobj);
    if( ! stmt )
    {
        return ThrowException(String::New("Internal error: created wrapped native but could not convert JS object back to the native!"));
    }
    if( ! stmt->isPrepared() )
    {
        StmtWeakWrap::DestroyObject( jsobj );
        return Null();
    }
    return jsobj;
}

JS_WRAPPER(sq3_finalize)
{
    SETUP_STMTFWD((argc==1));
    return st->finalize();;
}

JS_WRAPPER(sq3_bind_null)
{
    SETUP_STMTFWD((argc==3));
    return st->bindNull( JSToInt32(argv[1]) );
}

JS_WRAPPER(sq3_bind_int)
{
    SETUP_STMTFWD((argc==3));
    return st->bindInt( JSToInt32(argv[1]), JSToInt32(argv[2]) );
}

JS_WRAPPER(sq3_bind_int64)
{
    SETUP_STMTFWD((argc==3));
    return st->bindInt64( JSToInt32(argv[1]), JSToInt64(argv[2]) );
}

JS_WRAPPER(sq3_bind_double)
{
    SETUP_STMTFWD((argc==3));
    return st->bindDouble( JSToInt32(argv[1]), JSToDouble(argv[2]) );
}

JS_WRAPPER(sq3_bind_text)
{
    SETUP_STMTFWD((argc==3) || (argc==4));
    if( 2 == argc )
    {
        return st->bindText( JSToInt32(argv[1]), JSToStdString(argv[2]) );
    }
    else
    {
        return st->bindText( JSToInt32(argv[1]), JSToStdString(argv[2]), JSToUInt32(argv[3]) );
    }
}

JS_WRAPPER(sq3_reset)
{
    SETUP_STMTFWD((argc==1));
    return st->reset();
}
JS_WRAPPER(sq3_clear_bindings)
{
    SETUP_STMTFWD((argc==1));
    return st->clearBindings();
}

JS_WRAPPER(sq3_column_count)
{
    SETUP_STMTFWD((argc==1));
    return st->columnCount();
}

#if 0 // boilerplate code for copy/replace:
JS_WRAPPER(sq3_XXX)
{
    SETUP_DBFWD((argc==1));
    return ThrowException(String::New("Native function not implemented!"));
}
#endif

#undef ASSERTARGS
#undef DB_ARG
#undef STMT_ARG
#undef STMT_ARG_OR
#undef STMT_ARG_THROW
#undef JS_WRAPPER

static Handle<Value> SetupSQ3( Handle<Object> gl )
{
    CERR << "Initializing sqlite3 wrapper plugin...\n";

    Handle<Object> sqobj( Object::New() );
    gl->Set(JSTR("sqlite3"), sqobj);
    typedef Handle<Value> HV;
    {
        DBWeakWrap gen(sqobj);
        typedef DBWrapper DBW;
        gen.BindMemFunc< HV, &DBW::close >( "close" );
        gen.BindMemFunc< HV, &DBW::changes >( "changes" );
        gen.BindMemFunc< HV, &DBW::insertid >( "lastInsertID" );
        gen.BindMemFunc< HV, int, &DBW::setbusytimeout >( "setBusyTimeout" );
        gen.BindMemFunc< &DBW::execute >("execute");
        gen.BindMemFunc< HV, &DBW::errmsg >("errorMessage");
        gen.BindMemFunc< HV, &DBW::vacuum >("vacuum");
        gen.BindMemFunc< HV, &DBW::clear >("clear");
        gen.BindMemFunc< HV, std::string const &, &DBW::pragma >("execPragma");
        gen.Seal();
        if( DBW::js_ctor.IsEmpty() )
        {
            DBW::js_ctor = Persistent<Function>::New( gen.CtorTemplate()->GetFunction() );
        }
    }


    {
        StmtWeakWrap stmt(sqobj);
        typedef StmtWrapper STW;
        stmt.BindMemFunc< HV, &STW::finalize >("finalize");
        stmt.BindMemFunc< &STW::prepare >("prepare");
        stmt.BindMemFunc< bool, &STW::isPrepared >("isPrepared");

        stmt.BindMemFunc< HV, int, &STW::bindNull >( "bindNull" );
        stmt.BindMemFunc< HV, int, int, &STW::bindInt >( "bindInt" );
        stmt.BindMemFunc< HV, int, int64_t, &STW::bindInt64 >( "bindInt64" );
        stmt.BindMemFunc< HV, int, double, &STW::bindDouble >( "bindDouble" );
        stmt.BindMemFunc< HV, int, std::string const &, &STW::bindText >( "bindText" );
        stmt.BindMemFunc< HV, &STW::columnCount >( "columnCount" );
        stmt.BindMemFunc< HV, &STW::reset >( "reset" );
        stmt.BindMemFunc< HV, &STW::clearBindings >( "clearBindings" );
        stmt.BindMemFunc< HV, &STW::getCursor >( "getCursor" );
        stmt.Seal();
        if( STW::js_ctor.IsEmpty() )
        {
            STW::js_ctor = Persistent<Function>::New( stmt.CtorTemplate()->GetFunction() );
        }
    }

    {
        CursorWeakWrap curs(sqobj);
        typedef CursorWrapper CURW;
        curs.BindMemFunc< HV, &CURW::close >("close");
        curs.BindMemFunc< HV, &CURW::columnCount >( "columnCount" );
        curs.BindMemFunc< HV, &CURW::step >( "step" );
        curs.BindMemFunc< HV, &CURW::reset >( "reset" );
        curs.BindMemFunc< HV, int, &CURW::columnName >( "columnName" );
        curs.BindMemFunc< HV, int, &CURW::isNull >( "isNull" );

        curs.BindMemFunc< HV, int, &CURW::getT<int> >( "getInt" );
        curs.BindMemFunc< HV, int, &CURW::getT<int64_t> >( "getInt64" );
        curs.BindMemFunc< HV, int, &CURW::getT<double> >( "getDouble" );
        curs.BindMemFunc< HV, int, &CURW::getT<std::string> >( "getString" );

        curs.Seal();
        if( CURW::js_ctor.IsEmpty() )
        {
            CURW::js_ctor = Persistent<Function>::New( curs.CtorTemplate()->GetFunction() );
        }
    }


#define ADDFUNC(SUF) gl->Set(String::New("sqlite3_" # SUF), FunctionTemplate::New(sq3_ ## SUF)->GetFunction() )
    ADDFUNC(open);
    ADDFUNC(close);
    ADDFUNC(changes);
    ADDFUNC(last_insert_rowid);
    ADDFUNC(errmsg);
    ADDFUNC(prepare);
    ADDFUNC(finalize);
    ADDFUNC(bind_null);
    ADDFUNC(bind_int);
    ADDFUNC(bind_int64);
    ADDFUNC(bind_double);
    ADDFUNC(bind_text);
    ADDFUNC(reset);
    ADDFUNC(clear_bindings);
    ADDFUNC(column_count);
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

V8_JUICE_PLUGIN_STATIC_INIT( SetupSQ3 );
