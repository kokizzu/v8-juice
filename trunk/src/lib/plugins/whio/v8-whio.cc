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
#include <v8/juice/juice-config.h>
#include <v8/juice/convert.h>
#include <v8/juice/plugin.h>
#include <v8/juice/cleanup.h>
#include <v8/juice/JSClassCreator.h>
#include <v8/juice/RCPtr.h>

#include "whio_amalgamation.h" // this is the i/o lib we're uses as a basis.

/* only for debuggering */
#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

namespace v8 { namespace juice { namespace whio {

    namespace bind = ::v8::juice::bind;
    //namespace juice = ::v8::juice;
    using namespace ::v8::juice::convert;

#define JSTR(X) String::New(X)
#define TOSS(X) return ::v8::ThrowException(JSTR(X)) /*juice::ThrowException(X)*/
#define WHIO_RTFM "RTFM: " v8_juice_HOME_PAGE "/wiki/PluginWhio"

    /**
       Static/shared strings, some of which are used as template
       parameters (where we can't use string literals but can,
       curiously enough, use references to pointers to shared
       strings)...
    */
    struct strings
    {
	// Class names:
	static char const * IODevice;
	static char const * InStream;
	static char const * OutStream;
	static char const * IOBase;

	// IOBase:
	static char const * read;
	static char const * write;
	static char const * flush;
	static char const * close;
	static char const * toString;
	static char const * isGood;
	static char const * SEEK_SET_;//need suffix b/c SEEK_SET is a #define'd numeric constant
	static char const * SEEK_CUR_;
	static char const * SEEK_END_;

	// IODevice:
        static char const * error;
        static char const * clearError;
	static char const * eof;
	static char const * tell;
	static char const * seek;
	static char const * truncate;
	static char const * size;
	static char const * rewind;

    };
    char const * strings::IODevice = "IODevice";
    char const * strings::InStream = "InStream";
    char const * strings::OutStream = "OutStream";
    char const * strings::IOBase = "IOBase";
    char const * strings::read = "read";
    char const * strings::write = "write";
    char const * strings::flush = "flush";
    char const * strings::close = "close";
    char const * strings::toString = "toString";
    char const * strings::isGood = "isGood";
    char const * strings::error = "error";
    char const * strings::clearError = "clearError";
    char const * strings::eof = "eof";
    char const * strings::tell = "tell";
    char const * strings::seek = "seek";
    char const * strings::truncate = "truncate";
    char const * strings::size = "size";
    char const * strings::rewind = "rewind";
    char const * strings::SEEK_SET_ = "SEEK_SET";
    char const * strings::SEEK_CUR_ = "SEEK_CUR";
    char const * strings::SEEK_END_ = "SEEK_END";

    /**
       Internal binding context for BindNative() and friends.
    */
    static const void * bind_cx() { static const int x=42;return &x;}

    /** Internal util to reduce code duplication elsewhere: unbinds
	dev and deletes obj using obj->api->finalize(obj).
     */
    template <bool removeCleaner,typename T>
    static void dev_cleanup( void * obj, T * dev )
    {
	CERR << "dev_cleanup( void@"<<obj<<",  dev@"<<dev<<")\n";
	if( removeCleaner )
	{
	    ::v8::juice::cleanup::RemoveFromCleanup(obj);
	    // ^^^ maintenance reminider: we must call that from here, instead
	    // of dev_dtor() because of how IOBase.close() works. When this
	    // func is called by cleanup::Cleanup(), that will become
	    // a no-op.
	}
	bind::UnbindNative( bind_cx(), obj, dev );
	if( dev )
	{
	    dev->api->finalize(dev);
	}
    }

    /**
       Destructor for use with v8::juice::cleanup::AddToCleanup().
       obj MUST be-a (whio_dev*) or (whio_stream*). It simply calls
       dev_cleanup<false,T>( obj, (T*)obj ).

    */
    template <typename T>
    static void cleanup_callback( void * obj )
    {
	dev_cleanup<false,T>( obj, static_cast<T*>( obj ) );
    }

    /**
       Destructor callback for Pesistent::MakeWeak(). T must be
       one of whio_dev or whio_stream. It calls dev_cleanup<true,T>(parameter)
       and disposes object.
    */
    template <typename T>
    static void dev_dtor(Persistent< Value > object, void *obj)
    {
	dev_cleanup<true,T>( obj, static_cast<T*>( obj ) );
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
	::v8::juice::cleanup::AddToCleanup(obj, cleanup_callback<T> );
	bind::BindNative( bind_cx(), obj, obj );
	Persistent<Object> self( Persistent<Object>::New(_self) );
	self.MakeWeak( obj, dev_dtor<T> );
	self->SetInternalField(0,External::New(obj));
	//CERR << "Wrapped device @"<<obj<<" in a Persistent<Object>.\n";
	return self;
    }

    /**
       Casts v to (T*). T must be one of whio_dev or whio_stream.
    */
    template <typename T>
    static T * dev_cast( Handle< Value > const & v )
    {
	if( v.IsEmpty() ) return 0;
	if( ! v->IsObject() )
	{
	    return v->IsExternal()
		? bind::GetBoundNative<T>( bind_cx(), v )
		: 0;
	}
	Local<Object> o( Object::Cast( *v ) );
	return bind::GetBoundNative<T>( bind_cx(), o->GetInternalField(0) );
    }

// Helper macros for args and type checking:
#define ARGS(COND) const int argc = argv.Length(); if( !(COND) ) TOSS("argument assertion failed: " # COND)
#define DEVH(T,H) T * dev = dev_cast<T>( H )
#define DEVHT(T,H) DEVH(T,H); if( ! dev ) TOSS("Native device pointer has already been destroyed!")
#define DEVHV(T,H) DEVH(T,H); if( ! dev ) return
#define DEVTHIS(T) DEVHT(T,argv.This())

    /**
       whio_dev_subdev_rebound() wrapper. It will only work with
       devices created using whio_dev_subdev_create().
    */
    static Handle<Value> dev_rebound(const Arguments& argv)
    {
	ARGS((argc==2));
	DEVTHIS(whio_dev);
	whio_size_t low = CastFromJS<whio_size_t>( argv[0] );
	whio_size_t high = CastFromJS<whio_size_t>( argv[1] );
	return CastToJS( whio_dev_subdev_rebound( dev, low, high ) );
    }

    /**
       Constructor for the native half of IODevice objects.
       
       Curiously, it may not be static because then
       it can't be used as a template type parameter (see
       combined_ctor()). Weird.

       The exception argument should be set to a non-empty value if a
       detailed error message can be reported, otherwise simply return
       0 to indicate failure. If the string is non-empty when this
       function returns, combined_ctor() will throw on our behalf with
       that message (and will destroy the returned object if it is not
       0).

       JS arguments:

       IODevice ctor args:

       - (string filename, bool writeMode)  = file
       - (":memory:" [, int initialSize])   = in-memory buffer
       - (IODevice, int lower, int upper)   = subdevice
     */
    whio_dev * dev_construct( Local<Object> self,
			      int argc,
			      Handle<Value> argv[],
			      std::string & exception )
    {
	whio_dev * dev = 0;
	if( 0 == argc )
	{
	    exception = "Not enough arguments for the constructor!";
	}
	//char const * dontknow = "Don't know how to handle the given constructor arguments!";
	if( argv[0]->IsString() )
	{ //ctor(":memory:")
	  //ctor(string filename[, bool writeMode])
	    std::string fname( JSToStdString(argv[0]) );
	    if( ":memory:" == fname )
	    { // (":memory:" [,int size])
		whio_size_t sz = (argc > 1) ? CastFromJS<whio_size_t>(argv[1]) : 0;
		dev = whio_dev_for_membuf( sz, 1.33 );
		if( ! dev )
		{
		    std::ostringstream msg;
		    msg << "Could not create in-memory i/o device of "
			<<sz<<" bytes!";
		    exception = msg.str();
		}
		else
		{
		    self->Set(JSTR("canWrite"), Boolean::New(true) );
		    self->Set(JSTR("canRead"), Boolean::New(true) );
		}
		return dev;
	    }
	    else if( 2 == argc )
	    { // (string filename, bool writeMode)
		bool writeMode = JSToBool(argv[1]);
		char const * mode = (writeMode) ? "w+b" : "rb";
		dev = whio_dev_for_filename( fname.c_str(), mode );
		if( dev )
		{
		    self->Set(JSTR("fileName"), argv[0], v8::ReadOnly );
		    self->Set(JSTR("canWrite"), Boolean::New(writeMode) );
		    self->Set(JSTR("canRead"), Boolean::New(!writeMode) );
		}
		return dev;
	    }
	}
	if( (3 == argc) && argv[0]->IsObject() )
	{ // (IODevice,lower,upper)
	    Local<Object> par( Object::Cast(*argv[0]) );
	    whio_dev * iod = dev_cast<whio_dev>(par);
	    if( iod )
	    {
		whio_size_t low = CastFromJS<whio_size_t>(argv[1]);
		whio_size_t high = CastFromJS<whio_size_t>(argv[2]);
		dev = whio_dev_subdev_create( iod, low, high );
		if( ! dev )
		{
		    std::ostringstream msg;
		    msg << "Creation of subdevice for range ("
			<<low<<","<<high<<") failed!";
		    exception = msg.str();
		}
		else
		{
		    self->Set(JSTR("rebound"),
			      FunctionTemplate::New(dev_rebound)->GetFunction());
		    self->Set(JSTR("ioDevice"), par, v8::ReadOnly);
		    self->Set(JSTR("canWrite"), par->Get(JSTR("canWrite")), v8::ReadOnly );
		    self->Set(JSTR("canRead"), par->Get(JSTR("canRead")), v8::ReadOnly );
		}
		return dev;
	    }
	}
	exception = "Invalid arguments for IODevice constructor. "
	    WHIO_RTFM;
	return 0;
    }

    /**
       Constructor for the native half of InStream and OutStream
       objects.

       This cannot be static, as explained in dev_construct().

       Note that the writeMode arg is only a template do that we can
       get match the func signature required by combined_ctor().

       Accepted JS args:

       InStream: (writeMode==false)

       - (string filename)

       OutStream (writeMode==true):

       - (string filename [,bool truncate=true])

       Both types:
       - (IODevice)
    */
    template <bool writeMode>
    whio_stream * stream_construct( Local<Object> self,
				    int argc,
				    Handle<Value> argv[],
				    std::string & exception )
    {
	if( argc < 1 )
	{
	    exception = "Not enough arguments for the constructor!";
	    return 0;
	}
	HandleScope boo;
	whio_stream * dev = 0;
	if( argv[0]->IsString() )
	{// (string filename [, truncate=true])
	    std::string fname = JSToStdString(argv[0]);
	    bool trunc = (argc>1) ? argv[1]->BooleanValue() : true;
	    char const * mode = (writeMode) ? (trunc ? "wb" : "ab") : "rb";
	    dev = whio_stream_for_filename( fname.c_str(), mode );
	    if(!dev)
	    {
		std::ostringstream msg;
		msg << "Could not open file ["<<fname<<"] using mode '"<<mode<<"'!";
		exception = msg.str();
		return 0;
	    }
	    self->Set(JSTR("fileName"), argv[0], v8::ReadOnly );
	    self->Set(JSTR("canWrite"), Boolean::New(writeMode), v8::ReadOnly );
	    self->Set(JSTR("canRead"), Boolean::New(writeMode ? false : true) );
	    //CERR << "whio_stream_for_filename( "<<fname <<", "<<mode<<" ) == dev@"<<dev<<"\n";
	    return dev;
	}
	whio_dev * iod = dev_cast<whio_dev>(argv[0]);
	if( iod )
	{ // ctor(IODevice)
	    Local<Object> par( Object::Cast(*argv[0]) );
	    if( writeMode )
	    { // try a basic sanity check...
		Local<Value> parRW( par->Get(JSTR("canWrite")) );
		if( writeMode && !parRW->BooleanValue() )
		{
		    std::ostringstream msg;
		    msg << "Cannot create output stream for device because it says it is read-only!";
		    exception = msg.str();
		    return 0;
		}
	    }
	    dev = whio_stream_for_dev( iod, false );
	    if( ! dev )
	    {
		std::ostringstream msg;
		msg << "whio_stream_for_dev( @"<<iod<<", false ) failed!";
		exception = msg.str();
	    }
	    else
	    {
		self->Set(JSTR("ioDevice"), par, v8::ReadOnly );
		if( writeMode )
		{
		    self->Set(JSTR("canWrite"), par->Get(JSTR("canWrite")), v8::ReadOnly );
		    self->Set(JSTR("canRead"), Boolean::New(false) );
		}
		else
		{
		    self->Set(JSTR("canWrite"), Boolean::New(false), v8::ReadOnly );
		    self->Set(JSTR("canRead"), Boolean::New(true) );
		}
	    }
	    return dev;
	}
	exception = "Invalid arguments for stream constructor. " WHIO_RTFM;
	return 0;
    }

    static Handle<Value> abstract_ctor(const Arguments& argv)
    {
	TOSS("This is an abtract class and cannot be instantiated directly!");
    }

    static Handle<Value> abstract_reimplement(const Arguments& argv)
    {
	TOSS("This is an abtract virtual function which is unimplemented by this subclasses!");
    }

    /**
       ctor for IODevice, InStream, and OutStream classes. This common
       ctor simply forwards all arguments to Func(), which must return
       a (T*) on success (transfering ownership to us), or 0 on error.

       T must be whio_dev or whio_stream.
    */
    template <
	typename T,
	/* constructor proxy: */
	T * (*Func)(Local<Object>, int, Handle<Value>[], std::string &)
	>
    static Handle<Value> combined_ctor(const Arguments& argv)
    {
	if (!argv.IsConstructCall()) 
	{
	    return ThrowException(String::New("Cannot call this constructor as function!"));
	}
	const int argc = argv.Length();
	if( argc > 0 )
	{
	    std::vector< Handle<Value> > av(argc,Null());
	    for( int i = 0; i < argc; ++i ) av[i] = argv[i];
	    std::string err;
	    T * dev = Func( argv.This(), argc, &av[0], err );
	    //err = juice::ThrowException("Just testing at %s:%d!","here",__LINE__);
	    if( ! err.empty() )
	    {
		if( dev )
		{
		    dev->api->finalize(dev);
		    dev = 0;
		}
		TOSS(err.c_str());
	    }
	    if( ! dev )
	    {
		TOSS("Opening the stream failed for an unspecified reason!");
	    }
	    return dev_wrap( argv.This(), dev );
	}
	TOSS("Invalid arguments!");
    }


    /**
       write() impl for DevT, which must be one of whio_dev or whio_stream.

       If allowWrite is false then this function throws when called. Set it to
       false for the InStream class.
    */
    template <typename DevT,bool allowWrite>
    static Handle<Value> devT_write(const Arguments& argv)
    {
	if( ! allowWrite )
	{
	    TOSS("This device does not allow writing!");
	}
	else
	{
	    ARGS((argc==1 || argc==2));
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
	    return CastToJS( dev->api->write( dev, data.data(), l ) );
	}
    }

    /**
       read() impl for DevT, which must be one of whio_dev or whio_stream.

       If allowRead is false then this function throws when called. Set it to
       false for the OutStream class and true for all (or most) others.
    */
    template <typename DevT,bool allowRead>
    static Handle<Value> devT_read(const Arguments& argv)
    {
	if( ! allowRead )
	{
	    TOSS("This device type does not allow reading!");
	}
	else
	{
	    ARGS((argc==1));
	    DEVTHIS(DevT);
	    uint32_t l = CastFromJS<uint32_t>( argv[0] );
	    if( 0 == l ) TOSS("Number of bytes to read must be greater than 0!");
	    std::vector<char> data( l, '\0' );
	    uint32_t rlen = dev->api->read( dev, &data[0], l );
	    return String::New( &data[0], rlen );
	}
    }

    /**
       close() impl for whio_dev and whio_stream.
    */
    template <typename DevT>
    static Handle<Value> devT_close(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(DevT);
	argv.This()->SetInternalField(0,Null()); // is this working?
	dev_cleanup<true>( dev, dev );
	return Undefined();
    }
    /**
       whio_dev_api::close() wrapper.
    */
    static Handle<Value> dev_error(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(whio_dev);
	return Int32ToJS( dev->api->error( dev ) );
    }

    /**
       whio_dev_api::clear_error() wrapper.
    */
    static Handle<Value> dev_clear_error(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(whio_dev);
	return Int32ToJS( dev->api->clear_error( dev ) );
    }

    /**
       whio_dev_api::eof() wrapper.
    */
    static Handle<Value> dev_eof(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(whio_dev);
	return Int32ToJS( dev->api->eof(dev) );
    }

    /**
       whio_dev_api::tell() wrapper.
    */
    static Handle<Value> dev_tell(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(whio_dev);
	return UInt64ToJS( dev->api->tell( dev ) );
    }

    /**
       whio_dev_api::seek() wrapper.
    */
    static Handle<Value> dev_seek(const Arguments& argv)
    {
	ARGS(((argc==1) || (argc==2)));
	DEVTHIS(whio_dev);
	int32_t pos = JSToUInt32( argv[0] );
	const int whence = (argc>1) ? JSToInt32( argv[1] ) : SEEK_SET;
	switch( whence )
	{
	  case SEEK_SET:
	  case SEEK_CUR:
	  case SEEK_END:
	      break;
	  default:
	      TOSS("The second argument to seek() must be one of SEEK_SET, SEEK_CUR, or SEEK_END!");
	      break;
	};
	return UInt64ToJS( dev->api->seek( dev, pos, whence) );
    }

    /**
       flush() impl for DevT, which must be one of whio_dev or whio_stream.
    */
    template <typename DevT>
    static Handle<Value> devT_flush(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(DevT);
	return CastToJS( dev->api->flush( dev ) );
	// Potential fixme: for subdevices we have no direct (via whio_dev)
	// handle to the parent. If the "parent" is destroyed first then calling
	// flush() on the subdevice (which happens on close()) can crash.
	// In the generic whio API we actually have no way of doing anything about
	// this. Hmmm.
    }

    /**
       whio_dev_api::truncate() wrapper.
    */
    static Handle<Value> dev_truncate(const Arguments& argv)
    {
	ARGS((argc==1));
	DEVTHIS(whio_dev);
	whio_size_t len = CastFromJS<whio_size_t>( argv[0] );
	return CastToJS( dev->api->truncate(dev, len) );
    }

    /**
       whio_dev_size() wrapper.
    */
    static Handle<Value> dev_size(const Arguments& argv)
    {
	ARGS(0==argc);
	DEVTHIS(whio_dev);
	return CastToJS( whio_dev_size( dev ) );
    }

    /**
       whio_dev_rewind() wrapper.
    */
    static Handle<Value> dev_rewind(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(whio_dev);
	return CastToJS( whio_dev_rewind( dev ) );
    }

    /** IOBase isGood() impl for whio_dev. */
    static Handle<Value> dev_isgood(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(whio_dev);
	return BoolToJS( 0 == dev->api->error(dev) );
    }

    /** IOBase isGood() impl for whio_stream. */
    static Handle<Value> stream_isgood(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(whio_stream);
	return BoolToJS( dev->api->isgood(dev) );
    }

    template <typename T,char const *&N>
    static Handle<Value> devT_tostring(const Arguments& argv)
    {
	ARGS(0==argc);
	std::ostringstream os;
	Local<Object> self = argv.This();
	os << "[object "<<N;

	Local<String> key(JSTR("fileName"));
	if( self->Has(key) )
	{
	    os << " fileName:'"<<JSToStdString(self->Get(key))<<"',";
	}
	key = JSTR("canWrite");
	os << " canWrite:"<< (JSToStdString(self->Get(key))) << ',';
	key = JSTR("canRead");
	os << " canRead:"<< (JSToStdString(self->Get(key)));

	os <<"]";
	return CastToJS( os.str() );
    }



#define DEVHN(T,N,H) T * dev ## N = dev_cast<T>( H )
#define DEVHTN(T,N,H) DEVHN(T,N,H); if( ! dev ) TOSS("Handle is-not-a " # T "!")

    
#undef DEVHT
#undef DEVHV
#undef DEVH
#undef DEVHN
#undef DEVHNT
#undef DEVTHIS

    /** A bogus type used a a devT_tostring<>() template parameter. */
    struct IOBase
    {
    };

    /**
       Adds to target object:

       - whio.IOBase
       - whio.IODevice inherits IOBase
       - whio.InStream inherits IOBase
       - whio.OutStream inherits IOBase
    */
    Handle<Value> SetupWhioClasses(const Handle<Object> target )
    {
	HandleScope v8scope;
	Handle<Object> whio = Object::New();
	target->Set(JSTR("whio"),whio);

	////////////////////////////////////////////////////////////
	// IOBase class:
	v8::juice::JSClassCreator bindAbs( strings::IOBase, whio, abstract_ctor );
	{
	    Local<Function> noop = FunctionTemplate::New(abstract_reimplement)->GetFunction();
	    bindAbs
		.Set(strings::read, noop )
		.Set(strings::write ,noop)
		.Set(strings::isGood,noop)
		.Set(strings::close,noop)
		.Set(strings::flush,noop)
		.Set(strings::toString, devT_tostring<IOBase,strings::IOBase> )
		.Set(strings::SEEK_SET_,Integer::New(SEEK_SET) )
		.Set(strings::SEEK_END_, Integer::New(SEEK_END))
		.Set(strings::SEEK_CUR_,Integer::New(SEEK_CUR) )
		.Seal();
	}

	////////////////////////////////////////////////////////////
	// IODevice class:
	{
	    v8::juice::JSClassCreator
		bindIOD( strings::IODevice, whio, combined_ctor< whio_dev, &dev_construct >, 1 );
	    bindIOD
		.Inherit( bindAbs )
		.SetInternalFieldCount(1)
		.Set(strings::write, devT_write<whio_dev,true> )
		.Set(strings::read, devT_read<whio_dev,true> )
		.Set(strings::toString, devT_tostring<whio_dev,strings::IODevice> )
		.Set(strings::flush, devT_flush<whio_dev> )
		.Set(strings::close, devT_close<whio_dev> )
		.Set(strings::error,dev_error)
		.Set(strings::clearError,dev_clear_error)
		.Set(strings::eof,dev_eof)
		.Set(strings::tell,dev_tell)
		.Set(strings::seek,dev_seek)
		.Set(strings::truncate,dev_truncate)
		.Set(strings::size,dev_size)
		.Set(strings::rewind,dev_rewind)
		.Set(strings::isGood,dev_isgood)
		.Seal();
	}

	////////////////////////////////////////////////////////////
	// InStream class:
	{
	    v8::juice::JSClassCreator
		bindIS( strings::InStream, whio, combined_ctor< whio_stream, &stream_construct<false> >, 1 );
	    bindIS
		.Inherit( bindAbs )
		.Set(strings::read, FunctionTemplate::New( devT_read<whio_stream,true> )->GetFunction() )
		.Set(strings::flush, devT_flush<whio_stream> )
		.Set(strings::close, devT_close<whio_stream> )
		.Set(strings::toString, devT_tostring<whio_stream,strings::InStream> )
		.Set(strings::isGood,stream_isgood)
		.Seal();
        }

	////////////////////////////////////////////////////////////
	// OutStream class:
	{
	    v8::juice::JSClassCreator
		bindOS( strings::OutStream, whio, combined_ctor< whio_stream, &stream_construct<true> >, 1 );
	    bindOS.Inherit( bindAbs )
		.Set(strings::write, devT_write<whio_stream,true> )
		.Set(strings::flush, devT_flush<whio_stream> )
		.Set(strings::close, devT_close<whio_stream> )
		.Set(strings::toString, devT_tostring<whio_stream,strings::OutStream> )
		.Set(strings::isGood,stream_isgood)
		.Seal();
	}
	return whio;
    }


#undef JSTR
#undef TOSS
#undef ARGS
#undef WHIO_RTFM

    /** Plugin initializer for v8-juice. */
    V8_JUICE_PLUGIN_STATIC_INIT(SetupWhioClasses);

}}} // namespaces
