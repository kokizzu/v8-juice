#if !defined(CODE_GOOGLE_COM_V8_JUICE_PLUGIN_WHEFS_H_INCLUDED)
#define CODE_GOOGLE_COM_V8_JUICE_PLUGIN_WHEFS_H_INCLUDED 1

/************************************************************************
Some of the implementation details for the whio/whefs plugin which might
be useful in some cross-plugin contexts.
************************************************************************/

#include <v8.h>
#include <v8/juice/juice.h>
#include <v8/juice/juice-config.h>
#include <v8/juice/convert.h>
#include <v8/juice/cleanup.h>
#include <v8/juice/WeakJSClassCreator.h>

#include "whefs_amalgamation.h" // the core whefs library.

namespace v8 { namespace juice { namespace whio {

    /**
       Static/shared strings, some of which are used as template
       parameters (where we can't use string literals but can,
       curiously enough, use references to pointers to shared
       strings)...
    */
    struct WhefsStrings
    {
        static const char * openDevice;
        static const char * unlink;
        static const char * dumpToFile;
        static const char * ls;
        static const char * mkfs;
        static const char * vfsFormat;
    };

    /**
       Adds to target object:

       - whefs (generic holder object)
       - whefs.WhEFS class

       See full docs at:

       http://code.google.com/p/v8-juice/wiki/PluginWhio
    */
    Handle<Value> SetupWhefsClasses(const Handle<Object> target );

    /**
       Internal JS wrapper/helper for C-level whefs objects.
    */
    struct WHEFS
    {
        typedef whefs_fs type;
        /** Is not valid until SetupWhefsClasses() is called. */
        static Persistent<Function> js_ctor;
    };

    /**
       For use as a base class for WeakJSClassCreatorOps<WHEFS>.
    */
    struct WHEFSOps
    {
	enum { ExtraInternalFieldCount = 0 };
	typedef WHEFS::type WrappedType;
        static char const * ClassName() { return "WhEfs"; }
	static WrappedType * Ctor( Arguments const & argv,
                                   std::string & exceptionText);
	static void Dtor( WrappedType * obj );
    private:
	/** Callback for use with juice::cleanup::AddToCleanup(). */
	static void cleanup_callback( void * obj );
    };


} // namespace whio

    /** Specialization needed by WeakJSClassCreator. */
    template <>
    struct WeakJSClassCreatorOps<whio::WHEFS> : whio::WHEFSOps {};

} } //namespaces

#define WEAK_CLASS_TYPE v8::juice::whio::WHEFS
#include <v8/juice/WeakJSClassCreator-CastOps.h>


#endif // CODE_GOOGLE_COM_V8_JUICE_PLUGIN_WHEFS_H_INCLUDED
