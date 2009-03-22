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
#include <list>
#include <sstream>


#include <iostream> /* only for debuggering */
#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

#include <v8.h>
#include <v8/juice/juice.h>
#include <v8/juice/convert.h>
#include <v8/juice/forwarding.h>
#include <v8/juice/plugin.h>
#include <v8/juice/cleanup.h>
#include <v8/juice/ClassBinder.h>
#include "ncmode.hpp"
#include "ncpalette.hpp"
#include "ncutil.hpp"
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


    Persistent<Function> JWindow::js_ctor;

    typedef ::v8::juice::ClassBinder<JWindow> WindowBinder;
    typedef ::v8::juice::ClassBinder<JPanel> PanelBinder;

#define JSTR(X) ::v8::String::New(X)
#define TOSS(X) return ::v8::ThrowException(JSTR(X))
#define TOSSV(X) return ::v8::ThrowException(X)
#define PLUGIN_RTFM "RTFM: " v8_juice_HOME_PAGE "/wiki/PluginNCurses"
#define JS_WRAPPER(FN) static ::v8::Handle< ::v8::Value > FN( const ::v8::Arguments & argv )
#define ARGC int argc = argv.Length()
#define ASSERTARGS(COND) if(!(COND)) return ThrowException(String::New("Arguments assertion failed: " # COND))

#define JWIN_ARG(N,HND) JWindow * N = CastFromJS<JWindow>( HND );    \
    if( ! N ) return ThrowException(String::New("Argument is not a JWindow handle!"));
#define JWIN_THIS JWindow * jwin = CastFromJS<JWindow>( argv.This() ); \
        if( ! jwin ) return ThrowException(String::New("This object is not (or is no longer) a JWindow!"));

    typedef std::list<std::streambuf *> StreamBufferList;
    typedef std::multimap<JWindow *,std::streambuf *> StreamBufferMap;
    StreamBufferMap & nc_sbuf_map()
    {
        static StreamBufferMap bob;
        return bob;
    }


    JWindow * window_ctor( Arguments const & argv, std::string & exceptionText )
    {
        ARGC;
        if( ! ( (1==argc) || (2==argc) || (0==argc) || (4==argc) || (5==argc) ) )
        {
            exceptionText = "Incorrect ctor arguments!";
            return 0;
        }
        JWindow * jw = 0;
        if( 0 == argc )
        { // default ctor - a wrapper for stdscr
            jw = new JWindow; // will call initwin() if needed
            jw->ncwin = new ncutil::NCWindow(::stdscr);
            jw->canDestruct = true;
            return jw;
        }
        else if( (1 == argc) || (2 == argc) )
        { // ctor(NCWindow parent [, bool box = yes])
            JWindow * par = CastFromJS<JWindow>( argv[0] );
            if( ! par )
            {
                exceptionText = "First argument is not an NCWindow!";
                return 0;
            }
            bool box = (argc>1) ? argv[1]->BooleanValue() : true;
            NCWindow * w = 0;
            try
            {
                w = new NCWindow(*(par->ncwin),box);
            }
            catch(std::exception const & ex)
            {
                //CERR << "NCWindow ctor threw: "<<ex.what()<<'\n';
                exceptionText = ex.what();
                return 0;
            }
            jw = new JWindow(w,false);
            return jw;
        }
        else if( (4 == argc) || (5 == argc) )
        { // ctor(lines,cols,begy,begx)
            // ctor(NCWindow parent, lines,cols,begy,begx)
            int apos = 0;
            JWindow * par = 0;
            if( argc==5 )
            {
                par = CastFromJS<JWindow>( argv[apos++] );
                if( ! par )
                {
                    exceptionText = "Invalid arguments! " PLUGIN_RTFM;
                    return 0;
                }
            }
            jw = new JWindow; // will call initwin() if needed
            int l = JSToInt32(argv[apos++]);
            int c = JSToInt32(argv[apos++]);
            int y = JSToInt32(argv[apos++]);
            int x = JSToInt32(argv[apos++]);
            NCWindow * w = 0;
            try
            {
                w = par
                    ? new NCWindow(*(par->ncwin),l,c,y,x)
                    : new NCWindow(l,c,y,x);
            }
            catch(std::exception const & ex)
            {
                //CERR << "NCWindow ctor threw: "<<ex.what()<<'\n';
                exceptionText = ex.what();
                delete jw;
                return 0;
            }
            jw->ncwin = w;
            jw->canDestruct = (par ? false : true);
            return jw;
        }
        else
        {
            exceptionText = std::string("Invalid arguments! ")+ PLUGIN_RTFM;
        }
        return jw;
    }

    void JWindow_remove_stream_redirects( JWindow * win )
    {
        //CERR << "Looking for redirected streams.\n";
        StreamBufferMap & m = nc_sbuf_map();
        StreamBufferMap::iterator it = m.lower_bound(win);
        if( m.end() == it ) return;
        StreamBufferMap::iterator oldb = it;
        StreamBufferMap::iterator et = m.upper_bound(win);
        for( ; et != it; ++it )
        {
            delete( (*it).second );
            //popup_dialog( "Aaarrggl","Blub!" );
            //CERR << "Unhooking redirected stream.\n";
        }
        m.erase(oldb, et);
    }

    void window_dtor( JWindow * w )
    {
        //CERR << "Destroying wrapped NCWindow  object @"<<obj<<'\n';
        JWindow_remove_stream_redirects(w);
#if 0
        NCWindow * n = 0;
        for( NCWindow * p = w->ncwin->child(); p; p = n )
        {
            n = p->sibling();
            if( w->ncwin == p )
            {
                continue;
            }
            JWindow * b = bind::GetBoundNative<JWindow>( p );
            //CERR << "Cleaning up child window @"<<p<<'/'<<b<<'\n';
            if( b ) JWindow_remove_stream_redirects(b);
            //WindowBinder::DestroyObject( CastToJS(b) );
        }
#endif
        delete w;
    }

    JPanel * panel_ctor( Arguments const & argv, std::string & exceptionText )
    {
        ARGC;
        if( ! ( (0==argc) || (argc>1 && argc<5) ) )
        {
            exceptionText = "Incorrect ctor arguments!";
            return 0;
        }
        JPanel * jp = 0;
        if( 0 == argc )
        { // ctor()
            try
            {
                NCPanel * p = new NCPanel;
                jp = new JPanel(p);
                //CERR << "Created panel wrapper @"<<jp<<'\n';
                return jp;
            }
            catch(std::exception const & ex)
            {
                exceptionText = ex.what();
                return 0;
            }
        }
        else if( (argc>1) && (argc<5) )
        { // ctor(lines,cols[,y,x])
            int l = JSToInt32(argv[0]);
            int c = JSToInt32(argv[1]);
            int y = (argc>2) ? JSToInt32(argv[2]) : 0;
            int x = (argc>3) ? JSToInt32(argv[3]) : 0;
            try
            {
                NCPanel * p = new NCPanel(l,c,y,x);
                jp = new JPanel(p);
                return jp;
            }
            catch(std::exception const & ex)
            {
                exceptionText = ex.what();
                return 0;
            }
        }
        exceptionText = "Unhandled constructor arguments!";
        return jp;
    }

    void panel_dtor( JPanel * w )
    {
        //CERR << "Destroying wrapped NCPanel object @"<<w<<'\n';
        window_dtor(w);
    }

    JS_WRAPPER(ncwin_close)
    {
        ARGC; ASSERTARGS((0==argc));
        JWIN_THIS;
        WindowBinder::DestroyObject(argv.This());
        return Undefined();
    }

    Handle<Value> nc_capture_ostream( std::ostream & os,
                                      Arguments const & argv )
    {
        ARGC; ASSERTARGS(((argc==0) || (argc==1)));
        JWIN_THIS;
        uint32_t cattr = (argc>0) ? JSToUInt32(argv[0]) : 0;
        // The objects we create here are cleaned up via window_dtor()
        ncutil::NCStreamBuffer * sb = new ncutil::NCStreamBuffer( *jwin->ncwin, os, cattr );
        nc_sbuf_map().insert( std::make_pair(jwin, sb) );
        return Undefined();
    }

    JS_WRAPPER(nc_capture_cout)
    {
        return nc_capture_ostream( std::cout, argv );
    }
    JS_WRAPPER(nc_capture_cerr)
    {
        return nc_capture_ostream( std::cerr, argv );
    }

    JS_WRAPPER(nc_capture_end)
    {
        JWIN_THIS;
        JWindow_remove_stream_redirects( jwin );
        return Undefined();
    }

//     JS_WRAPPER(nc_capture_end)
//     {
//         return xxx;
//     }

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

    JS_WRAPPER(nc_color_pair)
    {
	ARGC; ASSERTARGS((2==argc));
        std::string fg = JSToStdString(argv[0]);
        std::string bg = JSToStdString(argv[1]);
        return CastToJS<uint32_t>( ncutil::color_pair( fg, bg ) );
    }

    JS_WRAPPER(nc_color_pairnum)
    {
	ARGC; ASSERTARGS((2==argc));
        std::string fg = JSToStdString(argv[0]);
        std::string bg = JSToStdString(argv[1]);
        return CastToJS<uint16_t>( ncutil::color_pairnum_for_names( fg, bg ) );
    }

    JS_WRAPPER(nc_KEY_F)
    {
	ARGC; ASSERTARGS((1==argc));
        int k = JSToInt32( argv[0] );
        if( (k < 0) || (k>64) ) return Undefined(); // the value 64 comes from ncurses.h docs
	return Int32ToJS( KEY_F( k ) );
    }

    JS_WRAPPER(nc_atoi)
    {
	ARGC; ASSERTARGS((1==argc));
        std::string a( JSToStdString(argv[0]) );
        if( a.empty() ) return Undefined();
	return Int32ToJS( a[0] );
    }

    JS_WRAPPER(nc_itoa)
    {
	ARGC; ASSERTARGS((1==argc));
        int a( JSToInt32(argv[0]) );
        if( (a < 0) || (a>127) ) return Undefined();
        char const ch[2] = { (char)a, 0 };
	return CastToJS( (char const *) ch );
    }

    JS_WRAPPER(nc_screen_width)
    {
	return CastToJS( stdscr ? stdscr->_maxx+1 : (int)0);
    }
    JS_WRAPPER(nc_screen_height)
    {
	return CastToJS( stdscr ? stdscr->_maxy+1 : (int)0 );
    }


    static Handle<Value> SetupNCurses( Handle<Object> gl )
    {
        //CERR << "Initializing ncurses-oo wrapper plugin...\n";

        Handle<Object> ncobj( Object::New() );
        gl->Set(JSTR("ncurses"), ncobj);

#define SETF(N,V) ncobj->Set(JSTR(N), FunctionTemplate::New(V)->GetFunction() )
        SETF("endwin", nc_endwin );
        SETF("color_pair",nc_color_pair);
        SETF("color_pairnum",nc_color_pairnum);
        SETF("KEY_F",nc_KEY_F);
        SETF("intVal",nc_atoi);
        SETF("charVal",nc_itoa);
        SETF("screenHeight",nc_screen_height);
        SETF("screenWidth",nc_screen_width);
#undef SETF

#define SET_MAC(MAC) ncobj->Set(String::New(# MAC), Integer::New(MAC), ::v8::ReadOnly)
	//FIXME: add KEY_F(n) as a JS function

	SET_MAC(OK);
	SET_MAC(ERR);

	SET_MAC(KEY_F0);

	// Page/Movement keys:
	SET_MAC(KEY_DOWN);
	SET_MAC(KEY_DOWN);
	SET_MAC(KEY_UP);
	SET_MAC(KEY_UP);
	SET_MAC(KEY_LEFT);
	SET_MAC(KEY_LEFT);
	SET_MAC(KEY_RIGHT);
	SET_MAC(KEY_RIGHT);
	SET_MAC(KEY_HOME);
	SET_MAC(KEY_HOME);
	SET_MAC(KEY_BACKSPACE);
	SET_MAC(KEY_BACKSPACE);
	SET_MAC(KEY_END);
	SET_MAC(KEY_END);
	SET_MAC(KEY_DC);
	SET_MAC(KEY_DC);
	SET_MAC(KEY_IC);
	SET_MAC(KEY_IC);
	SET_MAC(KEY_NPAGE);
	SET_MAC(KEY_NPAGE);
	SET_MAC(KEY_PPAGE);
	SET_MAC(KEY_PPAGE);

	// color/state attributes:
	SET_MAC(A_BOLD);
	SET_MAC(A_BLINK);
	SET_MAC(A_DIM);
	SET_MAC(A_STANDOUT);
	SET_MAC(A_UNDERLINE);
	SET_MAC(A_REVERSE);
	SET_MAC(A_NORMAL);
	SET_MAC(A_COLOR);
	SET_MAC(A_CHARTEXT);
	SET_MAC(A_ATTRIBUTES);
	SET_MAC(A_ALTCHARSET);
	SET_MAC(A_INVIS);
	SET_MAC(A_PROTECT);
	SET_MAC(A_HORIZONTAL);
	SET_MAC(A_LEFT);
	SET_MAC(A_LOW);
	SET_MAC(A_RIGHT);
	SET_MAC(A_TOP);
	SET_MAC(A_VERTICAL);

	// Misc weird entries:
	SET_MAC(KEY_MOUSE);
	SET_MAC(KEY_RESIZE);
	SET_MAC(KEY_EVENT);
	SET_MAC(KEY_MAX);

	SET_MAC(NCURSES_MOUSE_VERSION);
	ncobj->Set(String::New("CTRL_KEY"), Integer::New(0x1f), ::v8::ReadOnly);

#if NCURSES_MOUSE_VERSION
#define SET_BTN(B) ncobj->Set(String::New(# B), Integer::New(B), ::v8::ReadOnly)
	SET_BTN(BUTTON1_RELEASED);
	SET_BTN(BUTTON1_PRESSED);
	SET_BTN(BUTTON1_CLICKED);
	SET_BTN(BUTTON1_DOUBLE_CLICKED);
	SET_BTN(BUTTON1_TRIPLE_CLICKED);
	SET_BTN(BUTTON2_RELEASED);
	SET_BTN(BUTTON2_PRESSED);
	SET_BTN(BUTTON2_CLICKED);
	SET_BTN(BUTTON2_DOUBLE_CLICKED);
	SET_BTN(BUTTON2_TRIPLE_CLICKED);
	SET_BTN(BUTTON3_RELEASED);
	SET_BTN(BUTTON3_PRESSED);
	SET_BTN(BUTTON3_CLICKED);
	SET_BTN(BUTTON3_DOUBLE_CLICKED);
	SET_BTN(BUTTON3_TRIPLE_CLICKED);
	SET_BTN(BUTTON4_RELEASED);
	SET_BTN(BUTTON4_PRESSED);
	SET_BTN(BUTTON4_CLICKED);
	SET_BTN(BUTTON4_DOUBLE_CLICKED);
	SET_BTN(BUTTON4_TRIPLE_CLICKED);
#undef SET_BTN
	SET_MAC(BUTTON_CTRL);
	SET_MAC(BUTTON_SHIFT);
	SET_MAC(BUTTON_ALT);
	SET_MAC(REPORT_MOUSE_POSITION);
	SET_MAC(ALL_MOUSE_EVENTS);
#endif // NCURSES_MOUSE_VERSION

#undef SET_MAC

        WindowBinder & bindW = WindowBinder::Instance();
        { // NCWindow class:
            bindW
                // Nullary funcs:
                .BindMemFunc<std::string, &JWindow::name>( "name" )
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

                // Unary funcs:
                .BindMemFunc<void, std::string, &JWindow::name>( "setName" )
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

                // Binary funcs:
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

                // Ternary funcs:
                .BindMemFunc< int, int,int,std::string, &JWindow::mvinsstr>( "mvinsstr" )
                .BindMemFunc< int, int,int,chtype, &JWindow::mvaddch>( "mvaddch" )
                .BindMemFunc< int, int, int, std::string, &JWindow::mvaddstr>( "mvaddstr" )
                .BindMemFunc< int, int,int,chtype, &JWindow::mvinsch>( "mvinsch" )

                // 4-ary funcs:
                .BindMemFunc< int, int, int, std::string,int, &JWindow::mvaddstrn>( "mvaddstrn" )
                .BindMemFunc< int, int,int,std::string,int, &JWindow::mvinsstrn>( "mvinsstrn" )
                .BindMemFunc< int, int,int,int,int, &JWindow::mvcur>( "mvcur" )

                // reminder: Set() returns a JSClassCreator, not a ClassBinder<>:
                .Set("captureCout", nc_capture_cout)
                .Set("captureCerr", nc_capture_cerr)
                .Set("captureReset", nc_capture_end)
                .Set("close",ncwin_close)
                .Set("getch",ncwin_getch)
                ;
            Handle<Function> ctor = bindW.Seal();
            bindW.AddClassTo(ncobj);
            if( JWindow::js_ctor.IsEmpty() )
            {
                JWindow::js_ctor = Persistent<Function>::New( ctor ); //bindW.CtorTemplate()->GetFunction() );
            }
        }

        {
            PanelBinder & bindP( PanelBinder::Instance() );
            bindP.Inherit( bindW );
            bindP
                .BindMemFunc<void, &JPanel::hide >( "hide" )
                .BindMemFunc<void, &JPanel::show >( "show" )
                .BindMemFunc<void, &JPanel::top >( "top" )
                .BindMemFunc<void, &JPanel::bottom >( "bottom" )
                .BindMemFunc<int, int, int, &JPanel::mvwin >( "mvwin" )
                .BindMemFunc<bool, &JPanel::hidden >( "hidden" )
                ;
            bindP.Seal();
            bindP.AddClassTo(ncobj);
        }

        return ncobj;
    }        



    V8_JUICE_PLUGIN_STATIC_INIT( SetupNCurses );

}}} // namespaces
