/**
   v8 JS wrappers for the whio i/o device library:

   http://fossil.wanderinghorse.net/repos/whio/

   Author: stephan beal (http://wanderinghorse.net/home/stephan/)

   License: Public Domain

*/
#include <iostream>
#include <sstream>
#include <vector>

#include <v8.h>
#include <v8/juice/juice.h>
#include <v8/juice/convert.h>
#include <v8/juice/plugin.h>
#include <v8/juice/cleanup.h>

#include "whio_amalgamation.h"

/* only for debuggering */
#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

#include <set>
namespace v8 { namespace juice { namespace whio {
    namespace bind = ::v8::juice::bind;
    //namespace juice = ::v8::juice;
    using namespace ::v8::juice::convert;
#define JSTR(X) String::New(X)
#define TOSS(X) return juice::ThrowException(X)

    /**
       Internal binding context for BindNative() and friends.
    */
    static const void * bind_cx() { static const int x=42;return &x;}

    /** Internal util to reduce code duplication elsewhere: unbinds
	dev and deletes obj using obj->api->finalize(obj).
     */
    template <typename T>
    static void dev_cleanup( void * obj, T * dev )
    {
	::v8::juice::cleanup::RemoveFromCleanup(obj);
	//CERR << "dev_cleanup( void@"<<obj<<",  dev@"<<dev<<")\n";
	bind::UnbindNative( bind_cx(), obj, dev );
	if( dev )
	{
	    dev->api->finalize(dev);
	}
    }

    /**
       Destructor for use with v8::juice::cleanup::AddToCleanup().
       obj MUST be-a (whio_dev*) or (whio_stream*).
    */
    template <typename T>
    static void dev_cleanup( void * obj )
    {
	dev_cleanup( obj, static_cast<T*>( obj ) );
    }

    /**
       Destructor callback for Pesistent::MakeWeak(). T must be
       one of whio_dev or whio_stream.
    */
    template <typename T>
    static void dev_dtor(Persistent< Value > object, void *parameter)
    {
	dev_cleanup<T>(parameter);
	object.Dispose();
	object.Clear();
    }

    /**
       Makes a persistent weak pointer for the given object, using
       _self as the JS-side instance of the object. T must be one of
       whio_dev or whio_stream.
    */
    template <typename T>
    static Persistent<Object> dev_wrap( Handle<Object> _self, T * obj )
    {
	::v8::juice::cleanup::AddToCleanup(obj, dev_cleanup<T> );
	bind::BindNative( bind_cx(), obj, obj );
	Persistent<Object> self( Persistent<Object>::New(_self) );
	self.MakeWeak( obj, dev_dtor<T> );
	self->SetInternalField(0,External::New(obj));
	//CERR << "Wrapped device @"<<obj<<" in a Persistent<Object>.\n";
	return self;
    }

    /**
       ctor for IODevice class.
    */
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
		return juice::ThrowException("First argument to ctor failed External::Cast()");
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
		return juice::ThrowException("whio_dev_for_filename('%s','%s') failed!",
					     fname.c_str(), mode );
	    }
	    return dev_wrap( argv.This(), dev );
	}
	//return argv.This(); // we use this so the pseudo-ctors can get a proper instance 
	TOSS("Invalid arguments!");
	/**
	   Cases to handle:

	   - filename
	   - ":memory:" special filename (sqlite3-style)
	   - read or write mode
	*/
    }
    /**
       Casts v to (T*). T must be one of whio_dev or whio_stream.
    */
    template <typename T>
    static T * dev_cast( Local< Value > v )
    {
	if( v.IsEmpty() || ! v->IsObject() ) return 0;
	Local<Object> o( Object::Cast( *v ) );
	T * obj = bind::GetBoundNative<T>( bind_cx(), o->GetInternalField(0) );
#if 0
	if( obj )
	{
	    if(!LameAssKludge<T>().count(obj))
	    {
		bind::UnbindNative<T>( bind_cx(), obj, obj );
		return 0;
	    }
	}
#endif
	return obj;
    }

#define ARGS(FUNC,COND) const int argc = argv.Length(); if( !(COND) ) TOSS(FUNC "(): argument assertion failed: " # COND)
#define DEVH(T,H) T * dev = dev_cast<T>( H )
#define DEVHT(T,H) DEVH(T,H); if( ! dev ) TOSS("Native device pointer has already been destroyed!")
#define DEVHV(T,H) DEVH(T,H); if( ! dev ) return
#define DEVTHIS(T) DEVHT(T,argv.This())


    /**
       write() impl for DevT, which must be one of whio_dev or whio_stream.

       If allowWrite is false then this function throws when called. Set it to
       false for the InStream class.
    */
    template <typename DevT,bool allowWrite>
    static Handle<Value> dev_write(const Arguments& argv)
    {
	if( ! allowWrite )
	{
	    return juice::ThrowException("This device does not allow writing!");
	}
	else
	{
	    ARGS("write",(argc==1 || argc==2));
	    DEVTHIS(DevT);
	    std::string data( JSToStdString(argv[0]) );
	    uint32_t l = (argc>1)
		? CastFromJS<uint32_t>( argv[1] )
	    : 0;
	    if( 0 == l ) l = data.size();
	    if( l > data.size() )
	    {
		return juice::ThrowException("write(input,%u): input data is too short (%u bytes) for write request for %u bytes!",
					     l, data.size(), l);
	    }
	    return CastToJS<uint32_t>( dev->api->write( dev, data.data(), l ) );
	}
    }

    /**
       read() impl for DevT, which must be one of whio_dev or whio_stream.

       If allowRead is false then this function throws when called. Set it to
       false for the OutStream class.
    */
    template <typename DevT,bool allowRead>
    static Handle<Value> dev_read(const Arguments& argv)
    {
	if( ! allowRead )
	{
	    return juice::ThrowException("This device does not allow reading!");
	}
	else
	{
	    ARGS("read",(argc==1));
	    DEVTHIS(DevT);
	    uint32_t l = CastFromJS<uint32_t>( argv[0] );
	    if( 0 == l ) TOSS("Number of bytes to read must be greater than 0!");
	    std::vector<char> data( l, '\0' );
	    uint32_t rlen = dev->api->read( dev, &data[0], l );
	    return String::New( &data[0], rlen );
	}
    }

    /**
       close() impl for DevT, which must be one of whio_dev or whio_stream.
    */
    template <typename DevT>
    static Handle<Value> dev_close(const Arguments& argv)
    {
	ARGS("close",(argc==0));
	DEVTHIS(DevT);
	argv.This()->SetInternalField(0,Null()); // is not working?
	dev_cleanup( dev, dev );
	return Null();
    }

    /**
       whio_dev_api::close() wrapper.
    */
    static Handle<Value> dev_error(const Arguments& argv)
    {
	ARGS("error",(argc==0));
	DEVTHIS(whio_dev);
	return IntToJS( dev->api->error( dev ) );
    }

    /**
       whio_dev_api::clear_error() wrapper.
    */
    static Handle<Value> dev_clear_error(const Arguments& argv)
    {
	ARGS("clearError",(argc==0));
	DEVTHIS(whio_dev);
	return IntToJS( dev->api->clear_error( dev ) );
    }

    /**
       whio_dev_api::eof() wrapper.
    */
    static Handle<Value> dev_eof(const Arguments& argv)
    {
	ARGS("eof",(argc==0));
	DEVTHIS(whio_dev);
	return IntToJS( dev->api->eof(dev) );
    }

    /**
       whio_dev_api::tell() wrapper.
    */
    static Handle<Value> dev_tell(const Arguments& argv)
    {
	ARGS("tell",(argc==0));
	DEVTHIS(whio_dev);
	return ULongToJS( dev->api->tell( dev ) );
    }

    /**
       whio_dev_api::seek() wrapper.
    */
    static Handle<Value> dev_seek(const Arguments& argv)
    {
	ARGS("seek",((argc==1) || (argc==2)));
	DEVTHIS(whio_dev);
	uint32_t pos = JSToULong( argv[0] );
	const int whence = (argc>1) ? JSToInt( argv[1] ) : SEEK_SET;
	switch( whence )
	{
	  case SEEK_SET:
	  case SEEK_CUR:
	  case SEEK_END:
	      break;
	  default:
	      TOSS("The second argument to seek() must be one of SEEK_SET, SEEK_CUR, or SEEK_END!");
	};
	return ULongToJS( dev->api->seek( dev, pos, whence) );
    }

    /**
       flush() impl for DevT, which must be one of whio_dev or whio_stream.
    */
    template <typename DevT>
    static Handle<Value> dev_flush(const Arguments& argv)
    {
	ARGS("flush",(argc==0));
	DEVTHIS(DevT);
	return IntToJS( dev->api->flush( dev ) );
    }

    /**
       whio_dev_api::truncate() wrapper.
    */
    static Handle<Value> dev_truncate(const Arguments& argv)
    {
	ARGS("truncate",(argc==1));
	DEVTHIS(whio_dev);
	uint32_t len = JSToULong( argv[0] );
	return IntToJS( dev->api->truncate(dev, len) );
    }

    /**
       whio_dev_size() wrapper.
    */
    static Handle<Value> dev_size(const Arguments& argv)
    {
	ARGS("size",(argc==0));
	DEVTHIS(whio_dev);
	return ULongToJS( whio_dev_size( dev ) );
    }

    /**
       whio_dev_rewind() wrapper.
    */
    static Handle<Value> dev_rewind(const Arguments& argv)
    {
	ARGS("rewind",(argc==0));
	DEVTHIS(whio_dev);
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

	    return juice::ThrowException( "whio_dev_for_filename('%s','%s') failed!",
					  fname.c_str(), mode );
	}
	// how the hell to get the proper IODevice instance/ctor from here?
	Handle<Function> ctor( Function::Cast( *argv.Holder() ) );
	Handle<Object> self = ctor->NewInstance();// hope this doesn't call the ctor!
	return dev_wrap( self, dev );
    }
#endif

    /**
       ctor for InStream (if writeMode==false) and OutStream (if writeMode==true).
    */
    template <bool writeMode>
    static Handle<Value> stream_ctor(const Arguments& argv)
    { 
	/**
	   TODO:

	   ctor( whio_dev ) = whio_stream_for_dev(). The problem here is the
	   lifetime of the parent device.

	   Figure out how to consolidate this with the dev_dtor(), as it's 95%
	   the same code.
	*/
	if (!argv.IsConstructCall()) 
	{
	    return ThrowException(String::New("Cannot call whio_stream constructor as function!"));
	}

	const int argc = argv.Length();
	if( (1 == argc) && argv[0]->IsExternal() )
	{ // called by internal init routine which already allocated a whio_stream:
	    External * ex = External::Cast( *argv[0] );
	    if( ex )
	    {
		return dev_wrap( argv.This(), static_cast<whio_stream*>(ex->Value()) );
	    }
	    else
	    {
		return juice::ThrowException("First argument to ctor failed External::Cast()");
	    }
	}
	if( argc >=1 )
	{ // (string filename [, truncate=true])
	    std::string fname = JSToStdString(argv[0]);
	    bool trunc = (argc>1) ? argv[1]->BooleanValue() : true;
	    char const * mode = (writeMode) ? (trunc ? "wb" : "ab") : "rb";
	    whio_stream * dev = whio_stream_for_filename( fname.c_str(), mode );
	    //CERR << "whio_stream_for_filename('"<<fname<<"','"<<mode<<"') dev@"<<dev<<'\n';
	    if( ! dev )
	    {
		return juice::ThrowException("whio_stream_for_filename('%s','%s') failed!",
					     fname.c_str(), mode );
	    }
	    return dev_wrap( argv.This(), dev );
	}
	//return argv.This(); // we use this so the pseudo-ctors can get a proper instance 
	TOSS("Invalid arguments!");
    }

    /** whio_stream_api::isgood(). */
    static Handle<Value> stream_isgood(const Arguments& argv)
    {
	ARGS("tell",(argc==0));
	DEVTHIS(whio_stream);
	return BoolToJS( dev->api->isgood(dev) );
    }


#undef DEVHT
#undef DEVHV
#undef DEVH
#undef DEVTHIS
    Handle<Value> SetupWhioClasses(const Handle<Object> target )
    {
	HandleScope v8scope;
	Handle<Object> whio = Object::New();
	target->Set(JSTR("whio"),whio);

	// Set up prototype:
	/** Reminder: code order is very important here. See:
	   http://code.google.com/p/v8/issues/detail?id=262
	*/
	Handle<FunctionTemplate> devCtorTmpl = FunctionTemplate::New(dev_ctor);
	Local<ObjectTemplate> devInst = devCtorTmpl->InstanceTemplate();
	devInst->SetInternalFieldCount(1);
	Handle<Function> devCtor( devCtorTmpl->GetFunction() ); // MUST come after devInst->SetInteralFieldCount()
	devInst->Set("SEEK_SET",Integer::New(SEEK_SET));
	devInst->Set("SEEK_END",Integer::New(SEEK_END));
	devInst->Set("SEEK_CUR",Integer::New(SEEK_CUR));


	// IODevice class:
	whio->Set(JSTR("IODevice"), devCtor);
	devInst->Set(JSTR("write"), FunctionTemplate::New( dev_write<whio_dev,true> )->GetFunction() );
	devInst->Set(JSTR("read"), FunctionTemplate::New( dev_read<whio_dev,true> )->GetFunction() );
#define FUNC(N,F) devInst->Set(JSTR(N), FunctionTemplate::New(F)->GetFunction() )
	FUNC("close",dev_close<whio_dev>);
	FUNC("error",dev_error);
	FUNC("clearError",dev_clear_error);
	FUNC("eof",dev_eof);
	FUNC("tell",dev_tell);
	FUNC("seek",dev_seek);
	FUNC("flush",dev_flush<whio_dev>);
	FUNC("truncate",dev_truncate);
	FUNC("size",dev_size);
	FUNC("rewind",dev_rewind);
#undef FUNC


	// InStream class:
	Handle<FunctionTemplate> istrCtorTmpl = FunctionTemplate::New(stream_ctor<false>);
	Local<ObjectTemplate> istrInst = istrCtorTmpl->InstanceTemplate();
	istrInst->SetInternalFieldCount(1);
	Handle<Function> istrCtor( istrCtorTmpl->GetFunction() );
	whio->Set(JSTR("InStream"), istrCtor);
	istrInst->Set(JSTR("write"), FunctionTemplate::New( dev_write<whio_stream,false> )->GetFunction() );
	istrInst->Set(JSTR("read"), FunctionTemplate::New( dev_read<whio_stream,true> )->GetFunction() );
#define FUNC(N,F) istrInst->Set(JSTR(N), FunctionTemplate::New(F)->GetFunction() )
	FUNC("close",dev_close<whio_stream>);
	FUNC("flush",dev_flush<whio_stream>);
	FUNC("isGood",stream_isgood);
#undef FUNC

	// OutStream class:
	Handle<FunctionTemplate> ostrCtorTmpl = FunctionTemplate::New(stream_ctor<true>);
	Local<ObjectTemplate> ostrInst = ostrCtorTmpl->InstanceTemplate();
	ostrInst->SetInternalFieldCount(1);
	Handle<Function> ostrCtor( ostrCtorTmpl->GetFunction() );
	whio->Set(JSTR("OutStream"), ostrCtor);
	ostrInst->Set(JSTR("write"), FunctionTemplate::New( dev_write<whio_stream,true> )->GetFunction() );
	ostrInst->Set(JSTR("read"), FunctionTemplate::New( dev_read<whio_stream,false> )->GetFunction() );
#define FUNC(N,F) ostrInst->Set(JSTR(N), FunctionTemplate::New(F)->GetFunction() )
	FUNC("close",dev_close<whio_stream>);
	FUNC("flush",dev_flush<whio_stream>);
	FUNC("isGood",stream_isgood);
#undef FUNC

	return whio;
    }


#undef JSTR
#undef TOSS
#undef ARGS

    V8_JUICE_PLUGIN_STATIC_INIT(SetupWhioClasses);

}}} // namespaces
