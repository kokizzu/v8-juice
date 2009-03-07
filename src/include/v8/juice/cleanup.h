#ifndef WANDERINGHORSE_NET_GOOGLE_V8_CLEANUP_H_INCLUDED_
#define WANDERINGHORSE_NET_GOOGLE_V8_CLEANUP_H_INCLUDED_ 1

namespace v8 { namespace juice {
/**
   "Supplemental", client-side garbage collection routines, to work
   around certain deficiencies in v8's GC (namely that it's never guaranteed
   to be called by the time the engine shuts down. Ever.).
*/
namespace cleanup {

    /**
       Typedef for generic destructor functions. They are semantically
       compatible with free(), though whether or not they actually
       deallocate any memory is an internal implementation detail
       (e.g. they may use an object pool).
    */
    typedef void (*Destructor)( void * obj );

    namespace Detail
    {
	/**
	   Helper to generate destructor functions halfway type-safely.
	*/
	template <typename T>
	struct DestructorGen
	{
	static void dtor( void * obj )
	    {
	    T * t = obj ? static_cast<T*>( obj ) : 0;
	    if( t )
	    {
		delete t;
	    }
	    }
	};
    }

    /**
	Client apps can create one of these in a scope right above the
	scope in which their JS Context lives. The idea is that the context
	is allowed to go out of scope, and only then do we try to clean up.
	This allows us to go through the normal v8 channels up until the
	last dying moments of the context, and if the context isn't cleaned
	up, we can take over where it left off.

	It is never a good idea to have more than one of these active
	at any time, as that will cause unwanted, premature deletions.
	Only use one per application (not per JS context!).
   */
    class CleanupSentry
    {
    public:
	/** Does nothing. */
	CleanupSentry();
	/**
	   Calls Cleanup()
	*/
	~CleanupSentry();
    };

    /**
       Adds the given object to "cleanup list", such that
       Cleanup() will destroy obj by calling dtor(obj).
    */
    void AddToCleanup( void * obj, Destructor dtor );

    /**
       Adds obj to the cleanup list (see the 2-arg overload
       of this function). The destructor simply calls
       (delete obj), so that must be legal for T.
    */
    template <typename T>
    void AddToCleanup( T * obj )
    {
	AddToCleanup( obj, Detail::DestructorGen<T>::dtor );
    }

    /**
       Removes the given object from the cleanup list,
       so Cleanup() will not destroy it.
    */
    void RemoveFromCleanup( void const * obj );

    /**
       Destroys all objects which have been added by AddToCleanup() and clears
       the internal cleanup list.
    */
    void Cleanup();

}}} // namespaces

#endif /* WANDERINGHORSE_NET_GOOGLE_V8_CLEANUP_H_INCLUDED_ */
