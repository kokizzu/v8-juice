#if !defined(CODE_GOOGLE_COM_P_V8_V8_FORWARDING_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_V8_FORWARDING_H_INCLUDED 1
/*
   A min-framework to simplify casting between v8 JavaScript and
   C++ native objects. Requires the v8-bind.h header.

   v8: http://code.google.com/p/v8/

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: same as v8 (see below)
*/
// Copyright 2007-2008 the V8 project authors. All rights reserved.
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

//#include <cstdint> // arg! Requires C++0x!
#include <stdint.h> // hope the client's platform is recent!
#include <string>
#include <cstring>
#include <list>
#include <vector>
#include <map>
#include "bind.h"
#include "convert.h"
namespace v8 { namespace juice { namespace convert {
    using namespace v8;
    /** Convenience typedef. */
    typedef Handle<Value> ValueHandle;

#if ! defined(DOXYGEN)
    /** Internal library details. */
    namespace Detail
    {

    }
#endif // !DOXYGEN

    /**
       Identical to FwdToFunc1(), but for functions taking 0
       arguments. The cx argument is ignored, but exists for
       consistency with the other FwdToFuncN() functions.
    */
    template <typename ReturnT, typename Func>
    ::v8::Handle< ::v8::Value > FwdToFunc0( ::v8::juice::bind::BindKeyType cx, Func func, ::v8::Arguments const & argv )
    {
	if( argv.Length() != 0 ) return ::v8::ThrowException(::v8::String::New("Function expects exactly 0 arguments!"));
	return CastToJS( func() );
    }

    /**
       Uses CastFromJS() to convert function arguments for the given
       function. The return result is then cast to JS using
       CastToJS(), and returned.

       This routine will only work with types which work with
       CastFromJS() and CastToJS(). Native types will by default be
       sought using ::v8::juice::bind::GetBoundNative(), using the given
       binder context as he first parameter to that function. For the
       "standard" POD types, the converter will normally ignore the cx
       parameter, and 0 can safely be passed.

       For all functions of this family (FwdToFunc2(), etc.), if
       argv.Length() is not exactly the correct number then a JS-side
       exception is thrown - the return value will be that of
       ::v8::ThrowException() (which isn't really documented :/).

       Notable limitations:

       - (char const *) parameters will not be properly converted
       because the CastFromJS() implementation for those uses
       std::string as a proxy, which cannot be implicitly converted
       to (char const *).

       - Arguments with reference qualifiers wont work for non-JS-standard
       types, e.g. a parameter type of (T const &). For the standard types,
       const references should work okay but non-const references will not.

       - Pointer arguments are handled correctly for custom types but
       not for JS-standard types (which are necessarily passed by
       value because we don't have a pointer to their native
       representations).

       - Non-pointer/reference-qualified custom types will not work, because
       CastFromJS() and friends pass around pointers.

       - Void return types are not handled correctly (won't compile).

       - All native types relying on CastFromJS() must be bound with
       the same context key (the cx parameter).

       - When passing copyable args by const reference (e.g. std::string const &),
       the template types passed must be without the reference part (and optionally
       without the const).

       Some of these limitations can be solved by adding another level
       or two of template-based indirection. Maybe that will happen
       someday, maybe not.
    */
    template <typename ReturnT, typename A1, typename Func>
    ::v8::Handle< ::v8::Value > FwdToFunc1( ::v8::juice::bind::BindKeyType cx, Func func, ::v8::Arguments const & argv )
    {
	if( argv.Length() != 1 ) return ::v8::ThrowException(::v8::String::New("Function expects exactly 1 argument!"));
	return CastToJS( func( CastFromJS<A1>( cx, argv[0] ) ) );
    }

    /**
       Identical to FwdToFunc1(), but for functions taking 2 arguments.
    */
    template <typename ReturnT, typename A1, typename A2, typename Func>
    ::v8::Handle< ::v8::Value > FwdToFunc2( ::v8::juice::bind::BindKeyType cx, Func func, ::v8::Arguments const & argv )
    {
	if( argv.Length() != 2 ) return ::v8::ThrowException(::v8::String::New("Function expects exactly 2 arguments!"));
	return CastToJS( func(
			      CastFromJS<A1>( cx, argv[0] ),
			      CastFromJS<A2>( cx, argv[1] ) ) );
    }

    /**
       Identical to FwdToFunc1(), but for functions taking 3 arguments.
    */
    template <typename ReturnT, typename A1, typename A2, typename A3, typename Func>
    ::v8::Handle< ::v8::Value > FwdToFunc3( ::v8::juice::bind::BindKeyType cx, Func func, ::v8::Arguments const & argv )
    {
	if( argv.Length() != 3 ) return ::v8::ThrowException(::v8::String::New("Function expects exactly 3 arguments!"));
	return CastToJS( (ReturnT) func(
			      CastFromJS<A1>( cx, argv[0] ),
			      CastFromJS<A2>( cx, argv[1] ),
			      CastFromJS<A3>( cx, argv[2] )
			      ) );
    }

    /**
       Identical to FwdToFunc1(), but for functions taking 4 arguments.
    */
    template <typename ReturnT, typename A1, typename A2, typename A3, typename A4, typename Func>
    ::v8::Handle< ::v8::Value > FwdToFunc4( ::v8::juice::bind::BindKeyType cx, Func func, ::v8::Arguments const & argv )
    {
	if( argv.Length() != 4 ) return ::v8::ThrowException(::v8::String::New("Function expects exactly 4 arguments!"));
	return CastToJS( (ReturnT) func(
			      CastFromJS<A1>( cx, argv[0] ),
			      CastFromJS<A2>( cx, argv[1] ),
			      CastFromJS<A3>( cx, argv[2] ),
			      CastFromJS<A4>( cx, argv[3] )
			      ) );
    }

    /**
       Identical to FwdToFunc1(), but for functions taking 5 arguments.
    */
    template <typename ReturnT, typename A1, typename A2, typename A3, typename A4, typename A5, typename Func>
    ::v8::Handle< ::v8::Value > FwdToFunc5( ::v8::juice::bind::BindKeyType cx, Func func, ::v8::Arguments const & argv )
    {
	if( argv.Length() != 5 ) return ::v8::ThrowException(::v8::String::New("Function expects exactly 5 arguments!"));
	return CastToJS( (ReturnT) func(
			      CastFromJS<A1>( cx, argv[0] ),
			      CastFromJS<A2>( cx, argv[1] ),
			      CastFromJS<A3>( cx, argv[2] ),
			      CastFromJS<A4>( cx, argv[3] ),
			      CastFromJS<A5>( cx, argv[4] )
			      ) );
    }

    /**
       Identical to FwdToFunc1(), but for functions taking 6 arguments.
    */
    template <typename ReturnT, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename Func>
    ::v8::Handle< ::v8::Value > FwdToFunc6( ::v8::juice::bind::BindKeyType cx, Func func, ::v8::Arguments const & argv )
    {
	if( argv.Length() != 6 ) return ::v8::ThrowException(::v8::String::New("Function expects exactly 6 arguments!"));
	return CastToJS( (ReturnT) func(
			      CastFromJS<A1>( cx, argv[0] ),
			      CastFromJS<A2>( cx, argv[1] ),
			      CastFromJS<A3>( cx, argv[2] ),
			      CastFromJS<A4>( cx, argv[3] ),
			      CastFromJS<A5>( cx, argv[4] ),
			      CastFromJS<A6>( cx, argv[5] )
			      ) );
    }

    /**
       Identical to FwdToFunc1(), but for functions taking 7 arguments.
    */
    template <typename ReturnT, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename Func>
    ::v8::Handle< ::v8::Value > FwdToFunc7( ::v8::juice::bind::BindKeyType cx, Func func, ::v8::Arguments const & argv )
    {
	if( argv.Length() != 7 ) return ::v8::ThrowException(::v8::String::New("Function expects exactly 7 arguments!"));
	return CastToJS( (ReturnT) func(
			      CastFromJS<A1>( cx, argv[0] ),
			      CastFromJS<A2>( cx, argv[1] ),
			      CastFromJS<A3>( cx, argv[2] ),
			      CastFromJS<A4>( cx, argv[3] ),
			      CastFromJS<A5>( cx, argv[4] ),
			      CastFromJS<A6>( cx, argv[5] ),
			      CastFromJS<A7>( cx, argv[6] )
			      ) );
    }

    /**
       Identical to FwdToFunc1(), but for functions taking 8 arguments.
    */
    template <typename ReturnT, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename Func>
    ::v8::Handle< ::v8::Value > FwdToFunc8( ::v8::juice::bind::BindKeyType cx, Func func, ::v8::Arguments const & argv )
    {
	if( argv.Length() != 8 ) return ::v8::ThrowException(::v8::String::New("Function expects exactly 8 arguments!"));
	return CastToJS( (ReturnT) func(
			      CastFromJS<A1>( cx, argv[0] ),
			      CastFromJS<A2>( cx, argv[1] ),
			      CastFromJS<A3>( cx, argv[2] ),
			      CastFromJS<A4>( cx, argv[3] ),
			      CastFromJS<A5>( cx, argv[4] ),
			      CastFromJS<A6>( cx, argv[5] ),
			      CastFromJS<A7>( cx, argv[6] ),
			      CastFromJS<A8>( cx, argv[7] )
			      ));
    }


    /**
       Identical to FwdToFunc1(), but for functions taking 9 arguments. Yes, ncurses
       has a function or two which take this many arguments.
    */
    template <typename ReturnT, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename Func>
    ::v8::Handle< ::v8::Value > FwdToFunc9( ::v8::juice::bind::BindKeyType cx, Func func, ::v8::Arguments const & argv )
    {
	if( argv.Length() != 9 ) return ::v8::ThrowException(::v8::String::New("Function expects exactly 9 arguments!"));
	return CastToJS( (ReturnT) func(
			      CastFromJS<A1>( cx, argv[0] ),
			      CastFromJS<A2>( cx, argv[1] ),
			      CastFromJS<A3>( cx, argv[2] ),
			      CastFromJS<A4>( cx, argv[3] ),
			      CastFromJS<A5>( cx, argv[4] ),
			      CastFromJS<A6>( cx, argv[5] ),
			      CastFromJS<A7>( cx, argv[6] ),
			      CastFromJS<A8>( cx, argv[7] ),
			      CastFromJS<A9>( cx, argv[9] )
			      ));
    }



    /**
       See InvocationCallbackToArgv for details.
    */
    typedef ::v8::Handle< ::v8::Value > (*InvocationCallbackWithArray)( Handle<Object> self, int argc, Handle<Value> argv[] );

    /**
       A helper to allow re-use of certain JS/C++ functions. It's a bit of
       a long story...

       v8 defines the basic JS/C++ callback type as
       InvocationCallback, which takes a v8::Arguments list as its
       only argument. Since client code cannot create their own
       Arguments lists (only v8 can do that), it is impossible to
       re-use a subset of the arguments and pass them to another
       InvocationHandler. Such forwarding is often useful when
       implementing related functions which share a common basis, but
       v8's callback convention makes it impossible.

       So now the workaround:

       Instead of defining a callback as an InvocationCallback, it can
       be defined as a InvocationCallbackWithArray, and this type can
       be used to do the conversion.  For example:

       \code
       myobj->Set(String::New("func"), InvocationCallbackToArgv<MyCallback>::call );
       \endcode

       This is of course less efficient than directly calling an
       InvocationCallback, because we must synthesize an array of
       Value handles.

       The optional skipArgN parameter tells call() than it should skip
       over the first N arguments in the list, which can be useful when
       stripping a first argument for personal use then passing on the
       rest of the args.
    */
    template <InvocationCallbackWithArray proxy, int skipArgN = 0>
    struct InvocationCallbackToArgv
    {
	/**
	   Synthesizes an array of Value handles and calls
	   proxy(argv.This(),argCount,argsArray). If skipArgN is
	   greater than or equal to argv.Length() then
	   proxy(argv.This(),0,0) is called.
	*/
	static ::v8::Handle< ::v8::Value > call( ::v8::Arguments const & argv )
	{
	    typedef Handle<Value> HV;
	    if( skipArgN >= argv.Length() )
	    { // Is this sensible? Should we throw instead? Hmm.
		return proxy( argv.This(), 0, 0 );
	    }
	    const int argc = argv.Length() - skipArgN;
	    std::vector< HV > vec( static_cast<size_t>(argc), Null() );
	    int pos = 0;
	    for( int i = skipArgN; i < argc; ++i, ++pos )
	    {
		vec[pos] = argv[i];
	    }
	    return proxy( argv.This(), argc, &vec[0] );
	}
    };

}}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_V8_FORWARDING_H_INCLUDED */
