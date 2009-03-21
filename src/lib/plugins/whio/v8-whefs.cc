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

    //namespace juice = ::v8::juice;
    using namespace ::v8::juice::convert;

#define JSTR(X) String::New(X)
#define TOSS(X) return ::v8::ThrowException(JSTR(X)) /*juice::ThrowException(X)*/
#define WHIO_RTFM "RTFM: " v8_juice_HOME_PAGE "/wiki/PluginWhio"


    Persistent<Function> WHEFS::js_ctor;

    WHEFSOps::WrappedType * WHEFSOps::Ctor( Arguments const & argv,
                                            std::string & exceptionText)
    {
        const int argc = argv.Length();
        whefs_fs * fs = 0;
        if( (1 == argc) && argv[0]->IsExternal() )
        {
            // we hope it is a whio_dev passed by a "friendly" factory function.
            // fixme: make this a safe operation if someone passes in a different
            // (void *).
            Local< External > ex( External::Cast(*argv[0]) );
            fs = static_cast<whefs_fs*>( ex->Value() );
            if( ! fs )
            {
                exceptionText = "Internal ctor got incorrect arguments!";
            }
            CERR << ClassName()<<" ctor passing on @"<<fs<<'\n';
            return fs;
        }
        if( (argc != 1) )
        {
            exceptionText = "Constructor requires: (IODevice src)";
            return 0;
        }
        IODevice::type * dev = CastFromJS< IODevice >( argv[0] );
        if( ! dev )
        {
            exceptionText = "First argument to the constructor must be an IODevice!";
            return 0;
        }
        int rc = whefs_openfs_dev( dev, &fs, false );
        // fixme: the vfs should take over ownership of dev, but we
        // need to record that ownership so that we can clean up
        // properly.
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
        Local<External> a0( External::New(dev) );
        Handle<Value> av[] = { a0 };
        Handle<Object> jdev = IODevice::js_ctor->NewInstance( 1, av );
        if( jdev.IsEmpty() || !jdev->IsObject() )
        {
            dev->api->finalize(dev);
            return jdev;
        }
	return jdev;
    }

    static Handle<Value> whefs_mkfs(const Arguments& argv)
    {
        ARGS((argc == 1) || 2==argc);
        std::string fname( JSToStdString( argv[0] ) );
        whefs_fs_options op = WHEFS_FS_OPTIONS_DEFAULT;
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

        Local<External> a0( External::New(fs) );
        Handle<Value> av[] = { a0 };
        Handle<Object> jfs = WHEFS::js_ctor->NewInstance( 1, av );
        if( jfs.IsEmpty() || !jfs->IsObject() )
        {
            whefs_fs_finalize( fs );
            return jfs;
        }
        return jfs;
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
                .Set("close", whefs_close)
                .Set("openDevice", whefs_open_dev)
                ;
            Handle<Function> ctor = binder.Seal();
            //ctor->Set(JSTR("mkfs"), FunctionTemplate::New(whefs_mkfs)->GetFunction() );
            whefs->Set(JSTR("mkfs"), FunctionTemplate::New(whefs_mkfs)->GetFunction() );
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
