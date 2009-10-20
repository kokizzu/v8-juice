// EXPERIMENTAL/INCOMPLETE!
#include <sstream>
#include <v8/juice/static_assert.h>
#include <v8/juice/bind.h>
#include <v8/juice/convert.h>
#include <vector>

namespace v8 {
namespace juice {
    using namespace v8::juice;
    namespace Detail
    {
	/**
	   An internal helper type for only use by ClassWrap.
	*/
	template <typename NativeT>
	struct ClassWrapMapper
	{
	    typedef NativeT WrappedType;
            typedef WrappedType * NativeHandle;
            typedef v8::Handle<v8::Object> JSObjHandle;
            typedef std::pair<NativeHandle, JSObjHandle > ObjBindT;
	    typedef std::map<void const *,ObjBindT > OneOfUsT;
            typedef typename OneOfUsT::iterator Iterator;
	    static OneOfUsT & Map()
	    {
		static OneOfUsT bob;
		return bob;
            }
            
            static void Insert( v8::Handle<v8::Object> jself, NativeHandle obj )
            {
                JSObjHandle p = JSObjHandle(*jself);
                Map().insert( std::make_pair( obj, std::make_pair( obj, p ) ) );
            }

            static NativeHandle Remove( void const * key )
            {
                OneOfUsT & map( Map() );
                Iterator it = map.find( key );
                if( map.end() == it )
                {
                    return 0;
                }
                NativeHandle victim = (*it).second.first;
                map.erase(it);
                // FIXME? Do we need to Dispose() or Clear() the Persistent handles?
                return victim;
            }

            static WrappedType * GetNative( void const * key )
            {
                typename OneOfUsT::iterator it = Map().find(key);
                return (Map().end() == it)
                      ? 0
                      : (*it).second.first;
            }

            static Handle<Object> GetJSObject( void const * key )
	    {
                typename OneOfUsT::iterator it = Map().find(key);
                if( Map().end() == it ) return Handle<Object>();
                else return (*it).second.second;
	    }
        };

        /**
           Useless base instantiation - See the 0-specialization for details.
        */
        template <int Arity_>
        struct BoundMemberCaller
        {
            enum { Arity = Arity_ };
        };

    }

    template <typename ValT, ValT Val>
    struct ClassWrap_Opt_ConstVal
    {
        typedef ValT Type;
        static const Type Value = Val;
    };

    template <int Val>
    struct ClassWrap_Opt_Int : ClassWrap_Opt_ConstVal<int,Val>
    {};

    template <bool Val>
    struct ClassWrap_Opt_Bool : ClassWrap_Opt_ConstVal<bool,Val>
    {};
    


    template <typename T>
    struct ClassWrap_Types
    {
        typedef T Type;
        typedef T * NativeHandle;
        typedef v8::Handle<v8::Value> JSHandle;
        //static NativeHandle Pointer( NativeHandle x ) { return x; }
    };

    template <typename T>
    struct ClassWrap_Types<T const> : ClassWrap_Types<T>
    {};

    template <typename T>
    struct ClassWrap_Types<T *> : ClassWrap_Types<T>
    {};

    template <typename T>
    struct ClassWrap_Types<T const *> : ClassWrap_Types<T>
    {};

    template <typename T>
    struct ClassWrap_Types<T &> : ClassWrap_Types<T>
    {};

    template <typename T>
    struct ClassWrap_Types<T const &> : ClassWrap_Types<T>
    {};

    template <typename T>
    struct ClassWrap_Ops_ClassName
    {
        static char const * Value();
    };
    
    template <typename T>
    struct ClassWrap_Opt_InternalFields : ClassWrap_Opt_Int<1>
    {
        static const int NativeObjectIndex = 0;
    };

    template <typename T>
    struct ClassWrap_Opt_CtorWithoutNew : ClassWrap_Opt_Bool<false>
    {};
    
    template <typename T>
    struct ClassWrap_Ops_Memory
    {
        typedef typename ClassWrap_Types<T>::Type Type;
        typedef typename ClassWrap_Types<T>::NativeHandle NativeHandle;
	static NativeHandle Instantiate( Arguments const &  /*argv*/,
                                         std::string & exceptionText )
	{
	    exceptionText = "ClassWrap_Ops_Memory<T>::Instantiate() not implemented!";
	    return NativeHandle(0);
	}
	static void Destruct( NativeHandle obj )
	{
            //JUICE_STATIC_ASSERT(false,ClassWrap_Ops_NativeHandle_T_must_be_specialized);
	}
    };

    template <typename T>
    struct ClassWrap_Ops_ToNative
    {
        typedef typename ClassWrap_Types<T>::Type Type;
        typedef typename ClassWrap_Types<T>::NativeHandle NativeHandle;

    private:
    public:
        static NativeHandle Value( v8::Handle<v8::Object> const jo )
        {
            static const int FieldNum = ClassWrap_Opt_InternalFields<T>::NativeObjectIndex;
            if( jo.IsEmpty() || (jo->InternalFieldCount() < FieldNum) ) return 0;
            v8::Local<v8::Value> const lv( jo->GetInternalField( FieldNum ) );
            if( lv.IsEmpty() || !lv->IsExternal() ) return 0;
            Local<External> const ex( External::Cast( *lv ) );
            NativeHandle x = static_cast<NativeHandle>( ex.IsEmpty() ? 0 : ex->Value() );
            return x;
        }

        static NativeHandle Value( v8::Handle<v8::Value> const h )
        {
            if( h.IsEmpty() || !h->IsObject() )
            {
                return 0;
            }
            else
            {
                Handle<Object> const jo( Object::Cast( *h ) );
                return Value( jo );
            }
        }
    };

    template <typename T>
    struct ClassWrap_Ops_ToJS
    {
    public:
        typedef typename ClassWrap_Types<T>::Type Type;
        typedef typename ClassWrap_Types<T>::NativeHandle NativeHandle;
        static v8::Handle<v8::Object> Value( NativeHandle )
        {
            JUICE_STATIC_ASSERT(false,ClassWrap_Ops_T_ToJS_cannot_work_for_the_general_case);
        }
    };


#if 0
    // don't use this, because it forces us to instantiate both conversions,
    // whereas a Native-to-JS conversion is not generically possible.
    /**
       Casting routines used by ClassWrap. This can be defaulted as long as
       ClassWrap_Ops_ToJS<T> and ClassWrap_Ops_ToNative<T> are properly
       specialized.
    */
    template <typename T>
    struct ClassWrap_Ops_Cast
    {
        typedef typename ClassWrap_Types<T>::Type Type;
        typedef typename ClassWrap_Types<T>::NativeHandle NativeHandle;

    private:
    public:
        static NativeHandle ToNative( v8::Handle<v8::Object> const jo )
        {
            return ClassWrap_Ops_ToNative<T>::Value(jo);
        }

        static NativeHandle ToNative( v8::Handle<v8::Value> const h )
        {
            return ClassWrap_Ops_ToNative<T>::Value(h);
        }

        static v8::Handle<v8::Object> ToJS( NativeHandle h )
        {
            return ClassWrap_Ops_ToJS<T>::Value(h);
        }
    };
#endif

    template <typename T>
    class ClassWrap : public JSClassCreator
    {
    public:
        typedef ClassWrap_Types<T> TypeInfo;
        typedef typename TypeInfo::Type Type;
        typedef typename TypeInfo::NativeHandle NativeHandle;
        typedef ClassWrap_Ops_ClassName<T> ClassName;
        typedef ClassWrap_Ops_Memory<T> OpsMemory;
        typedef ClassWrap_Ops_ToJS<T> CastToJS;
        typedef ClassWrap_Ops_ToNative<T> CastToNative;
        static const int OptInternalFields = ClassWrap_Opt_InternalFields<T>::Value;
        static const int OptNativeFieldIndex = ClassWrap_Opt_InternalFields<T>::NativeObjectIndex;
        static const bool OptAllowCtorWithoutNew = ClassWrap_Opt_CtorWithoutNew<T>::Value;
    private:
        static void check_assertions()
        {
            JUICE_STATIC_ASSERT((OptInternalFields>0),ClassWrap_Ops_InternalFields_Value_May_Not_Be_Negative);
            JUICE_STATIC_ASSERT((OptNativeFieldIndex>=0),ClassWrap_Ops_InternalFields_NativeObjectIndex_Value_May_Not_Be_Negative);
            JUICE_STATIC_ASSERT((OptInternalFields>OptNativeFieldIndex),ClassWrap_Ops_InternalFields_Value_Is_Too_Small);
        }
        // TODO: move this into a policy class
	static void weak_callback(Persistent< Value > pv, void * nobj)
	{
	    CERR << ClassName::Value()<<"::weak_callback(@"<<(void const *)nobj<<")\n";
	    Local<Object> jobj( Object::Cast(*pv) );
	    if( jobj->InternalFieldCount() != (OptInternalFields) ) return; // how to warn about this?
	    Local<Value> lv( jobj->GetInternalField(OptNativeFieldIndex) );
	    if( lv.IsEmpty() || !lv->IsExternal() ) return; // how to warn about this?
	    /**
	       We have to ensure that we have no dangling External in JS space. This
	       is so that functions like IODevice.close() can act safely with the
	       knowledge that member funcs called after that won't get a dangling
	       pointer. Without this, some code will crash in that case.
	    */
	    jobj->SetInternalField(OptNativeFieldIndex,Null());
	    pv.Dispose();
	    pv.Clear();
            OpsMemory::Destruct( static_cast<NativeHandle>(nobj) );
	}

        // TODO: move this into a policy class
	static Persistent<Object> wrap_native( Handle<Object> _self, NativeHandle obj )
	{
            CERR << ClassName::Value() <<"::wrap_native(@"<<(void const *)obj<<") Binding to internal field #"<<OptNativeFieldIndex<<"\n";
	    Persistent<Object> self( Persistent<Object>::New(_self) );
	    self.MakeWeak( obj, weak_callback );
	    self->SetInternalField( OptNativeFieldIndex, External::New(obj) );
	    return self;
	}

	static Handle<Value> ctor_proxy( const Arguments & argv )
	{
            if( OptAllowCtorWithoutNew )
            {
                /**
                   Allow construction without 'new' by forcing this
                   function to be called in a ctor context...
                */
                if (!argv.IsConstructCall()) 
                {
                    const int argc = argv.Length();
                    Handle<Function> ctor( Function::Cast(*argv.Callee()));
                    std::vector< Handle<Value> > av(static_cast<size_t>(argc),Undefined());
                    for( int i = 0; i < argc; ++i ) av[i] = argv[i];
                    return ctor->NewInstance( argc, &av[0] );
                }
            }
            else
            {
                /**
                   Why have this limitation? If we don't, v8 pukes when
                   the ctor is called, with
                   "v8::Object::SetInternalField() Writing internal field
                   out of bounds".
                */
                if (!argv.IsConstructCall()) 
                {
                    std::ostringstream os;
                    os << "The "<< ClassName::Value() << " constructor cannot be called as function!";
                    return ThrowException(String::New(os.str().c_str()));
                }
            }
	    std::string err;
	    NativeHandle obj = NativeHandle(0);
	    try
	    {
		obj = OpsMemory::Instantiate( argv, err );
	    }
	    catch(std::exception const & ex)
	    {
		return ThrowException(String::New(ex.what()));
	    }
	    catch(...)
	    {
		return ThrowException(String::New("Native constructor threw an unknown native exception!"));
	    }
	    if( ! err.empty() )
	    {
		if( obj )
		{
		    OpsMemory::Destruct(obj);
		    obj = NativeHandle(0);
		}
		return ThrowException(String::New(err.c_str(),static_cast<int>(err.size())));
	    }
	    if( ! obj )
	    {
		return ThrowException(String::New("Constructor failed for an unspecified reason!"));
	    }
	    return wrap_native( argv.This(), obj );
	}
        
    public:
        explicit ClassWrap( Handle<Object> target)
	    : JSClassCreator( ClassName::Value(),
			      target,
			      ctor_proxy,
			      OptInternalFields )
	{
            check_assertions();
	}
        ClassWrap()
	    : JSClassCreator( ClassName::Value(),
			      ctor_proxy,
			      OptInternalFields )
	{
            check_assertions();
	}

        static NativeHandle GetNative( Handle<Value> const & h )
        {
            return CastToNative::Value(h);
        }

        /**
           Binds the native member function, specified as a template
           parameter, to a JS member function with the given
           name.
        */
        template < v8::Handle<Value> (Type::*Func)( v8::Arguments const & ) >
        ClassWrap & BindMemberFunc( char const * name )
        {
            this->Set(name, convert::InvocationCallbackMember<Type,Func>::Call );
            return *this;
        }
        
    };



} } // namespaces
