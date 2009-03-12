#ifndef CODE_GOOGLE_COM_V8_JUICE_JUICE_H_INCLUDED
#define CODE_GOOGLE_COM_V8_JUICE_JUICE_H_INCLUDED
#include <v8.h>

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

}} // namespace

#endif // CODE_GOOGLE_COM_V8_JUICE_JUICE_H_INCLUDED
