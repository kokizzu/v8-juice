/**
   v8 JS wrappers for the whio i/o device library:

   http://fossil.wanderinghorse.net/repos/whio/

   Author: stephan beal (http://wanderinghorse.net/home/stephan/)

   License: Public Domain

*/
#include <iostream>
#include <sstream>
#include <vector>

/* only for debuggering */
#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

#include <v8/juice/forwarding.h>
#include <v8/juice/plugin.h>

#include "v8-whio.h"
#include "v8-whefs.h"

#include <v8/juice/ClassBinder.h>
#include <v8/juice/ToSource.h>

namespace v8 { namespace juice { namespace whio {

    const char * WhefsStrings::openDevice = "openDevice";
    const char * WhefsStrings::unlink = "unlink";
    const char * WhefsStrings::dumpToFile = "dumpToFile";
    const char * WhefsStrings::ls = "ls";
    const char * WhefsStrings::mkfs = "mkfs";


    //namespace juice = ::v8::juice;
    using namespace ::v8::juice::convert;

#define JSTR(X) String::New(X)
#define TOSS(X) return ::v8::ThrowException(JSTR(X)) /*juice::ThrowException(X)*/
#define WHIO_RTFM "RTFM: " v8_juice_HOME_PAGE "/wiki/PluginWhio"


    Persistent<Function> WHEFS::js_ctor;

    /**
       JS Usage:

       - WhEfs( IODevice dev ), where dev is an existing whefs filesystem.

       - WhEfs( string fn, bool writeMode ) opens a pre-existing whefs
       filesystem by filename.

       All forms throw on error.
    */
    WHEFSOps::WrappedType * WHEFSOps::Ctor( Arguments const & argv,
                                            std::string & exceptionText)
    {
        const int argc = argv.Length();
        whefs_fs * fs = 0;
        if( (1 == argc) && argv[0]->IsExternal() )
        { // Internal use: passing a whio_dev instance from a factory function.
            Local< External > ex( External::Cast(*argv[0]) );
            fs = bind::GetBoundNative<whefs_fs>( ex->Value() );
            if( ! fs )
            {
                exceptionText = "Internal ctor got incorrect arguments!";
            }
            CERR << ClassName()<<" ctor passing on @"<<fs<<'\n';
            return fs;
        }
        Handle<Value> a0 = argv[0];
        int rc = 0;
        if( a0->IsString() )
        { // ctor(String filename, bool writeMode)
            std::string fn( JSToStdString(a0) );
            bool rw = (argc>1) ? argv[1]->BooleanValue() : false;
            rc = whefs_openfs( fn.c_str(), &fs, rw );
            if( whefs_rc.OK == rc )
            {
                argv.This()->Set(JSTR(WhioStrings::fileName),CastToJS(fn));
            }
        }
        else
        { // ctor(IODevice existingWhefsFS)
            IODevice::type * dev = CastFromJS< IODevice >( a0 );
            if( ! dev )
            {
                exceptionText = "First argument to the constructor must be an IODevice!";
                return 0;
            }
            rc = whefs_openfs_dev( dev, &fs, false );
            // fixme: the vfs should optionally take over ownership of dev, but we
            // need to record that ownership so that we can clean up
            // properly.
        }
        if( whefs_rc.OK != rc )
        {
            std::ostringstream os;
            os << "whefs_openfs_dev() failed with code "<<rc<<'!';
            exceptionText = os.str();
            return 0;
        }
        if( fs )
        {
            ::v8::juice::cleanup::AddToCleanup(fs, cleanup_callback );
        }
        else if( exceptionText.empty() )
        {
            exceptionText = "Constructor requires: (IODevice src) or (String file, bool writeMode)";
            return 0;
        }
        CERR << ClassName()<<" ctor passing on @"<<fs<<'\n';
        return fs;
    }
    
    void WHEFSOps::Dtor( WHEFSOps::WrappedType * obj )
    {
        CERR << ClassName()<<" dtor passing on @"<<obj<<'\n';
        if( obj )
        {
            ::v8::juice::cleanup::RemoveFromCleanup(obj);
            whefs_fs_finalize(obj);
        }
    }

    void WHEFSOps::cleanup_callback( void * obj )
    {
        Dtor( static_cast<WrappedType*>(obj) );
    }

#define ARGS(COND) const int argc = argv.Length(); if( !(COND) ) TOSS("argument assertion failed: " # COND)

#define EFSH(T,H) T::type * fs = CastFromJS< T >( H )
#define EFSHT(T,H) EFSH(T,H); if( ! fs ) TOSS("Native device pointer not found (maybe already closed?)!")
#define EFSTHIS EFSHT(WHEFS,argv.This())

    static Handle<Value> whefs_close(const Arguments& argv)
    {
        EFSTHIS;
	//CERR << "WHEFS.close() == " << fs << '\n';
	WeakJSClassCreator<WHEFS>::DestroyObject( argv.This() );
	return Undefined();
    }

    static Handle<Value> whefs_open_dev(const Arguments& argv)
    {
        ARGS(2==argc);
        EFSTHIS;
        std::string fname( JSToStdString(argv[0]) );
        bool rw = argv[1]->BooleanValue();
	//CERR << "WHEFS.openDevice("<<fname<<", "<<rw<<") == " << fs << '\n';
        whio_dev * dev = whefs_dev_open( fs, fname.c_str(), rw );
        if( ! dev )
        {
            std::ostringstream os;
            os << "openDevice("<<fname<<", "<<rw<<") failed for unknown reasons!";
            TOSS(os.str().c_str());
        }
        bind::BindNative( dev, dev );
        Local<External> a0( External::New(dev) );
        Handle<Value> av[] = { a0 };
        Handle<Object> jdev = IODevice::js_ctor->NewInstance( 1, av );
        if( jdev.IsEmpty() || !jdev->IsObject() )
        {
            bind::UnbindNative( dev, dev );
            dev->api->finalize(dev);
            return jdev;
        }
        jdev->Set(JSTR(WhioStrings::fileName),CastToJS(fname));
        jdev->Set(JSTR(WhioStrings::canWrite), Boolean::New(rw), v8::ReadOnly );
        jdev->Set(JSTR(WhioStrings::canRead), Boolean::New(true), v8::ReadOnly );
	return jdev;
    }

    static Handle<Value> whefs_unlink(const Arguments& argv)
    {
        ARGS(1==argc);
        EFSTHIS;
        if( ! argv[0]->IsString() )
        {
            TOSS("First argument must be a pseudofile name (string).");
        }
        std::string fname( JSToStdString(argv[0]) );
        int rc = whefs_unlink_filename( fs, fname.c_str() );
        return CastToJS(rc);
    }

    /**
       JS usage:

       var fs = whefs.mkfs( "/path/to/file", {options} );

       Where options should have these properties:

       blockSize, inodeCount, blockCount, filenameLength

       each corresponds to a whefs_fs_options entry.
    */
    static Handle<Value> whefs_mkfs(const Arguments& argv)
    {
        ARGS((argc == 1) || 2==argc);
        std::string fname( JSToStdString( argv[0] ) );
        whefs_fs_options op = whefs_fs_options_default;
        if( (argc > 1) && argv[1]->IsObject() )
        {
            Handle<Object> jop( Object::Cast(*argv[1]) );
            Local<String> key;
#define CHECK(K,MEM,T) key = String::New(K);                            \
            if( jop->Has( key ) ) op.MEM = convert::CastFromJS<T>( jop->Get(key))
            CHECK("blockSize",block_size,uint32_t);
            CHECK("inodeCount",inode_count,whefs_id_type);
            CHECK("blockCount",block_count,whefs_id_type);
            CHECK("filenameLength",filename_length,uint16_t);
#undef CHECK
        }
        whefs_fs * fs = 0;
        int rc = whefs_mkfs( fname.c_str(), &op, &fs );
        if( whefs_rc.OK != rc )
        {
            Handle<Value> jopt = Null();
            if( argc>1 ) jopt = argv[1];
            std::ostringstream os;
            os << "whefs_mkfs("<<fname<<", "<<ToSource(jopt)<<", &fs) failed with rc "<<rc<<'!';
            TOSS(os.str().c_str());
        }

        bind::BindNative( fs, fs );
        Local<External> a0( External::New(fs) );
        Handle<Value> av[] = { a0 };
        Handle<Object> jfs = WHEFS::js_ctor->NewInstance( 1, av );
        if( jfs.IsEmpty() || !jfs->IsObject() )
        {
            bind::UnbindNative( fs, fs );
            whefs_fs_finalize( fs );
            return jfs;
        }
        jfs->Set(JSTR(WhioStrings::canWrite), Boolean::New(true), v8::ReadOnly );
        jfs->Set(JSTR(WhioStrings::canRead), Boolean::New(true), v8::ReadOnly );
        jfs->Set(JSTR(WhioStrings::fileName),CastToJS(fname));
        return jfs;
    }

    static Handle<Value> whefs_dump_to_file(const Arguments& argv)
    {
        ARGS((argc == 1));
        EFSTHIS;
        std::string fname( JSToStdString( argv[0] ) );
        int rc = whefs_fs_dump_to_filename( fs, fname.c_str() );
        return CastToJS(rc);
    }

    static Handle<Value> whefs_ls(const Arguments& argv)
    {
        ARGS((argc == 1));
        EFSTHIS;
        std::string pat( JSToStdString(argv[0]) );
        whefs_id_type count = 0;
        whefs_string * ls = whefs_ls( fs, pat.c_str(), &count );
        if( ! ls ) return Null();
        whefs_string * head = ls;
        Handle<Array> ar( Array::New(count) );
        int pos = 0;
        for( ; ls; ++pos )
        {
            ar->Set( Integer::New(pos), CastToJS( static_cast<char const *>(ls->string) ) );
            ls = ls->next;
        }
        whefs_string_finalize( head, true );
        return ar;
    }
    static Handle<Value> whefs_size(const Arguments& argv)
    {
        ARGS((argc == 0));
        EFSTHIS;
        return CastToJS( whefs_fs_calculate_size( whefs_fs_options_get(fs) ) );
    }

#undef EFSH
#undef EFSHT
#undef EFSTHIS
#undef DEVHT
#undef DEVHV
#undef DEVH
#undef DEVTHIS

    Handle<Value> SetupWhefsClasses(const Handle<Object> target )
    {
	HandleScope v8scope;
	Handle<Object> whefs = Object::New();
	target->Set(JSTR("whefs"),whefs);
        {
            Handle<Object> whefsrc = Object::New();
            whefs->Set(JSTR("rc"),whefsrc);

#define SET(K,V) whefsrc->Set(JSTR(K), Integer::New(V), v8::ReadOnly ); \
            whefsrc->Set(Integer::New(V), JSTR(K), v8::ReadOnly )
            SET("OK", whefs_rc.OK );
            SET("ArgError",whefs_rc.ArgError);
            SET("IOError",whefs_rc.IOError);
            SET("AllocError",whefs_rc.AllocError);
            SET("BadMagicError",whefs_rc.BadMagicError);
            SET("InternalError",whefs_rc.InternalError);
            SET("RangeError",whefs_rc.RangeError);
            SET("AccessError",whefs_rc.AccessError);
            SET("ConsistencyError",whefs_rc.ConsistencyError);
            SET("NYIError",whefs_rc.NYIError);
            SET("UnsupportedError",whefs_rc.UnsupportedError);
            SET("SizeTError",-1);
#undef SET
        }

	////////////////////////////////////////////////////////////
	// WHEFS class:
	{
	    ClassBinder<WHEFS> binder( whefs );
            binder
                .Set(WhioStrings::close, whefs_close)
                .Set(WhefsStrings::openDevice, whefs_open_dev)
                .Set(WhefsStrings::unlink, whefs_unlink)
                .Set(WhefsStrings::dumpToFile, whefs_dump_to_file)
                .Set(WhefsStrings::ls, whefs_ls)
                .Set(WhioStrings::size, whefs_size)
                ;
            Handle<Function> ctor = binder.Seal();
            Handle<Function> fmkfs( FunctionTemplate::New(whefs_mkfs)->GetFunction() );
            //ctor->Set(JSTR("mkfs"), fmkfs );
            whefs->Set(JSTR(WhefsStrings::mkfs), fmkfs );
            if( WHEFS::js_ctor.IsEmpty() )
            {
                WHEFS::js_ctor = Persistent<Function>::New( ctor );
            }
	}
	return whefs;
    }

#undef JSTR
#undef TOSS
#undef ARGS
#undef WHIO_RTFM

    /** Plugin initializer for v8-juice. */
    V8_JUICE_PLUGIN_STATIC_INIT(SetupWhefsClasses);

}}} // namespaces
