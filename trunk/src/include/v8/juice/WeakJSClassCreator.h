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
#include <string>
#include <stdexcept>

namespace v8 {
namespace juice {

    namespace Detail
    {
	/**
	   A functor to delete objects.
	*/
	struct ObjectDeleter
	{
	    /** Calls (delete obj). */
	    template <typename T>
	    void operator()( T * obj ) const
	    {
		if( obj ) delete obj;
	    }
	};
    }

    /**
       Any type used with WeakJSClassCreator must provide
       a specialization of this type and provide all of
       the functions described herein.
    */
    template <typename T>
    struct WeakJSClassCreatorOps
    {
	/**
	   This type's associated native type.
	*/
	typedef T WrappedType;

	/**
	   This will be called in response to calling
	   "new WrappedType(...)" in JS code. It should:

	   - Create a new native object.

	   - If successful, return that object.

	   - On error, return 0 and optionally populate the (string &)
	   argument with an informative error message (e.g. "requires
	   arguments (String,Integer)").

	   If the client is using any "supplemental" garbage collection
	   (e.g. the v8::juice::cleanup API) then they should
	   register the object in this function, and use Dtor()
	   as the registered destruction callback.
	*/
	static WrappedType * Ctor( Arguments const &  /*argv*/,
				   std::string & /*exceptionText*/);

	/**
	   Must "destroy" the given object, though the exact meaning
	   of "destroy", and mechanism of destruction, is
	   type-specific.

	   This function is is responsible for destroying the native
	   object. Most implementations must simply call "delete obj",
	   which is fine for many types, but opaque types and types
	   allocated via something other than 'new' will need a
	   different implementation. A destructor functor for shared,
	   static, or stack-allocated objects should simply do nothing
	   (or maybe clear the object to some empty/default state).

	   If the client is using any "supplemental" garbage collection
	   (e.g. the v8::juice::cleanup API) then they should
	   deregister the object in this function.
	*/
	static void Dtor( WrappedType * obj );
    };

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
       An option is provided to support external garbage collection,
       in case calling dtors is an absolute must for your types.

       - If the client calls the static DestroyObject() function of
       this class. This also disposes the underlying weak pointers, so
       they won't point back to the object anymore. This allows us to
       implement member function which disconnect from the underlying native
       object (e.g. File.close()).

       The template arguments are:

       - WrappedT = the native type we will wrap.

       - ClassOpsType is a helper which defines the core-most
       operations needed by this class. There are two ways to provide
       this: 1) specialize WeakJSClassCreatorOps<WrappedT> for the
       type, following the interface shown in the API docs. 2) provide
       a custom class with the same interface and semantics documented
       in WeakJSClassCreatorOps.  See the docs for
       WeakJSClassCreatorOps for how types consistent with its
       interface should behave.

       - extraInternalFieldCount = the number of internal fields
       required by the class. This class will automatically allocate
       one additional field and store the underlying native object in
       the *last* internal field (so clients can work with fields 0..N
       as normal, without having to take too much care to not step on
       this class' slot).

       The wrapper works basically like this:

       When, in JS code, "new MyType(...)" is called, this call arranges
       to have a private constructor called. That contructor does the following:

       - Passes the Arguments array to ClassOpsType::Ctor(). If the
       ctor returns 0, the construction fails and throws and
       exception. If the exceptionText string (2nd parameter to
       ClassOpsType::Ctor()) is non-empty when the ctor returns, the
       private constructor will destroy the returned object (if
       necessary) using ClassOpsType::Dtor() and throw a JS exception
       using the exceptionText string.

       - The argv.This() argument passed to ClassOpsType::Ctor() is
       the "this" object of the "real" constructor.

       - If ClassOpsType::Ctor() succeeds (returns non-0) then the new
       object is bound to a weak pointer and gives the new weak
       pointer to v8. It will also add the native as an External in
       the Nth internal slot, where N is the extraInternalFieldCount
       value passed to this template (it reserves one extra slot at
       the end of the list for this purpose, so there will always
       be at least one slot).

       It's a lot simpler than it sounds and completely frees the caller of
       worry that destructors might not be called (unless the app crashes
       prematurely due to his buggy JS/Native wrapper ;).


       Example usage:

       \code
       // Custom ClassOpsType for use with WeakJSClassCreator:
       struct MyClassOps
       {
           typedef MyClass WrappedType;
           static MyType * Ctor( Arguments const & argv, std::string & exceptionText)
	   {
	       return new MyType;
	   }
	   static void Dtor( MyType * obj )
	   {
	       if(obj) delete obj;
	   }
       };

       ...
       // Add the class to JS:
       typedef WeakJSClassCreator<MyType,MyClassOps> WC;
       WC c( "MyType", objectToAddClassTo );
       c.Set(...)
         .Set(...)
	 .Set(...)
	 .Set(...)
	 .Seal(); // must be the last call made on this object.

       // Now create a JS subclass:
       typedef WeakJSClassCreator<MySubType,MySubTypeOps> WC2;
       WC2 c2( "MySubType", objectToAddClassTo );
       c2.Inherit( c )
         .Set(...)
	 .Seal(); // must be the last call made on this object.
       \endcode

       That's all there is to it.
    */
    template <typename WrappedT,
	      typename ClassOpsType = WeakJSClassCreatorOps<WrappedT>,
	      int extraInternalFieldCount = 0>
    class WeakJSClassCreator : public JSClassCreator
    {
    public:
	typedef WrappedT WrappedType;
	/**
	   Starts the setup of a new class with the given name. It
	   will be populated into the target object when Seal() is
	   called.  See the JSClassCreator ctor for more information
	   on that process.
	*/
	WeakJSClassCreator( char const * className,
			    Handle<Object> target)
	    : JSClassCreator( className, target, ctor_proxy, static_cast<int>(extraInternalFieldCount + 1) )
	{
	}

	/**
	   Returns the bound native for the given handle. The handle
	   is assumed to have been created using this class'
	   mechanisms. If it was not, this routine will return 0.

	   Ownership of the returned object remains unchanged.

	   Results are technically undefined if the given handle has
	   not created by this class, but this function makes every
	   effort to ensure that it doesn't do anything too stupid.
	*/
	static WrappedType * GetSelf( Local<Object> h )
	{
	    if( h.IsEmpty() || (h->InternalFieldCount() != (extraInternalFieldCount+1)) ) return 0;
	    Local<Value> lv( h->GetInternalField(extraInternalFieldCount) );
	    if( lv.IsEmpty() || !lv->IsExternal() ) return 0;
	    Local<External> ex( External::Cast(*lv) );
	    return static_cast<WrappedType*>( ex->Value() ); // we can only hope...
	}

	/** Reimplemented to add one to the given number (it is reserved
	    for use by this type for holding the native object).

	    TODO: consider allowing this and remove it from the template args
	*/
 	virtual WeakJSClassCreator & SetInternalFieldCount( int n )
 	{
	    JSClassCreator::SetInternalFieldCount(n+1);
	    // gcc 4.3.2 won't allow this syntax: :-?
 	    //this->JSClassCreator::SetInteralFieldCount( n + 1 );
	    return *this;
 	}

	/**
	   Like GetSelf(), but takes a Handle to a value. This can be
	   used for checking/converting arguments other than the
	   current This object (e.g., maybe the second argument to a
	   certain WrappedType member function is a different
	   WrappedType object).

	   This can be used in place of GetSelf(), but it does more
	   work than GetSelf() has to.
	*/
        static WrappedType * GetNative( Handle<Value> h )
        {
            if( h.IsEmpty() || ! h->IsObject() ) return 0;
            Local<Object> obj( Object::Cast(*h) );
	    return GetSelf( obj );
        }

	/**
	   If jo "seems" to have been created via this class' mechanism,
	   then the behaviour is as-if the weak pointer callback has been
	   triggered. That will delete the bound native and dispose/clear the
	   persistent handle.

	   The reason this function exists is so that native implementations
	   of member functions can implement functions which logically destroy
	   the JS object. For example:

	   \code
	   var os = new OutStream(...);
	   ...
	   os.close();
	   \endcode

	   That code "should" destroy the underlying device and
	   disconnect the JS reference to that object, and it can use
	   this function to do so.
	*/
	static bool DestroyObject( Local<Object> jo )
	{
	    WrappedType * t = GetSelf(jo);
	    if( ! t ) return false;
	    Persistent<Object> p = Persistent<Object>::New( jo ); // is this correct? Seems to work.
	    weak_callback( p, t );
	    return true;
	}

	/**
	   Like the other DestroyObject() overload, but takes a handle
	   to a generic value. If h is not an Object reference, false
	   is returned, otherwise the result of calling the other overload
	   is returned.
	*/
        static bool DestroyObject( Handle<Value> h )
        {
            return ( h.IsEmpty() || ! h->IsObject() )
		? false
		: DestroyObject( Local<Object>(Object::Cast(*h)) );
        }

    private:
	/**
	   Internal binding context for BindNative() and friends.
	*/
	static const void * bind_cx() { return 0; /*(void const *)bind_cx*/; }
	/**
	   Unbinds native and destroys it using CleanupFunctor.
	*/
	static void the_cleaner( WrappedType * native )
	{
	    //CERR << "the_cleaner( native@"<<native<<")\n";
	    if( native )
	    {
		ClassOpsType::Dtor( native );
	    }
	}

	/**
	   Only called by v8 GC. It removes the object from "supplemental" GC,
	   calls the_cleaner(nobj), and Dipose()es the given object.
	*/
	static void weak_callback(Persistent< Value > pv, void * nobj)
	{
	    Local<Object> jobj( Object::Cast(*pv) );
	    if( jobj->InternalFieldCount() != (1+extraInternalFieldCount) ) return;
	    Local<Value> lv( jobj->GetInternalField(extraInternalFieldCount) );
	    if( lv.IsEmpty() || !lv->IsExternal() ) return;
	    /**
	       We have to ensure that we have no dangling External in JS space. This
	       is so that functions like IODevice.close() can act safely with the
	       knowledge that member funcs called after that won't get a dangling
	       pointer. Without this, some code will crash in that case.
	    */
	    jobj->SetInternalField(extraInternalFieldCount,Null());
	    pv.Dispose();
	    pv.Clear();
	    the_cleaner( static_cast<WrappedType*>( nobj ) );
	}

	/**
	   "Wraps" a native pointer in a JS weak pointer. Returns the new
	   Persistent weak object, which is a wrapper for the self object.
	*/
	static Persistent<Object> wrap_native( Handle<Object> _self, WrappedType * obj )
	{
	    const int ic = _self->InternalFieldCount();
	    if( ! ic )
	    {
		return Persistent<Object>();
	    }
	    Persistent<Object> self( Persistent<Object>::New(_self) );
	    self.MakeWeak( obj, weak_callback );
	    self->SetInternalField( ic - 1, External::New(obj) );
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
	    std::string err;
	    WrappedType * obj = 0;
	    try
	    {
		obj = ClassOpsType::Ctor( argv, err );
	    }
	    catch(std::exception const & ex)
	    {
		return ThrowException(String::New(ex.what()));
	    }
	    catch(...)
	    {
		return ThrowException(String::New("Native constructor threw an unknown native exception!"));
	    }
	    if( ! err.empty() )
	    {
		if( obj )
		{
		    ClassOpsType::Dtor(obj);
		    obj = 0;
		}
		return ThrowException(String::New(err.c_str(),static_cast<int>(err.size())));
	    }
	    if( ! obj )
	    {
		return ThrowException(String::New("Constructor failed for an unspecified reason!"));
	    }
	    return wrap_native( argv.This(), obj );
	}

    };

}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_V8_WEAKCLASSGENERATOR_H_INCLUDED */
