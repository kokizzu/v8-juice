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
    const char * strings::fName = "name";
    const char * strings::fNameSet = "setName";

    Persistent<Function> JWindow::js_ctor;

    typedef ::v8::juice::ClassBinder<JWindow> WindowBinder;

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
        WindowBinder::DestroyObject(argv.This());
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
            WindowBinder & wr = WindowBinder::Instance();
            wr
                // Nullary:
                .BindMemFunc<std::string, &JWindow::name>( strings::fName )
                .BindMemFunc<int, &JWindow::lines>("lines")
                .BindMemFunc<int, &JWindow::cols>("cols")
                .BindMemFunc<int, &JWindow::tabsize>("tabsize")
                .BindMemFunc<int, &JWindow::height>("height")
                .BindMemFunc<int, &JWindow::width>("width")
                .BindMemFunc<int, &JWindow::begy>("begy")
                .BindMemFunc<int, &JWindow::begx>("begx")
                .BindMemFunc<int, &JWindow::maxx>("maxx")
                .BindMemFunc<int, &JWindow::maxy>("maxy")
                .BindMemFunc<chtype, &JWindow::inch>("inch")
                .BindMemFunc<int, &JWindow::insertln>("insertln")
                .BindMemFunc<chtype, &JWindow::getbkgd>("getbkgd")
                .BindMemFunc<int, &JWindow::erase>("erase")
                .BindMemFunc<int, &JWindow::clear>("clear")
                .BindMemFunc<int, &JWindow::clrtobot>("clrtobot")
                .BindMemFunc<int, &JWindow::delch>("delch")
                .BindMemFunc<int, &JWindow::deleteln>("deleteln")
                .BindMemFunc<int, &JWindow::touchwin>("touchwin")
                .BindMemFunc<int, &JWindow::untouchwin>("untouchwin")
                .BindMemFunc<bool, &JWindow::is_wintouched>("is_wintouched")
                .BindMemFunc<int, &JWindow::redrawwin>("redrawwin")
                .BindMemFunc<int, &JWindow::doupdate>("doupdate")
                .BindMemFunc<void, &JWindow::syncdown>("syncdown")
                .BindMemFunc<void, &JWindow::syncup>("syncup")
                .BindMemFunc<void, &JWindow::cursyncup>("cursyncup")
                .BindMemFunc<int, &JWindow::standout>("standout")
                .BindMemFunc<int, &JWindow::standend>("standend")
                .BindMemFunc<int, &JWindow::refresh>("refresh")
                .BindMemFunc<int, &JWindow::noutrefresh>("noutrefresh")
                .BindMemFunc<bool, &JWindow::has_mouse>("hasmouse")

                // Unary:
                .BindMemFunc<void, std::string, &JWindow::name>( strings::fNameSet )
                .BindMemFunc< int, chtype, &JWindow::insch >( "insch" )
                .BindMemFunc< int, int, &JWindow::insdelln >( "insdelln" )
                .BindMemFunc< int, std::string, &JWindow::insstr >( "insstr" )
                .BindMemFunc< int, chtype, &JWindow::attron >( "attron" )
                .BindMemFunc< int, chtype, &JWindow::attroff >( "attroff" )
                .BindMemFunc< int, chtype, &JWindow::attrset >( "attrset" )
                .BindMemFunc< int, short, &JWindow::color_set >( "color_set" )
                .BindMemFunc< int, chtype, &JWindow::bkgd >( "bkgd" )
                .BindMemFunc< int, bool, &JWindow::clearok >( "clearok" )
                .BindMemFunc< int, int, &JWindow::scroll >( "scroll" )
                .BindMemFunc< int, bool, &JWindow::scrollok >( "scrollok" )
                .BindMemFunc< int, bool, &JWindow::idlok >( "idlok" )
                .BindMemFunc< void, bool, &JWindow::idcok >( "idcok" )
                .BindMemFunc< int, bool, &JWindow::leaveok >( "leaveok" )
                .BindMemFunc< void, bool, &JWindow::immedok >( "immedok" )
                .BindMemFunc< int, bool, &JWindow::keypad >( "keypad" )
                .BindMemFunc< int, chtype, &JWindow::addch >( "addch" )
                .BindMemFunc< int, chtype, &JWindow::echochar >( "echochar" )
                .BindMemFunc< int, std::string, &JWindow::addstr >( "addstr" )
                .BindMemFunc< bool, int, &JWindow::is_linetouched >( "is_linetouched" )

                // Binary:
                .BindMemFunc< int, int, int, &JWindow::mvwin >("mvwin")
                .BindMemFunc< int, int, int, &JWindow::move >("move")
                .BindMemFunc< int, int, int, &JWindow::getch >("getch")
                .BindMemFunc< int, std::string, int, &JWindow::addstrn >("addstrn")
                .BindMemFunc< chtype, int, int, &JWindow::mvinch >("mvinch")
                .BindMemFunc< int, std::string, int, &JWindow::insstrn >("insstrn")
                .BindMemFunc< int, int, int, &JWindow::mvdelch >("mvdelch")
                .BindMemFunc< int, int, int, &JWindow::setscrreg >("setscrreg")
                .BindMemFunc< int, int, int, &JWindow::touchln >("touchln")
                .BindMemFunc< int, int, int, &JWindow::redrawln >("redrawln")

                // Ternary:
                .BindMemFunc< int, int,int,std::string, &JWindow::mvinsstr>( "mvinsstr" )
                .BindMemFunc< int, int,int,chtype, &JWindow::mvaddch>( "mvaddch" )
                .BindMemFunc< int, int, int, std::string, &JWindow::mvaddstr>( "mvaddstr" )
                .BindMemFunc< int, int,int,chtype, &JWindow::mvinsch>( "mvinsch" )

                // 4-ary:
                .BindMemFunc< int, int, int, std::string,int, &JWindow::mvaddstrn>( "mvaddstrn" )
                .BindMemFunc< int, int,int,std::string,int, &JWindow::mvinsstrn>( "mvinsstrn" )
                .BindMemFunc< int, int,int,int,int, &JWindow::mvcur>( "mvcur" )

                // reminder: Set() returns a JSClassCreator, not a ClassBinder<>:
                .Set(strings::fClose,ncwin_close)
                .Set(strings::fGetch,ncwin_getch)
                ;
            Handle<Function> ctor = wr.Seal();
            wr.AddClassTo(ncobj);
            if( JWindow::js_ctor.IsEmpty() )
            {
                JWindow::js_ctor = Persistent<Function>::New( ctor ); //wr.CtorTemplate()->GetFunction() );
            }
        }
        return ncobj;
    }        



    V8_JUICE_PLUGIN_STATIC_INIT( SetupNCurses );

}}} // namespaces
