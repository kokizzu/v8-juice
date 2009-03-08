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
#define TOSS(X) return ::v8::ThrowException(JSTR(X)) /*juice::ThrowException(X)*/

#define WHIO_RTFM "RTFM: " v8_juice_HOME_PAGE "/wiki/PluginWhio"

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


#define ARGS(FUNC,COND) const int argc = argv.Length(); if( !(COND) ) TOSS(FUNC "(): argument assertion failed: " # COND)
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
	ARGS("rebound",(argc==2));
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
	{
	    std::string fname = JSToStdString(argv[0]);
	    if( ":memory:" == fname )
	    { // (":memory:" [,int size])
		whio_size_t sz = (argc > 1) ? CastFromJS<whio_size_t>(argv[1]) : 0;
		dev = whio_dev_for_membuf( sz, 1.25 );
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
		    self->Set(JSTR("canRead"), Boolean::New(true) );
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
		    self->Set(JSTR("ioDevice"),par, v8::ReadOnly);
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
	    CERR << "whio_stream_for_filename( "<<fname <<", "<<mode<<" ) == dev@"<<dev<<"\n";
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
    static Handle<Value> dev_write(const Arguments& argv)
    {
	if( ! allowWrite )
	{
	    TOSS("This device does not allow writing!");
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
	    return CastToJS( dev->api->write( dev, data.data(), l ) );
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
	    TOSS("This device does not allow reading!");
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
	return Int32ToJS( dev->api->error( dev ) );
    }

    /**
       whio_dev_api::clear_error() wrapper.
    */
    static Handle<Value> dev_clear_error(const Arguments& argv)
    {
	ARGS("clearError",(argc==0));
	DEVTHIS(whio_dev);
	return Int32ToJS( dev->api->clear_error( dev ) );
    }

    /**
       whio_dev_api::eof() wrapper.
    */
    static Handle<Value> dev_eof(const Arguments& argv)
    {
	ARGS("eof",(argc==0));
	DEVTHIS(whio_dev);
	return Int32ToJS( dev->api->eof(dev) );
    }

    /**
       whio_dev_api::tell() wrapper.
    */
    static Handle<Value> dev_tell(const Arguments& argv)
    {
	ARGS("tell",(argc==0));
	DEVTHIS(whio_dev);
	return UInt64ToJS( dev->api->tell( dev ) );
    }

    /**
       whio_dev_api::seek() wrapper.
    */
    static Handle<Value> dev_seek(const Arguments& argv)
    {
	ARGS("seek",((argc==1) || (argc==2)));
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
    static Handle<Value> dev_flush(const Arguments& argv)
    {
	ARGS("flush",(argc==0));
	DEVTHIS(DevT);
	return Int32ToJS( dev->api->flush( dev ) );
    }

    /**
       whio_dev_api::truncate() wrapper.
    */
    static Handle<Value> dev_truncate(const Arguments& argv)
    {
	ARGS("truncate",(argc==1));
	DEVTHIS(whio_dev);
	uint32_t len = JSToUInt64( argv[0] );
	return Int32ToJS( dev->api->truncate(dev, len) );
    }

    /**
       whio_dev_size() wrapper.
    */
    static Handle<Value> dev_size(const Arguments& argv)
    {
	ARGS("size",(argc==0));
	DEVTHIS(whio_dev);
	return UInt64ToJS( whio_dev_size( dev ) );
    }

    /**
       whio_dev_rewind() wrapper.
    */
    static Handle<Value> dev_rewind(const Arguments& argv)
    {
	ARGS("rewind",(argc==0));
	DEVTHIS(whio_dev);
	return Int32ToJS( whio_dev_rewind( dev ) );
    }

    /** whio_stream_api::isgood(). */
    static Handle<Value> stream_isgood(const Arguments& argv)
    {
	ARGS("tell",(argc==0));
	DEVTHIS(whio_stream);
	return BoolToJS( dev->api->isgood(dev) );
    }

    template <typename T,char const *&N>
    static Handle<Value> to_string(const Arguments& argv)
    {
	ARGS("toString",(0==argc));
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


    template <typename FromT, typename ToT>
    static Handle<Value> filter( FromT * src,
				 ToT * dest,
				 Handle<Object> destJObj,
				 Handle<Function> callback,
				 Handle<Value> userData )
    {
	enum { bufSize = 1024 * 16 };
	whio_size_t rdsz = 0;
	std::vector<unsigned char> rbuf(bufSize,0);
	typedef Handle<Value> HV;
	const unsigned int argc = 4;
	std::vector<HV> vec(argc,Null());
	vec[3] = userData;
	bool isEnd = false;
	whio_size_t total = 0;
	while( 0 != (rdsz = src->api->read( src, &rbuf[0], bufSize)) )
	{
	    if( rdsz < bufSize ) isEnd = true;
	    vec[0] = destJObj;
	    vec[1] = String::New( reinterpret_cast<char const*>(&rbuf[0]), static_cast<int>( rdsz ) );
	    vec[2] = Boolean::New( isEnd );
	    Local<Value> rv = callback->Call( destJObj/*???*/, argc, &vec[0] );
	    if( rv.IsEmpty() ) return rv;
	    whio_size_t check = CastFromJS<whio_size_t>(rv);
	    total += check;
	    if( check != rdsz )
	    {
		std::ostringstream msg;
		msg << "filter() callback returned "<<check<<" for a write request of "<<rdsz<<"\n";
		TOSS(msg.str().c_str());
	    }
	    if( isEnd ) break;
	}
	if( (total>0) && (0 == rdsz) && !isEnd )
	{ // we hit a block boundary. Let callback know, in case it wants to do something like flush a zlib buffer.
	    vec[0] = destJObj;
	    vec[1] = String::New( "" );
	    vec[2] = Boolean::New( true );
	    Local<Value> rv = callback->Call( destJObj/*???*/, argc, &vec[0] );
	    if( rv.IsEmpty() ) return rv;
	}
	return CastToJS( total );
    }

#define DEVHN(T,N,H) T * dev ## N = dev_cast<T>( H )
#define DEVHTN(T,N,H) DEVHN(T,N,H); if( ! dev ) TOSS("Handle is-not-a " # T "!")

    /**
       JS usage:

       Signature:

       \code
       int (IODevice|InStream).filter( OStream|IODevice target,
                                       Function callback
                                       [, mixed userData = undefined] );
       \endcode

       The callback signature is:

       \code
       int callback(OutStream out,
                    string data,
		    bool isEnd,
		    mixed userData );
       \endcode

       filter() reads the 'this' object until EOF, in blocks of an
       unspecified size. Each block is passed to the callback function,
       which is presumed to do some sort of filtering on the data.

       The callback should:

       - Perform any translation of the input data it wants to do and
       write it out to the destination stream. If it cannot
       (e.g. target not writable, or write fails), it should throw.

       - Return the number of bytes consumed (not output, which might
       be very different). On success, data.length should be returned,
       and any other value will cause filter() to abort. When
       returning an error value, you must simply ensure that it is
       some value other than data.length 0 is only an error when
       data.length is not 0.

       - Be prepared to be called multiple times, as the data is read
       in blocks. If the callback needs persistent data, it can be
       passed as the third argument to filter() and the callback will
       get it.

       - If the isEnd parameter is true then this will be the last call for
       this filter loop. If needed, the callack can check this and perform
       any necessary actions (e.g. flushing a zlib stream, for example).
       It IS possible that the callback gets called with an empty data
       parameter and (isEnd==true), which simply means the last call to
       read data returned 0 bytes.

       - The callback must not close the output destination.

       filter() returns the total number of bytes it thinks
       where consumed. This number depends on the callback
       returning valid values. In general, no assumptions can
       be made about the state of the two streams on error.
       On success, this stream will be at EOF.
    */
    static Handle<Value> filter(const Arguments& argv)
    {
	ARGS("filter",(argc==2 || argc==3));
	/**
	 filter(IOObj dest, Function callback [, mixed userdata = undefined])

	 function callback(OutStream out, string data, bool isEnd);

	 callback should return the number of bytes processed (need not be the
	 same number as output), or 0 on error or if length==0 (which can happen
	 at the end of the stream).
      */
	whio_dev * srcD = dev_cast<whio_dev>( argv.This() );
	whio_stream * srcS = srcD ? 0 : dev_cast<whio_stream>( argv.This() );
	if( ! srcD && !srcS ) TOSS("Could not determine source object type!");
	HandleScope sentry;
	Handle<Value> destV( argv[0] );

	if( argv.This() == destV ) TOSS("Object cannot be both the source and target for a filter!");

	whio_dev * destD = dev_cast<whio_dev>( destV );
	whio_stream * destS = destD ? 0 : dev_cast<whio_stream>( destV );
	if( ! destD && !destS ) TOSS("First argument is-not-a i/o stream or device!");

	Local<Object> destO( Object::Cast(*destV) );
	{
	    Local<String> key( String::New("canWrite") );
	    if( ! destO->Has(key)
		|| ! destO->Get(key)->BooleanValue() )
	    {
		TOSS("Destination stream/device does not seem to be writable!");
	    }
	}

	//CERR << "Filter: srcD="<<srcD<<", srcS="<<srcS<<", destD="<<destD<<", destS="<<destS<<'\n';

	if( ! argv[1]->IsFunction() ) TOSS("Second argument must be-a Function!");
	Handle<Function> func( Function::Cast(*argv[1]) );
	Handle<Value> udata;
	if( argc>2 ) udata =argv[2];
	else udata =Undefined();
	//whio_size_t len = (argc>2) ? CastFromJS<whio_size_t>(argv[2]) : 0U;
	if( srcD )
	{
	    return ( destD )
		? filter( srcD, destD, destO, func, udata )
		: filter( srcD, destS, destO, func, udata );
	}
	else
	{
	    return ( destD )
		? filter( srcS, destD, destO, func, udata )
		: filter( srcS, destS, destO, func, udata );
	}
    }
    
#undef DEVHT
#undef DEVHV
#undef DEVH
#undef DEVHN
#undef DEVHNT
#undef DEVTHIS

    /**
       Strings which are used as template parameters...
    */
    struct strings
    {
	static char const * IODevice;
	static char const * InStream;
	static char const * OutStream;
    };
    char const * strings::IODevice = "IODevice";
    char const * strings::InStream = "InStream";
    char const * strings::OutStream = "OutStream";

    /**
       Adds to target object:

       - whio.IODevice
       - whio.InStream
       - whio.OutStream
    */
    Handle<Value> SetupWhioClasses(const Handle<Object> target )
    {
	HandleScope v8scope;
	Handle<Object> whio = Object::New();
	target->Set(JSTR("whio"),whio);

	// Set up prototype:
	/** Reminder: code order is very important here. See:
	   http://code.google.com/p/v8/issues/detail?id=262
	*/
	Handle<FunctionTemplate> devCtorTmpl =
	    FunctionTemplate::New(combined_ctor< whio_dev, &dev_construct >);
	Local<ObjectTemplate> devInst = devCtorTmpl->InstanceTemplate();
	devInst->SetInternalFieldCount(1);
	Handle<Function> devCtor( devCtorTmpl->GetFunction() ); // MUST come after devInst->SetInteralFieldCount()
	devInst->Set("SEEK_SET",Integer::New(SEEK_SET));
	devInst->Set("SEEK_END",Integer::New(SEEK_END));
	devInst->Set("SEEK_CUR",Integer::New(SEEK_CUR));

	if(0)
	{ // a test
	    Handle<Object> dev = devCtorTmpl->PrototypeTemplate()->NewInstance();
	    if( dev.IsEmpty() ) return dev; // pass on exception
	    /**
	       Doh:
	       ./test.js:5: TypeError: undefined is not a constructor
	       new whio.IODevice(fname, true );
	    */
	}

	// IODevice class:
	whio->Set(JSTR(strings::IODevice), devCtor);
	devInst->Set(JSTR("write"), FunctionTemplate::New( dev_write<whio_dev,true> )->GetFunction() );
	devInst->Set(JSTR("read"), FunctionTemplate::New( dev_read<whio_dev,true> )->GetFunction() );
	devInst->Set(JSTR("toString"), FunctionTemplate::New( to_string<whio_dev,strings::IODevice> )->GetFunction() );
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
	FUNC("filter",filter);
#undef FUNC


	// InStream class:
	Handle<FunctionTemplate> istrCtorTmpl =
	    FunctionTemplate::New(combined_ctor<
				  whio_stream,
				  &stream_construct<false> // okay, that looks weird.
				  >);
	if(1)
	{
	    //istrCtorTmpl->Inherit( devCtorTmpl );
	    Local<ObjectTemplate> proto = istrCtorTmpl->PrototypeTemplate();
	    proto->Set("read", FunctionTemplate::New( dev_read<whio_stream,true> )->GetFunction() );
	}
	Local<ObjectTemplate> istrInst = istrCtorTmpl->InstanceTemplate();
	istrInst->SetInternalFieldCount(1);
	Handle<Function> istrCtor( istrCtorTmpl->GetFunction() );
	whio->Set(JSTR(strings::InStream), istrCtor);
	istrInst->Set(JSTR("write"), FunctionTemplate::New( dev_write<whio_stream,false> )->GetFunction() );
	//istrInst->Set(JSTR("read"), FunctionTemplate::New( dev_read<whio_stream,true> )->GetFunction() );
	istrInst->Set(JSTR("toString"), FunctionTemplate::New( to_string<whio_stream,strings::InStream> )->GetFunction() );
#define FUNC(N,F) istrInst->Set(JSTR(N), FunctionTemplate::New(F)->GetFunction() )
	FUNC("close",dev_close<whio_stream>);
	FUNC("flush",dev_flush<whio_stream>);
	FUNC("isGood",stream_isgood);
	FUNC("filter",filter);
#undef FUNC

	// OutStream class:
	Handle<FunctionTemplate> ostrCtorTmpl =
	    FunctionTemplate::New(combined_ctor<
				  whio_stream,
				  &stream_construct<true>
				  >);
	Local<ObjectTemplate> ostrInst = ostrCtorTmpl->InstanceTemplate();
	ostrInst->SetInternalFieldCount(1);
	Handle<Function> ostrCtor( ostrCtorTmpl->GetFunction() );
	whio->Set(JSTR(strings::OutStream), ostrCtor);
	ostrInst->Set(JSTR("write"), FunctionTemplate::New( dev_write<whio_stream,true> )->GetFunction() );
	ostrInst->Set(JSTR("read"), FunctionTemplate::New( dev_read<whio_stream,false> )->GetFunction() );
	ostrInst->Set(JSTR("toString"), FunctionTemplate::New( to_string<whio_stream,strings::OutStream> )->GetFunction() );
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
#undef WHIO_RTFM

    /** Plugin initializer for v8-juice. */
    V8_JUICE_PLUGIN_STATIC_INIT(SetupWhioClasses);

}}} // namespaces
