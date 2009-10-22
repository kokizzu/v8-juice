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

    /**
       Base class for static ClassWrap options.
     */
    template <typename ValT, ValT Val>
    struct ClassWrap_Opt_ConstVal
    {
        typedef ValT Type;
        static const Type Value = Val;
    };

    /**
       Base class for static integer ClassWrap options.
    */
    template <int Val>
    struct ClassWrap_Opt_Int : ClassWrap_Opt_ConstVal<int,Val>
    {};

    /**
       Base class for static boolean ClassWrap options.
    */
    template <bool Val>
    struct ClassWrap_Opt_Bool : ClassWrap_Opt_ConstVal<bool,Val>
    {};


    /**
       Stores the base type info for ClassWrap-able types. All other
       policies "should" use these typedefs for the parameters and
       arguments.
    */
    template <typename T>
    struct ClassWrap_Types
    {
        typedef T Type;
        typedef T * NativeHandle;
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
    struct ClassWrap_ClassName
    {
        static char const * Value();
    };

    /**
       ClassWrap option to set the number of internal fields reserved for
       JS objects. It must be greater than 0 and greater than NativeIndex.
    */
    template <typename T>
    struct ClassWrap_Opt_InternalFields : ClassWrap_Opt_Int<1>
    {
        /**
           The internal field index at which ClassWrap policies should
           expect the native object to be found in any give JS object.
           It must be 0 or greater and less than Value.
        */
        static const int NativeIndex = 0;
    };

    namespace Detail
    {
        /**
           Performs a compile-time assertion for the parameters of the
           given type, which must conform to the
           ClassWrap_Opt_InternalFields interface.
        */
        template <typename InternalFields>
        void assert_internal_fields()
        {
            typedef InternalFields IFT;
            JUICE_STATIC_ASSERT((IFT::Value>0),
                                ClassWrap_InternalFields_Value_MayNotBeNegative);
            JUICE_STATIC_ASSERT((IFT::NativeIndex>=0),
                                ClassWrap_InternalFields_NativeIndex_MayNotBeNegative);
            JUICE_STATIC_ASSERT((IFT::Value>IFT::NativeIndex),
                                ClassWrap_InternalFields_Value_IsTooSmallForNativeIndex);
        }
    }
    
    /**
       Policy class responsible for setting whether or not a ClassWrap-bound
       class should allow "Foo()" and "new Foo()" to behave the same
       or not.
    */
    template <typename T>
    struct ClassWrap_Opt_CtorWithoutNew : ClassWrap_Opt_Bool<false>
    {};

    /**
       Policy class responsible for instantiating and destroying
       ClassWrap_Types<T>::NativeHandle objects.
    */
    template <typename T>
    struct ClassWrap_Memory
    {
        typedef typename ClassWrap_Types<T>::Type Type;
        typedef typename ClassWrap_Types<T>::NativeHandle NativeHandle;
	static NativeHandle Instantiate( Arguments const &  /*argv*/,
                                         std::ostream & exceptionText )
	{
            JUICE_STATIC_ASSERT(false,ClassWrap_Memory_T_MustBeSpecialized);
	    exceptionText << "ClassWrap_Memory<"
                          << ClassWrap_ClassName<T>::Value()
                          << ">::Instantiate() not implemented!";
	    return NativeHandle(0);
	}
	static void Destruct( NativeHandle obj )
	{
            JUICE_STATIC_ASSERT(false,ClassWrap_Memory_T_MustBeSpecialized);
	}
    };


#if 0
    /**
       Calls CWOps_ToNative(h), where CWOps_ToNative _must_
       be-a ClassWrap_ToNative<> specialization.

       It is intended as a convenience to be used by
       ClassWrap_ToNative<> specializations to implement their
       Value(Handle<Value>) overload.

       If h is empty or !h->IsObject() then 0 is returned, else it
       returns the result of passing that object to
       CWOps_ToNative::Value(Handle<Object>).
    */
    template <typename CWOps_ToNative>
    static typename CWOps_ToNative::NativeHandle
    ClassWrap_ToNative_ValueToObj( v8::Handle<v8::Value> const & h )
    {
        if( h.IsEmpty() || !h->IsObject() )
        {
            return 0;
        }
        else
        {
            Handle<Object> const jo( Object::Cast( *h ) );
            return CWOps_ToNative::Value(h);
        }
    }
#endif

    /**
       A ClassWrap_ToNative<T> implementation which uses
       static_cast<ClassWrap_Types<T>::NativeHandle>(void*) to convert
       objects from JS to native space. It takes a certain, but not
       infallible, deal of care in ensuring that the (void*) is
       actually a native object of the proper type.
    */
    template <typename T>
    struct ClassWrap_ToNative_StaticCast
    {
        typedef typename ClassWrap_Types<T>::Type Type;
        typedef typename ClassWrap_Types<T>::NativeHandle NativeHandle;
    public:
        /**
           The default implementation looks for a v8::External object
           stored in the N'th internal field (where N ==
           ClassWrap_Opt_InternalFields<T>::NativeIndex).

           If it finds one, it static_cast()'s it to a NativeHandle
           and returns it.

           Returns 0 on error.

           Ownership of the returned object is not modified by this call.
        */
        static NativeHandle Value( v8::Handle<v8::Object> const & jo )
        {
            typedef ClassWrap_Opt_InternalFields<T> IFT;
            { static bool inited = (Detail::assert_internal_fields<IFT>(),true); }
            static const int FieldNum = IFT::NativeIndex;
            if( jo.IsEmpty() || (jo->InternalFieldCount() < FieldNum) ) return 0;
            v8::Local<v8::Value> const lv( jo->GetInternalField( FieldNum ) );
            void * ext = lv.IsEmpty() ? 0 : External::Unwrap(lv);
            return ext ?
                static_cast<NativeHandle>( ext )
                : 0;
        }
        /**
           Convenience overload. If h is empty or !h->IsObject() then
           0 is returned, else it returns the result of passing that
           object to the other Value() overload.
        */
        static NativeHandle Value( v8::Handle<v8::Value> const & h )
        {
            if( h.IsEmpty() || !h->IsObject() )
            {
                return 0;
            }
            else
            {
                Handle<Object> const jo( Object::Cast( *h ) );
                return Value(jo);
            }
        }
    };

    /**
       The type responsible for converting
       v8 Value handles to ClassWrap_Types<T>::NativeHandle objects.
       The default implementation is designed to work with the
       other ClassWrap_xxx classes, specifically:

       - ClassWrap_Opt_InternalFields
       - ClassWrap_Types
    */
    template <typename T>
    struct ClassWrap_ToNative : ClassWrap_ToNative_StaticCast<T> {};


    /**
       The type responsible for converting
       ClassWrap_Types<T>::NativeHandle to v8::Objects. This
       instantiation will cause a compile-time error, as Native-to-JS
       is not possible in the generic case (it requires an extra level
       of binding info, which can be provided by a specialization).
    */
    template <typename T>
    struct ClassWrap_ToJS
    {
    public:
        typedef typename ClassWrap_Types<T>::Type Type;
        typedef typename ClassWrap_Types<T>::NativeHandle NativeHandle;
        /**
           Converts the given handle to a JS object. If it cannot it may:

           1) Optionally throw a JS exception.

           2) Return an empty handle.

           TODO: review whether throwing a JS exception from here is
           really desired.
        */
        static v8::Handle<v8::Object> Value( NativeHandle )
        {
            JUICE_STATIC_ASSERT(false,ClassWrap_T_ToJS_CannotWorkForTheGeneralCase);
        }
    };


    /**
       A policy class for ClassWrap, responsible for doing optional
       class-specific binding-related work as part of the JS/Native
       object construction process.

       The default specialization does nothing (which is okay for the
       general case) but defines the interface which specializations
       must implement.
    */
    template <typename T>
    struct ClassWrap_WeakWrap
    {
        typedef typename ClassWrap_Types<T>::NativeHandle NativeHandle;
        /**
           This operation is called one time from ClassWrap for each
           new object, directly after the native has been connected to
           a Persistent handle.
   
           Note that the ClassWrap code which calls this has already
           taken care of connecting nativeSelf to jsSelf. Client
           specializations of this policy may opt to add their own
           binding mechanisms, e.g. to allow CastToJS<T>() to work.

           Clients should do any bindings-related cleanup in
           ClassWrap_Memory::Destruct().
        */
        static void Wrap( v8::Persistent<v8::Object> jsSelf, NativeHandle nativeSelf )
        {
            return;
        }
    };

    /**
       A policy-based native-to-JS class binder.

       TODO: document it!
    */
    template <typename T>
    class ClassWrap : public JSClassCreator
    {
    public:
        /**
           Basic type info.
        */
        typedef ClassWrap_Types<T> TypeInfo;
        /**
           The real native wrapped type. In some cases is may differ
           from T!
         */
        typedef typename TypeInfo::Type Type;
        /**
           A native object handle, typically (T*) but _theoretically_
           could be a pointer-like wrapper.
        */
        typedef typename TypeInfo::NativeHandle NativeHandle;
        /**
           The JS-side class name.
        */
        typedef ClassWrap_ClassName<T> ClassName;
        /**
           The native ctor/dtor routines which are called from
           JS.
        */
        typedef ClassWrap_Memory<T> OpsMemory;

        /**
           The Native-to-JS cast operation. Not possible for the
           generic case.
        */
        //typedef ClassWrap_ToJS<T> CastToJS;

        /**
           The JS-to-Native cast operation.
           See ClassWrap-CastOps.h for an easy way to generate
           this for a given type.
        */
        typedef ClassWrap_ToNative<T> ToNative;
        
        /**
           This operation is called one time after a native has been
           connected to a Persistent handle. Client specializations of
           this policy may perform their own binding mechanisms,
           e.g. to allow Native-to-JS conversions to work.
        */
        typedef ClassWrap_WeakWrap<T> WeakWrap;
        /**
           Information about the "internal fields" (which holds native
           data inside the JS object representation). The fields are:

           - Value = the number of internal fields.

           - NativeIndex = the index at which the bound native object
           is stored within the list. Must be less than Value.
        */
        typedef ClassWrap_Opt_InternalFields<T> InternalFields;
        /**
           A boolean option specifying whether the JS code "Foo()"
           should be treated like "new Foo()". If false, the former will cause
           a JS exception.
        */
        typedef ClassWrap_Opt_CtorWithoutNew<T> AllowCtorWithoutNew;
    private:
        /** Checks a few static assertions . */
        static void check_assertions()
        {
            Detail::assert_internal_fields<InternalFields>();
        }
        /**
           Called by v8 when pv should be destructed. This function
           removes the native handle from the JS object, desposes of
           the persistent handle, and calls OpsMemory::Destruct() to
           destroy the object.
        */
	static void weak_callback(Persistent< Value > pv, void * nobj)
	{
	    //CERR << ClassName::Value()<<"::weak_callback(@"<<(void const *)nobj<<")\n";
            if( pv.IsEmpty() || ! pv->IsObject() )
            {
                CERR << "WARNING: ClassWrap<"<<ClassName::Value()<<">::weak_callback() was passed "
                     << "an empty or non-object handle!\n";
                return;
            }
	    Local<Object> jobj( Object::Cast(*pv) );
#if 0
	    if( jobj->InternalFieldCount() != (InternalFields::Value) )
            {
                CERR << "SERIOUS INTERNAL ERROR:\n"
                     << "ClassWrap<"<<ClassName::Value()<<">::weak_callback() was passed "
                     << "an object with an unexpected internal field count: "
                     << "JS="<<jobj->InternalFieldCount()
                     << ", Native="<<InternalFields::Value
                     << "\nSKIPPING DESTRUCTION! NOT DOING ANYTHING!!!\n"
                    ;
                return;
            }
#endif
#if 1
            NativeHandle nh = ToNative::Value( jobj );
            if( nh != nobj )
            {
                if( ! nh )
                {
                    /**
                       blithely assume that the native was removed via outside measures, e.g.
                       a call to a member Close() method.
                    */
                    if(0) CERR << "WARNING: ClassWrap<"<<ClassName::Value()<<">::weak_callback(): "
                               << "From JS=@"<<(void const *)nobj
                               << ", Converted to Native=@"<<(void const *)nh
                               << '\n'
                        ;
                    jobj->SetInternalField(InternalFields::NativeIndex,Null());
                    pv.Dispose();
                    pv.Clear();

                    return;
                }
                CERR << "SERIOUS INTERNAL ERROR:\n"
                     << "ClassWrap<"<<ClassName::Value()<<">::weak_callback() was passed "
                     << "two different values for the native object:\n"
                     << "From JS=@"<<(void const *)nobj
                     << ", Converted to Native=@"<<(void const *)nh
                     << "\nSKIPPING DESTRUCTION! NOT DOING ANYTHING!!!\n"
                    ;
                return;
            }
#endif
#if 0
	    Local<Value> lv( jobj->GetInternalField(InternalFields::NativeIndex) );
            //Local<External> ex( External::Cast(*pv) );
            void const * ext = lv.IsEmpty()
                ? 0
                //: ex->Value(); //v8::External::Unwrap(pv)
                : External::Unwrap(lv)
                ;
            if( ext != nobj )
            {
                CERR << "SERIOUS INTERNAL ERROR: ClassWrap::weak_callback() was passed "
                     << "two different values for the native object:\n"
                     << "JS=@"<<(void const *)ext
                     << ", "
                     << "Native=@"<<(void const *)nobj
                     << "\nSKIPPING DESTRUCTION! NOT DOING ANYTHING!!!\n"
                    ;
                return;
            }
#endif
            OpsMemory::Destruct( nh
                                 //static_cast<NativeHandle>(nobj)
                                 );
	    /**
	       We have to ensure that we have no dangling External in JS space. This
	       is so that functions like IODevice.close() can act safely with the
	       knowledge that member funcs called after that won't get a dangling
	       pointer. Without this, some code will crash in that case.
	    */
	    jobj->SetInternalField(InternalFields::NativeIndex,Null());
	    pv.Dispose();
	    pv.Clear();
	}

	static Persistent<Object> wrap_native( Handle<Object> _self, NativeHandle obj )
	{
            //CERR << ClassName::Value() <<"::wrap_native(@"<<(void const *)obj<<") Binding to internal field #"<<InternalFields::NativeIndex<<"\n";
	    Persistent<Object> self( Persistent<Object>::New(_self) );
	    self.MakeWeak( obj, weak_callback );
	    self->SetInternalField( InternalFields::NativeIndex,
                                    //External::New(obj)
                                    External::Wrap(obj)
                                    );
            WeakWrap::Wrap( self, obj );
	    return self;
	}

	static Handle<Value> ctor_proxy( const Arguments & argv )
	{
            if( AllowCtorWithoutNew::Value )
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
	    NativeHandle obj = NativeHandle(0);
            std::string err;
            {
                std::ostringstream errstr;
                try
                {
                    obj = OpsMemory::Instantiate( argv, errstr );
                }
                catch(std::exception const & ex)
                {
                    //return ThrowException(String::New(ex.what()));
                    return convert::CastToJS( ex );
                }
                catch(...)
                {
                    return ThrowException(String::New("Native constructor threw an unknown native exception!"));
                }
                err = errstr.str();
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
        /**
           Inititalizes the binding of T as a JS class which will
           become a member of the given target object.
        */
        explicit ClassWrap( Handle<Object> target)
	    : JSClassCreator( ClassName::Value(),
			      target,
			      ctor_proxy,
			      InternalFields::Value )
	{
            check_assertions();
	}
        /**
           Inititalizes the binding of T, but AddClassTo() must be
           used to install the class in a JS object.
        */
        ClassWrap()
	    : JSClassCreator( ClassName::Value(),
			      ctor_proxy,
			      InternalFields::Value )
	{
            check_assertions();
	}

        /**
           Destroys the given object by disconnecting its associated
           native object and calling the native destructor function
           for it.

           If jo cannot be converted to a NativeHandle then false is
           returned. Otherwise the true is returned, and the object
           referenced by jo is no longer valid and should not be used.

           Native functions bound to that object should take care to
           bail out with an exception once the native pointer is gone,
           as opposed to blindly stepping on its null/dangling pointer
           (which _might_ have been re-allocated to a different
           object, even of a different type, in the mean time).
        */
	static bool DestroyObject( Handle<Object> const & jo )
	{
	    NativeHandle t = ToNative::Value(jo);
	    if( ! t ) return false;
	    v8::Persistent<v8::Object> p( v8::Persistent<v8::Object>::New( jo ) );
	    weak_callback( p, t );
	    return true;
	}

        /**
           If jv is empty or !jv->IsObject() then 0 is returned,
           otherwise it returns the result of
           DestroyObject(Handle<Object>).
        */
        static bool DestroyObject( Handle<Value> const & jv )
	{
            return (jv.IsEmpty() || !jv->IsObject())
                ? 0
                : DestroyObject( Handle<Object>( Object::Cast(*jv) ) );
	}

        /**
           An InvocationCallback implementation which calls
           DestroyObject( argv.This() ).

           It is intended to be used as a "manual destructor" for
           classes which need it. The canonical examples are
           Stream.close() and Database.close().
        */
	static v8::Handle<v8::Value> DestroyObject( v8::Arguments const & argv )
	{
            return convert::CastToJS( DestroyObject(argv.This()) );
	}
        
#if 0
        /**
           Binds the native member function, specified as a template
           parameter, to a JS member function with the given
           name.
        */
        template < v8::Handle<Value> (Type::*Func)( v8::Arguments const & ) >
        ClassWrap & Set( char const * name )
        {
            this->Set(name, convert::InvocationCallbackMember<Type,Func>::Call );
            return *this;
        }
#endif

    };

} } // namespaces
