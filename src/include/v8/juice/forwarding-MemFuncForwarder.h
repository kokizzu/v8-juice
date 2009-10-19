// GENERATED CODE! Edit at your own risk!
#if !defined(DOXYGEN)
/**
A helper class for forwarding JS arguments to member functions
taking 1 arguments.
*/
template <>
struct MemFuncForwarder<1>
{
    enum { Arity = 1 };

    template <typename T, typename RV,  typename A0>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<1>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<1>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)(  CastFromJS< A0 >(argv[0]) ) );
    }
    
    template <typename T, typename RV,  typename A0>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<1>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<1>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)( CastFromJS< A0 >(argv[0]) ) );
    }
    
    template <typename T,  typename A0>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<1>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<1>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]) );
	return Undefined();
    }

    template <typename T,  typename A0 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<1>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<1>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]) );
	return Undefined();
    }
};
/**
A helper class for forwarding JS arguments to member functions
taking 2 arguments.
*/
template <>
struct MemFuncForwarder<2>
{
    enum { Arity = 2 };

    template <typename T, typename RV,  typename A0,  typename A1>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<2>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<2>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<2>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<2>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]) ) );
    }
    
    template <typename T,  typename A0,  typename A1>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<2>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<2>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<2>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<2>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]) );
	return Undefined();
    }
};
/**
A helper class for forwarding JS arguments to member functions
taking 3 arguments.
*/
template <>
struct MemFuncForwarder<3>
{
    enum { Arity = 3 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<3>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<3>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<3>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<3>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<3>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<3>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<3>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<3>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]) );
	return Undefined();
    }
};
/**
A helper class for forwarding JS arguments to member functions
taking 4 arguments.
*/
template <>
struct MemFuncForwarder<4>
{
    enum { Arity = 4 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<4>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<4>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<4>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<4>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<4>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<4>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<4>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<4>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]) );
	return Undefined();
    }
};
/**
A helper class for forwarding JS arguments to member functions
taking 5 arguments.
*/
template <>
struct MemFuncForwarder<5>
{
    enum { Arity = 5 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<5>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<5>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<5>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<5>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<5>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<5>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<5>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<5>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]) );
	return Undefined();
    }
};
/**
A helper class for forwarding JS arguments to member functions
taking 6 arguments.
*/
template <>
struct MemFuncForwarder<6>
{
    enum { Arity = 6 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<6>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<6>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<6>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<6>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<6>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<6>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<6>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<6>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]) );
	return Undefined();
    }
};
/**
A helper class for forwarding JS arguments to member functions
taking 7 arguments.
*/
template <>
struct MemFuncForwarder<7>
{
    enum { Arity = 7 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<7>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<7>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<7>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<7>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<7>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<7>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<7>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<7>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]) );
	return Undefined();
    }
};
/**
A helper class for forwarding JS arguments to member functions
taking 8 arguments.
*/
template <>
struct MemFuncForwarder<8>
{
    enum { Arity = 8 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<8>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<8>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<8>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<8>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<8>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<8>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<8>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<8>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]) );
	return Undefined();
    }
};
/**
A helper class for forwarding JS arguments to member functions
taking 9 arguments.
*/
template <>
struct MemFuncForwarder<9>
{
    enum { Arity = 9 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7, A8), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<9>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<9>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7, A8) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<9>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<9>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7, A8), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<9>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<9>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7, A8) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<9>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<9>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]) );
	return Undefined();
    }
};
/**
A helper class for forwarding JS arguments to member functions
taking 10 arguments.
*/
template <>
struct MemFuncForwarder<10>
{
    enum { Arity = 10 };

    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9>
    static Handle<Value> Call( T * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<10>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<10>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)(  CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]), CastFromJS< A9 >(argv[9]) ) );
    }
    
    template <typename T, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9>
    static Handle<Value> Call( T const * obj, RV (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<10>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<10>::Call(): wrong argument count!"));
	return CastToJS<RV>( (obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]), CastFromJS< A9 >(argv[9]) ) );
    }
    
    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9>
    static Handle<Value> Call( T * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9), Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<10>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<10>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]), CastFromJS< A9 >(argv[9]) );
	return Undefined();
    }

    template <typename T,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4,  typename A5,  typename A6,  typename A7,  typename A8,  typename A9 >
    static Handle<Value> Call( T const * obj, void (T::*MemFunc)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const, Arguments const & argv )
    {
	if( ! obj ) return v8::ThrowException(v8::String::New("MemFuncForwarder<10>::Call(): Native object is null!"));
	else if( argv.Length() < Arity ) return v8::ThrowException(v8::String::New("MemFuncForwarder<10>::Call(): wrong argument count!"));
	(obj->*MemFunc)( CastFromJS< A0 >(argv[0]), CastFromJS< A1 >(argv[1]), CastFromJS< A2 >(argv[2]), CastFromJS< A3 >(argv[3]), CastFromJS< A4 >(argv[4]), CastFromJS< A5 >(argv[5]), CastFromJS< A6 >(argv[6]), CastFromJS< A7 >(argv[7]), CastFromJS< A8 >(argv[8]), CastFromJS< A9 >(argv[9]) );
	return Undefined();
    }
};
#endif // if !defined(DOXYGEN)
