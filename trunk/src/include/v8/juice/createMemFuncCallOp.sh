#!/bin/bash
########################################################################
# a very, very ugly hack to generate code for the ClassBinder API.
# It generates MemFuncCallOp${1} and friends.
count=${1-0}

test "$count" -gt 0 || {
    echo "Usage: $0 NUMBER (>=0) [COMMAND=BindMemFunc]"
    echo "Commands: MemFuncCallOps, BindMemFunc, MemFuncCaller"
    exit 1
}

command=${2-BindMemFunc}

tmplsig="typename WrappedType, typename RV, "
ttlist="WrappedType, RV,"
tmplsigV="typename WrappedType, void, "
aTDecl=""
aTParam=""
castCalls=""
at=0
callop=MemFuncCallOp${count}
MemFuncCaller=MemFuncCaller
callBase="${MemFuncCaller}<${count}>"
callBaseWeak="WeakMemFuncCaller<${count}>"

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
	castCalls="${castCalls} convert::CastFromJS< ${AT} >(argv[${at}])"
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


function makeCallBase()
{
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
	if( ! obj ) return ThrowException(String::New("${callBase}::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("${callBase}::Call(): wrong argument count!"));
	return convert::CastToJS<RV>( (obj->*MemFunc)( ${castCalls} ) );
    }
    
    template <typename T, typename RV, ${aTDecl}>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("${callBase}::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("${callBase}::Call(): wrong argument count!"));
	return convert::CastToJS<RV>( (obj->*MemFunc)(${castCalls} ) );
    }
    
    template <typename T, ${aTDecl}>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)(${aTParam}), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("${callBase}::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("${callBase}::Call(): wrong argument count!"));
	(obj->*MemFunc)(${castCalls} );
	return Undefined();
    }

    template <typename T, ${aTDecl} >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)(${aTParam}) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("${callBase}::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("${callBase}::Call(): wrong argument count!"));
	(obj->*MemFunc)(${castCalls} );
	return Undefined();
    }

    template <typename RV, ${aTDecl}, typename Func>
    static Handle<Value> Call( Func, Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ThrowException(String::New("${callBase}::Call(): wrong argument count!"));
	return convert::CastToJS<RV>( Func( ${castCalls} ) );
    }

    template <${aTDecl}, typename Func>
    static Handle<Value> CallVoid( Func, Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ThrowException(String::New("${callBase}::Call(): wrong argument count!"));
	Func( ${castCalls} );
        return Undefined();
    }

};
template <>
struct ${callBaseWeak} : ${callBase}
{
    enum { Arity = ${count} };

    template <typename WeakWrappedType, typename RV, ${aTDecl} >
    static Handle<Value> CallOnWeakSelf( RV (WeakWrappedType::*func)(${aTParam}), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("${callBaseWeak}::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType, typename RV, ${aTDecl} >
    static Handle<Value> CallOnWeakSelf( const RV (WeakWrappedType::*func)(${aTParam}) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("${callBaseWeak}::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType, ${aTDecl} >
    static Handle<Value> CallOnWeakSelf( void (WeakWrappedType::*func)(${aTParam}), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("${callBaseWeak}::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType, ${aTDecl} >
    static Handle<Value> CallOnWeakSelf( const void (WeakWrappedType::*func)(${aTParam}) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("${callBaseWeak}::Call() could not find native 'this' object!"));
        return Call( obj, func, argv );
    }

};
EOF
} # makeCallBase()


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
            return convert::CastToJS<RV>( f( ${castCalls} ) );
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
   Functionally identical to FwdToFunc${count}(), but the return type can
   be deduced by the compiler.
*/
template <typename ReturnT, ${aTDecl}>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)(${aTParam}), ::v8::Arguments const & argv )
{
    return FunctorForwarder<${count},ReturnT>::template Call< ${aTParam} >( func, argv );
}
EOF
}

function makeMemFuncCallOps()
{
    cat <<EOF
/**
    Member function call forwarder for member functions taking ${count} arguments.
*/
template < ${tmplsig} >
struct ${callop}
{
    enum { Arity = $count };
    typedef WrappedType Type;
    typedef ${callBaseWeak} OpBase;
    static Handle<Value> Call( Type * obj, Arguments const & argv )
    {
	return OpBase::Call( obj, Func, argv );
    }
    static Handle<Value> CallOnWeakSelf( Arguments const & argv )
    {
	return OpBase::CallOnWeakSelf<Type>( Func, argv );
    }
};

/**
    Specialization for member functions taking ${count} args and returning void.
*/
template < ${tmplsigV} >
struct ${callop}< WrappedType,void,${aTParam}, Func >
{
    enum { Arity = $count };
    typedef WrappedType Type;
    typedef ${callBaseWeak} OpBase;
    static Handle<Value> Call( Type * obj, Arguments const & argv )
    {
	return OpBase::Call( obj, Func, argv );
    }
    static Handle<Value> CallOnWeakSelf( Arguments const & argv )
    {
	return OpBase::CallOnWeakSelf<Type>( Func, argv );
    }
};
EOF
echo
return
}

function makeMemFuncCallOpsConst()
{
    cat <<EOF
/**
    Member function call forwarder for const member functions taking ${count} arguments
    and returning RV.
*/
template < typename WrappedType, typename RV, ${aTDecl}, RV ${funcSig} const >
struct ${callop}< const WrappedType, RV,${aTParam}, Func>
{
    enum { Arity = $count };
    typedef WrappedType Type;
    typedef ${callBaseWeak} OpBase;
    static Handle<Value> Call( Type const * obj, Arguments const & argv )
    {
	return OpBase::Call( obj, Func, argv );
    }
    static Handle<Value> CallOnWeakSelf( Arguments const & argv )
    {
	return OpBase::CallOnWeakSelf<const Type>( Func, argv );
    }
};

/**
    Specialization for const member functions taking ${count} args and returning void.
*/
template < typename WrappedType, ${aTDecl}, void ${funcSig} const >
struct ${callop}< const WrappedType,void,${aTParam}, Func >
{
    enum { Arity = $count };
    typedef WrappedType Type;
    static Handle<Value> Call( Type const * obj, Arguments const & argv )
    {
	return OpBase::Call( obj, Func, argv );
    }
    static Handle<Value> CallOnWeakSelf( Arguments const & argv )
    {
	return OpBase::CallOnWeakSelf<const Type>( Func, argv );
    }
};
EOF
}

function makeBindFunc()
{
cat <<EOF
/**
   Overload requiring a WrappedType CONSTNESS member function
   taking ${count} args and returning RV.
*/
template < typename RV, ${aTDecl}, RV ${funcSig} CONSTNESS >
ClassBinder & BindMemFunc( char const * name )
{
    typedef Detail::${callop}< CONSTNESS WrappedType, RV, ${aTParam}, Func > Caller;
    this->Set(name, Detail::MemFuncCallOp< Caller >::Call );
    return *this;
}
EOF

}

makeLists
false && {
    echo funcSig=${funcSig}
    echo tmplsig=${tmplsig}
    echo tmplsigV=${tmplsigV}
    echo tmplspecV=${tmplspecV}
}


case $command in
    'BindMemFunc')
        makeBindFunc | sed -e 's/CONSTNESS //g'
	makeBindFunc | sed -e 's/CONSTNESS /const /g'
	;;
    'MemFuncCallOps')
	    makeMemFuncCallOps
	    #makeMemFuncCallOpsConst
	    ;;
    'MemFuncCaller')
	makeCallBase
	;;
    'FunctorForwarder')
	makeFunctorForwarder
	;;
    *)
	echo "Unhandled command: ${command}"
	exit 2
    ;;
esac
