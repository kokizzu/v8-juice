#include <v8/juice/juice.h>

#include <ostream>
#include <sstream>
#include "whprintf.h"
namespace v8 { namespace juice {

    using namespace v8;

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
