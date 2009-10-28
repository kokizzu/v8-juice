#if !defined(CODE_GOOGLE_COM_P_V8_V8_CONVERT_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_V8_CONVERT_H_INCLUDED 1
/*
   A min-framework to simplify casting between v8 JavaScript and
   C++ native objects.

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
#include <v8.h>
#include <stdint.h> // hope the client's platform is recent!
#include <string>
#include <cstring>
#include <list>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>
namespace v8 {
namespace juice {

/**
   The convert namespace encapsulates routines and types for converting
   between JS and native values using the v8 API.
*/
namespace convert {


    /**
       A convenience base type for TypeInfo<T> specializations.
    */
    template <typename T, typename NHT = T *>
    struct TypeInfoBase
    {
        /**
           The unqualified type T. In some special cases, Type _may_
           differ from T.
        */
        typedef T Type;
        /**
           The "handle" type used to pass around native objects
           between the JS and Native worlds.

           In _theory_ we can also use shared/smart pointers with this
           typedef, but that requires custom handling in other
           ClassWrap policies (mainly because we cannot store a
           full-fledged shared pointer object directly inside a JS
           object).
        */
        typedef NHT NativeHandle;

        // MAYBE to do: add this function to get a pointer to the object, e.g.
        // for dereferencing smart pointers. So far it's not been necessary.
        // static NativeHandle Pointer( NativeHandle x ) { return x; }
    };

    /**
       Describes basic type information regarding a type, for purposes
       of static typing during JS-to/from-Native conversions.

       The default instantiation is suitable for most
       cases. Specializations may be required in certain JS/Native
       binding cases.
    */
    template <typename T>
    struct TypeInfo : TypeInfoBase<T>
    {
    };

    template <typename T>
    struct TypeInfo<T *> : TypeInfo<T> {};

    template <typename T>
    struct TypeInfo<T const *> : TypeInfo<T> {};

    template <typename T>
    struct TypeInfo<T const &> : TypeInfo<T> {};


    using namespace v8;

    /** Convenience typedef. */
    typedef Handle<Value> ValueHandle;

    /**
       Base instantiation for T-to-ValueHandle conversion functor.
       Must be specialized or it will not compile.
    */
    template <typename NT>
    struct NativeToJS
    {
	//! Must be specialized.
	template <typename X>
	v8::Handle<v8::Value> operator()( X const & ) const
#if defined(JUICE_STATIC_ASSERT)
        {
            JUICE_STATIC_ASSERT(false,NativeToJS_T_MustBeSpecialized);
        }
#else
        ;
#endif
    };

    template <typename NT>
    struct NativeToJS<NT *> : NativeToJS<NT> {};

    template <typename NT>
    struct NativeToJS<NT &> : NativeToJS<NT> {};

    template <typename NT>
    struct NativeToJS<const NT &> : NativeToJS<NT> {};

    template <>
    struct NativeToJS<void>
    {
	/**
	   Returns Undefined().
	*/
	v8::Handle<v8::Value> operator()(...) const
	{
	    return ::v8::Undefined();
	}
    };

#if !defined(DOXYGEN)
    /**
       Base implementation for "small" integer conversions (<=32
       bits).
    */
    template <typename IntegerT>
    struct NativeToJS_int_small
    {
	v8::Handle<v8::Value> operator()( IntegerT v ) const
	{
	    return Integer::New( static_cast<int32_t>(v) );
	}
    };
#endif // if !defined(DOXYGEN)

    template <>
    struct NativeToJS<int16_t> : NativeToJS_int_small<int16_t> {};

    template <>
    struct NativeToJS<uint16_t> : NativeToJS_int_small<uint16_t> {};

    template <>
    struct NativeToJS<int32_t> : NativeToJS_int_small<int32_t> {};

    template <>
    struct NativeToJS<uint32_t> : NativeToJS_int_small<uint32_t> {};

#if !defined(DOXYGEN)
    /**
       Base implementation for "big" numeric conversions (>32 bits).
    */
    template <typename IntegerT>
    struct NativeToJS_int_big
    {
	v8::Handle<v8::Value> operator()( IntegerT v ) const
	{
	    return Number::New( static_cast<double>(v) );
	}
    };
#endif // if !defined(DOXYGEN)

    template <>
    struct NativeToJS<int64_t> : NativeToJS_int_big<int64_t> {};

    template <>
    struct NativeToJS<uint64_t> : NativeToJS_int_big<uint64_t> {};

    template <>
    struct NativeToJS<double>
    {
	v8::Handle<v8::Value> operator()( double v ) const
	{
	    return Number::New( v );
	}
    };

    template <>
    struct NativeToJS<bool>
    {
	v8::Handle<v8::Value> operator()( bool v ) const
	{
	    return Boolean::New( v );
	}
    };

    template <typename T>
    struct NativeToJS< ::v8::Handle<T> >
    {
	typedef ::v8::Handle<T> handle_type;
	v8::Handle<v8::Value> operator()( handle_type & li ) const
	{
	    return li;
	}
    };

    template <typename T>
    struct NativeToJS< ::v8::Local<T> >
    {
	typedef ::v8::Local<T> handle_type;
	v8::Handle<v8::Value> operator()( handle_type const & li ) const
	{
	    return li;
	}
    };

    template <typename T>
    struct NativeToJS< ::v8::Persistent<T> >
    {
	typedef ::v8::Persistent<T> handle_type;
	v8::Handle<v8::Value> operator()( handle_type const & li ) const
	{
	    return li;
	}
    };

    template <>
    struct NativeToJS< ::v8::InvocationCallback >
    {
	v8::Handle<v8::Value> operator()( ::v8::InvocationCallback f ) const
	{
	    return ::v8::FunctionTemplate::New(f)->GetFunction();
	}
    };


    // 	template <>
    // 	struct NativeToJS< ::v8::Function >
    // 	{
    // 	    v8::Handle<v8::Value> operator()( ::v8::Function const & li ) const
    // 	    {
    // 		return Handle<Function>(li);
    // 	    }
    // 	};



#if 0
    // apparently doesn't do what i want...
    template <>
    struct NativeToJS<std::string const &>
    {
	v8::Handle<v8::Value> operator()( std::string const & v ) const
	{
	    return String::New( v.data(), static_cast<int>( v.size() ) );
	}
    };
#endif

    template <>
    struct NativeToJS< v8::Handle<v8::Value> >
    {
	v8::Handle<v8::Value> operator()( v8::Handle<v8::Value> const & v ) const
	{
	    return v;
	}
    };

    template <>
    struct NativeToJS<std::string>
    {
	v8::Handle<v8::Value> operator()( std::string const & v ) const
	{
	    /** This use of v.data() instead of v.c_str() is highly arguable. */
	    return String::New( v.data(), static_cast<int>( v.size() ) );
	}
    };
	
    template <>
    struct NativeToJS<char const *>
    {
	v8::Handle<v8::Value> operator()( char const * v ) const
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
    v8::Handle<v8::Value> CastToJS( T const & v )
    {
	typedef NativeToJS<T> F;
	return F()( v );
    }
    
    /**
       Overload to avoid ambiguity in certain calls.
    */
    static inline v8::Handle<v8::Value> CastToJS( char const * v )
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

    /**
       Converts a native std::exception to a JS exception.
     */
    template <>
    struct NativeToJS<std::exception>
    {
        /** Calls v8::ThrowException(ex.what()) and returns an empty
            handle. It must call ThrowException() because that is
            apparently the only way to create an Error object from the
            native API.
        */
	v8::Handle<v8::Value> operator()( std::exception const & ex ) const
	{
            char const * msg = ex.what();
	    return v8::ThrowException( String::New( msg ? msg : "unknown std::exception!" ) );
	    /** ^^^ String::New() internally calls strlen(), which hates it when string==0. */
	}
    };
    
    /**
       Base interface for converting from native objects to JS
       objects. By default it uses GetBoundNative() to find
       a match. Specializations may be provided to use custom
       conversions.
    */
    template <typename JST>
    struct JSToNative
    {
	typedef typename TypeInfo<JST>::NativeHandle ResultType;
	ResultType operator()( v8::Handle<v8::Value> const & h ) const;
        // Must be specialized to be useful
    };
    template <typename JST>
    struct JSToNative<JST *> : JSToNative<JST> {};

    template <typename JST>
    struct JSToNative<JST const *> : JSToNative<JST> {};

    template <typename JST>
    struct JSToNative<JST const &> : JSToNative<JST> {};


    template <>
    struct JSToNative<v8::Handle<v8::Value> >
    {
	typedef v8::Handle<v8::Value> ResultType;
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
	{
	    return h;
	}
    };

    template <>
    struct JSToNative<void>
    {
	typedef void ResultType;
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
	{
	    return;
	}
    };

    template <>
    struct JSToNative<void *>
    {
	typedef void * ResultType;
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
	{
	    if( h.IsEmpty() || ! h->IsExternal() ) return 0;
	    return External::Cast(*h)->Value();
	}
    };

    template <>
    struct JSToNative<int16_t>
    {
	typedef int16_t ResultType;
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
	{
	    return h->IsNumber()
		? static_cast<ResultType>(h->Int32Value())
		: 0;
	}
    };	

    template <>
    struct JSToNative<uint16_t>
    {
	typedef uint16_t ResultType;
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
	{
	    return h->IsNumber()
		? static_cast<ResultType>(h->Int32Value())
		: 0;
	}
    };

    template <>
    struct JSToNative<int32_t>
    {
	typedef int32_t ResultType;
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
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
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
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
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
	{
	    return h->IsNumber()
		? static_cast<ResultType>(h->IntegerValue())
		: 0;
	}
    };

    template <>
    struct JSToNative<uint64_t>
    {
	typedef uint64_t ResultType;
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
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
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
	{
	    return h->IsNumber()
		? h->NumberValue()
		: 0;
	}
    };

    template <>
    struct JSToNative<bool>
    {
	typedef bool ResultType;
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
	{
	    return h->BooleanValue();
	}
    };

    template <>
    struct JSToNative<std::string>
    {
	typedef std::string ResultType;
	ResultType operator()( v8::Handle<v8::Value> const & h ) const
	{
	    String::AsciiValue asc( h );
	    return std::string( *asc ? *asc : "" );
	}
    };

#if 0 // 
    /**
       Nonono! This will Cause Grief when used together with CastFromJS()
       because the returned pointer will refer to a now-dead std::string
       after the return.

       This specialization requires that a single copy per
       conversion be used. Do not use a shared/static instance for
       conversions!
    */
    template <>
    struct JSToNative<char const *>
    {
    private:
	std::string kludge;
    public:
	typedef char const * ResultType;
	ResultType operator()( v8::Handle<v8::Value> const & h )
	{
	    this->kludge = JSToNative<std::string>()( h );
	    return this->kludge.c_str();
	}
    };
#else
    /** Not great, but a happy medium. */
    template <>
    struct JSToNative<char const *> : JSToNative<std::string> {};
#endif

    /**
       Converts h to an object of type NT, using JSToNative<NT> to do
       the conversion.
    */
    template <typename NT>
    typename JSToNative<NT>::ResultType CastFromJS( v8::Handle<v8::Value> const & h )
    {
	typedef JSToNative<NT> F;
	return F()( h );
    }
// These seem to be unnecessary:
//     template <typename NT>
//     typename JSToNative<NT>::ResultType CastFromJS( v8::Handle<v8::Object> const & h )
//     {
// #if defined(CERR)
//         CERR << "CastFromJS(Object)\n";
// #endif
// 	//typedef JSToNative<NT> F;
// 	//return F()( h );
//         return CastFromJS<NT>( v8::Handle<v8::Value>(h) );
//     }

//     template <typename NT, typename VT>
//     typename JSToNative<NT>::ResultType CastFromJS( Handle< VT > const & h )
//     {
// #if defined(CERR)
//         CERR << "CastFromJS(Handle<T>)\n";
// #endif
// #if 0
//  	typedef JSToNative<NT> F;
//  	return F()( h );
// #else
//         return CastFromJS<NT>( v8::Handle<v8::Value>( h ) );
// #endif
//     }

//    template <typename NT>
//    typename JSToNative<NT>::ResultType CastFromJS( ::v8::Local< ::v8::Value > const & h )
//    {
// 	typedef JSToNative<NT> F;
// 	return F()( h );
//     }


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
       A utility to append entries to a JS array.

       It is intended to be used like this:

       \code
       Handle<Object> obj = ...;
       ObjectPropSetter set(obj);
       set("propOne", CastToJS(32) )
          ("propTwo", ... )
	  (32, ... )
	  ("func1", CastToJS( anInvocationCallback ) ) // "should" work
	  ;
       \endcode
    */
    class ObjectPropSetter
    {
    private:
	v8::Handle< ::v8::Object > target;
    public:
	/**
	   Initializes this object to use the given array
	   as its append target. Results are undefined if
	   target is not a valid Object.
	*/
	explicit ObjectPropSetter( v8::Handle< v8::Object > obj ) :target(obj)
	{}
	~ObjectPropSetter(){}

	/**
	   Adds an arbitrary property to the target object.
	*/
	template <typename T>
	ObjectPropSetter & operator()( v8::Handle<v8::Value> key, T const & v )
	{
	    this->target->Set(key, CastToJS(v));
	    return *this;
	}

	/**
	   Adds a numeric property to the target object.
	*/
	template <typename T>
	ObjectPropSetter & operator()( int32_t ndx, T const & v )
	{
	    return this->operator()( Integer::New(ndx), v );
	}

	/**
	   Adds a string-keyed property to the target object.
	*/
	template <typename T>
	ObjectPropSetter & operator()( char const * key, T const & v )
	{
	    return this->operator()( String::New(key), v );
	}

	/**
	   Adds an arbtirary property to the target object.

           WTF did i add the tmpl args here for? They DO make
           a difference, but i can't for the life of me remember
           why.          
	*/
	template <typename T1, typename T2>
	ObjectPropSetter & operator()( v8::Handle<v8::Value> key, v8::Handle<v8::Value> v )
	{
	    return this->operator()( key, v );
	}

        /**
           Adds the given function as a member of the target object.
        */
        ObjectPropSetter & operator()( char const * name, v8::InvocationCallback pf )
        {
            return this->operator()( name,
                                     v8::FunctionTemplate::New(pf)->GetFunction() );
        }
        
	/**
	   Returns this object's JS object.
	*/
	Handle< ::v8::Object > Object() const
	{
	    return this->target;
	}
    };

    /**
       NativeToJS classes which act on list types compatible with the
       STL can subclass this to get an implementation.
    */
    template <typename ListT>
    struct NativeToJS_list
    {
	v8::Handle<v8::Value> operator()( ListT const & li ) const
	{
	    typedef typename ListT::const_iterator IT;
	    IT it = li.begin();
	    const size_t sz = li.size();
#if 1
	    Handle<Array> rv( Array::New( static_cast<int>(sz) ) );
	    for( int i = 0; li.end() != it; ++it, ++i )
	    {
		rv->Set( Integer::New(i), CastToJS( *it ) );
	    }
	    return rv;
#else
	    ObjectPropSetter app(Array::New( static_cast<int>(sz) ));
	    for( int32_t i = 0; li.end() != it; ++it, ++i )
	    {
		app( i, CastToJS( *it ) );
	    }
	    return app.Object();
#endif
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
	v8::Handle<v8::Value> operator()( MapT const & li ) const
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
	ResultType operator()( v8::Handle<v8::Value> jv ) const
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


    /**
       A utility class for building up message strings, most notably
       exception messages, using a mixture of native and JS message
       data.

       It is used like a std::ostream:

       @code
       StringBuffer msg;
       msg << "Could not set property "
           << "'" << propName
           <<"' on object " << myJSObject << '!';
       return v8::ThrowException(msg);
       @endcode
    */
    class StringBuffer
    {
    private:
        std::ostringstream os;
    public:
        /**
           Initializes the message stream.
        */
        StringBuffer() : os()
        {
        }

        /**
           Empties out the message buffer. This invalidates any value
           returned from previous calls to the (char const *)
           operator.
        */
        void Clear()
        {
            this->os.str("");
        }

        /**
           Returns a copy of the current message content.
         */
        std::string Content() const
        {
            return this->os.str();
        }

        /**
           Converts the message state to a JS string.
        */
        inline operator v8::Handle<v8::Value>() const
        {
            return CastToJS<std::string>( this->os.str() );
        }

        /**
           Appends to the message using CastFromJS<std::string>(t) 
        */
        template <typename T>
        StringBuffer & operator<<( v8::Handle<T> const t )
        {
            this->os << CastFromJS<std::string>( t );
            return *this;
        }

        /**
           Appends t to the message via std::ostream<<.
        */
        template <typename T>
        StringBuffer & operator<<( T const t)
        {
            this->os << t;
            return *this;
        }
    };

    
}}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_V8_CONVERT_H_INCLUDED */
