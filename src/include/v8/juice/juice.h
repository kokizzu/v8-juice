#ifndef CODE_GOOGLE_COM_V8_JUICE_JUICE_H_INCLUDED
#define CODE_GOOGLE_COM_V8_JUICE_JUICE_H_INCLUDED

#include <v8.h>
namespace v8 {

    /**
       The juice namespace is the top-most namespace used by the
       v8-juice toolkit.
    */
    namespace juice
    {
	using ::v8::Handle;
	using ::v8::Value;

	/**
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

    }
}

#endif // CODE_GOOGLE_COM_V8_JUICE_JUICE_H_INCLUDED
