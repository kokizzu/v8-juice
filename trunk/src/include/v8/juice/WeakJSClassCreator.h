#if !defined(CODE_GOOGLE_COM_P_V8_V8_WEAKCLASSGENERATOR_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_V8_WEAKCLASSGENERATOR_H_INCLUDED 1
/*
   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: same as v8 (see below)
*/
// Copyright 2009 Stephan Beal. All rights reserved.  Redistribution
// and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <v8.h>
#include <v8/juice/JSClassCreator.h>
#include <v8/juice/bind.h>
#include <v8/juice/cleanup.h>

#include <string>

namespace v8 {
namespace juice {

    namespace Detail
    {
	struct ObjectDeleter
	{
	    template <typename T>
	    void operator()( T * obj ) const
	    {
		if( obj ) delete obj;
	    }
	};
    }
    /**
       WeakJSClassCreator is a tool to simplify the creation of new
       wrapped classes, including addition of automatic cleanup handling
       via weak-pointer callbacks.

       It is used almost exactly like JSClassGenerator, but adds some
       features (and requirements) for handling JS weak pointers to
       arbitrary types. It combines three forms of deletion for bound
       objects:

       - Via v8's weak pointers, the native will be deleted *if* (BIG
       *if*) the gc is triggered and the pointer is due for deletion.

       - If v8::juice::cleanup::Cleanup() is called.

       - If the client calls the static DestroyNative() function of this class,
       passing it the native. This also disposes the underlying weak pointers,
       so they won't point back to the object anymore.

       The template arguments are:

       - WrappedT = the native type we will wrap.

       - NativeConstructor = explained below

       - internalFieldCount = the number of internal fields which the client
       will use. This class adds one to that number and uses the last field
       to store a handle to the native object.

       - CleanupFunctorT = a functor which must have an operator()(WrappedT*).
       The cleanup functor is responsible for destroying the native object.
       The default implementation simply calls delete, which is fine for many
       types.

       The NativeConstructor function is a callback which looks like this:

       \code
       WrappedT * construct( Local<Object> self,
                             int argc,
			     Handle<Value> argv[],
			     std::string & exception );
       \endcode

       And it works like this:

       When, in JS code, "new MyType(...)" is called, this call arranges
       to have a private constructor called. That contructor does the following:

       - Converts the Arguments array to an array of Handle<Value> and passes
       them on to the NativeConstructor. If the NativeConstructor returns 0,
       the construction fails and throws and exception. If the exceptionText
       (4th parameter to NativeConstructor) is non-empty when NativeConstructor
       returns, the private constructor will destroy the returned object (if
       necessary) and throw an exception using the exceptionText string.

       - If NativeConstructor succeeds (returns non-0) then the new object
       is bound to a weak pointer, bound to v8::juice::bind::BindNative<WrappedT>()
       with a BindContextKey of 0 (so CastFromJS() will work with it), adds it to
       v8::juice::cleanup::AddToCleanup(), and gives the new weak pointer to
       v8.

       It's a lot simpler than it sounds and completely frees the caller of
       worry that destructors might not be called (unless the app crashes
       prematurely due to his buggy JS/Native wrapper ;).

    */
    template <typename WrappedT,
	      WrappedT * (*NativeConstructor)( Local<Object> /*self*/,
					       int /*argc*/,
					       Handle<Value> /*argv*/[],
					       std::string & /*exceptionText*/),
	      int internalFieldCount,
	      typename CleanupFunctorT = Detail::ObjectDeleter>
    class WeakJSClassCreator : public JSClassCreator
    {
    public:
	typedef WrappedT WrappedType;
	typedef CleanupFunctorT CleanupType;

	WeakJSClassCreator( char const * className,
			    Handle<Object> target )
	    : JSClassCreator( className, target, ctor_proxy, internalFieldCount + 1 )
	{
	}

	/**
	   Returns the bound native for the given handle. The handle is assumed to
	   have been created using this class' mechanisms. If it was not, this routine
	   will return 0.
	*/
	static WrappedType * GetBoundNative( Handle<Value> h )
	{
	    if( h.IsEmpty() || ! h->IsObject() ) return 0;
	    Local<Object> obj( Object::Cast(*h) );
	    if( obj->InternalFieldCount() != (internalFieldCount+1) ) return 0;
	    Handle<Value> key( obj->GetInternalField(internalFieldCount) );
	    return ::v8::juice::bind::GetBoundNative<WrappedType>( bind_cx(), key );
	}


	static void DestroyNative( WrappedType * t )
	{
	    the_cleaner<true>( t, t );
	}
    private:
	/**
	   Internal binding context for BindNative() and friends.
	*/
	static const void * bind_cx() { return 0; /*(void const *)bind_cx*/; }
	/**
	   Unbinds native and destroys it using CleanupFunctor.
	*/
	template <bool removeFromCleanup>
	static void the_cleaner( void * obj, WrappedType * native )
	{
	    //CERR << "dev_cleanup( void@"<<obj<<",  native@"<<native<<")\n";
	    if( removeFromCleanup )
	    {
		::v8::juice::cleanup::RemoveFromCleanup(obj);
		// ^^^ maintenance reminider: we must call that from here, instead
		// of the primary dtor because in some cases (IOBase.close()) we want
		// to destroy the object from inside a member function, and we
		// can re-use this function for that purpose.
	    }
	    if( native )
	    {
		::v8::juice::bind::UnbindNative( bind_cx(), obj, native );
		CleanupFunctorT()( native );
	    }
	}

	/**
	   Destructor for use with v8::juice::cleanup::AddToCleanup().
	   obj MUST be-a (WrappedType*). It simply calls the_cleaner(obj,(T*)obj).
	*/
	static void cleanup_callback( void * obj )
	{
	    the_cleaner<false>( obj, static_cast<WrappedType*>( obj ) );
	}

	/**
	   Only called by v8 GC. It removes the object from "supplemental" GC,
	   calls cleanup_callback(), and Dipose()es the given object.
	*/
	static void weak_callback(Persistent< Value > jobj, void * nobj)
	{
	    the_cleaner<true>( nobj, static_cast<WrappedType*>( nobj ) );
	    jobj.Dispose();
	    jobj.Clear();
	}

	/**
	   "Wraps" a native pointer in a JS weak pointer. Returns the new
	   Persistent weak object, which is a wrapper for the self object.
	*/
	static Persistent<Object> wrap_native( Handle<Object> _self, WrappedType * obj )
	{
	    ::v8::juice::cleanup::AddToCleanup(obj, cleanup_callback );
	    ::v8::juice::bind::BindNative( bind_cx(), obj, obj );
	    Persistent<Object> self( Persistent<Object>::New(_self) );
	    self.MakeWeak( obj, weak_callback );
	    self->SetInternalField( internalFieldCount, External::New(obj) );
	    //CERR << "Wrapped object @"<<obj<<" in a Persistent<Object>.\n";
	    return self;
	}

	/**
	   Constructs a WrappedType by calling NativeConstructor. If it
	   succeeds, it returns wrap_native(), otherwise it throws.

	   Returns argv.This() unless it throws.
	 */
	static Handle<Value> ctor_proxy( const Arguments & argv )
	{
	    if (!argv.IsConstructCall()) 
	    {
		return ThrowException(String::New("Cannot call this constructor as function!"));
	    }
	    const int argc = argv.Length();
	    std::vector< Handle<Value> > av(argc,Null());
	    for( int i = 0; i < argc; ++i ) av[i] = argv[i];
	    std::string err;
	    WrappedType * obj = NativeConstructor( argv.This(), argc, &av[0], err );
	    if( ! err.empty() )
	    {
		if( obj )
		{
		    CleanupFunctorT()( obj );
		    obj = 0;
		}
		return ThrowException(String::New(err.c_str(),static_cast<int>(err.size())));
	    }
	    if( ! obj )
	    {
		return ThrowException(String::New("Opening the stream failed for an unspecified reason!"));
	    }
	    return wrap_native( argv.This(), obj );
	    //return argv.This();
	}

    };

}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_V8_WEAKCLASSGENERATOR_H_INCLUDED */
