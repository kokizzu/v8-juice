//    Author: stephan beal <stephan@s11n.net>
//    License: Public Domain

#include <iostream>
#include <sstream>
#include <vector>

#include <v8.h>
#include <v8/juice/convert.h>
#include <v8/juice/plugin.h>
#include <v8/juice/cleanup.h>

#include "whio_amalgamation.h"

/* only for debuggering */
#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

namespace v8 { namespace juice { namespace whio {
    using namespace ::v8::bind;
    using namespace ::v8::convert;
#define JSTR(X) String::New(X)
#define TOSS(X) ThrowException(JSTR(X))
    static const void * bind_cx() { static const int x=42;return &x;}

    //! Internal util to reduce code duplication elsewhere: unbinds and deletes obj.
    static void dev_cleanup( void * obj, whio_dev * dev )
    {
	//CERR << "dev_cleanup( void@"<<obj<<",  dev@"<<dev<<")\n";
	if( dev )
	{
	    UnbindNative( bind_cx(), obj, dev );
	    dev->api->finalize(dev);
	}
    }

    /**
       Destructor for use with v8::juice::cleanup::AddToCleanup().
       obj MUST be-a (whio_dev*).
    */
    static void dev_cleanup( void * obj )
    {
	return dev_cleanup( obj, static_cast<whio_dev*>( obj ) );
    }

    static void dev_dtor(Persistent< Value > object, void *parameter)
    {
	::v8::juice::cleanup::RemoveFromCleanup(parameter);
	dev_cleanup(parameter);
	object.Dispose();
	object.Clear();
    }

    static Persistent<Object> dev_wrap( Handle<Object> _self, whio_dev * value )
    {
	::v8::juice::cleanup::AddToCleanup(value, dev_cleanup);
	BindNative( bind_cx(), value, value );
	Persistent<Object> self( Persistent<Object>::New(_self) );
	self.MakeWeak( value, dev_dtor );
	self->SetInternalField(0,External::New(value));
	//CERR << "Wrapped whio_dev @"<<value<<" in a Persistent<Object>.\n";
	return self;
    }

    static Handle<Value> dev_ctor(const Arguments& argv)
    {
	if (!argv.IsConstructCall()) 
	{
	    return ThrowException(String::New("Cannot call whio_dev constructor as function!"));
	}

	const int argc = argv.Length();
	if( (1 == argc) && argv[0]->IsExternal() )
	{ // called by internal init routine which already allocated a whio_dev:
	    External * ex = External::Cast( *argv[0] );
	    if( ex )
	    {
		return dev_wrap( argv.This(), static_cast<whio_dev*>(ex->Value()) );
	    }
	    else
	    {
		return ThrowException(String::New("First argument to ctor failed External::Cast()"));
	    }
	}
	if( 2 == argc )
	{ // (string filename, bool writeMode)
	    std::string fname = JSToStdString(argv[0]);
	    bool writeMode = JSToBool(argv[1]);
	    char const * mode = (writeMode) ? "w+b" : "rb";
	    whio_dev * dev = whio_dev_for_filename( fname.c_str(), mode );
	    //CERR << "whio_dev_for_filename('"<<fname<<"','"<<mode<<"') dev@"<<dev<<'\n';
	    if( ! dev )
	    {
		std::ostringstream msg;
		msg << "whio_dev_for_filename('"<<fname<<"','"<<mode<<"') failed!";
		TOSS(msg.str().c_str());
	    }
	    return dev_wrap( argv.This(), dev );
	}
	//return argv.This(); // we use this so the pseudo-ctors can get a proper instance 
	return TOSS("Invalid arguments!");
	/**
	   Cases to handle:

	   - filename
	   - ":memory:" special filename (sqlite3-style)
	   - read or write mode
	*/
    }
    static whio_dev * dev_cast( Local< Value > v )
    {
	if( v.IsEmpty() || ! v->IsObject() ) return 0;
	Local<Object> o( Object::Cast( *v ) );
	return GetBoundNative<whio_dev>( bind_cx(), o->GetInternalField(0) );
    }

#define ARGS(FUNC,COND) const int argc = argv.Length(); if( !(COND) ) TOSS(FUNC "(): argument assertion failed: " # COND)
#define DEVH(H) whio_dev * dev = dev_cast( H )
#define DEVHT(H) DEVH(H); if( ! dev ) TOSS("Native whio_dev pointer is null!")
#define DEVHV(H) DEVH(H); if( ! dev ) return
#define DEVTHIS DEVHT(argv.This())


    static Handle<Value> dev_write(const Arguments& argv)
    {
	ARGS("write",(argc==1 || argc==2));
	DEVTHIS;
	std::string data( JSToStdString(argv[0]) );
	uint32_t l = (argc>1)
	    ? CastFromJS<uint32_t>( argv[1] )
	    : 0;
	if( 0 == l ) l = data.size();
	if( l > data.size() )
	{
	    TOSS("Input data is not long enough to satisfy write request!");
	}
	return CastToJS<uint32_t>( dev->api->write( dev, data.data(), l ) );
    }
    static Handle<Value> dev_read(const Arguments& argv)
    {
	ARGS("read",(argc==1));
	DEVTHIS;
	uint32_t l = CastFromJS<uint32_t>( argv[0] );
	if( 0 == l ) TOSS("Number of bytes to read must be greater than 0!");
	std::vector<char> data( l, '\0' );
	uint32_t rlen = dev->api->read( dev, &data[0], l );
	return String::New( &data[0], rlen );
    }

    static Handle<Value> dev_close(const Arguments& argv)
    {
	ARGS("close",(argc==0));
	DEVTHIS;
	dev->api->close(dev);
	argv.This()->SetInternalField(0,External::New(0));
	return Null();
    }


    static Handle<Value> dev_error(const Arguments& argv)
    {
	ARGS("error",(argc==0));
	DEVTHIS;
	return IntToJS( dev->api->error( dev ) );
    }

    static Handle<Value> dev_clear_error(const Arguments& argv)
    {
	ARGS("clearError",(argc==0));
	DEVTHIS;
	return IntToJS( dev->api->clear_error( dev ) );
    }

    static Handle<Value> dev_eof(const Arguments& argv)
    {
	ARGS("eof",(argc==0));
	DEVTHIS;
	return IntToJS( dev->api->eof(dev) );
    }

    static Handle<Value> dev_tell(const Arguments& argv)
    {
	ARGS("tell",(argc==0));
	DEVTHIS;
	return ULongToJS( dev->api->tell( dev ) );
    }

    static Handle<Value> dev_seek(const Arguments& argv)
    {
	ARGS("seek",((argc==1) || (argc==2)));
	DEVTHIS;
	uint32_t pos = JSToULong( argv[0] );
	const int whence = (argc>1) ? JSToInt( argv[1] ) : SEEK_SET;
	switch( whence )
	{
	  case SEEK_SET:
	  case SEEK_CUR:
	  case SEEK_END:
	      break;
	  default:
	      TOSS("The whence argument must be one of SEEK_SET, SEEK_CUR, or SEEK_END!");
	};
	return ULongToJS( dev->api->seek( dev, pos, whence) );
    }

    static Handle<Value> dev_flush(const Arguments& argv)
    {
	ARGS("flush",(argc==0));
	DEVTHIS;
	return IntToJS( dev->api->flush( dev ) );
    }

    static Handle<Value> dev_truncate(const Arguments& argv)
    {
	ARGS("truncate",(argc==1));
	DEVTHIS;
	uint32_t len = JSToULong( argv[0] );
	return IntToJS( dev->api->truncate(dev, len) );
    }

    static Handle<Value> dev_size(const Arguments& argv)
    {
	ARGS("size",(argc==0));
	DEVTHIS;
	return ULongToJS( whio_dev_size( dev ) );
    }

    static Handle<Value> dev_rewind(const Arguments& argv)
    {
	ARGS("rewind",(argc==0));
	DEVTHIS;
	return IntToJS( whio_dev_rewind( dev ) );
    }

#if 0
    static Handle<Value> dev_for_file(const Arguments& argv)
    {
	ARGS("forFile",(argc==2));
	std::string fname = JSToStdString(argv[0]);
	bool writeMode = JSToBool(argv[1]);
	char const * mode = (writeMode) ? "w+b" : "rb";
	whio_dev * dev = whio_dev_for_filename( fname.c_str(), mode );
	if( ! dev )
	{

	    std::ostringstream msg;
	    msg << "whio_dev_for_filename('"<<fname<<"','"<<mode<<"') failed!";
	    TOSS(msg.str().c_str());
	}
	// how the hell to get the proper IODevice instance/ctor from here?
	Handle<Function> ctor( Function::Cast( *argv.Holder() ) );
	Handle<Object> self = ctor->NewInstance();// hope this doesn't call the ctor!
	return dev_wrap( self, dev );
    }
#endif

#undef ARGS
#undef DEVHT
#undef DEVHV
#undef DEVH
#undef DEVTHIS
    Handle<Value> SetupWhioClasses(const Handle<Object> target )
    {
	// Set up prototype:
	/** Reminder: code order is very important here. See:
	   http://code.google.com/p/v8/issues/detail?id=262
	*/
	Handle<FunctionTemplate> devCtorTmpl = FunctionTemplate::New(dev_ctor);
	Local<ObjectTemplate> devInst = devCtorTmpl->InstanceTemplate();
	devInst->SetInternalFieldCount(1);
	Handle<Function> devCtor( devCtorTmpl->GetFunction() );

	devInst->Set("SEEK_SET",Integer::New(SEEK_SET));
	devInst->Set("SEEK_END",Integer::New(SEEK_END));
	devInst->Set("SEEK_CUR",Integer::New(SEEK_CUR));

#if 0
#define FUNC(N,F) devCtor->Set(JSTR(N), FunctionTemplate::New( F )->GetFunction() )
	FUNC("forFile",dev_for_file);
#undef FUNC
#endif

	Handle<Object> whio = Object::New();
	target->Set(JSTR("whio"),whio);
	whio->Set(JSTR("IODevice"), devCtor);
#define FUNC(N,F) devInst->Set(JSTR(N), FunctionTemplate::New( F )->GetFunction() )
	FUNC("read",dev_read);
	FUNC("write",dev_write);
	FUNC("close",dev_close);
	FUNC("error",dev_error);
	FUNC("clearError",dev_clear_error);
	FUNC("eof",dev_eof);
	FUNC("tell",dev_tell);
	FUNC("seek",dev_seek);
	FUNC("flush",dev_flush);
	FUNC("truncate",dev_truncate);
	FUNC("size",dev_size);
	FUNC("rewind",dev_rewind);
#undef FUNC	    
#if 0
	objt->Set(JSTR("read"), FunctionTemplate::New( dev_read ) );
	// Install ctor:
	target->Set(String::New("IODevice"), pffunc );

	Handle<Object> shared = Object::New();
	pffunc->Set(String::New("shared"), shared );

	{
	    // Plugins path:
	    Handle<Object> pfobj = pffunc->NewInstance( 1, &pfex );
	    shared->Set(String::New("plugins"), pfobj );
	}

	{
	    // Includes path:
	    Handle<Value> tmp = JSTR(".:/usr/lib/v8/plugins");
	    Handle<Object> pfInc = pffunc->NewInstance( 1, &tmp );
	    shared->Set(String::New("include"), pfInc );
	}
#endif
	return whio;
    }
#undef JSTR
#undef TOSS

    namespace {
	static void InitPlugin()
	{
	    using v8::juice::plugin::LoadPluginScope;
	    LoadPluginScope * sc = LoadPluginScope::Current();
	    if( ! sc ) return; // not called by LoadModule()
	    sc->SetReturnValue( SetupWhioClasses( sc->Target() ) );
	}
	static bool dll_initializer = (InitPlugin(),true);
    }

}}} // namespaces
