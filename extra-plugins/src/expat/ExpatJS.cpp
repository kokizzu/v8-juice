#include "ExpatJS.h"

#include <v8/juice/convert.h>
#include <v8/juice/plugin.h>

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

namespace v8 { namespace juice { namespace expat {
    static void XMLCALL start_node( void * ud, const char * name, const char ** attr );
    static void XMLCALL end_node( void * ud, const char * );
    static void XMLCALL char_handler( void * ud, const char * txt, int len );
    static v8::Handle<v8::Value> StartNodeNoop( v8::Arguments const & argv );
    static v8::Handle<v8::Value> EndNodeNoop( v8::Arguments const & argv );
    static v8::Handle<v8::Value> CharHandlerNoop( v8::Arguments const & argv );

    struct ExpatImpl
    {
        XML_Parser ps;
        PFuncHnd startNode;
        PFuncHnd endNode;
        PFuncHnd charHandler;
        v8::Handle<v8::Object> jself; // we assume this is Persistent elsewhere
        PValHnd udata;
        ExpatImpl()
            : ps(0),
#define PF(X) PFuncHnd::New( v8::FunctionTemplate::New(X)->GetFunction() )
              startNode( PF(StartNodeNoop) ),
              endNode( PF(EndNodeNoop) ),
              charHandler( PF(CharHandlerNoop) ),
#undef PF
              jself(),
              udata(v8::Undefined())
        {
            this->reset();
            //CERR << "ExpatImpl() @"<<(void const *)this<<'\n';
        }
        ~ExpatImpl()
        {
            //CERR << "~ExpatImpl() @"<<(void const *)this<<'\n';
            XML_ParserFree(ps);
        }

        void reset()
        {
            if( ! this->ps )
            {
                this->ps = XML_ParserCreate(NULL);
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
           A setter proxy function for a given PFuncHnd member
           property. Sets this->*Mem to func iff func->IsFunction(),
           else it throws a JS-side exception.
        */
        template <PFuncHnd ExpatImpl::*Mem>
        void setHandler( v8::Handle<v8::Value> func )
        {
            if( func.IsEmpty() || !func->IsFunction() )
            {
                TOSS("XML parser function must be-a Function!");
                return;
            }
            //(this->*Mem).Clear();
            this->*Mem = PFuncHnd::New( FuncHnd( v8::Function::Cast(*func) ) );
        }
    };

}}} // v8::juice::expat
namespace v8 { namespace juice { namespace cw {
    using v8::juice::expat::ExpatJS;
    using namespace ::v8::juice;
    char const * ClassName<ExpatJS>::Value()
    {
        return "ExpatParser";
    }

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
        : impl(new ExpatImpl)
    {
    }
    ExpatJS::~ExpatJS()
    {
        delete this->impl;
    }

    void ExpatJS::setStartNode( v8::Handle<v8::Value> func )
    {
        this->impl->setHandler<&ExpatImpl::startNode>( func );
    }
    ValHnd ExpatJS::getStartNode() const
    {
        return this->impl->startNode;
    }
    void ExpatJS::setEndNode( v8::Handle<v8::Value> func )
    {
        this->impl->setHandler<&ExpatImpl::endNode>( func );
    }
    ValHnd ExpatJS::getEndNode() const
    {
        return this->impl->endNode;
    }
    void ExpatJS::setCharHandler( v8::Handle<v8::Value> func )
    {
        this->impl->setHandler<&ExpatImpl::charHandler>( func );
    }
    ValHnd ExpatJS::getCharHandler() const
    {
        return this->impl->charHandler;
    }
    void ExpatJS::userData( v8::Handle<v8::Value> d )
    {
        //CERR << "userData ="<<cv::JSToStdString(d)<<'\n';
        this->impl->udata = PValHnd::New( d );
    }
    v8::Handle<v8::Value> ExpatJS::userData() const
    {
        return this->impl->udata;
    }

    void ExpatJS::Reset()
    {
        this->impl->reset();
    }
    bool ExpatJS::Parse( std::string const & part, bool finished )
    {
        if( part.empty() ) return true;
        char const * inp = part.c_str();
        bool rc = true;
        if( XML_STATUS_ERROR == XML_Parse( this->impl->ps, inp, static_cast<int>(part.size()), finished ? 1 : 0 ) )
        {
            cv::StringBuffer msg;
            msg << "Parse error at line "
                << XML_GetCurrentLineNumber(this->impl->ps)
                << ": "
                << XML_ErrorString(XML_GetErrorCode(this->impl->ps))
                << ": input=["<<part<<"]";
            TOSSV(msg);
            this->impl->reset();
            rc = false;
        }
        if( finished )
        {
            this->impl->reset();
        }
        return rc;
    }
    
    v8::Handle<v8::Value> StartNodeNoop( v8::Arguments const & argv )
    {
        //CERR << "StartNodeNoop() should be re-implemented!\n";
        return v8::Undefined();
    }

    static void XMLCALL start_node( void * ud, const char * name, const char ** attr )
    {
        //CERR << "start_node()\n";
        ExpatImpl * impl;
        impl = static_cast<ExpatImpl *>( ud );
        if( impl->startNode.IsEmpty() ) return;
        typedef std::map< std::string, std::string > MapT;
        MapT map;
#if 1
        char const * key = 0;
        char const * val = 0;
        for( int i = 0; attr[i]; i += 2 )
        {
            key = attr[i];
            val = attr[1+i];
            map[key] = val ? val : "";
        }
#endif
        enum { avlen = 3 };
        ValHnd av[avlen] = {
            impl->udata,
            cv::CastToJS( name ),
            map.empty() ? v8::Object::New() : cv::CastToJS( map )
        };
        impl->startNode->Call( impl->jself, avlen, av );
    }
    v8::Handle<v8::Value> EndNodeNoop( v8::Arguments const & argv )
    {
        //CERR << "EndNodeNoop() should be re-implemented!\n";
        return v8::Undefined();
    }
    static void XMLCALL end_node( void * ud, const char * name )
    {
        //CERR << "end_node()\n";
        ExpatImpl * impl;
        impl = static_cast<ExpatImpl *>( ud );
        if( impl->endNode.IsEmpty() ) return;
        enum { avlen = 2 };
        ValHnd av[avlen] = {
            impl->udata,
            cv::CastToJS( name )
        };
        impl->endNode->Call( impl->jself, avlen, av );
    }
    v8::Handle<v8::Value> CharHandlerNoop( v8::Arguments const & argv )
    {
        //CERR << "charHandler() should be re-implemented!\n";
        return v8::Undefined();
    }
    static void XMLCALL char_handler( void * ud, const char * txt, int len )
    {
        //CERR << "char_handler()\n";
        ExpatImpl * impl;
        impl = static_cast<ExpatImpl *>( ud );
        if( impl->charHandler.IsEmpty() ) return;
        enum { avlen = 3 };
        ValHnd av[avlen] = {
            impl->udata,
            v8::String::New(txt,len),
            cv::CastToJS( len )
        };
        impl->charHandler->Call( impl->jself, avlen, av );
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

        cw.BindGetterSetter<ValHnd,&N::getStartNode,void,ValHnd,&N::setStartNode>("startNode");
        cw.BindGetterSetter<ValHnd,&N::getEndNode,void,ValHnd,&N::setEndNode>("endNode");
        cw.BindGetterSetter<ValHnd,&N::getCharHandler,void,ValHnd,&N::setCharHandler>("charHandler");
        cw.BindGetterSetter<ValHnd,&N::userData,void,ValHnd,&N::userData>("userData");
        
#define JF v8::FunctionTemplate::New(cb)->GetFunction()
#define F(X) cw.Set( X, JF )
        v8::InvocationCallback cb;
        cb = CW::DestroyObject;
        F("destroy");
        cb = ICM::M2::Invocable<bool,std::string const &,bool,&N::Parse>;
        F("parse");
        cb = ICM::M0::Invocable<void,&N::Reset>;
        F("reset");
        
        cw.Seal();
        cw.AddClassTo(target);
#undef JF
#undef F
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
