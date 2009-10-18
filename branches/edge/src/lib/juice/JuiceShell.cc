#include <v8/juice/JuiceShell.h>
#include <v8/juice/convert.h>

#include <v8/juice/sprintf.h>
#include <v8/juice/plugin.h>
#include <v8/juice/time.h>

#include <vector>
#include <string>
#include <iostream> // cout/cerr
#include <sstream>
#include <iterator>

#if 0
#ifndef CERR
#include <iostream>
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif
#endif

namespace v8 {
namespace juice {

    namespace Detail
    {
        static void DefaultErrorMessageReporter( char const * msg )
        {
            if( msg && *msg ) std::cerr << msg << '\n';
        }
        class JuiceShellImpl
        {
        public:
            //v8::HandleScope handle_scope;
            //v8::juice::cleanup::CleanupSentry cleaner;
            v8::Handle<v8::ObjectTemplate> globt;
            v8::Handle<v8::Context> context;
            v8::Context::Scope context_scope;
            v8::Handle<v8::Object> global;
            v8::juice::JuiceShell::ErrorMessageReporter reporter;
            JuiceShellImpl()
                :
                //handle_scope(),
                globt( v8::ObjectTemplate::New() ),
                context( v8::Context::New(NULL, globt) ),
                context_scope(context),
                global( context->Global() ),
                reporter( DefaultErrorMessageReporter )
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
        int i = (int)offset;
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

    void JuiceShell::SetExecuteErrorReporter( ErrorMessageReporter r )
    {
        this->impl->reporter = r;
    }
    
    void JuiceShell::ReportException(v8::TryCatch* try_catch)
    {
        if( ! this->impl->reporter ) return;

        v8::HandleScope handle_scope;
        v8::String::Utf8Value exception(try_catch->Exception());
        //std::string const exception_stdstr = convert::CastFromJS<std::string>( exception
#define TOCSTR(X) (*X ? *X : "<exception string conversion failed!>")
        const char* exception_string = TOCSTR(exception);
        v8::Handle<v8::Message> message = try_catch->Message();
        std::ostringstream os;
        if (message.IsEmpty())
        {
            // V8 didn't provide any extra information about this error; just
            // print the exception.
            os << exception_string << '\n';
        }
        else
        {
            // Print (filename):(line number): (message).
            v8::String::Utf8Value filename(message->GetScriptResourceName());
            const char* filename_string = TOCSTR(filename);
            int linenum = message->GetLineNumber();
            os << filename_string << ':'
               << linenum << ": "
               << exception_string << '\n';
            // Print line of source code.
            v8::String::Utf8Value sourceline(message->GetSourceLine());
            const char* sourceline_string = TOCSTR(sourceline);
#undef TOCSTR
            os << sourceline_string << '\n';
            // Print wavy underline (GetUnderline is deprecated).
            int start = message->GetStartColumn();
            for (int i = 0; i < start; i++) {
                os << ' ';
            }
            int end = message->GetEndColumn();
            for (int i = start; i < end; i++) {
                os << '^';
            }
            os << '\n';
        }
        std::string const & str = os.str();
        this->impl->reporter( str.c_str() );
    }


    bool JuiceShell::ExecuteString(v8::Handle<v8::String> source,
                                   v8::Handle<v8::Value> name,
                                   std::ostream * out,
                                   bool reportExceptions)
    {
        v8::HandleScope handle_scope;
        v8::TryCatch try_catch;
        v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
        if (script.IsEmpty())
        {
            // Print errors that happened during compilation.
            if (reportExceptions)
                this->ReportException(&try_catch);
            return false;
        }
        else
        {
            v8::Handle<v8::Value> result = script->Run();
            if (result.IsEmpty())
            {
                if (reportExceptions)
                {
                    this->ReportException(&try_catch);
                }
                return false;
            }
            else
            {
                if (out && !result->IsUndefined())
                {
                    std::string str = convert::JSToStdString(result);
                    (*out) << str << '\n';
                }
                return true;
            }
        }
    }
    bool JuiceShell::ExecuteString(std::string const & source,
                                   std::string const & name,
                                   std::ostream * resultGoesTo,
                                   bool reportExceptions)
    {
        v8::HandleScope scope;
        Local<v8::String> s( v8::String::New( source.c_str(), static_cast<int>(source.size()) ) );
        Local<v8::String> n( v8::String::New( name.c_str(), static_cast<int>(name.size()) ) );
        return this->ExecuteString( s, n, resultGoesTo, reportExceptions );
    }

    void JuiceShell::StdinLineFetcher( std::string & dest, bool & breakOut )
    {
        breakOut = false;
        std::cout << "> ";
        std::cout.flush();
        std::ostringstream os;
        std::istream & is( std::cin );
        is >> std::noskipws;
        char x;
        size_t pos = 0;
        while(true)
        {
            is >> x;
            if( is.eof() )
            {
                breakOut = true;
                break;
            }
            if( ('\n' == x) || ('\r' == x) ) break;
            if( !(pos++) && !x )
            {
                breakOut = true;
                break;
            }
            os << x;
        }
        dest = os.str();
        return;
    }
    
    void JuiceShell::InputLoop( LineFetcher input,
                                std::ostream * os,
                                bool reportExceptions )
    {
        std::string sbuf;
        bool dobreak = false;
        while( true )
        {
            sbuf.clear();
            input( sbuf, dobreak );
            if( dobreak )
            {
                break;
            }
            v8::HandleScope handle_scope;
            this->ExecuteString(sbuf, "(JuiceShell::InputLoop())", os, true);
        }
    }

    
}} // namespaces
