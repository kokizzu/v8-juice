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
       end of the typelist) or a TypeListBase<...> (or strictly
       compatible).

       Client code will normally use the generated TypeList<> types
       instead of using this one directly.
    */
    template <typename H, typename T>
    struct TypeListBase
    {
#if 0 // artifact from older source tree:
	/**
	   Subtypes of this type should not override the 'type'
	   typedef, as it is used by the core lib to trick some
	   overloads into working, such that subclasses of
	   TypeListBase will be picked up by specializations for
	   certain rules, as if they actually were a TypeListBase.

	   There might be exceptions to the no-override rule, but none
	   come to mind.
	*/
	typedef TypeListBase ListType;
#endif
	/** This typedef is used by the Rules API. */
	/* First Type in the list. */
	typedef H Head;
	/* Second type in the list (must be either NilType or
	   TypeListBase<>).
	*/
	typedef T Tail;
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
