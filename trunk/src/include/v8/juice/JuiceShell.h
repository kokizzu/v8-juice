#if !defined(CODE_GOOGLE_COM_V8_JUICE_JUICESHELL_H_INCLUDED)
#define CODE_GOOGLE_COM_V8_JUICE_JUICESHELL_H_INCLUDED 1

#include <v8/juice/juice.h>
#include <ostream>

namespace v8 {
namespace juice {

    namespace Detail
    {
        /** Internal impl details for JuiceShell. */
        class JuiceShellImpl;
    }
    /**
       JuiceShell is a very basic wrapper to simplify the
       initialization of the v8 environment at application startup. It
       is used like:

       @code
       int main(int argc, char * argv[])
       {
           v8::V8::SetFlagsFromCommandLine(&argc,argv,false);
           v8::HandleScope handle_scope;
           // OPTIONAL: v8::juice::cleanup::CleanupSentry cleaner;
           v8::juice::JuiceShell shell;
           shell.ProcessMainArgv(argc,argv,1);
           // OPTIONAL: shell.SetupJuiceEnvironment();
           ... use v8 ...
       }
       @endcode

       Not that v8::V8::SetFlagsFromCommandLine(), if used, should come before
       the JuiceShell object and that a HandleScope should be created before the
       JuiceShell is created. The HandleScope is not created as part of the shell
       because the GC/ownership of handles generated from nested instances is not
       clear in this context.

       These objects should not be created on the heap, as this can
       theoretically play havok with handle scoping/lifetime rules.

       Warning: v8::V8::SetFlagsFromCommandLine(), if used, should be called before
       v8 is set up, but it may change the argument list. e.g. it strips out
       all args at or after "--". Thus it cannot be used directly with this
       class unless the arguments vector is first copied (e.g. into a vector<char*>)
       for use with this class or SetFlagsFromCommandLine().

       
       TODOs:

       - Functions for running arbitrary script code or executing
       scripts from files.

       - An input loop mechanism, with a callback for fetching input from
       the user (e.g. for implementing an interactive shell).
    */
    class JuiceShell
    {
    private:
        Detail::JuiceShellImpl * impl;
        void ReportException(v8::TryCatch* try_catch);
    public:
        /**
           A callback function signature for reporing JS-side
           exception messages to the native world.
        */
        typedef void (*ErrorMessageReporter)( char const * msg );

        /**
           LineFetcher is an interface for reading JS input
           interactively from the user, for use in an input loop.
           
           Reads a line of input from the user and stores it in dest.
           If it sets breakOut to true then the caller should stop
           accepting input. e.g. this should be set when Ctrl-D is
           tapped on most terminals. It should assume that dest is
           initially empty.
        */
        typedef void (*LineFetcher)( std::string & dest, bool & breakOut );


        /**
           Initialize a v8 context and global object belonging to this object.

           If globalObjectName is not null and not empty then the global object
           is given a refernce to itself using the given name, such that client
           JS code can then refer to it.
        */
        explicit JuiceShell( char const * globalObjectName = 0 );
        /**
           Takes this object's v8 resources out of scope, allowing v8
           to free them (when (or whether or not) it will do so is
           another question entirely).
        */
        ~JuiceShell();

        /** Binds the given function to the global object. */
        void AddGlobalFunc( char const * name, v8::Handle<v8::Function> const & f );
        
        /** Binds the given function to the global object. */
        void AddGlobalFunc( char const * name, v8::Handle<v8::FunctionTemplate> const & f );
        
        /** Binds the given function to the global object. */
        void AddGlobalFunc( char const * name, v8::InvocationCallback f );
        
        /** This object's global object. Owned by this object. */
        v8::Handle<v8::Object> Global();
        
        /** This object's context. Owned by this object. */
        v8::Handle<v8::Context> Context();

        /**
           Intended to be called from main() and passed the argc/argv
           which are passed to main. offset is the number of arguments
           to skip, and defaults to one to skip the argv[0] argument,
           which is conventionally the application name.

           It skips all arguments up to "--". For each argument after
           "--", it adds the argument to a list. At the end of the
           list, the global object is assigned a property named
           "arguments" which contains that list.

           This function does no interpretation of the arguments.
        */
        void ProcessMainArgv( int argc, char const * const * argv, unsigned short offset = 1 );

        /**
           Sets up a collection of v8-juice-pseodostandard properties
           and functions.

           The following functions are added to the global object:

           - include() (v8::juice::IncludeScript())
           - loadPlugin() (v8::juice::plugin::LoadPlugin())
           - sprintf() (v8::juice::sprintf())
           - sleep() (v8::juice::sleep())
           - mssleep() (v8::juice::mssleep())
           - usleep() (v8::juice::usleep())
           - setTimeout() (v8::juice::setTimeout())
           - setInterval() (v8::juice::setInterval())
           - clearTimeout() (v8::juice::clearTimeout())
           - clearInterval() (v8::juice::clearInterval())

           The following properties are added to the global object:

           - none yet
        */
        void SetupJuiceEnvironment();

        /**
           Sets the error reporter function used by
           ExecuteString(). Passing 0 will disable exception
           reporting. The default reporter sends its output to
           std::cerr.
        */
        void SetExecuteErrorReporter( ErrorMessageReporter );

        /**
           Executes the given source string in the current
           context.

           If resultGoesTo is not null and the result is neither an
           error nor undefined, then the result is converted to a
           string and send to that stream.

           If reportExceptions is true then JS-side exceptions will
           trigger a call to the error reporter defined via
           SetExecuteErrorReporter() (or the default, which sends the
           exception to std::cerr). The error reporter will get passed
           a formatted error message (possible multiple lines)
           detailing, if possible, the exact location of the error.

           Returns true if execution of the script generates no JS
           errors, else false.
        */
        bool ExecuteString(v8::Handle<v8::String> source,
                           v8::Handle<v8::Value> name,
                           std::ostream * resultGoesTo = 0,
                           bool reportExceptions = true);

        /**
           Convenience overload.
        */
        bool ExecuteString(std::string const & source,
                           std::string const & name,
                           std::ostream * resultGoesTo = 0,
                           bool reportExceptions = true);

        /**
           Runs an interactive shell in a loop. It runs until the
           second argument passed to the input callback is set to true
           by the callback. Each line of input is passed to
           ExecuteString(), and the resultsTo and reportExceptions
           arguments are passed to that function.
        */
        void InputLoop( LineFetcher input,
                        std::ostream * resultGoesTo = 0,
                        bool reportExceptions = true );
        /**
           A LineFetcher() implementation which simply reads a single
           line from stdin and assigns the result (minus the newline
           character) to dest. It sets breakOut to true at EOF
           (normally when Ctrl-D is pressed at the start of input on
           the console).

           This implementation prints a prompt to std::cout before
           reading input from std::cin.
        */
        static void StdinLineFetcher( std::string & dest, bool & breakOut );

    private:
        //!Copying is disabled.
        JuiceShell & operator=(JuiceShell const &);
        //!Copying is disabled.
        JuiceShell( JuiceShell const & );
    };
    

}} // namespaces

#endif /* CODE_GOOGLE_COM_V8_JUICE_JUICESHELL_H_INCLUDED */

