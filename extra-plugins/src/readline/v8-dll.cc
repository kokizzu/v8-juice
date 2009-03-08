/**
   A v8-juice wrapper/plugin around readlinecpp, which is basically a
   thin wrapper around GNU Readline, libeditline, or plain old stdin,
   depending on how it's built.

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: Pulic Domain. That said, if ./Readline_config.hpp is configured
   to use GNU Readline then it will link against that library, making this
   code GPL. So, the license depends on how you configure it.   
*/
#include <v8.h>
#include <v8/juice/plugin.h>
#include <v8/juice/convert.h>
#include "Readline.hpp"
#include <string>

namespace {
    using namespace v8;
    using namespace v8::juice::convert;
    using namespace readlinecpp;
#define JS_WRAPPER(FN) static Handle< Value > FN( const Arguments & argv )
#define ASSERTARGS(FUNCNAME,COND) const int argc = argv.Length();	\
	if(!(COND)) return ThrowException(String::New(# FUNCNAME "(): assertion failed: " # COND))

    static Readline & reader()
    {
	static Readline bob;
	return bob;
    }

    JS_WRAPPER(read)
    {
	std::string prompt = (0!=argv.Length()) ? JSToStdString(argv[0]) : "";
	bool breakout = false;
	std::string str( reader().readline( prompt, breakout ) );
	if( breakout ) return Undefined();
	else return CastToJS( str );
    }

    JS_WRAPPER(loadHistory)
    {
	if( argv.Length() == 0 )
	{
	    return ThrowException(String::New("loadHistory() requires a filename argument!"));
	}
	return CastToJS( reader().load_history( JSToStdString(argv[0])) );
    }

    JS_WRAPPER(saveHistory)
    {
	return CastToJS( reader().save_history( (argv.Length() == 0)
						? std::string()
						: JSToStdString(argv[0])) );
	return Null();
    }

    JS_WRAPPER(clearHistory)
    {
	reader().clear_history();
	return Null();
    }


    static Handle<Value> SetupAddon( Handle<Object> gl )
    {
	HandleScope sentry;
	Handle<Object> jobj = Object::New();
	gl->Set(String::New("readline"),jobj);

	Handle<Function> func;
#define ADDFUNC2(JS,NATIVE)					\
	func = FunctionTemplate::New(NATIVE)->GetFunction();	\
	jobj->Set(String::New(# JS), func )

#define ADDFUNC(SUF) ADDFUNC2(SUF,SUF)
	ADDFUNC(read);
	ADDFUNC(clearHistory);
	ADDFUNC(saveHistory);
	ADDFUNC(loadHistory);
	return gl;
    }

    V8_JUICE_PLUGIN_STATIC_INIT(SetupAddon);
    
} // namespaces
