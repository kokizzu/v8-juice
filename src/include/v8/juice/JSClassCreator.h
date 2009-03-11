#if !defined(CODE_GOOGLE_COM_P_V8_V8_CLASSWRAP_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_V8_CLASSWRAP_H_INCLUDED 1
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
namespace v8 {
namespace juice {


    /**
       JSClassCreator is a tool to simplify the creation of new
       JS-side classes. It does not currently have any features for
       automatic conversions between the native and JS object, for
       purposes of, e.g., forwarding JS funcs to member functions -
       that'll require a bit of templates magic later on.

       Example usage:

       \code
       JSClassCreator binder( "MyClass", targetObject, myCtor, 1 );
       binder
           .Inherit( otherBinder )
	   .Set( "funcOne", my_callback_func )
	   .Set( "valOne", String::New("hi there!") )
	   .Seal(); // Seal() MUST come last!
       \endcode

       Seal() MUST be the last function called and no setters should
       be called after calling Seal() (it might work, it might not,
       but in my experimentation v8 is very sensitive to certain
       orders of operations).
    */
    class JSClassCreator
    {
    private:
	char const * className;
	Handle<Object> target;
	Handle<FunctionTemplate> ctorTmpl;
	Local<ObjectTemplate> proto;
	JSClassCreator( JSClassCreator const & ); // not implemented
	JSClassCreator & operator=( JSClassCreator const & ); // not implemented
    public:
	/**
	   Begins setting up a new JS class, as a member of the given target object,
	   with the given class name and constructor function. The optionan internalFieldCount
	   can be set here or by calling SetINternalFieldCount().
	*/
	JSClassCreator( char const * className,
			Handle<Object> target,
			v8::InvocationCallback ctor,
			int internalFieldCount = 0 );
	/**
	   Does nothing.
	 */
	virtual ~JSClassCreator();

	/**
	   Sets the number of internal fields for this JS class (also
	   setable via the constructor).

	   Returns this object.
	*/
	virtual JSClassCreator & SetInternalFieldCount( int n );

	/**
	   Returns the constructor template.
	*/
	Handle<FunctionTemplate> CtorTemplate() const;

	/**
	   Returns this object's JS class name.
	*/
	char const * ClassName() const;

	/**
	   Returns the prototype object.
	*/
	Local<ObjectTemplate> Prototype() const;

	/**
	   Returns the target set in the constructor.
	*/
	Handle<Object> Target() const;

	/**
	   Finalizes the class creation process and adds the construtor
	   to this->Target(). No setter methods should be
	   called after this! Returns the target object which was set in the
	   ctor.

	   It returns the result of CtorTemplate()->GetFunction(), for reasons
	   too deep and dark to shed light on at the moment.
	*/
	Handle<Function> Seal();

	/**
	   Sets a property with the given name and value.

	   Returns this object.
	*/
	JSClassCreator & Set( char const * name, Handle< Data > const & value , PropertyAttribute attributes = v8::None );

	/**
	   Sets a function property with the given name and callback function.

	   Returns this object.
	*/
	JSClassCreator & Set( char const * name, InvocationCallback func, PropertyAttribute attributes = v8::None );

	/**
	   Adds a property accessor to the prototype object.

	   Returns this object.
	*/
	JSClassCreator & Set( char const * name,
			      AccessorGetter  	getter,
			      AccessorSetter  	setter = 0,
			      Handle< Value >  	data = Handle< Value >(),
			      AccessControl  	settings = v8::DEFAULT,
			      PropertyAttribute attribute = None
			      );
	/**
	   Sets this JS class to inherit the given parent.

	   Returns this object.
	*/
	JSClassCreator & Inherit( Handle< FunctionTemplate > parent );

	/**
	   Convenience form of Inherit( parent.CtorTemplate() ).

	   Returns this object.
	*/
	JSClassCreator & Inherit( JSClassCreator & parent );

    };

}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_V8_CLASSWRAP_H_INCLUDED */
