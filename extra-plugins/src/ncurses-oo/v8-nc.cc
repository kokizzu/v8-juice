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
#include <v8/juice/bind.h>
#include <v8/juice/convert.h>
#include <v8/juice/forwarding.h>
#include <v8/juice/plugin.h>
#include <v8/juice/cleanup.h>
#include <v8/juice/ClassBinder.h>
#include "ncwindow.hpp"
#include "ncmode.hpp"
#include "ncpalette.hpp"
#include "ncutil.hpp"
#include "ncwidgets.hpp"
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

    static Handle<Object> & ncJSObj()
    {
        static Handle<Object> bob;
        return bob;
    }

    using namespace ::v8;
    using namespace ::v8::juice::convert;
    namespace bind = ::v8::juice::bind;
    namespace juice = ::v8::juice;

    const char * strings::classWindow = "NCWindow";
    const char * strings::classPanel = "NCPanel";
    const char * strings::classPad = "NCPad";
    const char * strings::classFramedPad = "NCFramedPad";


    Persistent<Function> JWindow::js_ctor;

    typedef ::v8::juice::ClassBinder<JWindow> WindowBinder;
    typedef ::v8::juice::ClassBinder<JPanel> PanelBinder;
    typedef ::v8::juice::ClassBinder<JPad> PadBinder;
    typedef ::v8::juice::ClassBinder<JFramedPad> FramedPadBinder;

#define JSTR(X) ::v8::String::New(X)
#define TOSS(X) return ::v8::ThrowException(JSTR(X))
#define TOSSV(X) return ::v8::ThrowException(X)
#define PLUGIN_RTFM "RTFM: " v8_juice_HOME_PAGE "/wiki/PluginNCurses"
#define JS_WRAPPER(FN) static ::v8::Handle< ::v8::Value > FN( const ::v8::Arguments & argv )
#define ARGC int argc = argv.Length()
#define ASSERTARGS(COND) if(!(COND)) return ThrowException(String::New("Arguments assertion failed: " # COND))


#define JWIN_ARG(N,HND) JWindow * N = CastFromJS<JWindow>( HND );
#define JWIN_ARGT(N,HND) JWIN_ARG(N,HND); \
        if( ! N ) return ThrowException(String::New("Argument is not a JWindow handle!"));
#define JXX_THIS(T,N) T * N = CastFromJS<T>( argv.This() );           \
        if( ! N ) return ThrowException(String::New("This object is not (or is no longer) a " # T "!"));
#define JWIN_THIS JXX_THIS(JWindow,jwin)
#define JPAD_THIS JXX_THIS(JPad,jpad)

    typedef std::list<std::streambuf *> StreamBufferList;
    typedef std::multimap<JWindow *,std::streambuf *> StreamBufferMap;
    StreamBufferMap & nc_sbuf_map()
    {
        static StreamBufferMap bob;
        return bob;
    }


    typedef std::pair< NCWindow *, JWindow * > RipPair;
    struct ripoff_set : std::vector< RipPair >
    {
        ripoff_set()
        {}
        ~ripoff_set()
        {
#if 0
            // When i delete these objects here, i'm seeing segfaults post-main.
            this->nuke();
#endif
        }
        void nuke()
        {
            iterator it = this->begin();
            iterator et = this->end();
            for( ; et != it; ++it )
            {
#if 0
                // we're crashing sometimes post-main()/post-v8 if we do this:
                if( (*it).second ) delete (*it).second;
                else delete (*it).first;
#else
                delete (*it).first;
#endif
            }
            this->clear();
        }

    };

    static ripoff_set & ripset()
    {
        static ripoff_set bob;
        return bob;
    }


    static int argv_convert(  Arguments const & argv_,
                               std::vector< Handle<Value> > & tgt )
    {
        const int argc = argv_.Length();
        typedef std::vector< Handle<Value> > VT;
        if( (1 == argc) && argv_[0]->IsArray() )
        { // special case to assist in subclassing: if
            // first arg is an array then convert that array to
            // the argv...
            Local<Array> ao( Array::Cast( *argv_[0] ) );
            Local<Value> alenv( ao->Get(JSTR("length")));
            int lenv = alenv->IntegerValue();
            tgt.reserve(lenv);
            for( int i = 0; i < lenv; ++i )
            {
                tgt[i] = ao->Get(Integer::New(i));
            }
            return lenv;
        }
        else
        {
            tgt.reserve(argc);
            for( int i = 0; i < argc; ++i ) tgt[i] = argv_[i];
            return argc;
        }
    }

    JWindow * window_ctor( Arguments const & argv_, std::string & exceptionText )
    {
        typedef std::vector< Handle<Value> > VT;
        VT argv;
        const int argc = argv_convert(argv_,argv);
        JWindow * jw = 0;
        if( 0 == argc )
        { // default ctor - a wrapper for stdscr
            jw = new JWindow; // will call initwin() if needed
            jw->ncwin = new ncutil::NCWindow(::stdscr);
            return jw;
        }
        else if( (1 ==argc) && (argv[0]->IsExternal() ) )
        { // internal-use ctor
            Local<External> ex( External::Cast(*argv[0]) );
            NCWindow * w = bind::GetBoundNative<NCWindow>( ex->Value() );
            if( ! w )
            {
                exceptionText = "First argument failed type check!";
                return 0;
            }
            return new JWindow(w);
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
            jw = new JWindow(w);
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
            return jw;
        }
        else
        {
            exceptionText = "Invalid arguments! " PLUGIN_RTFM;
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


    JWindow::~JWindow()
    {
        if( this->ncwin && NCWindow::is_alive(this->ncwin) )
        {
            JWindow_remove_stream_redirects(this);
            //CERR << "~JWindow @"<<this<<" destroying NCWindow (or subclass) @"<<this->ncwin<<'\n';
            if( this->ncwin->ncwindow() == ::stdscr )
            {
                ripset().nuke();
            }
            delete this->ncwin;
        }
        else
        {
            //CERR << "SKIPPING destruction of JWindow (or subclass) @"<<this<<'\n';
        }
    }

    /**
       JS usage:

       - string func( int y, int x [, int maxlen] )
       - string func( [int maxlen] )
    */
    Handle<Value> JWindow::getstring( Arguments const & argv )
    {
        ARGC;
        typedef std::vector<char> VT;
        
        int y = -1;
        int x = -1;
        unsigned int len = 0;
        int apos = 0;
        if( (argc>1) && argv[apos]->IsInt32() )
        { // (int y, int x ...)
            y = JSToInt32(argv[apos++]);
            x = JSToInt32(argv[apos++]);
        }
        if( (argc > apos) && argv[apos]->IsInt32() )
        { // (... int len)
            len = JSToUInt32(argv[apos++]);
        }

        if( len < 1 ) len = 128; // arbitrarily chosen
        else if( len > 1024 ) len = 1024; // also arbitrary
        VT buf( len+1, 0);
        int rc = 0;
        if( (y<0) || (x<0) )
        {
            rc = this->ncwin->getstr( &buf[0], static_cast<int>(len) );
        }
        else
        {
            rc = this->ncwin->getstr( y, x, &buf[0], static_cast<int>(len) );
        }
        if( ERR == rc )
        {
            TOSS("native getstr() failed!");
        }
        return String::New( &buf[0] );
    }

    Handle<Value> JWindow::box( Arguments const & argv )
    {
        ARGC;
        chtype vert = (argc>0) ? JSToInt32(argv[0]) : 0;
        chtype hor = (argc>1) ? JSToInt32(argv[1]) : 0;
        return CastToJS( this->ncwin->box(vert,hor) );
    }
    Handle<Value> JWindow::border( Arguments const & argv )
    {
        ARGC;
#define NEXT(N) chtype a ## N = (argc>N) ? JSToInt32(argv[N]) : 0
        NEXT(0);
        NEXT(1);
        NEXT(2);
        NEXT(3);
        NEXT(4);
        NEXT(5);
        NEXT(6);
        NEXT(7);
#undef NEXT
        return CastToJS( this->ncwin->border(a0,a1,a2,a3,a4,a5,a6,a7) );
    }


    JPanel::~JPanel()
    {
        this->ncpnl = 0; // points to this->ncwin, which the parent class will free
    }

    JPad::~JPad()
    {
        if( this->ncpad && NCWindow::is_alive(this->ncpad) )
        {
            delete this->ncpad;
        }
    }

#if 0
    Handle<Value> JPad::setWindow( Arguments const & argv )
    {
        ARGC; ASSERTARGS( (argc>0) && (argc<4) );
        JWIN_ARGT(w,argv[0]);
        int gy = (argc>1) ? JSToInt32(argv[0]) : 1;
        int gh = (argc>2) ? JSToInt32(argv[1]) : 1;
        if( gy < 1 ) gy = 1;
        if( gh < 1 ) gh = 1;
        try
        {
            this->ncpad->setWindow(*w->ncwin,gy,gh);
        }
        catch(std::exception const & ex)
        {
            return ThrowException(String::New(ex.what()));
        }
        return argv[0];
    }
#endif


    JPanel * panel_ctor( Arguments const & _argv_, std::string & exceptionText )
    {
        typedef std::vector< Handle<Value> > VT;
        VT argv;
        const int argc = argv_convert(_argv_,argv);
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
            //CERR << argc << ": "<<l <<", "<< c <<", "<< y <<", "<< x <<"\n";
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


    /**
       Internal NCPad/NCFramedPad ctor impl. If (!fp) then this acts
       as NCPad, otherwise it constructs an NCFramedPad and sets fp to
       that NCFramedPad value.
    */
    JPad * pad_ctor_impl( Arguments const & argv_,
                          std::string & exceptionText,
                          JFramedPad ** fp
                          )
    {
        typedef std::vector< Handle<Value> > VT;
        VT argv;
        const int argc = argv_convert(argv_,argv);
        if(! ((argc>2) && (argc<6)))
        {
            exceptionText = "Constructor expects (NCWindow,int lines, int cols [, int gridVert=1, int gridHoriz=1])";
            return 0;
        }
        JWIN_ARG(par,argv[0]);
        if( ! par )
        {
            exceptionText = "First argument to constructor must be an NCWindow!";
            return 0;
        }
        NCPad * pad = 0;
        int l = JSToInt32(argv[1]);
        int c = JSToInt32(argv[2]);
        int gv = (argc>3) ? JSToInt32(argv[3]) : 1;
        int gh = (argc>4) ? JSToInt32(argv[4]) : 1;
        if( gv < 1 ) gv = 1;
        if( gh < 1 ) gh = 1;
        JPad * jp = 0;
        try
        {
            if( fp )
            {
                pad = new NCFramedPad(*par->ncwin,l,c,gv,gh); 
                jp = *fp = new JFramedPad(pad);
            }
            else
            {
                pad = new NCPad(l,c);
                pad->setWindow(*par->ncwin,gv,gh);
                jp = new JPad(pad);
            }
            return jp;
        }
        catch(std::exception const & ex)
        {
            exceptionText = ex.what();
            return 0;
        }
        exceptionText = "Unhandled ctor arguments!";
        return 0;
    }

    JPad * pad_ctor( Arguments const & argv, std::string & exceptionText )
    {
        return pad_ctor_impl( argv, exceptionText, (JFramedPad**)0 );
    }

    JFramedPad * pad_framed_ctor( Arguments const & argv, std::string & exceptionText )
    {
        JFramedPad * rc = 0;
        pad_ctor_impl( argv, exceptionText, &rc );
        return rc;
    }



    /**
       close() impl for JWindow and subclasses. BinderType must be
       the WeakJSClassBinder<T>, where T is one of: JWindow, JPanel,
       JPad.
    */
    template <typename BinderType>
    JS_WRAPPER(ncwin_close)
    {
        ARGC; ASSERTARGS((0==argc));
        JWIN_THIS;
        //CERR << "NC*.close() @"<<jwin<<'\n';
        JWindow_remove_stream_redirects( jwin );
        BinderType::DestroyObject(argv.This());
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

    JS_WRAPPER(nc_color_names)
    {
        Handle<Array> ar( Array::New(8) );
        int pos = 0;
#define COLOR(X) ar->Set(Integer::New(pos++), JSTR(# X))
        COLOR(red);
        COLOR(magenta);
        COLOR(green);
        COLOR(yellow);
        COLOR(blue);
        COLOR(cyan);
        COLOR(black);
        COLOR(white);
#undef COLOR
        return ar;
    }

    JS_WRAPPER(ncwin_getch)
    {
        ARGC; ASSERTARGS((0==argc));
        JWIN_THIS;
        return CastToJS( jwin->ncwin->getch() );
    }

    JS_WRAPPER(nc_getch)
    {
        return CastToJS( ::wgetch( ::stdscr ) );
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

    /**
       JS Usage: ncurses.KEY_CTRL(int | char), returns (asciiValue & 0x1f).
    */
    JS_WRAPPER(nc_KEY_CTRL)
    {
	ARGC; ASSERTARGS((1==argc));
        int k;
        if( argv[0]->IsString() )
        {
            std::string s( JSToStdString(argv[0]) );
            k = s.empty() ? -1 : s[0];
        }
        else
        {
            k = JSToInt32( argv[0] );
        }
        if( (k < 0) || (k>127) ) return Undefined(); // the value 64 comes from ncurses.h docs
	return Int32ToJS( 0x1f & k );
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

    JS_WRAPPER(nc_beep)
    {
        return CastToJS( ::beep() );
    }
    JS_WRAPPER(nc_flash)
    {
        return CastToJS( ::flash() );
    }
    JS_WRAPPER(nc_curs_set)
    {
        ARGC; ASSERTARGS((1==argc));
        return CastToJS( ::curs_set( JSToInt32(argv[0]) ) );
    }
    JS_WRAPPER(nc_napms)
    {
        ARGC; ASSERTARGS((1==argc));
        return CastToJS( ::napms( JSToInt32(argv[0]) ) );
    }

    JS_WRAPPER(ncpad_refresh)
    {
        ARGC; ASSERTARGS((0==argc) || (6==argc));
        JPAD_THIS;
        if( 0 == argc )
        {
            return CastToJS( jpad->refresh() );
        }
        int apos = 0;
        int i1 = JSToInt32(argv[apos++]);
        int i2 = JSToInt32(argv[apos++]);
        int i3 = JSToInt32(argv[apos++]);
        int i4 = JSToInt32(argv[apos++]);
        int i5 = JSToInt32(argv[apos++]);
        int i6 = JSToInt32(argv[apos++]);
        return CastToJS( jpad->prefresh(i1,i2,i3,i4,i5,i6) );
    }

    JS_WRAPPER(ncpad_noutrefresh)
    {
        ARGC; ASSERTARGS((0==argc) || (6==argc));
        JPAD_THIS;
        if( 0 == argc )
        {
            return CastToJS( jpad->noutrefresh() );
        }
        int apos = 0;
        int i1 = JSToInt32(argv[apos++]);
        int i2 = JSToInt32(argv[apos++]);
        int i3 = JSToInt32(argv[apos++]);
        int i4 = JSToInt32(argv[apos++]);
        int i5 = JSToInt32(argv[apos++]);
        int i6 = JSToInt32(argv[apos++]);
        return CastToJS( jpad->pnoutrefresh(i1,i2,i3,i4,i5,i6) );
    }

    JS_WRAPPER(nc_echo)
    {
        ARGC;
        if( (0 == argc) || argv[0]->BooleanValue() )
        {
            return CastToJS( ::echo() );
        }
        else
        {
            return CastToJS( ::noecho() );
        }
    }
    JS_WRAPPER(nc_noecho)
    {
        return CastToJS( ::noecho() );
    }

    JS_WRAPPER(nc_raw)
    {
        ARGC;
        if( (0 == argc) || argv[0]->BooleanValue() )
        {
            return CastToJS( ::raw() );
        }
        else
        {
            return CastToJS( ::noraw() );
        }
    }
    JS_WRAPPER(nc_noraw)
    {
        return CastToJS( ::noraw() );
    }
    JS_WRAPPER(nc_cbreak)
    {
        ARGC;
        if( (0 == argc) || argv[0]->BooleanValue() )
        {
            return CastToJS( ::cbreak() );
        }
        else
        {
            return CastToJS( ::nocbreak() );
        }
    }
    JS_WRAPPER(nc_nocbreak)
    {
        return CastToJS( ::nocbreak() );
    }

    JS_WRAPPER(nc_halfdelay)
    {
        ARGC; ASSERTARGS((argc==1));
        return CastToJS( ::halfdelay( JSToInt32(argv[0]) ) );
    }

    static int ripoff_init( NCWindow * win )
    {
        //CERR << "ripoff_init(@"<<win<<")\n";
        ripset().push_back( std::make_pair( win, (JWindow *)0 ) );
        return 0;
    }

    JS_WRAPPER(nc_ripoffline)
    {
        ARGC; ASSERTARGS((argc==1));
        //int from = JSToInt32(argv[0]);
        bool fromTop = JSToBool(argv[0]);
        //if( ! argv[1]->IsFunction() ) TOSS("Second argument must be-a Function!");
        //Local<Function> func( Function::Cast(*argv[0]) );
        return CastToJS( NCWindow::ripoffline( fromTop ? 1 : -1, ripoff_init ) );
    }

    JS_WRAPPER(nc_getrippedline)
    {
        ARGC; ASSERTARGS((argc==1));
        uint16_t pos = CastFromJS<uint16_t>( argv[0] );
        if( pos >= ripset().size() ) return Null();
        RipPair & p = ripset()[pos];
        if( p.second ) return WindowBinder::GetJSObject(p.second);

        bind::BindNative( p.first, p.first );
        Handle<Value> ex( External::New(p.first) );
        Local<Object> winjo = JWindow::js_ctor->NewInstance( 1, &ex );
        if( winjo.IsEmpty() || ! winjo->IsObject() ) return winjo;
        JWindow * jw = WindowBinder::GetSelf( winjo );
        assert(jw && "internal error - could not get native self from JS object!");
        p.second = jw;
        //if( ! argv[1]->IsFunction() ) TOSS("Second argument must be-a Function!");
        //Local<Function> func( Function::Cast(*argv[0]) );
        //return CastToJS( NCWindow::ripoffline( fromTop ? 1 : -1, ripoff_init ) );
        return winjo;
    }

    JS_WRAPPER(nc_popup_dialog)
    {
        ARGC; ASSERTARGS((argc>1) && (argc<6));
        std::string title( JSToStdString(argv[0]) );
        std::string text( JSToStdString(argv[1]) );
        int r = (argc>2) ? JSToInt32(argv[2]) : -1;
        int l = (argc>3) ? JSToInt32(argv[3]) : -1;
        int y = (argc>4) ? JSToInt32(argv[4]) : -1;
        int x = (argc>5) ? JSToInt32(argv[5]) : -1;

        return CastToJS( ncutil::popup_dialog(title,text,r,l,y,x) );
    }

#if 0
    JS_WRAPPER(nc_enable_inheritance)
    {
        ARGC; ASSERTARGS(argc<=1);
        bool b = (argc ? argv[0]->BooleanValue() : true);
        WindowBinder::SearchPrototypesForNative( b );
        PanelBinder::SearchPrototypesForNative( b );
        PadBinder::SearchPrototypesForNative( b );
        FramedPadBinder::SearchPrototypesForNative( b );

        WindowBinder::AllowCtorWithoutNew(b);
        PanelBinder::AllowCtorWithoutNew( b );
        PadBinder::AllowCtorWithoutNew( b );
        FramedPadBinder::AllowCtorWithoutNew( b );

        return BoolToJS(b);
    }
#endif

    JS_WRAPPER(nc_endwin)
    {
        ARGC; ASSERTARGS((0==argc));
        // FIXME: destroy open windows properly!
        ripset().nuke();
        NCMode::shutdown();
        return Undefined();
    }

    template <char const *&N>
    JS_WRAPPER(nc_window_tostring)
    {
        JWIN_THIS;
	std::ostringstream os;
	os << "[object "<<N;
        os << " name=["<<jwin->name()<<"]";
	os <<']';
	return CastToJS( os.str() );
    }


    static Handle<Value> SetupNCurses( Handle<Object> gl )
    {
        //CERR << "Initializing ncurses-oo wrapper plugin...\n";

        Handle<Object> ncobj( Object::New() );
        gl->Set(JSTR("ncurses"), ncobj);
        ncJSObj() = ncobj;
#define SETF(N,V) ncobj->Set(JSTR(N), FunctionTemplate::New(V)->GetFunction() )
        SETF("endwin", nc_endwin );
        SETF("color_pair",nc_color_pair);
        SETF("color_pairnum",nc_color_pairnum);
        SETF("KEY_F",nc_KEY_F);
        SETF("KEY_CTRL",nc_KEY_CTRL);
        SETF("intVal",nc_atoi);
        SETF("charVal",nc_itoa);
        SETF("screenHeight",nc_screen_height);
        SETF("screenWidth",nc_screen_width);
        SETF("beep",nc_beep);
        SETF("flash",nc_flash);
        SETF("getch",nc_getch);
        SETF("curs_set",nc_curs_set);
        SETF("napms",nc_napms);
        SETF("echo",nc_echo);
        SETF("noecho",nc_noecho);
        SETF("raw",nc_raw);
        SETF("no",nc_noraw);
        SETF("cbreak",nc_cbreak);
        SETF("nocbreak",nc_nocbreak);
        SETF("halfdelay",nc_halfdelay);
        SETF("colorNames",nc_color_names);
        SETF("ripoffline",nc_ripoffline);
        SETF("getRippedLine",nc_getrippedline);
        SETF("popupDialog",nc_popup_dialog);
        //SETF("enableWindowInheritance", nc_enable_inheritance );
#undef SETF
        {
            Handle<Array> rips( Array::New(5) );
            ncobj->Set( JSTR("rippedOffLines"), rips );
        }

#define SET_MAC(MAC) ncobj->Set(String::New(# MAC), Integer::New(MAC), ::v8::ReadOnly)
	//FIXME: add KEY_F(n) as a JS function

	SET_MAC(OK);
	SET_MAC(ERR);

	SET_MAC(KEY_F0);

	// Page/Movement keys:
	SET_MAC(KEY_DOWN);
	SET_MAC(KEY_UP);
	SET_MAC(KEY_LEFT);
	SET_MAC(KEY_RIGHT);
	SET_MAC(KEY_HOME);
	SET_MAC(KEY_BACKSPACE);
	SET_MAC(KEY_END);
	SET_MAC(KEY_DC);
	SET_MAC(KEY_IC);
	SET_MAC(KEY_NPAGE);
	SET_MAC(KEY_PPAGE);
        SET_MAC(KEY_ENTER);

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
	//ncobj->Set(String::New("CTRL_KEY"), Integer::New(0x1f), ::v8::ReadOnly);

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

        if(0) {
            /**
               Weird: this causes interactive loads of this plugin to hang:

stephan@jareth:~/cvs/v8-juice/extra-plugins/src/ncurses-oo$ v8-juice-shell 
V8 version 1.1.1.4
> load_plugin('v8-juice-ncurses-oo.so');

<waits until i press Ctrl-C or Ctrl-D, then...>

[object Object]

            */
            NCMode sentry;
            // The ACS_xxx defines don't actually work until curses has been started.
#define SET_MAC(MAC) ncobj->Set(String::New(# MAC), Integer::New(MAC), ::v8::ReadOnly)
            SET_MAC(ACS_ULCORNER);
            SET_MAC(ACS_LLCORNER);
            SET_MAC(ACS_URCORNER);
            SET_MAC(ACS_LRCORNER);
            SET_MAC(ACS_LTEE);
            SET_MAC(ACS_RTEE);
            SET_MAC(ACS_BTEE);
            SET_MAC(ACS_TTEE);
            SET_MAC(ACS_HLINE);
            SET_MAC(ACS_VLINE);
            SET_MAC(ACS_PLUS);
            SET_MAC(ACS_S1);
            SET_MAC(ACS_S9);
            SET_MAC(ACS_DIAMOND);
            SET_MAC(ACS_CKBOARD);
            SET_MAC(ACS_DEGREE);
            SET_MAC(ACS_PLMINUS);
            SET_MAC(ACS_BULLET);
            SET_MAC(ACS_LARROW);
            SET_MAC(ACS_RARROW);
            SET_MAC(ACS_DARROW);
            SET_MAC(ACS_UARROW);
            SET_MAC(ACS_BOARD);
            SET_MAC(ACS_LANTERN);
            SET_MAC(ACS_BLOCK);
            SET_MAC(ACS_S3);
            SET_MAC(ACS_S7);
            SET_MAC(ACS_LEQUAL);
            SET_MAC(ACS_GEQUAL);
            SET_MAC(ACS_PI);
            SET_MAC(ACS_NEQUAL);
            SET_MAC(ACS_STERLING);
            SET_MAC(ACS_BSSB);
            SET_MAC(ACS_SSBB);
            SET_MAC(ACS_BBSS);
            SET_MAC(ACS_SBBS);
            SET_MAC(ACS_SBSS);
            SET_MAC(ACS_SSSB);
            SET_MAC(ACS_SSBS);
            SET_MAC(ACS_BSSS);
            SET_MAC(ACS_BSBS);
            SET_MAC(ACS_SBSB);
            SET_MAC(ACS_SSSS);
#undef SET_MAC
        }

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
                .BindMemFunc<uint32_t, &JWindow::windowID>("windowID")

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
                .BindMemFunc< void, chtype, &JWindow::bkgdset >( "bkgdset" )
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
                .BindMemFunc< void, int, &JWindow::timeout > ("timeout")
                .BindMemFunc< int, bool, &JWindow::nodelay > ("nodelay")
                .BindMemFunc< int, JWindow *, &JWindow::overlay > ("overlay")
                .BindMemFunc< int, JWindow *, &JWindow::overwrite > ("overwrite")

                // Binary funcs:
                .BindMemFunc< int, int, int, &JWindow::mvwin >("mvwin")
                .BindMemFunc< int, int, int, &JWindow::move >("move")
                .BindMemFunc< int, int, int, &JWindow::getch >("mvgetch")
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
                //.BindMemFunc< int, int,int,int,int, &JWindow::mvcur>( "mvcur" )

                // N-aray and 5+-ary funcs:
                .BindMemFunc< &JWindow::getstring >("getstr" )
                .BindMemFunc< &JWindow::box >("box")
                .BindMemFunc< &JWindow::border >("border")
                .BindMemFunc< int, JWindow *, int, int, int, int, int, int, bool, &JWindow::copywin > ("copywin")

                // reminder: Set() returns a JSClassCreator, not a ClassBinder<>:
                .Set( "toString", nc_window_tostring<strings::classWindow> )
                .Set("captureCout", nc_capture_cout)
                .Set("captureCerr", nc_capture_cerr)
                .Set("captureReset", nc_capture_end)
                .Set("close",ncwin_close<WindowBinder> )
                .Set("getch",ncwin_getch)
                ;
            Handle<Function> ctor = bindW.Seal();
            bindW.AddClassTo(ncobj);
            if( JWindow::js_ctor.IsEmpty() )
            {
                JWindow::js_ctor = Persistent<Function>::New( ctor ); //bindW.CtorTemplate()->GetFunction() );
            }
        }


        /////////////////////////////////////////////////////////////////////////
        { // NCPanel class
            PanelBinder & bindP( PanelBinder::Instance() );
            bindP.Inherit( bindW );
            bindP
                .BindMemFunc<void, &JPanel::hide >( "hide" )
                .BindMemFunc<void, &JPanel::show >( "show" )
                .BindMemFunc<void, &JPanel::top >( "top" )
                .BindMemFunc<void, &JPanel::bottom >( "bottom" )
                .BindMemFunc<int, int, int, &JPanel::mvwin >( "mvwin" )
                .BindMemFunc<bool, &JPanel::hidden >( "hidden" )
                .BindMemFunc<void, &JPanel::interactivelyMove>( "interactivelyMove" )
                .Set( "toString", nc_window_tostring<strings::classPanel> )
                .Set("close",ncwin_close<PanelBinder> )
                ;
            bindP.Seal();
            bindP.AddClassTo(ncobj);
        }

        /////////////////////////////////////////////////////////////////////////
        PadBinder & bindD( PadBinder::Instance() );
        { // NCPad class
            bindD.Inherit( bindW );
            bindD
                .BindMemFunc< int, int, &JPad::requestOp >( "requestOp" )
                .BindMemFunc< int, int, int, int, int, int, int, &JPad::prefresh >( "prefresh" )
                .BindMemFunc< int, int, int, int, int, int, int, &JPad::pnoutrefresh >( "pnoutrefresh" )
                .BindMemFunc< int, &JPad::lineCount >( "lineCount" )
                .BindMemFunc< int, &JPad::columnCount >( "columnCount" )
                .BindMemFunc<bool, int ,&JPad::consumeKey>( "consumeKey" )
                .BindMemFunc<void, int, int ,&JPad::mapKeyToReq>( "mapKeyToReq" )
                .BindMemFunc<int,chtype,&JPad::echochar>( "echochar" )
                .BindMemFunc<int,int,&JPad::reqForKey>( "reqForKey" )
                .BindMemFunc<void,&JPad::inputLoop>( "inputLoop" )
                //.BindMemFunc< &JPad::setWindow >( "setWindow" )
                .Set( "toString", nc_window_tostring<strings::classPad> )
                .Set("refresh", ncpad_refresh)
                .Set("noutrefresh", ncpad_noutrefresh)
                .Set("close",ncwin_close<PadBinder> )
                ;
            Handle<Function> ctor = bindD.Seal();
#define SET(K) ctor->Set(JSTR(#K), Integer::New(NCPad::K))
            SET(PadReqFirstRequest);
            SET(PadReqRefresh);
            SET(PadReqUp);
            SET(PadReqDown);
            SET(PadReqLeft);
            SET(PadReqRight);
            SET(PadReqExit);
            SET(PadReqPageUp);
            SET(PadReqPageDown);
            SET(PadReqScrollToTop);
            SET(PadReqScrollToBottom);
            SET(PadReqIgnored);
            SET(PadReqUnknown);
            SET(PadReqLastRequest);
#undef SET
            bindD.AddClassTo(ncobj);
        }
        /////////////////////////////////////////////////////////////////////////
        { // NCFramedPad class
            FramedPadBinder & bindFP( FramedPadBinder::Instance() );
            bindFP.Inherit( bindD );
            bindFP.InheritNative<JWindow>();
            bindFP.Seal();
            bindFP.AddClassTo(ncobj);
        }

        {
            bool activeInheritance = true;
            WindowBinder::SearchPrototypesForNative( activeInheritance );
            PanelBinder::SearchPrototypesForNative( activeInheritance );
            PadBinder::SearchPrototypesForNative( activeInheritance );
            FramedPadBinder::SearchPrototypesForNative( activeInheritance );
        }

        return ncobj;
    }        

    V8_JUICE_PLUGIN_STATIC_INIT( SetupNCurses );

}}} // namespaces
