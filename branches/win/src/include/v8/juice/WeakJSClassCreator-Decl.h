//
// Helper file for generic WeakJSClassCreatorOps instantiation
//

#if !defined(WEAK_CLASS_TYPE) || !defined(WEAK_CLASS_NAME)
#error "You must define WEAK_CLASS_TYPE and WEAK_CLASS_NAME before including this file!"
#endif

#ifndef WEAK_CLASS_CLEANUP
#define WEAK_CLASS_CLEANUP true
#endif

namespace v8 { namespace juice {

	template <>
	struct WeakJSClassCreator_Opt_CleanupOwnedObjects<WEAK_CLASS_TYPE> :  WeakJSClassCreator_Opt_Bool<WEAK_CLASS_CLEANUP> {};

	template <>
	struct WeakJSClassCreatorOps<WEAK_CLASS_TYPE>
	{
	public:
		enum { ExtraInternalFieldCount = 0 };
		typedef WEAK_CLASS_TYPE WrappedType;
		static char const * ClassName() { return WEAK_CLASS_NAME; }

#ifdef WEAK_CLASS_PLAIN_CTOR
		static WrappedType * Ctor( v8::Arguments const & argv, std::string & exceptionText)
		{
			return new WrappedType();
		}
#else
		static WrappedType * Ctor( v8::Arguments const & argv, std::string & exceptionText);
#endif

#ifdef WEAK_CLASS_PLAIN_DTOR
		static void Dtor(WrappedType *o)
		{
			delete o;
		}
#else
		static void Dtor(WrappedType *o);
#endif

	};

}} // v8::juice

#include <v8/juice/WeakJSClassCreator-CastOps.h>

#undef WEAK_CLASS_TYPE
#undef WEAK_CLASS_NAME
#undef WEAK_CLASS_CLEANUP
#undef WEAK_CLASS_PLAIN_CTOR
#undef WEAK_CLASS_PLAIN_DTOR
