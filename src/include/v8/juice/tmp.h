#ifndef V8_JUICE_TMP_HPP_INCLUDED
#define V8_JUICE_TMP_HPP_INCLUDED

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
       this type is invoced, causing a compile-time error.
    */
    template <bool Condition>
    struct Assertion
    {
        enum { Value = 1 };
    };
    /** Unimplemented - causes a compile-time error if used. */
    template <>
    struct Assertion<false>;

    /** A metatype whos Value member is true if X and Y are the same type. */
    template <typename X,typename Y>
    struct SameType
    {
        enum { Value = 0 };
    };
    /** Specialization for X==Y. */
    template <typename X>
    struct SameType<X,X>
    {
        enum { Value = 1 };
    };
    
}}} // namespaces

#endif // V8_JUICE_TMP_HPP_INCLUDED
