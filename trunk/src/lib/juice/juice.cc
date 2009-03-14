#include <v8/juice/juice.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <v8/juice/PathFinder.h>
#include <v8/juice/Phoenix.h>
#include "whprintf.h"
#include <v8/juice/convert.h>

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
	    t.path_separator(":");
	    t.extensions(".js:.juice:.v8");
	    t.path(v8_juice_CONFIG_SCRIPTS_PATH);
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
	Handle<Value> rv = Undefined();
	Handle<Value> exc;
	for (int i = 0; i < argv.Length(); i++)
	{
	    HandleScope handle_scope;
	    String::Utf8Value fname( argv[i]->ToString() );
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
	    {
		TryCatch tryer;
		Handle<Script> Scr( Script::Compile( jsrc, argv[i] ) );
		if( Scr.IsEmpty() )
		{
		    return ThrowException(tryer.Exception());
		}
		rv = Scr->Run();
		if( rv.IsEmpty() )
		{
		    //return ThrowException(tryer.Exception());
		    exc = tryer.Exception();
		}
	    }
	    if( ! exc.IsEmpty() )
	    {
		return ThrowException(exc);
	    }
	}
	return rv;
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

}}
