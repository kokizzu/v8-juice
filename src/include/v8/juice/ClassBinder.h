#if !defined(CODE_GOOGLE_COM_P_V8_JUICE_CLASSBINDER_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_JUICE_CLASSBINDER_H_INCLUDED 1
/*
   A JS/C++ class binding mechanism based on the juice::convert framework.

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: Public Domain
*/

#include <v8.h>
#include <v8/juice/convert.h>
#include <v8/juice/WeakJSClassCreator.h>

namespace v8 {
namespace juice {

#if !defined(DOXYGEN)
    namespace Detail
    {
	using namespace v8;
	using namespace v8::juice;

        /**
           Useless base instantiation - See the 0-specialization for details.
        */
        template <int Arity_>
        struct MemFuncCaller
        {
            enum { Arity = Arity_ };
        };
	/**
	   A helper type for forwarding JS arguments to member functions taking 0
	   arguments. The classes MemFuncCaller(1..N) are generated but follow
	   this class' API.

	   All variants of this class except the nullary one throw if the argument
	   list does not have at least the required number of parameters.
	*/
        template <>
	struct MemFuncCaller<0>
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
           Useless base instantiation - See the 0-specialization for details.
        */
        template <int Arity>
	struct WeakMemFuncCaller
        {};
	/**
	   A helper type for forwarding function calls on objects bound via WeakJSClassCreator
	   through native member functions. This type supports only nullary functions, and
	   the generated classes WeakMemFuncCaller(1..N) each provide support for a different
	   number of arguments.
	*/
        template <>
	struct WeakMemFuncCaller<0> : MemFuncCaller<0>
	{
	    template <typename WeakWrappedType, typename RV>
	    static Handle<Value> CallOnWeakSelf( RV (WeakWrappedType::*func)(), Arguments const & argv )
	    {
		typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
		typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
		if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<0>::Call() could not find native 'this' object!"));
		return Call( obj, func, argv );
	    }

	    template <typename WeakWrappedType, typename RV>
	    static Handle<Value> CallOnWeakSelf( const RV (WeakWrappedType::*func)() const, Arguments const & argv )
	    {
		typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
		typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
		if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<0>::Call() could not find native 'this' object!"));
		return Call( obj, func, argv );
	    }

	    template <typename WeakWrappedType>
	    static Handle<Value> CallOnWeakSelf( void (WeakWrappedType::*func)(), Arguments const & argv )
	    {
		typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
		typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
		if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<0>::Call() could not find native 'this' object!"));
		return Call( obj, func, argv );
	    }

	    template <typename WeakWrappedType>
	    static Handle<Value> CallOnWeakSelf( const void (WeakWrappedType::*func)() const, Arguments const & argv )
	    {
		typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
		typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
		if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<0>::Call() could not find native 'this' object!"));
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
	    typedef WeakMemFuncCaller<0> OpBase;
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
	    typedef WeakMemFuncCaller<0> OpBase;
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
	    typedef WeakMemFuncCaller<0> OpBase;
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
	    typedef WeakMemFuncCaller<0> OpBase;
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
	typedef Detail::WeakMemFuncCaller<0> Caller;
	return Caller::CallOnWeakSelf<NativeType>( MemFunc, argv );
    }
    template <typename NativeType, typename RV>
    Handle<Value> CallWeakMemFunc( const RV (NativeType::*MemFunc)() const, Arguments const & argv )
    {
	typedef Detail::WeakMemFuncCaller<0> Caller;
	return Caller::CallOnWeakSelf<const NativeType>( MemFunc, argv );
    }
#endif

    /**
       InvocationCallbackMember is a helper type for binding InvocationCallback-like
       member functions. It requires that T be supported by CastFromJS(). The Func
       template parameter is the member invocation callback which we want to proxy.
    */
    template <typename T,
              ::v8::Handle< ::v8::Value > (T::*Func)( ::v8::Arguments const & argv )
              >
    struct InvocationCallbackMember
    {
	/**
           Extracts a native T object from argv using
           CastFromJS(argv.This()) and passes the call on to
           obj->Func(). If no object can be found it throws a JS
           exception, otherwise it returns the result of the proxied
           call.
	*/
	static ::v8::Handle< ::v8::Value > Call( ::v8::Arguments const & argv )
	{
            T * self = //WeakJSClassCreator<T>::GetSelf( argv.This() );
                convert::CastFromJS<T>( argv.This() );
            if( ! self ) return ThrowException(String::New("InvocationCallbackMember could not find native 'this' object in argv!"));
            return (self->*Func)( argv );
	}
    };


    /**
       This template can be used as an argument to
       v8::ObjectTemplate::SetAccessor()'s Getter parameter to
       generically tie a native member variable to a JS value.

       Requirements:

       - (WrappedType*) must be convertible via CastFromJS<WrappedType>().
       - PropertyType must be convertible via CastToJS<PropertyType>().

       If the underlying native This object cannot be found then this
       routine will trigger a JS exception.

       Example:

       Assume we have a native type Foo with a std::string member
       called str. We can bind the member variable with:

       \code
       myObjectTemplate.SetAccessor("foo",
               MemberVarGetter<Foo,std::string,&Foo::str>,
               MemberVarSetter<Foo,std::string,&Foo::str> );
       \endcode

       When using the JSClassWrapper interface, these can be passed to the
       Set() overload which takes accessor information. e.g.:

       \code
       wrapper.Set( "str",
                    MemberVarGetter<Foo,std::string, &Foo::str>,
                    MemberVarGStter<Foo,std::string, &Foo::str> );
       \endcode

       In almost 10 years of C++ coding, this is the first time i've ever had
       a use for a pointer-to-member.
    */
    template <typename WrappedType, typename PropertyType, PropertyType WrappedType::*MemVar>
    Handle<Value> MemberVarGetter(Local<String> property, const AccessorInfo &info)
    {
        WrappedType * self = convert::CastFromJS<WrappedType>( info.This() );
        if( ! self ) return v8::ThrowException( v8::String::New( "Native member property getter could not access native This object!" ) );
        return convert::CastToJS( (self->*MemVar) );
    }

    /**
       This is the Setter counterpart of MemberVarGetter(). See
       that function for most of the details.

       Requirements:

       - (WrappedType*) must be convertible via CastFromJS<WrappedType>().
       - PropertyType must be convertible via CastFromJS<PropertyType>().

       If the underlying native This object cannot be found then this
       routine will trigger a JS exception, though it is currently
       unclear whether this is actually legal in v8 (and difficult to
       test, since the native bindings work so well ;).
    */
    template <typename WrappedType, typename PropertyType, PropertyType WrappedType::*MemVar>
    void MemberVarSetter(Local<String> property, Local<Value> value, const AccessorInfo& info)
    {
        WrappedType * self = convert::CastFromJS<WrappedType>( info.This() );
        if( self )
        {
            self->*MemVar = convert::CastFromJS<PropertyType>( value );
        }
        else
        {
            // It is legal to do this from here?
            ::v8::ThrowException(v8::String::New("Native member property setter could not access native This object!"));
        }
    }

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

        /** Does nothing. Is virtual to satisfy inheritance rules and please my compiler. */
	virtual ~ClassBinder() {}

        template < Handle<Value> (WrappedType::*Func)( Arguments const & ) >
        ClassBinder & BindMemFunc( char const * name )
        {
            this->Set(name, InvocationCallbackMember<WrappedType,Func>::Call );
            return *this;
        }

        /**
           Binds automatically-generated getter/setter functions to the given
           member variable. See MemberVarGetter() and MemberVarSetter()
           for the requirements of the templatized types.

           If you only want to bind one of the getter OR the setter then
           use the 5-argument variant of Set() instead and pass MemberVarGetter
           or MemberVarGetter, as appropriate, to that function.
        */
        template <typename VarType, VarType WrappedType::*MemVar >
        ClassBinder & BindMemVar( char const * name,
                                  AccessControl settings = v8::PROHIBITS_OVERWRITING,
                                  PropertyAttribute attribute = v8::DontDelete
                                  )
        {
            this->Set( name,
                       MemberVarGetter<WrappedType,VarType,MemVar>,
                       MemberVarSetter<WrappedType,VarType,MemVar>,
                       v8::Handle< v8::Value >(), settings, attribute );
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


}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_JUICE_CLASSBINDER_H_INCLUDED */
