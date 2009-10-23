// EXPERIMENTAL/INCOMPLETE!
#include <sstream>
#include <v8/juice/static_assert.h>
#include <v8/juice/bind.h>
#include <v8/juice/convert.h>
#include <vector>

namespace v8 {
namespace juice {

#if !defined(DOXYGEN)
    namespace Detail
    {
        template <typename T1,typename T2>
        struct IsSameType
        {
            static const bool Value = false;
        };
        template <typename T1>
        struct IsSameType<T1,T1>
        {
            static const bool Value = 1;
        };
        template <typename T1,typename T2>
        struct HasParent
        {
            static const bool Value = true;
        };
        template <typename T1>
        struct HasParent<T1,T1>
        {
            static const bool Value = false;
        };
    }
#endif // DOXYGEN

    /**
       Base class for static ClassWrap options.
     */
    template <typename ValT, ValT Val>
    struct ClassWrap_Opt_ConstVal
    {
        typedef ValT Type;
        //static Type const Value = Val; // giving me an undefined ref error?
        static Type const Value;
    };
    template <typename ValT, ValT Val>
    const ValT ClassWrap_Opt_ConstVal<ValT,Val>::Value = Val;

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
       EXPERIMENTAL/INCOMPLETE!
       
       A base implementation for ClassWrap_Inheritance policy
       specializations, which may subclass this type, passing some
       base type of T as the ParentType_ parameter. This adds ClassWrap
       inheritance information, which may someday be used to help support
       cross-JS/Native inheritance features.
    */
    template <typename T,typename ParentType_ = T>
    struct ClassWrap_Inheritance_Base
    {
        //! Not required to be defined by specializations.
        typedef convert::TypeInfo<T> TypeInfo;
        //! Not required to be defined by specializations.
        typedef convert::TypeInfo<ParentType_> ParentTypeInfo;
        //! Required to be defined by specializations.
        typedef typename TypeInfo::Type Type;
        //! Required to be defined by specializations.
        typedef typename ParentTypeInfo::Type ParentType;
        //! Required to be defined by specializations.
        static const bool HasParent = Detail::HasParent<Type,ParentType>::Value;
    private:
    protected:
        /**
           Tries to statically assert that (T*) can be implicitly
           converted to (ParentType*) (which implies that T and
           ParentType are the same class or T subclasses ParentType).
        */
        ClassWrap_Inheritance_Base()
        {
            const typename ParentTypeInfo::NativeHandle * P = 0;
            const typename TypeInfo::NativeHandle * X = 0;
            /**
               If a compiler error led you here, it means that T is
               neither ParentType nor derived from ParentType.
            */
            P = /* See the comments above!!! */ X;
        }
    };

    //! Experimental/incomplete.
    template <typename T>
    struct ClassWrap_Inheritance : ClassWrap_Inheritance_Base<T,T>
    {};

    
    /**
       A ClassWrap policy class for defining a class name to the JS
       counterpart of T.
    */
    template <typename T>
    struct ClassWrap_ClassName
    {
        /**
           Must return a valid, unqualified JS class name string. e.g.
           "MyClass" is legal but neither null nor "my.Class" are.

           The default implementation is useless, and will result in
           a compile-time error if static assertions are enabled.
         */
        static char const * Value()
        {
            JUICE_STATIC_ASSERT(false,
                                ClassWrap_ClassName_MustBeSpecialized);

        }
    };

    /**
       ClassWrap option to set the number of internal fields reserved for
       JS objects. It must be greater than 0 and greater than NativeIndex.
    */
    template <typename T>
    struct ClassWrap_InternalFields : ClassWrap_Opt_Int<1>
    {
        /**
           The internal field index at which ClassWrap policies should
           expect the native object to be found in any give JS object.
           It must be 0 or greater, and must be less than Value.
        */
        static const int NativeIndex = 0;
    };

    namespace Detail
    {
        /**
           Performs a compile-time assertion for the parameters of the
           given type, which must conform to the
           ClassWrap_InternalFields interface.
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
       A ClassWrap policy/options class responsible specifying whether
       or not a ClassWrap-bound class should allow "Foo()" and "new
       Foo()" to behave the same or not. If the Value member is false
       (the default) then "Foo()" is not allowed to behave as a
       constructor call (it will generate an error), otherwise it will
       be treated exactly as if "new Foo()" had been called.
    */
    template <typename T>
    struct ClassWrap_AllowCtorWithoutNew : ClassWrap_Opt_Bool<false>
    {};

    /**
       The ClassWrap Policy class responsible for instantiating and
       destroying convert::TypeInfo<T>::NativeHandle objects.
    */
    template <typename T>
    struct ClassWrap_Factory
    {
        /**
           Specifies the templatized type.
        */
        typedef typename convert::TypeInfo<T>::Type Type;
        /**
           Specifies the "native handle" typed used for
           passing around native objects.
        */
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
	/**
	   This will be called in response to calling
	   "new WrappedType(...)" in JS code. It should:

	   - Create a new native object.

	   - If successful, return that object.

	   - On error, return 0 and optionally populate the (ostream &)
	   argument with an informative error message (e.g. "requires
	   arguments (String,Integer)").

	   If the client is using any "supplemental" garbage
	   collection (e.g. the v8::juice::cleanup API) or class
	   binding mechanism, then they should register the object in
	   this function and deregister it in Destruct().
	*/
	static NativeHandle Instantiate( Arguments const &  /*argv*/,
                                         std::ostream & exceptionText )
	{
            JUICE_STATIC_ASSERT(false,ClassWrap_Factory_T_MustBeSpecialized);
	    exceptionText << "ClassWrap_Factory<"
                          << ClassWrap_ClassName<T>::Value()
                          << ">::Instantiate() not implemented!";
	    return NativeHandle(0);
	}
	/**
	   Must "destroy" the given object, though the exact meaning
	   of "destroy", and mechanism of destruction, is
	   type-specific.

	   This function is responsible for destroying the native
	   object. Most implementations must simply call "delete obj",
	   which is fine for many types, but opaque types and types
	   allocated via something other than 'new' will need a
	   different implementation. A destructor functor for shared,
	   static, or stack-allocated objects should simply do nothing
	   (or maybe clear the object to some empty/default state).

	   If the client is using any "supplemental" garbage
	   collection (e.g. the v8::juice::cleanup API) or class
	   binding mechanism then they should deregister the object in
	   this function.
	*/
	static void Destruct( NativeHandle obj )
	{
            JUICE_STATIC_ASSERT(false,ClassWrap_Factory_T_MustBeSpecialized);
	}

        /**
           Should be the approximate amount of native memory allocated
           to each bound object. The purpose of this is to allow
           ClassWrap to tell v8 about the memory cost (a metric it
           theoretically uses in figuring out when to garbage
           collect). It need not be a definitive value, and sizeof(T)
           is a suitable value for most cases. It is legal to be 0, in
           which case no memory adjustment is made.

           Note that it is a const, and not a function, because the
           sizes _must_ be the same at construction and destruction.
        */
        static const size_t AllocatedMemoryCost = 0;
    };

    /**
       A ClassWrap policy option class used by certain JS-to-Native
       conversions to determine whether they should search the
       prototype object chain for the native object if it is not found
       in the current JS object. Such a lookup is required for finding
       the native if a JS class inherits the bound class.

       See ClassWrap_ToNative_StaticCast<> for a case which uses this
       option.

       Note, however, that JS-side inheritance breaks certain types of
       operations.
    */
    template <typename T>
    struct ClassWrap_ToNative_SearchPrototypesForNative : ClassWrap_Opt_Bool<false>
    {};

    /**
       A ClassWrap_Extract policy base class for "unwrapping" JS
       object handles, extracting their native bound data.

       This implementation should be suitable for most, if not all,
       cases.

       The ClassWrap_Extract interface is responsible for:

       - Extracting (void*) handles from JS object.

       - Converting (void*) to native handles.

       But this base only implements the first part, leaving the
       second part to concrete subclasses.

       ClassWrap_Extract acts as the basis of ClassWrap_ToNative.
    */
    template <typename T>
    struct ClassWrap_Extract_Base
    {
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
        /**
           Required part of the ClassWrap_Extract interface.
           
           Looks for a JS value stored in the N'th internal field of
           the given object (where N ==
           ClassWrap_InternalFields<T>::NativeIndex).

           If it finds one, it the value is extracted to get its
           native (void *). If one is found it is returned.

           Ownership of the returned object is not modified by this
           call.

           If searchPrototypes is true and any of the following apply:

           * (jo->InternalFieldCount() != ClassWrap_InternalFields<T>::Value)

           * The field count matches, but no native is found at that index

           Then this function is called recursively on
           jo->GetPrototype(). This feature is necessary if JS-side
           classes must inherit the bound class, as without it they
           cannot find their native members. But it also doesn't work
           intuitively in conjunction with certain types of inherited
           bound functions.
        */
        static void * ExtractVoid( v8::Handle<v8::Object> const & jo,
                                   bool searchPrototypes )
        {
            if( jo.IsEmpty() ) return 0;
            typedef ClassWrap_InternalFields<T> IFT;
            { static bool inited = (Detail::assert_internal_fields<IFT>(),true); }
            void * ext = 0;
            if( jo->InternalFieldCount() == IFT::Value )
            {
                ext = jo->GetPointerFromInternalField( IFT::NativeIndex );
            }
            if( searchPrototypes && !ext )
            {
                //CERR << "Searching in prototype chain for "<<ClassWrap_ClassName<T>::Value()<<"...\n";
                v8::Local<v8::Value> proto = jo->GetPrototype();
                if( !proto.IsEmpty() && proto->IsObject() )
                {
                    return ExtractVoid( v8::Local<v8::Object>( v8::Object::Cast( *proto ) ), true );
                }
            }
            return ext;
        }

    };
    /**
       A concrete ClassWrap_Extract implementation which uses
       static_cast() to convert between (void*) and (T*).
    */
    template <typename T>
    struct ClassWrap_Extract_StaticCast : ClassWrap_Extract_Base<T>
    {
        typedef typename convert::TypeInfo<T>::Type Type;
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;

        /**
           Required part of the ClassWrap_Extract
           interface. Implementations must, if possible, return a
           NativeHandle equivalent for x. If it is not possible, they
           must return 0.

           This implementation returns static_cast<NativeHandle>(x).

           Specializations are free to use the (void*) as a lookup key
           to find, in a type-safe manner, an associated native.
        */
        static NativeHandle VoidToNative( void * x )
        {
            return x ? static_cast<NativeHandle>( x ) : 0;
        }
    };
    /**
       A ClassWrap policy for "unwrapping" JS object handles, extracting
       their native bound data.

       The default implementation should be suitable for most cases.
    */
    template <typename T>
    struct ClassWrap_Extract : ClassWrap_Extract_StaticCast<T>
    {};

    /**
       A base class for ClassWrap_ToNative implementations. The template arguments are:

       - T = the native type being bind.

       - ExtractPolicy must conform to the ClassWrap_Extract<T> interface, and is used
       for extracting (void*) from JS object.s

       - SearchPrototypesForNative_ is used as the second parameter to
       ExtractPolicy::ExtractVoid().

       
    */
    template <typename T,
              typename ExtractPolicy = ClassWrap_Extract<T>,
              bool SearchPrototypesForNative_ = ClassWrap_ToNative_SearchPrototypesForNative<T>::Value
              >
    struct ClassWrap_ToNative_Base
    {
        typedef typename convert::TypeInfo<T>::Type Type;
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
        /**
           The SearchPrototypesForNative_ template parameter.  See
           Value() for the description.
        */
        static const bool SearchPrototypesForNative = SearchPrototypesForNative_;
        /**
           Uses
           ClassWrap_Extract<T>::ExtractVoid(jo,SearchPrototypesForNative)
           to extract a native pointer. If it finds one, it returns
           VoidToNative(ptr), else it returns 0.

           Ownership of the returned object is not modified by this
           call.
        */
        static NativeHandle Value( v8::Handle<v8::Object> const jo )
        {
            void * ext = ExtractPolicy::ExtractVoid( jo, SearchPrototypesForNative );
            return ext
                ? ExtractPolicy::VoidToNative( ext )
                : 0;
        }
    };
    
    /**
       A concrete ClassWrap_ToNative<T> policy implementation which
       uses static_cast<convert::TypeInfo<T>::NativeHandle>(void*) to
       convert objects from JS to native space. It takes a certain,
       but not infallible, deal of care in ensuring that the (void*)
       is actually a native object of the proper type.

       See the Value() member for more details, including the meaning
       of the SearchPrototypesForNative_ parameter.
    */
    template <typename T,
              bool SearchPrototypesForNative_ = ClassWrap_ToNative_SearchPrototypesForNative<T>::Value
              >
    struct ClassWrap_ToNative_StaticCast
        : ClassWrap_ToNative_Base< T, ClassWrap_Extract_StaticCast<T>, SearchPrototypesForNative_ >
    {
    };

    /**
       The ClassWrap policy type responsible for converting
       v8 Value handles to convert::TypeInfo<T>::NativeHandle objects.
       The default implementation is designed to work with the
       other ClassWrap_xxx classes, specifically:

       - ClassWrap_InternalFields<T>
       - TypeInfo<T>

       This class can (and should) be used to implement a
       specialization of convert::JSToNative<T>.
    */
    template <typename T>
    struct ClassWrap_ToNative : ClassWrap_ToNative_StaticCast<T> {};

    /**
       The type responsible for converting
       convert::TypeInfo<T>::NativeHandle to v8::Objects. This
       instantiation will cause a compile-time error, as Native-to-JS
       is not possible in the generic case (it requires an extra level
       of binding info, which can be provided via custom ClassWrap
       policy specializations).
    */
    template <typename T>
    struct ClassWrap_ToJS
    {
    public:
        typedef typename convert::TypeInfo<T>::Type Type;
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
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
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
        /**
           This operation is called one time from ClassWrap for each
           new object, directly after the native has been connected to
           a Persistent handle.
   
           Note that the ClassWrap code which calls this has already
           taken care of connecting nativeSelf to jsSelf. Client
           specializations of this policy may opt to add their own
           binding mechanisms, e.g. to allow CastToJS<T>() to work.

           Clients should do any bindings-related cleanup in
           ClassWrap_Factory::Destruct().
        */
        static void Wrap( v8::Persistent<v8::Object> jsSelf, NativeHandle nativeSelf )
        {
            return;
        }
    };

    /**
       A utility function primarily intended to support various
       ClassWrap policy implementations.

       This function tries to extract a native from jo using ClassWrap_Extract<T>.
       If a native is found, and ClassWrap_Extract<T>::VoidToNative() says that it is
       the same as nh, then jo is returned. If none is found, jo's prototype object
       is search, recursively, until either nh is found in the prototype chain
       or the end of the chain is reached. If a match is found, the object
       in which the data were found is returned.

       If nh is found nowhere in the chain, an empty handle is returned.
    */
    template <typename T>
    v8::Handle<v8::Object> ClassWrap_FindHolder( v8::Handle<v8::Object> jo,
                                                 typename convert::TypeInfo<T>::NativeHandle nh )
    {
        if( jo.IsEmpty() || ! jo->IsObject() ) return v8::Handle<v8::Object>();
        typedef convert::TypeInfo<T> TI;
        typedef typename TI::Type Type;
        typedef typename TI::NativeHandle NH;
        typedef ClassWrap_Extract<T> XT;
        void * ext = XT::ExtractVoid( jo, false );
        if( ! ext )
        {
            v8::Local<v8::Value> proto = jo->GetPrototype();
            if( !proto.IsEmpty() && proto->IsObject() )
            {
                return ClassWrap_FindHolder<T>( v8::Local<v8::Object>( v8::Object::Cast( *proto ) ), nh );
            }
        }
        if( ext == nh ) return jo; // shortcut
        NH xnh = XT::VoidToNative( ext );
        if( xnh != nh )
        { // bound native, but the wrong one
            v8::Local<v8::Value> proto = jo->GetPrototype();
            if( !proto.IsEmpty() && proto->IsObject() )
            {
                return ClassWrap_FindHolder<T>( v8::Local<v8::Object>( v8::Object::Cast( *proto ) ), nh );
            }
        }
        return v8::Handle<v8::Object>();
    }

    
    /**
       A policy-based native-to-JS class binder, such that T
       object can be accessed via JS. This class is only
       a very thin wrapper around:

       - JSClassCreator (its base class)
       - Various policy classes named ClassWrap_XXX.

       The base class handles the more mundane details of creating a
       new JS class and this type adds weak pointer binding to
       that. The T-specific policy classes define how certain aspects
       of the binding are handled, e.g. how to convert a JS object
       handle back into a T object.

       Requirements for T:

       - Must be a complete type. i think.
       - ClassWrap_ClassName<T> must be implemented.
       - ClassWrap_Factory<T> must be implemented.
       - ClassWrap_ToNative<T> must be implemented.

       The other ClassWrap_XXX polcies can normally be defaulted, but
       may also be specialized to change certain aspects of the
       binding.


       At least the following specializations of
       other classes must be implemented:

       - ClassWrap_ClassName<T>
       - ClassWrap_Factory<T>

       Other ClassWrap_XXX<T> specializations may (or may have to) be
       implemented, depending the needs of the binding.


       BUGS AND SIGNIFICANT CAVEATS:

       Inheriting a wrapped class from the JS side will not work
       properly for all operations, but work is still underway in this
       area.
    */
    template <typename T>
    class ClassWrap : public JSClassCreator
    {
    public:
        /**
           Basic type info.
        */
        typedef convert::TypeInfo<T> TypeInfo;
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
        typedef ClassWrap_Factory<T> Factory;

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
        typedef ClassWrap_InternalFields<T> InternalFields;

        /**
           A boolean option specifying whether the JS code "Foo()"
           should be treated like "new Foo()". If false, the former will cause
           a JS exception.
        */
        typedef ClassWrap_AllowCtorWithoutNew<T> AllowCtorWithoutNew;

        /**
           Convenience typedef for accessing a property binder utility
           class.
        */
        typedef ::v8::juice::convert::PropertyBinder<T> PB;
        
    private:
        /** Checks a few static assertions. */
        static void check_assertions()
        {
            Detail::assert_internal_fields<InternalFields>();
        }
        /**
           Called by v8 when pv should be destructed. This function
           removes the native handle from the JS object, desposes of
           the persistent handle, and calls Factory::Destruct() to
           destroy the object.
        */
	static void weak_callback(Persistent< Value > pv, void * nobj)
	{
	    CERR << ClassName::Value()<<"::weak_callback(@"<<(void const *)nobj<<")\n";
            if( pv.IsEmpty() || ! pv->IsObject() )
            {
                CERR << "WARNING: ClassWrap<"<<ClassName::Value()<<">::weak_callback() was passed "
                     << "an empty or non-object handle!\n";
                return;
            }
            //pv.ClearWeak(); // try to avoid duplicate calls caused by DestroyObject()-like features
	    Local<Object> jobj( Object::Cast(*pv) );
#if 1
            NativeHandle nh = ToNative::Value( jobj );
            if( nh != nobj )
            {
                if( ! nh )
                {
                    /**
                       Blithely assume that the native was removed via outside measures, e.g.
                       a call to a member Close() method.

                       When we trigger this function via
                       DestroyObject() (or similar), i'm finding that
                       v8 later calls this function again (at the
                       proper cleanup time), but the native has been
                       unbound and triggers this condition.

                       [Later on...]

                       i believe the above-mentioned dual calls
                       condition has been fixed, but need to test it
                       more before removing this if() block.
                    */
                    if(1) CERR << "ACHTUNG: ClassWrap<"<<ClassName::Value()<<">::weak_callback(): "
                               << "From JS=@"<<(void const *)nobj
                               << ", Converted to Native=@"<<(void const *)nh
                               << '\n'
                        ;
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
	    /**
	       We have to ensure that we have no dangling External in JS space. This
	       is so that functions like IODevice.close() can act safely with the
	       knowledge that member funcs called after that won't get a dangling
	       pointer. Without this, some code will crash in that case.
	    */

            /**
               Reminder: the ClassWrap_FindHolder() bits are here to
               assist when the bound native exists somewhere in the
               prototype chain other than jobj itself. In that case,
               jobj is valid but we cannot clear out the native handle
               internal field on it because it has no internal fields
               (or none that belong to us).

               To fix this properly we have to be able to know
               _exactly_ which JS object in the prototype chain nh is
               bound to.
            */
            v8::Handle<v8::Object> nholder = ClassWrap_FindHolder<Type>( jobj, nh );
            if( nholder.IsEmpty() || (nholder->InternalFieldCount() != InternalFields::Value) )
            {
                CERR << "SERIOUS INTERNAL ERROR:\n"
                     << "ClassWrap<"<<ClassName::Value()<<">::weak_callback() "
                     << "validated that the JS/Native belong together, but "
                     << "ClassWrap_FindHolder() returned an empty handle!\n"
                     << "From JS=@"<<(void const *)nobj
                     << ", Converted to Native=@"<<(void const *)nh
                     << "\nTHIS MAY LEAD TO A CRASH IF THIS JS HANDLE IS USED AGAIN!!!\n"
                    ;
                Factory::Destruct( nh );
                return;
            }
            else
            {
                /**
                   Reminder: we call Destruct() before cleaning up the
                   JS-side fields for the case that the bound class actually
                   does tinker with the JS space directly.
                */
                Factory::Destruct( nh );
                nholder->SetInternalField(InternalFields::NativeIndex,Null());
                v8::V8::AdjustAmountOfExternalAllocatedMemory( -static_cast<int>( Factory::AllocatedMemoryCost ) );
            }
	}

        /**
           Makes a weak pointer from _self and sets obj as the N's internal field
           of _self, where N is InternalFields::NativeIndex. Returns the new
           weak pointer handle.
        */
	static Persistent<Object> wrap_native( Handle<Object> _self, NativeHandle obj )
	{
            //CERR << ClassName::Value() <<"::wrap_native(@"<<(void const *)obj<<") Binding to internal field #"<<InternalFields::NativeIndex<<"\n";
            v8::V8::AdjustAmountOfExternalAllocatedMemory( static_cast<int>( Factory::AllocatedMemoryCost ) );
	    Persistent<Object> self( Persistent<Object>::New(_self) );
	    self.MakeWeak( obj, weak_callback );
	    self->SetPointerInInternalField( InternalFields::NativeIndex, obj );
            WeakWrap::Wrap( self, obj );
	    return self;
	}

        /**
           Tries to instantiate a new NativeHandle object by calling
           Factory::Instantiate(). On success, the returned handle
           points to a weak-pointer handle to that object. On error
           a JS exception is triggered and the return value is
           theoretically empty.
        */
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
                    obj = Factory::Instantiate( argv, errstr );
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
		    Factory::Destruct(obj);
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

           @see v8::juice::JSClassCreator::JSClassCreator()
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

           @see v8::juice::JSClassCreator::JSClassCreator()
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
           returned. Otherwise the true is returned and the native
           object referenced by jo is no longer valid (it should not
           be used by JS code).

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
            p.ClearWeak(); // avoid a second call to weak_callback() via gc!
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
