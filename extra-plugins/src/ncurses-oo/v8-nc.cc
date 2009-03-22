#ifdef NDEBUG
#undef NDEBUG
#endif
/**
   Implementation code for v8 (JavaScript engine) bindings for ncurses.

   v8: http://code.google.com/p/v8/

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: same as v8 (see below)

   Pedantic license note: much of this code was ported from the
   SpiderApe JS/ncurses bindings (http://SpiderApe.sf.net). Though
   that code is GPL, i wrote it, and am re-licensing this copy to
   conform to v8 conventions.
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

#include <assert.h>
#include <ncurses.h>
#include <panel.h>

#include <vector>
#include <map>
#include <sstream>


#include <iostream> /* only for debuggering */
#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

#include <v8.h>
#include <v8/juice/convert.h>
#include <v8/juice/forwarding.h>
#include <v8/juice/plugin.h>
#include <v8/juice/cleanup.h>
#include <v8/juice/ClassBinder.h>
#include "ncmode.hpp"
#include "ncpalette.hpp"
#include "ncstream.hpp"
#include "v8-nc.h"
/**
   Convenience macro for marking wrapper functions.
*/
#define JS_WRAPPER(FN) static ::v8::Handle< ::v8::Value > FN( const ::v8::Arguments & argv )

namespace v8 {
namespace juice {
namespace convert {


} // namespace convert

namespace nc {
    using namespace ::v8;
    using namespace ::v8::juice::convert;
    namespace bind = ::v8::juice::bind;
    namespace juice = ::v8::juice;

    const char * strings::classWindow = "NCWindow";
    const char * strings::classPanel = "NCPanel";
    const char * strings::classPad = "NCPad";

    const char * strings::fClose = "close";
    const char * strings::fEndwin = "endwin";
    const char * strings::fGetch = "getch";

    Persistent<Function> JWindow::js_ctor;

    typedef ::v8::juice::ClassBinder<JWindow> WeakWindow;

#define JSTR(X) ::v8::String::New(X)
#define TOSS(X) return ::v8::ThrowException(JSTR(X))
#define TOSSV(X) return ::v8::ThrowException(X)
#define PLUGIN_RTFM "RTFM: " v8_juice_HOME_PAGE "/wiki/PluginSqlite3"
#define JS_WRAPPER(FN) static ::v8::Handle< ::v8::Value > FN( const ::v8::Arguments & argv )
#define ARGC int argc = argv.Length()
#define ASSERTARGS(COND) if(!(COND)) return ThrowException(String::New("Arguments assertion failed: " # COND))

#define JWIN_ARG(N,HND) JWindow * N = CastFromJS<JWindow>( HND );    \
    if( ! N ) return ThrowException(String::New("Argument is not a JWindow handle!"));
#define JWIN_THIS JWindow * jwin = CastFromJS<JWindow>( argv.This() ); \
        if( ! jwin ) return ThrowException(String::New("This object is not (or is no longer) a JWindow!"));


    /**
       kludge to work around an uncertain type size:
       chtype is defined in ncurses.h, but we're going
       to shadow it for CastTo/FromJS() purposes.
    */
    typedef uint32_t chtype;

#if 0
    /**
       A shared place to store WINDOW-to-T mappings.
    */
    template <typename T>
    static std::map<WINDOW *,T> & window_map()
    {
	static std::map<WINDOW *,T> bob;
	return bob;
    }
   
    /**
       A shared place to store WINDOW-to-T multi-mappings.
    */
    template <typename T>
    static std::multimap<WINDOW *,T> & window_multimap()
    {
	static std::multimap<WINDOW *,T> bob;
	return bob;
    }
    typedef std::multimap<WINDOW *,::std::streambuf *> WindowStreamMap;
    static WindowStreamMap & captured_streams()
    {
	return window_multimap< ::std::streambuf *>();
    }
#endif

    JWindow * window_ctor( Arguments const & argv, std::string & exceptionText )
    {
        ARGC;
        if( ! ( (0==argc) || (4==argc) || (5==argc) ) )
        {
            exceptionText = "Incorrect ctor arguments!";
            return 0;
        }
        JWindow * jw = 0;
        if( 0 == argc )
        {
            jw = new JWindow;
            jw->ncwin = new ncutil::NCWindow(::stdscr);
            return jw;
        }

        CERR << "NYI!\n";
        return jw;
    }
    void window_dtor( JWindow * w )
    {
        /**
           GC may try to clean up child windows, and we don't want that.
           They are owned by their parents, which may not like it if the
           children disappear.
        */
        if( w->ncwin && w->ncwin->parent() )
        {
            CERR << "Warning: not deleting parented item @"<<w<<'\n';
            return;
        }
        //CERR << "Destroying wrapped "<<ClassName()<<" object @"<<obj<<'\n';
        delete w;
    }

    JS_WRAPPER(ncwin_close)
    {
        ARGC; ASSERTARGS((0==argc));
        JWIN_THIS;
        WeakWindow::DestroyObject(argv.This());
        return Undefined();
    }

    JS_WRAPPER(nc_endwin)
    {
        ARGC; ASSERTARGS((0==argc));
        // FIXME: destroy open windows properly!
        NCMode::shutdown();
        return Undefined();
    }
    JS_WRAPPER(ncwin_getch)
    {
        ARGC; ASSERTARGS((0==argc));
        JWIN_THIS;
        return CastToJS( jwin->ncwin->getch() );
    }

    static Handle<Value> SetupNCurses( Handle<Object> gl )
    {
        CERR << "Initializing ncurses-oo wrapper plugin...\n";

        Handle<Object> ncobj( Object::New() );
        gl->Set(JSTR("ncurses"), ncobj);
        ncobj->Set(JSTR(strings::fEndwin), FunctionTemplate::New(nc_endwin)->GetFunction());

        typedef Handle<Value> HV;
        {
            WeakWindow & wr = WeakWindow::Instance();
            wr
                .Set(strings::fClose,ncwin_close)
                .Set(strings::fGetch,ncwin_getch);
            wr.Seal();
            wr.AddClassTo(ncobj);
            if( JWindow::js_ctor.IsEmpty() )
            {
                JWindow::js_ctor = Persistent<Function>::New( wr.CtorTemplate()->GetFunction() );
            }
        }
        return ncobj;
    }        



    V8_JUICE_PLUGIN_STATIC_INIT( SetupNCurses );

}}} // namespaces
