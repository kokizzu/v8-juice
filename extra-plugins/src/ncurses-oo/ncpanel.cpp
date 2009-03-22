// * this is for making emacs happy: -*-Mode: C++;-*-
/****************************************************************************
 * Copyright (c) 1998-2002,2003 Free Software Foundation, Inc.              *
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

/****************************************************************************
 *   Author: Juergen Pfeifer, 1993, 1997                                    *
 ****************************************************************************/

#include "ncpanel.hpp"
#include <string.h>

namespace ncutil {

	NCPanel* NCPanel::dummy = (NCPanel*)0;


	NCPanel::NCPanel(int lines,
			 int cols,
			 int begin_y,
			 int begin_x) throw(NCException)
		: NCWindow(lines,cols,begin_y,begin_x)
	{
		this->init();
	}

	NCPanel::NCPanel() throw(NCException)
		: NCWindow(::stdscr)
	{
		this->init();
	}

	void NCPanel::init() throw(NCException)
	{
		p = ::new_panel(this->ncwindow());
		if (!p)
			OnError(ERR);
	}

	NCPanel::~NCPanel() {
		::del_panel(p);
		::update_panels();
	}

	void
	NCPanel::redraw() {
		PANEL *pan;

		pan = ::panel_above(NULL);
		while (pan) {
			::touchwin(panel_window(pan));
			pan = ::panel_above(pan);
		}
		::update_panels();
		::doupdate();
	}

	int
	NCPanel::refresh() {
		::update_panels();
		return ::doupdate();
	}

	int
	NCPanel::noutrefresh() {
		::update_panels();
		return OK;
	}

	void
	NCPanel::boldframe(const char *title, const char* btitle) {
		standout();
		frame(title, btitle);
		standend();
	}

	void
	NCPanel::frame(const char *title,const char *btitle) {
		int err = OK;
		if (!title && !btitle) {
			err = box();
		}
		else {
			err = box();
			if (err==OK)
				label(title,btitle);
		}
		OnError(err);
	}

	void
	NCPanel::label(const char *tLabel, const char *bLabel) {
		if (tLabel)
			centertext(0,tLabel);
		if (bLabel)
			centertext(maxy(),bLabel);
	}

	void
	NCPanel::centertext(int row,const char *label) {
		if (label) {
			int x = (maxx() - ::strlen(label)) / 2;
			if (x<0)
				x=0;
			OnError(addstr(row, x, label, width()));
		}
	}

	int
	NCPanel::getKey(void) {
		return getch();
	}

} // namespace ncutil
