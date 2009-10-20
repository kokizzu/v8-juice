// GENERATED CODE! Edit at your own risk!
#if !defined(DOXYGEN)
/** Specialization for functor taking 1 arguments. */
template <>
struct FunctionForwarder<1>
{
    enum { Arity = 1 };
    template < typename RV,  typename A0, typename Func >
    static v8::Handle<v8::Value> Call( RV (*Func)( A0), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<1>::Call() expects at least 1 JS arguments!"));
        try
        {
            return CastToJS<RV>( Func(  CastFromJS< A0 >(argv[0]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<1>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }

    template <  typename A0 >
    static v8::Handle<v8::Value> CallVoid( void (*Func)( A0), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<1>::Call() expects at least 1 JS arguments!"));
        try
        {
            Func(  CastFromJS< A0 >(argv[0]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<1>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }

    template <typename RV,  typename A0, RV (*Func)( A0) >
    static v8::Handle<v8::Value> Invoke( v8::Arguments const & argv )
    {
        return Call( Func, argv );
    }

    template < typename A0, void (*Func)( A0) >
    static v8::Handle<v8::Value> InvokeVoid( v8::Arguments const & argv )
    {
        return CallVoid< A0>( Func, argv );
    }

};

/** Specialization for functor taking 2 arguments. */
template <>
struct FunctionForwarder<2>
{
    enum { Arity = 2 };
    template < typename RV,  typename A0,  typename A1, typename Func >
    static v8::Handle<v8::Value> Call( RV (*Func)( A0, A1), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<2>::Call() expects at least 2 JS arguments!"));
        try
        {
            return CastToJS<RV>( Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<2>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }

    template <  typename A0,  typename A1 >
    static v8::Handle<v8::Value> CallVoid( void (*Func)( A0, A1), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<2>::Call() expects at least 2 JS arguments!"));
        try
        {
            Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<2>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }

    template <typename RV,  typename A0,  typename A1, RV (*Func)( A0, A1) >
    static v8::Handle<v8::Value> Invoke( v8::Arguments const & argv )
    {
        return Call( Func, argv );
    }

    template < typename A0,  typename A1, void (*Func)( A0, A1) >
    static v8::Handle<v8::Value> InvokeVoid( v8::Arguments const & argv )
    {
        return CallVoid< A0, A1>( Func, argv );
    }

};

/** Specialization for functor taking 3 arguments. */
template <>
struct FunctionForwarder<3>
{
    enum { Arity = 3 };
    template < typename RV,  typename A0,  typename A1,  typename A2, typename Func >
    static v8::Handle<v8::Value> Call( RV (*Func)( A0, A1, A2), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<3>::Call() expects at least 3 JS arguments!"));
        try
        {
            return CastToJS<RV>( Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<3>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }

    template <  typename A0,  typename A1,  typename A2 >
    static v8::Handle<v8::Value> CallVoid( void (*Func)( A0, A1, A2), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<3>::Call() expects at least 3 JS arguments!"));
        try
        {
            Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<3>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }

    template <typename RV,  typename A0,  typename A1,  typename A2, RV (*Func)( A0, A1, A2) >
    static v8::Handle<v8::Value> Invoke( v8::Arguments const & argv )
    {
        return Call( Func, argv );
    }

    template < typename A0,  typename A1,  typename A2, void (*Func)( A0, A1, A2) >
    static v8::Handle<v8::Value> InvokeVoid( v8::Arguments const & argv )
    {
        return CallVoid< A0, A1, A2>( Func, argv );
    }

};

/** Specialization for functor taking 4 arguments. */
template <>
struct FunctionForwarder<4>
{
    enum { Arity = 4 };
    template < typename RV,  typename A0,  typename A1,  typename A2,  typename A3, typename Func >
    static v8::Handle<v8::Value> Call( RV (*Func)( A0, A1, A2, A3), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<4>::Call() expects at least 4 JS arguments!"));
        try
        {
            return CastToJS<RV>( Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<4>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }

    template <  typename A0,  typename A1,  typename A2,  typename A3 >
    static v8::Handle<v8::Value> CallVoid( void (*Func)( A0, A1, A2, A3), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<4>::Call() expects at least 4 JS arguments!"));
        try
        {
            Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<4>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }

    template <typename RV,  typename A0,  typename A1,  typename A2,  typename A3, RV (*Func)( A0, A1, A2, A3) >
    static v8::Handle<v8::Value> Invoke( v8::Arguments const & argv )
    {
        return Call( Func, argv );
    }

    template < typename A0,  typename A1,  typename A2,  typename A3, void (*Func)( A0, A1, A2, A3) >
    static v8::Handle<v8::Value> InvokeVoid( v8::Arguments const & argv )
    {
        return CallVoid< A0, A1, A2, A3>( Func, argv );
    }

};

/** Specialization for functor taking 5 arguments. */
template <>
struct FunctionForwarder<5>
{
    enum { Arity = 5 };
    template < typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4, typename Func >
    static v8::Handle<v8::Value> Call( RV (*Func)( A0, A1, A2, A3, A4), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<5>::Call() expects at least 5 JS arguments!"));
        try
        {
            return CastToJS<RV>( Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<5>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }

    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4 >
    static v8::Handle<v8::Value> CallVoid( void (*Func)( A0, A1, A2, A3, A4), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<5>::Call() expects at least 5 JS arguments!"));
        try
        {
            Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<5>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }

    template <typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4, RV (*Func)( A0, A1, A2, A3, A4) >
    static v8::Handle<v8::Value> Invoke( v8::Arguments const & argv )
    {
        return Call( Func, argv );
    }

    template < typename A0,  typename A1,  typename A2,  typename A3,  typename A4, void (*Func)( A0, A1, A2, A3, A4) >
    static v8::Handle<v8::Value> InvokeVoid( v8::Arguments const & argv )
    {
        return CallVoid< A0, A1, A2, A3, A4>( Func, argv );
    }

};

/** Specialization for functor taking 6 arguments. */
template <>
struct FunctionForwarder<6>
{
    enum { Arity = 6 };
    template < typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5, typename Func >
    static v8::Handle<v8::Value> Call( RV (*Func)( A0, A1, A2, A3, A4, A5), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<6>::Call() expects at least 6 JS arguments!"));
        try
        {
            return CastToJS<RV>( Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<6>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }

    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5 >
    static v8::Handle<v8::Value> CallVoid( void (*Func)( A0, A1, A2, A3, A4, A5), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<6>::Call() expects at least 6 JS arguments!"));
        try
        {
            Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<6>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }

    template <typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5, RV (*Func)( A0, A1, A2, A3, A4, A5) >
    static v8::Handle<v8::Value> Invoke( v8::Arguments const & argv )
    {
        return Call( Func, argv );
    }

    template < typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5, void (*Func)( A0, A1, A2, A3, A4, A5) >
    static v8::Handle<v8::Value> InvokeVoid( v8::Arguments const & argv )
    {
        return CallVoid< A0, A1, A2, A3, A4, A5>( Func, argv );
    }

};

/** Specialization for functor taking 7 arguments. */
template <>
struct FunctionForwarder<7>
{
    enum { Arity = 7 };
    template < typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6, typename Func >
    static v8::Handle<v8::Value> Call( RV (*Func)( A0, A1, A2, A3, A4, A5, A6), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<7>::Call() expects at least 7 JS arguments!"));
        try
        {
            return CastToJS<RV>( Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<7>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }

    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6 >
    static v8::Handle<v8::Value> CallVoid( void (*Func)( A0, A1, A2, A3, A4, A5, A6), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<7>::Call() expects at least 7 JS arguments!"));
        try
        {
            Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<7>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }

    template <typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6, RV (*Func)( A0, A1, A2, A3, A4, A5, A6) >
    static v8::Handle<v8::Value> Invoke( v8::Arguments const & argv )
    {
        return Call( Func, argv );
    }

    template < typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6, void (*Func)( A0, A1, A2, A3, A4, A5, A6) >
    static v8::Handle<v8::Value> InvokeVoid( v8::Arguments const & argv )
    {
        return CallVoid< A0, A1, A2, A3, A4, A5, A6>( Func, argv );
    }

};

/** Specialization for functor taking 8 arguments. */
template <>
struct FunctionForwarder<8>
{
    enum { Arity = 8 };
    template < typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7, typename Func >
    static v8::Handle<v8::Value> Call( RV (*Func)( A0, A1, A2, A3, A4, A5, A6, A7), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<8>::Call() expects at least 8 JS arguments!"));
        try
        {
            return CastToJS<RV>( Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<8>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }

    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7 >
    static v8::Handle<v8::Value> CallVoid( void (*Func)( A0, A1, A2, A3, A4, A5, A6, A7), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<8>::Call() expects at least 8 JS arguments!"));
        try
        {
            Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<8>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }

    template <typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7, RV (*Func)( A0, A1, A2, A3, A4, A5, A6, A7) >
    static v8::Handle<v8::Value> Invoke( v8::Arguments const & argv )
    {
        return Call( Func, argv );
    }

    template < typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7, void (*Func)( A0, A1, A2, A3, A4, A5, A6, A7) >
    static v8::Handle<v8::Value> InvokeVoid( v8::Arguments const & argv )
    {
        return CallVoid< A0, A1, A2, A3, A4, A5, A6, A7>( Func, argv );
    }

};

/** Specialization for functor taking 9 arguments. */
template <>
struct FunctionForwarder<9>
{
    enum { Arity = 9 };
    template < typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8, typename Func >
    static v8::Handle<v8::Value> Call( RV (*Func)( A0, A1, A2, A3, A4, A5, A6, A7, A8), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<9>::Call() expects at least 9 JS arguments!"));
        try
        {
            return CastToJS<RV>( Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<9>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }

    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8 >
    static v8::Handle<v8::Value> CallVoid( void (*Func)( A0, A1, A2, A3, A4, A5, A6, A7, A8), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<9>::Call() expects at least 9 JS arguments!"));
        try
        {
            Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<9>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }

    template <typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8, RV (*Func)( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
    static v8::Handle<v8::Value> Invoke( v8::Arguments const & argv )
    {
        return Call( Func, argv );
    }

    template < typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8, void (*Func)( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
    static v8::Handle<v8::Value> InvokeVoid( v8::Arguments const & argv )
    {
        return CallVoid< A0, A1, A2, A3, A4, A5, A6, A7, A8>( Func, argv );
    }

};

/** Specialization for functor taking 10 arguments. */
template <>
struct FunctionForwarder<10>
{
    enum { Arity = 10 };
    template < typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9, typename Func >
    static v8::Handle<v8::Value> Call( RV (*Func)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<10>::Call() expects at least 10 JS arguments!"));
        try
        {
            return CastToJS<RV>( Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]), CastFromJS< A9 >(argv[9]) ) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<10>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined(); // cannot be reached.
    }

    template <  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9 >
    static v8::Handle<v8::Value> CallVoid( void (*Func)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9), ::v8::Arguments const & argv )
    {
	if( argv.Length() < Arity ) return ::v8::ThrowException(::v8::String::New("FunctionForwarder<10>::Call() expects at least 10 JS arguments!"));
        try
        {
            Func(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]), CastFromJS< A9 >(argv[9]) );
        }
        catch( std::exception const & ex )
        {
            return ::v8::ThrowException( ::v8::String::New(ex.what()) );
        }
        catch( ... )
        {
            return ::v8::ThrowException( ::v8::String::New("FunctionForwarder<10>::Call() Native function threw an unknown native exception type!"));
        }
        return Undefined();
    }

    template <typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9, RV (*Func)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
    static v8::Handle<v8::Value> Invoke( v8::Arguments const & argv )
    {
        return Call( Func, argv );
    }

    template < typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9, void (*Func)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
    static v8::Handle<v8::Value> InvokeVoid( v8::Arguments const & argv )
    {
        return CallVoid< A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>( Func, argv );
    }

};

#endif // if !defined(DOXYGEN)
