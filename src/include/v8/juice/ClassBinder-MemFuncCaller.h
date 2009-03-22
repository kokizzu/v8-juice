/**
A helper class for forwarding JS arguments to member functions
taking 1 arguments.
*/
template <>
struct MemFuncCaller<1>
{
    enum { Arity = 1 };

    template <typename T, typename RV,  typename A0>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<1>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<1>::Call(): wrong argument count!"));
	//return convert::CastToJS( (RV) (obj->*Func)(  convert::CastFromJS< A0 >(argv[0]) ) );

	return convert::CastToJS<RV>( (obj->*MemFunc)(  convert::CastFromJS< A0 >(argv[0]) ) );
    }
    
    template <typename T, typename RV,  typename A0>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<1>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<1>::Call(): wrong argument count!"));
	return convert::CastToJS<RV>( (obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]) ) );
    }
    
    template <typename T,  typename A0>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<1>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<1>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]) );
	return Undefined();
    }

    template <typename T,  typename A0 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<1>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<1>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]) );
	return Undefined();
    }
};
template <>
struct WeakMemFuncCaller<1> : MemFuncCaller<1>
{
    enum { Arity = 1 };

    template <typename WeakWrappedType, typename RV,  typename A0 >
    static Handle<Value> CallOnWeakSelf( RV (WeakWrappedType::*func)( A0), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<1>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType, typename RV,  typename A0 >
    static Handle<Value> CallOnWeakSelf( const RV (WeakWrappedType::*func)( A0) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<1>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0 >
    static Handle<Value> CallOnWeakSelf( void (WeakWrappedType::*func)( A0), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<1>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0 >
    static Handle<Value> CallOnWeakSelf( const void (WeakWrappedType::*func)( A0) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<1>::Call() could not find native 'this' object!"));
        return Call( obj, func, argv );
    }

};
/**
A helper class for forwarding JS arguments to member functions
taking 2 arguments.
*/
template <>
struct MemFuncCaller<2>
{
    enum { Arity = 2 };

    template <typename T, typename RV,  typename A0,  typename A1>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<2>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<2>::Call(): wrong argument count!"));
	//return convert::CastToJS( (RV) (obj->*Func)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]) ) );

	return convert::CastToJS<RV>( (obj->*MemFunc)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<2>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<2>::Call(): wrong argument count!"));
	return convert::CastToJS<RV>( (obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]) ) );
    }
    
    template <typename T,  typename A0,  typename A1>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<2>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<2>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<2>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<2>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]) );
	return Undefined();
    }
};
template <>
struct WeakMemFuncCaller<2> : MemFuncCaller<2>
{
    enum { Arity = 2 };

    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1 >
    static Handle<Value> CallOnWeakSelf( RV (WeakWrappedType::*func)( A0, A1), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<2>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1 >
    static Handle<Value> CallOnWeakSelf( const RV (WeakWrappedType::*func)( A0, A1) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<2>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1 >
    static Handle<Value> CallOnWeakSelf( void (WeakWrappedType::*func)( A0, A1), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<2>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1 >
    static Handle<Value> CallOnWeakSelf( const void (WeakWrappedType::*func)( A0, A1) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<2>::Call() could not find native 'this' object!"));
        return Call( obj, func, argv );
    }

};
/**
A helper class for forwarding JS arguments to member functions
taking 3 arguments.
*/
template <>
struct MemFuncCaller<3>
{
    enum { Arity = 3 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<3>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<3>::Call(): wrong argument count!"));
	//return convert::CastToJS( (RV) (obj->*Func)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]) ) );

	return convert::CastToJS<RV>( (obj->*MemFunc)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<3>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<3>::Call(): wrong argument count!"));
	return convert::CastToJS<RV>( (obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<3>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<3>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<3>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<3>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]) );
	return Undefined();
    }
};
template <>
struct WeakMemFuncCaller<3> : MemFuncCaller<3>
{
    enum { Arity = 3 };

    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2 >
    static Handle<Value> CallOnWeakSelf( RV (WeakWrappedType::*func)( A0, A1, A2), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<3>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2 >
    static Handle<Value> CallOnWeakSelf( const RV (WeakWrappedType::*func)( A0, A1, A2) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<3>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2 >
    static Handle<Value> CallOnWeakSelf( void (WeakWrappedType::*func)( A0, A1, A2), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<3>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2 >
    static Handle<Value> CallOnWeakSelf( const void (WeakWrappedType::*func)( A0, A1, A2) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<3>::Call() could not find native 'this' object!"));
        return Call( obj, func, argv );
    }

};
/**
A helper class for forwarding JS arguments to member functions
taking 4 arguments.
*/
template <>
struct MemFuncCaller<4>
{
    enum { Arity = 4 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<4>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<4>::Call(): wrong argument count!"));
	//return convert::CastToJS( (RV) (obj->*Func)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]) ) );

	return convert::CastToJS<RV>( (obj->*MemFunc)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<4>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<4>::Call(): wrong argument count!"));
	return convert::CastToJS<RV>( (obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<4>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<4>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<4>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<4>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]) );
	return Undefined();
    }
};
template <>
struct WeakMemFuncCaller<4> : MemFuncCaller<4>
{
    enum { Arity = 4 };

    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3 >
    static Handle<Value> CallOnWeakSelf( RV (WeakWrappedType::*func)( A0, A1, A2, A3), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<4>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3 >
    static Handle<Value> CallOnWeakSelf( const RV (WeakWrappedType::*func)( A0, A1, A2, A3) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<4>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2,  typename A3 >
    static Handle<Value> CallOnWeakSelf( void (WeakWrappedType::*func)( A0, A1, A2, A3), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<4>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2,  typename A3 >
    static Handle<Value> CallOnWeakSelf( const void (WeakWrappedType::*func)( A0, A1, A2, A3) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<4>::Call() could not find native 'this' object!"));
        return Call( obj, func, argv );
    }

};
/**
A helper class for forwarding JS arguments to member functions
taking 5 arguments.
*/
template <>
struct MemFuncCaller<5>
{
    enum { Arity = 5 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<5>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<5>::Call(): wrong argument count!"));
	//return convert::CastToJS( (RV) (obj->*Func)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]) ) );

	return convert::CastToJS<RV>( (obj->*MemFunc)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<5>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<5>::Call(): wrong argument count!"));
	return convert::CastToJS<RV>( (obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<5>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<5>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<5>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<5>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]) );
	return Undefined();
    }
};
template <>
struct WeakMemFuncCaller<5> : MemFuncCaller<5>
{
    enum { Arity = 5 };

    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4 >
    static Handle<Value> CallOnWeakSelf( RV (WeakWrappedType::*func)( A0, A1, A2, A3, A4), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<5>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4 >
    static Handle<Value> CallOnWeakSelf( const RV (WeakWrappedType::*func)( A0, A1, A2, A3, A4) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<5>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4 >
    static Handle<Value> CallOnWeakSelf( void (WeakWrappedType::*func)( A0, A1, A2, A3, A4), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<5>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4 >
    static Handle<Value> CallOnWeakSelf( const void (WeakWrappedType::*func)( A0, A1, A2, A3, A4) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<5>::Call() could not find native 'this' object!"));
        return Call( obj, func, argv );
    }

};
/**
A helper class for forwarding JS arguments to member functions
taking 6 arguments.
*/
template <>
struct MemFuncCaller<6>
{
    enum { Arity = 6 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<6>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<6>::Call(): wrong argument count!"));
	//return convert::CastToJS( (RV) (obj->*Func)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]) ) );

	return convert::CastToJS<RV>( (obj->*MemFunc)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<6>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<6>::Call(): wrong argument count!"));
	return convert::CastToJS<RV>( (obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<6>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<6>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<6>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<6>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]) );
	return Undefined();
    }
};
template <>
struct WeakMemFuncCaller<6> : MemFuncCaller<6>
{
    enum { Arity = 6 };

    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5 >
    static Handle<Value> CallOnWeakSelf( RV (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<6>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5 >
    static Handle<Value> CallOnWeakSelf( const RV (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<6>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5 >
    static Handle<Value> CallOnWeakSelf( void (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<6>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5 >
    static Handle<Value> CallOnWeakSelf( const void (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<6>::Call() could not find native 'this' object!"));
        return Call( obj, func, argv );
    }

};
/**
A helper class for forwarding JS arguments to member functions
taking 7 arguments.
*/
template <>
struct MemFuncCaller<7>
{
    enum { Arity = 7 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<7>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<7>::Call(): wrong argument count!"));
	//return convert::CastToJS( (RV) (obj->*Func)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]) ) );

	return convert::CastToJS<RV>( (obj->*MemFunc)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<7>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<7>::Call(): wrong argument count!"));
	return convert::CastToJS<RV>( (obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<7>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<7>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<7>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<7>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]) );
	return Undefined();
    }
};
template <>
struct WeakMemFuncCaller<7> : MemFuncCaller<7>
{
    enum { Arity = 7 };

    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6 >
    static Handle<Value> CallOnWeakSelf( RV (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5, A6), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<7>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6 >
    static Handle<Value> CallOnWeakSelf( const RV (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5, A6) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<7>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6 >
    static Handle<Value> CallOnWeakSelf( void (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5, A6), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<7>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6 >
    static Handle<Value> CallOnWeakSelf( const void (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5, A6) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<7>::Call() could not find native 'this' object!"));
        return Call( obj, func, argv );
    }

};
/**
A helper class for forwarding JS arguments to member functions
taking 8 arguments.
*/
template <>
struct MemFuncCaller<8>
{
    enum { Arity = 8 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<8>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<8>::Call(): wrong argument count!"));
	//return convert::CastToJS( (RV) (obj->*Func)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]), convert::CastFromJS< A7 >(argv[7]) ) );

	return convert::CastToJS<RV>( (obj->*MemFunc)(  convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]), convert::CastFromJS< A7 >(argv[7]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<8>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<8>::Call(): wrong argument count!"));
	return convert::CastToJS<RV>( (obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]), convert::CastFromJS< A7 >(argv[7]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7), Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<8>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<8>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]), convert::CastFromJS< A7 >(argv[7]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7) const, Arguments const & argv )
    {
	if( ! obj ) return ThrowException(String::New("MemFuncCaller<8>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCaller<8>::Call(): wrong argument count!"));
	(obj->*MemFunc)( convert::CastFromJS< A0 >(argv[0]), convert::CastFromJS< A1 >(argv[1]), convert::CastFromJS< A2 >(argv[2]), convert::CastFromJS< A3 >(argv[3]), convert::CastFromJS< A4 >(argv[4]), convert::CastFromJS< A5 >(argv[5]), convert::CastFromJS< A6 >(argv[6]), convert::CastFromJS< A7 >(argv[7]) );
	return Undefined();
    }
};
template <>
struct WeakMemFuncCaller<8> : MemFuncCaller<8>
{
    enum { Arity = 8 };

    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7 >
    static Handle<Value> CallOnWeakSelf( RV (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5, A6, A7), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<8>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7 >
    static Handle<Value> CallOnWeakSelf( const RV (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5, A6, A7) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<8>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7 >
    static Handle<Value> CallOnWeakSelf( void (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5, A6, A7), Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<8>::Call() could not find native 'this' object!"));
	return Call( obj, func, argv );
    }
    
    template <typename WeakWrappedType,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7 >
    static Handle<Value> CallOnWeakSelf( const void (WeakWrappedType::*func)( A0, A1, A2, A3, A4, A5, A6, A7) const, Arguments const & argv )
    {
	typedef WeakJSClassCreator<WeakWrappedType> Wrapper;
	//typename Wrapper::WrappedType const * obj = Wrapper::GetSelf( argv.This() );
        typedef typename Wrapper::WrappedType WT;
        WT * obj = CastFromJS<WT>( argv.This() );
	if( ! obj ) return ThrowException(String::New("WeakMemFuncCaller<8>::Call() could not find native 'this' object!"));
        return Call( obj, func, argv );
    }

};
