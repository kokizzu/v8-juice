// GENERATED CODE! Edit at your own risk!
#if ! defined(DOXYGEN)
/** Member function call forwarder for member functions taking 1 arguments. */
template < typename WrappedType, typename RV,  typename A0, RV (WrappedType::*Func)( A0)  >
struct MemFuncCallOp1
{
enum { Arity = 1 };
typedef WrappedType Type;
static Handle<Value> Call( Type * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp1::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp1::Call(): wrong argument count!"));
	return convert::CastToJS( (RV) (obj->*Func)(
		convert::CastFromJS< A0 >(argv[0])
) );
}
};
/** Specialization for member functions taking 1 args and returning void. */
template < typename WrappedType,  typename A0, void (WrappedType::*Func)( A0)  >
struct MemFuncCallOp1< WrappedType,void, A0, Func >
{
enum { Arity = 1 };
typedef WrappedType Type;
static Handle<Value> Call( Type * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp1::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp1::Call(): wrong argument count!"));
	(obj->*Func)(
		convert::CastFromJS< A0 >(argv[0])
		);
		return ::v8::Undefined();
	}
};

/** Member function call forwarder for const member functions taking 1 arguments. */
template < typename WrappedType, typename RV,  typename A0, RV (WrappedType::*Func)( A0)  const >
struct MemFuncCallOp1<const WrappedType, RV, A0, Func>
{
enum { Arity = 1 };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp1::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp1::Call(): wrong argument count!"));
	return convert::CastToJS( (RV) (obj->*Func)(
		convert::CastFromJS< A0 >(argv[0])
) );
}
};
/** Specialization for const member functions taking 1 args and returning void. */
template < typename WrappedType,  typename A0, void (WrappedType::*Func)( A0)  const >
struct MemFuncCallOp1< const WrappedType,void, A0, Func >
{
enum { Arity = 1 };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp1::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp1::Call(): wrong argument count!"));
	(obj->*Func)(
		convert::CastFromJS< A0 >(argv[0])
		);
		return ::v8::Undefined();
	}
};

/** Member function call forwarder for member functions taking 2 arguments. */
template < typename WrappedType, typename RV,  typename A0,  typename A1, RV (WrappedType::*Func)( A0, A1)  >
struct MemFuncCallOp2
{
enum { Arity = 2 };
typedef WrappedType Type;
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
/** Specialization for member functions taking 2 args and returning void. */
template < typename WrappedType,  typename A0,  typename A1, void (WrappedType::*Func)( A0, A1)  >
struct MemFuncCallOp2< WrappedType,void, A0, A1, Func >
{
enum { Arity = 2 };
typedef WrappedType Type;
static Handle<Value> Call( Type * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp2::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp2::Call(): wrong argument count!"));
	(obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1])
		);
		return ::v8::Undefined();
	}
};

/** Member function call forwarder for const member functions taking 2 arguments. */
template < typename WrappedType, typename RV,  typename A0,  typename A1, RV (WrappedType::*Func)( A0, A1)  const >
struct MemFuncCallOp2<const WrappedType, RV, A0, A1, Func>
{
enum { Arity = 2 };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp2::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp2::Call(): wrong argument count!"));
	return convert::CastToJS( (RV) (obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1])
) );
}
};
/** Specialization for const member functions taking 2 args and returning void. */
template < typename WrappedType,  typename A0,  typename A1, void (WrappedType::*Func)( A0, A1)  const >
struct MemFuncCallOp2< const WrappedType,void, A0, A1, Func >
{
enum { Arity = 2 };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp2::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp2::Call(): wrong argument count!"));
	(obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1])
		);
		return ::v8::Undefined();
	}
};

/** Member function call forwarder for member functions taking 3 arguments. */
template < typename WrappedType, typename RV,  typename A0,  typename A1,  typename A2, RV (WrappedType::*Func)( A0, A1, A2)  >
struct MemFuncCallOp3
{
enum { Arity = 3 };
typedef WrappedType Type;
static Handle<Value> Call( Type * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp3::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp3::Call(): wrong argument count!"));
	return convert::CastToJS( (RV) (obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2])
) );
}
};
/** Specialization for member functions taking 3 args and returning void. */
template < typename WrappedType,  typename A0,  typename A1,  typename A2, void (WrappedType::*Func)( A0, A1, A2)  >
struct MemFuncCallOp3< WrappedType,void, A0, A1, A2, Func >
{
enum { Arity = 3 };
typedef WrappedType Type;
static Handle<Value> Call( Type * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp3::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp3::Call(): wrong argument count!"));
	(obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2])
		);
		return ::v8::Undefined();
	}
};

/** Member function call forwarder for const member functions taking 3 arguments. */
template < typename WrappedType, typename RV,  typename A0,  typename A1,  typename A2, RV (WrappedType::*Func)( A0, A1, A2)  const >
struct MemFuncCallOp3<const WrappedType, RV, A0, A1, A2, Func>
{
enum { Arity = 3 };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp3::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp3::Call(): wrong argument count!"));
	return convert::CastToJS( (RV) (obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2])
) );
}
};
/** Specialization for const member functions taking 3 args and returning void. */
template < typename WrappedType,  typename A0,  typename A1,  typename A2, void (WrappedType::*Func)( A0, A1, A2)  const >
struct MemFuncCallOp3< const WrappedType,void, A0, A1, A2, Func >
{
enum { Arity = 3 };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp3::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp3::Call(): wrong argument count!"));
	(obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2])
		);
		return ::v8::Undefined();
	}
};

/** Member function call forwarder for member functions taking 4 arguments. */
template < typename WrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3, RV (WrappedType::*Func)( A0, A1, A2, A3)  >
struct MemFuncCallOp4
{
enum { Arity = 4 };
typedef WrappedType Type;
static Handle<Value> Call( Type * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp4::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp4::Call(): wrong argument count!"));
	return convert::CastToJS( (RV) (obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2]),
		convert::CastFromJS< A3 >(argv[3])
) );
}
};
/** Specialization for member functions taking 4 args and returning void. */
template < typename WrappedType,  typename A0,  typename A1,  typename A2,  typename A3, void (WrappedType::*Func)( A0, A1, A2, A3)  >
struct MemFuncCallOp4< WrappedType,void, A0, A1, A2, A3, Func >
{
enum { Arity = 4 };
typedef WrappedType Type;
static Handle<Value> Call( Type * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp4::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp4::Call(): wrong argument count!"));
	(obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2]),
		convert::CastFromJS< A3 >(argv[3])
		);
		return ::v8::Undefined();
	}
};

/** Member function call forwarder for const member functions taking 4 arguments. */
template < typename WrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3, RV (WrappedType::*Func)( A0, A1, A2, A3)  const >
struct MemFuncCallOp4<const WrappedType, RV, A0, A1, A2, A3, Func>
{
enum { Arity = 4 };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp4::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp4::Call(): wrong argument count!"));
	return convert::CastToJS( (RV) (obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2]),
		convert::CastFromJS< A3 >(argv[3])
) );
}
};
/** Specialization for const member functions taking 4 args and returning void. */
template < typename WrappedType,  typename A0,  typename A1,  typename A2,  typename A3, void (WrappedType::*Func)( A0, A1, A2, A3)  const >
struct MemFuncCallOp4< const WrappedType,void, A0, A1, A2, A3, Func >
{
enum { Arity = 4 };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp4::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp4::Call(): wrong argument count!"));
	(obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2]),
		convert::CastFromJS< A3 >(argv[3])
		);
		return ::v8::Undefined();
	}
};

/** Member function call forwarder for member functions taking 5 arguments. */
template < typename WrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4, RV (WrappedType::*Func)( A0, A1, A2, A3, A4)  >
struct MemFuncCallOp5
{
enum { Arity = 5 };
typedef WrappedType Type;
static Handle<Value> Call( Type * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp5::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp5::Call(): wrong argument count!"));
	return convert::CastToJS( (RV) (obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2]),
		convert::CastFromJS< A3 >(argv[3]),
		convert::CastFromJS< A4 >(argv[4])
) );
}
};
/** Specialization for member functions taking 5 args and returning void. */
template < typename WrappedType,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4, void (WrappedType::*Func)( A0, A1, A2, A3, A4)  >
struct MemFuncCallOp5< WrappedType,void, A0, A1, A2, A3, A4, Func >
{
enum { Arity = 5 };
typedef WrappedType Type;
static Handle<Value> Call( Type * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp5::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp5::Call(): wrong argument count!"));
	(obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2]),
		convert::CastFromJS< A3 >(argv[3]),
		convert::CastFromJS< A4 >(argv[4])
		);
		return ::v8::Undefined();
	}
};

/** Member function call forwarder for const member functions taking 5 arguments. */
template < typename WrappedType, typename RV,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4, RV (WrappedType::*Func)( A0, A1, A2, A3, A4)  const >
struct MemFuncCallOp5<const WrappedType, RV, A0, A1, A2, A3, A4, Func>
{
enum { Arity = 5 };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp5::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp5::Call(): wrong argument count!"));
	return convert::CastToJS( (RV) (obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2]),
		convert::CastFromJS< A3 >(argv[3]),
		convert::CastFromJS< A4 >(argv[4])
) );
}
};
/** Specialization for const member functions taking 5 args and returning void. */
template < typename WrappedType,  typename A0,  typename A1,  typename A2,  typename A3,  typename A4, void (WrappedType::*Func)( A0, A1, A2, A3, A4)  const >
struct MemFuncCallOp5< const WrappedType,void, A0, A1, A2, A3, A4, Func >
{
enum { Arity = 5 };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("MemFuncCallOp5::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("MemFuncCallOp5::Call(): wrong argument count!"));
	(obj->*Func)(
		convert::CastFromJS< A0 >(argv[0]),
		convert::CastFromJS< A1 >(argv[1]),
		convert::CastFromJS< A2 >(argv[2]),
		convert::CastFromJS< A3 >(argv[3]),
		convert::CastFromJS< A4 >(argv[4])
		);
		return ::v8::Undefined();
	}
};

#endif //if !defined(DOXYGEN)
