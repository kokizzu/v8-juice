#if defined(NDEBUG)
#undef NDEBUG
#endif
#include <cassert>
// Copyright 2008 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/************************************************************************
This code is based off of the shell.cc sample application in the v8
source tree. It has been mangled to become the default shell for the
v8-juice project.

Some changes from the original shell:

- Binds most of the v8-juice add-on functions by default.

- If the argv list contains "--", all arguments after that are ignored
by the shell but passed on to each script which it runs. Each script
gets a global arguments object, where arguments[0] is the name of the
shell (it SHOULD be the name of the script, but it's not), and arguments
1..N are those after the "--"

 ************************************************************************/

/**

BUG_NUMBER() is a marker which should be interpreted as:

http://code.google.com/p/v8-juice/issues/detail?id=N

*/
#define BUG_NUMBER(N)

#include <v8.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <iostream>

#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif
#ifndef COUT
#define COUT std::cout << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

#include <v8/juice/juice.h>
#include <v8/juice/convert.h>
#include <v8/juice/time.h>
#include <v8/juice/sprintf.h>
#include <v8/juice/plugin.h>
#include <v8/juice/PathFinder.h>
#include <v8/juice/cleanup.h>
#include <v8/juice/forwarding.h>
#include <v8/juice/WeakJSClassCreator.h>
#include <v8/juice/ToSource.h>
#include <v8/juice/JuiceShell.h>

namespace bind = ::v8::juice::bind;

void RunShell(v8::Handle<v8::Context> context);
bool ExecuteString(v8::Handle<v8::String> source,
                   v8::Handle<v8::Value> name,
                   bool print_result,
                   bool report_exceptions);
v8::Handle<v8::Value> Print(const v8::Arguments& args);
v8::Handle<v8::Value> Load(const v8::Arguments& args);
v8::Handle<v8::Value> Quit(const v8::Arguments& args);
v8::Handle<v8::Value> Version(const v8::Arguments& args);
v8::Handle<v8::String> ReadFile(const char* name);
void ReportException(v8::TryCatch* handler);

typedef v8::Handle<v8::ObjectTemplate> V8Object;
typedef v8::Local<v8::Object> V8LObject;
typedef v8::Handle<v8::Context> V8CxH;


using namespace v8;
using namespace ::v8::juice;


struct my_native
{
    std::string str;
private:
    int proxied;
public:
    int propGetter() const
    {
        CERR << "my_native::propGetter()\n";
        return this->proxied;
    }
    int propSetter(int v)
    {
        CERR << "my_native::propSetter("<<v<<")\n";
        return this->proxied = v;
    }
    void propSetterVoid(int v)
    {
        CERR << "my_native::propSetterVoid("<<v<<")\n";
        this->proxied = v;
        return;
    }
    int func1() { return 42; }
    int func2(int x) { return x*2; }
    double func3(int x,int y)
    {
        CERR << "func3("<<x<<","<<y<<")\n";
        return x*y;
    }
    std::string hi() { return "hi!"; }

    my_native * me() { CERR << "my_native::me()="<<this<<'\n';return this; }
    bool him(my_native * him) { CERR << "my_native::him("<<him<<")\n"; return 0 != him; }

    void avoid() {CERR << "my_native::avoid()="<<this<<'\n'; }
    void avoid1(int x ) {CERR << "my_native::avoid1("<<x<<")="<<this<<'\n'; }
    void avoid2(int x,double d ) {CERR << "my_native::avoid2("<<x<<","<<d<<")="<<this<<'\n'; }

    double takes3(int x, int y, int z) { return x * y * z; }

    Handle<Value> forwarder( Arguments const & );

    void someref1( my_native & x )
    {
        CERR << "someref1("<<&x<<")\n";
        return;
    }
    BUG_NUMBER(11)
    my_native & someref2( my_native & x )
    {
        CERR << "someref2("<<&x<<")\n";
        return x;
    }
    BUG_NUMBER(11)
    my_native const & someref3( my_native const & x )
    {
        CERR << "someref3("<<&x<<")\n";
        return x;
    }

    my_native * other;
    my_native() : str(),
                  proxied(19),
                  other(0)
    {}
};

namespace v8 { namespace juice {
    template <>
    struct WeakJSClassCreatorOps<my_native>
    {
    private:
	/** Callback for use with juice::cleanup::AddToCleanup(). */
	static void cleanup_callback( void * obj )
	{
	    Dtor( static_cast<WrappedType*>(obj) );
	}
    public:
	enum { ExtraInternalFieldCount = 0 };
	typedef my_native WrappedType;
	static char const * ClassName() { return "MyNative"; }
	static WrappedType * Ctor( Arguments const & argv,
				   std::string & exceptionText)
	{
	    my_native * obj = new my_native;
	    obj->str = "hi, world";
	    CERR << "Ctor() create @"<<obj<<'\n';
	    ::v8::juice::cleanup::AddToCleanup(obj,cleanup_callback);
	    //bind::BindNative( 0, obj, obj );
	    return obj;
	}

	static void Dtor( WrappedType * obj )
	{
	    CERR << "Dtor() passing on @"<<obj<<'\n';
	    if( obj )
	    {
		//bind::UnbindNative( 0, obj, obj );
		::v8::juice::cleanup::RemoveFromCleanup(obj);
		delete obj;
	    }
	}
    };
}} // v8::juice

#define WEAK_CLASS_TYPE my_native
#include <v8/juice/WeakJSClassCreator-CastOps.h>

#include <v8/juice/ClassBinder.h>

int my_forwarded_func()
{
    CERR << "my_forwarded_func()!\n";
    return 42;
}
int my_forwarded_func3(int x,int y,int z)
{
    CERR << "my_forwarded_func()!\n";
    return x + y + z;
}
void my_void_forwarded_func(int x)
{
    CERR << "my_void_forwarded_func("<<x<<")!\n";
}

Handle<Value> my_native::forwarder( Arguments const & argv )
{
    v8::juice::convert::FwdToFunc( my_forwarded_func3, argv );
    return v8::juice::convert::FwdToFunc3<int,int,int,int>( my_forwarded_func3, argv );
}
/**
   Demonstration of adding a close() or destroy() member to
   a bound class.
*/
Handle<Value> my_native_destroy( Arguments const & argv )
{
    if(1)
    { // informational only: has no real effect:
        int argc = argv.Length();
        my_native * N = v8::juice::convert::CastFromJS<my_native>( argv.This() );
        if( ! N ) return ThrowException(String::New("This object is not (or is no longer) a my_native!"));
        CERR << "my_native_destroy( @"<<(void const *)N<<" )\n";
    }
    typedef v8::juice::ClassBinder<my_native> BinderType;
    BinderType::DestroyObject(argv.This());
    return Undefined();
}

int my_fwd( V8CxH const & cx )
{
    //typedef WeakJSClassCreator<my_native> WT;
    typedef ClassBinder<my_native> WT;
    //WT & w = WT::Instance();
    WT w;
    WT::SearchPrototypesForNative(true); // allows JS classes to subclass my_native
    WT::AllowCtorWithoutNew(true);
    //BindMemFunc( w, "func1", &my_native::func1 );
    typedef my_native MY;
    w
        .BindMemFunc< int, &MY::func1>( "func1" )
        .BindMemFunc< int, int, &MY::func2 >( "func2" )
        .BindMemFunc< std::string,&MY::hi >( "hi" )
        .BindMemFunc< MY *,&MY::me >( "me" )
        .BindMemFunc< bool,MY *,&MY::him >( "him" )
        .BindMemFunc< double,int,int,&MY::func3 >( "func3" )
        .BindMemFunc< void,&MY::avoid >( "avoid" )
        .BindMemFunc< void,int,&MY::avoid1 >( "avoid1" )
        .BindMemFunc< void,int,double, &MY::avoid2 >( "avoid2" )
        .BindMemFunc< double,int,int,int, &MY::takes3 >( "takes3" )
        .BindMemVar<std::string, &MY::str>( "str" )
        .BindMemVar<my_native *, &MY::other>("other")
        .BindMemFunc< &MY::forwarder >( "forwarder" )
        .BindMemFunc< void, MY &, &MY::someref1 >( "someref1" )
        .BindPropToAccessors< int, &MY::propGetter, int, int, &MY::propSetter >( "proxiedProp" )
        //.BindPropToAccessors< int, &MY::propGetter, void, int, &MY::propSetterVoid >( "proxiedProp" )
        //.BindPropToGetter< int, &MY::propGetter >( "proxiedProp" )
        //.BindPropToSetter< void, int, &MY::propSetterVoid >( "proxiedProp" )
        BUG_NUMBER(11)
        //.BindMemFunc< MY &, MY &, &MY::someref2 >( "someref2" )
        //.BindMemFunc< MY const &, MY const &, &MY::someref3 >( "someref3" )
        .Set( "destroy", my_native_destroy )
        .Seal();
    w.AddClassTo( cx->Global() );
    return 0;
}

int my_tosource( V8CxH & cx )
{
    using namespace v8;
    using namespace v8::juice;
    using namespace v8::juice::convert;

    CERR << ToSource( 43 )
         << '\n' << ToSource( "He's one of us, isn't he?" )
         << '\n' << ToSource( "He said, \"hi!\"" )
         << '\n' << ToSource( "He said, \"she's not home!\"" )
         << '\n';

    typedef std::list<std::string> SL;
    SL li;
    li.push_back( "He's one of us, isn't he?" );
    li.push_back( "He said, \"hi!\"" );
    li.push_back( "He said, \"she's not home!\"" );
    CERR <<  ToSource(li) << '\n';
    std::list<SL> lili;
    lili.push_back(li);
    lili.push_back(li);
    lili.push_back(li);
    CERR << ToSource(lili) << '\n';

    CERR << ToSource( Local<String>( String::New("hi, world") ) ) << '\n';
    CERR << ToSource( Integer::New(17) ) << '\n';
    CERR << ToSource( Null() )  << '\n';
    CERR << ToSource( Undefined() )  << '\n';
    CERR << ToSource( Boolean::New(true) )  << '\n';
    CERR << ToSource( Boolean::New(false) )  << '\n';
    Local<Object> gl = v8::Context::GetCurrent()->Global();
    CERR << ToSource( gl )  << '\n';
    CERR << ToSource( gl->Get(String::New("print")) )  << '\n';
    CERR << ToSource( CastToJS( li ) ) << '\n';
    return 0;
}


int my_test( V8CxH & cx )
{
    using namespace v8::juice::convert;
    using namespace v8;


    v8::HandleScope handle_scope;
    V8LObject gl = cx->Global();
    COUT << "gl.MyVar = ["<<CastFromJS<std::string>( gl->Get( String::New("MyVar") ) )<<"]\n";
    Handle<Value> hv1 = CastToJS<int>( 43 );
    char const * a = 0;
    {
	String::AsciiValue asc( hv1 );
	COUT << "hv1.IsEmpty() ?= "<<hv1.IsEmpty()<<'\n';
	a = *asc;
	COUT << "hv1 : [" << (a ? a : "<IsEmpty()==true>") << "]\n";
    }
    int myInt = CastFromJS<int>( hv1 );
    COUT << "myInt = "<<myInt<<"\n";
    Handle<Value> hv2 = CastToJS( 42.24 );
    //Handle<Value> hv2 = CastToJS( std::cout );
    myInt = CastFromJS<int>( hv2 );
    double myDbl = CastFromJS<double>( hv2 );
    COUT << "myInt = "<<myInt<<", myDbl = " << myDbl<<"\n";
    {
	COUT << "JsToNative<...>(hv2): ["<<JSToStdString(hv2)<<"]\n";
    }
    Local<Value> lvE = Exception::Error( String::New("My error test") );
    COUT << "lvE = ["<<JSToStdString(lvE)<<"]\n";
    return 0;
}

struct bind_tester
{
    std::string str;
    bind_tester() : str("hi, world!")
    {}
};

int my_bind_test2( V8CxH & cx )
{
    using namespace v8::juice::convert;
    using namespace v8::juice::bind;
    using namespace v8;
    v8::HandleScope handle_scope;
    V8LObject gl = cx->Global();
    Local<Value> jv = gl->Get( String::New("MyNative") );
    bind_tester * n = GetBoundNative<bind_tester>( jv );
    if( ! n )
    {
	CERR << "ERROR: Could not fetch MyNative as an External!\n";
	return 1;
    }
    COUT << "MyNative = " << n << " == ["<<(n?n->str:"<NULL>")<<"]\n";
#if 0
    n = CastFromJS<bind_tester>( jv );
    COUT << "MyNative = " << n << " == ["<<(n?n->str:"<NULL>")<<"]\n";
#endif
    return 0;
}

int my_bind_test( V8CxH & cx )
{
    using namespace v8::juice::convert;
    using namespace v8::juice::bind;
    using namespace v8;
    v8::HandleScope handle_scope;
    bind_tester mynat;
    mynat.str = "hi, world?";
    Local<External> ex = External::New( &mynat );
    bool rc = BindNative( ex, &mynat );
    COUT << "bind key@"<<ex->Value() << " to @"<<&mynat<<", rc = " << rc << '\n';
    bind_tester * bs = GetBoundNative<bind_tester>( ex );
    COUT << "bound mynat = " << bs << " == ["<<(bs?bs->str:"<NULL>")<<"]\n";
    V8LObject gl = cx->Global();

//     Local<BoundNative> bn = BoundNative::New( &mynat );
//     COUT << "bn->Value<bind_tester>() == "<<bn->Value<bind_tester>()<<'\n';

    gl->Set( String::New("MyNative"), ex );
    my_bind_test2( cx );
    rc = UnbindNative( ex, &mynat );
    COUT << "unbind rc = " << rc << '\n';
    rc = UnbindNative( ex, &mynat );
    COUT << "unbind rc = " << rc << '\n';
    bs = GetBoundNative<bind_tester>( ex );
    assert( 0 == bs );
    COUT << "unbind bs = " << bs << " == ["<<(bs?bs->str:"<NULL>")<<"]\n";
    return 0;
}


int my_class_test( V8CxH & cx )
{
#if 0
    COUT << "Class tests...\n";
    using namespace v8::juice;
    using namespace v8;
    using namespace v8::bind;
    using namespace v8::convert;
    char const * CName = "MyType";
    typedef my_native NT;

    //Handle<Object> jobj = WrapNative<my_native_dtor>( my

    typedef ClassWrapper<CName,NT> CW;

    CW & w = CW::Inst().
	Init( cx->Global(), my_native_ctor )
	;

    COUT << "end ClassWrapper tests.\n";
#endif
    return 0;
}

static bool PrintUsesStdErr = false;

int main(int argc, char * argv[])
{
#define JSTR(X) v8::String::New(X)
    if(0)
    { /** fuck - SetFlagsFromCommandLine() changes argv such that the
          "--" arg and those following it are stripped!
       */
        typedef std::vector<char *> AV;
        AV vargv(static_cast<size_t>(argc), 0);
        int i;
        for( i = 0; i < argc; ++i )
        {
            vargv[(unsigned int)i] = argv[i];
        }
        v8::V8::SetFlagsFromCommandLine(&i, &vargv[0], true);
    }
    else
    {
        // or we could use the undocumented 3rd arg:
        v8::V8::SetFlagsFromCommandLine(&argc, argv, false);
    }
    {
        v8::HandleScope handle_scope;
        v8::juice::cleanup::CleanupSentry cleaner;
        v8::juice::JuiceShell shell("v8juice");
        shell.ProcessMainArgv(argc,argv,1);
        shell.SetupJuiceEnvironment();
#define FT v8::FunctionTemplate::New
#define BIND(K,V) shell.AddGlobalFunc( K, V )
        BIND("print", Print);
        BIND("load", Load);
        BIND("quit", Quit);
        BIND("version", Version);
#undef BIND
#undef FT
        
        if(1)
        {
            //v8::Handle<v8::Value> iv = v8::juice::sq3::SetupAddon( context->Global() );
            v8::Handle<v8::Value> iv;
            iv = v8::juice::SetupPathFinderClass( shell.Context()->Global() );
            //iv = v8::juice::convert::SetupAddon( context->Global() );
            //COUT << "SetupAddon() == " << v8::convert::CastFromJS<std::string>( iv ) << '\n';
        }
        if(1)
        {
            // my_test( context );
            //my_class_test( context );
            my_fwd(shell.Context());
            //my_tosource(context);
            //my_bind_test( context );
        }
        bool run_shell = (argc == 1);
        v8::Locker tlocker;
        std::string const endofargs("--");
        for (int i = 1; i < argc; i++) {
            const char* str = argv[i];
            if( 0 == strcmp(str,"--print-cerr"))
            {
                PrintUsesStdErr = true;
                continue;
            }
            else if( endofargs == str )
            {
                break;
            }
            else if (strcmp(str, "--shell") == 0) {
                run_shell = true;
                continue;
            } else if (strcmp(str, "-f") == 0) {
                // Ignore any -f flags for compatibility with the other stand-
                // alone JavaScript engines.
                continue;
            } else if (strncmp(str, "--", 2) == 0) {
                printf("Warning: unknown flag %s.\nTry --help for options\n", str);
            } else if (strcmp(str, "-e") == 0 && i + 1 < argc) {
                // Execute argument given to -e option directly
                v8::HandleScope handle_scope;
                v8::Handle<v8::String> file_name = JSTR("unnamed");
                v8::Handle<v8::String> source = JSTR(argv[i + 1]);
                if (!ExecuteString(source, file_name, false, true))
                {
                    return 1;
                }
                i++;
            } else {
                // Use all other arguments as names of files to load and run.
                v8::HandleScope handle_scope;
                v8::Handle<v8::String> file_name = JSTR(str);
                v8::Handle<v8::String> source = ReadFile(str);
                if (source.IsEmpty()) {
                    printf("Error reading '%s'\n", str);
                    return 1;
                }
                if (!ExecuteString(source, file_name, true, true))
                    return 1;
            }
        }
        if (run_shell) RunShell( shell.Context() );
    }
    return 0;
#undef JSTR
}
// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}


// The callback that is invoked by v8 whenever the JavaScript 'print'
// function is called.  Prints its arguments on stdout separated by
// spaces and ending with a newline.
v8::Handle<v8::Value> Print(const v8::Arguments& args) {
  bool first = true;
  std::ostream & os( PrintUsesStdErr ? std::cerr : std::cout );
  for (int i = 0; i < args.Length(); i++) {
    v8::HandleScope handle_scope;
    if (first) {
      first = false;
    } else {
	os << ' ';
    }
    v8::String::Utf8Value str(args[i]);
    const char* cstr = ToCString(str);
    if( cstr ) os << cstr;
  }
  os << '\n';
  return v8::Undefined();
}



// The callback that is invoked by v8 whenever the JavaScript 'load'
// function is called.  Loads, compiles and executes its argument
// JavaScript file.
v8::Handle<v8::Value> Load(const v8::Arguments& args) {
  for (int i = 0; i < args.Length(); i++) {
    v8::HandleScope handle_scope;
    v8::String::Utf8Value file(args[i]);
    if (*file == NULL) {
      return v8::ThrowException(v8::String::New("Error loading file"));
    }
    v8::Handle<v8::String> source = ReadFile(*file);
    if (source.IsEmpty()) {
      return v8::ThrowException(v8::String::New("Error loading file"));
    }
    if (!ExecuteString(source, v8::String::New(*file), false, false)) {
      return v8::ThrowException(v8::String::New("Error executing file"));
    }
  }
  return v8::Undefined();
}


// The callback that is invoked by v8 whenever the JavaScript 'quit'
// function is called.  Quits.
v8::Handle<v8::Value> Quit(const v8::Arguments& args) {
  // If not arguments are given args[0] will yield undefined which
  // converts to the integer value 0.
  int exit_code = args[0]->Int32Value();
  exit(exit_code);
  return v8::Undefined();
}


v8::Handle<v8::Value> Version(const v8::Arguments& args) {
  return v8::String::New(v8::V8::GetVersion());
}


// Reads a file into a v8 string.
v8::Handle<v8::String> ReadFile(const char* name) {
  FILE* file = fopen(name, "rb");
  if (file == NULL) return v8::Handle<v8::String>();

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);

  char* chars = new char[size + 1];
  chars[size] = '\0';
  for (int i = 0; i < size;) {
    int read = fread(&chars[i], 1, size - i, file);
    i += read;
  }
  fclose(file);
  v8::Handle<v8::String> result = v8::String::New(chars, size);
  delete[] chars;
  return result;
}


// The read-eval-execute loop of the shell.
void RunShell(v8::Handle<v8::Context> context) {
  printf("V8 version %s\n", v8::V8::GetVersion());
  static const int kBufferSize = 256;
  while (true) {
    char buffer[kBufferSize];
    printf("> ");
    char* str = fgets(buffer, kBufferSize, stdin);
    if (str == NULL) break;
    v8::HandleScope handle_scope;
    ExecuteString(v8::String::New(str),
                  v8::String::New("(shell)"),
                  true,
                  true);
  }
  printf("\n");
}


// Executes a string within the current v8 context.
bool ExecuteString(v8::Handle<v8::String> source,
                   v8::Handle<v8::Value> name,
                   bool print_result,
                   bool report_exceptions) {
  v8::HandleScope handle_scope;
  v8::TryCatch try_catch;
  v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
  if (script.IsEmpty()) {
    // Print errors that happened during compilation.
    if (report_exceptions)
      ReportException(&try_catch);
    return false;
  } else {
    v8::Handle<v8::Value> result = script->Run();
    if (result.IsEmpty()) {
      // Print errors that happened during execution.
      if (report_exceptions)
        ReportException(&try_catch);
      return false;
    } else {
      if (print_result && !result->IsUndefined()) {
        // If all went well and the result wasn't undefined then print
        // the returned value.
        v8::String::Utf8Value str(result);
        const char* cstr = ToCString(str);
        printf("%s\n", cstr);
      }
      return true;
    }
  }
}


void ReportException(v8::TryCatch* try_catch) {
  v8::HandleScope handle_scope;
  v8::String::Utf8Value exception(try_catch->Exception());
  const char* exception_string = ToCString(exception);
  v8::Handle<v8::Message> message = try_catch->Message();
  if (message.IsEmpty()) {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    printf("%s\n", exception_string);
  } else {
    // Print (filename):(line number): (message).
    v8::String::Utf8Value filename(message->GetScriptResourceName());
    const char* filename_string = ToCString(filename);
    int linenum = message->GetLineNumber();
    printf("%s:%i: %s\n", filename_string, linenum, exception_string);
    // Print line of source code.
    v8::String::Utf8Value sourceline(message->GetSourceLine());
    const char* sourceline_string = ToCString(sourceline);
    printf("%s\n", sourceline_string);
    // Print wavy underline (GetUnderline is deprecated).
    int start = message->GetStartColumn();
    for (int i = 0; i < start; i++) {
      printf(" ");
    }
    int end = message->GetEndColumn();
    for (int i = start; i < end; i++) {
      printf("^");
    }
    printf("\n");
  }
}

