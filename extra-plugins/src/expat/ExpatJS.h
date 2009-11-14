#if !defined(CODE_GOOGLE_COM_V8_JUICE_EXPAT_H_INCLUDED)
#define CODE_GOOGLE_COM_V8_JUICE_EXPAT_H_INCLUDED 1

#include <expat.h>
#include <v8.h>
#include <v8/juice/ClassWrap.h>
#include <v8/juice/ClassWrap_TwoWay.h>

namespace v8 { namespace juice { namespace expat {

    /** Internal implementation detail. */
    struct ExpatImpl;

    /**
       Class for binding basic libexpat functionality to JS.
       Intended for use with the v8::juice::cw API.

       JS API:

       Class name = ExpatParser

       Properties:

       - userData = arbitrary client data used by the callback
       routines.


       Functions:

       - parse(string xml,bool finished)

       Parses the given XML, triggering the handler callbacks as it
       parses.  When parsing XML incrementally (e.g. line by line),
       pass false as the second parameter EXCEPT for the last
       line of input, where true must be passed. If all XML is to
       be parsed in one go, pass true.


       Callback functions which the client should override:
       
       - startNode(userData,string name,array attr)

       Is called when an XML node is opened. attr is a set of
       key/value pairs containing the node's attributes.

       - endNode( userData, string name )

       Is called when an XML node is closed.
       
       - charHandler( userData, string text, int textLength )

       Is called when CDATA is read.
       
    */
    class ExpatJS
    {
    public:
        ExpatJS();
        ~ExpatJS();

        /**
           Parses the given XML string. If finished is false then xml
           need not be complete XML. This can be called many times as
           XML input becomes available. Pass a true value for finished
           at the end of collecting input.

           This function throws a JS-side exception on error, so the
           return value is only relevant if this is called from C++
           code, as opposed to JS.

           On error, or if finished is true, the internal XML
           parser state is reset, so it may be used again
           to parse another XML tree.
        */
        bool Parse( std::string const & xml, bool finished );

        /**
           Resets the XML parser state, but keeps the existing
           callback handlers. It is not normally necessary to call
           this - it would only be needed when you would like to
           explicitly discard the results of an ongoing incremental
           parse.
        */
        void Reset();
        
        /**
           For internal use only, but in the public API because
           several of our non-member bound functions need access to
           it.
        */
        ExpatImpl * impl;

        static v8::Handle<v8::Value> SetupBindings( v8::Handle<v8::Object> target );
    private:
        void setStartNode( v8::Handle<v8::Value> func );
        v8::Handle<v8::Value> getStartNode() const;
        void setEndNode( v8::Handle<v8::Value> func );
        v8::Handle<v8::Value> getEndNode() const;
        void setCharHandler( v8::Handle<v8::Value> func );
        v8::Handle<v8::Value> getCharHandler() const;
        void userData( v8::Handle<v8::Value> func );
        v8::Handle<v8::Value> userData() const;

        friend class v8::juice::cw::Installer<ExpatJS>;
    };
} } }

////////////////////////////////////////////////////////////////////////
// Declare v8::juice::cw parts of the ExpatJS binding...
namespace v8 { namespace juice {  namespace cw {
    using v8::juice::expat::ExpatJS;
    /**
       ClassWrap ClassName policy specialization for
       v8::juice::expat::ExpatJS.
    */
    template <>
    struct ClassName<ExpatJS>
    {
        /** Returns the JS-side class name of the ExpatJS native class. */
        static char const * Value();
    };

    /**
       ClassWrap Factory policy specialization for
       v8::juice::expat::ExpatJS.
    */
    template <>
    struct Factory<ExpatJS> : Factory_NewDelete<ExpatJS>
    {
    };

    template <>
    struct WeakWrap<ExpatJS>
    {
        typedef ExpatJS * NativeHandle;
        static void Wrap( v8::Persistent<v8::Object> const & jsSelf, NativeHandle nativeSelf );
        static void Unwrap( v8::Handle<v8::Object> const & jsSelf, NativeHandle nativeSelf );
    private:
        typedef TwoWayBind_WeakWrap<ExpatJS> WeakWrapBase;

    };

    template <>
    struct Extract< ExpatJS > :
        TwoWayBind_Extract< ExpatJS > {};

    template <>
    struct ToNative< ExpatJS > :
        ToNative_Base< ExpatJS > {};

    template <>
    struct ToJS< ExpatJS > :
        TwoWayBind_ToJS< ExpatJS > {};
    template <>
    struct Installer<ExpatJS>
    {
        /** Installs the ExpatJS class into the given object. */
        static void SetupBindings( ::v8::Handle< ::v8::Object> target )
        {
            v8::juice::expat::ExpatJS::SetupBindings(target);
        }
    };
    
} // namespace cw
namespace convert
{
    using v8::juice::expat::ExpatJS;
    template <>
    struct NativeToJS< ExpatJS > : v8::juice::cw::NativeToJSImpl< ExpatJS >
    {};

    template <>
    struct JSToNative< ExpatJS > : v8::juice::cw::JSToNativeImpl< ExpatJS >
    {};

} } } // namespaces

#endif // CODE_GOOGLE_COM_V8_JUICE_EXPAT_H_INCLUDED
