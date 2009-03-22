// * This makes emacs happy -*-Mode: C++;-*-
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
by stephan@s11n.net, starting 1 March 2005.
************************************************************************/


/* SCO 3.2v4 curses.h includes term.h, which defines lines as a macro.
   Undefine it here, because NCWindow uses lines as a method.  */
#undef lines

/* "Convert" macros to inlines. We'll define it as another symbol to avoid
 * conflict with library symbols.
 */
#undef RENAMEFUNC
#define RENAMEFUNC(name) NCCUR_ ##name

#ifdef addch
inline int RENAMEFUNC(addch)(chtype ch)  { return addch(ch); }
#undef addch
#define addch RENAMEFUNC(addch)
#endif

#ifdef echochar
inline int RENAMEFUNC(echochar)(chtype ch)  { return echochar(ch); }
#undef echochar
#define echochar RENAMEFUNC(echochar)
#endif

#ifdef insdelln
inline int RENAMEFUNC(insdelln)(int n)  { return insdelln(n); }
#undef insdelln
#define insdelln RENAMEFUNC(insdelln)
#endif

#ifdef addstr
inline int RENAMEFUNC(addstr)(const char * str)  { return addstr(str); }
#undef addstr
#define addstr RENAMEFUNC(addstr)
#endif

#ifdef attron
inline int RENAMEFUNC(attron)(chtype at) { return attron(at); }
#undef attron
#define attron RENAMEFUNC(attron)
#endif

#ifdef attroff
inline int RENAMEFUNC(attroff)(chtype at) { return attroff(at); }
#undef attroff
#define attroff RENAMEFUNC(attroff)
#endif

#ifdef attrset
inline chtype RENAMEFUNC(attrset)(chtype at) { return attrset(at); }
#undef attrset
#define attrset RENAMEFUNC(attrset)
#endif

#ifdef color_set
inline chtype RENAMEFUNC(color_set)(short p, void* opts) { return color_set(p, opts); }
#undef color_set
#define color_set RENAMEFUNC(color_set)
#endif

#ifdef border
inline int RENAMEFUNC(border)(chtype ls, chtype rs, chtype ts, chtype bs, chtype tl, chtype tr, chtype bl, chtype br)
{ return border(ls, rs, ts, bs, tl, tr, bl, br); }
#undef border
#define border RENAMEFUNC(border)
#endif

#ifdef box
inline int RENAMEFUNC(box)(WINDOW *win, int v, int h) { return box(win, v, h); }
#undef box
#define box RENAMEFUNC(box)
#endif

#ifdef mvwhline
inline int RENAMEFUNC(mvwhline)(WINDOW *win, int y, int x, chtype c, int n) {
	return mvwhline(win, y, x, c, n); }
#undef mvwhline
#define mvwhline RENAMEFUNC(mvwhline)
#endif

#ifdef mvwvline
inline int RENAMEFUNC(mvwvline)(WINDOW *win, int y, int x, chtype c, int n) {
	return mvwvline(win, y, x, c, n); }
#undef mvwvline
#define mvwvline RENAMEFUNC(mvwvline)
#endif

#ifdef clear
inline int RENAMEFUNC(clear)()  { return clear(); }
#undef clear
#define clear RENAMEFUNC(clear)
#endif

#ifdef clearok
inline int RENAMEFUNC(clearok)(WINDOW* win, bool bf)  { return clearok(win, bf); }
#undef clearok
#define clearok RENAMEFUNC(clearok)
#else
extern "C"  int NCURSES_API clearok(WINDOW*, bool);
#endif

#ifdef clrtobot
inline int RENAMEFUNC(clrtobot)()  { return clrtobot(); }
#undef clrtobot
#define clrtobot RENAMEFUNC(clrtobot)
#endif

#ifdef clrtoeol
inline int RENAMEFUNC(clrtoeol)()  { return clrtoeol(); }
#undef clrtoeol
#define clrtoeol RENAMEFUNC(clrtoeol)
#endif

#ifdef delch
inline int RENAMEFUNC(delch)()  { return delch(); }
#undef delch
#define delch RENAMEFUNC(delch)
#endif

#ifdef deleteln
inline int RENAMEFUNC(deleteln)()  { return deleteln(); }
#undef deleteln
#define deleteln RENAMEFUNC(deleteln)
#endif

#ifdef erase
inline int RENAMEFUNC(erase)()  { return erase(); }
#undef erase
#define erase RENAMEFUNC(erase)
#endif

#ifdef flushok
inline int RENAMEFUNC(flushok)(WINDOW* _win, bool _bf)  {
	return flushok(_win, _bf); }
#undef flushok
#define flushok RENAMEFUNC(flushok)
#else
#define _no_flushok
#endif

#ifdef getch
inline int RENAMEFUNC(getch)()  { return getch(); }
#undef getch
#define getch RENAMEFUNC(getch)
#endif

#ifdef getstr
inline int RENAMEFUNC(getstr)(char *_str)  { return getstr(_str); }
#undef getstr
#define getstr RENAMEFUNC(getstr)
#endif

#ifdef instr
inline int RENAMEFUNC(instr)(char *_str)  { return instr(_str); }
#undef instr
#define instr RENAMEFUNC(instr)
#endif

#ifdef innstr
inline int RENAMEFUNC(innstr)(char *_str, int n)  { return innstr(_str, n); }
#undef innstr
#define innstr RENAMEFUNC(innstr)
#endif

#ifdef mvwinnstr
inline int RENAMEFUNC(mvwinnstr)(WINDOW *win, int y, int x, char *_str, int n) {
	return mvwinnstr(win, y, x, _str, n); }
#undef mvwinnstr
#define mvwinnstr RENAMEFUNC(mvwinnstr)
#endif

#ifdef mvinnstr
inline int RENAMEFUNC(mvinnstr)(int y, int x, char *_str, int n) {
	return mvinnstr(y, x, _str, n); }
#undef mvinnstr
#define mvinnstr RENAMEFUNC(mvinnstr)
#endif

#ifdef winsstr
inline int RENAMEFUNC(winsstr)(WINDOW *w, const char *_str)  {
	return winsstr(w, _str); }
#undef winsstr
#define winsstr RENAMEFUNC(winsstr)
#endif

#ifdef mvwinsstr
inline int RENAMEFUNC(mvwinsstr)(WINDOW *w, int y, int x,  const char *_str)  {
	return mvwinsstr(w, y, x, _str); }
#undef mvwinsstr
#define mvwinsstr RENAMEFUNC(mvwinsstr)
#endif

#ifdef insstr
inline int RENAMEFUNC(insstr)(const char *_str)  {
	return insstr(_str); }
#undef insstr
#define insstr RENAMEFUNC(insstr)
#endif

#ifdef mvinsstr
inline int RENAMEFUNC(mvinsstr)(int y, int x, const char *_str)  {
	return mvinsstr(y, x, _str); }
#undef mvinsstr
#define mvinsstr RENAMEFUNC(mvinsstr)
#endif

#ifdef insnstr
inline int RENAMEFUNC(insnstr)(const char *_str, int n)  {
	return insnstr(_str, n); }
#undef insnstr
#define insnstr RENAMEFUNC(insnstr)
#endif

#ifdef mvwinsnstr
inline int RENAMEFUNC(mvwinsnstr)(WINDOW *w, int y, int x, const char *_str, int n) {
	return mvwinsnstr(w, y, x, _str, n); }
#undef mvwinsnstr
#define mvwinsnstr RENAMEFUNC(mvwinsnstr)
#endif

#ifdef mvinsnstr
inline int RENAMEFUNC(mvinsnstr)(int y, int x, const char *_str, int n) {
	return mvinsnstr(y, x, _str, n); }
#undef mvinsnstr
#define mvinsnstr RENAMEFUNC(mvinsnstr)
#endif

#ifdef getnstr
inline int RENAMEFUNC(getnstr)(char *_str, int n)  { return getnstr(_str, n); }
#undef getnstr
#define getnstr RENAMEFUNC(getnstr)
#endif

#ifdef getyx
inline void RENAMEFUNC(getyx)(const WINDOW* win, int& y, int& x) {
	getyx(win, y, x); }
#undef getyx
#define getyx RENAMEFUNC(getyx)
#endif

#ifdef getbegyx
inline void RENAMEFUNC(getbegyx)(WINDOW* win, int& y, int& x) { getbegyx(win, y, x); }
#undef getbegyx
#define getbegyx RENAMEFUNC(getbegyx)
#endif

#ifdef getmaxyx
inline void RENAMEFUNC(getmaxyx)(WINDOW* win, int& y, int& x) { getmaxyx(win, y, x); }
#undef getmaxyx
#define getmaxyx RENAMEFUNC(getmaxyx)
#endif

#ifdef hline
inline int RENAMEFUNC(hline)(chtype ch, int n) { return hline(ch, n); }
#undef hline
#define hline RENAMEFUNC(hline)
#endif

#ifdef inch
inline chtype RENAMEFUNC(inch)()  { return inch(); }
#undef inch
#define inch RENAMEFUNC(inch)
#endif

#ifdef insch
inline int RENAMEFUNC(insch)(char c)  { return insch(c); }
#undef insch
#define insch RENAMEFUNC(insch)
#endif

#ifdef insertln
inline int RENAMEFUNC(insertln)()  { return insertln(); }
#undef insertln
#define insertln RENAMEFUNC(insertln)
#endif

#ifdef leaveok
inline int RENAMEFUNC(leaveok)(WINDOW* win, bool bf)  { return leaveok(win, bf); }
#undef leaveok
#define leaveok RENAMEFUNC(leaveok)
#else
extern "C"  int NCURSES_API leaveok(WINDOW* win, bool bf);
#endif

#ifdef move
inline int RENAMEFUNC(move)(int x, int y)  { return move(x, y); }
#undef move
#define move RENAMEFUNC(move)
#endif

#ifdef refresh
inline int RENAMEFUNC(refresh)()  { return refresh(); }
#undef refresh
#define refresh RENAMEFUNC(refresh)
#endif

#ifdef redrawwin
inline int RENAMEFUNC(redrawwin)(WINDOW *win)  { return redrawwin(win); }
#undef redrawwin
#define redrawwin RENAMEFUNC(redrawwin)
#endif

#ifdef scrl
inline int RENAMEFUNC(scrl)(int l) { return scrl(l); }
#undef scrl
#define scrl RENAMEFUNC(scrl)
#endif

#ifdef scroll
inline int RENAMEFUNC(scroll)(WINDOW *win) { return scroll(win); }
#undef scroll
#define scroll RENAMEFUNC(scroll)
#endif

#ifdef scrollok
inline int RENAMEFUNC(scrollok)(WINDOW* win, bool bf)  { return scrollok(win, bf); }
#undef scrollok
#define scrollok RENAMEFUNC(scrollok)
#else
#if	defined(__NCURSES_H)
extern "C"  int NCURSES_API scrollok(WINDOW*, bool);
#else
extern "C"  int NCURSES_API scrollok(WINDOW*, char);
#endif
#endif

#ifdef setscrreg
inline int RENAMEFUNC(setscrreg)(int t, int b) { return setscrreg(t, b); }
#undef setscrreg
#define setscrreg RENAMEFUNC(setscrreg)
#endif

#ifdef standend
inline int RENAMEFUNC(standend)()  { return standend(); }
#undef standend
#define standend RENAMEFUNC(standend)
#endif

#ifdef standout
inline int RENAMEFUNC(standout)()  { return standout(); }
#undef standout
#define standout RENAMEFUNC(standout)
#endif

#ifdef subpad
inline WINDOW *RENAMEFUNC(subpad)(WINDOW *p, int l, int c, int y, int x)
{ return derwin(p, l, c, y, x); }
#undef subpad
#define subpad RENAMEFUNC(subpad)
#endif

#ifdef timeout
inline void RENAMEFUNC(timeout)(int delay) { timeout(delay); }
#undef timeout
#define timeout RENAMEFUNC(timeout)
#endif

#ifdef touchline
inline int RENAMEFUNC(touchline)(WINDOW *win, int s, int c)
{ return touchline(win, s, c); }
#undef touchline
#define touchline RENAMEFUNC(touchline)
#endif

#ifdef touchwin
inline int RENAMEFUNC(touchwin)(WINDOW *win) { return touchwin(win); }
#undef touchwin
#define touchwin RENAMEFUNC(touchwin)
#endif

#ifdef untouchwin
inline int RENAMEFUNC(untouchwin)(WINDOW *win) { return untouchwin(win); }
#undef untouchwin
#define untouchwin RENAMEFUNC(untouchwin)
#endif

#ifdef vline
inline int RENAMEFUNC(vline)(chtype ch, int n) { return vline(ch, n); }
#undef vline
#define vline RENAMEFUNC(vline)
#endif

#ifdef waddstr
inline int RENAMEFUNC(waddstr)(WINDOW *win, char *str) { return waddstr(win, str); }
#undef waddstr
#define waddstr RENAMEFUNC(waddstr)
#endif

#ifdef waddchstr
inline int RENAMEFUNC(waddchstr)(WINDOW *win, chtype *at) { return waddchstr(win, at); }
#undef waddchstr
#define waddchstr RENAMEFUNC(waddchstr)
#endif

#ifdef wstandend
inline int RENAMEFUNC(wstandend)(WINDOW *win)  { return wstandend(win); }
#undef wstandend
#define wstandend RENAMEFUNC(wstandend)
#endif

#ifdef wstandout
inline int RENAMEFUNC(wstandout)(WINDOW *win)  { return wstandout(win); }
#undef wstandout
#define wstandout RENAMEFUNC(wstandout)
#endif


#ifdef wattroff
inline int RENAMEFUNC(wattroff)(WINDOW *win, int att) { return wattroff(win, att); }
#undef wattroff
#define wattroff RENAMEFUNC(wattroff)
#endif

#ifdef chgat
inline int RENAMEFUNC(chgat)(int n, attr_t attr, short color, const void *opts) {
	return chgat(n, attr, color, opts); }
#undef chgat
#define chgat RENAMEFUNC(chgat)
#endif

#ifdef mvchgat
inline int RENAMEFUNC(mvchgat)(int y, int x, int n,
			  attr_t attr, short color, const void *opts) {
	return mvchgat(y, x, n, attr, color, opts); }
#undef mvchgat
#define mvchgat RENAMEFUNC(mvchgat)
#endif

#ifdef mvwchgat
inline int RENAMEFUNC(mvwchgat)(WINDOW *win, int y, int x, int n,
			   attr_t attr, short color, const void *opts) {
	return mvwchgat(win, y, x, n, attr, color, opts); }
#undef mvwchgat
#define mvwchgat RENAMEFUNC(mvwchgat)
#endif

#ifdef wattrset
inline int RENAMEFUNC(wattrset)(WINDOW *win, int att) { return wattrset(win, att); }
#undef wattrset
#define wattrset RENAMEFUNC(wattrset)
#endif

#ifdef winch
inline chtype RENAMEFUNC(winch)(const WINDOW* win) { return winch(win); }
#undef winch
#define winch RENAMEFUNC(winch)
#endif

#ifdef mvwaddch
inline int RENAMEFUNC(mvwaddch)(WINDOW *win, int y, int x, const chtype ch)
{ return mvwaddch(win, y, x, ch); }
#undef mvwaddch
#define mvwaddch RENAMEFUNC(mvwaddch)
#endif

#ifdef mvwaddchnstr
inline int RENAMEFUNC(mvwaddchnstr)(WINDOW *win, int y, int x, chtype *str, int n)
{ return mvwaddchnstr(win, y, x, str, n); }
#undef mvwaddchnstr
#define mvwaddchnstr RENAMEFUNC(mvwaddchnstr)
#endif

#ifdef mvwaddchstr
inline int RENAMEFUNC(mvwaddchstr)(WINDOW *win, int y, int x, chtype *str)
{ return mvwaddchstr(win, y, x, str); }
#undef mvwaddchstr
#define mvwaddchstr RENAMEFUNC(mvwaddchstr)
#endif

#ifdef addnstr
inline int RENAMEFUNC(addnstr)(const char *str, int n)
{ return addnstr(str, n); }
#undef addnstr
#define addnstr RENAMEFUNC(addnstr)
#endif

#ifdef mvwaddnstr
inline int RENAMEFUNC(mvwaddnstr)(WINDOW *win, int y, int x, const char *str, int n)
{ return mvwaddnstr(win, y, x, str, n); }
#undef mvwaddnstr
#define mvwaddnstr RENAMEFUNC(mvwaddnstr)
#endif

#ifdef mvwaddstr
inline int RENAMEFUNC(mvwaddstr)(WINDOW *win, int y, int x, const char * str)
{ return mvwaddstr(win, y, x, str); }
#undef mvwaddstr
#define mvwaddstr RENAMEFUNC(mvwaddstr)
#endif

#ifdef mvwdelch
inline int RENAMEFUNC(mvwdelch)(WINDOW *win, int y, int x)
{ return mvwdelch(win, y, x); }
#undef mvwdelch
#define mvwdelch RENAMEFUNC(mvwdelch)
#endif

#ifdef mvwgetch
inline int RENAMEFUNC(mvwgetch)(WINDOW *win, int y, int x) { return mvwgetch(win, y, x);}
#undef mvwgetch
#define mvwgetch RENAMEFUNC(mvwgetch)
#endif

#ifdef mvwgetstr
inline int RENAMEFUNC(mvwgetstr)(WINDOW *win, int y, int x, char *str)
{return mvwgetstr(win, y, x, str);}
#undef mvwgetstr
#define mvwgetstr RENAMEFUNC(mvwgetstr)
#endif

#ifdef mvwgetnstr
inline int RENAMEFUNC(mvwgetnstr)(WINDOW *win, int y, int x, char *str, int n)
{return mvwgetnstr(win, y, x, str, n);}
#undef mvwgetnstr
#define mvwgetnstr RENAMEFUNC(mvwgetnstr)
#endif

#ifdef mvwinch
inline chtype RENAMEFUNC(mvwinch)(WINDOW *win, int y, int x) {
	return mvwinch(win, y, x);}
#undef mvwinch
#define mvwinch RENAMEFUNC(mvwinch)
#endif

#ifdef mvwinsch
inline int RENAMEFUNC(mvwinsch)(WINDOW *win, int y, int x, char c)
{ return mvwinsch(win, y, x, c); }
#undef mvwinsch
#define mvwinsch RENAMEFUNC(mvwinsch)
#endif

#ifdef mvaddch
inline int RENAMEFUNC(mvaddch)(int y, int x, chtype ch)
{ return mvaddch(y, x, ch); }
#undef mvaddch
#define mvaddch RENAMEFUNC(mvaddch)
#endif

#ifdef mvaddnstr
inline int RENAMEFUNC(mvaddnstr)(int y, int x, const char *str, int n)
{ return mvaddnstr(y, x, str, n); }
#undef mvaddnstr
#define mvaddnstr RENAMEFUNC(mvaddnstr)
#endif

#ifdef mvaddstr
inline int RENAMEFUNC(mvaddstr)(int y, int x, const char * str)
{ return mvaddstr(y, x, str); }
#undef mvaddstr
#define mvaddstr RENAMEFUNC(mvaddstr)
#endif

#ifdef mvdelch
inline int RENAMEFUNC(mvdelch)(int y, int x) { return mvdelch(y, x);}
#undef mvdelch
#define mvdelch RENAMEFUNC(mvdelch)
#endif

#ifdef mvgetch
inline int RENAMEFUNC(mvgetch)(int y, int x) { return mvgetch(y, x);}
#undef mvgetch
#define mvgetch RENAMEFUNC(mvgetch)
#endif

#ifdef mvgetstr
inline int RENAMEFUNC(mvgetstr)(int y, int x, char *str) {return mvgetstr(y, x, str);}
#undef mvgetstr
#define mvgetstr RENAMEFUNC(mvgetstr)
#endif

#ifdef mvgetnstr
inline int RENAMEFUNC(mvgetnstr)(int y, int x, char *str, int n) {
	return mvgetnstr(y, x, str, n);}
#undef mvgetnstr
#define mvgetnstr RENAMEFUNC(mvgetnstr)
#endif

#ifdef mvinch
inline chtype RENAMEFUNC(mvinch)(int y, int x) { return mvinch(y, x);}
#undef mvinch
#define mvinch RENAMEFUNC(mvinch)
#endif

#ifdef mvinsch
inline int RENAMEFUNC(mvinsch)(int y, int x, char c)
{ return mvinsch(y, x, c); }
#undef mvinsch
#define mvinsch RENAMEFUNC(mvinsch)
#endif

#ifdef napms
inline void RENAMEFUNC(napms)(unsigned long x) { napms(x); }
#undef napms
#define napms RENAMEFUNC(napms)
#endif

#ifdef fixterm
inline int RENAMEFUNC(fixterm)(void) { return fixterm(); }
#undef fixterm
#define fixterm RENAMEFUNC(fixterm)
#endif

#ifdef resetterm
inline int RENAMEFUNC(resetterm)(void) { return resetterm(); }
#undef resetterm
#define resetterm RENAMEFUNC(resetterm)
#endif

#ifdef saveterm
inline int RENAMEFUNC(saveterm)(void) { return saveterm(); }
#undef saveterm
#define saveterm RENAMEFUNC(saveterm)
#endif

#ifdef crmode
inline int RENAMEFUNC(crmode)(void) { return crmode(); }
#undef crmode
#define crmode RENAMEFUNC(crmode)
#endif

#ifdef nocrmode
inline int RENAMEFUNC(nocrmode)(void) { return nocrmode(); }
#undef nocrmode
#define nocrmode RENAMEFUNC(nocrmode)
#endif

#ifdef getbkgd
inline chtype RENAMEFUNC(getbkgd)(const WINDOW *win) { return getbkgd(win); }
#undef getbkgd
#define getbkgd RENAMEFUNC(getbkgd)
#endif

#ifdef bkgd
inline int RENAMEFUNC(bkgd)(chtype ch) { return bkgd(ch); }
#undef bkgd
#define bkgd RENAMEFUNC(bkgd)
#endif

#ifdef bkgdset
inline void RENAMEFUNC(bkgdset)(chtype ch) { bkgdset(ch); }
#undef bkgdset
#define bkgdset RENAMEFUNC(bkgdset)
#endif
