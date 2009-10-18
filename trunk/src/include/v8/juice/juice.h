#ifndef CODE_GOOGLE_COM_V8_JUICE_JUICE_H_INCLUDED
#define CODE_GOOGLE_COM_V8_JUICE_JUICE_H_INCLUDED

#include <v8.h>
#include <string>
#include "juice-config.h"
/** @mainpage v8-juice

v8-juice is a C++ library providing extended functionality based on
Google's v8 JavaScript engine (http://code.google.com/p/v8).

The v8-juice home page is:

http://code.google.com/p/v8-juice


Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)


License: the core library is mostly New BSD with some Public Domain code.
Certain addons/extensions may have other licenses, in particular those which
use 3rd-party libraries with viral licenses (e.g. the GPL).


Where to start:


The wiki pages on the home page should be informative to new users. Aside
from that, there are tons of API docs here amongst the humble header files.
*/
namespace {}

/**
   A macro which evaluates to nothing bug can be used to mark places
   which are known to be associated with (or the cause of) a certain
   bug number. The N parameter should be a number from the bug list
   on the home page:

   http://code.google.com/p/v8-juice/issues
*/
#define V8_JUICE_BUG_NUMBER(N)

namespace v8 {

/**
   The juice namespace is the top-most namespace used by the
   v8-juice toolkit.
*/
namespace juice {

	using ::v8::Handle;
	using ::v8::Value;

	/**
	   Do not use - it can lead to stack corruption
	   in some cases for unknown reasons. It's only
	   in the public API so i can use the existing client
	   code to track down the problem.

	   A convenience function to throw a v8 exception from a
	   printf-style formatted string.

	   The return value is the result of calling
	   v8::ThrowException(String::New(the_formatted_string)).

	   Example:

	   \code
	   return Toss("Error #%d while performing action %s!", errCode, actionType);
	   \endcode
	*/
	Handle<Value> ThrowException(char const * fmt,...);

    /**
       This can be registered as a JS function to provide an include()
       feature.  It reads the given file as JS, using the search path
       specified by ScriptsPath(). Returns the result of evaluting the
       file, throwing exception on error. The returned value will be
       IsEmpty() if an exception is thrown or propagated.
    */
    Handle<Value> IncludeScript( v8::Arguments const & argv );

    /**
       Works like the JS-bindable version, but takes a single script
       name as its argument. If useSearchPath is false then the
       ScriptsPath() path is not used for searching for the filename,
       and it is assumed to resolve as-is.
    */
    Handle<Value> IncludeScript( char const * filename, bool useSearchPath = true );
        
    /**
       Escapes all single-quote characters in the given string by inserting
       backslashes before them. Returns the escaped string, which may be
       the same as the original.
    */
    std::string StringJSEscape( std::string const & inp );

    /**
       Works like StringJSEscape(), but also adds outer quotes
       to the string. The type of quotes it adds is determined
       like so:

       - If s contains no single quotes, it gets quoted in single quotes. Else...
       - If s contains no double quotes, it gets quoted in double quotes. Else...
       - All single quotes get escaped and the string is quoted in single quotes.

       Returns the modified string.
    */
    std::string StringJSQuote( std::string const & s );


}} // namespace

#endif // CODE_GOOGLE_COM_V8_JUICE_JUICE_H_INCLUDED
