#if !defined(CODE_GOOGLE_COM_P_V8JUICE_PLUGIN_NCURSES_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8JUICE_PLUGIN_NCURSES_H_INCLUDED 1
/**
   Implementation code for v8 (JavaScript engine) bindings for sqlite3.

   v8: http://code.google.com/p/v8/

   sqlite3: http://sqlite.org

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: same as v8 (see below)
*/
// Copyright 2007-2009 Stephan Beal. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
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

#include "ncwindow.hpp"
#include "ncpanel.hpp"

#include <v8/juice/WeakJSClassCreator.h>

namespace v8 { namespace juice {
/**
   The v8::juice::nc namespace encapsulates ncurses bindings for
   v8. Call v8::juice::nc::SetupAddon() to install the ncurses API into
   an existing JS object (normally the global object). Functions in
   the native API marked with the V8_NCURSES_BINDING macro represent
   JS-side functions made available by this addon (and no native
   implementations exist using those names, so don't try to call them
   from native code).
*/
namespace nc {

    using namespace ::ncutil;

    class JWidgetBase
    {
    public:
        JWidgetBase()
        {}
        virtual ~JWidgetBase() {}
    };


    class JWindow : private NCMode,
                    public JWidgetBase
    {
    public:
        NCWindow * ncwin;
        JWindow()
        {}
        virtual ~JWindow()
        {
            if( this->ncwin ) delete this->ncwin;
        }
        static Persistent<Function> js_ctor;

    };

    class JPanel : public JWidgetBase
    {
        NCPanel * ncwin;
        JPanel()
        {}
        virtual ~JPanel()
        {}
    };

    class JPad : public JWidgetBase // JWindow?
    {
    };

    enum { library_version = 0x20090322 };


    /**
       Common WeakJSClassCreatorOps impl for JWindow and friends.
    */
    template <typename WrappedT,
              char const * &className,
              WrappedT * (*ctor_proxy)( Arguments const &, std::string & exceptionText ),
              void (*dtor_proxy)( WrappedT * )
        >
    struct BaseWeakOps
    {
        typedef WrappedT WrappedType;
        enum { ExtraInternalFieldCount = 0 };
        static char const * ClassName() { return className; }
        static WrappedType * Ctor( Arguments const & argv, std::string & exceptionText )
        {
            WrappedType * x = ctor_proxy(argv,exceptionText);
            if( x )
            {
                cleanup::AddToCleanup( x, cleanup_callback );
            }
            return x;
        }

        static void Dtor( WrappedType * obj )
        {
            cleanup::RemoveFromCleanup( obj );
            dtor_proxy( obj );
        }
    private:
        static void cleanup_callback( void * obj )
        {
            Dtor( static_cast<WrappedType *>( obj ) );
        }

    };

    struct strings
    {
        static const char * classWindow;
        static const char * classPanel;
        static const char * classPad;

        static const char * fClose;
        static const char * fEndwin;
        static const char * fGetch;
    };

    JWindow * window_ctor( Arguments const & argv, std::string & exceptionText );
    void window_dtor( JWindow * w );

} /* namespaces */

    template <>
    struct WeakJSClassCreatorOps< nc::JWindow >
        : nc::BaseWeakOps< nc::JWindow,
                           nc::strings::classWindow,
                           nc::window_ctor,
                           nc::window_dtor
                           >
    {
    };

//     template <>
//     struct WeakJSClassCreatorOps< JPanel >;
//     template <>
//     struct WeakJSClassCreatorOps< JPad >;

}} /* namespaces */

#define WEAK_CLASS_TYPE v8::juice::nc::JWindow
#include <v8/juice/WeakJSClassCreator-CastOps.h>

#endif /* CODE_GOOGLE_COM_P_V8JUICE_PLUGIN_NCURSES_H_INCLUDED */
