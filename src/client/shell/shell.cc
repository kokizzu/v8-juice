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

#include <v8/juice/convert.h>
#include <v8/juice/bind.h>
#include <v8/juice/plugin.h>
#include <v8/juice/cleanup.h>
#include <v8/juice/WeakJSClassCreator.h>
//#include <v8/juice/v8-classwrap.h> // failed experiment

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
    int func1() { return 42; }
    int func2(int x) { return x*2; }
    double func3(int x,int y) { return x*y; }
    std::string hi() { return "hi!"; }

    my_native * me() { CERR << "my_native::me()="<<this<<'\n';return this; }
    bool him(my_native * him) { CERR << "my_native::him("<<him<<")\n"; return true; }

    void avoid() {CERR << "my_native::avoid()="<<this<<'\n'; }
    void avoid1(int x ) {CERR << "my_native::avoid1("<<x<<")="<<this<<'\n'; }
    void avoid2(int x,double d ) {CERR << "my_native::avoid2("<<x<<","<<d<<")="<<this<<'\n'; }

    double takes3(int x, int y, int z) { return x * y * z; }

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
    namespace convert {
	// Required for CastTo/FromJS() to work:
	template <>
	struct NativeToJS<my_native>
	{
	    ValueHandle operator()( my_native * p ) const
	    {
		Handle<Object> jo = ::v8::juice::WeakJSClassCreator<my_native>::GetJSObject(p);
		if( jo.IsEmpty() ) return Null();
		else return jo;
	    }
	};
	template <>
	struct JSToNative<my_native>
	{
	    typedef my_native * ResultType;
	    ResultType operator()( ValueHandle const & h ) const
	    {
		return ::v8::juice::WeakJSClassCreator<my_native>::GetNative(h);
	    }
	    ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	    {
		return this->operator()( h );
	    }

	};
    } // namespace convert
}} // v8::juice

// template <typename T, typename RV, typename A1  >
// struct MemFuncType< RV (T::*)(A1) >
// {
//     typedef T Type;
//     typedef RV ReturnType;
//     typedef ReturnType (T::*MemFunc)(A1);
// };


template <typename T, typename RV>
struct MemFuncSig0
{
    typedef T Type;
    typedef RV (T::*Member)();
};

#include <v8/juice/ClassBinder.h>

int my_fwd( V8CxH & cx )
{
    //typedef WeakJSClassCreator<my_native> WT;
    typedef ClassBinder<my_native> WT;
    WT w( cx->Global() );

    //BindMemFunc( w, "func1", &my_native::func1 );
    typedef my_native MY;
    w.BindMemFunc< int, &MY::func1>( "func1" );
    w.BindMemFunc< int, int, &MY::func2 >( "func2" );
    w.BindMemFunc< std::string,&MY::hi >( "hi" );
    w.BindMemFunc< MY *,&MY::me >( "me" );
    w.BindMemFunc< bool,MY *,&MY::him >( "him" );
    w.BindMemFunc< double,int,int,&MY::func3 >( "func3" );
    w.BindMemFunc< void,&MY::avoid >( "avoid" );
    w.BindMemFunc< void,int,&MY::avoid1 >( "avoid1" );
    w.BindMemFunc< void,int,double,&MY::avoid2 >( "avoid2" );
    w.BindMemFunc< double,int,int,int,&MY::takes3 >( "takes3" );
    w.Seal();
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

int my_bind_test2( V8CxH & cx )
{
    using namespace v8::juice::convert;
    using namespace v8::juice::bind;
    using namespace v8;
    v8::HandleScope handle_scope;
    V8LObject gl = cx->Global();
    Local<Value> jv = gl->Get( String::New("MyNative") );
    my_native * n = GetBoundNative<my_native>( 0, jv );
    if( ! n )
    {
	CERR << "ERROR: Could not fetch MyNative as an External!\n";
	return 1;
    }
    COUT << "MyNative = " << n << " == ["<<(n?n->str:"<NULL>")<<"]\n";
#if 1
    // Fix this:
    n = CastFromJS<my_native>( 0, jv );
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
    my_native mynat;
    mynat.str = "hi, world";
    Local<External> ex = External::New( &mynat );
    bool rc = BindNative( 0, **ex, &mynat );
    COUT << "bind rc = " << rc << '\n';
    my_native * bs = GetBoundNative<my_native>( 0, **ex );
    COUT << "bound mynat = " << bs << " == ["<<(bs?bs->str:"<NULL>")<<"]\n";
    V8LObject gl = cx->Global();

//     Local<BoundNative> bn = BoundNative::New( &mynat );
//     COUT << "bn->Value<my_native>() == "<<bn->Value<my_native>()<<'\n';

    gl->Set( String::New("MyNative"), ex );
    my_bind_test2( cx );
    rc = UnbindNative( 0, **ex, &mynat );
    COUT << "unbind rc = " << rc << '\n';
    rc = UnbindNative( 0, **ex, &mynat );
    COUT << "unbind rc = " << rc << '\n';
    bs = GetBoundNative<my_native>( 0, **ex );
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

int main(int argc, char* argv[]) {
    v8::V8::SetFlagsFromCommandLine(&argc, argv, true);

    {
	v8::juice::cleanup::CleanupSentry cleaner;
	v8::HandleScope handle_scope;
	// Create a template for the global object.
	v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New();
	// Bind the global 'print' function to the C++ Print callback.
	global->Set(v8::String::New("print"), v8::FunctionTemplate::New(Print));
	// Bind the global 'load' function to the C++ Load callback.
	global->Set(v8::String::New("load"), v8::FunctionTemplate::New(Load));
	global->Set(v8::String::New("load_plugin"), v8::FunctionTemplate::New(v8::juice::plugin::LoadPlugin));
	// Bind the 'quit' function
	global->Set(v8::String::New("quit"), v8::FunctionTemplate::New(Quit));
	// Bind the 'version' function
	global->Set(v8::String::New("version"), v8::FunctionTemplate::New(Version));

	// Create a new execution environment containing the built-in
	// functions
	v8::Handle<v8::Context> context = v8::Context::New(NULL, global);
	// Enter the newly created execution environment.
	v8::Context::Scope context_scope(context);

	if(1)
	{
	    //v8::Handle<v8::Value> iv = v8::juice::sq3::SetupAddon( context->Global() );
	    v8::Handle<v8::Value> iv;
	    //iv = v8::juice::sq3::SetupAddon( context->Global() );
	    //iv = v8::juice::nc::SetupAddon( context->Global(), false );
	    iv = v8::juice::SetupPathFinderClass( context->Global() );
	    //iv = v8::juice::convert::SetupAddon( context->Global() );
	    //COUT << "SetupAddon() == " << v8::convert::CastFromJS<std::string>( iv ) << '\n';
	}
	if(1)
	{
	    //my_test( context );
	    //my_bind_test( context );
	    my_class_test( context );
	    my_fwd(context);
	}
	bool run_shell = (argc == 1);
	for (int i = 1; i < argc; i++) {
	    const char* str = argv[i];
	    if (strcmp(str, "--shell") == 0) {
		run_shell = true;
	    } else if (strcmp(str, "-f") == 0) {
		// Ignore any -f flags for compatibility with the other stand-
		// alone JavaScript engines.
		continue;
	    } else if (strncmp(str, "--", 2) == 0) {
		printf("Warning: unknown flag %s.\nTry --help for options\n", str);
	    } else if (strcmp(str, "-e") == 0 && i + 1 < argc) {
		// Execute argument given to -e option directly
		v8::HandleScope handle_scope;
		v8::Handle<v8::String> file_name = v8::String::New("unnamed");
		v8::Handle<v8::String> source = v8::String::New(argv[i + 1]);
		if (!ExecuteString(source, file_name, false, true))
		{
		    return 1;
		}
		i++;
	    } else {
		// Use all other arguments as names of files to load and run.
		v8::HandleScope handle_scope;
		v8::Handle<v8::String> file_name = v8::String::New(str);
		v8::Handle<v8::String> source = ReadFile(str);
		if (source.IsEmpty()) {
		    printf("Error reading '%s'\n", str);
		    return 1;
		}
		if (!ExecuteString(source, file_name, true, true))
		    return 1;
	    }
	}
	if (run_shell) RunShell(context);
	return 0;
    }
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
  std::ostream & os( std::cout );
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

