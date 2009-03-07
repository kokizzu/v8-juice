#if !defined(CODE_GOOGLE_COM_P_V8_V8_BIND_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_V8_BIND_H_INCLUDED 1

/**
   A mini-framework for binding C++ natives to v8 (JavaScript engine)
   script-side objects in a type-safe manner (the v8 API only covers
   (void *) natively).

   v8: http://code.google.com/p/v8/

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: same as v8 (see below)
*/
// Copyright 2007-2008 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
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


//#include <cstdint> // arg! Requires C++0x!
//#include <stdint.h> // hope the client's platform is recent!
#include <v8.h>
#include <string>
#include <map>
namespace v8 {
namespace juice {
/**
   The bind namespace holds functions and types for binding JS-side values
   to native values in a type-safe manner. For starters, see BindNative().
*/
namespace bind {
    using namespace v8;

    /**
       Convenience typedef.
    */
    typedef Handle<Value> ValueHandle;

    /**
       The "context key" type used by the BindNative() family
       of functions.
    */
    typedef void const * BindKeyType;
    /**
       Internal library details.
    */
    namespace Detail
    {
	/**
	   Shared map for binding native data to JS handles.
	*/
	template <typename T>
	std::map<BindKeyType, std::map<void const *,T> > &
	native_data_master()
	{
	    typedef std::map<BindKeyType, std::map<void const *,T> > MT;
	    static MT bob;
	    return bob;
	}

	/**
	   Shared map for binding native data to JS handles.
	*/
	template <typename T>
	std::map<void const *,T> &
	native_data( BindKeyType cx )
	{
	    return native_data_master<T>()[cx];
	}

	/**
	   Utility type for some template algos.
	   
	   Specializations ensure that type_stripper<T>::type
	   is always T, unqualified.
	*/
	template <typename T>
	struct type_stripper
	{
	    typedef T type;
	};
	template <typename T>
	struct type_stripper<T &>
	{
	    typedef T type;
	};
	template <typename T>
	struct type_stripper<T const &>
	{
	    typedef T type;
	};
	template <typename T>
	struct type_stripper<T *>
	{
	    typedef T type;
	};
	template <typename T>
	struct type_stripper<T const *>
	{
	    typedef T type;
	};

    }


    /**
       Associates obj with the given key in the given context (an
       arbitrary pointer), such that GetBoundNative(cx,key) will
       return that object. It does no manipulation of cx or key, but
       requires that key be non-0. In practice it is simplest to
       construct key with the value of obj (but note that key holds a
       typeless void pointer, instead of a type-safe pointer).

       obj must live until UnbindNative() is called, or
       GetBoundNative() may return a dangling pointer.

       Calls to BindNative() must be balanced by calls to UnbindNative()
       or internal mapping resources may be leaked. The ScopedBinder
       class can simplify ensuring this requirement is met.

       The context argument may be 0 and is provided so that client code
       can hide their registrations from other code by associating it
       with a private context value (e.g. a pointer to a static function
       or shared object).

       This function returns true if it binds the object, else false. Error
       conditions include:

       - The given key is already bound to some value in the given
       context.

       - If this routine is called post-main(), results are
       undefined. This is because the destruction order of
       static/shared data is undefined, and this affects both the
       bindings data and the state of the associated v8 engine. It is
       possible that it would be called after either v8 or the shared
       map have been destroyed, either of which is likely to cause a
       crash (v8 normally provides very pretty trace information when
       it does this!).


       Notes about thread safety:

       This code is technically not thread-safe because it uses
       static, shared data to store the bindings. That said, the v8
       engine does not allow more than one thread to use it at a
       time. Since the binding API is only intended to be used from v8
       client code, we inherit the property that only one thread at a
       time will access the shared data.


       Potential TODOs:

       - Consider adding a bind count, essentially a reference count
       for the bindings, and don't unbind until the count goes to 0.
       This would simplify some binding operations which we currently
       can't do because we end up with multiple unbind operations
       happening at unpredictable (or "predictable but unfortunate")
       times. e.g. with that we could make ScopedBinder() copyable.

       - Consider making the context parameter a template type
       argument instead of a pointer
       argument. e.g. BindNative<MyType,MyContext>(key,val). The end
       effect would be the same, and we could default the ContextType
       argument to the first template param (oh, damn - we can't use
       default template types in functions, only classes). The current
       interface's context parameter is very much an artefact of its
       port from the SpiderApe API.
    */
    template <typename NT>
    bool BindNative( BindKeyType cx, void const * key,
		     NT * obj
		     // typename type_stripper<NT>::type * obj
		     )
    {
	if( 0 == key ) return false;
	typedef typename Detail::type_stripper<NT>::type * PT;
	typedef std::map< void const *, PT > MT;
	MT & map = Detail::native_data<PT>( cx );
	typename MT::iterator it = map.find( key );
	if( it != map.end() ) return false;
	map[key] = obj;
	return true;
    }

    /**
       Equivalent to BindNative( cx, id.Value(), obj ).
    */
    template <typename NT>
    bool BindNative( BindKeyType cx, External const & id,
		     NT * obj
		     // typename type_stripper<NT>::type * obj
		     )
    {
	return BindNative( cx, id.Value(), obj );
    }

    /**
       Returns the NT object (if any) bound to the given context
       and lookup key, or 0 if BindNative() has not been called for
       that combination of arguments.
    */
    template <typename NT>
    NT * GetBoundNative( BindKeyType cx, BindKeyType key )
    {
	if( 0 == key ) return false;
	typedef typename Detail::type_stripper<NT>::type * PT;
	typedef std::map< void const *, PT > MT;
	MT & map = Detail::native_data<PT>( cx );
	typename MT::iterator it = map.find( key );
	return (it == map.end() )
	    ? 0
	    : (*it).second;
    }

    /**
       If id.Value() has been registered with cx using BindNative<NT>()
       then the bound object is returned, otherwise 0 is returned.
    */
    template <typename NT>
    NT * GetBoundNative( BindKeyType cx, External const & id )
    {
	return GetBoundNative<NT>( cx, id.Value() );
    }

    /**
       Convenience overload. If id->IsExternal() then this function
       returns the bound NT object (if any) associated with the
       External data, otherwise 0 is returned.
    */
    template <typename NT>
    NT * GetBoundNative( BindKeyType cx, Handle<Value> const id )
    {
	External * ex = id.IsEmpty() ? 0 :
	    (id->IsExternal() ? External::Cast( *id ) : 0);
	return ex
	    ? GetBoundNative<NT>( cx, ex->Value() )
	    : 0;
    }
    /**
       Disassociates a native object which has been bound using
       BindNative(). The arguments must be the same ones passed to
       BindNative(). On success true is returned and the association
       is removed. On error (object not registered, invalid arguments,
       or key is bound to a different object) false is returned.
    */
    template <typename NT>
    bool UnbindNative( BindKeyType cx, BindKeyType key,
		       NT const * obj
		       // typename type_stripper<NT>::type * obj
		       )
    {
	if( 0 == key ) return false;
	typedef typename Detail::type_stripper<NT>::type BT;
	typedef BT * PT;
	typedef std::map< void const *, PT > MT;
	MT & map = Detail::native_data<PT>( cx );
	typename MT::iterator it = map.find( key );
	if( map.end() == it ) return false;
	if( (*it).second != obj ) return false;
	map.erase( it );
	if( map.empty() )
	{
	    Detail::native_data_master<PT>().erase(cx);
	}
	return true;
    }

    template <typename NT>
    bool UnbindNative( BindKeyType cx, External const & id,
		       NT const * obj
		       // typename type_stripper<NT>::type * obj
		       )
    {
	return UnbindNative( cx, id.Value(), obj );
    }

    /**
       A sentry type which calls BindNative() at construction
       and UnbindNative() at destruction.
    */
    template <typename NT>
    class ScopedBinder
    {
    public:
	/**
	   Calls BindNative(cx,id,obj) and sets this->bound
	   to the result.
	*/
	ScopedBinder( BindKeyType cx, BindKeyType key, NT * obj )
	    : bound(false),context(cx),id(key),native(obj)
	{
	    bound = BindNative( cx, key, obj );
	}

	/**
	   Calls UnbindNative(this->context,this->id,this->native).
	*/
	~ScopedBinder()
	{
	    if( this->bound )
	    {
		UnbindNative( this->context, this->id, this->native );
	    }
	}
	bool bound;
    private:
	ScopedBinder( ScopedBinder const & that );
	ScopedBinder & operator=( ScopedBinder const & that );
	BindKeyType context;
	BindKeyType id;
	NT * native;
    };

}}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_V8_BIND_H_INCLUDED */
