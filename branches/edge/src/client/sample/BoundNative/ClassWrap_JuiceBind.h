#if !defined(V8_JUICE_CLASSWRAP_POLICY_JUICEBIND_INCLUDED)
#define V8_JUICE_CLASSWRAP_POLICY_JUICEBIND_INCLUDED
#include <v8/juice/bind.h>
/** @file ClassWrap_JuiceBind.h
   This file has two uses:

   1) When included "normally", it defines a set of ClassWrap policies
   which work together to provide JS-to-native conversions for the
   ClassWrap type.

   2) To act as a supermacro for use by clients which wish to use
   these policies, as is demonstrated here:


   @code
   // From global scope:
   #define CLASSWRAP_BOUND_TYPE MyType
   #define CLASSWRAP_BOUND_TYPE_NAME "MyType"
   #include <v8/juice/ClassWrap_JuiceBind.h>
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
   - ClassName if CLASSWRAP_BOUND_TYPE_NAME is set.


   If CLASSWRAP_BOUND_TYPE is defined:
   
   This file also sets up JSToNative specialization which use the
   ToNative policy.

   If the following policies will be customized by the client, the
   specializations must visible from this file! i.e. they must be
   defined before including this file.

   - ClassWrap_ToNative_SearchPrototypesForNative<T>
   - ClassWrap_InternalFields<T> 
   
*/
namespace v8 { namespace juice {

    /**
       A concrete ClassWrap_WeakWrap policy which uses the v8::juice::bind
       API to register objects for type-safe lookups later on.
    */
    template <typename T>
    struct ClassWrap_WeakWrap_JuiceBind
    {
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
        /**
           Calls v8::juice::bind::BindNative(nativeSelf).
        */
        static void Wrap( v8::Persistent<v8::Object> /*jsSelf*/, NativeHandle nativeSelf )
        {
            v8::juice::bind::BindNative( nativeSelf );
            return;
        }
    };

    /**
       A concrete ClassWrap_Extract policy which uses the
       v8::juice::bind API to extract, type-safely, native objects
       from JS object.
    */

    template <typename T>
    struct ClassWrap_Extract_JuiceBind : ClassWrap_Extract_Base<T>
    {
        typedef typename convert::TypeInfo<T>::Type Type;
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
        /**
           Returns v8::juice::bind::GetBoundNative<Type>(x).
        */
        static NativeHandle VoidToNative( void * x )
        {
            return x ? v8::juice::bind::GetBoundNative<Type>( x ) : 0;

        }
    };

    /**
       A concrete ClassWrap_ToNative policy which uses the v8::juice::bind
       API to extract, type-safely, native objects from JS object.

       Requires:

       - ClassWrap_WeakWrap<T> == ClassWrap_WeakWrap_JuiceBind<T>
       - ClassWrap_ToNative_SearchPrototypesForNative<T>

       So if those policies must be specialized for T, they must be specialized
       before this code is visible.
    */
    template <typename T>
    struct ClassWrap_ToNative_JuiceBind
        : ClassWrap_ToNative_Base<T, ClassWrap_Extract_JuiceBind<T> >
    {
    };


} }
#endif // V8_JUICE_CLASSWRAP_POLICY_JUICEBIND_INCLUDED

#if defined(CLASSWRAP_BOUND_TYPE)
namespace v8 { namespace juice {

    template <>
    struct ClassWrap_WeakWrap< CLASSWRAP_BOUND_TYPE > :
        ClassWrap_WeakWrap_JuiceBind< CLASSWRAP_BOUND_TYPE > {};

    template <>
    struct ClassWrap_Extract< CLASSWRAP_BOUND_TYPE > :
        ClassWrap_Extract_JuiceBind< CLASSWRAP_BOUND_TYPE > {};

    template <>
    struct ClassWrap_ToNative< CLASSWRAP_BOUND_TYPE > :
        ClassWrap_ToNative_JuiceBind< CLASSWRAP_BOUND_TYPE > {};

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
    
} }
#include "ClassWrap-JSToNative.h" // will undefine CLASSWRAP_BOUND_TYPE
#endif //CLASSWRAP_BOUND_TYPE
