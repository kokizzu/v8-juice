#!/bin/bash
########################################################################
# Generates code for the function forwarding parts of the
# v8::juice::convert API (and friends).
count=${1-0}

test "$count" -gt 0 || {
    echo "Usage: $0 NUMBER (>=1) [COMMAND=MemFuncForwarder]"
    echo "Commands: MemFuncForwarder TMemFuncForwarder FunctorForwarder CtorForwarder"
    echo "Generates specializations for operations taking exactly NUMBER argumnents."
    exit 1
}

command=${2-MemFuncForwarder}

tmplsig="typename WrappedType, typename RV, "
ttlist="WrappedType, RV,"
tmplsigV="typename WrappedType, void, "
aTDecl="" # typename A0, typename A1,...
aTParam="" # A0, A1 ...
castCalls="" # CastFromJS<A0>(argv[0), ...
at=0
callop=MemFuncCallOp${count}
MemFuncForwarder=MemFuncForwarder
callBase="${MemFuncForwarder}<${count}>"
callBaseWeak="WeakMemFuncForwarder<${count}>"

########################################################
# must be called first to populate the shared vars
function makeLists()
{
    for (( at = 0; at < count; at = at + 1)); do
    #echo "at=$at"
	local AT="A${at}"
	tmplsig="${tmplsig} typename ${AT},"
	ttlist="${ttlist} ${AT},"
	tmplsigV="${tmplsigV} ${AT},"
	aTDecl="${aTDecl} typename ${AT}"
	aTParam="${aTParam} ${AT}"
	castCalls="${castCalls} CastFromJS< ${AT} >(argv[${at}])"
	test $at -ne $((count-1)) && {
	    aTDecl="${aTDecl}, "
	    aTParam="${aTParam},"
	    castCalls="${castCalls},"
	}
    done
    #tmplsig="${tmplsig} RV (WrappedType::*Func)(${aTParam})";
    funcSig="(WrappedType::*Func)(${aTParam}) "
    tmplsig="typename WrappedType, typename RV, ${aTDecl}, RV ${funcSig}";
    tmplsigV="typename WrappedType, ${aTDecl}, void ${funcSig}";
    tmplspecV="WrappedType, void, ${aTParam}, void ${funcSig}"
}


#######################################################
# Creates ${callBase} and ${callWeakBase} implementations.
function makeMemFuncForwarder()
{
    local err_native_is_null="${callBase}::Call(): Native object is null!"
    local err_too_few_args="${callBase}::Call(): wrong argument count!"
cat <<EOF
/**
A helper class for forwarding JS arguments to member functions
taking ${count} arguments.
*/
template <>
struct ${callBase}
{
    enum { Arity = ${count} };

    template <typename T, typename RV, ${aTDecl}>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)(${aTParam}), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("${err_native_is_null}"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("${err_too_few_args}"));
	return CastToJS<RV>( (obj->*MemFunc)( ${castCalls} ) );
    }
    template <typename T, typename RV, ${aTDecl}>
    static Handle<Value> Call( RV (T::*MemFunc)(${aTParam}), Arguments const & argv )
    {
        T * obj = CastFromJS<T>( argv.This() );
	if( ! obj ) return v8::ThrowException(v8::String::New("${err_native_is_null}"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("${err_too_few_args}"));
	return Call( obj, MemFunc, argv );
    }

    
    template <typename T, typename RV, ${aTDecl}>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("${err_native_is_null}"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("${err_too_few_args}"));
	return CastToJS<RV>( (obj->*MemFunc)(${castCalls} ) );
    }

    template <typename T, typename RV, ${aTDecl}>
    static Handle<Value> Call( RV (T::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
        T const * obj = CastFromJS<T>( argv.This() );
	if( ! obj ) return v8::ThrowException(v8::String::New("${err_native_is_null}"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("${err_too_few_args}"));
	return Call( obj, MemFunc, argv );
    }

    template <typename T, typename VoidType, ${aTDecl}>
    static Handle<Value> CallVoid( T * obj, VoidType (T::*MemFunc)(${aTParam}), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("${err_native_is_null}"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("${err_too_few_args}"));
	(obj->*MemFunc)(${castCalls} );
	return Undefined();
    }
    template <typename T, ${aTDecl}>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)(${aTParam}), Arguments const & argv )
    {
	return CallVoid<T,void,${aTParam}>( obj, MemFunc, argv );
    }

    template <typename T, typename VoidType, ${aTDecl}>
    static Handle<Value> CallVoid( VoidType (T::*MemFunc)(${aTParam}), Arguments const & argv )
    {
        T * obj = CastFromJS<T>( argv.This() );
	if( ! obj ) return v8::ThrowException(v8::String::New("${err_native_is_null}"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("${err_too_few_args}"));
	Call( obj, MemFunc, argv );
	return Undefined();
    }
    template <typename T, ${aTDecl}>
    static Handle<Value> Call( void (T::*MemFunc)(${aTParam}), Arguments const & argv )
    {
        return CallVoid<T,void,${aTParam}>( MemFunc, argv );
    }


    template <typename T, typename VoidType, ${aTDecl} >
    static Handle<Value> CallVoid( T const * obj, VoidType (T::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("${err_native_is_null}"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("${err_too_few_args}"));
	(obj->*MemFunc)(${castCalls} );
	return Undefined();
    }
    template <typename T, ${aTDecl} >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
        return CallVoid<T,void,${aTParam}>( obj, MemFunc, argv );
    }

    template <typename T, typename VoidType, ${aTDecl} >
    static Handle<Value> CallVoid( VoidType (T::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
        T const * obj = CastFromJS<T>( argv.This() );
	if( ! obj ) return v8::ThrowException(v8::String::New("${err_native_is_null}"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("${err_too_few_args}"));
	Call( obj, MemFunc, argv );
	return Undefined();
    }
    template <typename T, ${aTDecl} >
    static Handle<Value> Call( void (T::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
        return CallVoid<T,void,${aTParam}>( MemFunc, argv );
    }

    template <typename T, typename RV, ${aTDecl}, RV(T::*MemFunc)(${aTParam}) >
    static Handle<Value> Invocable( Arguments const & argv )
    {
        return Call<T,RV,${aTParam}>( MemFunc, argv );
    }

    template <typename T, typename RV, ${aTDecl}, RV(T::*MemFunc)(${aTParam}) const >
    static Handle<Value> Invocable( Arguments const & argv )
    {
        return Call<T,RV,${aTParam}>( MemFunc, argv );
    }

    template <typename T, typename VoidType, ${aTDecl}, VoidType (T::*MemFunc)(${aTParam}) >
    static Handle<Value> InvocableVoid( Arguments const & argv )
    {
        return CallVoid<T,VoidType,${aTParam}>( MemFunc, argv );
    }

    template <typename T, typename VoidType, ${aTDecl}, VoidType (T::*MemFunc)(${aTParam}) const >
    static Handle<Value> InvocableVoid( Arguments const & argv )
    {
        return CallVoid<T,VoidType,${aTParam}>( MemFunc, argv );
    }

};
EOF
} # makeMemFuncForwarder()

#######################################################
# Creates TMemFuncForwarder implementations.
function makeTMemFuncForwarder()
{
    local err_native_is_null="${callBase}::Call(): Native object is null!"
    local err_too_few_args="${callBase}::Call(): wrong argument count!"
cat <<EOF
/**
A helper class for forwarding JS arguments to member functions
taking ${count} arguments.
*/
template <typename T>
struct TMemFuncForwarder<T,${count}>
{
private:
    typedef MemFuncForwarder<${count}> Proxy;
public:
    enum { Arity = ${count} };
    typedef typename TypeInfo<T>::Type Type;

    template <typename RV, ${aTDecl}>
    static Handle<Value> Call( T * obj, RV (Type::*MemFunc)(${aTParam}), Arguments const & argv )
    {
        return Proxy::Call<Type,RV,${aTParam}>( obj, MemFunc, argv );
    }
    template <typename RV, ${aTDecl}>
    static Handle<Value> Call( RV (Type::*MemFunc)(${aTParam}), Arguments const & argv )
    {
        return Proxy::Call<Type,RV,${aTParam}>( MemFunc, argv );
    }

    template <typename RV, ${aTDecl}>
    static Handle<Value> Call( Type const * obj, RV (Type::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
        return Proxy::Call<Type,RV,${aTParam}>(obj,MemFunc,argv);
    }

    template <typename RV, ${aTDecl}>
    static Handle<Value> Call( RV (Type::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
        return Proxy::Call<Type,RV,${aTParam}>( MemFunc, argv );
    }

    template <typename VoidType, ${aTDecl}>
    static Handle<Value> CallVoid( Type * obj, VoidType (Type::*MemFunc)(${aTParam}), Arguments const & argv )
    {
        return Proxy::Call<Type,VoidType,${aTParam}>( obj, MemFunc , argv );
    }

    template <${aTDecl}>
    static Handle<Value> Call( Type * obj, void (Type::*MemFunc)(${aTParam}), Arguments const & argv )
    {
        return Proxy::Call<Type,void,${aTParam}>( obj, MemFunc, argv );
    }

    template <typename VoidType, ${aTDecl}>
    static Handle<Value> CallVoid( VoidType (Type::*MemFunc)(${aTParam}), Arguments const & argv )
    {
        return Proxy::CallVoid<Type,VoidType,${aTParam}>( MemFunc, argv );
    }

    template <${aTDecl}>
    static Handle<Value> Call( void (Type::*MemFunc)(${aTParam}), Arguments const & argv )
    {
        return Proxy::Call<${aTParam}>( MemFunc, argv );
    }

    template <typename VoidType, ${aTDecl} >
    static Handle<Value> CallVoid( Type const * obj, VoidType (Type::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
        return Proxy::CallVoid<Type,VoidType,${aTParam}>( obj, MemFunc, argv );
    }

    template <${aTDecl} >
    static Handle<Value> Call( Type const * obj, void (Type::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
        return Proxy::Call<Type,${aTParam}>( obj, MemFunc, argv );
    }

    template <typename VoidType, ${aTDecl} >
    static Handle<Value> CallVoid( VoidType (Type::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
        return Proxy::Call<Type,VoidType,${aTParam}>( MemFunc, argv );
    }

    template <${aTDecl} >
    static Handle<Value> Call( void (Type::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
        return Proxy::Call<Type,${aTParam}>( MemFunc, argv );
    }

    template <typename RV, ${aTDecl}, RV(Type::*MemFunc)(${aTParam}) >
    static Handle<Value> Invocable( Arguments const & argv )
    {
        return Proxy::Invocable<Type,RV,${aTParam}, MemFunc>( argv );
    }

    template <typename RV, ${aTDecl}, RV(Type::*MemFunc)(${aTParam}) const >
    static Handle<Value> Invocable( Arguments const & argv )
    {
        return Proxy::Invocable<Type,RV,${aTParam},MemFunc>( argv );
    }

    template <typename VoidType, ${aTDecl}, VoidType (Type::*MemFunc)(${aTParam}) >
    static Handle<Value> InvocableVoid( Arguments const & argv )
    {
        return Proxy::InvocableVoid<Type,VoidType,${aTParam},MemFunc>( argv );
    }

    template <typename VoidType, ${aTDecl}, VoidType (Type::*MemFunc)(${aTParam}) const >
    static Handle<Value> InvocableVoid( Arguments const & argv )
    {
        return Proxy::InvocableVoid<Type,VoidType,${aTParam},MemFunc>( argv );
    }

};
EOF
} # makeTMemFuncForwarder()


#######################################################
# Creates FunctorForwarder specializations and FwdToFunc()
# overloads.
function makeFunctorForwarder()
{
    cat <<EOF
/** Specialization for functor taking ${count} arguments. */
template <typename RV>
struct FunctorForwarder<${count},RV>
{
    enum { Arity = ${count} };
    template < ${aTDecl}, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<${count},RV>::Call() expects at least ${count} JS arguments!"));
        try
        {
            return CastToJS<RV>( f( ${castCalls} ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<${count},ReturnType>: Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }
};
/** Specialization for functor taking ${count} arguments and returning void. */
template <>
struct FunctorForwarder<${count},void>
{
    enum { Arity = ${count} };
    template < ${aTDecl}, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<${count},void>::Call() expects at least ${count} JS arguments!"));
        try
        {
            f( ${castCalls} );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<${count},void>: Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }
};

/**
   See the docs for FwdToFunc0(). This variant takes ${count} argument(s).
*/
template <typename ReturnT, ${aTDecl}, typename Func>
::v8::Handle< ::v8::Value > FwdToFunc${count}( Func func, ::v8::Arguments const & argv )
{
    return FunctorForwarder<${count},ReturnT>::template Call< ${aTParam} >( func, argv );
}
/**
   Functionally identical to FwdToFunc${count}(), but the templatized types can
   be deduced by the compiler.
*/
template <typename ReturnT, ${aTDecl}>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)(${aTParam}), ::v8::Arguments const & argv )
{
    return FunctorForwarder<${count},ReturnT>::template Call< ${aTParam} >( func, argv );
}
EOF
}

#######################################################
# Creates FunctionForwarder specializations.
function makeFunctionForwarder()
{

    local err_too_few_args="FunctionForwarder<${count}>::Call() expects at least ${count} JS arguments!"
    local err_exception="FunctionForwarder<${count}>::Call() Native function threw an unknown native exception type!"

    cat <<EOF
/** Specialization for functor taking ${count} arguments. */
template <>
struct FunctionForwarder<${count}>
{
    enum { Arity = ${count} };
    template < typename RV, ${aTDecl} >
    static v8::Handle<v8::Value> Call( RV (*Func)(${aTParam}), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("${err_too_few_args}"));
        try
        {
            return CastToJS<RV>( Func( ${castCalls} ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("${err_exception}"));
        }
        return Undefined(); // cannot be reached.
    }

    template < typename VoidType, ${aTDecl} >
    static v8::Handle<v8::Value> CallVoid( VoidType (*Func)(${aTParam}), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("${err_too_few_args}"));
        try
        {
            Func( ${castCalls} );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("${err_exception}"));
        }
        return Undefined();
    }
    template < typename VoidType, ${aTDecl} >
    static v8::Handle<v8::Value> Call( void (*Func)(${aTParam}), ::v8::Arguments const & argv )
    {
        return CallVoid<VoidType, ${aTParam}>( Func, argv );
    }

    template <typename RV, ${aTDecl}, RV (*Func)(${aTParam}) >
    static v8::Handle<v8::Value> Invocable( v8::Arguments const & argv )
    {
        //return Call<RV,${aTParam}, RV (*)(${aTParam}) >( Func, argv );
        return Call<RV,${aTParam} >( Func, argv );
    }

    template <typename VoidType, ${aTDecl}, VoidType (*Func)(${aTParam}) >
    static v8::Handle<v8::Value> InvocableVoid( v8::Arguments const & argv )
    {
        //return CallVoid<VoidType, ${aTParam}, VoidType (*)(${aTParam}) >( Func, argv );
        return CallVoid<VoidType, ${aTParam} >( Func, argv );
    }

};

EOF
} # makeFunctionForwarder


#######################################################
# Creates CtorForwarder specializations.
function makeCtorForwarder()
{

    local err_too_few_args="CtorForwarder<T,${count}>::Ctor() expects at least ${count} JS arguments!"
    local err_exception="CtorForwarder<T,${count}>::Ctor() Native ctor threw an unknown native exception type!"

    cat <<EOF
/** Specialization ${count} arguments. */
template <typename T>
struct CtorForwarder<T,${count}>
{
    enum { Arity = ${count} };
    typedef typename TypeInfo<T>::Type Type;
    typedef typename TypeInfo<T>::NativeHandle NativeHandle;
    template < ${aTDecl} >
    static NativeHandle Ctor( ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity )
        {
            throw std::range_error("${err_too_few_args}");
        }
        else
        {
            return new Type( ${castCalls} );
        }
    }

};

EOF
} # makeCtorForwarder


##########################################################
# here we go...
makeLists
false && {
    echo funcSig=${funcSig}
    echo tmplsig=${tmplsig}
    echo tmplsigV=${tmplsigV}
    echo tmplspecV=${tmplspecV}
}


case $command in
    'MemFuncForwarder')
	makeMemFuncForwarder
	;;
    'TMemFuncForwarder')
	makeTMemFuncForwarder
	;;
    'FunctorForwarder')
	makeFunctorForwarder
	;;
    'FunctionForwarder')
        makeFunctionForwarder
        ;;
    'CtorForwarder')
	makeCtorForwarder
	;;
    *)
	echo "Unhandled command: ${command}"
	exit 2
    ;;
esac
