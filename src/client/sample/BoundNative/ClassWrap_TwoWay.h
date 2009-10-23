#if !defined(V8_JUICE_CLASSWRAP_TWOWAY_INCLUDED)
#define V8_JUICE_CLASSWRAP_TWOWAY_INCLUDED
/** @file ClassWrap_TwoWay.h
   This file has two uses:

   1) When included "normally", it defines a set of ClassWrap policies
   which work together to provide JS/Native conversions for the
   ClassWrap type.

   2) To act as a supermacro for use by clients which wish to use
   these policies, as is demonstrated here:


   @code
   // From global scope:
   #define CLASSWRAP_BOUND_TYPE MyType
   #define CLASSWRAP_BOUND_TYPE_NAME "MyType"
   #include <v8/juice/ClassWrap_TwoWay.h>
   @endcode

   That will install these policies as the defaults for
   ClassWrap<CLASSWRAP_BOUND_TYPE>, and then CLASSWRAP_BOUND_TYPE and
   CLASSWRAP_BOUND_TYPE_NAME will be undefined (so that this file can
   be directly included again).
   
   Defining CLASSWRAP_BOUND_TYPE_NAME is optional, but if it is not done
   then one must provide his own ClassWrap_ClassName<CLASSWRAP_BOUND_TYPE>
   specialization.

   The following ClassWrap policies are set up:

   - WeakWrap
   - Extract
   - ToNative
   - ToJS
   - ClassName if CLASSWRAP_BOUND_TYPE_NAME is set.

   This file also sets up JSToNative and NativeToJS specializations
   which use the ToNative/ToJS policies.
   
   If the following policies will be customized by the client, the
   specializations must visible from this file! i.e. they must be
   defined before including this file.

   - ClassWrap_ToNative_SearchPrototypesForNative<T>
   - ClassWrap_InternalFields<T> 
   
*/
namespace v8 { namespace juice {
    namespace Detail
    {
	/**
	   An internal helper type for only use by certain ClassWrap
	   policy classes.
	*/
	template <typename T>
	struct ClassWrapMapper
	{
            typedef ::v8::juice::convert::TypeInfo<T> TypeInfo;
            typedef typename TypeInfo::Type Type;
            typedef typename TypeInfo::NativeHandle NativeHandle;
            typedef v8::Persistent<v8::Object> JSObjHandle;
            //typedef v8::Handle<v8::Object> JSObjHandle;
            typedef std::pair<NativeHandle,JSObjHandle> ObjBindT;
	    typedef std::map<void const *, ObjBindT> OneOfUsT;
            typedef typename OneOfUsT::iterator Iterator;
	    static OneOfUsT & Map()
	    {
		static OneOfUsT bob;
		return bob;
            }

            /** Maps obj as a lookup key for jself. Returns false if !obj,
             else true. */
            static bool Insert( //v8::Handle<v8::Object> const & jself,
                                JSObjHandle const & jself,
                                NativeHandle obj )
            {
                return obj
                    ? (Map().insert( std::make_pair( obj, std::make_pair( obj, jself ) ) ),true)
                    : 0;
            }

            /**
               Removes any mapping of the given key. Returns the
               mapped native, or 0 if none is found.
            */
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
                return victim;
            }

            /**
               Returns the native associated (via Insert())
               with key, or 0 if none is found.
            */
            static NativeHandle GetNative( void const * key )
            {
                if( ! key ) return 0;
                else
                {
                    typename OneOfUsT::iterator it = Map().find(key);
                    return (Map().end() == it)
                        ? 0
                        : (*it).second.first;
                }
            }

            /**
               Returns the JS object associated with key, or
               an empty handle if !key or no object is found.
            */
            static Handle<Object> GetJSObject( void const * key )
	    {
                if( ! key ) return Handle<Object>();
                typename OneOfUsT::iterator it = Map().find(key);
                if( Map().end() == it ) return Handle<Object>();
                else return (*it).second.second;
	    }
        };

    }
    
    /**
       A concrete ClassWrap_WeakWrap policy which uses an internal
       native/js mapping for type-safe lookups later on.
    */
    template <typename T>
    struct ClassWrap_WeakWrap_TwoWayBind
    {
        typedef typename convert::TypeInfo<T>::Type Type;
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;

        /**
           Calls v8::juice::bind::BindNative(nativeSelf).
        */
        static void Wrap( v8::Persistent<v8::Object> jsSelf, NativeHandle nativeSelf )
        {
            typedef Detail::ClassWrapMapper<T> Mapper;
            Mapper::Insert( jsSelf, nativeSelf );
            return;
        }
    };

    /**
       A concrete ClassWrap_ToNative policy which uses a type-safe
       conversion.
    */
    template <typename T>
    struct ClassWrap_Extract_TwoWayBind : ClassWrap_Extract_Base<T>
    {
        typedef typename convert::TypeInfo<T>::Type Type;
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
        /**
        */
        static NativeHandle VoidToNative( void * x )
        {
            typedef Detail::ClassWrapMapper<T> Mapper;
            return x ? Mapper::GetNative(x) : 0;
        }
    };

    /**
       A concrete ClassWrap_ToNative policy which uses an internal API
       to extract, type-safely, native objects from JS object.

       Requires:

       - ClassWrap_WeakWrap<T> == ClassWrap_WeakWrap_TwoWayBind<T>
    */
    template <typename T>
    struct ClassWrap_ToNative_TwoWayBind
        : ClassWrap_ToNative_Base<T, ClassWrap_Extract_TwoWayBind<T> >
    {
    };
    
    template <typename T>
    struct ClassWrap_ToJS_TwoWayBind
    {
    public:
        typedef typename convert::TypeInfo<T>::Type Type;
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
        /**
        */
        static v8::Handle<v8::Value> Value( NativeHandle nh )
        {
            typedef Detail::ClassWrapMapper<T> Mapper;
            return Mapper::GetJSObject( nh );
        }
    };

} }
#endif // V8_JUICE_CLASSWRAP_TWOWAY_INCLUDED

#if defined(CLASSWRAP_BOUND_TYPE)
namespace v8 { namespace juice {

    template <>
    struct ClassWrap_WeakWrap< CLASSWRAP_BOUND_TYPE > :
        ClassWrap_WeakWrap_TwoWayBind< CLASSWRAP_BOUND_TYPE > {};

    template <>
    struct ClassWrap_Extract< CLASSWRAP_BOUND_TYPE > :
        ClassWrap_Extract_TwoWayBind< CLASSWRAP_BOUND_TYPE > {};

    template <>
    struct ClassWrap_ToNative< CLASSWRAP_BOUND_TYPE > :
        ClassWrap_ToNative_TwoWayBind< CLASSWRAP_BOUND_TYPE > {};

    template <>
    struct ClassWrap_ToJS< CLASSWRAP_BOUND_TYPE > :
        ClassWrap_ToJS_TwoWayBind< CLASSWRAP_BOUND_TYPE > {};

#if defined(CLASSWRAP_BOUND_TYPE_NAME)
    template <>
    struct ClassWrap_ClassName< CLASSWRAP_BOUND_TYPE >
    {
        static char const * Value()
        {
            return CLASSWRAP_BOUND_TYPE_NAME;
        }
    };
#undef CLASSWRAP_BOUND_TYPE_NAME
#endif
    
    namespace convert
    {
        template <>
        struct NativeToJS< CLASSWRAP_BOUND_TYPE >
        {
            typedef ::v8::juice::ClassWrap_ToJS< CLASSWRAP_BOUND_TYPE > Cast;
            typedef Cast::NativeHandle NativeHandle;
            v8::Handle<v8::Value> operator()( NativeHandle p ) const
            {
                return p ? Cast::Value(p) : v8::Handle<v8::Value>();
            }
        };
    }
   
} }
#include "ClassWrap-JSToNative.h" // will undefine CLASSWRAP_BOUND_TYPE
#endif //CLASSWRAP_BOUND_TYPE
