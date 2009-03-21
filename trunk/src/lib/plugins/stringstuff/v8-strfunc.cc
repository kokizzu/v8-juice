#include <string>

#include <v8/juice/convert.h>
#include <v8/juice/forwarding.h>
#include <v8/juice/plugin.h>
#include "v8-strfunc.h"
#include "v8-sprintf.h"

namespace v8 {
namespace juice {
namespace plugin {
namespace str {

    using namespace ::v8;
    using namespace ::v8::juice::convert;

    /**
       Internal. Not part of the public API. Okay, i admit it - i
       was lazy and stole this from the s11n source tree.
    */
    size_t escape_string( std::string & ins, const std::string & to_esc, const std::string & esc )
    {
	std::string::size_type pos;
	pos = ins.find_first_of( to_esc );
	size_t reps = 0;
	while( pos != std::string::npos )
	{
	    ins.insert( pos, esc );
	    ++reps;
	    pos = ins.find_first_of( to_esc, pos + esc.size() + 1 );
	}
	return reps;
    }
	
    std::string quote_js_string( std::string const & s )
    {
      if( std::string::npos == s.find('\'') ) return "'"+s+"'";
	else if( std::string::npos == s.find("\"") ) return "\""+s+"\"";
	else
	{
	    std::string x = s;
	    escape_string( x, "\'", "\\" );// "Quote it all, let God sort it out!"
	    return "'"+x+"'";
	}
    }

    static Handle< Value > v8_escape_js( const Arguments & argv )
    {
	std::string in( JSToStdString(argv[0]) );
	if( 0 == escape_string(in,"'", "\\") ) return argv[0];
	return StdStringToJS( in );
    }

    static Handle< Value > v8_quote_js( const Arguments & argv )
    {
	return FwdToFunc1<std::string,std::string>( quote_js_string, argv );
	//return FwdToFunc1<std::string,std::string const &>( 0, quote_js_string, argv );
	//return StdStringToJS( quote_js_string( JSToStdString(argv[0]) ) );
    }

    Handle<Value> SetupPlugin( Handle<Object> gl )
    {

	//SetupSprintf( gl );
#define ADDFUNC(JSF,NF) gl->Set(String::New(# JSF), FunctionTemplate::New(NF)->GetFunction() )
	ADDFUNC(escape_js_string,v8_escape_js);
	ADDFUNC(quote_js_string,v8_quote_js);
#undef ADDFUNC
	return SetupSprintf(gl);
    }

    V8_JUICE_PLUGIN_STATIC_INIT(SetupPlugin);

}}}} // namespaces
