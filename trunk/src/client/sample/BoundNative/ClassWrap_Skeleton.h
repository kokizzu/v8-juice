#if !defined(V8_JUICE_CLASSWRAP_POLICY_SKELETON_INCLUDED)
#define V8_JUICE_CLASSWRAP_POLICY_SKELETON_INCLUDED

/** @file ClassWrap_Skeleton.h
   This file has two uses:

   1) When included "normally", it defines a ClassWrap factory policy
   which uses the new/delete operators for object
   construction/destrution, and otherwise relies on the default
   ClassWrap policies all other functionality.

   2) To act as a supermacro for use by clients which wish to use
   these policies, as is demonstrated here:

   @code
   // From global scope:
   #define CLASSWRAP_BOUND_TYPE MyType
   #define CLASSWRAP_BOUND_TYPE_NAME "MyType"
   #include <v8/juice/ClassWrap_Skeleton.h>
   @endcode

   That will install the generated policies as the defaults for
   ClassWrap<CLASSWRAP_BOUND_TYPE>, and then CLASSWRAP_BOUND_TYPE and
   CLASSWRAP_BOUND_TYPE_NAME will be undefined (so that this file can
   be directly included again).

   Defining CLASSWRAP_BOUND_TYPE_NAME is optional, but if it is not done
   then one must provide his own ClassWrap_ClassName<CLASSWRAP_BOUND_TYPE>
   specialization.

   The following ClassWrap policies are set up:

   - ClassName if CLASSWRAP_BOUND_TYPE_NAME is set.
   - ToNative
   - Factory

   Requirements on CLASSWRAP_BOUND_TYPE:

   - Must be a complete type.

   - (new CLASSWRAP_BOUND_TYPE) must be legal.

   - (delete convert::TypeInfo<CLASSWRAP_BOUND_TYPE>::NativeHandle) must be legal.

*/
namespace v8 { namespace juice {
    /**
       A concrete ClassWrap_Factory implementation which
       uses new/delete to create/destroy objects, but requires
       a default constructor.
    */
    template <typename T>
    struct ClassWrap_Factory_Skeleton
    {
        /** Required by ClassWrap_Factory interface. */
        typedef typename convert::TypeInfo<T>::Type Type;
        /** Required by ClassWrap_Factory interface. */
        typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
        /**
           Returns (new Type).
        */
	static NativeHandle Instantiate( Arguments const &  /*argv*/,
                                         std::ostream & exceptionText )
	{
            return new Type;
        }
        /**
           Calls (delete obj).
        */
	static void Destruct( v8::Handle<v8::Object> /*ignored*/, NativeHandle obj )
	{
            delete obj;
        }
        static const size_t AllocatedMemoryCost = sizeof(Type);
    };
} }
#endif // V8_JUICE_CLASSWRAP_POLICY_SKELETON_INCLUDED

#if defined(CLASSWRAP_BOUND_TYPE)
namespace v8 { namespace juice {

    template <>
    struct ClassWrap_Factory< CLASSWRAP_BOUND_TYPE > :
        ClassWrap_Factory_Skeleton< CLASSWRAP_BOUND_TYPE > {};

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
