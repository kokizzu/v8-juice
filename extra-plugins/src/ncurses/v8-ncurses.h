#if !defined(CODE_GOOGLE_COM_P_V8_V8_P3_NCURSES_H_INCLUDED)
#define CODE_GOOGLE_COM_P_V8_V8_P3_NCURSES_H_INCLUDED 1
/**
   Implementation code for v8 (JavaScript engine) bindings for sqlite3.

   v8: http://code.google.com/p/v8/

   sqlite3: http://sqlite.org

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: same as v8 (see below)
*/
// Copyright 2007-2008 the V8 project authors. All rights reserved.
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

namespace v8 {
/**
   The juice namespace is for "3rd Party" add-ons written for v8.
*/
namespace juice {
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

    enum { library_version = 0x20090302 };

    /**
       This function installs JS bindings for the ncurses C API into
       the given object. How the API is installed depends on the poluteGlobal
       parameter:

       - If poluteGlobal is true, all wrapped ncurses functions are added
       to the given object and all function names are prefixed with "nc_"
       (because many of them are otherwise very ambiguous, like clear(),
       refresh(), and echo()).

       - If poluteGlobal is false then the given object only gets one member
       injected into it - an object called "ncurses" (see below).

       Regardless of the value of poluteGlobal, the passed-in object
       gets a JS object names "ncurses" added to it. That object has
       the following public properties:

       - All wrapped functions are bound as members of this object
       using their original names (with no "nc_" prefix). That is, the
       attached functions have the same names as in the native ncurses
       API.

       - Array i2a maps integer values 0-127 to their ASCII counterparts.

       - Array a2i maps characters with ascii values 0-127 to integers.

       - All (or most) of the various ncurses macros are defined as member
       values, e.g. ncurses.OK, ncurses.KEY_DOWN, etc. Note, however,
       that some of those "constants" do not actually have a value
       until after nc_initscr() is called (that is, until ncurses has
       been initialized).

       While the bound API closely matches the native API, there are a
       few notable differences and caveats:

       - Where routines exist in "w" and non-w forms, e.g. refresh()
       and wrefresh(), the non-w form is almost always omitted, as it
       is considered better form to call the w-variant and pass stdscr
       to them. e.g. call wrefresh(stdscr) instead of refresh(). One
       notable exception is getch(); in practice it makes no
       difference which window is passed to wgetch() (at least,
       assuming echoing is turned off), so we provide getch() as well
       as wgetch(), and client code can normally safely use getch() for
       most key-fetching.

       - See nc_endwin() for info about shelling out of ncurses mode.

       - Failing to destroy all created windows, panels, and pads using
       the C-style approach will result in memory leaks.

       - The bindings have no support for SIGWINCH/terminal resizing.


       General tips for working with curses:

       - Patience. Curses has its name for a reason. Even seasoned
       curses programmers curse at it quite often.

       - Never draw to stdscr unless it is your only window. Instead,
       make subwindows and draw on those.

       - Avoid moving WINDOWs around. It's problematic in
       curses. PANELs, on the other hand, are easy to move (and have
       predictable results).

       - Prefer the PANEL API for top-level widget management, and
       populate them with subwindows if needed. One useful practice is
       to create a "root" panel as large as stdscr, then layer "client
       area" panels on top of that.

       - If you're in a unix shell and a curses script quits without
       properly exiting curses mode, your terminal may misbehave
       (e.g. your keyboard input may become invisible and newlines
       don't work properly). Normally this is easily solved by typing
       "reset" into your terminal (and tap ENTER, even though you
       can't see the text).

       - If your application sends output to stdout or stderr, that
       WILL interfere with the screen. One workaround is to send any
       debugging output to stderr, then redirect stderr to a different
       terminal when starting the application/script. On Linux (and
       probably other Unicies): type "tty" at a shell prompt to get
       the device name for that tty, then (from another console
       window) run your app and redirect stderr to that device name. Do
       not redirect std out to another console, as that will confuse
       curses.
    */
    ::v8::Handle< ::v8::Value > SetupAddon( ::v8::Handle< ::v8::Object > target, bool poluteGlobal );

    /** \def V8_BINDING

    Functions declared with V8_BINDING are not really implemented -
    this is a marker to show which ncurses functions have JS
    bindings. Any undocumented entries work as described in the
    native C API. Documented entries normally have slightly
    different semantics than in the native API, due to differences
    in argument handling in C and JS. Every effort has been made to
    make the JS API as close to the native as possible, but some
    differences are unavoidable.

    The functions might not be named (in JS) the same as here! See
    SetupAddon() for how to enable/disable polution of the global
    namespace!

    Most of the functions handle errors the same as their C
    counterparts, but some throw exceptions. The guideline is this:
    errors reported by the native API will always be reported back
    using the native error code (the constant ncurses.ERR on error and
    "some other value" on success). For JS-side errors, for example
    incompatible argument types or count, the functions may throw a
    JS-side exception or translate the error to some native error code
    (most will throw, however).
    */
#define V8_BINDING
    
    /**
       Sets up curses mode, changing the screen state to "graphics"
       mode (if we can call it that).

       While the majority of the API is a 1-to-1 mapping of the native
       interface, this function does some convenience operations which
       are not normally done by ::initscr(), namely:

       - If color mode is available, it sets up a 64-color
       palette. See nc_color_pair().

       - It calls meta(stdscr,true), cbreak(stdscr,true), and
       noecho(), since those seem to be the most useful defaults.

       Some of the ncurses API constants (e.g. the ACS_xxx macros,
       defined script-side as properties of 'ncurses' object) are not
       initialized until this routine is called.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_initscr( ::v8::Arguments const & );
    /**
       Exits curses mode, restoring the screen to its previous state. One must
       call nc_delwin() (or equivalent) for each client-created window clean them
       up before calling this, or resources will leak.

       In the native API it is possible to call endwin(), shell out
       (e.g. call system()), then revert to curses mode using
       refresh().  Because of the way object ownership is implemented
       and tracked, that approach to shelling out is not possible in
       this implementation because the call to endwin() really
       destroys the underlying window object(s). A workaround? i'll
       let you know if i find one (the prerequisite of which is me
       personally needing a reason to shell out).
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_endwin( ::v8::Arguments const & );

    /** Returns integer value of pressed key. */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_getch( ::v8::Arguments const & );

    /**
       Creates a new top-level window handle, or returns null if
       ::newwin() fails.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_newwin( ::v8::Arguments const & );
    /**
       Draws a border around the given window.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wborder( ::v8::Arguments const & );
    /** Accepts 1 or more WINDOW or PANEL arguments.  For PANELs
	it does the update technique appropriate for PANELs (or at
	least it does what *it thinks* is appropriate for PANELs).
	If you pass it a PAD then it will silently fail without
	crashing.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wrefresh( ::v8::Arguments const & );
    /**
       Passed foreground and background color names (as strings), it
       returns a curses color pair encoded as an integer.

       The fore/background color names are case-insensitive, and must
       be one of:

       BLACK
       RED
       GREEN
       YELLOW
       BLUE
       MAGENTA
       CYAN
       WHITE
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_color_pair( ::v8::Arguments const & );
    /**
       Passed foreground and background color names (as strings), it
       returns a curses color pair id (not the color pair value!) as
       an integer. For the list of color names, see nc_color_pair().

       The function is only rarely useful, but is sometimes required,
       e.g. when using nc_wchgat().

       JS signature:

       [short] int nc_color_pairnum(foreground,background)

    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_color_pairnum( ::v8::Arguments const & );
    /** JS usage: nc_echo([bool=true]). */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_echo( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_noecho( ::v8::Arguments const & );
    /** JS usage: nc_cbreak([bool=true]). */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_cbreak( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_nocbreak( ::v8::Arguments const & );
    /**
       Cleans up WINDOW object argv[0] but does not clean up
       subwindows (delete those first!).

       Conversely, if the given WINDOW has a PANEL associated
       with it (created using new_panel()) then that panel object
       IS also destroyed. The curses docs are not specific on how
       they behave in this case, but this implementation demands
       that the associated window also be destroyed.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_delwin( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wclear( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_werase( ::v8::Arguments const & );
    /** Returns the width of the first argument, which must be a
	WINDOW handle.

	JS usage:

	var x = nc_wwidth( mywin );
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wwidth( ::v8::Arguments const & );
    /** Returns the height of the first arument, which must
	be a WINDOW handle.

	Usage:

	var x = nc_wheight( mywin );
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wheight( ::v8::Arguments const & );
    /** Returns the row number of the bottom edge of the
	first arument, which must be a WINDOW handle.

	Usage:

	var y = nc_wmaxy( mywin );
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wmaxy( ::v8::Arguments const & );
    /** Returns the column number of the right-hand edge of
	the first arument, which must be a WINDOW handle
	
	Usage:
	
	var x = nc_wmaxx( mywin );
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wmaxx( ::v8::Arguments const & );
    /** Returns the column number of argv[0]'s left edge. */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wbegy( ::v8::Arguments const & );
    /** Returns the column number of argv[0]'s top edge. */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wbegx( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_meta( ::v8::Arguments const & );
    /** Moves the CURSOR, not the WINDOW. */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wmove( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_newpad( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_doupdate( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_redrawwin( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wredrawln( ::v8::Arguments const & );
    /**
       nc_delete() takes any number of curses WINDOW, PAD, or PANEL
       handles as arguments and frees them. This is equivalent to
       using nc_delwin() or del_panel(), depending on the exact
       window handle type, and keeps the client from having to know
       which one to call for a given widget.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_delete( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_overlay( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_overwrite( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_keyok( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_clearok( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_box( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_whline( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wvline( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_ungetch( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_flushinp( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_halfdelay( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mvwhline( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mvwvline( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_touchline( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_touchwin( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_scrl( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_scroll( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_scrollok( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wscrl( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_pechochar( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_pnoutrefresh( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_prefresh( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wstandend( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wstandout( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wsyncdown( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wsyncup( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wtimeout( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wtouchln( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_idcok( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_idlok( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_immedok( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_nl( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_nonl( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wsetscrreg( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_beep( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_flash( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_has_colors( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_can_change_color( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_raw( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_noraw( ::v8::Arguments const & );
    /**
       Returns the current y (row) coordinate of the given window
       handle.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_getcury( ::v8::Arguments const & );
    /**
       Returns the current x (column) coordinate of the given window
       handle.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_getcurx( ::v8::Arguments const & );
    /**
       Returns y/x position of cursor in WINDOW as an object:

       JS USage: var coords = nc_getcuryx(myWin);

       Result: {y:INT,x:INT}
    */

    V8_BINDING ::v8::Handle< ::v8::Value > nc_getcuryx( ::v8::Arguments const & );

    /**
       Returns the width of the terminal, in curses columns, or 0 if
       curses has not yet been initialized.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_screen_width( ::v8::Arguments const & );

    /**
       Returns the width of the terminal, in curses rows, or 0 if
       curses has not yet been initialized.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_screen_height( ::v8::Arguments const & );

    V8_BINDING ::v8::Handle< ::v8::Value > nc_winch( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mvwinch( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_keypad( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mvwchgat( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wchgat( ::v8::Arguments const & );

    /**
       Sleeps for argv[0] miliseconds.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_napms( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_curs_set( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wenclose( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mouseinterval( ::v8::Arguments const & );

    /**
       Similar, but not identical, to the native function...

       If passed no arguments then it acts like a getter, fetching
       the current mousemask value. If passed 1 argument it is
       used as a mouse mask and the mask which gets actually
       implemented is returned (which may be different than the
       requested mask).

       JS Usage:

       var mask = nc_mousemask(); // getter

       var actualmask = nc_mousemask( ... requested mask ... ); // setter


    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mousemask( ::v8::Arguments const & );

    /** Not yet implemented - with throw a JS-side exception if called. */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_ungetmouse( ::v8::Arguments const & );

    /**
       If the NCURSES_MOUSE_VERSION macro evaluates to false then this
       function returns null, otherwise it returns a JS object with
       the same properties as the native getmouse() function. Instead
       of taking a destination object as a parameter (as in C), it
       returns that object.

       JS Usage: var mouseEvent = nc_getmouse();

       The returned object contains these properties:

       .id = unique ID to distinguish between separate mice. (i'd like
       to know how many curses apps actually explicitly support more
       than one mouse.)

       .x and .y = the screen coordinates

       .z = undefined, but can theoretically be used for the
       mouse wheel. Not sure how, though.

       .bstate = a bitmask which maps to one of the values listed
       below.

       The bit-values used by the bstate property are stored
       as properties of the global ncurses object:

       BUTTON1_RELEASED, BUTTON1_PRESSED, BUTTON1_CLICKED,
       BUTTON1_DOUBLE_CLICKED, BUTTON1_TRIPLE_CLICKED

       Those repeat for BUTTON2 and BUTTON3.

       Note that not all terminals support mice, or may
       intercept certain events for their own use.

       You can check for mouse events via nc_getch():

       \code
       var key = nc_getch();
       if( ncurses.KEY_MOUSE == key ) {
           var evt = nc_getmouse();
           ...
       }
       \endcode
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_getmouse( ::v8::Arguments const & );


    V8_BINDING ::v8::Handle< ::v8::Value > nc_subpad( ::v8::Arguments const & );
    /**
       See also: nc_derwin(). This function and that one are very
       similar but subtly different.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_subwin( ::v8::Arguments const & );
    /**
       See also: nc_subwin(). This function and that one are very
       similar but subtly different.
    */

    V8_BINDING ::v8::Handle< ::v8::Value > nc_derwin( ::v8::Arguments const & );
    /**
       Like nc_waddstr(), but writes top-to-bottom instead of
       left-to-right.

       JS Usage:

       int nc_waddvstr( WINDOW, string )
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_waddvstr( ::v8::Arguments const & );
    /**
       Like nc_waddnstr(), but writes top-to-bottom instead of
       left-to-right.

       JS Usage:

       int nc_waddvnstr( WINDOW, string, count )
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_waddvnstr( ::v8::Arguments const & );
    /**
       Like nc_mvwaddstr(), but writes top-to-bottom instead of
       left-to-right.

       JS Usage:

       int nc_mvwaddvstr( WINDOW, y, x, string )
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mvwaddvstr( ::v8::Arguments const & );
    /**
       Like nc_mvwaddnstr(), but writes top-to-bottom instead of
       left-to-right.

       JS Usage:

       int nc_mvwaddvnstr( WINDOW, y, x, string, count )
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mvwaddvnstr( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_waddstr( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_waddnstr( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mvwaddnstr( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mvwaddstr( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wgetch( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wbkgd( ::v8::Arguments const & );

    V8_BINDING ::v8::Handle< ::v8::Value > nc_wclrtobot( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wclrtoeol( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wdelch( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wdeleteln( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wechochar( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_waddch( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mvwaddch( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wbkgdset( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_getbkgd( ::v8::Arguments const & );

    /** Read the native docs before using this. It's not
	quite as useful as it sounds, due to curses' lack of
	dynamic layout management.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wresize( ::v8::Arguments const & );
    /**
       Interactively reads a string from a window handle.

       JS signature:

       String nc_wgetstr(WINDOW)

       Returns null if wgetstr() fails and throws if the
       given object is-not-a WINDOW.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wgetstr( ::v8::Arguments const & );

    /**
       Sets the cursor position for a window handle then interactively
       reads a string from the window.

       JS signature:

       String nc_mvwgetstr(WINDOW,int y, int x)

       Returns null if mvwgetstr() fails and throws if the
       given object is-not-a WINDOW.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mvwgetstr( ::v8::Arguments const & );

    /**
       Like mvwgetstr(), but also takes an argument specifying
       the maximum length of the string to read.

       JS signature:

       String nc_mvwgetnstr(WINDOW,int y, int x, int maxLen)
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_mvwgetnstr( ::v8::Arguments const & );

    /**
       Checks whether the first argument is-a WINDOW handle. The
       optional second argument is a bool value - if it is true (the
       default) then "strict mode" is enabled, in which case this
       function returns true only if the argument is-a WINDOW and
       is-not-a PANEL or PAD. If strict mode is false then
       a PANEL or PAD is also considered to be-a WINDOW.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_is_window( ::v8::Arguments const & );

    /**
       Returns true if its first argument is-a PAD handle.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_is_pad( ::v8::Arguments const & );
    /**
       Returns true if its first argument is-a PANEL handle.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_is_panel( ::v8::Arguments const & );

    V8_BINDING ::v8::Handle< ::v8::Value > nc_wattroff( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wattron( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wattrset( ::v8::Arguments const & );
    /**
       Returns a subwindow's Y origin relative to its parent.
       
       JS Usage: var y = nc_getpary(window);
	   
       JS return value is an integer.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_getpary( ::v8::Arguments const & );
    /**
       Returns a subwindow's X origin relative to its parent.

       JS Usage: var x = nc_getparx(window);

       JS return value is an integer.
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_getparx( ::v8::Arguments const & );
    /**
       Returns a subwindow's Y/X origin relative to its parent.

       JS Usage: var coords = nc_getparyx(window);

       JS return value is an object: {y:AnInteger,x:AnInteger}
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_getparyx( ::v8::Arguments const & );

    /**
       This non-standard curses function returns the
       geometry of it's first argument, which must be
       a WINDOW.

       JS usage: var geom = nc_wgeometry(win);

       The object is in the form {x:INT,y:INT,w:INT,h:INT}
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_wgeometry( ::v8::Arguments const & );

    V8_BINDING ::v8::Handle< ::v8::Value > nc_update_panels( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_new_panel( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_del_panel( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_hide_panel( ::v8::Arguments const & );
    V8_BINDING ::v8::Handle< ::v8::Value > nc_panel_hidden( ::v8::Arguments const & );
    /**
       Returns true if the given key ID (an integer) is supported,
       else false. If compiled with some curses implementation other
       than Ncurses, it will throw. (That said, i haven't tried
       compiling this against a non-N curses implementation.)
    */
    V8_BINDING ::v8::Handle< ::v8::Value > nc_has_key( ::v8::Arguments const & );


    V8_BINDING v8::Handle<v8::Value> nc_bottom_panel(const v8::Arguments&);
    V8_BINDING v8::Handle<v8::Value> nc_top_panel(const v8::Arguments&);
    V8_BINDING v8::Handle<v8::Value> nc_show_panel(const v8::Arguments&);
    V8_BINDING v8::Handle<v8::Value> nc_panel_window(const v8::Arguments&);

    V8_BINDING v8::Handle<v8::Value> nc_move_panel(const v8::Arguments&);
    V8_BINDING v8::Handle<v8::Value> nc_panel_above(const v8::Arguments&);
    V8_BINDING v8::Handle<v8::Value> nc_panel_below(const v8::Arguments&);

    /**
       Redirects std::cout to a WINDOW.  You should call
       nc_capture_end(WINDOW) before calling this function with a
       different window.

       JS usage: void nc_capture_cout(WINDOW).
    */
    V8_BINDING v8::Handle<v8::Value> nc_capture_cout(const v8::Arguments&);

    /** Identical to nc_capture_cout(), but works on std::cerr. */
    V8_BINDING v8::Handle<v8::Value> nc_capture_cerr(const v8::Arguments&);

    /**
       Removes any cout/cerr capturing being done by the given window.
       See nc_capture_cout() and nc_capture_cerr().

       JS signature:

       int nc_capture_end(WINDOW)

       Returns the number of capture bindings which were released.
    */
    V8_BINDING v8::Handle<v8::Value> nc_capture_end(const v8::Arguments&);

    /**
       This function has special requirements - please see the library
       manual for instructions on how to use nc_ripoffline().

       JS Usage:

       int nc_ripoffline( 1 or -1, HandlerFunction )

       If the first argument is 1, a line is ripped from the top of
       the screen, and -1 means to rip from the bottom.

       HandlerFunction must have this signature:

       handlerFunction( WINDOW, int )

       HandlerFunction is called by initscr(). As in the native API,
       ripoffline() *must* be called *before* initscr(), or it will
       have no effect. It may be called up to 5 times. Each call to
       nc_ripoffline() may pass a separate handler function. Calling
       it more than 5 times will cause an error to be returned.

       Unlike in the native API, most of the nc_xxx() API is not
       available inside the handler function, because most of it
       enforces that stdscr is available, which is not guaranteed
       to be the case when the handler is called. In practice, your
       handler function should simply capture the window handles
       in an array (e.g. myHandlerFunction.lines), and then initialize
       them after initscr() has been called.
    */
    V8_BINDING v8::Handle<v8::Value> nc_ripoffline(const v8::Arguments&);

#undef V8_BINDING

}}} /* namespaces */

#endif /* CODE_GOOGLE_COM_P_V8_V8_P3_NCURSES_H_INCLUDED */
