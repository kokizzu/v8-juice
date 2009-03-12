#if !defined(CODE_GOOGLE_COM_P_V8_JUICE_CLASSBINDER_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_JUICE_CLASSBINDER_H_INCLUDED 1
/*
   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: same as v8 (see below)
*/
// Copyright 2009 Stephan Beal. All rights reserved.  Redistribution
// and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
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

#include <v8.h>

#include <v8/juice/WeakJSClassCreator.h>
#include <v8/juice/forwarding.h>

namespace v8 {
namespace juice {

#if !defined(DOXYGEN)
    namespace Detail
    {
	using namespace v8;
	using namespace v8::juice;

	struct VoidType {};

	// TODO: use this to assist in handling void-returning functions:
	template <typename NonVoid>
	struct Returner
	{
	    typedef Handle< ::v8::Value> ResultType;
	    template <typename T, typename RV, RV (T::*Func)()  >
	    static ResultType Call( T * obj, Arguments const & argv )
	    {
		return convert::CastToJS( (obj->*Func)() );
	    }
	};

	template <>
	struct Returner<void>
	{
	    typedef Handle< ::v8::Value> ResultType;
	    template <typename T, void (T::*Func)()  >
	    static ResultType Call( T * obj, Arguments const & argv )
	    {
		(obj->*Func)();
		return Undefined();
	    }
	};

	template <typename T, typename RV, RV (T::*Func)()  >
	struct MemFuncCallOp0
	{
	    typedef T Type;
	    enum { Arity = 0 };
	    static Handle<Value> Call( Type * obj, Arguments const & argv )
	    {
		if( ! obj ) return ThrowException(String::New("MemFuncCallOp0::Call(): Native object is null!"));
		return convert::CastToJS( (RV) (obj->*Func)() );
	    }
	};

	template <typename T, void (T::*Func)()>
	struct MemFuncCallOp0<T, void, Func >
	{
	    typedef T Type;
	    enum { Arity = 0 };
	    static Handle<Value> Call( Type * obj, Arguments const & argv )
	    {
		if( ! obj ) return ThrowException(String::New("MemFuncCallOp0::Call(): Native object is null!"));
		(void) (obj->*Func)();
		return Undefined();
	    }
	};


	template <typename WrappedType, typename RV, typename A1, RV (WrappedType::*Func)(A1)  >
	struct MemFuncCallOp1
	{
	    typedef WrappedType Type;
	    enum { Arity = 1 };
	    //static Handle<Value> Call( Type * obj, Arguments const & argv )
	    static RV Call( Type * obj, Arguments const & argv )
	    {
#if 0
		if( ! obj ) return ThrowException(String::New("MemFuncCallOp0::Call(): Native object is null!"));
		return convert::CastToJS( (RV) (obj->*Func)() );
#else
		if( ! obj ) return RV();
		else if( argv.Length()<Arity ) return RV();
		return (RV) (obj->*Func)(
					 convert::CastFromJS<A1>(argv[0])
					 );
#endif
	    }
	};

	/** Member function call forwarder for member functions taking 2 arguments. */
	template < typename T, typename RV,  typename A0, typename A1, RV (T::*Func)( A0 , A1 ) >
	struct MemFuncCallOp2
	{
	    typedef T Type;
	    enum { Arity = 2 };
	    static Handle<Value> Call( Type * obj, Arguments const & argv )
	    {
		if( ! obj ) return ThrowException(String::New("MemFuncCallOp2::Call(): Native object is null!"));
		else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp2::Call(): wrong argument count!"));
		return convert::CastToJS( (RV) (obj->*Func)(
							    convert::CastFromJS< A0 >(argv[0]),
							    convert::CastFromJS< A1 >(argv[1])
							    ) );
	    }
	};


	/**
	   Helper used by ClassBinder::AddMemberFunc(). CallOpType
	   must be an existing MemFuncCallOpN type, where N is an
	   integer value.
	*/
	template <typename CallOpType,typename RT = bool>
	struct MemFuncCallOp
	{
	    typedef typename CallOpType::Type Type;
	    enum { Arity = CallOpType::Arity };
	    typedef WeakJSClassCreator<Type> Wrapper;
	    static Handle<Value> Call( Arguments const & argv )
	    {
		Type * obj = Wrapper::GetSelf( argv.This() );
		if( ! obj ) return ThrowException(String::New("MemberForwarder<>::Call() could not find native 'this' object!"));
		return convert::CastToJS( CallOpType::Call( obj, argv ) );
	    }
	};

	template <typename CallOpType>
	struct MemFuncCallOp<CallOpType,void>
	{
	    typedef typename CallOpType::Type Type;
	    enum { Arity = CallOpType::Arity };
	    typedef WeakJSClassCreator<Type> Wrapper;
	    static Handle<Value> Call( Arguments const & argv )
	    {
		Type * obj = Wrapper::GetSelf( argv.This() );
		if( ! obj ) return ThrowException(String::New("MemberForwarder<>::Call() could not find native 'this' object!"));
		CallOpType::Call( obj, argv );
		return Undefined();
	    }
	};

    }
#endif // DOXYGEN
    /**
       ClassBinder is a WeakJSClassCreator subclass which adds some
       binding features which required other v8::juice APIs (as opposed
       to WeakJSClassCreator, which requires only core v8 functionality).

       The extended functionality includes support for binding member
       functions of the wrapped type. It has some limitations on the
       automatic type conversion (see convert::CastToJS() and
       convert::CastFromJS(), as those are used for all casting). A
       notable limitation is that it doesn't currenly handle const
       member funcs or void return values - those currently require
       implementing v8::InvocationCallback handlers for them.

       \code
       ...
       // Add the class to JS:
       typedef ClassBinder<MyType> WC;
       WC c( "MyType", objectToAddClassTo );
       c.Set(...)
	 .Seal(); // must be the last call made on this object.
       \endcode

       That's all there is to it. More functionality is provided via
       the JSClassCreator base class, and there are several static
       member functions for "casting" between the JS/Native worlds
       and for destroying instances of the generated class.
    */
    template <typename WrappedT>
    class ClassBinder : public WeakJSClassCreator<WrappedT>
    {
    public:
	typedef WeakJSClassCreator<WrappedT> ParentType;
	typedef typename ParentType::WrappedType WrappedType;
	typedef typename ParentType::ClassOpsType ClassOpsType;

	explicit ClassBinder( Handle<Object> target )
	    : ParentType( target )
	{
	}

	explicit ClassBinder()
	    : ParentType()
	{
	}
	virtual ~ClassBinder() {}

	template <typename RV, RV (WrappedType::*Func)()>
	ClassBinder & AddMemFunc( char const * name )
	{
	    typedef Detail::MemFuncCallOp0< WrappedType, RV, Func > Caller;
	    this->Set(name, Detail::MemFuncCallOp<Caller,RV>::Call );
	    return *this;
	}

#if 0
	// not working
	template <typename RV, RV (WrappedType::*Func)() const>
	ClassBinder & AddMemFunc( char const * name )
	{
	    typedef Detail::MemFuncCallOp0< const WrappedType, RV, Func > Caller;
	    this->Set(name, Detail::MemFuncCallOp<Caller>::Call );
	    return *this;
	}
#endif
	/**
	   Overload requiring a WrappedType member function taking 1
	   arg and returning RV.
	*/
	template < typename RV, typename A1, RV (WrappedType::*Func)(A1) >
	ClassBinder & AddMemFunc( char const * name )
	{
	    typedef Detail::MemFuncCallOp1< WrappedType, RV, A1, Func > Caller;
	    this->Set(name, Detail::MemFuncCallOp<Caller,RV>::Call );
	    return *this;
	}

	/**
	   Overload requiring a WrappedType member function taking 2
	   args and returning RV.
	*/
	template <  typename RV, typename A0, typename A1, RV (WrappedType::*Func)( A0 , A1 ) >
	ClassBinder & AddMemFunc( char const * name )
	{
	    typedef Detail::MemFuncCallOp2< WrappedType, RV, A0, A1, Func > Caller;
	    this->Set(name, Detail::MemFuncCallOp< Caller,RV >::Call );
	    return *this;
	}

    }; // class ClassBinder

}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_JUICE_CLASSBINDER_H_INCLUDED */
