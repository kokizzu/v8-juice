#ifndef V8_JUICE_TYPELIST_HPP_INCLUDED
#define V8_JUICE_TYPELIST_HPP_INCLUDED

namespace v8 { namespace juice { namespace convert {

    /**
       A "null" type for use with TypeList.
    */
    struct NilType
    {
	/** This typedef is used by the Rules API. */
	typedef NilType type;
    };

    /**
       A base type for implementing a list of types, implemented
       as a Typelist, as described by Alexandrescu in "Modern C++
       Design". H may be any type. T must be either NilType (to mark the
       end of the typelist) or a TypePair<...> (or strictly
       compatible).

       Client code will normally use the generated TypeList<> types
       instead of using this one directly.
    */
    template <typename H, typename T>
    struct TypePair
    {
#if 0 // Artifact from older source tree. Might be useful later.
	/**
	   Subtypes of this type should not override the 'ListType'
	   typedef, as it is used by the core lib to trick some
	   overloads into working, such that subclasses of
	   TypePair will be picked up by specializations for
	   certain rules, as if they actually were a TypePair.

	   There might be exceptions to the no-override rule, but none
	   come to mind.
	*/
	typedef TypePair ListType;
#endif
	/* First Type in the list. */
	typedef H Head;
	/* Second type in the list. MUST be either NilType or a
	   TypePair<>.
	*/
	typedef T Tail;

    };

    namespace Detail
    {
        namespace cv = v8::juice::convert;
        template <typename T>
        struct TypeListSizeImpl
        {
            enum { Value = 1 };
        };
        template <>
        struct TypeListSizeImpl<cv::NilType>
        {
            enum { Value = 0 };
        };
//         template <typename H>
//         struct TypeListSizeImpl< cv::TypePair<H,cv::NilType> >
//         {
//             //enum { Value = TypeListSizeImpl<H>::Value };
//             enum { Value = 1 };
//         };
//         template <>
//         struct TypeListSizeImpl< cv::TypePair<cv::NilType,cv::NilType> >
//         {
//             enum { Value = 0 };
//         };
//         /** This specialization would only be called invalid typelists
//             which use NilType IN the list.
//         */
//         template <typename T>
//         struct TypeListSizeImpl< cv::TypePair<cv::NilType,T> >;

        template <typename H, typename T>
        struct TypeListSizeImpl< cv::TypePair<H,T> >
        {
            enum { Value = 1 + TypeListSizeImpl<T>::Value };
        };
    }
    /**
       A template metafunction to compute the length of a TypeList.
       ListT must conform to the TypePair interface.
    */
    template <typename ListT>
    struct TypeListSize
    {
        //typedef TypePair< typename ListT::Head, typename ListT::Tail > L;
        enum {
        /**
           The number of types in ListT.
        */
        Value =
#if 1
        Detail::TypeListSizeImpl< typename ListT::Head >::Value
        + TypeListSize< typename ListT::Tail >::Value
        // + Detail::TypeListSizeImpl< typename ListT::Tail >::Value // WTF does this always eval to 1?
#else
        Detail::TypeListSizeImpl<L>::Value
#endif
        
        };
    };
    /** Specialization for end-of-list. */
    template <>
    struct TypeListSize<NilType>
    {
        enum { Value = 0 };
    };

    
}}} // namespaces

#ifndef V8_JUICE_TYPELIST_MAX_ARGS
/**
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
namespace v8 { namespace juice { namespace convert {
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
