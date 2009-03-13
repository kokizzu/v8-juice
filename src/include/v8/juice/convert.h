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

    /**
       Base instantiation for T-to-ValueHandle conversion functor.
    */
    template <typename NT>
    struct NativeToJS
    {
	template <typename X>
	ValueHandle operator()( X const & ) const;
	// won't compile if not specialized.
    };
    template <typename NT>
    struct NativeToJS<NT *> : NativeToJS<NT> {};

    template <typename NT>
    struct NativeToJS<NT &> : NativeToJS<NT> {};

    template <typename NT>
    struct NativeToJS<const NT &> : NativeToJS<NT> {};

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
    struct NativeToJS<void>
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

    /**
       Base implementation for "small" integer conversions (<=32
       bits).
    */
    template <typename IntegerT>
    struct NativeToJS_int_small
    {
	ValueHandle operator()( IntegerT v ) const
	{
	    return Integer::New( static_cast<int32_t>(v) );
	}
    };

    template <>
    struct NativeToJS<int16_t> : NativeToJS_int_small<int16_t> {};

    template <>
    struct NativeToJS<uint16_t> : NativeToJS_int_small<uint16_t> {};

    template <>
    struct NativeToJS<int32_t> : NativeToJS_int_small<int32_t> {};

    template <>
    struct NativeToJS<uint32_t> : NativeToJS_int_small<uint32_t> {};

    /**
       Base implementation for "big" numeric conversions (>32 bits).
    */
    template <typename IntegerT>
    struct NativeToJS_int_big
    {
	ValueHandle operator()( IntegerT v ) const
	{
	    return Number::New( static_cast<double>(v) );
	}
    };

    template <>
    struct NativeToJS<int64_t> : NativeToJS_int_big<int64_t> {};

    template <>
    struct NativeToJS<uint64_t> : NativeToJS_int_big<uint64_t> {};

    template <>
    struct NativeToJS<double>
    {
	ValueHandle operator()( double v ) const
	{
	    return Number::New( v );
	}
    };

    template <>
    struct NativeToJS<bool>
    {
	ValueHandle operator()( bool v ) const
	{
	    return Boolean::New( v );
	}
    };

    template <typename T>
    struct NativeToJS< ::v8::Handle<T> >
    {
	typedef ::v8::Handle<T> handle_type;
	ValueHandle operator()( handle_type & li ) const
	{
	    return li;
	}
    };

    template <typename T>
    struct NativeToJS< ::v8::Local<T> >
    {
	typedef ::v8::Local<T> handle_type;
	ValueHandle operator()( handle_type const & li ) const
	{
	    return li;
	}
    };

    template <typename T>
    struct NativeToJS< ::v8::Persistent<T> >
    {
	typedef ::v8::Persistent<T> handle_type;
	ValueHandle operator()( handle_type const & li ) const
	{
	    return li;
	}
    };

    // 	template <>
    // 	struct NativeToJS< ::v8::Function >
    // 	{
    // 	    ValueHandle operator()( ::v8::Function const & li ) const
    // 	    {
    // 		return Handle<Function>(li);
    // 	    }
    // 	};



#if 0
    // apparently doesn't do what i want...
    template <>
    struct NativeToJS<std::string const &>
    {
	ValueHandle operator()( std::string const & v ) const
	{
	    return String::New( v.data(), static_cast<int>( v.size() ) );
	}
    };
#endif

    template <>
    struct NativeToJS<ValueHandle>
    {
	ValueHandle operator()( ValueHandle const & v ) const
	{
	    return v;
	}
    };

    template <>
    struct NativeToJS<std::string>
    {
	ValueHandle operator()( std::string const & v ) const
	{
	    /** This use of v.data() instead of v.c_str() is highly arguable. */
	    return String::New( v.data(), static_cast<int>( v.size() ) );
	}
    };
	
    template <>
    struct NativeToJS<char const *>
    {
	ValueHandle operator()( char const * v ) const
	{
	    return String::New( v ? v : "", v ? std::strlen(v) : 0 );
	    /** String::New() internally calls strlen(), which hates it when string==0. */
	}
    };

    /**
       "Casts" v to a JS value using NativeToJS<T>.

       TODO: use template metaprogramming to figure out the
       exact parameter type, instead of using (T const &).
    */
    template <typename T>
    ValueHandle CastToJS( T const & v )
    {
	typedef NativeToJS<T> F;
	return F()( v );
    }

    /**
       Overload to avoid ambiguity in certain calls.
    */
    static inline ValueHandle CastToJS( char const * v )
    {
	typedef NativeToJS<char const *> F;
	return F()( v );
    }

    /** Convenience instance of NativeToJS. */
    static const NativeToJS<int16_t> Int16ToJS = NativeToJS<int16_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<uint16_t> UInt16ToJS = NativeToJS<uint16_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<int32_t> Int32ToJS = NativeToJS<int32_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<uint32_t> UInt32ToJS = NativeToJS<uint32_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<int64_t> Int64ToJS = NativeToJS<int64_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<uint64_t> UInt64ToJS = NativeToJS<uint64_t>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<double> DoubleToJS = NativeToJS<double>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<bool> BoolToJS = NativeToJS<bool>();
    /** Convenience instance of NativeToJS. */
    static const NativeToJS<std::string> StdStringToJS = NativeToJS<std::string>();

    using ::v8::juice::bind::BindKeyType;
    /**
       Base interface for converting from native objects to JS
       objects. By default it uses GetBoundNative() to find
       a match. Specializations may be provided to use custom
       conversions.
    */
    template <typename JST>
    struct JSToNative
    {
	typedef JST * ResultType;
	ResultType operator()( BindKeyType cx, ValueHandle const & jv ) const
	{
	    // 	    External * ex = External::Cast( *jv );
	    ///CERR << "ex == " << ex << " data="<<(ex?ex->Value():0)<<"\n";
	    // 	    return ex ? ::v8::juice::bind::GetBoundNative<type>( *ex ) : 0;
	    // requires specialization
	    return ::v8::juice::bind::GetBoundNative<JST>( cx, jv );
	    //return 0;
	}
	ResultType operator()( ValueHandle const & h ) const
	{
	    return this->operator()( 0, h );
	}
    };
    template <typename JST>
    struct JSToNative<JST *> : JSToNative<JST> {};
    template <typename JST>
    struct JSToNative<JST const &> : JSToNative<JST> {};


    template <>
    struct JSToNative<ValueHandle>
    {
	typedef ValueHandle ResultType;
	ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	{
	    return h;
	}
	ResultType operator()( ValueHandle const & h ) const
	{
	    return this->operator()( 0, h );
	}
    };

    template <>
    struct JSToNative<void>
    {
	typedef void ResultType;
	ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	{
	    return;
	}
	ResultType operator()( ValueHandle const & h ) const
	{
	    return;
	}
    };

    template <>
    struct JSToNative<void *>
    {
	typedef void * ResultType;
	ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	{
	    if( h.IsEmpty() || ! h->IsExternal() ) return 0;
	    return External::Cast(*h)->Value();
	}
	ResultType operator()( ValueHandle const & h ) const
	{
	    return this->operator()( 0, h );
	}
    };

    template <>
    struct JSToNative<int16_t>
    {
	typedef int16_t ResultType;
	ResultType operator()( ValueHandle const & h ) const
	{
	    return h->IsNumber()
		? static_cast<ResultType>(h->Int32Value())
		: 0;
	}
	ResultType operator()( BindKeyType, ValueHandle const & h ) const
	{
	    return this->operator()( h );
	}
    };	

    template <>
    struct JSToNative<uint16_t>
    {
	typedef uint16_t ResultType;
	ResultType operator()( ValueHandle const & h ) const
	{
	    return h->IsNumber()
		? static_cast<ResultType>(h->Int32Value())
		: 0;
	}
	ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	{
	    return this->operator()( h );
	}
    };

    template <>
    struct JSToNative<int32_t>
    {
	typedef int32_t ResultType;
	ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	{
	    return this->operator()( h );
	}
	ResultType operator()( ValueHandle const & h ) const
	{
	    // FIXME: try to lexically cast, if we can
	    return h->IsNumber()
		? h->Int32Value()
		: 0;
	}
    };

    template <>
    struct JSToNative<uint32_t>
    {
	typedef uint32_t ResultType;
	ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	{
	    return this->operator()( h );
	}
	ResultType operator()( ValueHandle const & h ) const
	{
	    return h->IsNumber()
		? static_cast<ResultType>(h->Uint32Value())
		: 0;
	}
    };


    template <>
    struct JSToNative<int64_t>
    {
	typedef int64_t ResultType;
	ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	{
	    return h->IsNumber()
		? static_cast<ResultType>(h->IntegerValue())
		: 0;
	}
	ResultType operator()( ValueHandle const & h ) const
	{
	    return this->operator()( 0, h );
	}
    };

    template <>
    struct JSToNative<uint64_t>
    {
	typedef uint64_t ResultType;
	ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	{
	    return this->operator()( h );
	}
	ResultType operator()( ValueHandle const & h ) const
	{
	    return h->IsNumber()
		? static_cast<ResultType>(h->IntegerValue())
		: 0;
	}
    };

    template <>
    struct JSToNative<double>
    {
	typedef double ResultType;
	ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	{
	    return h->IsNumber()
		? h->NumberValue()
		: 0;
	}
	ResultType operator()( ValueHandle const & h ) const
	{
	    return this->operator()( 0, h );
	}
    };

    template <>
    struct JSToNative<bool>
    {
	typedef bool ResultType;
	ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	{
	    return h->BooleanValue();
	}
	ResultType operator()( ValueHandle const & h ) const
	{
	    return this->operator()( 0, h );
	}
    };

    template <>
    struct JSToNative<std::string>
    {
	typedef std::string ResultType;
	ResultType operator()( BindKeyType cx, ValueHandle const & h ) const
	{ // this causes a crash deep in v8 in some cases.
	    String::AsciiValue asc( h );
	    return std::string( *asc ? *asc : "" );
	}
	ResultType operator()( ValueHandle const & h ) const
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
    struct JSToNative<char const *>
    {
    private:
	std::string kludge;
    public:
	typedef char const * ResultType;
	ResultType operator()( ValueHandle const & h )
	{
	    this->kludge = JSToNative<std::string>()( h );
	    return this->kludge.c_str();
	}
	ResultType operator()( BindKeyType, ValueHandle const & h )
	{
	    return this->operator()( h );
	}
    };
#else
    /** Not great, but a happy medium. */
    template <>
    struct JSToNative<char const *> : JSToNative<std::string> {};
#endif

    /**
       Converts h to an object of type NT, using JSToNative<NT> to do
       the conversion. For most conversions it is normal to pass 0 for cx, but
       for conversions to work with native types bound via BindNative() this
       function must be given the proper context key.
    */
    template <typename NT>
    typename JSToNative<NT>::ResultType CastFromJS( ::v8::juice::bind::BindKeyType cx, ValueHandle const & h )
    {
	typedef JSToNative<NT> F;
	typedef typename JSToNative<NT>::ResultType RT;
	return (RT) F()( cx, h );
	// ^^^ that cast is to try to support returning void (MS compilers are known
	// to require a cast for that).
    }

    /**
       Convenience overload intended for types which are not fetched
       using the ::v8::bind API. This includes POD types.
    */
    template <typename NT>
    typename JSToNative<NT>::ResultType CastFromJS( ValueHandle const & h )
    {
	typedef JSToNative<NT> F;
	typedef typename JSToNative<NT>::ResultType RT;
	return (RT) F()( h );
	// ^^^ that cast is to try to support returning void (MS compilers are known
	// to require a cast for that).
    }

    /** Convenience instance of JSToNative. */
    static const JSToNative<int16_t> JSToInt16 = JSToNative<int16_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<uint16_t> JSToUInt16 = JSToNative<uint16_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<int32_t> JSToInt32 = JSToNative<int32_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<uint32_t> JSToUInt32 = JSToNative<uint32_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<int64_t> JSToInt64 = JSToNative<int64_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<uint64_t> JSToUInt64 = JSToNative<uint64_t>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<double> JSToDouble = JSToNative<double>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<bool> JSToBool = JSToNative<bool>();
    /** Convenience instance of JSToNative. */
    static const JSToNative<std::string> JSToStdString = JSToNative<std::string>();

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
       NativeToJS classes which act on list types compatible with the
       STL can subclass this to get an implementation.
    */
    template <typename ListT>
    struct NativeToJS_list
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
    /** Partial specialization for std::list<>. */
    template <typename T>
    struct NativeToJS< std::list<T> > : NativeToJS_list< std::list<T> > {};
    /** Partial specialization for std::vector<>. */
    template <typename T>
    struct NativeToJS< std::vector<T> > : NativeToJS_list< std::vector<T> > {};

    /**
       NativeToJS classes which act on map types compatible with the
       STL can subclass this to get an implementation.
    */
    template <typename MapT>
    struct NativeToJS_map
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

    /** Partial specialization for std::map<>. */
    template <typename KeyT,typename ValT>
    struct NativeToJS< std::map<KeyT,ValT> > : NativeToJS_map< std::map<KeyT,ValT> > {};


    template <typename ListT>
    struct JSToNative_list
    {
	typedef ListT ResultType;
	ResultType operator()( ValueHandle jv ) const
	{
	    typedef typename ListT::const_iterator IT;
	    typedef typename ListT::value_type VALT;
	    ListT li;
	    if( ! jv->IsArray() ) return li;
	    Handle<Array> ar( Array::Cast(*jv) );
	    uint32_t ndx = 0;
	    for( ; ar->Has(ndx); ++ndx )
	    {
		li.push_back( CastFromJS<VALT>( ar->Get(Integer::New(ndx)) ) );
	    }
	    return li;
	}
    };

    /** Partial specialization for std::list<>. */
    template <typename T>
    struct JSToNative< std::list<T> > : JSToNative_list< std::list<T> > {};

    /** Partial specialization for std::vector<>. */
    template <typename T>
    struct JSToNative< std::vector<T> > : JSToNative_list< std::vector<T> > {};

}}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_V8_CONVERT_H_INCLUDED */
