
#include <v8/v8-convert.h>
#include <v8/v8-sprintf.h>
#include <string>
namespace v8 {
namespace convert {

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
	if( 0 == s.find('\'') ) return "'"+s+"'";
	else if( std::string::npos == s.find("\"") ) return "\""+s+"\"";
	else
	{
	    std::string x = s;
	    escape_string( x, "\'", "\\" );// "Quote it all, let God sort it out!"
	    return "'"+x+"'";
	}
    }

    static ::v8::Handle< ::v8::Value > v8_escape_js( const ::v8::Arguments & argv )
    {
	std::string in( JSToStdString(argv[0]) );
	if( 0 == escape_string(in,"'", "\\") ) return argv[0];
	return StdStringToJS( in );
    }

    static ::v8::Handle< ::v8::Value > v8_quote_js( const ::v8::Arguments & argv )
    {
	return FwdToFunc1<std::string,std::string>( 0, quote_js_string, argv );
	//return FwdToFunc1<std::string,std::string const &>( 0, quote_js_string, argv );
	//return StdStringToJS( quote_js_string( JSToStdString(argv[0]) ) );
    }

    Handle<Value> SetupAddon( Handle<Object> gl )
    {

	SetupSprintf( gl );
#define ADDFUNC(JSF,NF) gl->Set(String::New(# JSF), FunctionTemplate::New(NF)->GetFunction() )
	ADDFUNC(escape_js_string,v8_escape_js);
	ADDFUNC(quote_js_string,v8_quote_js);
#undef ADDFUNC
	return gl;
    }


}} // namespaces

