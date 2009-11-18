#include "CurlJS.h"

#include <curl/curl.h>
#include <v8/juice/convert.h>
#include <v8/juice/plugin.h>
#include <map>
#include <iostream>
#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

typedef v8::Persistent<v8::Function> PFuncHnd;
typedef v8::Persistent<v8::Value> PValHnd;
typedef v8::Handle<v8::Function> FuncHnd;
typedef v8::Handle<v8::Value> ValHnd;

#define JSTR(X) v8::String::New(X)
#define TOSS(X) v8::ThrowException(JSTR(X))
#define TOSSV(X) v8::ThrowException(X)

namespace v8 { namespace juice { namespace curl {
    namespace cv = v8::juice::convert;
    // For use with CURLOPT_WRITEFUNCTION
    static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *data);
    // For use with CURLOPT_HEADERFUNCTION
    static size_t HeaderCallback(void *ptr, size_t size, size_t nmemb, void *data);

    /**
       Holds names of JS properties.
    */
    struct Strings
    {
        static char const * ctorArg;
        static char const * easyPerform;
        static char const * optBufferSize;
        static char const * optConnTimeout;
        static char const * optCRLF;
        static char const * optFailOnErr;
        static char const * optFollowLocation;
        static char const * optHeader;
        static char const * optHeaderData;
        static char const * optHeaderFunc;
        static char const * optInterface;
        static char const * optMaxRedirs;
        static char const * optNoBody;
        static char const * optObj;
        static char const * optPort;
        static char const * optProxy;
        static char const * optProxyNo;
        static char const * optProxyPort;
        static char const * optRange;
        static char const * optTimeout;
        static char const * optTimeoutMS;
        static char const * optVerbose;
        static char const * optWriteData;
        static char const * optWriteFunc;
        static char const * optURL;
        static char const * optUserAgent;
        static char const * optUserName;
        static char const * optUserPwd;
    };
    char const * Strings::ctorArg = "$_$ctorArg";
    char const * Strings::easyPerform = "easyPerform";
    char const * Strings::optCRLF = "crlf";
    char const * Strings::optBufferSize = "bufferSize"; 
    char const * Strings::optConnTimeout = "connectionTimeout";
    char const * Strings::optFollowLocation = "followLocation";
    char const * Strings::optFailOnErr = "failOnError";
    char const * Strings::optHeader = "header";
    char const * Strings::optHeaderData = "headerData";
    char const * Strings::optHeaderFunc = "headerFunction";
    char const * Strings::optInterface = "interface";
    char const * Strings::optMaxRedirs = "maxRedirs";
    char const * Strings::optNoBody = "noBody";
    char const * Strings::optObj = "opt";
    char const * Strings::optPort = "optPort";
    char const * Strings::optProxy = "proxy";
    char const * Strings::optProxyNo = "noProxy";
    char const * Strings::optProxyPort = "proxyPort";
    char const * Strings::optTimeout = "timeout";
    char const * Strings::optTimeoutMS = "timeoutMS";
    char const * Strings::optRange = "range";
    char const * Strings::optURL = "url";
    char const * Strings::optUserAgent = "userAgent";
    char const * Strings::optUserName = "userName";
    char const * Strings::optUserPwd = "userName";
    char const * Strings::optVerbose = "verbose";
    char const * Strings::optWriteData = "writeData";
    char const * Strings::optWriteFunc = "writeFunction";
    
    struct CurlJS::Impl
    {
        CURL * ch;
        /** The JS-side 'this' object for the CurlJS object holding this object. */
        v8::Handle<v8::Object> jself; // we assume this is Persistent elsewhere
        Impl()
            : ch(curl_easy_init()),
              jself()
        {
            if( ! this->ch )
            {
                throw std::runtime_error("curl_easy_init() returned NULL!");
            }
            curl_easy_setopt( this->ch, CURLOPT_WRITEFUNCTION, WriteCallback );
            curl_easy_setopt( this->ch, CURLOPT_HEADERFUNCTION, HeaderCallback );
            curl_easy_setopt( this->ch, CURLOPT_WRITEDATA, this );
            curl_easy_setopt( this->ch, CURLOPT_HEADERDATA, this );
            curl_easy_setopt( this->ch, CURLOPT_ENCODING, "" );
            //CERR << "Impl() @"<<(void const *)this<<'\n';
        }
        ~Impl()
        {
            curl_easy_cleanup( this->ch );
        }
        /** Returns this->jself->Get("opt"). That object should
            already exist (it is added to the prototype as part of the
            ClassWrap binding setup), but this function will create it
            if needed.
        */
        v8::Handle<v8::Object> opt()
        {
            v8::Local<v8::String> const okey = JSTR(Strings::optObj);
            ValHnd ov = this->jself->Get( okey );
            v8::Handle<v8::Object> jo;
            if( ov.IsEmpty() || ! ov->IsObject() )
            {
                jo = v8::Object::New();
                this->jself->Set( okey, jo );
            }
            else
            {
                jo = v8::Handle<v8::Object>( v8::Object::Cast( *ov ) );
            }
            return jo;
        }
        /** Returns the given key from jself['opt'][k] */
        v8::Handle<v8::Value> opt( char const * k )
        {
            v8::Local<v8::String> const jk = JSTR(k);
//             v8::Handle<v8::Object> op = this->opt();
//             if( ! op->Has( jk ) ) return ValHnd();
//             ValHnd const rv = this->opt()->Get( jk );
//             if( rv.IsEmpty() ) return v8::Undefined();
//             else return rv;
             return this->opt()->Get( jk );
        }
    public:
        /**
           Extracts jself['opt'][key] and sets it as a string
           property. opt must be a supported CURLOPT_xxx value.
           If the property is set to Undefined or Null then
           it is treated as an empty string.
        */
        int applyStringOpt( int opt, char const * key )
        {
            ValHnd uv = this->opt( key );
            std::string val;
            if( uv.IsEmpty() )
            {
                return -1;
            }
            else if( uv->IsUndefined()
                     || uv->IsNull() )
            {
                val = "";
            }
            else val = cv::JSToStdString( uv );
            return curl_easy_setopt( this->ch, CURLoption(opt), val.empty() ? 0 : val.c_str() );
        }

        /**
           Extracts jself['opt'][key] and sets it as a boolean
           property. opt must be a supported CURLOPT_xxx value.
        */
        int applyBoolOpt( int opt, char const * key )
        {
            return curl_easy_setopt( this->ch,
                                     CURLoption(opt),
                                     cv::JSToBool( this->opt(key) ) ? 1 : 0 );
        }

        /**
           Extracts jself['opt'][key] and sets it as a T
           property. opt must be a supported CURLOPT_xxx value.
        */
        template <typename T>
        int applyOpt( int opt, char const * key )
        {
            ValHnd const uv = this->opt( key );
            if( uv.IsEmpty() || uv->IsUndefined() )
            {
                return -1;
            }
            return curl_easy_setopt( this->ch,
                                     CURLoption(opt),
                                     cv::CastFromJS<T>( uv ) );
        }
        /**

        */
        void setOptVal( int curlOpt, char const * key, ValHnd const & vh )
        {
            this->opt()->Set( JSTR(key), vh );
        }
    private:
        int setupPerform()
        {
            int check = this->applyStringOpt( CURLOPT_URL, Strings::optURL );
            if( CURLE_OK != check )
            {
                cv::StringBuffer msg;
                msg << "[object " << CurlJS::ClassName()
                    << "]."<<Strings::optURL<<" option setup failed with error code "
                    << check << '.';
                TOSSV(msg);
                return check;
            }

            this->applyStringOpt( CURLOPT_INTERFACE, Strings::optInterface );
            this->applyStringOpt( CURLOPT_RANGE, Strings::optRange );
            this->applyStringOpt( CURLOPT_PROXY, Strings::optProxy );
            this->applyStringOpt( CURLOPT_NOPROXY, Strings::optProxyNo );
            this->applyStringOpt( CURLOPT_USERAGENT, Strings::optUserAgent );
            this->applyStringOpt( CURLOPT_USERNAME, Strings::optUserName );
            this->applyStringOpt( CURLOPT_USERPWD, Strings::optUserPwd );

            this->applyBoolOpt( CURLOPT_FOLLOWLOCATION, Strings::optFollowLocation );
            this->applyBoolOpt( CURLOPT_VERBOSE, Strings::optVerbose );
            this->applyBoolOpt( CURLOPT_FAILONERROR, Strings::optFailOnErr );
            this->applyBoolOpt( CURLOPT_HEADER, Strings::optHeader );
            this->applyBoolOpt( CURLOPT_NOBODY, Strings::optNoBody );
            this->applyBoolOpt( CURLOPT_CRLF, Strings::optCRLF );

            this->applyOpt<long>( CURLOPT_PROXYPORT, Strings::optProxyPort );
            this->applyOpt<long>( CURLOPT_MAXREDIRS, Strings::optMaxRedirs );
            this->applyOpt<long>( CURLOPT_CONNECTTIMEOUT, Strings::optConnTimeout );
            this->applyOpt<long>( CURLOPT_TIMEOUT, Strings::optTimeout );
            this->applyOpt<long>( CURLOPT_TIMEOUT_MS, Strings::optTimeoutMS );
            this->applyOpt<long>( CURLOPT_BUFFERSIZE, Strings::optBufferSize );
            this->applyOpt<long>( CURLOPT_PORT, Strings::optPort );
            return CURLE_OK;
        }
    public:
        int EasyPerform()
        {
            int rc = this->setupPerform();
            if( CURLE_OK != rc ) return rc;
            else
            {
                return curl_easy_perform(this->ch);
            }
        }
        /**
           Gets the handler callback function associated with n, or an
           empty handle if n is not set or is not-a Function.
        */
        FuncHnd getOptFunc( char const * n )
        {
            ValHnd const h = this->opt(n);
            return (!h.IsEmpty() && h->IsFunction() )
                ? FuncHnd( v8::Function::Cast(*h) )
                : FuncHnd();
        }
    };

    /**
       CURLOPT_xxxFUNCTION callback for WRITEFUNCTION and HEADERFUNCTION.

       OptKey == callback function name.
       DataKey == "data argument" property name.

       BUGS:

       - Our callback mechanism does not 100% match what libcurl specifies
       for CURLOPT_HEADERFUNCTION when HEADERFUNCTION is null but HEADERDATA
       is not. See:

       http://curl.haxx.se/libcurl/c/curl_easy_setopt.html#CURLOPTHEADERFUNCTION
       
    */
    template <char const * &OptKey,char const * &DataKey>
    static size_t WriterCallback( void *ptr, size_t size, size_t nmemb, void *data)
    {
        //CERR << "WriterCallback<"<<OptKey<<", "<<DataKey<<">() "<<(size*nmemb)<<" bytes.\n";
        CurlJS::Impl * im;
        im = reinterpret_cast<CurlJS::Impl *>( data );
        FuncHnd fh = im->getOptFunc( OptKey );
        if( fh.IsEmpty() ) return (size*nmemb);
        enum { argc = 3 };
        size_t len = size * nmemb;
        char const * cp = (len) ? reinterpret_cast<char const *>( ptr ) : 0;
        ValHnd argv[argc] = {
            v8::String::New( cp ? cp : "", cp ? static_cast<int>( len ) : 0 ),
            v8::Number::New( len ),
            im->opt( DataKey )
        };
        v8::Local<v8::Value> rv = fh->Call( im->jself, argc, argv );
        return rv.IsEmpty() ? 0 : cv::CastFromJS<size_t>( rv );
    }
    
    static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *data)
    {
        return WriterCallback<Strings::optWriteFunc,Strings::optWriteData>( ptr, size, nmemb, data );
    }
    static size_t HeaderCallback(void *ptr, size_t size, size_t nmemb, void *data)
    {
        return WriterCallback<Strings::optHeaderFunc,Strings::optHeaderData>( ptr, size, nmemb, data );
    }
    
}}} // v8::juice::curl

namespace v8 { namespace juice { namespace cw {
    //using v8::juice::curl::CurlJS;
    using namespace v8::juice::curl;
    using namespace ::v8::juice;

    CurlJS * Factory<CurlJS>::Instantiate( v8::Arguments const &  argv, std::ostream & errmsg )
    {
        const int argc = argv.Length();
        CurlJS * rc = 0;
        if( 0 == argc ) rc = new CurlJS;
        else if( argc > 1 )
        {
            errmsg << CurlJS::ClassName() << "(arg): arg must be a String (URL) or options Object.";
        }
#if 1
        rc = new CurlJS();
        argv.This()->SetHiddenValue( JSTR(Strings::ctorArg), argv[0] );
#else
        /**
           We can't do ctor(String url | object Options) properly here
           because at this point in the process rc is not bound to
           JS...

           To do this we'd have to stuff a property into argv.This()
           and handle it in WeakWrap().
        */
        else
        {
            v8::Handle<v8::Object> jThis = argv.This();
            // ^^^^ this is likely to break if this type is subclassed.
            rc = new CurlJS();
            v8::Handle< v8::Value > jv = argv[0];
            if( jv->IsObject() )
            {
                v8::Handle< v8::Object > jo( v8::Object::Cast(*jv) );
                jThis->Set( JSTR(Strings::optObj), jo );
            }
            else
            {
                v8::Handle<v8::Object> jopt( v8::Object::Cast( *(jThis->Get(JSTR(Strings::optObj)) ) ) );
                jopt->Set( JSTR(Strings::optURL), jv );
            }
        }
#endif   
        return rc; 
    }
    void Factory<CurlJS>::Destruct( v8::Handle<v8::Object> /*ignored*/, NativeHandle obj )
    {
        delete obj;
    }
    
    void WeakWrap<CurlJS>::Wrap( v8::Persistent<v8::Object> const & jsSelf, CurlJS * nativeSelf )
    {
#if V8_JUICE_CURL_USE_TWOWAY_WRAP
        WeakWrapBase::Wrap( jsSelf, nativeSelf );
#endif
        CurlJS::Impl * impl = nativeSelf->impl;
        impl->jself = jsSelf;
        v8::Local<v8::String> const ckey = JSTR(Strings::ctorArg);
        v8::Handle<v8::Value> const ctorArg = jsSelf->GetHiddenValue(ckey);
        if( ! ctorArg.IsEmpty() && !ctorArg->IsUndefined() )
        {
            jsSelf->DeleteHiddenValue( ckey );
            if( ctorArg->IsObject() )
            {
                impl->jself->Set( JSTR(Strings::optObj), ctorArg );
            }
            else
            {
                impl->jself->Set( JSTR(Strings::optURL), ctorArg );
            }
        }
        //nativeSelf->impl->opt(); //kludge to initialize .opt object!
        //argv.This()->Set( JSTR(Strings::ctorArg), argv[0] );
    }

    void WeakWrap<CurlJS>::Unwrap( v8::Handle<v8::Object> const & jsSelf, CurlJS * nativeSelf )
    {
#if V8_JUICE_CURL_USE_TWOWAY_WRAP
        WeakWrapBase::Unwrap( jsSelf, nativeSelf );
#endif
        nativeSelf->impl->jself.Clear();
    }

    

    
} } } // v8::juice::cw



namespace v8 { namespace juice { namespace curl {
    namespace cv = v8::juice::convert;

    CurlJS::CurlJS()
        : impl(new CurlJS::Impl)
    {
    }

    CurlJS::~CurlJS()
    {
        delete this->impl;
    }
    
    int CurlJS::EasyPerform()
    {
        return this->impl->EasyPerform();
    }

    v8::Handle<v8::Value> CurlJS::toString() const
    {
        cv::StringBuffer s;
        s << "[object "<<CurlJS::ClassName()
          <<'@'<<(void const *)this<<']';
        return s;
    }
//     /** Gets the 'opt' Object. */
//     static v8::Handle<v8::Value> OptGetObj( Local< String > jkey, const AccessorInfo & info )
//     {
//         CurlJS * c = cv::CastFromJS<CurlJS>( info.This() );
//         CERR << "OptGetObj()@"<<(void const *)c<<'\n';
//         if( ! c )
//         {
//             cv::StringBuffer msg;
//             msg << CurlJS::ClassName() << '.'<<Strings::optObj << " getter could not find native 'this' object!";
//             return TOSSV(msg);
//         }
//         return c->impl->opt();
//     }

//     /** Sets the 'opt' Object. */
//     static void OptSetObj(v8::Local< v8::String > property, v8::Local< v8::Value > value, const v8::AccessorInfo &info)
//     {
//         CurlJS * c = cv::CastFromJS<CurlJS>( info.This() );
//         if( ! c )
//         {
//             cv::StringBuffer msg;
//             msg << CurlJS::ClassName() << '.'<<Strings::optObj << " getter could not find native 'this' object!";
//             TOSSV(msg);
//             return;
//         }
//         if( value.IsEmpty() || !value->IsObject() )
//         {
//             cv::StringBuffer msg;
//             msg << CurlJS::ClassName() << "."<<Strings::optObj << " must be-a Object.";
//             TOSSV(msg);
//             return;
//         }
//         c->impl->jself->Set( JSTR(Strings::optObj), value );
//         //v8::Handle<v8::Object>( v8::Object::Cast(*value) ) );
//         return;
//     }

    /**
       v8::AccessorGetter impl.
       
       OptKey must be one of the Strings strings.
    */
    template <char const * & OptKey>
    static v8::Handle<v8::Value> OptGet( Local< String > jkey, const AccessorInfo & info )
    {
        CurlJS * c = cv::CastFromJS<CurlJS>( info.This() );
        if( ! c )
        {
            cv::StringBuffer msg;
            msg << CurlJS::ClassName() << '.'<<OptKey << " getter could not find native 'this' object!";
            return TOSSV(msg);
        }
        return c->impl->opt()->Get( jkey );
    }
    /**
       v8::AccessorSetter impl.
       
       OptKey must be one of the Strings strings.

       CurlOptID must be one of the supported CURLOPT_xxx values.
    */
    template <int CurlOptID, char const * & OptKey>
    static void OptSet(v8::Local< v8::String > /*property*/,
                       v8::Local< v8::Value > value,
                       const v8::AccessorInfo &info)
    {
        CurlJS * c = cv::CastFromJS<CurlJS>( info.This() );
        //CERR << "OptSet<"<<CurlOptID<<','<<OptKey<<">()@"<<(void const *)c<<" = "<<cv::JSToStdString(value)<<"\n";
        if( ! c )
        {
            cv::StringBuffer msg;
            msg << CurlJS::ClassName() << '.'<<OptKey << " setter could not find native 'this' object!";
            TOSSV(msg);
            return;
        }
        v8::Local<v8::String> const jkey = JSTR(OptKey);
        switch( CurlOptID )
        {
#define CASE(I) case I:  \
            c->impl->opt()->Set( jkey, value ); \
            c->impl->applyStringOpt( I, OptKey ); \
            break
            CASE(CURLOPT_INTERFACE);
            CASE(CURLOPT_NOPROXY);
            CASE(CURLOPT_PROXY);
            CASE(CURLOPT_RANGE);
            CASE(CURLOPT_URL);
            CASE(CURLOPT_USERAGENT);
            CASE(CURLOPT_USERNAME);
            CASE(CURLOPT_USERPWD);
#undef CASE
            // impl->applyBoolOpt():
#define CASE(I) case I: \
            c->impl->opt()->Set( jkey, value ); \
            c->impl->applyBoolOpt( I, OptKey ); \
            break
            CASE(CURLOPT_CRLF);
            CASE(CURLOPT_FAILONERROR);
            CASE(CURLOPT_FOLLOWLOCATION);
            CASE(CURLOPT_HEADER);
            CASE(CURLOPT_NOBODY);
            CASE(CURLOPT_VERBOSE);
#undef CASE
            // impl->applyOpt<long>():
#define CASE(I) case I: \
            c->impl->opt()->Set( jkey, value ); \
            c->impl->applyOpt<long>( I, OptKey ); \
            break
            CASE(CURLOPT_MAXREDIRS);
            CASE(CURLOPT_PROXYPORT);
            CASE(CURLOPT_CONNECTTIMEOUT);
            CASE(CURLOPT_TIMEOUT);
            CASE(CURLOPT_TIMEOUT_MS);
            CASE(CURLOPT_BUFFERSIZE);
            CASE(CURLOPT_PORT);
#undef CASE
          default: {
              // Internal error.
              cv::StringBuffer msg;
              msg << "Unknown CURLOPT_xxx ="<<CurlOptID<<" (property = "<<OptKey<<")";
              TOSSV(msg);
              break;
          }
        };
    }

    v8::Handle<v8::Value> CurlJS::SetupBindings( v8::Handle<v8::Object> target )
    {
        typedef CurlJS N;
        typedef cw::ClassWrap<N> CW;
        CW & cw( CW::Instance() );
        if( cw.IsSealed() )
        {
            cw.AddClassTo(target);
            return target;
        }
        typedef convert::InvocationCallbackCreator ICC;
        typedef convert::MemFuncInvocationCallbackCreator<N> ICM;

        curl_global_init(CURL_GLOBAL_ALL);
        cw
            .Set( Strings::easyPerform, ICM::M0::Invocable<int,&N::EasyPerform> )
            .Set( "toString", ICM::M0::Invocable<ValHnd,&N::toString> )
            //.Set( Strings::optObj, v8::Object::New() )
            /**^^^ if i do this then my Strings::ctorArg kludgery (elsewhere) isn't working! */
            ;
        {
            v8::Handle<v8::ObjectTemplate> proto = cw.Prototype();
            v8::AccessorGetter GF;
            v8::AccessorSetter SF;
#define FN(X) v8::FunctionTemplate::New( X )->GetFunction()
#define ACC(I,KEY) GF = OptGet<Strings::KEY>; \
            SF = OptSet<I,Strings::KEY>; \
            proto->SetAccessor( JSTR(Strings::KEY), GF, SF, ValHnd(), v8::DEFAULT, v8::DontEnum )

            ACC(CURLOPT_URL,optURL);
            ACC(CURLOPT_USERAGENT,optUserAgent);
            ACC(CURLOPT_USERPWD,optUserPwd );
            ACC(CURLOPT_USERNAME,optUserName );
            ACC(CURLOPT_INTERFACE,optInterface);
            ACC(CURLOPT_RANGE,optRange );
            ACC(CURLOPT_PROXY,optProxy);
            ACC(CURLOPT_NOPROXY,optProxyNo);
            ACC(CURLOPT_VERBOSE,optVerbose);
            ACC(CURLOPT_FAILONERROR, optFailOnErr );
            ACC(CURLOPT_HEADER, optHeader );
            ACC(CURLOPT_NOBODY, optNoBody );
            ACC(CURLOPT_CRLF, optCRLF );
            ACC(CURLOPT_FOLLOWLOCATION, optFollowLocation);
            ACC(CURLOPT_MAXREDIRS, optMaxRedirs);
            ACC(CURLOPT_CONNECTTIMEOUT, optConnTimeout);
            ACC(CURLOPT_TIMEOUT, optTimeout);
            ACC(CURLOPT_TIMEOUT_MS, optTimeoutMS);
            ACC(CURLOPT_BUFFERSIZE, optBufferSize);
            ACC(CURLOPT_PORT,optPort);
            //proto->SetAccessor( JSTR(Strings::optObj), OptGetObj );//, OptSetObj );
            //proto->SetAccessor( JSTR(Strings::optObj), 0, OptSetObj );
#undef FN
#undef ACC
        }


// #define JF v8::FunctionTemplate::New(cb)->GetFunction()
// #define F(X) cw.Set( X, JF )
//         v8::InvocationCallback cb;
// #undef JF
// #undef F
        cw.Set( "destroy", CW::DestroyObject );

//         cb = ICM::M0::Invocable<void,&N::Reset>;
//         F("reset");
        v8::Handle<v8::Function> ctor = cw.Seal();
        cw.AddClassTo(target);

// #define OPT(N,O) ctor->Set( JSTR(N), v8::Integer::New( O ) )
//         OPT( "OptURL", CURLOPT_URL );
//         OPT( "OptWriteFunction", CURLOPT_WRITEFUNCTION );
//         OPT( "OptWriteData", CURLOPT_WRITEDATA );
// #undef OPT
        return target;
    }

} } } // v8::juice::curl

namespace {
    v8::Handle<v8::Value> CurlPluginInit( v8::Handle<v8::Object> target )
    {
        return v8::juice::curl::CurlJS::SetupBindings(target);
    }
}
    V8_JUICE_PLUGIN_STATIC_INIT(CurlPluginInit);


#undef JSTR
#undef TOSS
#undef TOSSV
