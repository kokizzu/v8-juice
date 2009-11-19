#if !defined(CODE_GOOGLE_COM_V8_JUICE_CURL_H_INCLUDED)
#define CODE_GOOGLE_COM_V8_JUICE_CURL_H_INCLUDED 1

#include <v8.h>
#include <v8/juice/ClassWrap.h>
/** @def V8_JUICE_CURL_USE_TWOWAY_WRAP

    Set V8_JUICE_CURL_USE_TWOWAY_WRAP to a true value to enable
    "two-way" class binding (with conversions for JS-to-Native and
    Native-to-JS). This is only needed if native CurlJS objects will
    be passed back to JS via, e.g., function return values.

    Using the TwoWay wrapper increases the memory cost of each parser
    and is a tad bit slower than not using it.
*/
#define V8_JUICE_CURL_USE_TWOWAY_WRAP 0
#if V8_JUICE_CURL_USE_TWOWAY_WRAP
#include <v8/juice/ClassWrap_TwoWay.h>
#endif
#include <curl/curl.h>
/**
   The curl namespace houses a libcurl-based XML parser class for
   binding to JS.
*/
namespace v8 { namespace juice { namespace curl {

    /**
       Class for binding basic libcurl functionality to JS.
       Intended for use with the v8::juice::cw API.

       JS API:

       See: http://code.google.com/p/v8-juice/wiki/PluginCurl
       
       TODOs:

       - Lots.
    */
    class CurlJS
    {
    public:
        /** Internal implementation detail. */
        struct Impl;
        /**
           Internal details. In the public API only so some internal
           binding routines can get at it.
        */
        Impl * impl;
        /**
           Initializes a new curl handle using curl_easy_init().
        */
        CurlJS();
        /**
           Frees up internal resources.
        */
        ~CurlJS();

        CURL * Curl();
        
        /**
           See curl_easy_perform().
        */
        int EasyPerform();
        
        /**
           Adds the JS/C++ bindings for this class to the given target.
           Returns target, for reasons lost unto history.
        */
        static v8::Handle<v8::Value> SetupBindings( v8::Handle<v8::Object> target );

        /**
           Returns this type's JS-side class name.
        */
        static const char * ClassName()
        {
            return "Curl";
        }

        v8::Handle<v8::Value> toString() const;

        
        /**
           Analog to curl_easy_setopt(), but curlID must be one of our
           internally-supported options so we know which type
           conversion to use on val.
        */
        v8::Handle<v8::Value> SetOpt( int curlID, v8::Handle<v8::Value> const & val );

        v8::Handle<v8::Value> SetOpt( v8::Handle<v8::Value> const & k, v8::Handle<v8::Value> const & val );
        /**
           JS usage:

           @code
           myCurl.setOpt( { url:"htt://foo.bar", verbose:"true" ... });
           @endcode

           Replaces all options with the given ones, but does not "undo"
           settings previously made to options which are not in the
           new list.
           
           Returns the number of properties set.
        */
        v8::Handle<v8::Value> SetOpts( v8::Handle<v8::Value> const & obj );
        /**
           Like setOption(), but does not clear the list of options
           list before applying the new options.
        */
        v8::Handle<v8::Value> AddOpts( v8::Handle<v8::Value> const & obj );

	/**
	   Analog to curl_easy_getinfo(), but throws on error.
	*/
	v8::Handle<v8::Value> GetInfo( int flag );

    };

    
} } }

////////////////////////////////////////////////////////////////////////
// Declare v8::juice::cw parts of the CurlJS binding...
namespace v8 { namespace juice {  namespace cw {
    using v8::juice::curl::CurlJS;
    /**
       ClassWrap ClassName policy specialization for
       v8::juice::curl::CurlJS.
    */
    template <>
    struct ClassName<CurlJS>
    {
        /** Returns the JS-side class name of the CurlJS native class. */
        static char const * Value()
        {
            return CurlJS::ClassName();
        }
    };

    /**
       ClassWrap Factory policy specialization for
       v8::juice::curl::CurlJS.
    */
    template <>
    struct Factory<CurlJS>
    {
        typedef convert::TypeInfo<CurlJS>::Type Type;
        typedef convert::TypeInfo<CurlJS>::NativeHandle NativeHandle;
	static NativeHandle Instantiate( Arguments const &  argv, std::ostream & errmsg );
	static void Destruct( v8::Handle<v8::Object> /*ignored*/, NativeHandle obj );
        static const size_t AllocatedMemoryCost = sizeof(Type);
    };

    template <>
    struct WeakWrap<CurlJS>
    {
        typedef CurlJS * NativeHandle;
        static void Wrap( v8::Persistent<v8::Object> const & jsSelf, NativeHandle nativeSelf );
        static void Unwrap( v8::Handle<v8::Object> const & jsSelf, NativeHandle nativeSelf );
#if V8_JUICE_CURL_USE_TWOWAY_WRAP
    private:
        typedef TwoWayBind_WeakWrap<CurlJS> WeakWrapBase;
#endif
    };

    template <>
    struct Installer<CurlJS>
    {
        /** Installs the CurlJS class into the given object. */
        static void SetupBindings( ::v8::Handle< ::v8::Object> target )
        {
            v8::juice::curl::CurlJS::SetupBindings(target);
        }
    };

#if V8_JUICE_CURL_USE_TWOWAY_WRAP
    template <>
    struct Extract< CurlJS > :
        TwoWayBind_Extract< CurlJS > {};

    template <>
    struct ToNative< CurlJS > :
        ToNative_Base< CurlJS > {};

    template <>
    struct ToJS< CurlJS > :
        TwoWayBind_ToJS< CurlJS > {};
#endif

} // namespace cw
namespace convert
{
    using v8::juice::curl::CurlJS;
#if V8_JUICE_CURL_USE_TWOWAY_WRAP
    template <>
    struct NativeToJS< CurlJS > : v8::juice::cw::NativeToJSImpl< CurlJS >
    {};
#endif
    template <>
    struct JSToNative< CurlJS > : v8::juice::cw::JSToNativeImpl< CurlJS >
    {};

} } } // namespaces

#endif // CODE_GOOGLE_COM_V8_JUICE_CURL_H_INCLUDED
