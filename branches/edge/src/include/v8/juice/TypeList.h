#ifndef V8_JUICE_TYPELIST_HPP_INCLUDED
#define V8_JUICE_TYPELIST_HPP_INCLUDED

namespace v8 { namespace juice {
/**
   The tmp namespace contains code related to template metaprogramming,
   a-la Alexandrescu's Loki library or Boost MPL.

   All of it is independent of the core juice library.
*/
namespace tmp {

    /**
       An utmost most-based compile-time assertion template.
       If Condition is false, an incomplete specialization of
       this type is
    */
    template <bool Condition>
    struct Assertion
    {
        enum { Value = 1 };
    };
    template <>
    struct Assertion<false>;
    
    /**
       A "null" type for use with TypeList.
    */
    struct NilType
    {
	/** This typedef is used by the Rules API. */
	typedef NilType type;
    };

    /**
       A base type for implementing a list of types, implemented as a
       Typelist, as described by Alexandrescu in "Modern C++
       Design". H may be any type. T _must_ be either NilType (to mark
       the end of the typelist) or a TypeChain<...> (or strictly
       compatible).

       Client code will normally use the generated TypeList<> types
       instead of using this one directly.

       Unlike Alexandrescu's Typelists, which are implemented
       faithfully by this class, our TypeList type is parameterized
       for some maximum number of arguments. This simplifies
       client-side usage but imposes (A) a hard-coded maximum on the
       number of items in the list and (B) another (internal) level of
       indirection for implementing the typelist algorithms described
       by Alexandrescu (length, index-of, etc.).
    */
    template <typename H, typename T>
    struct TypeChain
    {
#if 0 // Artifact from older source tree. Might be useful later.
	/**
	   Subtypes of this type should not override the 'ListType'
	   typedef, as it is used by the core lib to trick some
	   overloads into working, such that subclasses of
	   TypeChain will be picked up by specializations for
	   certain rules, as if they actually were a TypeChain.

	   There might be exceptions to the no-override rule, but none
	   come to mind.
	*/
	typedef TypeChain<H,T> ChainType;
#endif
	/* First Type in the list. */
	typedef H Head;
	/* Second type in the list. MUST be either NilType or a
	   TypeChain<>.
	*/
	typedef T Tail;

    };

#if !defined(DOXYGEN)
    namespace Detail
    {
        namespace tmp = v8::juice::tmp;
        /** Internal impl of tmp::LengthOf. */
        template <typename T>
        struct LengthOfImpl
        {
            enum { Value = 1 };
        };
        template <>
        struct LengthOfImpl<tmp::NilType>
        {
            enum { Value = 0 };
        };
        template <typename H, typename T>
        struct LengthOfImpl< tmp::TypeChain<H,T> >
        {
            enum { Value = 1 + LengthOfImpl<T>::Value };
        };
    }
#endif

    /**
       A template metafunction to compute the length of a TypeList.
       ListT must conform to the TypeChain interface.
    */
    template <typename ListT>
    struct LengthOf
    {
        enum {
        /**
           The number of types in ListT.
        */
        Value = Detail::LengthOfImpl< typename ListT::Head >::Value
                + LengthOf< typename ListT::Tail >::Value
        };
    };

    /** Specialization for end-of-list. */
    template <>
    struct LengthOf<NilType>
    {
        enum { Value = 0 };
    };

    /** @struct TypeList
        
       The TypeList type is a quasi-variadic template type which is specialized to
       take up to some compile-time limit (see V8_JUICE_TYPELIST_MAX_ARGS) of _types_
       as arguments. All of the TypeList code is generated from a script.

       TypeList is simply a TypeChain type for which specializations
       restructure the template arguments to conform to the TypeChain
       interface.
    */



#if !defined(DOXYGEN)
    namespace Detail
    {
        namespace tmp = v8::juice::tmp;
        /** Internal impl of tmp::TypeListIndexOf. */
        template <typename T, unsigned char I> struct TypeAtImpl;
        template <int I>
        struct TypeAtImpl< tmp::NilType,I>
        {
            typedef NilType Type;
        };
        template <typename H, typename T>
        struct TypeAtImpl< tmp::TypeChain<H,T>,0>
        {
            typedef H Type;
        };
        template <typename H, typename T,unsigned char I>
        struct TypeAtImpl< tmp::TypeChain<H,T>, I>
        {
            typedef typename TypeAtImpl<T, I - 1>::Type Type;
        private:
            typedef tmp::TypeChain<H,T> Chain;
            enum {
            Length = tmp::LengthOf<Chain>::Value,
            SizeIsValid = tmp::Assertion< (I<Length) >::Value
            };
        };
    }
#endif

    /**
       A template metafunction to type at a specific offset in a TypeList.
       ListT must be a TypeList, or otherwise conform to the TypeChain
       interface.
    */
    template <typename ListT, unsigned char Index>
    struct TypeAt
    {
        /**
           The type at the given index in the list.
        */
        typedef typename Detail::TypeAtImpl< TypeChain<typename ListT::Head, typename ListT::Tail>, Index>::Type Type;
        //typedef typename Detail::TypeAtImpl< typename ListT::ChainType, Index>::Type Type;
    };
    
}}} // namespaces

#ifndef V8_JUICE_TYPELIST_MAX_ARGS
/** @def V8_JUICE_TYPELIST_MAX_ARGS
   Define V8_JUICE_TYPELIST_MAX_ARGS before including this file to set
   the maximum number of arguments which TypeList<> can accept. It
   defaults to "some reasonable number", and pre-generated versions
   are available for "some other reasonable number". For generating
   variants with higher arg counts, see the maketypelist.pl script in
   this code's source repo.

   ACHTUNG:

   Changing this number may require rebuilding any code compiled using
   a different number. The main template instantiations will be
   different for code generated with a different maximum arg count.
   So pick a number and stick with it.
*/
#define V8_JUICE_TYPELIST_MAX_ARGS 10
#endif
#if !defined(DOXYGEN)
namespace v8 { namespace juice { namespace tmp {
#if V8_JUICE_TYPELIST_MAX_ARGS < 6
#  include "TypeList_05.h"
#elif V8_JUICE_TYPELIST_MAX_ARGS < 11
#    include "TypeList_10.h"
#elif V8_JUICE_TYPELIST_MAX_ARGS < 16
#    include "TypeList_15.h"
#else
#    error V8_JUICE_TYPELIST_MAX_ARGS is too high. See the docs above this code for details.
#endif
}}} // namespaces
#endif
#undef V8_JUICE_TYPELIST_MAX_ARGS

#endif // V8_JUICE_TYPELIST_HPP_INCLUDED
