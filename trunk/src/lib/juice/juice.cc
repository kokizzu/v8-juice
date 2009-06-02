#include <v8/juice/juice.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <v8/juice/PathFinder.h>
#include <v8/juice/Phoenix.h>
#include "whprintf.h"
#include <v8/juice/convert.h>
#include <v8/juice/ToSource.h>

// #ifndef CERR
// #define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
// #endif
// #ifndef COUT
// #define COUT std::cout << __FILE__ << ":" << std::dec << __LINE__ << " : "
// #endif

namespace v8 { namespace juice {

    using namespace v8;

    struct PathFinder_includes_context {};
    struct PathFinder_phoenix_initializer
    {
	void operator()( PathFinder & t ) const
	{
	    t.PathSeparator(":");
	    t.Extensions(".js:.juice:.v8");
	    t.Path(v8_juice_CONFIG_SCRIPTS_PATH);
	}
    };

    PathFinder & ScriptsPath()
    {
	typedef ::v8::juice::Detail::phoenix<PathFinder,
	    PathFinder_includes_context,
	    PathFinder_phoenix_initializer> PHX;
	return PHX::instance();
    }

    Handle<Value> IncludeScript( Arguments const & argv )
    {
	HandleScope scope;
	Handle<Value> rv = Undefined();
	Handle<Value> exc;
	for (int i = 0; i < argv.Length(); i++)
	{
	    HandleScope handle_scope;
	    // Man, everyone needs a different type of String value...
	    std::string farg = convert::JSToStdString(argv[i]);
	    std::string found = ScriptsPath().Find( farg );
	    if( found.empty() )
	    {
		std::ostringstream os;
		os << "Could not find file ["<<farg<<"] in ScriptsPath()!";
		return ThrowException(v8::String::New(os.str().c_str()));
	    }
	    Handle<String> fnameS( String::New(found.c_str()) );
	    String::Utf8Value fname( fnameS );
	    if( !*fname || !**fname )
	    {
		return ThrowException(v8::String::New("Filename argument is empty!"));
	    }
	    std::ostringstream os;
	    {
		std::ifstream is( *fname );
		if( ! is.good() )
		{
		    os.str("");
		    os << "Error opening file "<<*fname<<"!";
		    return ThrowException(v8::String::New(os.str().c_str()));
		}
		is >> std::noskipws;
		std::copy( std::istream_iterator<char>(is),
                           std::istream_iterator<char>(),
			   std::ostream_iterator<char>(os) );
	    }
	    std::string src = os.str();
	    Handle<String> jsrc( String::New( src.c_str(), static_cast<int>( src.size() ) ) );
	    do
	    {
		TryCatch tryer;
		tryer.SetCaptureMessage(true);
		tryer.SetVerbose(true);
		Handle<Script> Scr( Script::Compile( jsrc, convert::CastToJS(found) ) );
		if( Scr.IsEmpty() )
		{
		    exc = tryer.Exception();
		    break;
		}
		rv = Scr->Run();
		if( rv.IsEmpty() )
		{
		    //return ThrowException(tryer.Exception());
		    exc = tryer.Exception();
		    break;
		}
		//tryer.Reset();
	    } while(false);
	    if( ! exc.IsEmpty() )
	    {
		return ThrowException(exc);
	    }
            rv = handle_scope.Close(rv);
	}
        return scope.Close(rv);
    }


    /** Callback for use with whprintf(). arg must be-a
	(std::ostream*).
     */
    static long ostream_appender( void * arg,
				  char const * data,
				  long n )
    {
	std::ostream * os = static_cast<std::ostream *>(arg);
	if( ! os ) return -1;
	long i = 0;
	for( ; i < n; ++i )
	{
	    *os << data[i];
	}
	return i;
    }

    Handle<Value> ThrowException( const char *format, ... )
    {
	if( ! format || !*format )
	{
	    return ThrowException(String::New("Empty exception text passed to v8::juice::Toss()!"));
	}
	va_list vargs;
	va_start( vargs, format );
	std::ostringstream os;
	::whprintf( ostream_appender, &os, format, vargs );
	va_end(vargs);
	return ::v8::ThrowException(String::New(os.str().c_str(),static_cast<int>(os.str().size())));
    }


    /**
       Internal. Not part of the public API. Okay, i admit it - i
       was lazy and stole this from the s11n source tree.

       ins = edited in place
       to_esc = characters to escape.
       esc = the escape sequence
    */
    static size_t StringEscape( std::string & ins, const std::string & to_esc, const std::string & esc )
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

    std::string StringJSEscape( const std::string & inp )
    {
        std::string out(inp);
        StringEscape( out, "\'", "\\" );
        return out;
    }
	
    std::string StringJSQuote( std::string const & s )
    {
      if( std::string::npos == s.find('\'') ) return "'"+s+"'";
      else if( std::string::npos == s.find("\"") ) return "\""+s+"\"";
      else
      {
          std::string x = s;
          StringEscape( x, "\'", "\\" );// "Quote it all, let God sort it out!"
          return "'"+x+"'";
      }
    }

    namespace convert
    {
        Handle<Value> ToSource( ::v8::Arguments const & argv )
        {
            if( 1 != argv.Length() )
                return v8::ThrowException(String::New("toSource requires exactly 1 argument"));
            return convert::CastToJS( convert::ToSource( argv[0] ) );
        }
    }


}}
