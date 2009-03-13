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

	template <typename T, typename RV, RV (T::*Func)() const>
	struct MemFuncCallOp0<const T,RV,Func>
	{
	    typedef const T Type;
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

	template <typename T, void (T::*Func)() const>
	struct MemFuncCallOp0<const T, void, Func >
	{
	    typedef const T Type;
	    enum { Arity = 0 };
	    static Handle<Value> Call( Type * obj, Arguments const & argv )
	    {
		if( ! obj ) return ThrowException(String::New("MemFuncCallOp0::Call(): Native object is null!"));
		(void) (obj->*Func)();
		return Undefined();
	    }
	};

#include "ClassBinder-MemFuncCallOpN.h" // generated code

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
       convert::CastFromJS(), as those are used for all casting).
       It does handle void return values, though CastFromJS()
       does not.

       Example (see also: WeakJSClassCreator)

       \code
       ...
       // Add the class to JS:
       typedef ClassBinder<MyType> WC;
       WC c( "MyType", objectToAddClassTo );
       c.Set(...)
           .BindMemFunc<void,&MyType::doSomething>("doSomething")
	   .Seal(); // must be the last call made on this object.
       \endcode

       That's all there is to it. More functionality is provided via
       the base classes.

       If you've got some weird arguments which cannot be used through
       the casting system, you may be able to partially work around it
       by creating a very thin wrapper class around the to-be-wrapped
       type, make the appropriate conversions there, and use that
       proxy to access the real object.

       Tip: if a compiler error tells you that there's "no matching
       function BindMemFunc()" then one or more of the template types
       passed to BindMemFunc() does not match the exact function
       signature of the function pointer template parameter.
    */
    template <typename WrappedT>
    class ClassBinder : public WeakJSClassCreator<WrappedT>
    {
    public:
	/** Convenience typedef. */
	typedef WeakJSClassCreator<WrappedT> ParentType;
	/**
	   The actual wrapped native type, which may differ from the
	   WrappedT template parameter. It is always the same as
	   WeakJSClassCreatorOps<WrappedT>::WrappedType.

	   It is sometimes useful to hide some internal native type
	   (or opaque pointer type) via a marker class, in which case
	   this type would be passed MyMarkerClass as the template
	   type and the ops specialization would define WrappedType
	   to be the internal type. This has proven useful in the
	   i/o plugin, where we don't want to expose the underlying
	   i/o library implementation to potential users.
	*/
	typedef typename ParentType::WrappedType WrappedType;
	/**
	   Convenience typedef. Always the same as
	   WeakJSClassCreatorOps<WrappedT>.
	*/
	typedef typename ParentType::ClassOpsType ClassOpsType;

	/**
	   Identical to the parent class ctor with the same signature.
	*/
	explicit ClassBinder( Handle<Object> target )
	    : ParentType( target )
	{
	}

	/**
	   Identical to the parent class ctor with the same signature.
	*/
	explicit ClassBinder()
	    : ParentType()
	{
	}

	/** Does nothing. */
	virtual ~ClassBinder() {}

	/**
	   Binds the given nullary member function to the given name.
	*/
	template <typename RV, RV (WrappedType::*Func)()>
	ClassBinder & BindMemFunc( char const * name )
	{
	    typedef Detail::MemFuncCallOp0< WrappedType, RV, Func > Caller;
	    this->Set(name, Detail::MemFuncCallOp<Caller,RV>::Call );
	    return *this;
	}

#if 1
	// const members not yet working
	template <typename RV, RV (WrappedType::*Func)() const>
	ClassBinder & BindMemFunc( char const * name )
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
	ClassBinder & BindMemFunc( char const * name )
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
	ClassBinder & BindMemFunc( char const * name )
	{
	    typedef Detail::MemFuncCallOp2< WrappedType, RV, A0, A1, Func > Caller;
	    this->Set(name, Detail::MemFuncCallOp< Caller,RV >::Call );
	    return *this;
	}

	/**
	   Overload requiring a WrappedType member function taking 3 args and returning RV.
	*/
	template <  typename RV, typename A0, typename A1, typename A2, RV (WrappedType::*Func)( A0, A1, A2 ) >
	ClassBinder & BindMemFunc( char const * name )
	{
	    typedef Detail::MemFuncCallOp3< WrappedType, RV,  A0, A1, A2, Func > Caller;
	    this->Set(name, Detail::MemFuncCallOp< Caller, RV >::Call );
	    return *this;
	}

	/**
	   Overload requiring a WrappedType member function taking 4 args and returning RV.
	*/
	template <  typename RV, typename A0, typename A1, typename A2, typename A3, RV (WrappedType::*Func)( A0, A1, A2, A3 ) >
	ClassBinder & BindMemFunc( char const * name )
	{
	    typedef Detail::MemFuncCallOp4< WrappedType, RV,  A0, A1, A2, A3, Func > Caller;
	    this->Set(name, Detail::MemFuncCallOp< Caller, RV >::Call );
	    return *this;
	}

	/**
	   Overload requiring a WrappedType member function taking 5 args and returning RV.
	*/
	template <  typename RV, typename A0, typename A1, typename A2, typename A3, typename A4, RV (WrappedType::*Func)( A0, A1, A2, A3, A4 ) >
	ClassBinder & BindMemFunc( char const * name )
	{
	    typedef Detail::MemFuncCallOp5< WrappedType, RV,  A0, A1, A2, A3, A4, Func > Caller;
	    this->Set(name, Detail::MemFuncCallOp< Caller, RV >::Call );
	    return *this;
	}


    }; // class ClassBinder

}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_JUICE_CLASSBINDER_H_INCLUDED */
