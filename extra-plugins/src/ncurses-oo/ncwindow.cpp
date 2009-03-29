// * this is for making emacs happy: -*-Mode: C++;-*-

/*
  The license below appears to be incorrect (it says this was part
  of the GNU C++ library???), and contradicts the one in the 
  header. (stephan@s11n.net)

  Copyright (C) 1989 Free Software Foundation
  written by Eric Newton (newton@rocky.oswego.edu)

  --------------------------- begin dubious license
  This file is part of the GNU C++ Library.  This library is free
  software; you can redistribute it and/or modify it under the terms of
  the GNU Library General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your
  option) any later version.  This library is distributed in the hope
  that it will be useful, but WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the GNU Library General Public License for more details.
  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free Software
  Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
  --------------------------- end dubious license

  modified by Ulrich Drepper  (drepper@karlsruhe.gmd.de)
  and Anatoly Ivasyuk (anatoly@nick.csh.rit.edu)

  modified by Juergen Pfeifer
  and Thomas Dickey (noting that more than 70% of this file has been changed)

*/

/****************************************************************************

The license pasted in from the header file reads:

* Copyright (c) 1998-2001,2004 Free Software Foundation, Inc.              *
*                                                                          *
* Permission is hereby granted, free of charge, to any person obtaining a  *
* copy of this software and associated documentation files (the            *
* "Software"), to deal in the Software without restriction, including      *
* without limitation the rights to use, copy, modify, merge, publish,      *
* distribute, distribute with modifications, sublicense, and/or sell       *
* copies of the Software, and to permit persons to whom the Software is    *
* furnished to do so, subject to the following conditions:                 *
*                                                                          *
* The above copyright notice and this permission notice shall be included  *
* in all copies or substantial portions of the Software.                   *
*                                                                          *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
* IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
*                                                                          *
* Except as contained in this notice, the name(s) of the above copyright   *
* holders shall not be used in advertising or otherwise to promote the     *
* sale, use or other dealings in this Software without prior written       *
* authorization.                                                           *
****************************************************************************/


/************************************************************************
This copy was forked from the ncurses 5.4 source tree and mutilated
by stephan@s11n.net, starting 1 March 2005. See the header file for
the list of notable/significant changes.

************************************************************************/

#include <string>
#include <sstream>
#include <ncurses.h>
#include "ncwindow.hpp"


#include <ncurses.h>
#include "ncwindow.hpp"

/*
 * The ncurses library has a fallback for vsscanf(), which may work...
 */
#if !(USE_STRSTREAM_VSCAN || USE_STRSTREAM_VSCAN_CAST)
#  undef  USE_STDIO_VSCAN
#  define USE_STDIO_VSCAN 1
#endif

#if defined(__GNUG__)
#  ifndef _IO_va_list
#    define _IO_va_list char *
#  endif
#endif

namespace ncutil {

    typedef std::map<void const *,NCWindow> LifetimeCheck;
    NCWindow::LifetimeCheck & NCWindow::lifecheck()
    {
        static NCWindow::LifetimeCheck bob;
        return bob;
    }
    bool NCWindow::is_alive( NCWindow const * b )
    {
        NCWindow::LifetimeCheck & m( lifecheck() );
        return m.end() != m.find(b);
    }

    bool NCWindow::consumeKey( int key, bool meta )
    {
        // REMINDER: NCPad currently has no way of telling an
        // NCWindow that it belongs to that window, so we
        // cannot forward events to them here. Looking for a
        // good fix.
        for(NCWindow* p = this->subwins; p != 0; p = p->sib)
        {
            if( this->subwins->consumeKey( key, meta ) ) return true;
        }
        return false;
    }

    int
    NCWindow::scanw(const char* fmt, ...)
    {
        int result = ERR;
        char buf[BUFSIZ];

        if (::wgetnstr(m_cwin, buf, sizeof(buf)) != ERR) {
            va_list args;
            va_start(args, fmt);
#if USE_STDIO_VSCAN
            if (::vsscanf(buf, fmt, args) != -1)
                result = OK;
#elif USE_STRSTREAM_VSCAN	/* powerpc, os390 */
            strstreambuf ss(buf, sizeof(buf));
            if (ss.vscan(fmt, args) != -1)
                result = OK;
#elif USE_STRSTREAM_VSCAN_CAST	/* pre-gcc 3.0 */
            strstreambuf ss(buf, sizeof(buf));
            if (ss.vscan(fmt, (_IO_va_list)args) != -1)
                result = OK;
#endif
            va_end(args);
        }
        return result;
    }


    int
    NCWindow::scanw(int y, int x, const char* fmt, ...)
    {
        int result = ERR;
        char buf[BUFSIZ];

        if (::wmove(m_cwin, y, x) != ERR) {
            if (::wgetnstr(m_cwin, buf, sizeof(buf)) != ERR) {
                va_list args;
                va_start(args, fmt);
#if USE_STDIO_VSCAN
                if (::vsscanf(buf, fmt, args) != -1) result = OK;
#elif USE_STRSTREAM_VSCAN	/* powerpc, os390 */
                strstreambuf ss(buf, sizeof(buf));
                if (ss.vscan(fmt, args) != -1) result = OK;
#elif USE_STRSTREAM_VSCAN_CAST	/* pre-gcc 3.0 */
                strstreambuf ss(buf, sizeof(buf));
                if (ss.vscan(fmt, (_IO_va_list)args) != -1)
                    result = OK;
#endif
                va_end(args);
            }
        }
        return result;
    }


    int
    NCWindow::printw(const char * fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        char buf[BUFSIZ];
        ::vsprintf(buf, fmt, args);
        va_end(args);
        return waddstr(m_cwin, buf);
    }


    int
    NCWindow::printw(int y, int x, const char * fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        int result = ::wmove(m_cwin, y, x);
        if (result == OK) {
            char buf[BUFSIZ];
            ::vsprintf(buf, fmt, args);
            result = waddstr(m_cwin, buf);
        }
        va_end(args);
        return result;
    }


    void
    NCWindow::err_handler(const char *msg) const throw(NCException)
    {
        throw NCException(msg);
    }

    void NCWindow::init()
    {
        static uint32_t IDs = 0;
        this->m_id = ++IDs;
        if( 0 == m_id ) /* overflow */
        {
            throw NCException("Numeric overflow while getting window ID!");
        }
        lifecheck().insert(this);
        this->m_clientflags = 0L;
        this->leaveok(0);
        this->keypad(1);
        this->meta(1);
    }

    void NCWindow::ncwindow( WINDOW * w )
    {
        if( (!w) || (this->m_cwin == w) ) return;
        if( this->m_ownswin && this->m_cwin )
        {
            ::delwin(m_cwin);
        }
        this->m_ownswin = true;
        this->m_cwin = w;
    }

    unsigned long NCWindow::clientFlags() const
    {
        return this->m_clientflags;
    }

    unsigned long NCWindow::clientFlags( unsigned long newflags )
    {
        unsigned long old = this->m_clientflags;
        this->m_clientflags = newflags;
        return old;
    }

    std::string NCWindow::name() const
    {
        if( ! this->m_name.empty() ) return this->m_name;
        std::ostringstream str;
        str << "NCWindow "
            << std::dec<<this->height() << "Rx"
            << this->width() << "C @ "
            << "r"<<this->begy()
            <<" c"<<this->begx();
        // ^^^ the position is of course useless: they're relative to parent window :/
        return str.str();
    }

    void NCWindow::name( const std::string & n )
    {
        this->m_name = n;
    }


    NCWindow::NCWindow() throw(NCException)
    {

        m_cwin = (WINDOW *)0;
        m_ownswin = FALSE;
        subwins = par = sib = 0;
        this->init();
    }

    NCWindow::NCWindow(int lines, int cols, int begin_y, int begin_x) throw(NCException)
    {

        m_cwin = ::newwin(lines, cols, begin_y, begin_x);
        if (m_cwin == 0) {
            err_handler("NCWindow(lines,cols,begy,begx): Cannot construct window");
        }
        m_ownswin = TRUE;
        subwins = par = sib = 0;
        this->init();
    }

    NCWindow::NCWindow(WINDOW* &window) throw(NCException)
    {
        m_cwin = window;
        m_ownswin = FALSE;
        subwins = par = sib = 0;
        this->init();
    }

    NCWindow::NCWindow(NCWindow& win, int l, int c,
                       int begin_y, int begin_x, bool absolute) throw(NCException)
    {
        if (absolute) { // absolute origin
            begin_y -= win.begy();
            begin_x -= win.begx();
        }

        // Even though we treat subwindows as a tree, the standard curses
        // library needs the `subwin' call to link to the parent in
        // order to correctly perform refreshes, etc.
        // Friendly enough, this also works for pads.
        m_cwin = ::derwin(win.m_cwin, l, c, begin_y, begin_x);
        if (m_cwin == 0) {
            std::ostringstream msg;
            msg << "NCWindow(NCWindow&,rows="<<l <<", cols="<<c
                << ", y="<<begin_y <<", x="<<begin_x
                <<",absolute="<<absolute
                <<")\ncannot construct subwindow: derwin() failed: :("
                ;
            err_handler( msg.str().c_str() );
        }

        par = &win;
        sib = win.subwins;
        win.subwins = this;
        subwins = 0;
        m_ownswin = TRUE;
        this->init();
    }

    NCWindow::NCWindow(NCWindow& win,
                       bool do_box) throw(NCException)
    {
        m_cwin = :: derwin(win.m_cwin,win.height()-2,win.width()-2,1,1);
        if (m_cwin == 0) {
            err_handler("NCWindow(NCWindow&,bool) Cannot construct subwindow!");
        }

        par = &win;
        sib = win.subwins;
        win.subwins = this;
        subwins = 0;
        m_ownswin = TRUE;

        this->init();
        if (do_box) {
            win.box();
            win.touchwin();
        }
    }

    typedef int (*RIPOFFINIT)(NCWindow&);
    static RIPOFFINIT R_INIT[5];       // There can't be more
    static int r_init_idx   = 0;
    static RIPOFFINIT* prip = R_INIT;

    extern "C" int _nc_ripoffline(int,int (*init)(WINDOW*,int));

    NCWindow::NCWindow(WINDOW *win, int cols) throw(NCException)
    {
        m_cwin = win;
        if( (m_cwin->_maxx+1)!=cols) throw NCException("m_cwin->_maxx+1 != cols");
        m_ownswin = FALSE;
        subwins = par = sib = 0;
        this->init();
    }

    int NCWindow::ripoff_init(WINDOW *m_cwin, int cols)
    {
        int res = ERR;

        RIPOFFINIT init = *prip++;
        if (init) {
            NCWindow* W = new NCWindow(m_cwin,cols);
            res = init(*W);
        }
        return res;
    }

    int NCWindow::ripoffline(int ripoff_lines,
                             int (*init)(NCWindow& win))
    {
        int code = ::ripoffline(ripoff_lines,ripoff_init);
        if (code==OK && init && ripoff_lines) {
            R_INIT[r_init_idx++] = init;
        }
        return code;
    }

    bool
    NCWindow::isDescendant(NCWindow& win)
    {
        for (NCWindow* p = subwins; p != NULL; p = p->sib) {
            if (p==&win)
                return TRUE;
            else {
                if (p->isDescendant(win))
                    return TRUE;
            }
        }
        return FALSE;
    }

    void
    NCWindow::kill_subwindows()
    {
        if( ! this->subwins ) return;
        for (NCWindow* p = subwins; p != 0; p = p->sib ) {
            p->kill_subwindows();
            if (p->m_ownswin) {
                if (p->m_cwin != 0)
                    ::delwin(p->m_cwin);
                p->m_ownswin = FALSE;
            }
            p->par = 0;
            p->m_cwin = 0; // cause a run-time error if anyone attempts to use...
        }
        this->subwins = 0;
    }


    NCWindow::~NCWindow()
    {
        lifecheck().erase(this);
        this->clear();
        kill_subwindows();
        if( (par != 0) ) //&& NCWindow::is_alive(par) )
        {  // Snip us from the parent's list of subwindows.
            NCWindow * win = par->subwins;
            NCWindow * trail = 0;
            for (;;)
            {
                if (win == 0)
                {
                    break;
                }
                else if (win == this)
                {
                    if (trail != 0)
                        trail->sib = win->sib;
                    else
                        par->subwins = win->sib;
                    break;
                } else
                {
                    trail = win;
                    win = win->sib;
                }
            }
        }
        if (m_ownswin && (0 != m_cwin))
            ::delwin(m_cwin);
    }

    uint32_t NCWindow::windowID() const
    {
        return this->m_id;
    }

} // namespace ncutil


#if NCURSES_MOUSE_VERSION
extern "C" int _nc_has_mouse(void);
#endif
bool
ncutil::NCWindow::has_mouse() const
{
#ifdef NCURSES_MOUSE_VERSION
    return ((::has_key(KEY_MOUSE) || ::_nc_has_mouse())
            ? true : false);
#else
    return false;
#endif
}

