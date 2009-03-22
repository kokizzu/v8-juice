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
#include <set>

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
class CallbackWrapper;
namespace v8 { namespace juice {
        template <>
        struct WeakJSClassCreatorOps< DBWrapper >;
        template <>
        struct WeakJSClassCreatorOps< StmtWrapper >;
        template <>
        struct WeakJSClassCreatorOps< CursorWrapper >;
        template <>
        struct WeakJSClassCreatorOps< CallbackWrapper >;
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
    static char const * classCallback;
    static char const * classStatement;
    static char const * classCursor;
};
char const * strings::classDB = "DB";
char const * strings::classCallback = "$CallbackWrapper$";
char const * strings::classStatement = "Statement";
char const * strings::classCursor = "Cursor";

/**
   A type for holding JS/sqlite3 callback info. We store this
   as a class in JS space so we can try to avoid the function
   handles going out of scope at inopportune times.
*/
class CallbackWrapper
{
public:
    Handle<Value> cb_exception; // when a callback throws a JS exception, we stuff it here.
    Handle<Array> exec_cols; // cached column names for use with sqlite3_exec() callback
    bool cols_cached; // set to true when exec_cols is populated.
    sqlite3 * dbh; // database handle
    Handle<Value> data; // arbitrary user data, used by sqlite3_xxx() callbacks.
    Handle<Value> return_val; // holds the return value for this->func calls.
    /** For creating new instances of this type. */
    static Persistent<Function> js_ctor;
    typedef WeakJSClassCreator<CallbackWrapper> WeakWrap;

    CallbackWrapper()
    {
        data = Undefined();
    }
    ~CallbackWrapper()
    {}
    // must be out-of-class b/c it uses WeakWrap before WeakWrap is defined.
    void AddTo( Handle<Object> tgt );
};
Persistent<Function> CallbackWrapper::js_ctor;

/**
   Internal type for binding sqlite3-related info to JS. It
   encapsulates a single db opened via sqlite3_open().
*/
class DBWrapper
{
public:
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

    typedef WeakJSClassCreator<DBWrapper> WeakWrap;
    /**
       Creates a new
    */
    DBWrapper( sqlite3 * d ) :
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
    HV execute( Arguments const & argv, bool calledAsMember );
    HV errmsg();
    HV vacuum();
    HV clear();

    /**
       T must be one of: int, int64_t, std::string, double
    */
    template <typename T>
    HV executeT( std::string const & sql )
    {
        T val;
        if( SQLITE_OK != proxy.execute( sql, val ) )
        { TOSS("Execution of SQL failed!"); }
        return CastToJS(val);
    }

    static DBWrapper * GetNative( HV );
    static DBWrapper * GetNative( Local<Object> );

};
Persistent<Function> DBWrapper::js_ctor;


class StmtWrapper
{
public:
    typedef WeakJSClassCreator<StmtWrapper> WeakWrap;
    static Persistent<Function> js_ctor;
    sq3::statement proxy;
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

    /**
       bind() impl for T, which must be one of: int, int64_t, double
     */
    template <typename T>
    HV bindNum( int ndx, T val )
    {
        return CastToJS( proxy.bind(ndx,val) );
    }

    HV bindNull( int ndx ) { return CastToJS(proxy.bind(ndx)); }
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

    HV execute() { return CastToJS(proxy.execute()); }

};
Persistent<Function> StmtWrapper::js_ctor;

class CursorWrapper
{
public:
    typedef WeakJSClassCreator<CursorWrapper> WeakWrap;
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
    HV finalize();
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

    HV columnNames()
    {
        const int c = proxy.colcount();
        Handle<Array> ar( Array::New(c) );
        std::string name;
        for( int i = 0; i < c; ++i )
        {
            if( SQLITE_OK != proxy.colname( i, name) )
            {TOSS("Error getting column info!");}
            ar->Set( Integer::New(i),CastToJS(name) );
        }
        return ar;
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
        CERR << "Destroying wrapped "<<ClassName()<<" object @"<<obj<<'\n';
        delete obj;
    }
    static void cleanup_callback( void * obj )
    {
        // FIXME: for Cursors we need to first ensure
        // that all Statements are cleaned up first, or else
        // we can crash at cleanup time if Cursor.close() was
        // never called by the client and the cursor was never GC'd
        Dtor( static_cast<WrappedType *>( obj ) );
    }
};

namespace v8 { namespace juice {

    using namespace convert;
    template <>
    struct WeakJSClassCreatorOps<CallbackWrapper> : BaseWeakOps<CallbackWrapper,strings::classCallback>
    {
        static WrappedType * Ctor( Arguments const & argv,
                                   std::string & exceptionText )
        {
            CallbackWrapper * cb = new CallbackWrapper;
            CERR << "Created "<<ClassName()<<" object @"<<cb<<'\n';
            AddToCleanup( cb );
            return cb;
        }
    };

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
            DBWrapper * db = new DBWrapper(dbh);
            //db->jsobj = argv.This();
            CERR << "Created "<<ClassName()<<" object @"<<db<<'\n';
            AddToCleanup( db );
            return db;
        }
    };

    using namespace convert;
    template <>
    struct WeakJSClassCreatorOps<StmtWrapper> : BaseWeakOps<StmtWrapper,strings::classStatement>
    {
        typedef std::set<StmtWrapper*> GCList;
        static GCList & gcList()
        {
            static GCList bob;
            return bob;
        }
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
                if( ! st->proxy.is_prepared() )
                {
                    delete st;
                    exceptionText = "sqlite3_prepare() failed!";
                    return 0;
                }
            }
            else
            {
                st = new StmtWrapper( db );
            }
            gcList().insert(st);
            CERR << "Created "<<ClassName()<<" object @"<<st<<'\n';
            AddToCleanup( st );
            return st;
        }
        static void Dtor( WrappedType * obj )
        {
            cleanup::RemoveFromCleanup( obj );
            gcList().erase(obj);
            CERR << "Destroying wrapped "<<ClassName()<<" object @"<<obj<<'\n';
            delete obj;
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
            CERR << "Created "<<ClassName()<<" object @"<<st<<'\n';
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
#define WEAK_CLASS_TYPE CallbackWrapper
#include <v8/juice/WeakJSClassCreator-CastOps.h>

using namespace ::v8;
using namespace ::v8::juice;

typedef ClassBinder<DBWrapper> DBWeakWrap;
typedef ClassBinder<StmtWrapper> StmtWeakWrap;
typedef ClassBinder<CursorWrapper> CursorWeakWrap;
typedef ClassBinder<CallbackWrapper> CallbackWeakWrap;

#define ARGC int argc = argv.Length()
#define ASSERTARGS(COND) if(!(COND)) return ThrowException(String::New("Arguments assertion failed: " # COND))


void CallbackWrapper::AddTo( Handle<Object> tgt )
{
    static uint64_t id = 0;
    std::ostringstream os;
    os << "CallbackWrapper_"<<id;
    Handle<String> str( String::New(os.str().c_str()) );
    tgt->Set(str, WeakWrap::GetJSObject( this ) );
}

Handle<Value> DBWrapper::close()
{
    DBWeakWrap::DestroyObject( DBWeakWrap::GetJSObject(this) );
    return Undefined();
}

Handle<Value> CursorWrapper::finalize()
{
    Handle<Object> self = CursorWeakWrap::GetJSObject(this);
    if( self.IsEmpty() ) TOSS("finalize() couldn't get associated JS object!");
    this->proxy.close(); // without this the object may not get disocnnected from its statement until after its statement is dead. Segfault.
    CursorWeakWrap::DestroyObject( self );
    return Undefined();
}

/**
   Internal callback callback for sqlite3_exec(dbh,sql,callback_func,data).
*/
static int sq3_exec_callback(void *data, int argc, char **argv, char **azColName)
{
    CallbackWrapper * fi = static_cast<CallbackWrapper *>(data);
    if( ! fi )
    {
        // Internal consistency error...
        CERR << "sq3c_exec_callback(): (CallbackWrapper *) is null.\n";
        return SQLITE_ERROR;
    }
    Handle<Object> cbjo = CallbackWrapper::WeakWrap::GetJSObject(fi);
    if( ! fi->cols_cached )
    {
        fi->cols_cached = true;
        fi->exec_cols = Array::New(argc);
        for( int i = 0; i < argc; ++i )
        {
            fi->exec_cols->Set( Integer::New(i),
                                CastToJS<std::string>( azColName[i] ) );
        }
        //cbjo->Set(JSTR("colCache"),fi->exec_cols);
    }

    Handle<Function> func;
    {
        Handle<Value> x = cbjo->Get(JSTR("func"));
        if( x.IsEmpty() || ! x->IsFunction() )
        {
            fi->cb_exception = ThrowException(JSTR("Callback function is-not-a Function!"));
            return SQLITE_MISUSE;
        }
        func = Handle<Function>( Function::Cast(*x) );
    }
    Local<Array> rows = Array::New(argc);
    for( int i = 0; i < argc; ++i )
    {
        rows->Set( Integer::New(i),
                   CastToJS<std::string>( argv[i] ) );
    }
    cbjo->Set(JSTR("rows"),fi->exec_cols);
    typedef std::vector< Handle<Value> > VT;
    VT vec( 3, Handle<Value>() );
    vec[0] = fi->data;
    vec[1] = rows;
    vec[2] = fi->exec_cols;
    TryCatch tryer;
    Local<Value> rv
        = func->Call( cbjo, 3, &vec[0] )
        ;
    if( rv.IsEmpty() )
    {
        fi->cb_exception = tryer.Exception();
        return CastFromJS<int>( Integer::New(SQLITE_ERROR) );
    }
    return CastFromJS<int>( rv );
}

Handle<Value> execute_impl( DBWrapper * db,
                            int argc,
                            Handle<Value> argv[] )
{
    ASSERTARGS(argc>0); // TODO: (sql[,callbackFunc[, userData]])
    struct {
        int sql;
        int func;
        int data;
    } pos = { 0, 1, 2 };

    std::string sql( JSToStdString(argv[pos.sql]) );
    CERR << "argc="<<argc<<", pos.func="<<pos.func<<", sql=["<<sql<<"]\n";
    if( argc < 2 )
    {
        return CastToJS( db->proxy.execute(sql) );
    }
    Handle<Value> fv = argv[pos.func];
    if( ! fv->IsFunction() )
    {
        CERR << "fv="<<JSToStdString(fv)<<'\n';
        TOSS("sqlite3_exec() 3rd argument is not a Function!");
    }
    Handle<Object> cbjo = CallbackWrapper::js_ctor->NewInstance( 0, 0 );
    cbjo->Set(JSTR("func"),fv);
    CallbackWrapper * cb = CallbackWrapper::WeakWrap::GetSelf(cbjo);
    cb->data = Undefined();
    if( argc > pos.data ) cb->data = argv[pos.data];
    std::string errmsg;
    int rc = db->proxy.execute( sql, sq3_exec_callback, cb, errmsg );
    Handle<Value> ex = cb->cb_exception;
    CallbackWrapper::WeakWrap::DestroyObject( cbjo );
    if( ! ex.IsEmpty() )
    {
        return ThrowException(ex);
    }
    if( SQLITE_OK != rc )
    {
        TOSS(errmsg.c_str());
    }
    return CastToJS(rc);
}
Handle<Value> DBWrapper::execute( Arguments const & argv, bool calledAsMember )
{
    ARGC; ASSERTARGS(argc>0); // TODO: (sql,callbackFunc[, userData])
    std::vector< Handle<Value> > vec(argc,Null());
    for( int i = 0; i < argc; ++i ) vec[i] = argv[i];
    return execute_impl( this, argc, &vec[0] );
}
Handle<Value> DBWrapper::execute( Arguments const & argv )
{
    return this->execute( argv, true );
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
    //return CursorWeakWrap::Instance().NewInstance(1, &jobj ); // crash in v8 :(
    return CursorWrapper::js_ctor->NewInstance(1,&jobj);
}

StmtWrapper * StmtWrapper::GetNative( Handle<Value> v )
{
    return WeakJSClassCreator<StmtWrapper>::GetNative(v);
}
StmtWrapper * StmtWrapper::GetNative( Local<Object> v )
{
    return WeakJSClassCreator<StmtWrapper>::GetSelf(v);
}




#define DB_ARG(HND) DBWrapper * db = CastFromJS<DBWrapper>( HND );            \
    if( ! db ) return ThrowException(String::New("Argument is not a sqlite3 handle!"));

#define SETUP_DBFWD(ARG_COND) ARGC; ASSERTARGS( ARG_COND ); DB_ARG(argv[0])


JS_WRAPPER(sq3_callback_noop)
{
    return Undefined();
}

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

JS_WRAPPER(sq3_sleep)
{
    ARGC; ASSERTARGS(1==argc);
    int ms = CastFromJS<int>( argv[0] );
    return CastToJS( ::sqlite3_sleep( ms ) );
}


JS_WRAPPER(sq3_exec)
{
    SETUP_DBFWD((argc>0));
    int ac = argc-1;
    std::vector< Handle<Value> > vec(ac,Null());
    for( int i = 1; i < ac; ++i ) vec[i-1] = argv[i];
    return execute_impl( db, ac, &vec[0] );
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
    return st->bindNum( JSToInt32(argv[1]), JSToInt32(argv[2]) );
}

JS_WRAPPER(sq3_bind_int64)
{
    SETUP_STMTFWD((argc==3));
    return st->bindNum( JSToInt32(argv[1]), JSToInt64(argv[2]) );
}

JS_WRAPPER(sq3_bind_double)
{
    SETUP_STMTFWD((argc==3));
    return st->bindNum( JSToInt32(argv[1]), JSToDouble(argv[2]) );
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
JS_WRAPPER(sq3_step)
{
    SETUP_STMTFWD((argc==1));
    return CastToJS( sqlite3_step( st->proxy.handle() ) );
}

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
    ARGC; ASSERTARGS((1==argc));
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
    ARGC; ASSERTARGS((1==argc));
    std::string s( CastFromJS<std::string>( argv[0] ) );
    if( ! s.empty() )
    {
        sql_escape_string( s );
    }
    s.insert(0,1,'\'');
    s.push_back('\'');
    return CastToJS( s );
}

JS_WRAPPER(sq3_quote_field)
{
    ARGC; ASSERTARGS((1==argc));
    return CastToJS( "`"+CastFromJS<std::string>( argv[0] )+"`" );
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
        DBWeakWrap & wr = DBWeakWrap::Instance();
        typedef DBWrapper DBW;
        wr.BindMemFunc< HV, &DBW::close >( "close" );
        wr.BindMemFunc< HV, &DBW::changes >( "changes" );
        wr.BindMemFunc< HV, &DBW::insertid >( "lastInsertID" );
        wr.BindMemFunc< HV, int, &DBW::setbusytimeout >( "setBusyTimeout" );
        wr.BindMemFunc< &DBW::execute >("execute");
        wr.BindMemFunc< HV, &DBW::errmsg >("errorMessage");
        wr.BindMemFunc< HV, &DBW::vacuum >("vacuum");
        wr.BindMemFunc< HV, &DBW::clear >("clear");
        wr.BindMemFunc< HV, std::string const &, &DBW::pragma >("execPragma");
        wr.BindMemFunc< HV, std::string const &, &DBW::executeT<int> >( "executeInt" );
        wr.BindMemFunc< HV, std::string const &, &DBW::executeT<int64_t> >( "executeInt64" );
        wr.BindMemFunc< HV, std::string const &, &DBW::executeT<double> >( "executeDouble" );
        wr.BindMemFunc< HV, std::string const &, &DBW::executeT<std::string> >( "executeString" );
        wr.Seal();
        wr.AddClassTo(sqobj);
        if( DBW::js_ctor.IsEmpty() )
        {
            DBW::js_ctor = Persistent<Function>::New( wr.CtorTemplate()->GetFunction() );
        }
    }


    {
        StmtWeakWrap & stmt = StmtWeakWrap::Instance();
        typedef StmtWrapper STW;
        stmt.BindMemFunc< HV, &STW::finalize >("finalize");
        stmt.BindMemFunc< &STW::prepare >("prepare");
        stmt.BindMemFunc< bool, &STW::isPrepared >("isPrepared");
        stmt.BindMemFunc< HV, int, &STW::bindNull >( "bindNull" );
        stmt.BindMemFunc< HV, int, int, &STW::bindNum<int> >( "bindInt" );
        stmt.BindMemFunc< HV, int, int64_t, &STW::bindNum<int64_t> >( "bindInt64" );
        stmt.BindMemFunc< HV, int, double, &STW::bindNum<double> >( "bindDouble" );
        stmt.BindMemFunc< HV, int, std::string const &, &STW::bindText >( "bindText" );
        stmt.BindMemFunc< HV, &STW::columnCount >( "columnCount" );
        stmt.BindMemFunc< HV, &STW::reset >( "reset" );
        stmt.BindMemFunc< HV, &STW::clearBindings >( "clearBindings" );
        stmt.BindMemFunc< HV, &STW::getCursor >( "getCursor" );
        stmt.BindMemFunc< HV, &STW::execute >( "execute" );
        stmt.Seal();
        stmt.AddClassTo(sqobj);
        if( STW::js_ctor.IsEmpty() )
        {
            STW::js_ctor = Persistent<Function>::New( stmt.CtorTemplate()->GetFunction() );
        }
    }

    {
        CursorWeakWrap & bind = CursorWeakWrap::Instance();
        typedef CursorWrapper CURW;
        bind.BindMemFunc< HV, &CURW::finalize >("finalize");
        bind.BindMemFunc< HV, &CURW::columnCount >( "columnCount" );
        bind.BindMemFunc< HV, &CURW::step >( "step" );
        bind.BindMemFunc< HV, &CURW::reset >( "reset" );
        bind.BindMemFunc< HV, int, &CURW::columnName >( "columnName" );
        bind.BindMemFunc< HV, &CURW::columnNames >( "columnNames" );
        bind.BindMemFunc< HV, int, &CURW::isNull >( "isNull" );
        bind.BindMemFunc< HV, int, &CURW::getT<int> >( "getInt" );
        bind.BindMemFunc< HV, int, &CURW::getT<int64_t> >( "getInt64" );
        bind.BindMemFunc< HV, int, &CURW::getT<double> >( "getDouble" );
        bind.BindMemFunc< HV, int, &CURW::getT<std::string> >( "getString" );
        bind.Seal();
        bind.AddClassTo(sqobj);
        if( CURW::js_ctor.IsEmpty() )
        {
            CURW::js_ctor = Persistent<Function>::New( bind.CtorTemplate()->GetFunction() );
        }
    }

    {
        CallbackWeakWrap & bind = CallbackWeakWrap::Instance();
        typedef CallbackWrapper CURW;
        Handle<Function> noop = FunctionTemplate::New(sq3_callback_noop)->GetFunction();
        bind.Set("func",noop);
        bind.Set("step",noop);
        bind.Set("final",noop);
        bind.Seal();
        bind.AddClassTo(sqobj);
        if( CURW::js_ctor.IsEmpty() )
        {
            CURW::js_ctor = Persistent<Function>::New( bind.CtorTemplate()->GetFunction() );
        }
    }


#define ADDFUNC(SUF) gl->Set(String::New("sqlite3_" # SUF), FunctionTemplate::New(sq3_ ## SUF)->GetFunction() )
    ADDFUNC(bind_double);
    ADDFUNC(bind_int);
    ADDFUNC(bind_int64);
    ADDFUNC(bind_null);
    ADDFUNC(bind_text);
    ADDFUNC(changes);
    ADDFUNC(clear_bindings);
    ADDFUNC(close);
    ADDFUNC(column_count);
    ADDFUNC(errmsg);
    ADDFUNC(exec);
    ADDFUNC(finalize);
    ADDFUNC(last_insert_rowid);
    ADDFUNC(open);
    ADDFUNC(prepare);
    ADDFUNC(reset);
    ADDFUNC(step);
    ADDFUNC(escape_sql);
    ADDFUNC(quote_field);
    ADDFUNC(quote_sql);
    ADDFUNC(sleep);
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
