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

/**
   The curl namespace houses a libcurl-based XML parser class for
   binding to JS.
*/
namespace v8 { namespace juice { namespace curl {


    /**
       Class for binding basic libcurl functionality to JS.
       Intended for use with the v8::juice::cw API.

       JS API:

       Class name = Curl

       Ctors:

       - Curl()
       - Curl(string url)
       - Curl(Object options)
       
       Functions:

       - int easyPerform() = a wrapper around curl_easy_perform(). Processes
       the current request options. It requires that opt.url and the appropriate
       other options properties be set.

       Curl Options Properties, all of which can be set as members
       of the 'opt' property, e.g. mycurl.opt.url = "...", or can be
       set directly in the Curl object (but they will not show up in
       enumerations!).

       - string url = the URL to read from

       - int port = default port number to use
       
       - function writeFunction(data,readLength,userData) = callback
       called when data arrives.  Analogous to curl's
       CURLOPT_WRITEFUNCTION except that the signature is different
       due to differences in JS/C++ argument passing conventions. If
       data.length is not the same as readLength then there was an
       unwanted conversion in converting the data to JS space (e.g. it
       may contain non-ASCII data), and it should be treated as suspect.

       - any writeData = passed as second arg to
       writeFunction(). Analogous to curl's CURLOPT_WRITEDATA.

       - function headerFunction(data,readLength,userData) = callback
       called when a header line arrives. Analogous to
       CURLOPT_HEADERFUNCTION. headerFunction() is passed the header
       lines with the trailing \\r\\n from the server, to avoid
       confusion regarding the read size.
       
       - bool verbose = analogous to curl's CURLOPT_VERBOSE.

       - bool failOnError = CURLOPT_FAILONERROR

       - bool header = CURLOPT_HEADER

       - string userAgent = CURLOPT_USERAGENT

       - string proxy = CURLOPT_PROXY

       - int proxyPort = CURLOPT_PROXYPORT

       - string noProxy = CURLOPT_NOPROXY

       - string interface = CURLOPT_INTERFACE

       - bool nobody = CURLOPT_NOBODY

       - bool crlf = CURLOPT_CRLF

       - string range = CURLOPT_RANGE

       - int maxRedirs = CURLOPT_MAXREDIRS

       - bool followLocation = CURLOPT_FOLLOWLOCATION

       - int connectionTimeout = CURLOPT_CONNECTIONTIMEOUT

       - int timeout = CURLOPT_TIMEOUT

       - int timeoutMS = CURLOPT_TIMEOUT_MS

       - int bufferSize = CURLOPT_BUFFERSIZE

       - string userName = CURLOPT_USERNAME

       - string userPwd = CURLOPT_USERPWD
       
       TODOs:

       - Lots.
    */
    class CurlJS
    {
    public:
        /** Internal implementation detail. */
        struct Impl;
        CurlJS();
        ~CurlJS();

        /**
        */
        int EasyPerform();
        
        /**
           Adds the JS/C++ bindings for this class to the given target.
           Returns target, for reasons lost unto history.
        */
        static v8::Handle<v8::Value> SetupBindings( v8::Handle<v8::Object> target );
        /**
           Internal details. In the public API so some internal
           binding routines can get at it.
        */
        Impl * impl;
        static const char * ClassName()
        {
            return "Curl";
        }

        v8::Handle<v8::Value> toString() const;
    private:
        friend class v8::juice::cw::WeakWrap<CurlJS>;
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
        static char const * Value();
    };

    /**
       ClassWrap Factory policy specialization for
       v8::juice::curl::CurlJS.
    */
    template <>
    struct Factory<CurlJS>// :
//         Factory_CtorForwarder<CurlJS,
//             v8::juice::tmp::TypeList<
//                Factory_CtorForwarder0<CurlJS>,
//                Factory_CtorForwarder1<CurlJS,v8::Handle<v8::Value> >
//         > >
                  //Factory_NewDelete<CurlJS>
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
