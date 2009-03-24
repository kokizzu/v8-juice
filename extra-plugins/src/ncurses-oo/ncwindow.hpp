#ifndef ncutil_NCWINDOW_HPP_INCLUDED
#define ncutil_NCWINDOW_HPP_INCLUDED 1

/****************************************************************************
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
by stephan@s11n.net, starting 1 March 2005:

- Moved into ncutil namespace and all classes renamed: s/NCurses/NC/

- Removed palette/color-related functions, as they are handled at
  the ncutil framework level.

- An ongoing redesign of some of the OO components, especially vis-a-vis
  OO design, event handling and NCPad.

- NCWindow no longer does ::initwin()/::endwin() management: that belongs
  to NCMode so that we can use ::getch(), ::init_pair(), etc., without
  having to know if an NCWindow has been created.

- Added throw(NCException) qualifiers to many functions.

- Added NCKeyConsumer as an abstract base for NCWindow, mainly so we can
  treat NCPad and NCPanel the same for event consuming purposes. (This
  simplifies some forms of window management.)

- Some non-strictly-curses functions renamed (like Win() !?!?!).

- Lots of docs added. Standard curses funcs are not documented here:
  see 'man ncurses' for more than you want to know.

- Added NCWindow::is_alive() to help support scripting bindings with
indeterminate destruction timing (e.g.  a JavaScript wrapper).
************************************************************************/


#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#include <map>
#include <set>
#include <string>

#include "ncexception.hpp"
#include "ncmode.hpp"
#include "nckey.hpp"

extern "C" {
#  include   <ncurses.h>
}

#include "ncwindow.redefs.hpp" // accomodates lots of gory curses details

//#include "sigslot.hpp"

namespace ncutil {



    /***
	
    C++ class for curseswindows.


    TODOs: (stephan@s11n.net)

    - Add get/setKeyConsumer(), so we can direct key handling to
    other handlers without subclassing and reimplementing
    consumeKey().

    - investigate window resize possibilities. The trick is
    notifying clients, but that can be solved with a functor and a
    signal/slots mechanism.
    */
    class  NCWindow : public NCKeyConsumer
    {
    private:
        NCMode m_sentry;
        static void    initialize(){} // ancient - not needed with NCMode
        static int     ripoff_init(WINDOW *, int);
        unsigned long m_clientflags;
        std::string m_name;

        WINDOW*        m_cwin;                // the curses WINDOW

        NCWindow* par;              // parent, if subwindow
        NCWindow* subwins;          // head of subwindows list
        NCWindow* sib;              // next subwindow of parent
        bool           m_ownswin;          // TRUE if we own the WINDOW
        uint32_t m_id;
        void init();

        typedef std::set<NCWindow const *> LifetimeCheck;
        static LifetimeCheck & lifecheck();

        // This private constructor is only used during the initialization
        // of windows generated by ripoffline() calls.
        NCWindow(WINDOW* win, int cols) throw(NCException);
    protected:
        virtual void err_handler(const char *) const throw(NCException);
        // Signal an error with the given message text.

        void           kill_subwindows(); // disable all subwindows
        // Destroy all subwindows.

        /* Only for use by derived classes. They are then in charge to
           fill the member variables correctly. */
        NCWindow() throw(NCException);

        /**
           Transfers ownership of w to this object, freeing any
           previously-owned WINDOW of this object.

           This is a no-op if w is 0 or is the same as this
           object's current WINDOW.
        */
        void ncwindow( WINDOW * w );

    public:
        NCWindow(WINDOW* &window) throw(NCException);  // useful only for stdscr

        NCWindow(int lines,         // number of lines
                 int cols,          // number of columns
                 int begin_y,       // line origin
                 int begin_x) throw(NCException);      // col origin

        NCWindow(NCWindow& par,// parent window
                 int lines,         // number of lines
                 int cols,          // number of columns
                 int begin_y,       // absolute or relative
                 int begin_x,       //   origins:
                 bool absolute = false ) throw(NCException);
        // if absolute, begin_y & begin_x are
        // absolute screen pos, else they are relative to par origin

        NCWindow(NCWindow& par,// parent window
                 bool do_box = TRUE) throw(NCException);
        // this is the very common case that we want to create the subwindow that
        // is two lines and two columns smaller and begins at (1,1).
        // We may automatically request the box around it.


        /**
           Returns true if the given pointer is a non-destructed NCWindow object.
           This is used to assist in a JavaScript wrapper, where indeterminate
           destruction order can cause problems.
        */
        static bool is_alive( NCWindow const * );

        /**
           Reimplemented from NCKeyConsumer interface. Default
           implementation does nothing and returns false
           unless...

           If this object has subwindows, we iterate through
           the subwindows, in the order they were added, until
           one's consumeKey() returns true.  If none return
           true, we return false.

           NOTE that NCPads which wrap this window have NO
           line of communication FROM this window to that one,
           so we cannot pass events to an NCFramedPad "child"
           (and NCPads do not have parent windows in the
           conventional sense). This is on the to-fix list
           somewhere.

           It is arguably cleaner to leave this OUT of this
           class and force clients to use a non-widget event
           handler, but: a) that is still possible with this
           model and b) the main intent here is to simplify
           generic window handling by providing one common event
           handing interface.

           The implementation currently disregards the second
           argument, but does pass it to subwindows for
           analysis.
        */
        virtual bool consumeKey( int, bool );

        /**
           Returns this objects client-designated flags (default = 0).
        */
        unsigned long clientFlags() const;

        /**
           Returns this object's name. If name(string) has not
           been called then a default name is used, which
           shows the window's geometry (mainly of interest for
           locating an (NCWindow *) during debuggering/testing).
        */
        std::string name() const;

        /**
           Sets this object's name. The interpretation/use of
           the name is client-dependent: this class places no
           meaning on it and does not use it.
        */
        void name( const std::string & n );

        /**
           Sets client-dependent flags. This object places no value on them.
           Returns the old value of the flags.
        */
        unsigned long clientFlags( unsigned long newflags );

        virtual ~NCWindow();

        /**
           Returns this object's associate curses WINDOW. Does
           not transfer ownership.
        */
        WINDOW * ncwindow() const { return this->m_cwin; }


        static int ripoffline(int ripoff_lines,
                              int (*init)(NCWindow& win));
        // This function is used to generate a window of ripped-of lines.
        // If the argument is positive, lines are removed from the top, if it
        // is negative lines are removed from the bottom. This enhances the
        // lowlevel ripoffline() function because it uses the internal
        // implementation that allows to remove more than just a single line.
        // This function must be called before any other ncurses function. The
        // creation of the window is deferred until ncurses gets initialized.
        // The initialization function is then called.

        // -------------------------------------------------------------------------
        // terminal status
        // -------------------------------------------------------------------------
        int            lines() const { initialize(); return LINES; }
        // Number of lines on terminal, *not* window

        int            cols() const { initialize(); return COLS; }
        // Number of cols  on terminal, *not* window

        int            tabsize() const { initialize(); return TABSIZE; }
        // Size of a tab on terminal, *not* window

//         static int     NumberOfColors();
//         // Number of available colors

//         int            colors() const { return NumberOfColors(); }
        // Number of available colors

        // -------------------------------------------------------------------------
        // window status
        // -------------------------------------------------------------------------
        int            height() const { return maxy() + 1; }
        // Number of lines in this window

        int            width() const { return maxx() + 1; }
        // Number of columns in this window

        int            begx() const { return m_cwin->_begx; }
        // Column of top left corner relative to stdscr

        int            begy() const { return m_cwin->_begy; }
        // Line of top left corner relative to stdscr

        int            maxx() const { return m_cwin->_maxx; }
        // Largest x coord in window

        int            maxy() const { return m_cwin->_maxy; }
        // Largest y coord in window



        // -------------------------------------------------------------------------
        // window positioning
        // -------------------------------------------------------------------------
        virtual int    mvwin(int begin_y, int begin_x) {
            return ::mvwin(m_cwin, begin_y, begin_x); }
        // Move window to new position with the new position as top left corner.
        // This is virtual because it is redefined in NCPanel.

        // -------------------------------------------------------------------------
        // coordinate positioning
        // -------------------------------------------------------------------------
        int            move(int y, int x) { return ::wmove(m_cwin, y, x); }
        // Move cursor the this position

        void           getyx(int& y, int& x) const { ::getyx(m_cwin, y, x); }
        // Get current position of the cursor

        int            mvcur(int oldrow, int oldcol, int newrow, int newcol) const {
            return ::mvcur(oldrow, oldcol, newrow, newcol); }
        // Perform lowlevel cursor motion that takes effect immediately.

        // -------------------------------------------------------------------------
        // input
        // -------------------------------------------------------------------------
        void timeout(int delay) { ::wtimeout( this->m_cwin, delay); }
        int nodelay(bool b) { return ::nodelay( this->m_cwin, b); }

        int            getch() { return ::wgetch(m_cwin); }
        // Get a keystroke from the window.

        int            getch(int y, int x) { return ::mvwgetch(m_cwin, y, x); }
        // Move cursor to position and get a keystroke from the window

        int            getstr(char* str, int n=-1) {
            return ::wgetnstr(m_cwin, str, n); }
        // Read a series of characters into str until a newline or carriage return
        // is received. Read at most n characters. If n is negative, the limit is
        // ignored.

        int            getstr(int y, int x, char* str, int n=-1) {
            return ::mvwgetnstr(m_cwin, y, x, str, n); }
        // Move the cursor to the requested position and then perform the getstr()
        // as described above.

        int            instr(char *s, int n=-1) { return ::winnstr(m_cwin, s, n); }
        // Get a string of characters from the window into the buffer s. Retrieve
        // at most n characters, if n is negative retrieve all characters up to the
        // end of the current line. Attributes are stripped from the characters.

        int            instr(int y, int x, char *s, int n=-1) {
            return ::mvwinnstr(m_cwin, y, x, s, n); }
        // Move the cursor to the requested position and then perform the instr()
        // as described above.

        int            scanw(const char* fmt, ...)
        // Perform a scanw function from the window.
#if __GNUG__ >= 2
            __attribute__ ((format (scanf, 2, 3)));
#else
        ;
#endif

        int            scanw(int y, int x, const char* fmt, ...)
        // Move the cursor to the requested position and then perform a scanw
        // from the window.
#if __GNUG__ >= 2
            __attribute__ ((format (scanf, 4, 5)));
#else
        ;
#endif

        // -------------------------------------------------------------------------
        // output
        // -------------------------------------------------------------------------
        int            addch(const chtype ch) { return ::waddch(m_cwin, ch); }
        // Put attributed character to the window.

        int            addch(int y, int x, const chtype ch) {
            return ::mvwaddch(m_cwin, y, x, ch); }
        // Move cursor to the requested position and then put attributed character
        // to the window.

        int            echochar(const chtype ch) { return ::wechochar(m_cwin, ch); }
        // Put attributed character to the window and refresh it immediately.

        int            addstr(const char* str, int n=-1) {
            return ::waddnstr(m_cwin, str, n); }
        // Write the string str to the window, stop writing if the terminating
        // NUL or the limit n is reached. If n is negative, it is ignored.

        int            addstr(int y, int x, const char * str, int n=-1) {
            return ::mvwaddnstr(m_cwin, y, x, str, n); }
        // Move the cursor to the requested position and then perform the addstr
        // as described above.

        int            printw(const char* fmt, ...)
        // Do a formatted print to the window.
#if (__GNUG__ >= 2) && !defined(printf)
            __attribute__ ((format (printf, 2, 3)));
#else
        ;
#endif

        int            printw(int y, int x, const char * fmt, ...)
        // Move the cursor and then do a formatted print to the window.
#if (__GNUG__ >= 2) && !defined(printf)
            __attribute__ ((format (printf, 4, 5)));
#else
        ;
#endif

        chtype         inch() const { return ::winch(m_cwin); }
        // Retrieve attributed character under the current cursor position.

        chtype         inch(int y, int x) { return ::mvwinch(m_cwin, y, x); }
        // Move cursor to requested position and then retrieve attributed character
        // at this position.

        int            insch(chtype ch) { return ::winsch(m_cwin, ch); }
        // Insert attributed character into the window before current cursor
        // position.

        int            insch(int y, int x, chtype ch) {
            return ::mvwinsch(m_cwin, y, x, ch); }
        // Move cursor to requested position and then insert the attributed
        // character before that position.

        int            insertln() { return ::winsdelln(m_cwin, 1); }
        // Insert an empty line above the current line.

        int            insdelln(int n=1) { return ::winsdelln(m_cwin, n); }
        // If n>0 insert that many lines above the current line. If n<0 delete
        // that many lines beginning with the current line.

        int            insstr(const char *s, int n=-1) {
            return ::winsnstr(m_cwin, s, n); }
        // Insert the string into the window before the current cursor position.
        // Insert stops at end of string or when the limit n is reached. If n is
        // negative, it is ignored.

        int            insstr(int y, int x, const char *s, int n=-1) {
            return ::mvwinsnstr(m_cwin, y, x, s, n); }
        // Move the cursor to the requested position and then perform the insstr()
        // as described above.

        int            attron (chtype at) { return ::wattron (m_cwin, at); }
        // Switch on the window attributes;

        int            attroff(chtype at) { return ::wattroff(m_cwin, (int) at); }
        // Switch off the window attributes;

        int            attrset(chtype at) { return ::wattrset(m_cwin, (int) at); }
        // Set the window attributes;

        int            color_set(short color_pair_number, void* opts=NULL) {
            return ::wcolor_set(m_cwin, color_pair_number, opts); }
        // Set the window color attribute;

        int            chgat(int n, attr_t attr, short color, const void *opts=NULL) {
            return ::wchgat(m_cwin, n, attr, color, opts); }
        // Change the attributes of the next n characters in the current line. If
        // n is negative or greater than the number of remaining characters in the
        // line, the attributes will be changed up to the end of the line.

        int            chgat(int y, int x,
                             int n, attr_t attr, short color, const void *opts=NULL) {
            return ::mvwchgat(m_cwin, y, x, n, attr, color, opts); }
        // Move the cursor to the requested position and then perform chgat() as
        // described above.

        // -------------------------------------------------------------------------
        // background
        // -------------------------------------------------------------------------
        chtype         getbkgd() const { return ::getbkgd(m_cwin); }
        // Get current background setting.

        int            bkgd(chtype ch) { return ::wbkgd(m_cwin, ch); }
        // Set the background property and apply it to the window.

        void           bkgdset(chtype ch) { ::wbkgdset(m_cwin, ch); }
        // Set the background property.

        // -------------------------------------------------------------------------
        // borders
        // -------------------------------------------------------------------------
        int            box(chtype vert=0, chtype  hor=0) {
            return ::wborder(m_cwin, vert, vert, hor, hor, 0, 0, 0, 0); }
        // Draw a box around the window with the given vertical and horizontal
        // drawing characters. If you specify a zero as character, curses will try
        // to find a "nice" character.

        int            border(chtype left=0, chtype right=0,
                              chtype top =0, chtype bottom=0,
                              chtype top_left =0, chtype top_right=0,
                              chtype bottom_left =0, chtype bottom_right=0) {
            return ::wborder(m_cwin, left, right, top, bottom, top_left, top_right,
                             bottom_left, bottom_right); }
        // Draw a border around the window with the given characters for the
        // various parts of the border. If you pass zero for a character, curses
        // will try to find "nice" characters.

        // -------------------------------------------------------------------------
        // lines and boxes
        // -------------------------------------------------------------------------
        int            hline(int len, chtype ch=0) { return ::whline(m_cwin, ch, len); }
        // Draw a horizontal line of len characters with the given character. If
        // you pass zero for the character, curses will try to find a "nice" one.

        int            hline(int y, int x, int len, chtype ch=0) {
            return ::mvwhline(m_cwin, y, x, ch, len); }
        // Move the cursor to the requested position and then draw a horizontal line.

        int            vline(int len, chtype ch=0) { return ::wvline(m_cwin, ch, len); }
        // Draw a vertical line of len characters with the given character. If
        // you pass zero for the character, curses will try to find a "nice" one.

        int            vline(int y, int x, int len, chtype ch=0) {
            return ::mvwvline(m_cwin, y, x, ch, len); }
        // Move the cursor to the requested position and then draw a vertical line.

        // -------------------------------------------------------------------------
        // erasure
        // -------------------------------------------------------------------------
        int            erase() { return ::werase(m_cwin); }
        // Erase the window.

        int            clear() { return ::wclear(m_cwin); }
        // Clear the window.

        int            clearok(bool bf) { return ::clearok(m_cwin, bf); }
        // Set/Reset the clear flag. If set, the next refresh() will clear the
        // screen.

        int            clrtobot() { return ::wclrtobot(m_cwin); }
        // Clear to the end of the window.

        int            clrtoeol() { return ::wclrtoeol(m_cwin); }
        // Clear to the end of the line.

        int            delch() { return ::wdelch(m_cwin); }
        // Delete character under the cursor.

        int            delch(int y, int x) { return ::mvwdelch(m_cwin, y, x); }
        // Move cursor to requested position and delete the character under the
        // cursor.

        int            deleteln() { return ::winsdelln(m_cwin, -1); }
        // Delete the current line.

        // -------------------------------------------------------------------------
        // screen control
        // -------------------------------------------------------------------------
        int            scroll(int amount=1) { return ::wscrl(m_cwin, amount); }
        // Scroll amount lines. If amount is positive, scroll up, otherwise
        // scroll down.

        int            scrollok(bool bf) { return ::scrollok(m_cwin, bf); }
        // If bf is TRUE, window scrolls if cursor is moved off the bottom
        // edge of the window or a scrolling region, otherwise the cursor is left
        // at the bottom line.

        int            setscrreg(int from, int to) {
            return ::wsetscrreg(m_cwin, from, to); }
        // Define a soft scrolling region.

        int            idlok(bool bf) { return ::idlok(m_cwin, bf); }
        // If bf is TRUE, use insert/delete line hardware support if possible.
        // Otherwise do it in software.


        void           idcok(bool bf) { ::idcok(m_cwin, bf); }
        // If bf is TRUE, use insert/delete character hardware support if possible.
        // Otherwise do it in software.

        int            touchwin()   { return ::wtouchln(m_cwin, 0, height(), 1); }
        // Mark the whole window as modified.

        int            untouchwin() { return ::wtouchln(m_cwin, 0, height(), 0); }
        // Mark the whole window as unmodified.

        int            touchln(int s, int cnt, bool changed=TRUE) {
            return ::wtouchln(m_cwin, s, cnt, (int)(changed?1:0)); }
        // Mark cnt lines beginning from line s as changed or unchanged, depending
        // on the value of the changed flag.

        bool           is_linetouched(int line) const {
            return (::is_linetouched(m_cwin, line) ? TRUE:FALSE); }
        // Return TRUE if line is marked as changed, FALSE otherwise

        bool           is_wintouched() const {
            return (::is_wintouched(m_cwin) ? TRUE:FALSE); }
        // Return TRUE if window is marked as changed, FALSE otherwise

        int            leaveok(bool bf) { return ::leaveok(m_cwin, bf); }
        // If bf is TRUE, curses will leave the cursor after an update whereever
        // it is after the update.

        int            redrawln(int from, int n) { return ::wredrawln(m_cwin, from, n); }
        // Redraw n lines starting from the requested line

        int            redrawwin() { return ::wredrawln(m_cwin, 0, height()); }
        // Redraw the whole window

        int            doupdate()  { return ::doupdate(); }
        // Do all outputs to make the physical screen looking like the virtual one

        void           syncdown()  { ::wsyncdown(m_cwin); }
        // Propagate the changes down to all descendant windows

        void           syncup()    { ::wsyncup(m_cwin); }
        // Propagate the changes up in the hierarchy

        void           cursyncup() { ::wcursyncup(m_cwin); }
        // Position the cursor in all ancestor windows corresponding to our setting

        int            syncok(bool bf) { return ::syncok(m_cwin, bf); }
        // If called with bf=TRUE, syncup() is called whenever the window is changed

#ifndef _no_flushok
        int            flushok(bool bf) { return ::flushok(m_cwin, bf); }
#endif

        void           immedok(bool bf) { ::immedok(m_cwin, bf); }
        // If called with bf=TRUE, any change in the window will cause an
        // automatic immediate refresh()

        int            keypad(bool bf) { return ::keypad(m_cwin, bf); }
        // If called with bf=TRUE, the application will interpret function keys.

        int            meta(bool bf) { return ::meta(m_cwin, bf); }
        // If called with bf=TRUE, keys may generate 8-Bit characters. Otherwise
        // 7-Bit characters are generated.

        int            standout() { return ::wstandout(m_cwin); }
        // Enable "standout" attributes

        int            standend() { return ::wstandend(m_cwin); }
        // Disable "standout" attributes

        // -------------------------------------------------------------------------
        // The next two are virtual, because we redefine them in the
        // NCPanel class.
        // -------------------------------------------------------------------------
        virtual int    refresh() { return ::wrefresh(m_cwin); }
        // Propagate the changes in this window to the virtual screen and call
        // doupdate(). This is redefined in NCPanel.

        virtual int    noutrefresh() { return ::wnoutrefresh(m_cwin); }
        // Propagate the changes in this window to the virtual screen. This is
        // redefined in NCPanel.

        // -------------------------------------------------------------------------
        // multiple window control
        // -------------------------------------------------------------------------
        int            overlay(NCWindow& win) {
            return ::overlay(m_cwin, win.m_cwin); }
        // Overlay this window over win.

        int            overwrite(NCWindow& win) {
            return ::overwrite(m_cwin, win.m_cwin); }
        // Overwrite win with this window.

        int            copywin(NCWindow& win,
                               int sminrow, int smincol,
                               int dminrow, int dmincol,
                               int dmaxrow, int dmaxcol, bool overlay=TRUE) {
            return ::copywin(m_cwin, win.m_cwin, sminrow, smincol, dminrow, dmincol,
                             dmaxrow, dmaxcol, (int)(overlay?1:0)); }
        // Overlay or overwrite the rectangle in win given by dminrow,dmincol,
        // dmaxrow,dmaxcol with the rectangle in this window beginning at
        // sminrow,smincol.

        // -------------------------------------------------------------------------
        // Mouse related
        // -------------------------------------------------------------------------
        bool has_mouse() const;
        // Return TRUE if terminal supports a mouse, FALSE otherwise

        // -------------------------------------------------------------------------
        // traversal support
        // -------------------------------------------------------------------------
        NCWindow*  child() { return subwins; }
        // Get the first child window.

        NCWindow*  sibling() { return sib; }
        // Get the next child of my parent.

        NCWindow*  parent() { return par; }
        // Get my parent.

        bool isDescendant(NCWindow& win);
        // Return TRUE if win is a descendant of this.

        /**
           Returns a unique per-instance identifier. This is intended
           mainly to assist in mapping IDs to windows in
           script-language bindings (where C++ code can use the
           object's pointer as an identifier).

           The exact identifier number is unspecified, except that it
           will stay constant for the life of a given instance. The
           interface does not guaranty that IDs will be sequential
           numbers for sequentially-created windows.  The ID number 0
           is reserved for internal use, and is never a valid window
           ID.
        */
        uint32_t windowID() const;
    };

    /**
        Pad Support. We allow an association of a pad with a "real" window
	through which the pad may be viewed.

	Note that NCPad appears to be pretty useless by itself: use
	NCFramedPad. (stephan) It might be interesting for this class
	to NOT subclass NCWindow. In my experience the two have such
	different client-side usages that the subclassing doesn't make
	much sense.
    **/
    class  NCPad : public NCWindow {

    public:
        enum PadRequests {
        PadReqFirstRequest = KEY_MAX+1, // for use in looping. KEY_MAP ==> curses.h
        PadReqRefresh = PadReqFirstRequest,
        PadReqUp,
        PadReqDown,
        PadReqLeft,
        PadReqRight,
        PadReqExit,
        PadReqPageUp,
        PadReqPageDown,
        PadReqScrollToTop,
        PadReqScrollToBottom,
        // the remaining entries are used as return values, not operation types
        PadReqIgnored, // Returned to signal non-fatal requests, like attempts to scroll out of bounds.
        PadReqUnknown, // Returned to signify an unknown request.
        PadReqLastRequest = PadReqUnknown // for use in loops
        };


        /**
           Creates a pad with the given size. For the pad to be visible
           it needs to be associated with an NCWindow. Call setWindow()
           and/or setSubWindow
        */
        NCPad(int lines, int cols)  throw(NCException);
        // 

        virtual ~NCPad() {}


        NCWindow* getDrawWindow(void) const {
            // Get the window into which the pad should be copied (if any)
            return (viewSub?viewSub:(viewWin?viewWin:0));
        }

        NCWindow* getViewWindow(void) const {
            return viewWin;
        }

        NCWindow* getSubWindow(void) const {
            return viewSub;
        }


        /**
           Returns the number of pad rows/lines passed to the ctor.
        */
        int lineCount() const;

        /**
           Returns the number of pad columns passed to the ctor.
        */
        int columnCount() const;

        /**
           Reimplemented to do:


           Calls reqForKey(key). If that evaluates to
           PadReqUnknown then false is returned, otherwise we
           call requestOp() and return true.

           Notes about the true return for PadReqIgnored and PadReqExit:

           These could arguably be false. e.g., this object
           does nothing when PadReqExit is "handled", but
           returning false might confuse clients.

           The implementation currently disregards the second
           argument.
        */
        virtual bool consumeKey( int key, bool );

        // WTF do i need this for downstream code to see it???
        using NCKeyConsumer::consumeKey;

        /**
           For use in mapping KEY_xxx entries to PadReqXxx
           entries.
        */
        typedef std::map<int,int> key_req_map;

        /**
           By setting entries in this map you my assign curses
           KEY_xxx keystroke codes to PadReqXxx pad
           commands. The keys of the map are KEY_xxx values
           and the values are PadReqXxx values.

           e.g.:

           mypad.keyReqMap()['x'] = 
           mypad.keyReqMap()['X'] = NCFramedPad::PadReqExit;

        */
        key_req_map & keyReqMap();

        /**
           A const form of keyReqMap(), for pedantic stylistic
           reasons.
        */
        const key_req_map & keyReqMap() const;

        /**
           Convenience function to map keys into keyReqMap().
        */
        void mapKeyToReq( int key, int padreq );



        int echochar(chtype ch) { return ::pechochar(this->ncwindow(), ch); }
        // Put the attributed character onto the pad and immediately do a
        // prefresh().

        int refresh();
        // If a viewport is defined the pad is displayed in this window, otherwise
        // this is a noop.

        int refresh(int pminrow, int pmincol,
                    int sminrow, int smincol,
                    int smaxrow, int smaxcol) {
            return ::prefresh(this->ncwindow(), pminrow, pmincol,
                              sminrow, smincol, smaxrow, smaxcol);
        }
        // The coordinates sminrow,smincol,smaxrow,smaxcol describe a rectangle
        // on the screen. <b>refresh</b> copies a rectangle of this size beginning
        // with top left corner pminrow,pmincol onto the screen and calls doupdate().

        int noutrefresh();
        // If a viewport is defined the pad is displayed in this window, otherwise
        // this is a noop.

        int noutrefresh(int pminrow, int pmincol,
                        int sminrow, int smincol,
                        int smaxrow, int smaxcol) {
            return ::pnoutrefresh(this->ncwindow(), pminrow, pmincol,
                                  sminrow, smincol, smaxrow, smaxcol);
        }
        // Does the same as refresh() but without calling doupdate().

        /**
           Makes this object use view as it's quasi-parent widget. The view object
           will be completely obscured by this object and will have 1 extra level
           of padding to accomodate this object's scrollbars.

           This window is usually used to provide some decorations
           like frames, titles etc.

           If either v_grid or h_grid are less than 1 then they are silently
           converted to 1.
        */
        virtual void setWindow(NCWindow& view, int v_grid = 1, int h_grid = 1) throw(NCException);

        /**
           Use the subwindow "sub" of the viewport window for the
           actual viewing.  This will throw if setWindow() has not
           been called or if sub is not a descendant of the window set
           in setWindow(). Don't ask me why the latter is a
           requirement - those bits weren't implemented by me and the
           decision wasn't documented except in code form.
        */
        virtual void setSubWindow(NCWindow & sub) throw(NCException);

        /**
           Performs an input loop, waiting for keypresses from
           the user until a PadReqExit is triggered (try F10
           and see reqForKey()).

           requestOp(PadReqRefresh) will be called once before
           the actual input loop starts, and then once for each
           keypress until requestOp() returns PadReqExit.

           Note the default implementation does not call
           consumeKey(), because that function has no way of
           triggering an exit request.
        */
        virtual void inputLoop();


        /**
           Uses keyReqMap() to translate KEY_xxx curses values
           to PadReqXxx values.

           Passing a non-mapped op causes PadReqUnknown.

           Default key/request handling is:

           KEY_UP, KEY_DOWN, KEY_LEFT, and KEY_RIGHT are
           mapped appropriately. F10 is PadReqExit, though this is rather
           arbitrary (can anyone out there tell me how to
           properly capture the Escape key as a close key?).
           Ctrl-L is mapped to Redraw, as is conventional for
           that key.

           The PadReqHome/End (KEY_HOME, KEY_END) operations
           scroll to the top or bottom of the pad,
           respectively.

           PageUp/PageDown (KEY_PPAGE, KEY_NPAGE) return
           PadReqPageUp or PadReqPageDown, as appropriate.

        */
        int reqForKey(int key) const;


        /**
           This is the client-side entry to this class's
           operations.  Pass it PadRequest. It accepts an int
           so that we have room to extend the Request
           operations in subclasses without having to change
           this type's PadRequest enum.

           Return values:

           - The return value is op if the request
           is recognized and processed.

           - If PadReqIgnored is returned then the request was
           recognized as a valid command, but not carried out
           for some reason (attempts to move out of bounds or
           a similar non-serious quasi-error).

           - On an unknown command then PadReqUnknown is
           returned, which allows the user to then potentially
           check the requested op against their own list of
           commands, knowing that the pad didn't internally
           recognize it. (This is theoretically useful for,
           e.g., using this pad as a display of some sort of
           menu, and a keypress corresponds to one of the
           available options.)

           Subclasses should honor the above conventions.

           This function calls the protected OnOperation()
           function to perform it's actual work or
           OnNavigation() to report a navigation error (which
           is normally not a true error, but an "indicator",
           like a console beep).

           Note that a request might be honored yet cause no
           visual changes.

           Passing in PadReqExit does not cause any
           side-effects from this function. That request is
           used by inputLoop() to trigger a close of this pad.
           Clients may use that request to signal an exit of
           their own app, or closing of the pad, if they like.

           Notes regarding PadReqPageUp or PadReqPageDown:

           An "approximate" page up/down is done if this
           object's vertical grid is not 1 (as set via the
           ctor). The range of error is plus or
           minus the "vertical grid size" of this object,
           which determines how many screen lines are scrolled
           via a PadReqUp/Down. Page up/down are simulated
           by calculating how many UP/DOWN requests to perform
           (plus or minus one vgrid increment), and then
           perform them all at once. Only one OnOperation() is
           performed, no matter how big of a jump a "page" is.
           PadReqIgnored is only returned for a page up/down
           if the "first step" of the scroll fails (i.e., it's
           already scrolled to that point), otherwise
           PadReqPageUp or PadReqPageDown is returned.

        */
        virtual int requestOp( int op  );


        /** The number of ticks an up/down scroll will perform. */
        int gridsize_y() const { return this->v_gridsize; }
        /** The number of ticks a left/right scroll will perform. */
        int gridsize_x() const { return this->h_gridsize; }

        /**
           Returns the logical line number of the top-most
           in-viewed line.
        */
        int topViewLine() const;

        /**
           Returns the logical column number of the left-most
           in-viewed column.
        */
        int leftViewColumn() const;


        /**
           Returns the height of this object's view window
        */
        int viewHeight() const;

    private:
        NCWindow* viewWin;       // the "viewport" window
        NCWindow* viewSub;       // the "viewport" subwindow

        int m_lines; // number of scroll lines
        int m_cols; // number of scroll cols
        int h_gridsize, v_gridsize; // cols/rows to jump for left/right/up/down requests
        key_req_map m_keys; // key-to-request map

        int min_row; // top row of display area
        int min_col; // left col of the display area

    protected:

        /**
           Sets internal line number. Does not update view. Does no bounds checking.
        */
        void topViewLine( int );
        /**
           Sets internal column number. Does not update view.
           Does no bounds checking.
        */
        void leftViewColumn( int );
        /**
           This is called from requestOp() if an unknown operation
           is requested. Default implementation does nothing.
        */
        virtual void OnUnknownOperation(int pad_req);

        /**
           This is called from requestOp() if a navigation
           request couldn't be satisfied.

           Default implementation calls ::beep().
        */
        virtual void OnNavigationError(int pad_req);

        /***

        OnOperation is called if a PadReqXXX was executed in
        requestOp(), just before the refresh() operation
        is done.

        The default implementation does nothing.
        */
        virtual void OnOperation(int pad_req);


    };



    /**
       An NCFramedPad is constructed always with a viewport
       window. This viewport will be framed (by a box() command)
       and the interior of the box is the viewport subwindow. On
       the frame we display scrollbar sliders.

       This type does not force the user into an input loop, and
       should play well alongside other windows using a
       client-side input loop. It optionally supports it's own
       input loop. Normally, however, Pad requests are dispatched
       to it by the user, via requestOp().  Calling inputLoop()
       enters an input loop private to this object.

       Note that it is possible to use multiple NCFramedPads actively
       at once, by catching input via a client-side loop and
       sending it to the requestOp() function of the appropriate
       pad(s).

       Pads are, IMO, a bit awkward to use. Apparently - from what
       i've gathered by looking at sample code and experimentation
       - a Pad is intended to overlay a whole other Window. Keep
       that in mind when using them.

       Drawing operations made to these objects will be reflected in
       the assigned subwindow (see setSubWindow()) during pad
       scrolling operations or when requestOp(PadReqRefresh) is called.

    */
    class  NCFramedPad : public NCPad
    {

    public:
        /**
           win == window to act as our quasi-parent, where we will paint ourselves.

           lines == number of logical (scrollable) lines in our view.

           cols == number of cols in our view. The default of
           0 means win.width()-2.


           v/h_grid == the number of screen lines which one
           PadReqUp/Down/Left/Right will scroll.


           BUGS:

           - If lines or cols are smaller than win then this
           widget will not render it's contents.

        */
        NCFramedPad(NCWindow& win, int lines, int cols = 0,
                    int v_grid = 1, int h_grid = 1);

        virtual ~NCFramedPad();

        /**
           GUARANTEED to throw an NCException. This object
           does not accomodate changing windows from the one
           set in the ctor.

           The automatic throw() behaviour is historical,
           inherited from the ncurses++ implementation. As far
           as i can see, if THIS class doesn't support it then
           neither does NCPad *after* the first time you've
           set a view.


           The problem with changing views is only that this->getSubWindow() must
           be created as a subwindow of viewWin. i'm not sure
           why, but the code goes through the trouble to throw
           if that's not the case, so i'll assume it's desired
           (i didn't write those bits).

           This class may change in the future to accomodate
           setting the view/sub windows, with the caveat that
           sub must have been created (by the client) as a
           subwindow.
        */
        void setWindow(NCWindow& viewWin, int v_grid = 1, int h_grid = 1)  throw(NCException);
        // Disable this call: the viewport is already defined

        /**
           GUARANTEED to throw an NCException. This object
           owns it's one and only subwindow.

           See the docs for setWindow(), which explain the
           historical nature of this exception.
        */
        virtual void setSubWindow(NCWindow& sub) throw(NCException);

        /**
           Reimplemented to draw this object's scrollbars.
        */
        virtual void OnOperation(int pad_req);

        // WTF do i need this for downstream code to see it???
        using NCKeyConsumer::consumeKey;

    };

} // namespace ncutil

#endif // ncutil_NCWINDOW_HPP_INCLUDED
