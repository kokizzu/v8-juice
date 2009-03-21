#if !defined(CODE_GOOGLE_COM_V8_JUICE_PLUGIN_WHIO_H_INCLUDED)
#define CODE_GOOGLE_COM_V8_JUICE_PLUGIN_WHIO_H_INCLUDED 1

/************************************************************************
Some of the implementation details for the whio plugin which might
be useful in some cross-plugin contexts.
************************************************************************/

#include <v8.h>
#include <v8/juice/juice.h>
#include <v8/juice/juice-config.h>
#include <v8/juice/convert.h>
#include <v8/juice/cleanup.h>
//#include <v8/juice/JSClassCreator.h>
#include <v8/juice/WeakJSClassCreator.h>
#include "whio_amalgamation.h" // this is the i/o lib we're uses as a basis.

namespace v8 { namespace juice { namespace whio {

    /**
       Adds to target object:

       - whio (generic holder object)
       - whio.IOBase abstract class
       - whio.IODevice class, inherits IOBase
       - whio.StreamBase abstract class
       - whio.InStream class, inherits StreamBase
       - whio.OutStream class, inherits StreamBase

       See full docs at:

       http://code.google.com/p/v8-juice/wiki/PluginWhio
    */
    Handle<Value> SetupWhioClasses(const Handle<Object> target );

    /** Abstract base class of all stream/device classes. */
    struct IOBase
    {
    };
    /** JS wrapper class for an i/o device. */
    struct IODevice : IOBase
    {
	typedef whio_dev type;
        /**
           This object is not valid until SetupWhioClasses()
           has been called.
        */
        static Persistent<Function> js_ctor;
    };
    /** INCOMPLETE.
        JS wrapper class for a byte array. */
    struct ByteArray : IODevice
    {
    };
    /** Abstract JS wrapper class for streams. */
    struct StreamBase : IOBase
    {
	typedef whio_stream type;
    };
    /** JS wrapper class for input streams. */
    struct InStream : StreamBase
    {
    };
    /** JS wrapper class for output streams. */
    struct OutStream : StreamBase
    {
    };

    // whio_dev ctor. Can't be static for templates reasons.
    whio_dev * dev_construct( Arguments const &,
			      std::string & exception );

    // whio_stream ctor. Can't be static for templates reasons.
    template <int mode /* 0=out, 1=in, -1=no-op*/>
    whio_stream * stream_construct( Arguments const &,
				    std::string & exception );
    // whio_dev ctor for ByteArray. Can't be static for templates reasons.
    whio_dev * ba_construct( Arguments const &,
			     std::string & exception );

    
    /**
       A WeakJSClassCreatorOps base type used with WeakJSClassCreator
       for the IOBase family of types.

       DevT must be one of: IODevice, StreamBase, InStream, OutStream,
       ByteArray.
     */
    template <typename DevT,
	      typename DevT::type * (*ctor)( Arguments const &, std::string & ) >
    struct DevClassOps
    {
	enum { ExtraInternalFieldCount = 0 };
	typedef typename DevT::type WrappedType;
	static WrappedType * Ctor( Arguments const & argv,
			    std::string & exceptionText)
	{
	    WrappedType * d = ctor( argv, exceptionText );
	    //CERR << "Ctor() got @"<<d<<'\n';
	    if( d )
	    {
		::v8::juice::cleanup::AddToCleanup(d, cleanup_callback );
	    }
	    return d;
	}

	static void Dtor( WrappedType * obj )
	{
	    if( obj )
	    {
                bind::UnbindNative<WrappedType>( obj, obj );
		::v8::juice::cleanup::RemoveFromCleanup(obj);
                //CERR << "dtor passing on @"<<obj<<'\n';
                obj->api->finalize(obj);
	    }
	}
    private:
	/** Callback for use with juice::cleanup::AddToCleanup(). */
	static void cleanup_callback( void * obj )
	{
	    Dtor( static_cast<WrappedType*>(obj) );
	}
    };




} // namespace whio

    /** Specialization needed by WeakJSClassCreator. */
    template <>
    struct WeakJSClassCreatorOps<whio::StreamBase> :
	whio::DevClassOps<whio::StreamBase, &whio::stream_construct<-1> >
    {
	static char const * ClassName() { return "StreamBase"; }
    };

    /** Specialization needed by WeakJSClassCreator. */
    template <>
    struct WeakJSClassCreatorOps<whio::OutStream> :
	whio::DevClassOps<whio::OutStream, &whio::stream_construct<0> >
    {
	static char const * ClassName() { return "OutStream"; }
    };

    /** Specialization needed by WeakJSClassCreator. */
    template <>
    struct WeakJSClassCreatorOps<whio::InStream> :
	whio::DevClassOps<whio::InStream, &whio::stream_construct<1> >
    {
	static char const * ClassName() { return "InStream"; }
    };

    /** Specialization needed by WeakJSClassCreator. */
    template <>
    struct WeakJSClassCreatorOps<whio::IODevice> :
	whio::DevClassOps<whio::IODevice, &whio::dev_construct >
    {
	static char const * ClassName() { return "IODevice"; }
    };

    /** Specialization needed by WeakJSClassCreator. */
    template <>
    struct WeakJSClassCreatorOps<whio::ByteArray> :
	whio::DevClassOps<whio::ByteArray, &whio::ba_construct >
    {
	static char const * ClassName() { return "ByteArray"; }
    };

} } //namespaces

namespace WHIO = ::v8::juice::whio;

#define WEAK_CLASS_TYPE WHIO::IODevice
#include <v8/juice/WeakJSClassCreator-CastOps.h>

#define WEAK_CLASS_TYPE WHIO::ByteArray
#include <v8/juice/WeakJSClassCreator-CastOps.h>

#define WEAK_CLASS_TYPE WHIO::StreamBase
#include <v8/juice/WeakJSClassCreator-CastOps.h>

#define WEAK_CLASS_TYPE WHIO::InStream
#include <v8/juice/WeakJSClassCreator-CastOps.h>

#define WEAK_CLASS_TYPE WHIO::OutStream
#include <v8/juice/WeakJSClassCreator-CastOps.h>



#endif // CODE_GOOGLE_COM_V8_JUICE_PLUGIN_WHIO_H_INCLUDED
