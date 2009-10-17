#include <v8/juice/JuiceShell.h>
#include <v8/juice/convert.h>

#include <v8/juice/sprintf.h>
#include <v8/juice/plugin.h>
#include <v8/juice/time.h>

#include <vector>
#include <string>
namespace v8 {
namespace juice {

    namespace Detail
    {
        class JuiceShellImpl
        {
        public:
            //v8::HandleScope handle_scope;
            //v8::juice::cleanup::CleanupSentry cleaner;
            v8::Handle<v8::ObjectTemplate> globt;
            v8::Handle<v8::Context> context;
            v8::Context::Scope context_scope;
            v8::Handle<v8::Object> global;
            JuiceShellImpl()
                :
                //handle_scope(),
                globt( v8::ObjectTemplate::New() ),
                context( v8::Context::New(NULL, globt) ),
                context_scope(context),
                global( context->Global() )
            {
            }
            ~JuiceShellImpl()
            {
            }
        };
    }

    JuiceShell::JuiceShell( char const * globalObjectName )
        : impl( new Detail::JuiceShellImpl )
    {
        if( globalObjectName && *globalObjectName )
        {
            impl->global->Set( v8::String::New(globalObjectName), impl->global );
        }
    }
    JuiceShell::~JuiceShell()
    {
        delete this->impl;
    }

    void JuiceShell::ProcessMainArgv( int argc, char const * const * _argv, unsigned short offset )
    {
        if( (argc<1) || !_argv ) return;
        std::string const endofargs("--");
        typedef std::vector<std::string> ArgvList;
        ArgvList argv;
        {
            int i = 1;
            for( ; i < argc; ++i )
            {
                if( endofargs == _argv[i] )
                {
                    ++i;
                    break;
                }
            }
            for( ; i < argc; ++i )
            {
                argv.push_back( _argv[i] );
            }
        }
        impl->global->Set( v8::String::New("arguments"), v8::juice::convert::CastToJS( argv ) );
    }

    void JuiceShell::AddGlobalFunc( char const * name, v8::Handle<v8::Function> const & f )
    {
        impl->global->Set( v8::String::New(name), f );
    }

    void JuiceShell::AddGlobalFunc( char const * name, v8::Handle<v8::FunctionTemplate> const & f )
    {
        this->AddGlobalFunc( name, f->GetFunction() );
    }

    void JuiceShell::AddGlobalFunc( char const * name, v8::InvocationCallback f )
    {
        this->AddGlobalFunc( name, FunctionTemplate::New(f) );
    }

    v8::Handle<v8::Object> JuiceShell::Global()
    {
        return impl->global;
    }
    v8::Handle<v8::Context> JuiceShell::Context()
    {
        return impl->context;
    }

    void JuiceShell::SetupJuiceEnvironment()
    {

#define BIND(K,V) this->AddGlobalFunc( K, V )
        BIND("include", v8::juice::IncludeScript );
        BIND("loadPlugin", v8::juice::plugin::LoadPlugin);
        BIND("sprintf", v8::juice::sprintf);
        BIND("sleep", v8::juice::sleep);
        BIND("mssleep", v8::juice::mssleep);
        BIND("usleep", v8::juice::usleep);
        BIND("setTimeout", v8::juice::setTimeout);
        BIND("setInterval", v8::juice::setInterval);
        BIND("clearTimeout", v8::juice::clearTimeout);
        BIND("clearInterval", v8::juice::clearInterval);
#undef BIND
    }
    
}} // namespaces
