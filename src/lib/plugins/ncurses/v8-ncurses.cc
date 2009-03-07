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
// Copyright 2007-2009 the V8 project authors. All rights reserved.
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


#include <ncurses.h>
#include <panel.h>

#include <v8.h>
#include <v8/juice/convert.h>
#include <v8/juice/plugin.h>
#include "ncmode.hpp"
#include "nccolors.hpp"
#include "ncstream.hpp"

/**
   Convenience macro for marking wrapper functions.
*/
#define JS_WRAPPER(FN) static ::v8::Handle< ::v8::Value > FN( const ::v8::Arguments & argv )

namespace v8 {
namespace juice {
namespace nc {
    using namespace ::v8;
    using namespace ::v8::juice::convert;
    namespace bind = ::v8::juice::bind;

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

    /**
    */
    class NCWrapper
    {
    public:
	WINDOW * win;
	PANEL * pnl;
	NCMode nc_sentry;
	Local< Value > jsval;
	Handle<Object> ncObj; // the 'ncurses' JS object
	static void const * bind_context;
	static NCWrapper * StdScr;
	
	/**
	   TODOs:

	   - add parent/child relationships so on delwin()
	   we can clean up subwindows automatically.
	*/
	/**
	   Initiialized a new new wrapper object.
	*/
	explicit NCWrapper( WINDOW * w )
	    : win(0),
	      pnl(0),
	      nc_sentry(),
	      jsval(),
	      ncObj( Object::Cast( *(Context::GetCurrent()->Global()->Get(String::New("ncurses"))) ) )
	{
	    this->SetWindow(w);
	    bind::BindNative( NCWrapper::bind_context, this, this );
	    bind::BindNative( 0, this, this ); // So CastFromJS() will work
	}

	/**
	   Unbinds this object from the JS/native bindings. Does not
	   (cannot?) tell JS to clean up its references, though.
	*/
	~NCWrapper()
	{
	    bind::UnbindNative( NCWrapper::bind_context, this, this );
	    bind::UnbindNative( 0, this, this );
	    if( this->pnl )
	    {
		PANEL * p = this->pnl;
		this->SetPanel( 0 );
		del_panel( p );
	    }
	    if( this->win )
	    {
		WINDOW * w = this->win;
		this->SetWindow(0);
		if( ::stdscr == w )
		{
		    // we don't need to call endwin(): this->nc_sentry() will do it
		}
		else
		{
		    ::delwin( w );
		}
	    }
	}

	bool SetWindow( WINDOW * w )
	{
	    if( this->win == w ) return true;
	    if( this->win )
	    {
		bind::UnbindNative( NCWrapper::bind_context, this, this->win );
		bind::UnbindNative( 0, this->win, this->win );
		{ // KLUDGE #1: remove stream-to-window redirects:
		    WindowStreamMap & str = captured_streams();
		    typedef WindowStreamMap::iterator WIT;
		    WIT stit = str.lower_bound(this->win);
		    WIT head = stit;
		    WIT stet = str.upper_bound(this->win);
		    for( ; stet != stit; ++stit )
		    {
			delete (*stit).second;
		    }
		    str.erase( head, stet );
		} // /KLUDGE #1
	    }
	    this->win = w;
	    if( this->win )
	    {
		this->jsval = External::New(this);
		::keypad( w, true );
		::meta( w, true );
		bind::BindNative( 0, this, this->win ); // So default CastFromJS() will work
		return bind::BindNative( NCWrapper::bind_context, this, this->win );
	    }
	    this->jsval = External::New(0);
	    return true;
	}

	bool SetPanel( PANEL * p )
	{
	    if( this->pnl == p ) return true;
	    if( this->pnl )
	    {
		bind::UnbindNative( 0, this, this->pnl );
		bind::UnbindNative( NCWrapper::bind_context, this, this->pnl );
	    }
	    this->pnl = p;
	    if( this->pnl )
	    {
		bind::BindNative( 0, this, this->pnl ); // So default CastFromJS() will work
		return bind::BindNative( NCWrapper::bind_context, this, this->pnl );
	    }
	    return true;
	}

	/**
	   Returns the NCWrapper object (if any) bound to the given JS value.
	*/
	static NCWrapper * GetNative( Handle<Value> const id )
	{
	    return bind::GetBoundNative<NCWrapper>( NCWrapper::bind_context, id );
	}

    };
    static int bogo_placeholder;
    void const * NCWrapper::bind_context = &bogo_placeholder;
    NCWrapper * NCWrapper::StdScr = 0;


    /** Internal helper for implementing nc_ripoffline(). */
    struct ripoff
    {
    public:
	static int addpos;
	static int runpos;

	typedef std::vector<NCWrapper*> BinderVec;
	typedef Persistent<Function> FH;
	typedef std::map<int, FH > FuncMap;
	static FuncMap & functions()
	{
	    static FuncMap bob;
	    if( bob.empty() )
	    {
		for( int i = 0; i < 5; ++i )
		{
		    bob[i] = FH();
		}
	    }
	    return bob;
	}

	static BinderVec & binders()
	{
	    static BinderVec bob;
	    if( bob.empty() )
	    {
		for( int i = 0; i < 5; ++i )
		{
		    bob.push_back( 0 );
		}
	    }
	    return bob;
	}

	/**
	   This function gets installed as a ripoffline()
	   handler.

	   Calls the "next" ripoff handler in the list
	   (created by add_handler()). Returns -1 on error, 0
	   on success.
	*/
	static int handler( WINDOW * w, int l )
	{
	    int rp = runpos++;
	    FH func = (rp < 5)
		? functions()[rp]
		: FH();
	    if( func.IsEmpty() || ! func->IsFunction() )
	    {
		//JS_ReportError( context, "ripoff::handler() no function found for call #%d", rp );
		return -1;
	    }
	    //JSObject * obj = JS_GetGlobalObject( context );
	    Handle<Object> jobj = Context::GetCurrent()->Global(); // not technically correct, but we don't have access to the JS caller/callee here!
	    NCWrapper * wr = binders()[rp] = new NCWrapper( w );
	    Handle<Value> argv[] = {
	    wr->jsval,
	    IntToJS(l)
	    };
	    TryCatch tryer;
	    func->Call( jobj, 2, argv );
	    // we have no direct way of reporting the call result to the caller, so we'll ignore it.
	    // We ignore any exception - maybe we shouldn't use TryCatch, and see what happens?


	    // We're guessing on the return value here: ncurses does not document
	    // what this handler should return and in fact appears
	    // to ignore the return value.
	    return 0;
	}

	/**
	   To be called by nc_ripoffline() to add a function.
	   Neither cx nor f may be 0.
	*/
	static bool add_handler( Handle<Function> const f )
	{
	    if( f.IsEmpty() || ! f->IsFunction() )
	    {
		//JS_ReportError( context, "ape::nc::ripoff::add_handler(): argument is-no-a Function." );
		return false;
	    }
	    int ap = addpos++;
	    if( ap > 4 )
	    {
		//JS_ReportError( context, "nc_ripoffline() is only callable up to 5 times for any given application." );
		return false;
	    }
	    functions()[ap] = FH::New(f);
	    return true;
	}

	/**
	   Should be called by nc_endwin() to reset
	   this object for the next (potential) use.
	*/
	static void reset()
	{
	    for( int i = 0; i < 5; ++i )
	    {
		delete binders()[i];
		binders()[i] = 0;
		functions()[i] = FH();
	    }
	    addpos = runpos = 0;
	}
    };
    int ripoff::addpos = 0;
    int ripoff::runpos = 0;

/**
   Convenience macro for marking wrapper functions.
*/
#define JS_WRAPPER(FN) static ::v8::Handle< ::v8::Value > FN( const ::v8::Arguments & argv )

#define ASSERTARGS(FUNCNAME,COND) const int argc = argv.Length();	\
	if(!(COND)) return ThrowException(String::New(# FUNCNAME "(): assertion failed: " # COND))

#define WR_ARG(HND) NCWrapper * wr = NCWrapper::GetNative( HND );			\
	if( ! wr ) return ThrowException(String::New("Argument is not a proper handle!"))
#define WR_ARG_OR(HND,RV) NCWrapper * wr = NCWrapper::GetNative( HND );	\
	if( ! wr ) return RV
#define WR_ARG_N(N) NCWrapper * wr ## N = NCWrapper::GetNative( argv[N] );	\
	if( ! wr ## N ) return ThrowException(String::New("Argument #" # N " is not a proper handle!"))
#define PNL_ARG(HND) WR_ARG(HND); if( ! wr->pnl ) return ThrowException(String::New("Window handle has no associated panel!"))
#define WIN_IS_PAD(WIN) ((WIN->_flags & _ISPAD))

    int screen_width()
    {
	return stdscr ? stdscr->_maxx+1 : 0;
    }
    int screen_height()
    {
	return stdscr ? stdscr->_maxy+1 : 0;
    }
    

    JS_WRAPPER(nc_initscr)
    {
	ASSERTARGS(nc_initscr,(0 == argc));
	if( NCWrapper::StdScr )
	{ // already initialized
	    return Boolean::New(true);
	}

	NCWrapper * wr = new NCWrapper(0); // will call initscr() if needed;
	if( ! ::stdscr )
	{
	    return Null();
	}
	wr->SetWindow( ::stdscr );
	NCWrapper::StdScr = wr;

#if 0
	// FIXME: some ncurses "consts" are not set up until after initscr() is called!
	// Set those up here.
	if( ! create_ncurses_objects(cx,globobj) )
	{
	    return ape::set_pending_exception( cx, "nc_initscr(): Error setting up ncurses global object!" );
	}
#endif

#define SET_MAC(MAC) wr->ncObj->Set(String::New(# MAC), Integer::New(MAC), ::v8::ReadOnly)
	// The ACS_xxx defines don't actually work until curses has been started.
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
	Context::GetCurrent()->Global()->Set(String::New("stdscr"),wr->jsval);
	wr->ncObj->Set(String::New("stdscr"),wr->jsval);
	return wr->jsval;
    }

    JS_WRAPPER(nc_endwin)
    {
	if( NCWrapper::StdScr )
	{
	    // FIXME: clean up all open NCWrapper objects!
	    // To consider: unbind all ncurses functions except for initscr()?
	    ripoff::reset();
	    Context::GetCurrent()->Global()->Delete(String::New("stdscr"));
	    delete NCWrapper::StdScr;
	    NCWrapper::StdScr = 0;
	    NCMode::shutdown();
	    return Integer::New(OK);
	}
	else
	{
	    return Integer::New(ERR);
	}
    }


    JS_WRAPPER(nc_getch)
    {
	ASSERTARGS("nc_getch",(0==argc));
	return CastToJS( ::getch() );
    }

    JS_WRAPPER(nc_ungetch)
    {
	ASSERTARGS("nc_ungetch",(1==argc));
	return FwdToFunc1<int,int>( 0, ::ungetch, argv );
    }

    JS_WRAPPER(nc_flushinp)
    {
	ASSERTARGS("nc_flushinp",(0==argc));
	return IntToJS( ::flushinp() );
    }

    JS_WRAPPER(nc_halfdelay)
    {
	ASSERTARGS("nc_halfdelay",(1==argc));
	return FwdToFunc1<int,int>( 0, ::halfdelay, argv );
    }


    JS_WRAPPER(nc_newwin)
    {
	ASSERTARGS("nc_newwin",(4==argc));
	int a = 0;
	int lines = CastFromJS<int>( argv[a++] );
	int cols = CastFromJS<int>( argv[a++] );
	int y = CastFromJS<int>( argv[a++] );
	int x = CastFromJS<int>( argv[a++] );
	WINDOW * w = newwin( lines, cols, y, x );
	if( ! w )
	{
	    return Null();
	}
	NCWrapper * wr = new NCWrapper(w);
	return External::New(wr);
    }

    JS_WRAPPER(nc_newpad)
    {
	ASSERTARGS("nc_newpad",(2==argc));
	int a = 0;
	int lines = CastFromJS<int>( argv[a++] );
	int cols = CastFromJS<int>( argv[a++] );
	WINDOW * w = newpad( lines, cols );
	if( ! w )
	{
	    return Null();
	}
	NCWrapper * wr = new NCWrapper(w);
	return External::New(wr);
    }

    JS_WRAPPER(nc_subpad)
    {
	ASSERTARGS("nc_subpad",(5==argc));
	WR_ARG(argv[0]);
#define MYINT(N) int i ## N = JSToInt(argv[N])
	MYINT(1);
	MYINT(2);
	MYINT(3);
	MYINT(4);
#undef MYINT
	WINDOW * sub = ::subpad( wr->win, i1, i2, i3, i4 );
	if( ! sub )
	{
	    return Null();
	}
	NCWrapper * sw = new NCWrapper( sub );
	//sw->SetParent( wr );
	return External::New(sw);
    }
    
    JS_WRAPPER(nc_subwin)
    {
	ASSERTARGS("nc_subwin",(5==argc));
	int pos = 0;
	WR_ARG(argv[pos++]);
	int a1 = JSToInt( argv[pos++] );
	int a2 = JSToInt( argv[pos++] );
	int a3 = JSToInt( argv[pos++] );
	int a4 = JSToInt( argv[pos++] );
	// We can't use FwdToFunc5() here because the returned WINDOW isn't yet bound:
	WINDOW * sub = subwin( wr->win, a1, a2, a3, a4 );
	if( ! sub )
	{
	    return Null();
	}
	NCWrapper * newb = new NCWrapper( sub );
	//newb->SetParent( wr );
	return External::New(newb);
    }

    JS_WRAPPER(nc_derwin)
    {
	ASSERTARGS("nc_derwin",(5==argc));
	int pos = 0;
	WR_ARG(argv[pos++]);
	int a1 = JSToInt( argv[pos++] );
	int a2 = JSToInt( argv[pos++] );
	int a3 = JSToInt( argv[pos++] );
	int a4 = JSToInt( argv[pos++] );
	// We can't use FwdToFunc5() here because the returned WINDOW isn't yet bound:
	WINDOW * sub = derwin( wr->win, a1, a2, a3, a4 );
	if( ! sub )
	{
	    return Null();
	}
	NCWrapper * newb = new NCWrapper( sub );
	//newb->SetParent( wr );
	return External::New(newb);
    }


    JS_WRAPPER(nc_delwin)
    {
	ASSERTARGS("nc_delwin",(1==argc));
	WR_ARG(argv[0]);
	delete wr;
	return IntToJS(OK);
    }

    JS_WRAPPER(nc_wclear)
    {
	ASSERTARGS("nc_wclear",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, ::wclear, argv );
    }

    JS_WRAPPER(nc_wclrtobot)
    {
	ASSERTARGS("nc_wclrtobot",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, wclrtobot, argv);
    }


    JS_WRAPPER(nc_wclrtoeol)
    {
	ASSERTARGS("nc_wclrtoeol",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, wclrtoeol, argv);
    }

    JS_WRAPPER(nc_wdelch)
    {
	ASSERTARGS("nc_wdelch",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, wdelch, argv);
    }

    JS_WRAPPER(nc_wdeleteln)
    {
	ASSERTARGS("nc_wdeleteln",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, wdeleteln, argv);
    }

    JS_WRAPPER(nc_wechochar)
    {
	ASSERTARGS("nc_wechochar",(2==argc));
	WR_ARG(argv[0]);
	return FwdToFunc2<int,WINDOW*,chtype>( NCWrapper::bind_context, wechochar, argv );
    }

    JS_WRAPPER(nc_doupdate)
    {
	ASSERTARGS("nc_doupdate",(0==argc));
	::doupdate();
	return BoolToJS(true);
    }

    JS_WRAPPER(nc_update_panels)
    {
	ASSERTARGS("nc_update_panels",(0==argc));
	::update_panels();
	return Undefined();
    }

    JS_WRAPPER(nc_redrawwin)
    {
	ASSERTARGS("nc_redrawwin",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, ::redrawwin, argv );
    }

    JS_WRAPPER(nc_wredrawln)
    {
	ASSERTARGS("nc_wredrawln",(3==argc));
	WR_ARG(argv[0]);
	return FwdToFunc3<int,WINDOW *,int,int>( NCWrapper::bind_context, ::wredrawln, argv );
    }


    JS_WRAPPER(nc_werase)
    {
	ASSERTARGS("nc_werase",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, ::werase, argv );
    }

    JS_WRAPPER(nc_wborder)
    {
	ASSERTARGS("nc_wborder",((argc>0) && (9>=argc)));
	WR_ARG(argv[0]);
	int pos = 0;
	int a1 = (argc>1) ? JSToInt( argv[pos++] ) : 0;
	int a2 = (argc>2) ? JSToInt( argv[pos++] ) : 0;
	int a3 = (argc>3) ? JSToInt( argv[pos++] ) : 0;
	int a4 = (argc>4) ? JSToInt( argv[pos++] ) : 0;
	int a5 = (argc>5) ? JSToInt( argv[pos++] ) : 0;
	int a6 = (argc>6) ? JSToInt( argv[pos++] ) : 0;
	int a7 = (argc>7) ? JSToInt( argv[pos++] ) : 0;
	int a8 = (argc>8) ? JSToInt( argv[pos++] ) : 0;
	return IntToJS( wborder(wr->win,a1,a2,a3,a4,a5,a6,a7,a8) );
    }

    /**
       Internal implementation of wrefresh() which chooses
       a refresh strategy based on the exact WINDOW/PANEL/PAD
       type of wr. Returns OK on success.
    */
    static int do_wrefresh( NCWrapper * wr )
    {
	if( ! wr ) return ERR;
	WINDOW * w = wr->win;
	if( WIN_IS_PAD(w) )
	{
	    // todo???: prefresh()
	    //JS_ReportWarning(cx,"nc_wrefresh() was passed a PAD. This is not legal." );
	    //return IntToJS(OK);
	    return OK;
	}
	else if( wr->pnl )
	{
	    //*rval = int_to_jsval( wnoutrefresh(w) );
	    touchwin(w);
	    update_panels();
	    doupdate();
	    return OK;
	}
	else if( ERR == wrefresh(w) )
	{
	    return ERR;
	}
	return OK;
    }

    JS_WRAPPER(nc_wrefresh)
    {
	ASSERTARGS("nc_wrefresh",(argc>0));
	for( int i = 0; i < argc; ++i )
	{
	    
	    WR_ARG(argv[i]);
	    int rc = do_wrefresh(wr);
	    if( OK != rc ) return IntToJS(rc);
	}
	return IntToJS(OK);
    }

    JS_WRAPPER(nc_color_pair)
    {
	ASSERTARGS("nc_color_pair",(2==argc));
	return FwdToFunc2<uint32_t,std::string,std::string>( 0, color_pair, argv );
    }

    JS_WRAPPER(nc_color_pairnum)
    {
	ASSERTARGS("nc_color_pairnum",(2==argc));
	return FwdToFunc2<short,std::string,std::string>( 0, color_pairnum_for_names, argv );
    }


    JS_WRAPPER(nc_noecho)
    {
	return CastToJS( noecho() );
    }

    JS_WRAPPER(nc_echo)
    {
	if( 0 == argv.Length() )
	{
	    return CastToJS( echo() );
	}
	else
	{
	    return CastToJS( argv[0]->BooleanValue() ? echo() : noecho() );
	}
    }

    JS_WRAPPER(nc_nocbreak)
    {
	return CastToJS( nocbreak() );
    }

    JS_WRAPPER(nc_cbreak)
    {
	if( 0 == argv.Length() )
	{
	    return CastToJS( cbreak() );
	}
	else
	{
	    return CastToJS( argv[0]->BooleanValue() ? cbreak() : nocbreak() );
	}
    }

    JS_WRAPPER(nc_wheight)
    {
	ASSERTARGS("nc_wheight",(1==argc));
	WR_ARG(argv[0]);
	return IntToJS( wr->win->_maxy+1 );
    }
    JS_WRAPPER(nc_wwidth)
    {
	ASSERTARGS("nc_wwidth",(1==argc));
	WR_ARG(argv[0]);
	return IntToJS( wr->win->_maxx+1 );
    }

    JS_WRAPPER(nc_wmaxx)
    {
	ASSERTARGS("nc_wmaxx",(1==argc));
	WR_ARG(argv[0]);
	return IntToJS( wr->win->_maxx );
    }

    JS_WRAPPER(nc_wmaxy)
    {
	ASSERTARGS("nc_wmaxy",(1==argc));
	WR_ARG(argv[0]);
	return IntToJS( wr->win->_maxy );
    }

    JS_WRAPPER(nc_wbegy)
    {
	ASSERTARGS("nc_wbegy",(1==argc));
	WR_ARG(argv[0]);
	return IntToJS( wr->win->_begy );
    }

    JS_WRAPPER(nc_wbegx)
    {
	ASSERTARGS("nc_wbegy",(1==argc));
	WR_ARG(argv[0]);
	return IntToJS( wr->win->_begx );
    }

    JS_WRAPPER(nc_meta)
    {
	ASSERTARGS("nc_meta",(2==argc));
	return FwdToFunc2<int,WINDOW *,bool>( NCWrapper::bind_context, ::meta, argv );
    }

    JS_WRAPPER(nc_wmove)
    {
	ASSERTARGS("nc_wmove",(3==argc));
	return FwdToFunc3<int,WINDOW *,int,int>( NCWrapper::bind_context, ::wmove, argv );
    }

    JS_WRAPPER(nc_wresize)
    {
	ASSERTARGS("nc_wresize",(3==argc));
	WR_ARG(argv[0]);
	return FwdToFunc3<int,WINDOW *, int, int>( NCWrapper::bind_context, wresize, argv );
    }


    JS_WRAPPER(nc_delete)
    {
	ASSERTARGS("nc_delete",(argc>=1));
	for( int i = 0; i < argc; ++i )
	{
	    delete NCWrapper::GetNative( argv[i] );
	}
	return IntToJS(OK);
    }

    JS_WRAPPER(nc_overlay)
    {
	ASSERTARGS("nc_overlay",(2==argc));
	return FwdToFunc2<int,const WINDOW*,WINDOW *>( NCWrapper::bind_context, ::overlay, argv );
    }


    JS_WRAPPER(nc_overwrite)
    {
	ASSERTARGS("nc_overwrite",(2==argc));
	return FwdToFunc2<int,const WINDOW*,WINDOW *>( NCWrapper::bind_context, ::overwrite, argv );
    }

    JS_WRAPPER(nc_copywin)
    {
	ASSERTARGS("nc_copywin",(9==argc));
	WR_ARG(argv[0]);
	WR_ARG_N(1);
	return FwdToFunc9<int,const WINDOW*,WINDOW*,int,int,int,int,int,int,int>( NCWrapper::bind_context, copywin, argv );
    }


    JS_WRAPPER(nc_keyok)
    {
	ASSERTARGS("nc_keyok",(2==argc));
	return FwdToFunc2<int,int, bool>( 0, ::keyok, argv );
    }

    JS_WRAPPER(nc_clearok)
    {
	ASSERTARGS("nc_clearok",(2==argc));
	WR_ARG(argv[0]);
	return FwdToFunc2<int,WINDOW *,bool>( NCWrapper::bind_context, ::clearok, argv );
    }

    JS_WRAPPER(nc_box)
    {
	ASSERTARGS("nc_box",((argc>=1) && (argc<=3)));
	WR_ARG(argv[0]);
	chtype vch = (argc>1) ? JSToLong(argv[1]) : 0;
	chtype hch = (argc>2) ? JSToLong(argv[2]) : 0;
	return IntToJS( box( wr->win, vch, hch ) );
    }

    JS_WRAPPER(nc_whline)
    {
	ASSERTARGS("nc_whline",(3==argc));
	WR_ARG(argv[0]);
	return FwdToFunc3<int,WINDOW *, chtype, int>( NCWrapper::bind_context, ::whline, argv );
    }

    JS_WRAPPER(nc_wvline)
    {
	ASSERTARGS("nc_wvline",(3==argc));
	WR_ARG(argv[0]);
	return FwdToFunc3<int,WINDOW *, chtype, int>( NCWrapper::bind_context, ::wvline, argv );
    }

    JS_WRAPPER(nc_mvwhline)
    {
	ASSERTARGS("nc_mvwhline",(5==argc));
	WR_ARG(argv[0]);
	return FwdToFunc5<int,WINDOW *, int, int, chtype, int>( NCWrapper::bind_context, ::mvwhline, argv );
    }

    JS_WRAPPER(nc_mvwvline)
    {
	ASSERTARGS("nc_mvwvline",(5==argc));
	WR_ARG(argv[0]);
	return FwdToFunc5<int,WINDOW *, int, int, chtype, int>( NCWrapper::bind_context, ::mvwvline, argv );
    }

    JS_WRAPPER(nc_touchline)
    {
	ASSERTARGS("nc_touchline",(3==argc));
	WR_ARG(argv[0]);
	return FwdToFunc3<int,WINDOW *, int, int>( NCWrapper::bind_context, ::touchline, argv );
    }

    JS_WRAPPER(nc_touchwin)
    {
	ASSERTARGS("nc_touchwin",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, ::touchwin, argv );
    }

    JS_WRAPPER(nc_scrl)
    {
	ASSERTARGS("nc_scrl",(1==argc));
	return FwdToFunc1<int,int>( 0, ::scrl, argv );
    }

    JS_WRAPPER(nc_scroll)
    {
	ASSERTARGS("nc_scroll",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, ::scroll, argv );
    }

    JS_WRAPPER(nc_scrollok)
    {
	ASSERTARGS("nc_scrollok",(2==argc));
	WR_ARG(argv[0]);
	return FwdToFunc2<int,WINDOW *,bool>( NCWrapper::bind_context, ::scrollok, argv );
    }


    JS_WRAPPER(nc_wscrl)
    {
	ASSERTARGS("nc_wscrl",(2==argc));
	return FwdToFunc2<int,WINDOW *,int>( NCWrapper::bind_context, ::wscrl, argv );
    }

    JS_WRAPPER(nc_pechochar)
    {
	ASSERTARGS("nc_pechochar",(2==argc));
	WR_ARG(argv[0]);
	return FwdToFunc2<int,WINDOW *, chtype>( NCWrapper::bind_context, ::pechochar, argv );
    }

    JS_WRAPPER(nc_pnoutrefresh)
    {
	ASSERTARGS("nc_pnoutrefresh",(7==argc));
	WR_ARG(argv[0]);
	return FwdToFunc7<int,WINDOW*,int,int,int,int,int,int>( NCWrapper::bind_context, ::pnoutrefresh, argv );
    }

    JS_WRAPPER(nc_prefresh)
    {
	ASSERTARGS("nc_prefresh",(7==argc));
	WR_ARG(argv[0]);
	return FwdToFunc7<int,WINDOW *,int,int,int,int,int,int>( NCWrapper::bind_context, ::prefresh, argv );
    }

    JS_WRAPPER(nc_wstandend)
    {
	ASSERTARGS("nc_wstandend",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, ::wstandend, argv );
    }

    JS_WRAPPER(nc_wstandout)
    {
	ASSERTARGS("nc_wstandout",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, ::wstandout, argv );
    }

    JS_WRAPPER(nc_wsyncdown)
    {
	ASSERTARGS("nc_wsyncdown",(1==argc));
	WR_ARG(argv[0]);
	::wsyncdown( wr->win );
	return Undefined();
    }

    JS_WRAPPER(nc_wsyncup)
    {
	ASSERTARGS("nc_wsyncup",(1==argc));
	WR_ARG(argv[0]);
	::wsyncup( wr->win );
	return Undefined();
    }

    JS_WRAPPER(nc_wtimeout)
    {
	ASSERTARGS("nc_wtimeout",(2==argc));
	WR_ARG(argv[0]);
	::wtimeout( wr->win, JSToInt(argv[1]) );
	return Undefined();
    }

    JS_WRAPPER(nc_wtouchln)
    {
	ASSERTARGS("nc_wtouchln",(4==argc));
	WR_ARG(argv[0]);
	return FwdToFunc4<int,WINDOW *,int,int,int>( NCWrapper::bind_context, ::wtouchln, argv );
    }

    JS_WRAPPER(nc_idcok)
    {
	ASSERTARGS("nc_idcok",(2==argc));
	WR_ARG(argv[0]);
	::idcok( wr->win, JSToBool(argv[1]) );
	return Undefined();
    }

    JS_WRAPPER(nc_idlok)
    {
	ASSERTARGS("nc_idlok",(2==argc));
	WR_ARG(argv[0]);
	return FwdToFunc2<int,WINDOW *, bool>( NCWrapper::bind_context, ::idlok, argv );
    }

    JS_WRAPPER(nc_immedok)
    {
	ASSERTARGS("nc_immedok",(2==argc));
	WR_ARG(argv[0]);
	::immedok( wr->win, JSToBool(argv[1]) );
	return Undefined();
    }


    JS_WRAPPER(nc_nl)
    {
	ASSERTARGS("nc_nl",(0==argc));
	return FwdToFunc0<int>( 0, ::nl, argv );
    }
    
    JS_WRAPPER(nc_nonl)
    {
	ASSERTARGS("nc_nonl",(0==argc));
	return FwdToFunc0<int>( 0, ::nonl, argv );
    }

    JS_WRAPPER(nc_wsetscrreg)
    {
	ASSERTARGS("nc_wsetscrreg",(3==argc));
	WR_ARG(argv[0]);
	return FwdToFunc3<int,WINDOW *,int,int>( NCWrapper::bind_context, ::wsetscrreg, argv );
    }

    JS_WRAPPER(nc_beep)
    {
	ASSERTARGS("nc_beep",(0==argc));
	return IntToJS( ::beep() );
    }

    JS_WRAPPER(nc_flash)
    {
	ASSERTARGS("nc_flash",(0==argc));
	return IntToJS( ::flash() );
    }

    JS_WRAPPER(nc_has_colors)
    {
	ASSERTARGS("has_colors",(0==argc));
	return BoolToJS( has_colors() );
    }

    JS_WRAPPER(nc_can_change_color)
    {
	ASSERTARGS("can_change_color",(0==argc));
	return BoolToJS( can_change_color() );
    }

    JS_WRAPPER(nc_raw)
    {
	ASSERTARGS("nc_raw",(0==argc));
	return IntToJS( ::raw() );
    }

    JS_WRAPPER(nc_noraw)
    {
	ASSERTARGS("nc_noraw",(0==argc));
	return IntToJS( ::noraw() );
    }

    JS_WRAPPER(nc_getcury)
    {
	ASSERTARGS("nc_getcury",(1==argc));
	WR_ARG(argv[0]);
	return IntToJS( getcury(wr->win) );
    }

    JS_WRAPPER(nc_getcurx)
    {
	ASSERTARGS("nc_getcurx",(1==argc));
	WR_ARG(argv[0]);
	return IntToJS( getcurx(wr->win) );
    }

    JS_WRAPPER(nc_getcuryx)
    {
	ASSERTARGS("nc_getcuryx",(1==argc));
	WR_ARG(argv[0]);
	Handle<Object> obj = Object::New();
#define OSET(K,V) obj->Set(String::New(# K), IntToJS(V) )
	OSET( "y", getcury( wr->win ) );
	OSET( "x", getcurx( wr->win ) );
#undef OSET
	return obj;
    }

    JS_WRAPPER(nc_screen_width)
    {
	return IntToJS( screen_width() );
    }

    JS_WRAPPER(nc_screen_height)
    {
	return IntToJS( screen_height() );
    }

    JS_WRAPPER(nc_winch)
    {
	ASSERTARGS("nc_winch",(1==argc));
	WR_ARG(argv[0]);
	return CastToJS( ::winch( wr->win ) );
    }

    JS_WRAPPER(nc_mvwinch)
    {
	ASSERTARGS("nc_mvwinch",(3==argc));
	WR_ARG(argv[0]);
	return FwdToFunc3<chtype,WINDOW *,int,int>( NCWrapper::bind_context, ::mvwinch, argv );
    }

    JS_WRAPPER(nc_keypad)
    {
	ASSERTARGS("nc_mvwinch",(2==argc));
	WR_ARG(argv[0]);
	return FwdToFunc2<int,WINDOW *,bool>( NCWrapper::bind_context, ::keypad, argv );
    }

    JS_WRAPPER(nc_mvwchgat)
    {
	ASSERTARGS("nc_mvwchgat",(6==argc));
	WR_ARG(argv[0]);
	int y = JSToInt( argv[1] );
	int x = JSToInt( argv[2] );
	int n = JSToInt( argv[3] );
	int attr = JSToInt( argv[4] );
	short color = JSToShort( argv[5] );
	return IntToJS( mvwchgat(wr->win,y,x,n,attr,color,0) );
    }

    JS_WRAPPER(nc_wchgat)
    {
	ASSERTARGS("nc_wchgat",(4==argc));
	WR_ARG(argv[0]);
	int n = JSToInt( argv[1] );
	int attr = JSToInt( argv[2] );
	short color = JSToShort( argv[3] );
	return IntToJS( wchgat(wr->win,n,attr,color,0) );
    }

    JS_WRAPPER(nc_napms)
    {
	ASSERTARGS("nc_napms",(1==argc));
	return FwdToFunc1<int,int>( 0, napms, argv);
    }

    JS_WRAPPER(nc_curs_set)
    {
	ASSERTARGS("nc_curs_set",(1==argc));
	return FwdToFunc1<int,int>( 0, curs_set, argv);
    }

    JS_WRAPPER(nc_wenclose)
    {
	ASSERTARGS("nc_wenclose",(3==argc));
	WR_ARG(argv[0]);
	return FwdToFunc3<bool,WINDOW const *,int,int>( NCWrapper::bind_context, wenclose, argv );
    }

    JS_WRAPPER(nc_mouseinterval)
    {
	ASSERTARGS("nc_mouseinterval",(1==argc));
#if ! NCURSES_MOUSE_VERSION
	return Undefined();
#else
	return FwdToFunc1<int,int>( 0, mouseinterval, argv );
#endif
    }

    JS_WRAPPER(nc_mousemask)
    {
	ASSERTARGS("nc_mousemask",((0==argc) || (1==argc)));
#if ! NCURSES_MOUSE_VERSION
	return Undefined();
#else
	mmask_t mret;
	if( 0 == argc )
	{
	    // A weird workaround for the lack of a real getter:
	    mousemask( ALL_MOUSE_EVENTS, &mret );
	    mousemask( mret, NULL );
	}
	else
	{
	    mmask_t arg = JSToLong( argv[0] );
	    mret = mousemask( arg, NULL );
	}
	return CastToJS( mret );
#endif
    }

    JS_WRAPPER(nc_ungetmouse)
    {
#if ! NCURSES_MOUSE_VERSION
	return Null();
#else
	return ThrowException(String::New("nc_ungetmouse() not yet implemented."));
#endif // NCURSES_MOUSE_VERSION
    }

    JS_WRAPPER(nc_getmouse)
    {
#if ! NCURSES_MOUSE_VERSION
	return Null();
#else
	ASSERTARGS("nc_getmouse",(0==argc));
	MEVENT evt;
	getmouse(&evt);
	Handle<Object> obj = Object::New();
#define OSET(K,V) obj->Set(String::New(# K), V)
	OSET( "y", IntToJS(evt.y) );
	OSET( "x", IntToJS(evt.x) );
	OSET( "z", IntToJS(evt.z) );
	OSET( "id", IntToJS(evt.id) );
	OSET( "bstate", IntToJS(evt.bstate) );
	OSET( "x", IntToJS(evt.x) );
	OSET( "z", IntToJS(evt.z) );
	OSET( "id", IntToJS(evt.id) );
	OSET( "bstate", IntToJS(evt.bstate) );
#undef OSET
	return obj;
#endif // NCURSES_MOUSE_VERSION
    }

    /**
       Writes n bytes of str to w at position y/x.
    */
    static int nc_addvstr_impl( WINDOW * w, int y, int x, 
				std::string const & str, int n )
    {
	
	if( (! w) || str.empty() ) return ERR;
	if( (n < 0) || (n < (int)str.size()) ) n = static_cast<int>( str.size() );
	size_t pos = 0;
	for( int c = y; c < y+n; ++c )
	{
	    if( ERR == wmove(w,c,x) )
	    {
		return ERR;
	    }
	    if( ERR == waddch( w, str[pos++] ) )
	    {
		return ERR;
	    }
	}
	return OK;
    }

    JS_WRAPPER(nc_waddvstr)
    {
	ASSERTARGS("nc_waddvstr",(2==argc));
	WR_ARG(argv[0]);
	std::string s( JSToStdString(argv[1]) );
	WINDOW * w = wr->win;
	return IntToJS( nc_addvstr_impl( w, getcury(w), getcurx(w), s, -1 ) );
    }
    JS_WRAPPER(nc_waddvnstr)
    {
	ASSERTARGS("nc_waddvnstr",(3==argc));
	WR_ARG(argv[0]);
	std::string s = JSToStdString( argv[1] );
	int n = JSToInt( argv[2] );
	WINDOW * w = wr->win;
	return IntToJS( nc_addvstr_impl( w, getcury(w), getcurx(w), s, n ) );
    }

    JS_WRAPPER(nc_mvwaddvstr)
    {
	ASSERTARGS("nc_mvwaddvstr",(4==argc));
	WR_ARG(argv[0]);
	WINDOW * w = wr->win;
	int y = JSToInt(argv[1]);
	int x = JSToInt(argv[2]);
	std::string s( JSToStdString(argv[3]) );
	return IntToJS( nc_addvstr_impl( w, y, x, s, -1 ) );
    }

    JS_WRAPPER(nc_mvwaddvnstr)
    {
	ASSERTARGS("nc_mvwaddvnstr",(5==argc));
	WR_ARG(argv[0]);
	WINDOW * w = wr->win;
	int y = JSToInt(argv[1]);
	int x = JSToInt(argv[2]);
	std::string str( JSToStdString(argv[3]) );
	int n = JSToInt(argv[4]);
	int slen = static_cast<int>( str.size() );
	if( (n < 0) || (n>slen) ) n = slen;
	return IntToJS( nc_addvstr_impl( w, y, x, str.c_str(), n ) );
    }


    JS_WRAPPER(nc_waddstr)
    {
	ASSERTARGS("nc_waddstr",(2==argc));
	WR_ARG(argv[0]);
	std::string str = JSToStdString(argv[1]);
	return IntToJS( waddstr( wr->win, str.c_str() ) );
    }

    JS_WRAPPER(nc_waddnstr)
    {
	ASSERTARGS("nc_waddnstr",(3==argc));
	WR_ARG(argv[0]);
	std::string str = JSToStdString(argv[1]);
	int n = JSToInt(argv[3]);
	int slen = static_cast<int>( str.size() );
	if( (n < 0) || (n>slen) ) n = slen;
	return IntToJS( waddnstr( wr->win, str.c_str(), n ) );
    }


    JS_WRAPPER(nc_wgetch)
    {
	ASSERTARGS("nc_wgetch",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<int,WINDOW *>( NCWrapper::bind_context, wgetch, argv );
    }

    JS_WRAPPER(nc_wbkgd)
    {
	ASSERTARGS("nc_wbkgd",(2==argc));
	WR_ARG(argv[0]);
	return FwdToFunc2<int,WINDOW *,chtype>( NCWrapper::bind_context, wbkgd, argv );
	//chtype c = CastFromJS<chtype>( argv[1] );
	//return IntToJS( wbkgd( w->win, c ) );
    }

    /**
       Works similarly to print(), but assumes that its first
       argument is a WINDOW. The remaining args are
       space-separated and added to the WINDOW using addstr() and
       a trailing '\n' is then added. Unlike addstr(), this
       function DOES call refresh() when it is done (this
       simplifies its usage). If you are operating on PADs then
       nc_addstr() is recommended, because refresh() is not legal
       for PADs.

       JS usage: nc_wprint(myWin,...);
    */
    JS_WRAPPER(nc_wprint)
    {
	ASSERTARGS("nc_printw",(1<argc));
	WR_ARG(argv[0]);
	WINDOW * w = wr->win;
	int rc = OK;
	for( int i = 1; i < argc; ++i )
	{
	    std::string s( JSToStdString(argv[i]) );
	    rc = ::waddstr( w, s.c_str() );
	    if( OK != rc ) return IntToJS(rc);
	    if( i < (argc-1) )
	    {
		rc = waddch(w,' ');
		if( OK != rc ) return IntToJS(rc);
	    }
	}
	rc = waddch(w,'\n');
	// Use do_wrefresh() instead of wrefresh() in case wr is
	// associated with a panel, because do_wrefresh() handles that
	// case:
	return IntToJS( do_wrefresh(wr) );
    }

    JS_WRAPPER(nc_waddch)
    {
	ASSERTARGS("nc_waddch",(2==argc));
	WR_ARG(argv[0]);
	return FwdToFunc2<int,WINDOW*,chtype>( NCWrapper::bind_context, waddch, argv );
    }

    JS_WRAPPER(nc_mvwaddch)
    {
	ASSERTARGS("nc_mvwaddch",(4==argc));
	WR_ARG(argv[0]);
	return FwdToFunc4<int,WINDOW*,int,int,chtype>( NCWrapper::bind_context, mvwaddch, argv );
    }

    JS_WRAPPER(nc_wbkgdset)
    {
	ASSERTARGS("nc_wbkgdset",(2==argc));
	WR_ARG(argv[0]);
	wbkgdset( wr->win, CastFromJS<chtype>( argv[1] ) );
	return Undefined();
    }

    JS_WRAPPER(nc_getbkgd)
    {
	ASSERTARGS("nc_getbkgd",(1==argc));
	WR_ARG(argv[0]);
	return FwdToFunc1<chtype,WINDOW *>( NCWrapper::bind_context, getbkgd, argv );
    }


    JS_WRAPPER(nc_wgetstr)
    {
	ASSERTARGS("nc_wgetstr",(1==argc));
	WR_ARG(argv[0]);
	int wid = wr->win->_maxx+1;
	int ht = wr->win->_maxy+1;
	int sz = wid * ht;
	std::vector<char> buf(sz+1,'\0');
	if( ERR == wgetstr(wr->win,&buf[0]) )
	{
	    return Null();
	}
	buf[buf.size()-1] = '\0';
	return String::New( &buf[0], static_cast<int>( buf.size() ) );
    }

    JS_WRAPPER(nc_mvwgetstr)
    {
	ASSERTARGS("nc_mvwgetstr",(3==argc));
	WR_ARG(argv[0]);
	int y = JSToInt( argv[1] );
	int x = JSToInt( argv[2] );
	int wid = wr->win->_maxx+1;
	int ht = wr->win->_maxy+1;
	int sz = wid * ht;
	std::vector<char> buf(sz+1,'\0');
	if( ERR == mvwgetnstr(wr->win,y,x,&buf[0],buf.size()) )
	{
	    return Null();
	}
	buf[buf.size()-1] = '\0';
	return String::New( &buf[0], static_cast<int>( buf.size() ) );
    }

    JS_WRAPPER(nc_mvwgetnstr)
    {
	ASSERTARGS("nc_mvwgetnstr",(4==argc));
	WR_ARG(argv[0]);
	int y = JSToInt( argv[1] );
	int x = JSToInt( argv[2] );
	int n = JSToInt( argv[3] );
	int wid = wr->win->_maxx+1;
	int ht = wr->win->_maxy+1;
	int sz = wid * ht;
	if( n > sz ) n = sz; // avoid allocating a huge amount when NaN is passed
	std::vector<char> buf(n+1,'\0');
	if( ERR == mvwgetnstr(wr->win,y,x,&buf[0],n) )
	{
	    return Null();
	}
	buf[buf.size()-1] = '\0';
	return String::New( &buf[0], static_cast<int>( buf.size() ) );
    }



    JS_WRAPPER(nc_is_window)
    {
	ASSERTARGS("nc_is_window",(argc<=2));
	bool strict = (argc>1) ? argv[1]->BooleanValue() : true;
	NCWrapper * wr = NCWrapper::GetNative( argv[0] );
	if( ! wr )
	{
	    return BoolToJS(false);
	}
	WINDOW * w = wr->win;
	if( ! w )
	{
	    return ThrowException(String::New("nc_is_window(): internal error: NCWrapper has a NULL WINDOW." ));
	}
	Handle<Value> rval = BoolToJS(true);
	if( strict )
	{ 
	    if( WIN_IS_PAD(w) || wr->pnl )
	    {
		rval = BoolToJS(false);
	    }
	}
	return rval;
    }

    JS_WRAPPER(nc_is_panel)
    {
	ASSERTARGS("nc_is_panel",(1==argc));
	NCWrapper * wr = NCWrapper::GetNative(argv[0]);
	if( ! wr ) return BoolToJS(false);
	return BoolToJS( wr->pnl ? true : false);
    }

    JS_WRAPPER(nc_is_pad)
    {
	ASSERTARGS("nc_is_pad",(1==argc));
	NCWrapper * wr = NCWrapper::GetNative(argv[0]);
	return BoolToJS( wr ? WIN_IS_PAD(wr->win) : false );
    }

    JS_WRAPPER(nc_mvwaddnstr)
    {
	ASSERTARGS("nc_mvwaddnstr",(5==argc));
	WR_ARG(argv[0]);
	int i1 = JSToInt(argv[1]);
	int i2 = JSToInt(argv[2]);
	std::string str( JSToStdString( argv[3] ) );
	int i3 = JSToInt(argv[4]);
	return IntToJS( mvwaddnstr( wr->win, i1, i2, str.c_str(), i3 ) );
    }

    JS_WRAPPER(nc_mvwaddstr)
    {
	ASSERTARGS("nc_mvwaddstr",(4==argc));
	WR_ARG(argv[0]);
	int i1 = JSToInt(argv[1]);
	int i2 = JSToInt(argv[2]);
	std::string str( JSToStdString( argv[3] ) );
	return IntToJS( mvwaddstr( wr->win, i1, i2, str.c_str() ) );
    }

    JS_WRAPPER(nc_wattroff)
    {
	ASSERTARGS("nc_wattroff",(2==argc));
	WR_ARG(argv[0]);
	return FwdToFunc2<int,WINDOW *, int>( NCWrapper::bind_context, wattroff, argv );
    }
    JS_WRAPPER(nc_wattron)
    {
	ASSERTARGS("nc_wattron",(2==argc));
	WR_ARG(argv[0]);
	return FwdToFunc2<int,WINDOW *, int>( NCWrapper::bind_context, wattron, argv );
    }
    JS_WRAPPER(nc_wattrset)
    {
	ASSERTARGS("nc_wattrset",(2==argc));
	WR_ARG(argv[0]);
	return FwdToFunc2<int,WINDOW *, int>( NCWrapper::bind_context, wattrset, argv );
    }

    JS_WRAPPER(nc_getpary)
    {
	ASSERTARGS("nc_getpary",(1==argc));
	WR_ARG_OR(argv[0],Null());
	int y = -1;
	int x = -1;
	getparyx( wr->win, y, x );
	return IntToJS( y );
    }


    JS_WRAPPER(nc_getparx)
    {
	ASSERTARGS("nc_getparx",(1==argc));
	WR_ARG_OR(argv[0],Null());
	int y = -1;
	int x = -1;
	getparyx( wr->win, y, x );
	return IntToJS( x );
    }

    JS_WRAPPER(nc_getparyx)
    {
	ASSERTARGS("nc_getparyx",(1==argc));
	WR_ARG_OR(argv[0],Null());
	int y = -1;
	int x = -1;
	getparyx( wr->win, y, x );
	Handle<Object> obj = Object::New();
#define OSET(K,V) obj->Set(String::New(# K), V)
	OSET( "y", IntToJS(y) );
	OSET( "x", IntToJS(x) );
	return obj;
#undef OSET
    }

    JS_WRAPPER(nc_wgeometry)
    {
	ASSERTARGS("nc_wgeometry",(1==argc));
	WR_ARG(argv[0]);
	int y, x, w, h;
	getbegyx( wr->win, y, x );
	getmaxyx( wr->win, h, w );
	Handle<Object> obj = Object::New();
#define OSET(K,V) obj->Set(String::New(# K), IntToJS(V) )
	OSET( "y", y );
	OSET( "x", x );
	OSET( "w", w );
	OSET( "h", h );
	return obj;
#undef OSET
    }

    JS_WRAPPER(nc_getmaxyx)
    {
	ASSERTARGS("nc_getmaxyx",(1==argc));
	WR_ARG(argv[0]);
	int y = - 1;
	int x = -1;
	getmaxyx( wr->win, x, y );
	Handle<Object> obj = Object::New();
#define OSET(K,V) obj->Set(String::New(# K), IntToJS(V) )
	OSET( "y", y );
	OSET( "x", x );
	return obj;
#undef OSET
    }


    JS_WRAPPER(nc_new_panel)
    {
	ASSERTARGS("nc_new_panel",(1==argc));
	WR_ARG(argv[0]);
	if( wr->pnl ) return ThrowException(String::New("Window handle already has a panel associated with it!"));
	PANEL * p = new_panel( wr->win );
	if( p ) wr->SetPanel(p);
	return External::New(wr);
    }

    JS_WRAPPER(nc_del_panel)
    {
	ASSERTARGS("nc_del_panel",(1==argc));
	PNL_ARG(argv[0]);
	PANEL * p = wr->pnl;
	wr->SetPanel(0);
	return IntToJS( del_panel( p ) );
    }

    JS_WRAPPER(nc_hide_panel)
    {
	ASSERTARGS("nc_hide_panel",(1==argc));
	PNL_ARG(argv[0]);
	return IntToJS( hide_panel( wr->pnl ) );
    }

    JS_WRAPPER(nc_panel_hidden)
    {
	ASSERTARGS("nc_panel_hidden",(1==argc));
	PNL_ARG(argv[0]);
	return BoolToJS( (0==panel_hidden( wr->pnl )) ? false : true );
    }

    JS_WRAPPER(nc_bottom_panel)
    {
	ASSERTARGS("nc_bottom_panel",(1==argc));
	PNL_ARG(argv[0]);
	return IntToJS( bottom_panel( wr->pnl ) );
    }

    JS_WRAPPER(nc_top_panel)
    {
	ASSERTARGS("nc_top_panel",(1==argc));
	PNL_ARG(argv[0]);
	return IntToJS( top_panel( wr->pnl ) );
    }

    JS_WRAPPER(nc_show_panel)
    {
	ASSERTARGS("nc_show_panel",(1==argc));
	PNL_ARG(argv[0]);
	return IntToJS( show_panel( wr->pnl ) );
    }

    JS_WRAPPER(nc_panel_window)
    {
	ASSERTARGS("nc_panel_hidden",(1==argc));
	PNL_ARG(argv[0]);
	if( wr->win ) return wr->jsval;
	else return Null();
    }

    JS_WRAPPER(nc_move_panel)
    {
	ASSERTARGS("nc_move_panel",(3==argc));
	PNL_ARG(argv[0]);
	return FwdToFunc3<int,PANEL*,int,int>( NCWrapper::bind_context, ::move_panel, argv );
    }

    JS_WRAPPER(nc_panel_above)
    {
	ASSERTARGS("nc_panel_above",(1==argc));
	PNL_ARG(argv[0]);
	return FwdToFunc1<PANEL*,PANEL const *>( NCWrapper::bind_context, ::panel_above, argv );
    }

    JS_WRAPPER(nc_panel_below)
    {
	ASSERTARGS("nc_panel_below",(1==argc));
	PNL_ARG(argv[0]);
	return FwdToFunc1<PANEL*,PANEL const *>( NCWrapper::bind_context, ::panel_below, argv );
    }


    JS_WRAPPER(nc_KEY_F)
    {
	ASSERTARGS("KEY_F",(1==argc));
	return IntToJS( KEY_F( JSToInt( argv[0] ) ) );
    }

    JS_WRAPPER(nc_has_key)
    {
	ASSERTARGS("nc_has_key",(1==argc));
#if defined(NCURSES_VERSION)
	return BoolToJS( (0 == has_key( JSToInt( argv[0] ) )) ? false : true );
#else
	return ThrowException(String::New("nc_has_key() requires Ncurses!"));
#endif
    }


    static Handle<Value>
    nc_capture_stream( std::ostream & os, Arguments const & argv )
    {
	ASSERTARGS("nc_capture_cout/cerr",((argc>0) && (argc<3)));
	WR_ARG(argv[0]);
	long attr = (argc>1) ? JSToInt(argv[1]) : 0L;
	captured_streams().insert( ::std::make_pair( wr->win, new nc_window_streambuf(wr->win,os,attr) ) );
	return BoolToJS(true);
    }

    JS_WRAPPER(nc_capture_cout)
    {
	return nc_capture_stream( std::cout, argv );
    }

    JS_WRAPPER(nc_capture_cerr)
    {
	return nc_capture_stream( std::cerr, argv );
    }

    JS_WRAPPER(nc_capture_end)
    {
	ASSERTARGS("nc_capture_end",(1==argc));
	WR_ARG(argv[0]);
	WindowStreamMap & str = captured_streams();
	typedef WindowStreamMap::iterator WIT;
	WIT stit = str.lower_bound(wr->win);
	WIT head = stit;
	WIT stet = str.upper_bound(wr->win);
	int undid = 0;
	if( stit != stet )
	{
	    for( ; stet != stit; ++stit )
	    {
		++undid;
		delete (*stit).second;
	    }
	    str.erase( head, stet );
	}
	return IntToJS( undid );
    }

    JS_WRAPPER(nc_ripoffline)
    {
	ASSERTARGS("nc_ripoffline",(argc==2));
	int l = JSToInt(argv[0]);
	if( 0 == l )
	{
	    return ThrowException(String::New("nc_ripoffline(): first argument must be a non-zero integer."));
	}
	Handle<Function> f( Function::Cast(*argv[1]) );
	if( f.IsEmpty() || !f->IsFunction() )
	{
	    return ThrowException(String::New("nc_ripoffline() second argument must be a Function." ));
	}
	if( ! ripoff::add_handler( f ) )
	{
	    return ThrowException(String::New("Could not add new handler! Maybe you called this more than 5 times?" ));
	}
	int rc = ::ripoffline( l, ripoff::handler );
	if( 0 != rc )
	{
	    std::ostringstream os;
	    os << "nc_ripoffline(): ::ripoffline("<<l<<",Function) failed with code "<<rc<<"!";
	    return ThrowException(String::New( os.str().c_str(), static_cast<int>(os.str().size()) ));
	}
	return IntToJS(OK);
    }



#if 0 // boilerplate code for copy/replace:
    JS_WRAPPER(nc_XXX)
    {
	//ASSERTARGS(sqlite3_XXX,(2==argc));
	//WR_ARG(argv[0]);
	//return CastToJS( ::XXX( wr->win ) );
	return ThrowException(String::New("Native function not implemented!"));
    }
#endif



#undef ASSERTARGS
#undef WR_ARG
#undef JS_WRAPPER

    Handle<Value> SetupAddon( Handle<Object> gl, bool polute )
    {
	HandleScope sentry;
	Handle<Object> nco = Object::New();
	gl->Set(String::New("ncurses"),nco);

	Handle<Function> func;
#define ADDFUNC2(JS,NATIVE)						\
	func = FunctionTemplate::New(nc_ ## NATIVE)->GetFunction();	\
	if( polute ) gl->Set(String::New("nc_" # JS), func );		\
	nco->Set(String::New(# JS), func )

#define ADDFUNC(SUF) ADDFUNC2(SUF,SUF)

	ADDFUNC(initscr);
	ADDFUNC(endwin);
	ADDFUNC(getch);
	ADDFUNC(newwin);
	ADDFUNC(wborder);
	ADDFUNC(wrefresh);
	ADDFUNC(color_pair);
	ADDFUNC2(color,color_pair);
	ADDFUNC(color_pairnum);
	ADDFUNC(echo);
	ADDFUNC(noecho);
	ADDFUNC(cbreak);
	ADDFUNC(nocbreak);
	ADDFUNC(delwin);
	ADDFUNC(wclear);
	ADDFUNC(werase);
	ADDFUNC(wwidth);
	ADDFUNC(wheight);
	ADDFUNC(wmaxy);
	ADDFUNC(wmaxx);
	ADDFUNC(wbegy);
	ADDFUNC(wbegx);
	ADDFUNC(meta);
	ADDFUNC(wmove);
	ADDFUNC(newpad);
	ADDFUNC(doupdate);
	ADDFUNC(redrawwin);
	ADDFUNC(wredrawln);
	ADDFUNC(delete);
	ADDFUNC(overlay);
	ADDFUNC(overwrite);
	ADDFUNC(copywin);
	ADDFUNC(keyok);
	ADDFUNC(clearok);
	ADDFUNC(box);
	ADDFUNC(whline);
	ADDFUNC(wvline);
	ADDFUNC(ungetch);
	ADDFUNC(flushinp);
	ADDFUNC(halfdelay);
	ADDFUNC(mvwhline);
	ADDFUNC(mvwvline);
	ADDFUNC(touchline);
	ADDFUNC(touchwin);
	ADDFUNC(scrl);
	ADDFUNC(scroll);
	ADDFUNC(scrollok);
	ADDFUNC(wscrl);
	ADDFUNC(pechochar);
	ADDFUNC(pnoutrefresh);
	ADDFUNC(prefresh);
	ADDFUNC(wstandend);
	ADDFUNC(wstandout);
	ADDFUNC(wsyncdown);
	ADDFUNC(wsyncup);
	ADDFUNC(wtimeout);
	ADDFUNC(wtouchln);
	ADDFUNC(idcok);
	ADDFUNC(idlok);
	ADDFUNC(immedok);
	ADDFUNC(nl);
	ADDFUNC(nonl);
	ADDFUNC(wsetscrreg);
	ADDFUNC(beep);
	ADDFUNC(flash);
	ADDFUNC(has_colors);
	ADDFUNC(can_change_color);
	ADDFUNC(raw);
	ADDFUNC(noraw);
	ADDFUNC(getcury);
	ADDFUNC(getcurx);
	ADDFUNC(getcuryx);
	ADDFUNC(getmaxyx);
	ADDFUNC(screen_width);
	ADDFUNC(screen_height);
	ADDFUNC(winch);
	ADDFUNC(mvwinch);
	ADDFUNC(keypad);
	ADDFUNC(mvwchgat);
	ADDFUNC(wchgat);
	ADDFUNC(napms);
	ADDFUNC(curs_set);
	ADDFUNC(wenclose);
	ADDFUNC(mouseinterval);
	ADDFUNC(mousemask);
	ADDFUNC(ungetmouse);
	ADDFUNC(getmouse);
	ADDFUNC(subpad);
	ADDFUNC(subwin);
	ADDFUNC(derwin);
	ADDFUNC(waddvstr);
	ADDFUNC(waddvnstr);
	ADDFUNC(mvwaddvstr);
	ADDFUNC(mvwaddvnstr);
	ADDFUNC(waddstr);
	ADDFUNC(waddnstr);
	ADDFUNC(wgetch);
	ADDFUNC(wbkgd);
	//ADDFUNC(printw);
	ADDFUNC(wprint);
	ADDFUNC(wclrtobot);
	ADDFUNC(wclrtoeol);
	ADDFUNC(wdelch);
	ADDFUNC(wdeleteln);
	ADDFUNC(wechochar);
	ADDFUNC(waddch);
	ADDFUNC(mvwaddch);
	ADDFUNC(wbkgdset);
	ADDFUNC(getbkgd);
	ADDFUNC(wresize);
	ADDFUNC(wgetstr);
	ADDFUNC(mvwgetstr);
	ADDFUNC(mvwgetnstr);
	ADDFUNC(is_window);
	ADDFUNC(is_pad);
	ADDFUNC(is_panel);
	ADDFUNC(mvwaddnstr);
	ADDFUNC(mvwaddstr);
	ADDFUNC(wattroff);
	ADDFUNC(wattron);
	ADDFUNC(wattrset);
	ADDFUNC(getpary);
	ADDFUNC(getparx);
	ADDFUNC(getparyx);
	ADDFUNC(wgeometry);
	ADDFUNC(update_panels);
	ADDFUNC(new_panel);
	ADDFUNC(del_panel);
	ADDFUNC(hide_panel);
	ADDFUNC(panel_hidden);
	ADDFUNC(has_key);
	ADDFUNC(bottom_panel);
	ADDFUNC(top_panel);
	ADDFUNC(show_panel);
	ADDFUNC(panel_window);
	ADDFUNC(move_panel);
	ADDFUNC(panel_above);
	ADDFUNC(panel_below);
	ADDFUNC(capture_cout);
	ADDFUNC(capture_cerr);
	ADDFUNC(capture_end);

	ADDFUNC(ripoffline);

#undef ADDFUNC

	Handle<Object> i2a = Object::New();
	Handle<Object> a2i = Object::New();
	nco->Set(String::New("i2a"),i2a); // int-to-ascii mapping
	nco->Set(String::New("a2i"),a2i); // ascii-to-int mapping

#define ADDFUNC(F) nco->Set(String::New(# F), FunctionTemplate::New(nc_ ## F)->GetFunction() )
	ADDFUNC(KEY_F);
#undef ADDFUNC
	for( int i = 0; i < 128; ++i )
	{
	    char const ch[2] = { static_cast<char>(i), 0 };
	    Handle<Integer> ji = Integer::New(i);
	    Handle<String> js = String::New( ch, 1 );
	    i2a->Set(ji, js);
	    a2i->Set(js, ji);
	}

#define SET_MAC(MAC) nco->Set(String::New(# MAC), Integer::New(MAC), ::v8::ReadOnly)
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
	nco->Set(String::New("CTRL_KEY"), Integer::New(0x1f), ::v8::ReadOnly);

#if NCURSES_MOUSE_VERSION
#define SET_BTN(B) nco->Set(String::New(# B), Integer::New(B), ::v8::ReadOnly)
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
	return nco;
    }

#undef JS_WRAPPER

    static Handle<Value> SetupAddonStatic( Handle<Object> gl )
    {
	return SetupAddon( gl, true );
    }
    V8_JUICE_PLUGIN_STATIC_INIT(SetupAddonStatic);

}}} /* namespaces */
