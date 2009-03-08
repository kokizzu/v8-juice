#if !defined(CODE_GOOGLE_COM_P_V8_V8_CONVERT_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_V8_CONVERT_H_INCLUDED 1
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
namespace v8 {
namespace juice {

/**
   The convert namespace encapsulates routines and types for converting
   between JS and native values using the v8 API.
*/
namespace convert {
    using namespace v8;
    /** Convenience typedef. */
    typedef Handle<Value> ValueHandle;

#if ! defined(DOXYGEN)
    /** Internal library details. */
    namespace Detail
    {

	/**
	   Base instantiation for T-to-ValueHandle conversion functor.
	*/
	template <typename NT>
	struct to_js_f
	{
	    template <typename X>
	    ValueHandle operator()( X const & ) const;
	    // won't compile if not specialized.
	};
	template <typename NT>
	struct to_js_f<NT *>
	{
	    /**
	       Returns a v8::External which references &p (not p directly,
	       because of the non-constness of the argument!).
	    */
	    ValueHandle operator()( NT const * p ) const
	    {
		return External::New( &p );
	    }
	};

	template <>
	struct to_js_f<void>
	{
	    /**
	       Returns a v8::External which references &p (not p directly,
	       because of the non-constness of the argument!).
	    */
	    template <typename AnyT>
	    ValueHandle operator()( AnyT const &  ) const
	    {
		return ::v8::Undefined();
	    }
	};

	template <typename IntegerT>
	struct to_js_f_int_small
	{
	    ValueHandle operator()( IntegerT v ) const
	    {
		return Integer::New( static_cast<int32_t>(v) );
	    }
	};

	template <>
	struct to_js_f<int16_t> : to_js_f_int_small<int16_t> {};

	template <>
	struct to_js_f<uint16_t> : to_js_f_int_small<uint16_t> {};

	template <>
	struct to_js_f<int32_t> : to_js_f_int_small<int32_t> {};

	template <>
	struct to_js_f<uint32_t> : to_js_f_int_small<uint32_t> {};


	template <typename IntegerT>
	struct to_js_f_int_big
	{
	    ValueHandle operator()( IntegerT v ) const
	    {
		return Number::New( static_cast<double>(v) );
	    }
	};

	template <>
	struct to_js_f<int64_t> : to_js_f_int_big<int64_t> {};

	template <>
	struct to_js_f<uint64_t> : to_js_f_int_big<uint64_t> {};

	template <>
	struct to_js_f<double>
	{
	    ValueHandle operator()( double v ) const
	    {
		return Number::New( v );
	    }
	};

	template <>
	struct to_js_f<bool>
	{
	    ValueHandle operator()( bool v ) const
	    {
		return Boolean::New( v );
	    }
	};

	template <typename ListT>
	struct to_js_f_list
	{
	    ValueHandle operator()( ListT const & li ) const
	    {
		typedef typename ListT::const_iterator IT;
		IT it = li.begin();
		size_t sz = li.size();
		Handle<Array> rv( Array::New( static_cast<int>(sz) ) );
		for( int i = 0; li.end() != it; ++it, ++i )
		{
		    rv->Set( Integer::New(i), CastToJS( *it ) );
		}
		return rv;
	    }
	};

	template <typename T>
	struct to_js_f< std::list<T> > : to_js_f_list< std::list<T> > {};

	template <typename T>
	struct to_js_f< std::vector<T> > : to_js_f_list< std::vector<T> > {};

	template <typename MapT>
	struct to_js_f_map
	{
	    ValueHandle operator()( MapT const & li ) const
	    {
		typedef typename MapT::const_iterator IT;
		IT it( li.begin() );
		size_t sz = li.size();
		Handle<Object> rv( Object::New() );
		for( int i = 0; li.end() != it; ++it, ++i )
		{
		    rv->Set( CastToJS( (*it).first ), CastToJS( (*it).second ) );
		}
		return rv;
	    }
	};

	template <typename KeyT,typename ValT>
	struct to_js_f< std::map<KeyT,ValT> > : to_js_f_map< std::map<KeyT,ValT> > {};

#if 0
	// apparently doesn't do what i want...
	template <>
	struct to_js_f<std::string const &>
	{
	    ValueHandle operator()( std::string const & v ) const
	    {
		return String::New( v.data(), static_cast<int>( v.size() ) );
	    }
	};
#endif

	template <>
	struct to_js_f<ValueHandle>
	{
	    ValueHandle operator()( ValueHandle const & v ) const
	    {
		return v;
	    }
	};

	template <>
	struct to_js_f<std::string>
	{
	    ValueHandle operator()( std::string const & v ) const
	    {
		return String::New( v.data(), static_cast<int>( v.size() ) );
	    }
	};
	
	template <>
	struct to_js_f<char const *>
	{
	    ValueHandle operator()( char const * v ) const
	    {
		return String::New( v ? v : "", v ? std::strlen(v) : 0 );
		/** String::New() internally calls strlen(), which hates it when string==0. */
	    }
	};
    }
#endif // !DOXYGEN

    /**
       "Casts" v to a JS value using Detail::to_js_f<T>.

       TODO: use template metaprogramming to figure out the
       exact parameter type, instead of using (T const &).
    */
    template <typename T>
    ValueHandle CastToJS( T const & v )
    {
	typedef Detail::to_js_f<T> F;
	return F()( v );
    }

    /**
       Overload to avoid ambiguity in certain calls.
    */
    static inline ValueHandle CastToJS( char const * v )
    {
	typedef Detail::to_js_f<char const *> F;
	return F()( v );
    }

    /** Convenience instance of to_js_f. */
    static const Detail::to_js_f<int16_t> Int16ToJS = Detail::to_js_f<int16_t>();
    /** Convenience instance of to_js_f. */
    static const Detail::to_js_f<uint16_t> UInt16ToJS = Detail::to_js_f<uint16_t>();
    /** Convenience instance of to_js_f. */
    static const Detail::to_js_f<int32_t> Int32ToJS = Detail::to_js_f<int32_t>();
    /** Convenience instance of to_js_f. */
    static const Detail::to_js_f<uint32_t> UInt32ToJS = Detail::to_js_f<uint32_t>();
    /** Convenience instance of to_js_f. */
    static const Detail::to_js_f<int64_t> Int64ToJS = Detail::to_js_f<int64_t>();
    /** Convenience instance of to_js_f. */
    static const Detail::to_js_f<uint64_t> UInt64ToJS = Detail::to_js_f<uint64_t>();
    /** Convenience instance of to_js_f. */
    static const Detail::to_js_f<double> DoubleToJS = Detail::to_js_f<double>();
    /** Convenience instance of to_js_f. */
    static const Detail::to_js_f<bool> BoolToJS = Detail::to_js_f<bool>();
    /** Convenience instance of to_js_f. */
    static const Detail::to_js_f<std::string> StdStringToJS = Detail::to_js_f<std::string>();

#if ! defined(DOXYGEN)
    namespace Detail
    {
	using ::v8::juice::bind::BindKeyType;
	/**
	   Base interface for converting from native objects to JS
	   objects. By default it uses GetBoundNative() to find
	   a match. Specializations may be provided to use custom
	   conversions.
	*/
	template <typename JST>
	struct to_native_f
	{
	    typedef JST * result_type;
	    result_type operator()( BindKeyType cx, ValueHandle const & jv ) const
	    {
		// 	    External * ex = External::Cast( *jv );
		// 	    //CERR << "ex == " << ex << " data="<<(ex?ex->Value():0)<<"\n";
		// 	    return ex ? ::v8::juice::bind::GetBoundNative<type>( *ex ) : 0;
		// requires specialization
		return ::v8::juice::bind::GetBoundNative<JST>( cx, jv );
		//return 0;
	    }
	    result_type operator()( ValueHandle const & h ) const
	    {
		return this->operator()( 0, h );
	    }
	};
	template <typename JST>
	struct to_native_f<JST *> : to_native_f<JST> {};


	template <>
	struct to_native_f<ValueHandle>
	{
	    typedef ValueHandle result_type;
	    result_type operator()( BindKeyType cx, ValueHandle const & h ) const
	    {
		return h;
	    }
	    result_type operator()( ValueHandle const & h ) const
	    {
		return this->operator()( 0, h );
	    }
	};

	template <>
	struct to_native_f<void>
	{
	    typedef void result_type;
	    result_type operator()( BindKeyType cx, ValueHandle const & h ) const
	    {
		return;
	    }
	    result_type operator()( ValueHandle const & h ) const
	    {
		return;
	    }
	};

	template <>
	struct to_native_f<void *>
	{
	    typedef void * result_type;
	    result_type operator()( BindKeyType cx, ValueHandle const & h ) const
	    {
		if( h.IsEmpty() || ! h->IsExternal() ) return 0;
		return External::Cast(*h)->Value();
	    }
	    result_type operator()( ValueHandle const & h ) const
	    {
		return this->operator()( 0, h );
	    }
	};

	template <>
	struct to_native_f<int16_t>
	{
	    typedef int16_t result_type;
	    result_type operator()( ValueHandle const & h ) const
	    {
		return h->IsNumber()
		    ? static_cast<result_type>(h->Int32Value())
		    : 0;
	    }
	    result_type operator()( BindKeyType, ValueHandle const & h ) const
	    {
		return this->operator()( h );
	    }
	};	

	template <>
	struct to_native_f<uint16_t>
	{
	    typedef uint16_t result_type;
	    result_type operator()( ValueHandle const & h ) const
	    {
		return h->IsNumber()
		    ? static_cast<result_type>(h->Int32Value())
		    : 0;
	    }
	    result_type operator()( BindKeyType cx, ValueHandle const & h ) const
	    {
		return this->operator()( h );
	    }
	};

	template <>
	struct to_native_f<int32_t>
	{
	    typedef int32_t result_type;
	    result_type operator()( BindKeyType cx, ValueHandle const & h ) const
	    {
		return this->operator()( h );
	    }
	    result_type operator()( ValueHandle const & h ) const
	    {
		// FIXME: try to lexically cast, if we can
		return h->IsNumber()
		    ? h->Int32Value()
		    : 0;
	    }
	};

	template <>
	struct to_native_f<uint32_t>
	{
	    typedef uint32_t result_type;
	    result_type operator()( BindKeyType cx, ValueHandle const & h ) const
	    {
		return this->operator()( h );
	    }
	    result_type operator()( ValueHandle const & h ) const
	    {
		return h->IsNumber()
		    ? static_cast<result_type>(h->Uint32Value())
		    : 0;
	    }
	};


	template <>
	struct to_native_f<int64_t>
	{
	    typedef int64_t result_type;
	    result_type operator()( BindKeyType cx, ValueHandle const & h ) const
	    {
		return h->IsNumber()
		    ? static_cast<result_type>(h->IntegerValue())
		    : 0;
	    }
	    result_type operator()( ValueHandle const & h ) const
	    {
		return this->operator()( 0, h );
	    }
	};

	template <>
	struct to_native_f<uint64_t>
	{
	    typedef uint64_t result_type;
	    result_type operator()( BindKeyType cx, ValueHandle const & h ) const
	    {
		return this->operator()( h );
	    }
	    result_type operator()( ValueHandle const & h ) const
	    {
		return h->IsNumber()
		    ? static_cast<result_type>(h->IntegerValue())
		    : 0;
	    }
	};

	template <>
	struct to_native_f<double>
	{
	    typedef double result_type;
	    result_type operator()( BindKeyType cx, ValueHandle const & h ) const
	    {
		return h->IsNumber()
		    ? h->NumberValue()
		    : 0;
	    }
	    result_type operator()( ValueHandle const & h ) const
	    {
		return this->operator()( 0, h );
	    }
	};

	template <>
	struct to_native_f<bool>
	{
	    typedef bool result_type;
	    result_type operator()( BindKeyType cx, ValueHandle const & h ) const
	    {
		return h->BooleanValue();
	    }
	    result_type operator()( ValueHandle const & h ) const
	    {
		return this->operator()( 0, h );
	    }
	};

	template <>
	struct to_native_f<std::string>
	{
	    typedef std::string result_type;
	    result_type operator()( BindKeyType cx, ValueHandle const & h ) const
	    { // this causes a crash deep in v8 in some cases.
		String::AsciiValue asc( h );
		return std::string( *asc ? *asc : "" );
	    }
	    result_type operator()( ValueHandle const & h ) const
	    {
		return this->operator()( 0, h );
	    }
	};

#if 0 // 
	/**
	   Nonono! This will Cause Grief when used together with CastFromJS()
	   because the returned pointer will refer to a now-dead std::string
	   after the return.

	   This specialization requires that a single copy per
	   conversion be used. Do not use a shared/static instance for
	   conversions! To enforce this, the operator()() is
	   non-const.
	*/
	template <>
	struct to_native_f<char const *>
	{
	private:
	    std::string kludge;
	    public:
	    typedef char const * result_type;
	    result_type operator()( ValueHandle const & h )
	    {
		this->kludge = to_native_f<std::string>()( h );
		return this->kludge.c_str();
	    }
	    result_type operator()( BindKeyType, ValueHandle const & h )
	    {
		return this->operator()( h );
	    }
	};
#else
	/** Not great, but a happy medium. */
	template <>
	struct to_native_f<char const *> : to_native_f<std::string> {};
#endif

    }
#endif // !DOXYGEN

    /**
       Converts h to an object of type NT, using Detail::to_native_f<NT> to do
       the conversion. For most conversions it is normal to pass 0 for cx, but
       for conversions to work with native types bound via BindNative() this
       function must be given the proper context key.
    */
    template <typename NT>
    typename Detail::to_native_f<NT>::result_type CastFromJS( ::v8::juice::bind::BindKeyType cx, ValueHandle const & h )
    {
	typedef Detail::to_native_f<NT> F;
	typedef typename Detail::to_native_f<NT>::result_type RT;
	return (RT) F()( cx, h );
	// ^^^ that cast is to try to support returning void (MS compilers are known
	// to require a cast for that).
    }

    /**
       Convenience overload intended for types which are not fetched
       using the ::v8::bind API. This includes POD types.
    */
    template <typename NT>
    typename Detail::to_native_f<NT>::result_type CastFromJS( ValueHandle const & h )
    {
	typedef Detail::to_native_f<NT> F;
	typedef typename Detail::to_native_f<NT>::result_type RT;
	return (RT) F()( h );
	// ^^^ that cast is to try to support returning void (MS compilers are known
	// to require a cast for that).
    }

    /** Convenience instance of to_native_f. */
    static const Detail::to_native_f<int16_t> JSToInt16 = Detail::to_native_f<int16_t>();
    /** Convenience instance of to_native_f. */
    static const Detail::to_native_f<uint16_t> JSToUInt16 = Detail::to_native_f<uint16_t>();
    /** Convenience instance of to_native_f. */
    static const Detail::to_native_f<int32_t> JSToInt32 = Detail::to_native_f<int32_t>();
    /** Convenience instance of to_native_f. */
    static const Detail::to_native_f<uint32_t> JSToUInt32 = Detail::to_native_f<uint32_t>();
    /** Convenience instance of to_native_f. */
    static const Detail::to_native_f<int64_t> JSToInt64 = Detail::to_native_f<int64_t>();
    /** Convenience instance of to_native_f. */
    static const Detail::to_native_f<uint64_t> JSToUInt64 = Detail::to_native_f<uint64_t>();
    /** Convenience instance of to_native_f. */
    static const Detail::to_native_f<double> JSToDouble = Detail::to_native_f<double>();
    /** Convenience instance of to_native_f. */
    static const Detail::to_native_f<bool> JSToBool = Detail::to_native_f<bool>();
    /** Convenience instance of to_native_f. */
    static const Detail::to_native_f<std::string> JSToStdString = Detail::to_native_f<std::string>();

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
       Adds these script-side features to the given target object:

       Functions:

       - escape_js_string(string) escapes its argument as a JS string and returns it.

       - quote_js_string(string) like escape_js_string(), but also adds quotes around
       the returned value.

       - sprintf(), as described in v8-sprintf.h.
    */
    ::v8::Handle< ::v8::Value > SetupAddon( ::v8::Handle< ::v8::Object > target );
				       

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

#endif /* CODE_GOOGLE_COM_P_V8_V8_CONVERT_H_INCLUDED */
