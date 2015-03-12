

# About #

The ncurses bindings wrap up a great deal (well over 100 functions) of
the ncurses API for use in JavaScript. Instead of providing an API
directly compatible with the low-level C API (as earlier versions of
this plugin did), it provides an object-oriented interface. This is
much easier to manage, in terms of window ownership, subwindows, etc.,
than the low-level interface. That said, the OO interface is only a
simple wrapper around the curses API (essentially adding only child
ownership semantics), and will require some curses know-how in order
to use effectively. As always, curses "takes some getting used to."

The advantages of writing curses apps in JS instead of C include:

  * MUCH faster development of curses UIs, as the whole compile/link cycle goes away.
  * The event model of curses (keypress) ties in very well with JavaScript, where it is trivial to bind the keypresses to event handlers.

Unfortunately, no amount of wrapping will make the overall difficulties of
developing with curses completely disappear.

TODOs:
  * lots more testing, in particular with the PANEL/PAD APIs.
  * Utility routines/classes, e.g. dialog boxes, basic user input routines, scrollable areas, etc.

ncurses features which i have no plans to implement include:
  * The FORMS and MENU sub-libraries will not be implemented.
  * Terminal resize handling. That's a can of worms i don't want to open.
  * The wide-char ncurses API.

## Source Code ##

http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/ncurses-oo

## History ##

Curses has been around since the 80's and is the _de facto_ library
for implementing text-base user interfaces. It presumably gets its
name from the fact that one spends half his time cursing while using
it. _ncurses_ is the _de facto_ open source implementation of curses
(curses is actually a standard), and is available on almost every
platform out there.

This plugin is derived from an old ncurses C++ wrapper which i forked
out of the ncurses 5.4 source tree in March of 2005. At one point it
was also the basis for JavaScript/ncurses binding for SpiderMonkey.

The first version of this plugin modelled the C-style API. After it was
implemented, i realized that the particular method of JS/C++ wrapping which it
used was not only type-unsafe on the native side, but also easily subject to
segfaults from script-side code. Aside from that, it also got seriously broken
in some refactoring of v8-juice. Rather than fix/refactor it, i decided an OO
wrapper would be more useful, and revived the ancient ncurses C++ wrapper for use
as a basis.

For new and old curses programmers alike,
[Dan Gookin's Programmer's Guide to nCurses](http://www.c-for-dummies.com/ncurses/)
is an excellent resource on how to work with the library. Printed in
2007, it is the only curses book i'm aware of which was penned in the
21st century. Curiously, even though Thomas Dickey still releases patches to ncurses
on a weekly basis, not much has fundamentally changed in curses since... well, since a very
long time. So if you programmed curses back in 1992 and still remember how it works,
there won't be much of a learning curve for you here.

# API Overview #

This plugin installs the following items:

  * The generic `ncurses` object holds all classes, shared functions, and shared symbolic constants (e.g. the curses macros like `A_BOLD`).
  * The `NCWindow` approximates an ncurses `WINDOW` handle.
  * The `NCPanel` approximates an ncurses `PANEL` handle.
  * The `NCPad` somewhat approximates an ncurses `PAD` handle.
  * The `NCFramedPad` is an `NCPad` subclass which displays scrollbars.

## Naming Conventions ##

Curses was developed back in a time when it was cool to use the shortest, most
ambiguous or cryptic names possible. For example, `refresh()` by itself could
mean anything, and `scrl()` and `scroll()` are both scrolling related, but without
looking in the man pages i can't tell you which one does what.

In order to make this API easier to use... for the vast majority of the cases
we've kept the cryptic names! Why? Because it makes it simpler to go look up
their meanings in the curses man pages. If we renamed them, curses gurus would
be confused by the new names and curses newbies wouldn't be able to find the
man pages. The functions which don't use names taken directly from the curses
API normally derive their names from the underlying C++ wrapper (which was
originally developed by someone else). An example which comes to mind is
`NCWindow.scroll()`, which behaves like `wscrl()`, and not like `scroll()`.

## Error Reporting ##

Most error codes used by curses are integers with the value `ERR` for failure,
and non-ERR on success (honestly, that's what the curses standard specifies).
In practice, non-ERR is simply `OK`, which is 0.

Many errors are reported via exceptions, either because they come from the
JS side of the bindings (as opposed to being an ncurses error value) or because
they inherit that behaviour from the underlying C++ wrapper (which throws for
a number of error cases, most notably for failed window construction).

# The `ncurses` Object #

This plugin installs an object (of an unspecified class) called `ncurses`
which holds all of the plugin's classes and the following non-class functions:

| **Return type** | **Name/Arguments** | **Notes** |
|:----------------|:-------------------|:----------|
| `int` | `color_pair(string fg, string bg)` | Takes foreground/background color names (can insensitive) and return the equivalent curses color attributes. Color names are: black, red, green, yellow, blue, magenta, cyan, white. They can normally be OR'd together with `ncurses.A_BOLD` to get a brighter foreground color. |
| `int` | `color_pairnum(string fg, string bg)` | Like `color_pair()`, but returns a curses _color pair_ number, which is only very rarely useful or needed. |
| `int` | `KEY_F(int)` | Equivalent to the `KEY_F()` native macro. |
| `int` | `KEY_CTRL(int | char)` | Returns the ASCII value of the given integer or character (length-1 string), bitwise ORed with 0x1f, or undefined if the value is not in the inclusive integer or ASCII range 0-127. |
| `int` | `intVal( string )` | Returns the ASCII integer value of the first character in the given string |
| `int` | `charVal(int)` | If the given int is in the inclusive range of 0-127, it returns the ASCII character (as a string), otherwise it returns undefined. |
| `int` | `screenHeight()` | Height of the terminal, in text rows. Not valid until in curses mode. |
| `int` | `screenWidth()` | Width of the terminal, in text columns. Not valid until in curses mode. |
| `int` | `beep()` | Beeps one time. |
| `int` | `flash()` | Flashes the terminal. |
| `int` | `getch()` | Similar, but not identical, to `NCWindow.getch()`. In theory they are indentical, but in practice the latter has no refresh side-effects, whereas the former sometimes does. |
| `int` | `curs_set(int)` | Sets the cursor to invisible, half-visible, or fully visible, depending on whether the argument is 0, 1, or 2, respectively. This has no effect if called before curses screen mode is active. |
| `int` | `napms(int)` | Sleeps for the given number of milliseconds. |
| `int` | `endwin()` | Never call unless absolutely necessary - forcibly exits curses screen mode without cleaning up resources. If all windows are closed properly, curses will be shut down automatically when the last window goes away. |
| `int` | `echo([bool = true])` | If passed true, tells curses to echo all keypresses, otherwise keypresses are not echoed. This is off by default but should be turned on when using `NCWindow.getstr()` (or similar). |
| `int` | `noecho()` | Equivalent to `echo(false)`. |
| `int` | `cbreak([bool = true])` | See `man cbreak`. |
| `int` | `nocbreak()` | Equivalent to `cbreak(false)`. |
| `int` | `raw([bool = true])` | See `man raw`. |
| `int` | `noraw()` | Equivalent to `raw(false)`. |
| `int` | `halfdelay(int tenths)` | See `man halfdelay`. |
| `Array` | `colorNames()` | Returns a list of the 8 basic color names, in an unspecified order. |
| `void` | `ripoffline(bool fromTop)` | EXPERIMENTAL. Described below. |
| `int` | `popupDialog(title,text[,rows,cols,y,x])` | Pops up a _very basic_ dialog with the given title and text (it will try to guess a reasonably position and size, but you may need to help it). It waits for a single keypress, then destroys the dialog and returns that pressed key's value. |


Aside from those functions, nearly all standard ncurses macros are defined as
symbolic constants of the `ncurses` object. For example, `ncurses.A_BOLD` and
`ncurses.A_UNDERLINE`.

## `ripoffline()` ##

ACHTUNG: EXPERIMENTAL and it might go away. Known to cause segfaults
post-main() in some case, or cause an app to not exit curses mode at
shutdown (hosing the console state). It has also been observed to
interfere with `NCPad` scrolling operations (for reasons i cannot
comprehend).

`ripoffline()` allows one to "rip off" lines from the screen. These
lines are outside of the `stdscr` area (and in fact cause `stdscr` to
be smaller) and have several important restrictions described below and
in the man pages for `ripoffline()` (though the native function has a slightly
different signature).

`ripoffline()`, if used at all, _must_ be called _before_ any window
objects are created (more specifically, before curses mode is ever
started). It can only rip one line at a time, and can only be called
up to 5 times (these are limitations of curses). Each time it is
called, it rips one line from the top or bottom of the screen,
depending on the boolean parameter passed to it. Each line becomes a
new window object, which is accessed via the roundabout approach
shown below.

```
load_plugin('v8-juice-ncurses-oo');
var nc = ncurses;
nc.ripoffline(true); // rip from top
nc.ripoffline(false); // rip from bottom

/** We MUST (it is a curses requirement) initialize
curses before we can access the ripped-off lines. That is done by creating
our first window object: */
var root = new nc.NCPanel();

// Now we can access the ripped-off lines like:
function setupRips()
{
    var cl = ['white','red','white','black','cyan'];
    for( var i = 0; i < 5; ++i ) // 5 is a built-in limit of curses
    {
        var rw = nc.getRippedLine(i);
        if( ! rw ) break;
        rw.bkgd( nc.color_pair( cl[i], cl[i+1] ) );
        rw.addstr("ripped line #"+i+" ="+rw);
        rw.refresh();
    }
}

setupRips();
```

When using `ripoffline()`, if you find that your ncurses apps are not returning to
normal screen mode when the last window is destroyed, try calling `ncwindow.endwin()`
directly after destroying the last window. That will forcibly shut down curses mode
and destroy those dangling ripped-off lines. Calling `endwin()` while other windows
are still open can lead to a memory leak, and is not recommended.


# NCWindow Class #

NCWindow is the base class of most other classes in this package. It is a wrapper around
an ncurses `WINDOW` handle, and provides most functions necessary for working with curses
windows.

## Member Function List ##

Most of the functions listed below are not documented here. Most of them map
1-to-1 with ncurses functions of the same (or very similar) names, and are
described in great detail in the ncurses man pages. That also explains why
their names are so cryptic and do not follow common JavaScript naming
conventions. The exceptions to the documentation rule are functions which are
unique to this API or, for reasons of argument passing conventions, work
differently than in the C API.

Some functions which are overloaded in the native C++ API instead have several
variants with slightly different names (all conforming to common curses naming
conventions). e.g. `getch()` vs `mvgetch()`. At some point i may go back and
implement proper overloads for JS, but for the moment it's more trouble than
it's worth.

Here we go...

### Constructors ###

| `NCWindow` | `new NCWindow()` | A proxy for the ncurses `stdscr` object. |
|:-----------|:-----------------|:-----------------------------------------|
| `NCWindow` | `new NCWindow(NCWindow [, bool box=true])` | Creates a subwindow of the given window, completely wrapping it with a 1-line/col border. If box is true, the border is decorated. Throws on error. |
| `NCWindow` | `new NCWindow(int lines, int cols, int y, int x)` | Creates a new wind with the given size/position. Throws on error. |
| `NCWindow` | `new NCWindow(NCWindow parent, int lines, int cols, int y, int x)` | Creates a subwindow of the given window, with coordinates relative to that window. |
| `NCWindow` | `new NCWindow(Array)` | Special-case variant for use with JS-side subclasses. Expands the array and acts as one of the other ctors. e.g. `new ncurses.NCWindow([1,2,3,4])` and `new ncurses.NCWindow(1,2,3,4)` are semantically the same. |

### Geometry, Cursor, and Movement ###

| **Return type** | **Name/Arguments** | **Notes** |
|:----------------|:-------------------|:----------|
| `int` | `move( int y, int x )` | Moves the _cursor_ (not the _window_) to the given position. |
| `int` | `mvwin( int y, int x )` | Moves the window. Do not do this if the window has a parent, or Grief may ensue. This is safe for panels, but probably not much else. |
| `int` |  `lines()` | The number of lines in the SCREEN, not the window. |
| `int` |  `cols()` | The number of columns in the SCREEN, not the window. |
| `int` |  `height()` | The height of the window, in lines. |
| `int` |  `width()` | The width of the window, in columns. |
| `int` |  `begy()` | See `man getbegyx`. Top coordinate line, relative to the screen(?). |
| `int` |  `begx()` | See `man getbegyx`. Left coordinate edge, relative to the screen(?). |
| `int` |  `maxx()` | See `man getmaxx`. |
| `int` |  `maxy()` | See `man getmaxy`. |

### Colors, Status, and Attributes ###

| **Return type** | **Name/Arguments** | **Notes** |
|:----------------|:-------------------|:----------|
| `int` |  `tabsize()` | ??? |
| `int` |  `inch()` | See `man winch`. |
| `int` | `mvinch( int, int )` | See `man mvwinch`. |
| `int` | `attron(int)` | See `man wattron`. |
| `int` | `attroff(int)` | See `man wattroff`. |
| `int` | `attrset(int)` | See `man wattrset`. |
| `int` | `color_set(int)` | Don't use this. Use `ncurses.color_pair()` instead - much easier. |
| `int` | `bkgd(int)` | Immediately changes the window's whole background to the given attributes. |
| `void` | `bkgdset(int)` | Similar to `bkgd()`, but does not apply to the whole window. The man pages are seriously lacking on this one. |
| `int` |  `getbkgd()` | Fetches the background attributes. |
| `bool` |  `is_wintouched()` | See `man is_wintouched`. |
| `bool` | `is_linetouched(int)` | See `man is_linetouched`. |
| `int` | `setscrreg( int, int )` | See `man wsetscrreg`. |
| `int` | `touchln( int start, int count )` | See `man wtouchln`. |
| `int` |  `touchwin()` | See `man touchwin`. |
| `int` |  `untouchwin()` | See `man untouchwin`. |
| `int` | `scrollok(bool)` | See `man scrollok`. |
| `int` | `idlok(bool)` | See `man idlok`. |
| `void` | `idcok(bool)` | See `man idcok`. |
| `int` | `leaveok(bool)` | See `man leaveok`. |
| `void` | `immedok(bool)` | See `man immedok`. |
| `int` | `keypad(bool)` | See `man keypad`. |
| `int` | `clearok(bool)` | See `man clearok`. |
| `int` | `nodelay(bool)` | Analog to curses `nodelay()` |
| `void` | `timeout(int)` | Analog to curses `wtimeout()`. |
| `bool` |  `has_mouse()` | Returns true if curses is compiled with mouse support. TODO: move this to a static member. |

### Drawing/Output ###

| **Return type** | **Name/Arguments** | **Notes** |
|:----------------|:-------------------|:----------|
| `int` |  `erase()` | See `man werase`. |
| `int` |  `clear()` | See `man wclear`. |
| `int` |  `clrtobot()` | See `man wclrtobot`. |
| `int` |  `delch()` | See `man wdelch`. |
| `int` |  `deleteln()` | See `man wdeleteln`. |
| `int` |  `redrawwin()` | See `man redrawwin`. |
| `int` |  `doupdate()` | See `man doupdate`. |
| `void` |  `syncdown()` | See `man wsyncdown`. |
| `void` |  `syncup()` | See `man wsyncup`. |
| `void` |  `cursyncup()` | See `man wcursyncup`. |
| `int` |  `refresh()` | See `man wrefresh`. |
| `int` |  `noutrefresh()` | See `man wnoutrefresh`. |
| `int` |  `standout()` | See `man wstandout`. |
| `int` |  `standend()` | See `man wstandend`. |
| `int` | `insch(int)` | See `man winsch`. |
| `int` | `insdelln(int)` | See `man winsdelln`. |
| `int` | `insstr(string)` | See `man winsstr`. |
| `int` | `insstrn( string, int )` | See `man winsstr`. |
| `int` | `mvinsstr( int,int,string )` | See `man mvwinsstr`. |
| `int` | `mvinsstrn( int,int,string,int )` | See `man mvwinsstr`. |
| `int` | `addch(int)` | See `man addch`. |
| `int` | `echochar(int)` | See `man wechochar`. |
| `int` | `addstrn( string, int n )` | Adds up to n bytes of the given string to the window. |
| `int` | `addstr(string)` | Adds the given string to the window. |
| `int` | `mvaddstr( int, int, string )` | See `man mvaddstr`. |
| `int` | `mvaddstrn( int, int, string,int maxLen )` | See `man mvaddstr`. |
| `int` | `mvdelch( int, int )` | See `man mvwdelch`. |
| `int` | `redrawln( int, int )` | See `man wredrawln`. |
| `int` | `mvaddch( int,int,int )` | See `man mvwaddch`. |
| `int` | `mvinsch( int,int,int )` | See `man mvwinsch`. |
| `int` | `box([int vchar=0, int hchar=0])` | Draws a box around the window. |
| `int` | `border([eight optional arguments])` | Analog to curses `wborder()`. |
| `int` | `overlay(NCWindow)` | See `man overlay`. |
| `int` | `overwrite(NCWindow)` | See `man overwrite`. |
| `int` | `copywin(NCWindow,int,int,int,int,int,int,bool)` | See `man copywin`. |
| `int` | `scroll(int)` | See `man scroll`. |
| `int` |  `insertln()` | See `man winsertln`. |

### User Input ###

| **Return type** | **Name/Arguments** | **Notes** |
|:----------------|:-------------------|:----------|
| `int` | `getch()` | Waits for a keypress and returns its key code. |
| `int` | `mvgetch( int, int )` | Moves to the given y/x position, then acts as `getch()`. |
| `string` | `getstr([int maxlen])`, `getstr(int y, int x [, int maxlen])` | Reads up to maxlen (or some arbitrarily chosen limit) characters from the window. |

### Misc. ###

| **Return type** | **Name/Arguments** | **Notes** |
|:----------------|:-------------------|:----------|
| `void` | `close()` | Closes/destroys this windows. The object cannot be used after this is called. |
| `string` |  `name()` | Returns the window's name. |
| `void` | `setName(string)` | Sets the window's name. |
| `int` | `windowID()` | Returns a unique integer ID for this window. No two windows have the same ID. It can be used to assist in mapping windows to arbitrary other data. |
| `void` | `captureCout([int attributes])` | Redirects the `std::cout` object to this window until the window is closed or `captureReset()` is called. If the attributes argument is set, the output from that stream is colored using those attributes. |
| `void` | `captureCerr()` | Same as `captureCout()`, but applies to the native `std::cerr` object. |
| `void` | `captureReset()` | Undoes stream capturing enabled with `captureCout()` and `captureCerr()`, reverting `std::cout` and/or `std::cerr` to their previous states. This automatically happens when `close()` is called. |


## TODOs ##

TODOs:
  * Sort the member function lists, at least partially (keeping like-named "overloads" together).
  * Maybe provide a common API for dispatching `getch()` key codes via JS callbacks.


# NCPanel Class #

NCPanel is analogous to the curses `PANEL` type. It is basically a top-level window widget
which can be stacked with other panels and moved around (both of which are problematic
with non-panel windows).

## Member Function List ##

In addition to the NCWindow API, NCPanel has:

| **Return type** | **Name/Arguments** | **Notes** |
|:----------------|:-------------------|:----------|
| `NCPanel` | `new NCPanel()` | A top-level panel. |
| `NCPanel` | `new NCPanel(int lines, int cols [, int y=0, int x=0])` | Creates a new panel at the given position. A window can be wrapped around it with `new NCWindow(myPanel)`. Throws on error. |
| `NCPanel` | `new NCPanel(Array)` | See `NCWindow(Array)` - this acts the same way. |
| `void` | `hide()` | Hides the panel. |
| `void` | `show()` | Shows the panel. |
| `void` | `top()` | Moves the panel to the top of the stack. |
| `void` | `bottom()` | Moves the panel to the bottom of the stack. |
| `bool` | `hidden()` | Returns true if the panel is hidden. |
| `int` | `mvwin(int y, int x)` | Moves the panel |
| `void` | `interactivelyMove()` | When called, the panel can be moved using the arrow keys. Movement stops when a non-movement key is pressed or the panel would be moved out of the screen bounds. |

Additionally, some methods are overridden at the native level to
accommodate for the slightly differing update/refresh conventions
between windows and panels.

# NCPad and NCFramedPad Classes #

NCPad is a NCWindow subclass wrapping the very unusual ncurses PAD
type.  Pads are windows which may be larger than the screen (normal
curses windows cannot be out of the screen bounds). They have some
very intricate refreshing rules and are difficult to use properly (at
least in my experience). They are, however, very useful in creating
scrollable areas.

The NCFramedPad class is identical to the NCPad class in every way, including
the constructor arguments, with the exception that it will draw a border around
its scrollable area and will draw scroll indicators along the right and bottom
of the scrollable area.

Whether to use NCPad or NCFramedPad for a particular case is largely a
matter of UI design tastes.

## Known bugs ##

  * The native NCPad implementation is known to have problems drawing itself if its viewport smaller than the containing window. If your pad appears blank after drawing to it and refreshing it, try making it bigger as a workaround.
  * Pads aren't known for their user friendliness.

## Member Function List ##

NCPad deserves much more documentation than is given here. Maybe someday. Until then...

Aside from the NCWindow API (not all of which is strictly legal for a pad -
see the curses man pages!), NCPad also provides:


| **Return type** | **Name/Arguments** | **Notes** |
|:----------------|:-------------------|:----------|
| `NCPad` | `new NCPad(NCWindow windowToWrap, int lines, int cols)` | Creates a new pad of the given size (which may be bigger than the screen). The given window is used as the drawing surface. |
| `NCFramedPad` | `new NCFramedPad(NCWindow windowToWrap, int lines, int cols)` | Identical to the `NCPad` ctor, except that the returned object is rendered with a border and scroll indicators. |
| `NCPad` | `new NCPad(Array)` | See `NCWindow(Array)` - this acts the same way. |
| `NCFramedPad` | `new NCFramedPad(Array)` | See `NCWindow(Array)` - this acts the same way. |
| `int` |`requestOp(int)` | Detailed in the native API. |
| `int` | `prefresh(six integers)` | Same as ncurses `prefresh()`. |
| `int` | `pnoutrefresh(six integers)` | Same as ncurses `pnoutrefresh()` |
| `int` | `lineCount()` | Number of lines in the pad. |
| `int` | `columnCount()` | Number of columns in the pad. |
| `bool` | `consumeKey(int)` | Tries to process a keypress, returning true if it can. See the native API. |
| `void` | `mapKeyToReq(int key, int requestID)` | See the list of `PadReq` vars below. This function maps key IDs to "pad requests", such that `inputLoop()` will react to them. |
| `int` | `reqForKey(int key)` | Returns a `PadReq` value (see below) corresponding to the given key. |
| `int` | `echochar(int ch)` | Equivalent to `pechochar`. |
| `void` | `inputLoop()` | Starts a local input loop. Pressing the arrow/page keys scrolls the pad and Ctrl-W or F10 stops the loop. |
| `int` | `refresh()` | Reimplemented to take no args (as for NCWindow) or 6 args (equivalent to `prefresh()`) |
| `int` | `noutrefresh()` | Reimplemented to accept no args (as for NCWindow) or 6 args (equivalent to `pnoutrefresh()`) |

The NCPad class also has the following shared symbolic contants, all
of which are numeric and all of which are accepted as arguments to the
`requestOp()` and `mapKeyToReq()` member functions. The native API
(documented in
[ncwindow.hpp](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/ncurses-oo/ncwindow.hpp))
goes into much more detail about these.

| **Name** | **Notes** |
|:---------|:----------|
| `PadReqFirstRequest` | First request type in the list, for use in loops. |
| `PadReqRefresh` | Triggers a refresh operation, which has somewhat "special" internals for a pad. |
| `PadReqUp` | Scroll one line up. |
| `PadReqDown` | Scroll one line down. |
| `PadReqLeft` | Scroll left. |
| `PadReqRight` | Scroll right. |
| `PadReqExit` | End input loop. |
| `PadReqPageUp` | Page up. |
| `PadReqPageDown` | Page down. |
| `PadReqScrollToTop` | Scrolls to the top. |
| `PadReqScrollToBottom` | Scrolls to the bottom. |
| `PadReqIgnored` | Used by some routines to report that a key was explicitly ignored. |
| `PadReqUnknown` | Used by `reqForKey()` to report an unknown key mapping. |
| `PadReqLastRequest` | Last request type in the list, for use in loops. |

TODOs:
  * The ability to map keys/`PadReq`s to a function might be interesting.

# Tips and Tricks #

General tips include:

  * Remember that ncurses uses backwards coordinates: (y,x) instead of (x,y)
  * Never use stdscr (unless it is your _only_ window), and don't use overlapping windows (ever). Both cases are very problematic to redraw correctly. Use NCPanel objects to partition off your main working space, and use non-overlapping sub-windows within the panels to further organize the UI.
  * NCPad is weird. Just plain weird. It takes lots of study to get it to do what you want it to.
  * The `NCWindow.captureCout()` and `captureCerr()` methods should always be matched with `captureReset()`, or else "good luck!" Calling `close()` on a window will automatically undo any capturing for that window, revering it to its previous state.
  * If your curses script dies with a message like `object X has no function Y` it is easy to miss the error text unless one adds a try/catch block around the main app and sends it to stdout after curses mode is ended (which happens automatically when all windows are destroyed).

## Send debugging output to another console ##

Any output to the console from non-curses methods (e.g. `print()`) may
hose the screen state. If you want to send debugging output to a separate
console, open that console and type `tty`. That will give a device name,
like `/dev/pts/4`. From your terminal where you're running ncurses
code, you can then do:

```
~> v8-juice-shell myScript.js 2>/dev/pts/4 # adjust that value, of course!
```

and the output which is intended for `std::cerr` will then go to that console. Do not
redirect stdout to another console, or curses will try to write to that
console (but won't work properly)!

Alternately, you can use `NCWindow.captureCerr()`, but that will only
work if all client debugging code uses the `std::cerr` object for
debug output (as opposed to using `fprintf(stderr,...)` or similar). When
using both `captureCerr()` and `captureCout()` on the same window it is
sometimes useful to pass color attributes to one or both of the streams
so that their output looks different:

```
mywin.captureCout(); // cout will use mywin's current colors
mywin.captureCerr( ncurses.color_pair('red','black') | ncurses.A_BOLD );
...
mywin.captureReset(); // restores cout/cerr to their previous states
```

Note that JS has no standard (or even pseudostandard routines) which output to
stderr, and it is generally expected that all intercepted `std::cerr` output
will be coming from debuggering routines.

Newer versions of the v8-juice shell application (under `src/client/shell/`)
have the command-line option `--print-cerr`, which forces the JS `print()` function
to use `std::cerr`, instead of `std::cout`, for output. Combined with redirection
to another console, this is often useful when debugging [curses applications](PluginNCurses.md).

# Inheriting Window Types from JS #

To summarize a very long story...

If you want to inherit any of the window classes from JS classes, follow
the instructions [on the ClassBinder page](ClassBinder#Inheriting_Native_Classes_from_JS.md).
Here is a shortened summary of what a subclass looks like:

```
/** Custon NCPanel subclass: */
function MyPanel()
{
    var argv = Array.prototype.slice.apply(arguments,[0]);
    // ^^^ convert 'arguments' pseudoarray to a real array
    // for use with:
    this.prototype = this.__proto__ = new ncurses.NCPanel(argv);
    // (this instanceof ncurses.NCPanel) is now true!
    ... add members, etc. ...

   // To call an inherited implementation of a member:
   this.toString = function() {
        return '[MyPanel:'+this.__proto__.toString()+']';
        // ^^^ this.prototype.toString() won't work for this purpose!
        // so use __proto__ inside of member functions instead.
    };
    return this;
}

var p = new MyPanel(10,10,4,4);
p.bkgd(ncurses.color_pair('white','red'));
...
p.close();
```


# Examples #

It's hard to make _short_/_concise_ examples for curses. i'll try to get
some written up at some point. Until then...

## NCPanel/NCPad Example ##

Most of my curses UIs use panels as the primary UI widgets, and then
i add the app-specific widgets to those. i never touch stdscr, and instead
do everything on panels. This saves a lot of hassle when it comes to moving
windows and refreshing overlapping windows. For example, using a screen-sized
panel as the "root window" overcomes screen corruption problems which we
see when we have panels on top of stdscr _and_ use stdscr for output. Summary:
don't output anything to stdscr unless it's the only window your app needs.

Here's an example:

```
load_plugin('v8-juice-ncurses-oo');
var nc = ncurses;
nc.curs_set(0);
var root = new nc.NCPanel(); // full-screen panel.
root.attrset( nc.color_pair('white','red') );
root.mvaddstr(4,0,"I am the root panel.");
root.captureCout();
root.captureCerr( nc.color_pair('red','white') | ncurses.A_BLINK | ncurses.A_BOLD );
print("std::cout is redirected here.");
function tryPad()
{
    try
    {
        var pnl = new nc.NCPanel(10,30,3,3);
        print('new child =',pnl.name());
        pnl.scrollok(true);
        pnl.bkgd( nc.intVal(' ') | nc.color_pair('white','blue') );

        var pad = new nc.NCPad(pnl,100,100);
        print("Wrapper pad added.");
        pad.bkgd( nc.intVal(' ') | nc.color_pair('blue','white') | ncurses.A_BIM );
        pad.mapKeyToReq( ncurses.KEY_CTRL('G'), ncurses.NCPad.PadReqExit );
        pad.scrollok(true);
        cn = pad.name();
        for( var i = 0; i < 5; ++i )
        {
            pad.addstr("#"+i+": This is window "+cn+"\n");
        }
        pad.addstr("Going into input loop.\nTry scrolling.\nTap Ctrl-G to stop.\n");
        pnl.refresh();
        pad.inputLoop();
        pad.addstr("Input loop finished.");
        pad.refresh();
        print("Now tap a key to destroy these objects.");
        ncurses.beep();
        pnl.getch();
        pad.close();
        pnl.close();
    }
    catch(e)
    {
        for( var i = 0; i < 5; ++i ) ncurses.beep();
        print("exception:",e);
    }
}
tryPad();
root.refresh();
print("All done. Tap a key to quit.");
ncurses.beep();
var rc = root.getch();
root.close();
print("Done! :-D");
```