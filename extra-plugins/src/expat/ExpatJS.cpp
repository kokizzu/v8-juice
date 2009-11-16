#include "ExpatJS.h"

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

namespace v8 { namespace juice {
namespace cw {
    char const * ClassName<v8::juice::expat::ExpatJS>::Value()
    {
        return "ExpatParser";
    }
}
namespace expat {
    namespace cv = v8::juice::convert;
    static void XMLCALL start_node( void * ud, const char * name, const char ** attr );
    static void XMLCALL end_node( void * ud, const char * );
    static void XMLCALL char_handler( void * ud, const char * txt, int len );
    static v8::Handle<v8::Value> StartNodeNoop( v8::Arguments const & argv );
    static v8::Handle<v8::Value> EndNodeNoop( v8::Arguments const & argv );
    static v8::Handle<v8::Value> CharHandlerNoop( v8::Arguments const & argv );

    /**
       Holder for names of standard callback functions.
    */
    struct Strings
    {
        static char const * startNode;
        static char const * endNode;
        static char const * charHandler;
        static char const * userData;
    };
    char const * Strings::startNode = "startNode";
    char const * Strings::endNode = "endNode";
    char const * Strings::charHandler = "charHandler";
    char const * Strings::userData = "userData";
    
    struct ExpatJS::Impl
    {
        XML_Parser ps;
        typedef std::map<char const *, PFuncHnd> CBMap;
        CBMap cb;
        v8::Handle<v8::Object> jself; // we assume this is Persistent elsewhere
        PValHnd udata;
        Impl()
            : ps(0),
              cb(),
              jself(),
              udata(v8::Undefined())
        {
#define PF(X) PFuncHnd::New( v8::FunctionTemplate::New(X)->GetFunction() )
#define SF(Name,Func) cb[Name] = PF(Func)
            SF("startNode", StartNodeNoop);
            SF("endNode", EndNodeNoop );
            SF("charHandler",CharHandlerNoop);
#undef SF
#undef PF
            this->reset();
            //CERR << "Impl() @"<<(void const *)this<<'\n';
        }
        ~Impl()
        {
            //CERR << "~Impl() @"<<(void const *)this<<'\n';
            XML_ParserFree(ps);
        }
        v8::Handle<v8::Value> userData()
        {
            const v8::Handle<v8::Value> rc = this->jself->Get( JSTR(Strings::userData) );
            if( rc.IsEmpty() ) return v8::Undefined();
            else return rc;
        }
        void reset()
        {
            if( ! this->ps )
            {
                this->ps = XML_ParserCreate(NULL);
                if( ! this->ps )
                {
                    throw std::runtime_error("XML_ParserCreate() failed (OOM?)");
                }
            }
            else
            {
                XML_ParserReset( this->ps, NULL );
            }
            XML_SetElementHandler( this->ps, start_node, end_node );
            XML_SetCharacterDataHandler( this->ps, char_handler );
            XML_SetUserData( this->ps, this );
        }

        /**
           Installs the handler for a given callback routine. If
           !func->IsFunction() it throws a JS-side exception.
        */
        void setHandler( char const * n, v8::Handle<v8::Value> func )
        {
            if( func.IsEmpty() || !func->IsFunction() )
            {
                cv::StringBuffer msg;
                msg << v8::juice::cw::ClassName<ExpatJS>::Value() << '.' << n
                    << " must be-a Function!";
                TOSSV(msg);
                return;
            }
            this->cb[n] = PFuncHnd::New( FuncHnd( v8::Function::Cast(*func) ) );
        }
    };

}}} // v8::juice::expat

namespace v8 { namespace juice { namespace cw {
    //using v8::juice::expat::ExpatJS;
    using namespace v8::juice::expat;
    using namespace ::v8::juice;

    void WeakWrap<ExpatJS>::Wrap( v8::Persistent<v8::Object> const & jsSelf, ExpatJS * nativeSelf )
    {
#if V8_JUICE_EXPAT_USE_TWOWAY_WRAP
        WeakWrapBase::Wrap( jsSelf, nativeSelf );
#endif
        nativeSelf->impl->jself = jsSelf;
    }

    void WeakWrap<ExpatJS>::Unwrap( v8::Handle<v8::Object> const & jsSelf, ExpatJS * nativeSelf )
    {
#if V8_JUICE_EXPAT_USE_TWOWAY_WRAP
        WeakWrapBase::Unwrap( jsSelf, nativeSelf );
#endif
        nativeSelf->impl->jself.Clear();
    }

} } } // v8::juice::cw



namespace v8 { namespace juice { namespace expat {
    namespace cv = v8::juice::convert;

    ExpatJS::ExpatJS()
        : impl(new ExpatJS::Impl)
    {
    }
    ExpatJS::~ExpatJS()
    {
        delete this->impl;
    }
    
    void ExpatJS::Reset()
    {
        this->impl->reset();
    }
    bool ExpatJS::Parse( std::string const & chunk, bool finished )
    {
        if( chunk.empty() ) return true;
        char const * inp = chunk.c_str();
        bool rc = true;
        if( XML_STATUS_ERROR == XML_Parse( this->impl->ps,
                                           inp,
                                           static_cast<int>(chunk.size()),
                                           finished ? 1 : 0 ) )
        {
            rc = false;
            cv::StringBuffer msg;
            msg << "Parse error at line "
                << XML_GetCurrentLineNumber(this->impl->ps)
                << ": "
                << XML_ErrorString(XML_GetErrorCode(this->impl->ps))
                << ": input=["<<chunk<<"]";
            TOSSV(msg);
            this->impl->reset();
        }
        if( finished )
        {
            this->impl->reset();
        }
        return rc;
    }
    
    static v8::Handle<v8::Value> StartNodeNoop( v8::Arguments const & argv )
    {
        //CERR << "StartNodeNoop() should be re-implemented!\n";
        return v8::Undefined();
    }

    static void XMLCALL start_node( void * ud, const char * name, const char ** attr )
    {
        //CERR << "start_node()\n";
        ExpatJS::Impl * impl;
        impl = static_cast<ExpatJS::Impl *>( ud );
        //if( impl->startNode.IsEmpty() ) return;
        PFuncHnd cb = impl->cb[Strings::startNode];
        typedef std::map< std::string, std::string > MapT;
        v8::Handle<v8::Object> oattr;
        char const * key = 0;
        char const * val = 0;
        if( attr && attr[0] )
        {
            oattr = v8::Handle<v8::Object>( v8::Object::New() );
            cv::ObjectPropSetter set(oattr);
            for( int i = 0; attr[i]; i += 2 )
            {
                key = attr[i];
                val = attr[1+i];
                set( key, val ? val : "");
            }
        }
        enum { avlen = 3 };
        ValHnd av[avlen] = {
            impl->userData(),
            cv::CastToJS( name ),
            v8::Undefined()
        };
        if( ! oattr.IsEmpty() ) av[2] = oattr;
        cb->Call( impl->jself, avlen, av );
    }

    static v8::Handle<v8::Value> EndNodeNoop( v8::Arguments const & argv )
    {
        //CERR << "EndNodeNoop() should be re-implemented!\n";
        return v8::Undefined();
    }

    static void XMLCALL end_node( void * ud, const char * name )
    {
        ExpatJS::Impl * impl;
        impl = static_cast<ExpatJS::Impl *>( ud );
        PFuncHnd cb = impl->cb[Strings::endNode];
        if( cb.IsEmpty() ) return;
        enum { avlen = 2 };
        ValHnd av[avlen] = {
            impl->userData(),
            cv::CastToJS( name )
        };
        cb->Call( impl->jself, avlen, av );
    }

    static v8::Handle<v8::Value> CharHandlerNoop( v8::Arguments const & argv )
    {
        //CERR << "charHandler() should be re-implemented!\n";
        return v8::Undefined();
    }

    static void XMLCALL char_handler( void * ud, const char * txt, int len )
    {
        ExpatJS::Impl * impl;
        impl = static_cast<ExpatJS::Impl *>( ud );
        PFuncHnd cb = impl->cb[Strings::charHandler];
        if( cb.IsEmpty() ) return;
        enum { avlen = 3 };
        ValHnd av[avlen] = {
            impl->userData(),
            v8::String::New(txt,len),
            cv::CastToJS( len )
        };
        cb->Call( impl->jself, avlen, av );
    }

    template <char const *&PropName>
    v8::Handle<v8::Value> ExpatJS::jsCBGetter()
    {
        Impl::CBMap::const_iterator it = this->impl->cb.find( PropName );
        if( this->impl->cb.end() == it ) return v8::Undefined();
        return it->second;
    }
    template <char const *&PropName>
    void ExpatJS::jsCBSetter( v8::Handle<v8::Value> const & v )
    {
        this->impl->setHandler( PropName, v );
    }

    v8::Handle<v8::Value> ExpatJS::SetupBindings( v8::Handle<v8::Object> target )
    {
        typedef ExpatJS N;
        typedef cw::ClassWrap<N> CW;
        CW & cw( CW::Instance() );
        if( cw.IsSealed() )
        {
            cw.AddClassTo(target);
            return target;
        }
        typedef convert::InvocationCallbackCreator ICC;
        typedef convert::MemFuncInvocationCallbackCreator<N> ICM;

        //cw.BindGetterSetter<ValHnd,&N::startNode,void,ValHnd,&N::startNode>(Strings::startNode);
#define GETSET(Name) \
        cw.BindGetterSetter< \
            ValHnd, &N::jsCBGetter<Name>, \
            void, ValHnd const &, &N::jsCBSetter<Name> \
            >(Name);
        GETSET(Strings::startNode);
        GETSET(Strings::endNode);
        GETSET(Strings::charHandler);
#undef GETSET
        cw.Prototype()->Set( JSTR(Strings::userData), v8::Undefined() );
        
#define JF v8::FunctionTemplate::New(cb)->GetFunction()
#define F(X) cw.Set( X, JF )
        v8::InvocationCallback cb;

        cb = CW::DestroyObject;
        F("destroy");

        cb = ICM::M2::Invocable<bool,std::string const &,bool,&N::Parse>;
        F("parse");

        cb = ICM::M0::Invocable<void,&N::Reset>;
        F("reset");
#undef JF
#undef F
        cw.Seal();
        cw.AddClassTo(target);
        return target;
    }

} } } // v8::juice::expat

namespace {
    v8::Handle<v8::Value> ExpatPluginInit( v8::Handle<v8::Object> target )
    {
        return v8::juice::expat::ExpatJS::SetupBindings(target);
    }
}
    V8_JUICE_PLUGIN_STATIC_INIT(ExpatPluginInit);


#undef JSTR
#undef TOSS
#undef TOSSV
