#if !defined(CODE_GOOGLE_COM_P_V8_V8_OVERLOADING_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_V8_OVERLOADING_H_INCLUDED 1
/*
   Extensions to the forwarding.h API. These are in a separate file
   because they require TypeList.h, and forwarding.h otherwise
   has no dependencies other than convert.h (which has no external
   deps aside from the STL).

   v8: http://code.google.com/p/v8/

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: same as v8 (see below)
*/
// Copyright 2007-2009 Stephan Beal. All rights reserved.
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

#include <vector>
#include "forwarding.h"
#include "TypeList.h"
namespace v8 { namespace juice { namespace convert {


    namespace Detail
    {
        template <typename FWD>
        struct FwdInvocableOne
        {
            static v8::Handle<v8::Value> Invocable( v8::Arguments const & argv )
            {
                if( (FWD::Arity<0) || (FWD::Arity == argv.Length()) )
                {
                    return FWD::Invocable( argv );
                }
                else
                {
                    v8::juice::convert::StringBuffer msg;
                    msg << "FwdInvocableOne<>::Invocable(): "
                        << argv.Callee()->GetName()
                        << "() called with "<<argv.Length()<<" arguments, "
                        << "but requires "<<FWD::Arity<<"!\n";
                    return v8::ThrowException(msg);
                }
            }
        };
        /**
           Internal dispatch end-of-list routine.
        */
        template <>
        struct FwdInvocableOne<v8::juice::convert::NilType>
        {
            static v8::Handle<v8::Value> Invocable( v8::Arguments const & argv )
            {
                return v8::ThrowException(v8::String::New("FwdInvocableOne<> end-of-list specialization should not have been called!"));
            }
        };

        /**
           FwdList must be-a TypeList of FuncOverloadForwardXXX classes,
           where XXX is an integer value (the function arity).

        */
        template <typename List>
        struct FwdInvocableList
        {
            static v8::Handle<v8::Value> Dispatch( v8::Arguments const & argv )
            {
                typedef typename List::Head FWD;
                typedef typename List::Tail Tail;
                if( (FWD::Arity == argv.Length()) || (FWD::Arity<0) )
                {
                    return FwdInvocableOne< FWD >::Invocable( argv );
                }
                {
                    return FwdInvocableList< Tail >::Dispatch(argv);
                }
                return v8::Undefined(); // can't get this far.
            }
        };

        /**
           End-of-list specialization.
        */
        template <>
        struct FwdInvocableList<v8::juice::convert::NilType>
        {
            static v8::Handle<v8::Value> Dispatch( v8::Arguments const & argv )
            {
                StringBuffer msg;
                msg << "FwdInvocableList<>::Dispatch() there is no overload for "
                    << argv.Callee()->GetName()
                    << "() taking "<<argv.Length()<<" arguments!\n";
                return v8::ThrowException( msg );
            }
        };

    }
    /**
       A helper class which allows us to dispatch to multiple
       overloaded native functions from JS, depending on the argument
       count.

       FwdList must be-a TypeList containing elements which have
       the following function:

       static v8::Handle<v8::Value> Invocable( v8::Arguments const & argv );
    */
    template < typename FwdList >
    struct OverloadInvocables
    {
        /**
           Tries to dispatch argv to one of the bound functions defined
           in FwdList, based on the number of arguments in argv and
           the Arity 

           Implements the v8::InvocationCallback interface.
        */
        static v8::Handle<v8::Value> Invocable( v8::Arguments const & argv )
        {
            typedef Detail::FwdInvocableList<FwdList> X;
            return X::Dispatch( argv );
        }
    };

}}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_V8_OVERLOADING_H_INCLUDED */
