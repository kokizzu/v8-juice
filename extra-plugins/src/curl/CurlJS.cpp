#include "CurlJS.h"

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

typedef struct curl_slist curl_slist;
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
        static char const * easyPerform;
        static char const * fnAddOption;
        static char const * fnSetOption;
        static char const * optHeaderData;
        static char const * optHeaderFunc;
        static char const * optObj;
        static char const * optObjHidden;
        static char const * optWriteData;
        static char const * optWriteFunc;
    };
    char const * Strings::easyPerform = "easyPerform";
    char const * Strings::fnSetOption = "setOpt";
    char const * Strings::fnAddOption = "addOpt";
    char const * Strings::optHeaderData = "headerData";
    char const * Strings::optHeaderFunc = "headerFunction";
    char const * Strings::optObj = "opt";
    char const * Strings::optObjHidden = "$opt";
    char const * Strings::optWriteData = "writeData";
    char const * Strings::optWriteFunc = "writeFunction";

    /**
       Interface for setting a Curl option.

       - jso = the JS object in which to set the property.
       - cu = the CURL handle to set the property on.
       - key = the property name.
       - val = the new value.

       Implementations must respect the type required for the
       underlying curl_easy_setopt() call.
    */
    typedef int (*CurlOptSetter)( v8::Handle<v8::Object> jso, CURL * cu, v8::Handle<v8::Value> const & key, v8::Handle<v8::Value> const & val);


    /** Convenience base for CurlOpt specializations. */
    template <int _CurlOptID>
    struct CurlOpt_Base
    {
        static const int CurlOptID = _CurlOptID;
    };
    /**
       Interface for setting Curl options from JS. MUST
       be specialized on CurlOptID and the specializations
       must behave as demonstrated in CurlOptString,
       CurlOptLong, and friends.
    */
    template <int CurlOptID_>
    struct CurlOpt
    {
#if defined(DOXYGEN)
        /**
           Must be-a CURLOPT_xxx value.
        */
        static const int CurlOptID = CurlOptID_;
        /**
           Must return the JS property name.
         */
        static char const * Key();
        /**
           Must implement the CurlOptSetter interface.
        */
        static int Set( v8::Handle<v8::Object> jso, CURL * cu, v8::Handle<v8::Value> const & key, v8::Handle<v8::Value> const & jv);
#endif
    };

    /**
       Stores an arbitrary JS object as a curl option.
    */
    template <int CurlOptID>
    struct CurlOptJSVal : CurlOpt_Base<CurlOptID>
    {
        static int Set( v8::Handle<v8::Object> jso, CURL * cu, v8::Handle<v8::Value> const & key, v8::Handle<v8::Value> const & jv)
        {
            jso->Set( key, jv );
            return 0;
        }
    };
    /**
       Sets a curl string option.
    */
    template <int CurlOptID>
    struct CurlOptString : CurlOpt_Base<CurlOptID>
    {
        static int Set( v8::Handle<v8::Object> jso, CURL * cu, v8::Handle<v8::Value> const & key, v8::Handle<v8::Value> const & jv)
        {
            std::string val;
            if( jv.IsEmpty() )
            {
                return -1;
            }
            else if( jv->IsUndefined()
                     || jv->IsNull() )
            {
                val = "";
            }
            else val = cv::JSToStdString( jv );
            jso->Set( key, cv::CastToJS(val) );
            //CERR << "Setting "<<PropKey<<" STRING: "<<val<<'\n';
            return curl_easy_setopt( cu, CURLoption(CurlOptID), val.empty() ? 0 : val.c_str() );
        }
    };

    /**
       Sets a curl 'long' option.
    */
    template <int CurlOptID>
    struct CurlOptLong : CurlOpt_Base<CurlOptID>
    {
        static int Set( v8::Handle<v8::Object> jso, CURL * cu, v8::Handle<v8::Value> const & key, v8::Handle<v8::Value> const & jv)
        {
            long nv = 0;
            if( jv.IsEmpty() || jv->IsUndefined() )
            {
                return -1;
            }
            else
            {
                nv = cv::CastFromJS<long>( jv );
            }
            //CERR << "Setting "<<PropKey<<" LONG: "<<nv<<'\n';
            jso->Set( key, cv::CastToJS(nv) );
            return curl_easy_setopt( cu,
                                     CURLoption(CurlOptID),
                                     nv );
        }
    };
    /**
       Sets a curl boolean option (actually a long for libcurl).
    */
    template <int CurlOptID>
    struct CurlOptBool : CurlOpt_Base<CurlOptID>
    {
        static int Set( v8::Handle<v8::Object> jso, CURL * cu, v8::Handle<v8::Value> const & key, v8::Handle<v8::Value> const & jv)
        {
            bool nv = cv::CastFromJS<bool>( jv );
            jso->Set( key, jv );
            //CERR << "Setting "<<PropKey<<" BOOL: "<<nv<<'\n';
            return curl_easy_setopt( cu,
                                     CURLoption(CurlOptID),
                                     nv ? 1 : 0 );
        }
    };

#define OPTKEY(KEY) static char const * Key() { return KEY; }
#define COPT_LONG(SUFFIX,KEY) template <> struct CurlOpt< CURLOPT_ ## SUFFIX > : CurlOptLong<CURLOPT_ ## SUFFIX> {OPTKEY(KEY)}
#define COPT_BOOL(SUFFIX,KEY) template <> struct CurlOpt< CURLOPT_ ## SUFFIX > : CurlOptBool<CURLOPT_ ## SUFFIX> {OPTKEY(KEY)}
#define COPT_STR(SUFFIX,KEY) template <> struct CurlOpt< CURLOPT_ ## SUFFIX > : CurlOptString<CURLOPT_ ## SUFFIX> {OPTKEY(KEY)}
#define COPT_JVAL(SUFFIX,KEY) template <> struct CurlOpt< CURLOPT_ ## SUFFIX > : CurlOptJSVal<CURLOPT_ ## SUFFIX> {OPTKEY(KEY)}
    COPT_BOOL(FAILONERROR, "failOnErr");
    COPT_BOOL(FOLLOWLOCATION, "followLocation");
    COPT_BOOL(HEADER, "header");
    COPT_BOOL(NOBODY, "noBody");
    COPT_BOOL(VERBOSE,"verbose");
    COPT_JVAL(HEADERDATA, Strings::optHeaderData);
    COPT_JVAL(HEADERFUNCTION, Strings::optHeaderFunc);
    COPT_JVAL(WRITEDATA, Strings::optWriteData);
    COPT_JVAL(WRITEFUNCTION, Strings::optWriteFunc);
    COPT_LONG(BUFFERSIZE, "bufferSize");
    COPT_LONG(CONNECTTIMEOUT, "connectionTimeout");
    COPT_LONG(CRLF, "crlf");
    COPT_LONG(LOW_SPEED_LIMIT,"lowSpeedLimit");
    COPT_LONG(LOW_SPEED_TIME,"lowSpeedTime");
    COPT_LONG(MAXREDIRS, "maxRedirs");
    COPT_LONG(PORT, "port");
    COPT_LONG(PROXYPORT, "proxyPort");
    COPT_LONG(RESUME_FROM,"resumeFrom");
    COPT_LONG(TIMEOUT, "timeout");
    COPT_LONG(TIMEOUT_MS, "timeoutMS");
    COPT_STR(INTERFACE, "interface");
    COPT_STR(NOPROXY, "noProxy");
    COPT_STR(PROXY, "proxy");
    COPT_STR(RANGE, "range");
    COPT_STR(URL, "url");
    COPT_STR(USERAGENT, "userAgent");
    COPT_STR(USERNAME, "userName");
    COPT_STR(USERPWD, "userPwd");

#undef OPTKEY
#undef COPT_LONG
#undef COPT_BOOL
#undef COPT_STR
#undef COPT_JVAL

    /**
       Stores info for mapping between CURLOPT_xxx and JS-friendly names for
       various Curl options.
    */
    struct OptInfo
    {
        int ID;
        char const * PropName;
        CurlOptSetter Setter;
    };
    /**
       Each entry must have a unique ID and corresponding CurlOpt<> specialization.
       Order is irrelevant, but the list must end with an entry which has all fields
       set to 0.
    */
    static const OptInfo OptInfoList[] =
        {
#define O1(I) { CURLOPT_##I, CurlOpt<CURLOPT_##I>::Key(), CurlOpt<CURLOPT_##I>::Set }
#define O2(I,S) { CURLOPT_##I, Strings::S, CurlOpt<CURLOPT_##I>::Set }
        O1(BUFFERSIZE),
        O1(CONNECTTIMEOUT),
        O1(CRLF),
        O1(FAILONERROR),
        O1(FOLLOWLOCATION),
        O1(HEADER),
        O2(HEADERDATA, optHeaderData),
        O2(HEADERFUNCTION, optHeaderFunc),
        O1(INTERFACE),
        O1(MAXREDIRS),
        O1(NOBODY),
        O1(NOPROXY),
        O1(PORT),
        O1(PROXY),
        O1(PROXYPORT),
        O1(RANGE),
        O1(TIMEOUT),
        O1(TIMEOUT_MS),
        O1(URL),
        O1(USERAGENT),
        O1(USERNAME),
        O1(USERPWD),
        O1(VERBOSE),
        O2(WRITEDATA, optWriteData),
        O2(WRITEFUNCTION, optWriteFunc),
#undef O1
#undef O2
        {0,0,0}
        };
    typedef std::map<std::string,OptInfo const *> KeyToOptMap;
    typedef std::map<int,OptInfo const *> IntToOptMap;
    static KeyToOptMap const & keyToOpt()
    {
        static KeyToOptMap m;
        if( m.empty() )
        {
            OptInfo const * o = OptInfoList;
            for( ; o->ID; ++o )
            {
                m[o->PropName] = o;
            }
        }
        return m;
    }

    OptInfo const * optInfo( std::string const & prop )
    {
        KeyToOptMap const & m( keyToOpt() );
        KeyToOptMap::const_iterator it = m.find(prop);
        return (m.end() == it)
            ? 0
            : (*it).second;
    }
    
    static IntToOptMap const & optToProp()
    {
        static IntToOptMap m;
        if( m.empty() )
        {
            OptInfo const * o = OptInfoList;
            for( ; o->ID; ++o )
            {
                m[o->ID] = o;
            }
        }
        return m;
    }

    OptInfo const * optInfo( int id )
    {
        IntToOptMap const & m( optToProp() );
        IntToOptMap::const_iterator it = m.find(id);
        return (m.end() == it)
            ? 0
            : (*it).second;
    }
    
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
        /**
           Cleans up the libcurl resources associated with this object.
        */
        ~Impl()
        {
            curl_easy_cleanup( this->ch );
        }
        /** Returns curl_easy_perform(this->ch). */
        int EasyPerform()
        {
            return curl_easy_perform(this->ch);
        }
        /** Returns this->jself->Get("opt"), creating that object if
            needed.
        */
        v8::Handle<v8::Object> opt()
        {
#if 0
            // Reminder: we use a hidden field to avoid recursion in AccessorGetter impls.
            ValHnd ov = this->jself->GetHiddenValue( JSTR(Strings::optObjHidden) );
            v8::Handle<v8::Object> jo;
            if( ov.IsEmpty() || ! ov->IsObject() )
            {
                jo = v8::Object::New();
                this->opt( jo );
            }
            else
            {
                jo = v8::Handle<v8::Object>( v8::Object::Cast( *ov ) );
            }
            return jo;
#else
            ValHnd ov = this->jself->Get( JSTR(Strings::optObj) );
            v8::Handle<v8::Object> jo;
            if( ov.IsEmpty() || ! ov->IsObject() )
            {
                jo = v8::Object::New();
                this->opt( jo );
            }
            else
            {
                jo = v8::Handle<v8::Object>( v8::Object::Cast( *ov ) );
            }
            return jo;
#endif
        }
        /** Returns the given key from jself[Strings::optObj][k] */
        v8::Handle<v8::Value> opt( char const * k )
        {
            v8::Local<v8::String> const jk = JSTR(k);
            return this->opt()->Get( jk );
        }
        /** Sets jself[Strings::optObj] to o. */
        v8::Handle<v8::Value> opt( v8::Handle<v8::Object> o )
        {
            //this->jself->SetHiddenValue( JSTR(Strings::optObjHidden), o );
            this->jself->Set( JSTR(Strings::optObj), o );
            return o;
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
            v8::Integer::NewFromUnsigned( len ),
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
        CurlJS * rc = new CurlJS;
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
    using namespace v8::juice;
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
    
    int CurlJS::setOption( int curlID, v8::Handle<v8::Value> const & val )
    {
        //CERR << "setOption("<<curlID<<","<<cv::JSToStdString(val)<<")\n";
        OptInfo const * oi = optInfo(curlID);
        if( ! oi )
        {
            cv::StringBuffer msg;
            msg << "Unknown Curl option ID: "<<curlID;
            TOSSV(msg);
            return -1;
        }
        return oi->Setter( this->impl->opt(), this->impl->ch, cv::CastToJS(oi->PropName), val );
    }

    int CurlJS::setOption( v8::Handle<v8::Value> const & key, v8::Handle<v8::Value> const & val )
    {
        //CERR << "setOption("<<cv::JSToStdString(val)<<","<<cv::JSToStdString(val)<<")\n";
        const int curlID = cv::CastFromJS<int>(key);
        OptInfo const * oi = optInfo(curlID);
        if( oi )
        {
            return this->setOption( curlID, val );
        }
        else
        {
            oi = optInfo( cv::JSToStdString(key) );
        }
        if( ! oi )
        {
            cv::StringBuffer msg;
            msg << "Unknown Curl option: "<<cv::JSToStdString(key);
            TOSSV(msg);
            return -1;
        }
        return oi->Setter( this->impl->opt(), this->impl->ch, cv::CastToJS(oi->PropName), val );
    }

    /**
       If key is a known curl property then the registered OptInfo object
       will be used to set the option. True is returned if key is a known
       option, else true is returned if OptInfo::Setter returns 0.
     */
    static bool SetNamedCurlOption( CurlJS * c,
                                    v8::Handle<v8::Value> const & key,
                                    v8::Handle<v8::Value> const & val )
    {
        std::string const pname = cv::JSToStdString(key);
        OptInfo const * oi = optInfo( pname );
        if( ! oi )
        {
            CERR << "Warning: skipping non-Curl option '"<<cv::JSToStdString(key)<<"'.\n";
            return false;
        }
        return 0 == oi->Setter( c->impl->opt(), c->impl->ch, key, val );
    }

    uint32_t CurlJS::setOptions( v8::Handle<v8::Value> const & value )
    {
        if( value.IsEmpty() || !value->IsObject() )
        {
            cv::StringBuffer msg;
            msg << CurlJS::ClassName() << "."<<Strings::fnSetOption << "(1 argument): Argument must be-a Object.";
            TOSSV(msg);
            return 0;
        }
        v8::HandleScope hsc;
        v8::Local<v8::Object> src( v8::Object::Cast(*value) );
        v8::Handle<v8::Object> pobj( v8::Object::New() );
        this->impl->opt( pobj );
        v8::Local<v8::Array> ar = src->GetPropertyNames();
        const int arlen = cv::CastFromJS<int>( ar->Get(JSTR("length")) );
        uint32_t rc = 0;
        for( int i = 0; (i < arlen); ++i )
        {
            v8::Local<v8::Value> pkey = ar->Get( v8::Integer::New(i) );
            //this->impl->jself->Set( pkey, src->Get( pkey ) );
            if( SetNamedCurlOption( this, pkey, src->Get( pkey ) ) )
            {
                ++rc;
            }
        }
        return rc;
    }

    uint32_t CurlJS::addOptions( v8::Handle<v8::Value> const & value )
    {
        if( value.IsEmpty() || !value->IsObject() )
        {
            cv::StringBuffer msg;
            msg << CurlJS::ClassName() << "."<<Strings::fnAddOption << "(1 argument): Argument must be-a Object.";
            TOSSV(msg);
            return 0;
        }
        v8::HandleScope hsc;
        v8::Local<v8::Object> src( v8::Object::Cast(*value) );
        v8::Local<v8::Array> ar = src->GetPropertyNames();
        const int arlen = cv::CastFromJS<int>( ar->Get(JSTR("length")) );
        uint32_t rc = 0;
        for( int i = 0; (i < arlen); ++i, ++rc )
        {
            v8::Local<v8::Value> pkey = ar->Get( v8::Integer::New(i) );
            if( SetNamedCurlOption( this, pkey, src->Get( pkey ) ) )
            {
                ++rc;
            }
        }
        return rc;
    }

    /**
       v8::AccessorGetter impl.
       
       OptKey must be one of the Strings strings and must refer to one
       of the CURLOPT_xxx options.
    */
    static v8::Handle<v8::Value> OptGet( Local< String > jkey, const AccessorInfo & info )
    {
        CurlJS * c = cv::CastFromJS<CurlJS>( info.This() );
        //CERR << "OptGet("<<cv::JSToStdString(jkey)<<")@"<<(void const *)c<<"\n";
        if( ! c )
        {
            cv::StringBuffer msg;
            msg << CurlJS::ClassName() << '.'<<cv::JSToStdString(jkey)
                << " getter could not find native 'this' object!";
            return TOSSV(msg);
        }
        return c->impl->opt()->Get( jkey );
    }

//     static v8::Handle<v8::Value> OptGetter( Local< String > jkey, const AccessorInfo & info )
//     {
//         CurlJS * c = cv::CastFromJS<CurlJS>( info.This() );
//         CERR << "OptGetter("<<cv::JSToStdString(jkey)<<")@"<<(void const *)c<<"\n";
//         if( ! c )
//         {
//             cv::StringBuffer msg;
//             msg << CurlJS::ClassName() << '.'<<cv::JSToStdString(jkey)
//                 << " getter could not find native 'this' object!";
//             return TOSSV(msg);
//         }
//         return c->impl->opt();
//     }
    /**
       v8::AccessorSetter impl.
       
       CurlOptID must be one of the supported CURLOPT_xxx values and
       jkey is expected to be equal to one of the Strings members.

       CurlOpt<CurlOptID> must be specialized.
    */
    template <int CurlOptID>//, char const * & OptKey>
    static void OptSet(v8::Local< v8::String > jkey,
                       v8::Local< v8::Value > value,
                       const v8::AccessorInfo &info)
    {
        CurlJS * c = cv::CastFromJS<CurlJS>( info.This() );
        //CERR << "OptSet<"<<CurlOptID<<','<<cv::JSToStdString(jkey)<<">()@"<<(void const *)c<<" = "<<cv::JSToStdString(value)<<"\n";
        if( ! c )
        {
            cv::StringBuffer msg;
            msg << CurlJS::ClassName() << '.'<<cv::JSToStdString(jkey)
                << " setter could not find native 'this' object!";
            TOSSV(msg);
            return;
        }
        int check = CurlOpt<CurlOptID>::Set( c->impl->opt(), c->impl->ch, jkey, value );
        if( 0 != check )
        {
            cv::StringBuffer msg;
            msg << "Error setting Curl property '"
                << cv::JSToStdString(jkey) << "' (#"<<CurlOptID<<").";
            TOSSV(msg);
            return;
        }
    }

    /** Calls curl_global_init() at construction and
        curl_global_cleanup() at destruction.
    */
    struct CurlGlobalInitializer
    {
        CurlGlobalInitializer()
        {
            curl_global_init(CURL_GLOBAL_ALL);
        }
        ~CurlGlobalInitializer()
        {
            curl_global_cleanup();
        }
    };
    
    v8::Handle<v8::Value> CurlJS::SetupBindings( v8::Handle<v8::Object> target )
    {
        {
            /**
               Initialize these now, when we are almost certain to be running
               under a mutex (via v8::juice::plugin::LoadPlugin()) or otherwise
               not in multiple threads.
            */
            keyToOpt();
            optToProp();
        }

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


        static const CurlGlobalInitializer curlResources;
        /** ^^^ We have no real way of knowing if this was already done, or if we should
            clean up!
        */
        typedef tmp::TypeList<
            convert::InvocableMemFunc1<N,uint32_t,ValHnd const &,&N::setOptions>,
            convert::InvocableMemFunc2<N,int,ValHnd const &,ValHnd const &,&N::setOption>
            > SetOptList;
        cw
            .Set( Strings::easyPerform, ICM::M0::Invocable<int,&N::EasyPerform> )
            .Set( "toString", ICM::M0::Invocable<ValHnd,&N::toString> )
            .Set( Strings::fnAddOption, convert::InvocableMemFunc1<N,uint32_t,ValHnd const &,&N::addOptions>::Invocable )
            .Set( "destroy", CW::DestroyObject )
            //.Set( Strings::optObj, OptGetter ) // if i do this the opts are not enumerable!
            ;
         cw.Set( Strings::fnSetOption, convert::OverloadInvocables<SetOptList>::Invocable );

        
         if(0) // we currently need these so that setOptions can dispatch to the proper handler...
         {
            v8::Handle<v8::ObjectTemplate> proto = cw.Prototype();
            // Add curlObj.XXX interceptors for the supported CURLOPT_xxx options:
#define ACC(I) \
            proto->SetAccessor( JSTR(CurlOpt<CURLOPT_ ## I>::Key()), \
                                OptGet, OptSet<CURLOPT_ ## I>, \
                                ValHnd(), v8::DEFAULT, v8::DontEnum )

            ACC(BUFFERSIZE);
            ACC(CONNECTTIMEOUT);
            ACC(CRLF);
            ACC(FAILONERROR);
            ACC(FOLLOWLOCATION);
            ACC(HEADER);
            ACC(HEADERDATA);
            ACC(HEADERFUNCTION);
            ACC(INTERFACE);
            ACC(LOW_SPEED_LIMIT);
            ACC(LOW_SPEED_TIME);
            ACC(MAXREDIRS);
            ACC(NOBODY);
            ACC(NOPROXY);
            ACC(PORT);
            ACC(PROXY);
            ACC(RANGE);
            ACC(RESUME_FROM);
            ACC(TIMEOUT);
            ACC(TIMEOUT_MS);
            ACC(URL);
            ACC(USERAGENT);
            ACC(USERNAME);
            ACC(USERPWD);
            ACC(VERBOSE);
            ACC(WRITEDATA);
            ACC(WRITEFUNCTION);
#undef ACC
        }

        v8::Handle<v8::Function> ctor = cw.Seal();
        cw.AddClassTo(target);

        /**
           Add Curl.OPT_XXX mappings to CURLOPT_XXX.

           Maintenance reminder: we cannot combine this with the above ACC()
           bits because of order-of-operations requirements on the object
           creation process. If we fetch the ctor object we can no longer
           change the prototype template, so we cannot do this stuff until
           cw.Seal() has been called and cannot do the ACC() stuff after
           cw.Seal() has been called.
        */
#define OPTKEY(O) ctor->Set( JSTR("OPT_"#O), v8::Integer::New( CURLOPT_ ## O ) )
        OPTKEY(BUFFERSIZE);
        OPTKEY(CONNECTTIMEOUT);
        OPTKEY(CRLF);
        OPTKEY(FAILONERROR);
        OPTKEY(FOLLOWLOCATION);
        OPTKEY(HEADER);
        OPTKEY(HEADERDATA);
        OPTKEY(HEADERFUNCTION);
        OPTKEY(INTERFACE);
        OPTKEY(LOW_SPEED_LIMIT);
        OPTKEY(LOW_SPEED_TIME);
        OPTKEY(MAXREDIRS);
        OPTKEY(NOBODY);
        OPTKEY(NOPROXY);
        OPTKEY(PORT);
        OPTKEY(PROXY);
        OPTKEY(RANGE);
        OPTKEY(RESUME_FROM);
        OPTKEY(TIMEOUT);
        OPTKEY(TIMEOUT_MS);
        OPTKEY(URL);
        OPTKEY(USERAGENT);
        OPTKEY(USERNAME);
        OPTKEY(USERPWD);
        OPTKEY(VERBOSE);
        OPTKEY(WRITEDATA);
        OPTKEY(WRITEFUNCTION);
#undef OPTKEY
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
