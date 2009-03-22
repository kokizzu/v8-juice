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
#include <v8/juice/ClassBinder.h>


namespace v8 { namespace juice { namespace whio {

    //namespace juice = ::v8::juice;
    using namespace ::v8::juice::convert;

#define JSTR(X) String::New(X)
#define TOSS(X) return ::v8::ThrowException(JSTR(X)) /*juice::ThrowException(X)*/
#define WHIO_RTFM "RTFM: " v8_juice_HOME_PAGE "/wiki/PluginWhio"

    char const * WhioStrings::IOBase = "IOBase";
    char const * WhioStrings::IODevice = "IODevice";
    char const * WhioStrings::InStream = "InStream";
    char const * WhioStrings::OutStream = "OutStream";
    char const * WhioStrings::ByteArray = "ByteArray";
    char const * WhioStrings::SEEK_CUR_ = "SEEK_CUR";
    char const * WhioStrings::SEEK_END_ = "SEEK_END";
    char const * WhioStrings::SEEK_SET_ = "SEEK_SET";
    char const * WhioStrings::StreamBase = "StreamBase";
    char const * WhioStrings::canRead = "canRead";
    char const * WhioStrings::canWrite = "canWrite";
    char const * WhioStrings::clearError = "clearError";
    char const * WhioStrings::close = "close";
    char const * WhioStrings::eof = "eof";
    char const * WhioStrings::error = "error";
    char const * WhioStrings::fileName = "fileName";
    char const * WhioStrings::flush = "flush";
    char const * WhioStrings::gzip = "gzipTo";
    char const * WhioStrings::gunzip = "gunzipTo";
    char const * WhioStrings::homePage = "homePage";
    char const * WhioStrings::ioDevice = "ioDevice";
    char const * WhioStrings::isGood = "isGood";
    char const * WhioStrings::read = "read";
    char const * WhioStrings::rebound = "rebound";
    char const * WhioStrings::rewind = "rewind";
    char const * WhioStrings::seek = "seek";
    char const * WhioStrings::size = "size";
    char const * WhioStrings::tell = "tell";
    char const * WhioStrings::toString = "toString";
    char const * WhioStrings::truncate = "truncate";
    char const * WhioStrings::write = "write";

    /**
       Internal binding context for BindNative() and friends.
    */
    //static const void * bind_cx() { return 0;}

    Persistent<Function> IODevice::js_ctor;

    static Handle<Value> abstract_ctor(const Arguments& argv)
    {
	TOSS("This is an abtract class and cannot be instantiated directly!");
    }

    static Handle<Value> abstract_reimplement(const Arguments& argv)
    {
	TOSS("This is an abtract virtual function which is unimplemented by this subclasses!");
    }


// Helper macros for args and type checking:
#define ARGS(COND) const int argc = argv.Length(); if( !(COND) ) TOSS("argument assertion failed: " # COND)
#define DEVH(T,H) T::type * dev = CastFromJS< T >( H )
    //#define DEVH(T,H) WeakJSClassCreator<T>::WrappedType * dev = WeakJSClassCreator<T>::GetSelf( H )
#define DEVHT(T,H) DEVH(T,H); if( ! dev ) TOSS("Native device pointer not found (maybe already closed?)!")
#define DEVHV(T,H) DEVH(T,H); if( ! dev ) return
#define DEVTHIS(T) DEVHT(T,argv.This())

    /**
       whio_dev_subdev_rebound() wrapper. It will only work with
       devices created using whio_dev_subdev_create().
    */
    static Handle<Value> dev_rebound(const Arguments& argv)
    {
	ARGS((argc==2));
	DEVTHIS(IODevice);
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
    whio_dev * dev_construct( Arguments const & argv,
                              std::string & exception )
    {
	const int argc = argv.Length();
	whio_dev * dev = 0;
	if( 0 == argc )
	{
	    exception = "Not enough arguments for the constructor!";
	}
	HandleScope scope;
        if( (1 == argc) && argv[0]->IsExternal() )
        {
            // we hope it is a whio_dev passed by a "friendly" factory function.
            Local< External > ex( External::Cast(*argv[0]) );
            dev = bind::GetBoundNative<whio_dev>( ex->Value() );
            if( ! dev )
            {
                exception = "Internal ctor got incorrect arguments!";
            }
            return dev;
        }
	Local<Object> self = argv.This();
	if( argv[0]->IsString() )
	{ //ctor(":memory:")
	  //ctor(string filename[, bool writeMode])
	    std::string fname( JSToStdString(argv[0]) );
	    if( ":memory:" == fname )
	    { // (":memory:" [,int size])
		whio_size_t sz = (argc > 1) ? CastFromJS<whio_size_t>(argv[1]) : 0;
		dev = whio_dev_for_membuf( sz, 1.5 );
		if( ! dev )
		{
		    std::ostringstream msg;
		    msg << "Could not create in-memory i/o device of "
			<<sz<<" bytes!";
		    exception = msg.str();
		}
		else
		{
		    self->Set(JSTR(WhioStrings::fileName), argv[0], v8::ReadOnly );
		    self->Set(JSTR(WhioStrings::canWrite), Boolean::New(true) );
		    self->Set(JSTR(WhioStrings::canRead), Boolean::New(true) );
		}
		return dev;
	    }
	    else if( 2 == argc )
	    { // (string filename, bool writeMode)
		bool writeMode = JSToBool(argv[1]);
		char const * mode = (writeMode) ? "r+b" : "rb";
		dev = whio_dev_for_filename( fname.c_str(), mode );
		if( dev )
		{
		    self->Set(JSTR(WhioStrings::fileName), argv[0], v8::ReadOnly );
		    self->Set(JSTR(WhioStrings::canWrite), Boolean::New(writeMode) );
		    self->Set(JSTR(WhioStrings::canRead), Boolean::New(true) );
		}
		return dev;
	    }
	}
	if( (3 == argc) && argv[0]->IsObject() )
	{ // (IODevice,lower,upper)
	    Local<Object> par( Object::Cast(*argv[0]) );
	    whio_dev * iod = CastFromJS<IODevice>(par);
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
		    self->Set(JSTR(WhioStrings::rebound),
			      FunctionTemplate::New(dev_rebound)->GetFunction());
		    self->Set(JSTR(WhioStrings::ioDevice), par, v8::ReadOnly);
		    self->Set(JSTR(WhioStrings::canWrite), par->Get(JSTR(WhioStrings::canWrite)), v8::ReadOnly );
		    self->Set(JSTR(WhioStrings::canRead), par->Get(JSTR(WhioStrings::canRead)), v8::ReadOnly );
		}
		return dev;
	    }
	}
	exception = "Invalid arguments for IODevice constructor. "
	    WHIO_RTFM;
	return 0;
    }

    whio_dev * ba_construct( Arguments const & argv,
			     std::string & exception )
    {
	const int argc = argv.Length();
	whio_dev * dev = 0;
	HandleScope scope;
	Local<Object> self = argv.This();
	whio_size_t sz = (argc > 0) ? CastFromJS<whio_size_t>(argv[0]) : 0;
	dev = whio_dev_for_membuf( sz, 1.5 );
	if( ! dev )
	{
	    std::ostringstream msg;
	    msg << "Could not create in-memory i/o device of "
		<<sz<<" bytes!";
	    exception = msg.str();
	}
	else
	{
	    self->Set(JSTR(WhioStrings::canWrite), Boolean::New(true) );
	    self->Set(JSTR(WhioStrings::canRead), Boolean::New(true) );
	}
	return dev;
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
       - (IODevice), acts as a stream proxy for the given device.

       omModes:

       - (0)=Out
       - (1)=In
       - (-1)=No-op (throw)
      
    */
    template <int opMode >
    whio_stream * stream_construct(
// 				   Local<Object> self,
// 				    int argc,
// 				    Handle<Value> argv[],
				    Arguments const & argv,
				    std::string & exception )
    {
	if( opMode < 0 )
	{
	    exception = "This is an abstract base type which cannot be directly instantiated!";
	    return 0;
	}
	const int argc = argv.Length();
	if( argc < 1 )
	{
	    exception = "Not enough arguments for the constructor!";
	    return 0;
	}
	HandleScope boo;
	Local<Object> self = argv.This();
	whio_stream * dev = 0;
	const bool writeMode = (0==opMode);
	if( argv[0]->IsString() )
	{// (string filename [, truncate=true])
	    std::string fname = JSToStdString(argv[0]);
	    bool trunc = (argc>1) ? argv[1]->BooleanValue() : true;
	    char const * mode = writeMode ? (trunc ? "wb" : "ab") : "rb";
	    dev = whio_stream_for_filename( fname.c_str(), mode );
	    if(!dev)
	    {
		std::ostringstream msg;
		msg << "Could not open file ["<<fname<<"] using mode '"<<mode<<"'!";
		exception = msg.str();
		return 0;
	    }
	    self->Set(JSTR(WhioStrings::fileName), argv[0], v8::ReadOnly );
	    self->Set(JSTR(WhioStrings::canWrite), Boolean::New(writeMode), v8::ReadOnly );
	    self->Set(JSTR(WhioStrings::canRead), Boolean::New(writeMode ? false : true) );
	    //CERR << "whio_stream_for_filename( "<<fname <<", "<<mode<<" ) == dev@"<<dev<<"\n";
	    return dev;
	}
	whio_dev * iod = CastFromJS<IODevice>( argv[0] );
	if( iod )
	{ // ctor(IODevice)
	    Local<Object> par( Object::Cast(*argv[0]) );
	    if( writeMode && par->Has(JSTR(WhioStrings::canWrite)) )
	    { // try a basic sanity check...
                Local<Value> parRW( par->Get(JSTR(WhioStrings::canWrite)) );
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
		self->Set(JSTR(WhioStrings::ioDevice), par, v8::ReadOnly );
		if( writeMode )
		{
		    self->Set(JSTR(WhioStrings::canWrite), par->Get(JSTR(WhioStrings::canWrite)), v8::ReadOnly );
		    self->Set(JSTR(WhioStrings::canRead), Boolean::New(false) );
		}
		else
		{
		    self->Set(JSTR(WhioStrings::canWrite), Boolean::New(false), v8::ReadOnly );
		    self->Set(JSTR(WhioStrings::canRead), Boolean::New(true) );
		}
	    }
	    return dev;
	}
	exception = "Invalid arguments for stream constructor. " WHIO_RTFM;
	return 0;
    }


    /**
       write() impl for DevT, which must be one of whio_dev or whio_stream.

       If allowWrite is false then this function throws when called. Set it to
       false for the InStream class.
    */
    template <typename DevT>
    static Handle<Value> devT_write_impl( DevT * dev, const Arguments& argv)
    {
	ARGS((argc==1 || argc==2));
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

    static Handle<Value> stream_write( const Arguments& argv)
    {
	DEVTHIS(OutStream);
	return devT_write_impl<OutStream::type>( dev, argv );
    }
    static Handle<Value> dev_write( const Arguments& argv)
    {
	DEVTHIS(IODevice);
	return devT_write_impl<IODevice::type>( dev, argv );
    }
    /**
       read() impl for DevT, which must be one of whio_dev or whio_stream.
    */
    template <typename DevT>
    static Handle<Value> devT_read_impl( DevT * dev, const Arguments& argv)
    {
	uint32_t l = CastFromJS<uint32_t>( argv[0] );
	if( 0 == l ) TOSS("Number of bytes to read must be greater than 0!");
	std::vector<char> data( l, '\0' );
	whio_size_t rlen = dev->api->read( dev, &data[0], l );
	return String::New( &data[0], static_cast<int>(rlen) );
    }
    static Handle<Value> dev_read(const Arguments& argv)
    {
	ARGS((argc==1));
	DEVTHIS(IODevice);
	return devT_read_impl( dev, argv );
    }

    static Handle<Value> stream_read(const Arguments& argv)
    {
	ARGS((argc==1));
	DEVTHIS(InStream);
	return devT_read_impl( dev, argv );
    }


    /**
       close() impl for whio_dev and whio_stream.
    */
    static Handle<Value> dev_close(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(IODevice);
	//CERR << "IODevice.close() == " << dev << '\n';
	WeakJSClassCreator<IODevice>::DestroyObject( argv.This() );
	return Undefined();
    }

    /**
       whio_stream_api::close() wrapper.
    */
    static Handle<Value> stream_close(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(StreamBase);
	//CERR << "StreamBase.close() == " << dev << '\n';
	WeakJSClassCreator<StreamBase>::DestroyObject( argv.This() );
	return Undefined();
    }
    /**
       whio_dev_api::error() wrapper.
    */
    static Handle<Value> dev_error(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(IODevice);
	return CastToJS( dev->api->error( dev ) );
    }

    /**
       whio_dev_api::clear_error() wrapper.
    */
    static Handle<Value> dev_clear_error(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(IODevice);
	return CastToJS( dev->api->clear_error( dev ) );
    }

    /**
       whio_dev_api::eof() wrapper.
    */
    static Handle<Value> dev_eof(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(IODevice);
	return CastToJS( dev->api->eof(dev) );
    }

    /**
       whio_dev_api::tell() wrapper.
    */
    static Handle<Value> dev_tell(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(IODevice);
	return CastToJS( dev->api->tell( dev ) );
    }

    /**
       whio_dev_api::seek() wrapper.
    */
    static Handle<Value> dev_seek(const Arguments& argv)
    {
	ARGS(((argc==1) || (argc==2)));
	DEVTHIS(IODevice);
	typedef int32_t IntT;
	IntT pos = CastFromJS<IntT>( argv[0] );
	const int whence = (argc>1) ? CastFromJS<IntT>( argv[1] ) : SEEK_SET;
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
	return CastToJS( dev->api->seek( dev, pos, whence) );
    }

    /**
       flush() impl for DevT, which must be one of whio_dev or whio_stream.
    */
    template <typename DevT>
    static Handle<Value> devT_flush_impl(DevT * dev, const Arguments& argv)
    {
	return CastToJS( dev->api->flush( dev ) );
	// Potential fixme: for subdevices we have no direct (via whio_dev)
	// handle to the parent. If the "parent" is destroyed first then calling
	// flush() on the subdevice (which happens on close()) can crash.
	// In the generic whio API we actually have no way of doing anything about
	// this. Hmmm.
    }

    /**
       whio_dev_api::flush() wrapper.
    */
    static Handle<Value> dev_flush(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(IODevice);
	return devT_flush_impl<IODevice::type>( dev, argv );
    }
    /**
       whio_stream_api::flush() wrapper.
    */
    static Handle<Value> stream_flush(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(StreamBase);
	return devT_flush_impl<StreamBase::type>( dev, argv );
    }

    /**
       whio_dev_api::truncate() wrapper.
    */
    static Handle<Value> dev_truncate(const Arguments& argv)
    {
	ARGS((argc==1));
	DEVTHIS(IODevice);
	whio_size_t len = CastFromJS<whio_size_t>( argv[0] );
	return CastToJS( dev->api->truncate(dev, len) );
    }

    /**
       whio_dev_size() wrapper.
    */
    static Handle<Value> dev_size(const Arguments& argv)
    {
	ARGS(0==argc);
	DEVTHIS(IODevice);
	return CastToJS( whio_dev_size( dev ) );
    }

    /**
       whio_dev_rewind() wrapper.
    */
    static Handle<Value> dev_rewind(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(IODevice);
	return CastToJS( whio_dev_rewind( dev ) );
    }

    /** IOBase isGood() impl for whio_dev. */
    static Handle<Value> dev_isgood(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(IODevice);
	return BoolToJS( 0 == dev->api->error(dev) );
    }

    /** IOBase isGood() impl for whio_stream. */
    static Handle<Value> stream_isgood(const Arguments& argv)
    {
	ARGS((0==argc));
	DEVTHIS(StreamBase);
	return BoolToJS( dev->api->isgood(dev) );
    }

#ifndef Z_DEFAULT_COMPRESSION
#define Z_DEFAULT_COMPRESSION (-1)
#endif

    static Handle<Value> stream_gzip(const Arguments& argv)
    {
#if ! WHIO_ENABLE_ZLIB
        TOSS("zlib support was not build in to this library!");
#else
	ARGS((1==argc)||(2==argc));
	DEVTHIS(StreamBase);
        whio_stream * dest = CastFromJS<StreamBase>( argv[0] );
        // FIXME: check for writability on dest and readability on this.
        if( ! dest )
        {
            TOSS("First argument must be an output stream!");
        }
        int lv = (argc>1) ? CastFromJS<int>(argv[1]) : Z_DEFAULT_COMPRESSION;
        int rc = whio_stream_gzip( dev, dest, lv );
        return CastToJS( rc );
#endif
    }

    static Handle<Value> stream_gunzip(const Arguments& argv)
    {
#if ! WHIO_ENABLE_ZLIB
        TOSS("zlib support was not build in to this library!");
#else
	ARGS((1==argc)||(2==argc));
	DEVTHIS(StreamBase);
        whio_stream * dest = CastFromJS<StreamBase>( argv[0] );
        // FIXME: check for writability on dest and readability on this.
        if( ! dest )
        {
            TOSS("First argument must be an output stream!");
        }
        int rc = whio_stream_gunzip( dev, dest );
        return CastToJS( rc );
#endif
    }


    /**
       toString() impl for IOBase family of types. N must
       be the class's name.
    */
    template <char const *&N>
    static Handle<Value> devT_tostring(const Arguments& argv)
    {
	ARGS(0==argc);
	std::ostringstream os;
	Local<Object> self = argv.This();
	os << "[object "<<N;

	Local<String> key(JSTR(WhioStrings::fileName));
	if( self->Has(key) )
	{
	    os << ' ' << WhioStrings::fileName<<":'"<<JSToStdString(self->Get(key))<<"',";
	}
	key = JSTR(WhioStrings::canWrite);
	os << ' ' << WhioStrings::canWrite <<':'<< (JSToStdString(self->Get(key))) << ',';
	key = JSTR(WhioStrings::canRead);
	os << ' ' << WhioStrings::canRead << ':'<< (JSToStdString(self->Get(key)));

	os <<"]";
	return CastToJS( os.str() );
    }


#undef EFSH
#undef EFSHT
#undef EFSTHIS
#undef DEVHT
#undef DEVHV
#undef DEVH
#undef DEVTHIS

    Handle<Value> SetupWhioClasses(const Handle<Object> target )
    {
	HandleScope v8scope;
	Handle<Object> whio = Object::New();
	target->Set(JSTR("whio"),whio);

        {
            Handle<Object> whiorc = Object::New();
            whio->Set(JSTR("rc"),whiorc);

#define SET(K,V) whiorc->Set(JSTR(K), Integer::New(V), v8::ReadOnly ); \
            whiorc->Set(Integer::New(V), JSTR(K), v8::ReadOnly )
            SET("OK", whio_rc.OK );
            SET("ArgError",whio_rc.ArgError);
            SET("IOError",whio_rc.IOError);
            SET("AllocError",whio_rc.AllocError);
            SET("InternalError",whio_rc.InternalError);
            SET("RangeError",whio_rc.RangeError);
            SET("AccessError",whio_rc.AccessError);
            SET("ConsistencyError",whio_rc.ConsistencyError);
            SET("NYIError",whio_rc.NYIError);
            SET("UnsupportedError",whio_rc.UnsupportedError);
            SET("TypeError",whio_rc.TypeError);
            SET("SizeTError",-1);
#undef SET
        }

        whio->Set(JSTR(WhioStrings::homePage),JSTR("http://fossil.wanderinghorse.net/repos/whio/"));

	////////////////////////////////////////////////////////////
	// IOBase class:
	v8::juice::JSClassCreator bindAbs( WhioStrings::IOBase, whio, abstract_ctor );
	Local<Function> func_noop = FunctionTemplate::New(abstract_reimplement)->GetFunction();
	{
	    bindAbs
		.Set(WhioStrings::read, func_noop )
		.Set(WhioStrings::write ,func_noop)
		.Set(WhioStrings::isGood,func_noop)
		.Set(WhioStrings::close,func_noop)
		.Set(WhioStrings::flush,func_noop)
		.Set(WhioStrings::toString, devT_tostring<WhioStrings::IOBase> )
		.Set(WhioStrings::SEEK_SET_,Integer::New(SEEK_SET) )
		.Set(WhioStrings::SEEK_END_, Integer::New(SEEK_END))
		.Set(WhioStrings::SEEK_CUR_,Integer::New(SEEK_CUR) )
		.Seal();
	}

	////////////////////////////////////////////////////////////
	// IODevice class:
	WeakJSClassCreator<IODevice> bindIOD( whio );
	{
	    bindIOD
		.Inherit( bindAbs )
 		.Set(WhioStrings::write, dev_write )
 		.Set(WhioStrings::read, dev_read )
 		.Set(WhioStrings::toString, devT_tostring<WhioStrings::IODevice> )
 		.Set(WhioStrings::flush, dev_flush )
 		.Set(WhioStrings::close, dev_close )
		.Set(WhioStrings::error,dev_error)
		.Set(WhioStrings::clearError,dev_clear_error)
		.Set(WhioStrings::eof,dev_eof)
		.Set(WhioStrings::tell,dev_tell)
		.Set(WhioStrings::seek,dev_seek)
		.Set(WhioStrings::truncate,dev_truncate)
		.Set(WhioStrings::size,dev_size)
		.Set(WhioStrings::rewind,dev_rewind)
		.Set(WhioStrings::isGood,dev_isgood)
		.Seal();
            if( IODevice::js_ctor.IsEmpty() )
            {
                IODevice::js_ctor = Persistent<Function>::New( bindIOD.CtorTemplate()->GetFunction() );
            }
        }
	////////////////////////////////////////////////////////////
	// ByteArray class:
	if(0)
        {
	    WeakJSClassCreator<ByteArray>
		bindBA( whio );
            bindBA.Inherit( bindIOD )
 		.Set(WhioStrings::toString, devT_tostring<WhioStrings::ByteArray> )
		.Seal();
	}

	////////////////////////////////////////////////////////////
	// StreamBase class:
	WeakJSClassCreator<StreamBase> bindSB( whio );
	{
	    bindSB
		.Inherit( bindAbs )
 		.Set(WhioStrings::close, stream_close )
 		.Set(WhioStrings::flush, stream_flush )
 		.Set(WhioStrings::isGood,stream_isgood)
 		.Set(WhioStrings::toString, devT_tostring<WhioStrings::StreamBase> )
		.Seal();
        }

	////////////////////////////////////////////////////////////
	// InStream class:
	{
	    WeakJSClassCreator<InStream> bindIS( whio );
	    bindIS.Inherit( bindSB )
 		.Set(WhioStrings::read, stream_read )
 		.Set(WhioStrings::gzip, stream_gzip )
 		.Set(WhioStrings::gunzip, stream_gunzip )
 		.Set(WhioStrings::toString, devT_tostring<WhioStrings::InStream> )
		.Seal();
        }

	////////////////////////////////////////////////////////////
	// OutStream class:
	{
	    WeakJSClassCreator<OutStream> bindOS( whio );
            bindOS.Inherit( bindSB )
 		.Set(WhioStrings::write, stream_write )
 		.Set(WhioStrings::toString, devT_tostring<WhioStrings::OutStream> )
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
