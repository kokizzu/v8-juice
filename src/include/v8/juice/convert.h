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
	struct to_js_f<NT *> : to_js_f<NT> {};
// 	{
// 	    /**
// 	       Returns a v8::External which references &p (not p directly,
// 	       because of the non-constness of the argument!).
// 	    */
// 	    ValueHandle operator()( NT const * p ) const
// 	    {
// 		return External::New( &p );
// 	    }
// 	};

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

	template <typename T>
	struct to_js_f< ::v8::Handle<T> >
	{
	    typedef ::v8::Handle<T> handle_type;
	    ValueHandle operator()( handle_type & li ) const
	    {
		return li;
	    }
	};

	template <typename T>
	struct to_js_f< ::v8::Local<T> >
	{
	    typedef ::v8::Local<T> handle_type;
	    ValueHandle operator()( handle_type const & li ) const
	    {
		return li;
	    }
	};

	template <typename T>
	struct to_js_f< ::v8::Persistent<T> >
	{
	    typedef ::v8::Persistent<T> handle_type;
	    ValueHandle operator()( handle_type const & li ) const
	    {
		return li;
	    }
	};

// 	template <>
// 	struct to_js_f< ::v8::Function >
// 	{
// 	    ValueHandle operator()( ::v8::Function const & li ) const
// 	    {
// 		return Handle<Function>(li);
// 	    }
// 	};


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
		/** This use of v.data() instead of v.c_str() is highly arguable. */
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
		///CERR << "ex == " << ex << " data="<<(ex?ex->Value():0)<<"\n";
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
       Adds these script-side features to the given target object:

       Functions:

       - escape_js_string(string) escapes its argument as a JS string and returns it.

       - quote_js_string(string) like escape_js_string(), but also adds quotes around
       the returned value.

       - sprintf(), as described in v8-sprintf.h.
    */
    ::v8::Handle< ::v8::Value > SetupAddon( ::v8::Handle< ::v8::Object > target );
				       

}}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_V8_CONVERT_H_INCLUDED */
