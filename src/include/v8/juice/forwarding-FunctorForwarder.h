// GENERATED CODE! Edit at your own risk!
#if !defined(DOXYGEN)
/** Specialization for functor taking 1 arguments. */
template <typename RV>
struct FunctorForwarder<1,RV>
{
    enum { Arity = 1 };
    template <  typename A0, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<1,RV>::Call() expects at least 1 JS arguments!"));
        try
        {
            return convert::CastToJS<RV>( f(  convert::CastFromJS< A0 >(argv[0]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<1,ReturnType>: Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }
};
/** Specialization for functor taking 1 arguments and returning void. */
template <>
struct FunctorForwarder<1,void>
{
    enum { Arity = 1 };
    template <  typename A0, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<1,void>::Call() expects at least 1 JS arguments!"));
        try
        {
            f(  convert::CastFromJS< A0 >(argv[0]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<1,void>: Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }
};

/**
   See the docs for FwdToFunc0(). This variant takes 1 argument(s).
*/
template <typename ReturnT,  typename A0, typename Func>
::v8::Handle< ::v8::Value > FwdToFunc1( Func func, ::v8::Arguments const & argv )
{
    return FunctorForwarder<1,ReturnT>::template Call<  A0 >( func, argv );
}
/**
   Functionally identical to FwdToFunc1(), but the return type can
   be deduced by the compiler.
*/
template <typename ReturnT,  typename A0>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)( A0), ::v8::Arguments const & argv )
{
    return FunctorForwarder<1,ReturnT>::template Call<  A0 >( func, argv );
}
/** Specialization for functor taking 2 arguments. */
template <typename RV>
struct FunctorForwarder<2,RV>
{
    enum { Arity = 2 };
    template <  typename A0,  typename A1, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<2,RV>::Call() expects at least 2 JS arguments!"));
        try
        {
            return convert::CastToJS<RV>( f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<2,ReturnType>: Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }
};
/** Specialization for functor taking 2 arguments and returning void. */
template <>
struct FunctorForwarder<2,void>
{
    enum { Arity = 2 };
    template <  typename A0,  typename A1, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<2,void>::Call() expects at least 2 JS arguments!"));
        try
        {
            f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<2,void>: Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }
};

/**
   See the docs for FwdToFunc0(). This variant takes 2 argument(s).
*/
template <typename ReturnT,  typename A0,  typename A1, typename Func>
::v8::Handle< ::v8::Value > FwdToFunc2( Func func, ::v8::Arguments const & argv )
{
    return FunctorForwarder<2,ReturnT>::template Call<  A0, A1 >( func, argv );
}
/**
   Functionally identical to FwdToFunc2(), but the return type can
   be deduced by the compiler.
*/
template <typename ReturnT,  typename A0,  typename A1>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)( A0, A1), ::v8::Arguments const & argv )
{
    return FunctorForwarder<2,ReturnT>::template Call<  A0, A1 >( func, argv );
}
/** Specialization for functor taking 3 arguments. */
template <typename RV>
struct FunctorForwarder<3,RV>
{
    enum { Arity = 3 };
    template <  typename A0,  typename A1,  typename A2, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<3,RV>::Call() expects at least 3 JS arguments!"));
        try
        {
            return convert::CastToJS<RV>( f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<3,ReturnType>: Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }
};
/** Specialization for functor taking 3 arguments and returning void. */
template <>
struct FunctorForwarder<3,void>
{
    enum { Arity = 3 };
    template <  typename A0,  typename A1,  typename A2, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<3,void>::Call() expects at least 3 JS arguments!"));
        try
        {
            f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<3,void>: Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }
};

/**
   See the docs for FwdToFunc0(). This variant takes 3 argument(s).
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2, typename Func>
::v8::Handle< ::v8::Value > FwdToFunc3( Func func, ::v8::Arguments const & argv )
{
    return FunctorForwarder<3,ReturnT>::template Call<  A0, A1, A2 >( func, argv );
}
/**
   Functionally identical to FwdToFunc3(), but the return type can
   be deduced by the compiler.
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)( A0, A1, A2), ::v8::Arguments const & argv )
{
    return FunctorForwarder<3,ReturnT>::template Call<  A0, A1, A2 >( func, argv );
}
/** Specialization for functor taking 4 arguments. */
template <typename RV>
struct FunctorForwarder<4,RV>
{
    enum { Arity = 4 };
    template <  typename A0,  typename A1,  typename A2,  typename A3, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<4,RV>::Call() expects at least 4 JS arguments!"));
        try
        {
            return convert::CastToJS<RV>( f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<4,ReturnType>: Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }
};
/** Specialization for functor taking 4 arguments and returning void. */
template <>
struct FunctorForwarder<4,void>
{
    enum { Arity = 4 };
    template <  typename A0,  typename A1,  typename A2,  typename A3, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<4,void>::Call() expects at least 4 JS arguments!"));
        try
        {
            f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<4,void>: Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }
};

/**
   See the docs for FwdToFunc0(). This variant takes 4 argument(s).
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3, typename Func>
::v8::Handle< ::v8::Value > FwdToFunc4( Func func, ::v8::Arguments const & argv )
{
    return FunctorForwarder<4,ReturnT>::template Call<  A0, A1, A2, A3 >( func, argv );
}
/**
   Functionally identical to FwdToFunc4(), but the return type can
   be deduced by the compiler.
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)( A0, A1, A2, A3), ::v8::Arguments const & argv )
{
    return FunctorForwarder<4,ReturnT>::template Call<  A0, A1, A2, A3 >( func, argv );
}
/** Specialization for functor taking 5 arguments. */
template <typename RV>
struct FunctorForwarder<5,RV>
{
    enum { Arity = 5 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<5,RV>::Call() expects at least 5 JS arguments!"));
        try
        {
            return convert::CastToJS<RV>( f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<5,ReturnType>: Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }
};
/** Specialization for functor taking 5 arguments and returning void. */
template <>
struct FunctorForwarder<5,void>
{
    enum { Arity = 5 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<5,void>::Call() expects at least 5 JS arguments!"));
        try
        {
            f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<5,void>: Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }
};

/**
   See the docs for FwdToFunc0(). This variant takes 5 argument(s).
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4, typename Func>
::v8::Handle< ::v8::Value > FwdToFunc5( Func func, ::v8::Arguments const & argv )
{
    return FunctorForwarder<5,ReturnT>::template Call<  A0, A1, A2, A3, A4 >( func, argv );
}
/**
   Functionally identical to FwdToFunc5(), but the return type can
   be deduced by the compiler.
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)( A0, A1, A2, A3, A4), ::v8::Arguments const & argv )
{
    return FunctorForwarder<5,ReturnT>::template Call<  A0, A1, A2, A3, A4 >( func, argv );
}
/** Specialization for functor taking 6 arguments. */
template <typename RV>
struct FunctorForwarder<6,RV>
{
    enum { Arity = 6 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<6,RV>::Call() expects at least 6 JS arguments!"));
        try
        {
            return convert::CastToJS<RV>( f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<6,ReturnType>: Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }
};
/** Specialization for functor taking 6 arguments and returning void. */
template <>
struct FunctorForwarder<6,void>
{
    enum { Arity = 6 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<6,void>::Call() expects at least 6 JS arguments!"));
        try
        {
            f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<6,void>: Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }
};

/**
   See the docs for FwdToFunc0(). This variant takes 6 argument(s).
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5, typename Func>
::v8::Handle< ::v8::Value > FwdToFunc6( Func func, ::v8::Arguments const & argv )
{
    return FunctorForwarder<6,ReturnT>::template Call<  A0, A1, A2, A3, A4, A5 >( func, argv );
}
/**
   Functionally identical to FwdToFunc6(), but the return type can
   be deduced by the compiler.
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)( A0, A1, A2, A3, A4, A5), ::v8::Arguments const & argv )
{
    return FunctorForwarder<6,ReturnT>::template Call<  A0, A1, A2, A3, A4, A5 >( func, argv );
}
/** Specialization for functor taking 7 arguments. */
template <typename RV>
struct FunctorForwarder<7,RV>
{
    enum { Arity = 7 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<7,RV>::Call() expects at least 7 JS arguments!"));
        try
        {
            return convert::CastToJS<RV>( f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<7,ReturnType>: Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }
};
/** Specialization for functor taking 7 arguments and returning void. */
template <>
struct FunctorForwarder<7,void>
{
    enum { Arity = 7 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<7,void>::Call() expects at least 7 JS arguments!"));
        try
        {
            f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<7,void>: Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }
};

/**
   See the docs for FwdToFunc0(). This variant takes 7 argument(s).
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6, typename Func>
::v8::Handle< ::v8::Value > FwdToFunc7( Func func, ::v8::Arguments const & argv )
{
    return FunctorForwarder<7,ReturnT>::template Call<  A0, A1, A2, A3, A4, A5, A6 >( func, argv );
}
/**
   Functionally identical to FwdToFunc7(), but the return type can
   be deduced by the compiler.
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)( A0, A1, A2, A3, A4, A5, A6), ::v8::Arguments const & argv )
{
    return FunctorForwarder<7,ReturnT>::template Call<  A0, A1, A2, A3, A4, A5, A6 >( func, argv );
}
/** Specialization for functor taking 8 arguments. */
template <typename RV>
struct FunctorForwarder<8,RV>
{
    enum { Arity = 8 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<8,RV>::Call() expects at least 8 JS arguments!"));
        try
        {
            return convert::CastToJS<RV>( f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]), convert::CastFromJS< A7 >(argv[7]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<8,ReturnType>: Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }
};
/** Specialization for functor taking 8 arguments and returning void. */
template <>
struct FunctorForwarder<8,void>
{
    enum { Arity = 8 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<8,void>::Call() expects at least 8 JS arguments!"));
        try
        {
            f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]), convert::CastFromJS< A7 >(argv[7]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<8,void>: Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }
};

/**
   See the docs for FwdToFunc0(). This variant takes 8 argument(s).
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7, typename Func>
::v8::Handle< ::v8::Value > FwdToFunc8( Func func, ::v8::Arguments const & argv )
{
    return FunctorForwarder<8,ReturnT>::template Call<  A0, A1, A2, A3, A4, A5, A6, A7 >( func, argv );
}
/**
   Functionally identical to FwdToFunc8(), but the return type can
   be deduced by the compiler.
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)( A0, A1, A2, A3, A4, A5, A6, A7), ::v8::Arguments const & argv )
{
    return FunctorForwarder<8,ReturnT>::template Call<  A0, A1, A2, A3, A4, A5, A6, A7 >( func, argv );
}
/** Specialization for functor taking 9 arguments. */
template <typename RV>
struct FunctorForwarder<9,RV>
{
    enum { Arity = 9 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<9,RV>::Call() expects at least 9 JS arguments!"));
        try
        {
            return convert::CastToJS<RV>( f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]), convert::CastFromJS< A7 >(argv[7]), convert::CastFromJS< A8 >(argv[8]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<9,ReturnType>: Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }
};
/** Specialization for functor taking 9 arguments and returning void. */
template <>
struct FunctorForwarder<9,void>
{
    enum { Arity = 9 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<9,void>::Call() expects at least 9 JS arguments!"));
        try
        {
            f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]), convert::CastFromJS< A7 >(argv[7]), convert::CastFromJS< A8 >(argv[8]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<9,void>: Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }
};

/**
   See the docs for FwdToFunc0(). This variant takes 9 argument(s).
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8, typename Func>
::v8::Handle< ::v8::Value > FwdToFunc9( Func func, ::v8::Arguments const & argv )
{
    return FunctorForwarder<9,ReturnT>::template Call<  A0, A1, A2, A3, A4, A5, A6, A7, A8 >( func, argv );
}
/**
   Functionally identical to FwdToFunc9(), but the return type can
   be deduced by the compiler.
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)( A0, A1, A2, A3, A4, A5, A6, A7, A8), ::v8::Arguments const & argv )
{
    return FunctorForwarder<9,ReturnT>::template Call<  A0, A1, A2, A3, A4, A5, A6, A7, A8 >( func, argv );
}
/** Specialization for functor taking 10 arguments. */
template <typename RV>
struct FunctorForwarder<10,RV>
{
    enum { Arity = 10 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<10,RV>::Call() expects at least 10 JS arguments!"));
        try
        {
            return convert::CastToJS<RV>( f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]), convert::CastFromJS< A7 >(argv[7]), convert::CastFromJS< A8 >(argv[8]), convert::CastFromJS< A9 >(argv[9]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<10,ReturnType>: Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }
};
/** Specialization for functor taking 10 arguments and returning void. */
template <>
struct FunctorForwarder<10,void>
{
    enum { Arity = 10 };
    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9, typename Func >
    static Handle<Value> Call( Func f, ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctorForwarder<10,void>::Call() expects at least 10 JS arguments!"));
        try
        {
            f(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]), convert::CastFromJS< A7 >(argv[7]), convert::CastFromJS< A8 >(argv[8]), convert::CastFromJS< A9 >(argv[9]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctorForwarder<10,void>: Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }
};

/**
   See the docs for FwdToFunc0(). This variant takes 10 argument(s).
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9, typename Func>
::v8::Handle< ::v8::Value > FwdToFunc10( Func func, ::v8::Arguments const & argv )
{
    return FunctorForwarder<10,ReturnT>::template Call<  A0, A1, A2, A3, A4, A5, A6, A7, A8, A9 >( func, argv );
}
/**
   Functionally identical to FwdToFunc10(), but the return type can
   be deduced by the compiler.
*/
template <typename ReturnT,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9>
::v8::Handle< ::v8::Value > FwdToFunc( ReturnT (*func)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9), ::v8::Arguments const & argv )
{
    return FunctorForwarder<10,ReturnT>::template Call<  A0, A1, A2, A3, A4, A5, A6, A7, A8, A9 >( func, argv );
}
#endif // if !defined(DOXYGEN)
