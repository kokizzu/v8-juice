#if !defined(CODE_GOOGLE_COM_P_V8_JUICE_CLASSBINDER_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_JUICE_CLASSBINDER_H_INCLUDED 1
/*
   A JS/C++ class binding mechanism based on the juice::convert framework.

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: Public Domain
*/

#include <v8.h>
#include <v8/juice/WeakJSClassCreator.h>

namespace v8 {
namespace juice {

#if !defined(DOXYGEN)
    namespace Detail
    {
	using namespace v8;
	using namespace v8::juice;

	/**
	   A helper type for forwarding JS arguments to member functions taking 0
	   arguments. The classes MemFuncCaller(1..N) are generated but follow
	   this class' API.

	   All variants of this class except the nullary one throw if the argument
	   list does not have at least the required number of parameters.
	*/
	struct MemFuncCaller0
	{
	    enum { Arity = 0 };
	    template <typename T, typename RV>
	    static Handle<Value> Call( T * obj, RV (T::*MemFunc)(), Arguments const & argv )
	    {
		if( ! obj ) return ThrowException(String::New("MemFuncCaller0::Call(): Native object is null!"));
		//else if( argv.Length() < Arity ) return ThrowException(String::New("${callBase}::Call(): wrong argument count!"));
		return convert::CastToJS<RV>( (obj->*MemFunc)() );
	    }

	    template <typename T, typename RV>
	    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)() const, Arguments const & argv )
	    {
		if( ! obj ) return ThrowException(String::New("MemFuncCaller0::Call(): Native object is null!"));
		//else if( argv.Length() < Arity ) return ThrowException(String::New("${callBase}::Call(): wrong argument count!"));
		return convert::CastToJS<RV>( (obj->*MemFunc)() );
	    }

	    template <typename T>
	    static Handle<Value> Call( T * obj, void (T::*MemFunc)(), Arguments const & argv )
	    {
		if( ! obj ) return ThrowException(String::New("MemFuncCaller0::Call(): Native object is null!"));
		//else if( argv.Length() < Arity ) return ThrowException(String::New("${callBase}::Call(): wrong argument count!"));
		(obj->*MemFunc)();
		return Undefined();
	    }

	    template <typename T>
	    static Handle<Value> Call( T const * obj, void (T::*MemFunc)() const, Arguments const & argv )
	    {
		if( ! obj ) return ThrowException(String::New("MemFuncCaller0::Call(): Native object is null!"));
		//else if( argv.Length() < Arity ) return ThrowException(String::New("${callBase}::Call(): wrong argument count!"));
		(obj->*MemFunc)();
		return Undefined();
	    }
	};

	/**
	   A helper type for forwarding function calls on objects bound via WeakJSClassCreator
	   through native member functions. This type supports only nullary functions, and
	   the generated classes WeakMemFuncCaller(1..N) each provide support for a different
	   number of arguments.
	*/
	struct WeakMemFuncCaller0 : MemFuncCaller0
	{
	    template <typename WeakWrappedType, typename RV>
	    static Handle<Value> CallOnWeakSelf( RV (WeakWrappedType::*func)(), Arguments const & argv )
	    {
		typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
		typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
		if( ! obj ) return ThrowException(String::New("MemFuncCaller0<>::Call() could not find native 'this' object!"));
		return Call( obj, func, argv );
	    }

	    template <typename WeakWrappedType, typename RV>
	    static Handle<Value> CallOnWeakSelf( const RV (WeakWrappedType::*func)() const, Arguments const & argv )
	    {
		typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
		typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
		if( ! obj ) return ThrowException(String::New("MemFuncCaller0<>::Call() could not find native 'this' object!"));
		return Call( obj, func, argv );
	    }

	    template <typename WeakWrappedType>
	    static Handle<Value> CallOnWeakSelf( void (WeakWrappedType::*func)(), Arguments const & argv )
	    {
		typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
		typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
		if( ! obj ) return ThrowException(String::New("MemFuncCaller0<>::Call() could not find native 'this' object!"));
		return Call( obj, func, argv );
	    }

	    template <typename WeakWrappedType>
	    static Handle<Value> CallOnWeakSelf( const void (WeakWrappedType::*func)() const, Arguments const & argv )
	    {
		typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
		typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
		if( ! obj ) return ThrowException(String::New("MemFuncCaller0<>::Call() could not find native 'this' object!"));
		return Call( obj, func, argv );
	    }
	};

#include "ClassBinder-MemFuncCaller.h" // generated code

	/** An internal helper type for converting v8::Arguments to native args
	    and passing them on to the given native member function. This type
	    supports only functions with no arguments. The generated classes
	    MemFuncCallOp(1..N) add support for various argument counts.
	*/
	template <typename T, typename RV, RV (T::*Func)()  >
	struct MemFuncCallOp0
	{
	    typedef T Type;
	    enum { Arity = 0 };
	    typedef RV (T::*FuncSig)();
	    typedef WeakMemFuncCaller0 OpBase;
	    static Handle<Value> Call( Type * obj, Arguments const & argv )
	    {
		return OpBase::Call( obj, Func, argv );
	    }
	    static Handle<Value> CallOnWeakSelf( Arguments const & argv )
	    {
		return OpBase::CallOnWeakSelf<Type>( Func, argv );
	    }
	};

	/** Specializations for const members. */
	template <typename T, typename RV, RV (T::*Func)() const>
	struct MemFuncCallOp0<const T,RV,Func>
	{
	    typedef T Type;
	    enum { Arity = 0 };
	    typedef WeakMemFuncCaller0 OpBase;
	    static Handle<Value> Call( Type const * obj, Arguments const & argv )
	    {
		if( ! obj ) return ThrowException(String::New("MemFuncCallOp0::Call(): Native object is null!"));
		return convert::CastToJS( (RV) (obj->*Func)() );
	    }
	    static Handle<Value> CallOnWeakSelf( Arguments const & argv )
	    {
		return OpBase::CallOnWeakSelf<const Type>( Func, argv );
	    }
	};

	/** Specializations for members returning void. */
	template <typename T, void (T::*Func)()>
	struct MemFuncCallOp0<T, void, Func >
	{
	    typedef T Type;
	    typedef WeakMemFuncCaller0 OpBase;
	    static Handle<Value> Call( Type * obj, Arguments const & argv )
	    {
		//if( ! obj ) return ThrowException(String::New("MemFuncCallOp0::Call(): Native object is null!"));
		return OpBase::Call( obj, Func, argv );
	    }
	    static Handle<Value> CallOnWeakSelf( Arguments const & argv )
	    {
		return OpBase::CallOnWeakSelf<Type>( Func, argv );
	    }
	};

	/** Specializations for const members returning void. */
	template <typename T, void (T::*Func)() const>
	struct MemFuncCallOp0<const T, void, Func >
	{
	    typedef T Type;
	    typedef WeakMemFuncCaller0 OpBase;
	    static Handle<Value> Call( Type const * obj, Arguments const & argv )
	    {
		return OpBase::Call( obj, Func, argv );
	    }
	    static Handle<Value> CallOnWeakSelf( Arguments const & argv )
	    {
		return OpBase::CallOnWeakSelf<const Type>( Func, argv );
	    }
	};

#include "ClassBinder-MemFuncCallOps.h" // generated code

	/**
	   Helper used by ClassBinder::AddMemberFunc(). CallOpType
	   must be an existing MemFuncCallOpN type, where N is an
	   integer value.
	*/
	template <typename CallOpType,typename RT>
	struct MemFuncCallOp
	{
 	    typedef typename CallOpType::Type Type;
	    typedef WeakJSClassCreator<Type> Wrapper;
	    static Handle<Value> Call( Arguments const & argv )
	    {
		return CallOpType::CallOnWeakSelf( argv );
	    }
	};

	/**
	   Helper used by ClassBinder::AddMemberFunc(). CallOpType
	   must be an existing MemFuncCallOpN type, where N is an
	   integer value.
	*/
	template <typename CallOpType>
	struct MemFuncCallOp<CallOpType,void>
	{
	    typedef typename CallOpType::Type Type;
	    typedef WeakJSClassCreator<Type> Wrapper;
	    static Handle<Value> Call( Arguments const & argv )
	    {
		return CallOpType::CallOnWeakSelf( argv );
	    }
	};
    }
#endif // DOXYGEN

#if 0 // not yet read for prime-time
    /**
    */
    template <typename WrappedType, typename RV, RV (WrappedType::*Func)()>
    void WeakBindMemFunc( WeakJSClassCreator<WrappedType> & tgt, char const * name )
    {
	typedef Detail::MemFuncCallOp0< WrappedType, RV, Func > Caller;
	tgt.Set(name, Detail::MemFuncCallOp<Caller,RV>::Call );
    }
    
    /**
       Overloaded to handle const member functions.
    */
    template <typename WrappedType,typename RV, RV (WrappedType::*Func)() const>
    void WeakBindMemFunc( WeakJSClassCreator<WrappedType> & tgt, char const * name )
    {
	typedef Detail::MemFuncCallOp0< const typename WeakJSClassCreator<WrappedType>::WrappedType, RV, Func > Caller;
	tgt.Set(name, Detail::MemFuncCallOp<Caller,RV>::Call );
    }

    template <typename NativeType, typename RV>
    Handle<Value> CallWeakMemFunc( RV (NativeType::*MemFunc)(), Arguments const & argv  )
    {
	typedef Detail::WeakMemFuncCaller0 Caller;
	return Caller::CallOnWeakSelf<NativeType>( MemFunc, argv );
    }
    template <typename NativeType, typename RV>
    Handle<Value> CallWeakMemFunc( const RV (NativeType::*MemFunc)() const, Arguments const & argv )
    {
	typedef Detail::WeakMemFuncCaller0 Caller;
	return Caller::CallOnWeakSelf<const NativeType>( MemFunc, argv );
    }
#endif

    /**
       InvocationCallbackMember is a helper type for binding InvocationCallback-like
       member functions. It requires that T be supported by WeakJSClassCreator. The Func
       template parameter is the member invocation callback which we want to proxy.
    */
    template <typename T,
              ::v8::Handle< ::v8::Value > (T::*Func)( ::v8::Arguments const & argv )
              >
    struct InvocationCallbackMember
    {
	/**
           Extracts a native T object from argv using
           WeakJSClassCreator<T>::GetSelf() and passes the call on to
           obj->Func(). If no object can be found it throws a JS
           exception, otherwise it returns the result of the proxied call.
	*/
	static ::v8::Handle< ::v8::Value > Call( ::v8::Arguments const & argv )
	{
            T * self = WeakJSClassCreator<T>::GetSelf( argv.This() );
            if( ! self ) return ThrowException(String::New("InvocationCallbackMember could not find native 'this' object in argv!"));
            return (self->*Func)( argv );
	}
    };

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

        template < Handle<Value> (WrappedType::*Func)( Arguments const & ) >
        ClassBinder & BindMemFunc( char const * name )
        {
            this->Set(name, InvocationCallbackMember<WrappedType,Func>::Call );
            return *this;
        }

	/**
	   Binds the member function Func to the given name.
	*/
	template <typename RV, RV (WrappedType::*Func)()>
	ClassBinder & BindMemFunc( char const * name )
	{
	    typedef Detail::MemFuncCallOp0< WrappedType, RV, Func > Caller;
	    this->Set(name, Detail::MemFuncCallOp<Caller,RV>::Call );
	    return *this;
	}

	/**
	   Overload requiring a WrappedType const member function taking no
	   arg and returning RV.
	*/
	template <typename RV, RV (WrappedType::*Func)() const>
	ClassBinder & BindMemFunc( char const * name )
	{
	    typedef Detail::MemFuncCallOp0< const WrappedType, RV, Func > Caller;
	    this->Set(name, Detail::MemFuncCallOp<Caller,RV>::Call );
	    return *this;
	}
#include "ClassBinder-BindMemFunc.h" // generated code

    }; // class ClassBinder


    /** Experimental. Don't use. */
    template <typename WrappedT>
    class WrappedClassBase
    {
    public:
        typedef WrappedT WrappedType;
        explicit WrappedClassBase( Handle<Object> jsThis )
            : jself(jsThis)
        {
        }
        explicit WrappedClassBase( Arguments const & argv )
            : jself(argv.This())
        {
        }

        virtual ~WrappedClassBase() {}

        Handle<Object> GetJSSelf() const
        {
            return this->jself;
        }
    private:
        Handle<Object> jself;
    };

}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_JUICE_CLASSBINDER_H_INCLUDED */
