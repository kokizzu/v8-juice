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
 *   Author: Juergen Pfeifer, 1999                                          *
 ****************************************************************************/

/************************************************************************
This copy was forked from the ncurses 5.4 source tree and mutilated
by stephan@s11n.net, starting 1 March 2005:

- Moved into ncutil namespace and all classes renamed: s/NCurses/NC/

- An ongoing redesign of some of the OO components, especially vis-a-vis
  OO design, event handling and NCPad.

- NCPad: REQ_PAD_xxx global enum moved into class, entries renamed,
  and more options added:

- NCPad: added pageup/down and scroll-to top/bottom support.

************************************************************************/

#include "ncwindow.hpp"


#include <sstream>
#include "ncwidgets.hpp" // only for debuggering


#ifndef KEY_CTRL
#  define KEY_CTRL(X) ((X) & 0x1f)
#endif

namespace ncutil {

	NCPad::NCPad(int lines, int cols) throw(NCException)
		: NCWindow(),
		  viewWin((NCWindow*)0),
		  viewSub((NCWindow*)0),
		  m_lines(lines),
                  m_cols(cols),
		  h_gridsize(1), v_gridsize(1),
		  min_row(0), min_col(0)
	{
		this->ncwindow( ::newpad(lines,cols) );
		if ((WINDOW*)0==this->ncwindow())
		{
			std::ostringstream msg;
			msg << "NCPad(lines="<<lines
			    <<",cols="<<cols<<"): "
			    << "::newpad(lines,cols) failed.";
			throw NCException(msg.str());
		}


		NCPad::key_req_map & krm = this->keyReqMap();
		krm[KEY_UP] = PadReqUp;
		krm[KEY_DOWN] = PadReqDown;
		krm[KEY_LEFT] = PadReqLeft;
		krm[KEY_RIGHT] = PadReqRight;
		krm[KEY_NPAGE] = PadReqPageDown;
		krm[KEY_PPAGE] = PadReqPageUp;
		krm[KEY_HOME] = PadReqScrollToTop;
		krm[KEY_END] = PadReqScrollToBottom;
		krm[KEY_CTRL('L')] = PadReqRefresh;
		// ^^^^ we arguably should not handle this, because we may inadvertently
		// keep a top-level widget from handling it (happened to me while developing)
		// an NCWorkspace subclass).
		krm[KEY_CTRL('W')] = krm[KEY_F(10)] = PadReqExit;
		// ^^^ Ctrl-W is common under WMs like KDE

	}


	bool NCPad::consumeKey( int key, bool )
	{
 		int req = this->reqForKey( key );
 		if( PadReqUnknown == req ) return false;
 		this->requestOp( req );
		return true;
	}

	int NCPad::requestOp( int req  )
	{
		/**
		   Maintenance notes:

		   This code was moved here from the NCFramedPad
		   subclass, since this class has access to all of the
		   vars manipulated here. i do not yet know if this
		   makes sense, vis-a-vis the usage of this class.
		   It seems okay, but maybe this code needs to 
		   move back to NCFramedPad.
		*/

		NCWindow* W = this->getDrawWindow();
		NCWindow * viewWin = this->getViewWindow();
		if (0 == W || 0 == viewWin) return PadReqIgnored;
		int Width  = W->width();
		int Height = W->height();
	

		/**
		   This code was taken almost 100% from cursesw.cc,
		   from the ncurses 5.4 source tree, the NCursesFramedPad
		   class. 

		   It has been modified to:

		   - Move it into this base class.

		   - remove the getch() loop, which we perform from
		   client code and pass the events here.

		   - Added PageUp/Down.

		   - Added ScrollToTop/Bottom
		*/
		bool changed = FALSE;

		int ret = req;
		int pagesize = (W->height()-1) / this->v_gridsize;
		int loopto = 0;
		int loopcv = 0;
		switch (req)
		{
		  case PadReqExit:
			  break;
		  case PadReqRefresh:
			  // ================
			  changed = TRUE;
			  break;
		  case PadReqLeft:
			  // =============
			  if (min_col > 0) {
				  changed = TRUE;
				  if (min_col < h_gridsize)
					  min_col = 0;
				  else
					  min_col -= h_gridsize;
			  }
			  else
			  {
				  ret = PadReqIgnored;
				  OnNavigationError(req);
			  }
			  break;
		  case PadReqRight:
			  // ==============
			  if (min_col < (width() - Width - 1)) {
				  changed = TRUE;
				  if (min_col > (width() - Width - h_gridsize - 1))
					  min_col = width() - Width - 1;
				  else
					  min_col += h_gridsize;
			  }
			  else
			  {
				  ret = PadReqIgnored;
 				  OnNavigationError(req);
			  }
			  break;
		  case PadReqScrollToTop:
			  if (min_row > 0 )
			  {
				  changed = TRUE;
				  min_row = 0;
				  //this->inch( min_row, 0 );
			  }
			  else
			  {
 				  OnNavigationError(req);
				  ret = PadReqIgnored;
			  }
			  break;
		  case PadReqUp:
		  case PadReqPageUp:
			  // ===========
			  loopto = (req == PadReqUp) ? 1 : pagesize;
			  //std::cout << "loopto="<<loopto<<", pagesize="<<pagesize<<"\n";
			  for( loopcv = 0; loopcv < loopto; loopcv++ )
			  {
				  if (min_row > 0) {
					  changed = TRUE;
					  if (min_row < v_gridsize)
						  min_row = 0;
					  else
						  min_row -= v_gridsize;
				  }
				  else
				  {
					  if( loopcv > 0 )
					  {
						  // ^^^ we're going to ignore
						  // these on scrolls after
						  // the first, so that a page
						  // up/down returns the
						  // original request code.
						  break;
					  }
					  OnNavigationError(req);
					  ret = PadReqIgnored;
					  break; // so we don't keep trying on a Page request
				  }
			  }
			  break;
		  case PadReqScrollToBottom:
			  if (min_row < (height() - Height - 1)) {
				  changed = TRUE;
				  min_row = height() - Height - 1;
				  //this->inch( height() - 1, 0 );
			  }
			  else
			  {
				  OnNavigationError(req);
				  ret = PadReqIgnored;
			  }
			  break;
		  case PadReqDown:
		  case PadReqPageDown:
			  // =============
			  //std::cout << "loopto="<<loopto<<", pagesize="<<pagesize<<"\n";
			  loopto = (req == PadReqDown) ? 1 : pagesize;
			  for( loopcv = 0; loopcv < loopto; loopcv++ )
			  {
				  if (min_row < (height() - Height - 1)) {
					  changed = TRUE;
					  if (min_row > (height() - Height - v_gridsize - 1))
						  min_row = height() - Height - 1;
					  else
						  min_row += v_gridsize;
				  }
				  else
				  {
					  if( loopcv > 0 )
					  {
						  // ^^^ we're going to ignore
						  // these on scrolls after
						  // the first, so that a page
						  // up/down returns the
						  // original request code.
						  break;
					  }
					  ret = PadReqIgnored;
					  OnNavigationError(req);
					  break; // so we don't keep trying on a Page request
				  }
			  }
			  break;
		  default:
			  ret = PadReqUnknown;
			  OnUnknownOperation(req);
		}

		if (changed) {
			noutrefresh();
			W->syncup();
			OnOperation(req);
			viewWin->refresh();
		}

		return ret;
	}


	void NCPad::inputLoop(void)
	{
		NCWindow* W = this->getDrawWindow();
		if( ! W ) return;
		int key = 0;
		int req = this->requestOp( PadReqRefresh );
		while( true )
		{
			req = this->reqForKey((key=::getch())); // using this->getch() causes a weird locking behaviour
			if( req == PadReqUnknown ) continue;
			if( req == PadReqExit ) break;
			this->requestOp( req );
		}
	}


	int NCPad::refresh()
	{
		int res = this->noutrefresh();
		if (res==OK && ((NCWindow*)0 != viewWin)) {
			res = (viewWin->refresh());
		}
		return(res);
	}

	int NCPad::noutrefresh()
	{
		int res = OK;
		NCWindow* W = getDrawWindow();
		if ((NCWindow*)0 != W) {
			res = copywin(*W,min_row,min_col,
				      0,0,W->maxy(),W->maxx(),
				      FALSE);
			if (res==OK) {
				W->syncup();
				res = viewWin->noutrefresh();
			}
		}
		return (res);
	}

	void NCPad::setWindow(NCWindow& view,
				   int v_grid,
				   int h_grid) throw(NCException)
	{
		viewWin = &view;
		min_row = min_col = 0;
                h_gridsize = (h_grid > 0) ? h_grid : 1;
                v_gridsize = (v_grid > 0) ? v_grid : 1;
	}

	void NCPad::setSubWindow(NCWindow& sub) throw(NCException)
	{
		if ((NCWindow*)0 == viewWin)
			throw NCException("NCPad::setSubWindow(): this pad has no viewport");
		if (!viewWin->isDescendant(sub))
			throw NCException("NCPad::setSubWindow(): !viewWin->isDescendant(sub)");
		viewSub = &sub;
	}

	NCPad::key_req_map &
	NCPad::keyReqMap()
	{
		return this->m_keys;
	}

	const NCPad::key_req_map &
	NCPad::keyReqMap() const
	{
		return this->m_keys;
	}

	void NCPad::mapKeyToReq( int key, int padreq )
	{
		this->keyReqMap()[key] = padreq;
	}


	int NCPad::reqForKey( int key ) const
	{
		key_req_map::const_iterator it = this->m_keys.find( key );
		if( this->m_keys.end() == it ) return PadReqUnknown;
		return (*it).second;
	}



	int NCPad::topViewLine() const
	{
		return min_row;
	}

	int NCPad::leftViewColumn() const
	{
		return min_col;
	}

	void NCPad::topViewLine( int l )
	{
		this->min_row = l;
	}
	void NCPad::leftViewColumn( int c )
	{
		this->min_col = c;
	}

	int NCPad::lineCount() const
	{
		return this->m_lines;
	}
	int NCPad::columnCount() const
	{
		return this->m_cols;
	}


	void NCPad::OnUnknownOperation(int pad_req)
	{
	}

	void NCPad::OnNavigationError(int pad_req)
	{
		::beep();
	}

 	void NCPad::OnOperation(int pad_req)
	{
	}

	int NCPad::viewHeight() const
	{
		const NCWindow * v = this->viewSub  ? this->viewSub  : (this->viewWin ? this->viewWin : 0);
		return v ? v->height() : 0;
	}


	void NCFramedPad::OnOperation(int pad_req)
	{

		NCWindow* W = getDrawWindow();
		NCWindow* Win = getViewWindow();

		if (((NCWindow*)0 == W) || ((NCWindow*)0 == Win)) return;

		int Width  = W->width();
		int Height = W->height();
		int i, row, col, h_len, v_len;
		const int atrow = this->topViewLine();
		const int atcol = this->leftViewColumn();
		h_len = (Width*Width + width() - 1)/width();
		if (h_len==0)
			h_len = 1;
		if (h_len > Width)
			h_len = Width;
		
		v_len = (Height*Height + height() - 1)/height();
		if (v_len==0)
			v_len = 1;
		if (v_len > Height)
			v_len = Height;

		col  = (atcol * Width + width() - 1)  / width();
		if (col + h_len > Width)
			col = Width - h_len;
		
		row  = (atrow * Height + height() - 1) / height();
		if (row + v_len > Height)
			row = Height - v_len;

		Win->vline(1,Width+1,Height);
		Win->attron(A_REVERSE);
		if (v_len>=2) {
			Win->addch(row+1,Width+1,ACS_UARROW);
			for(i=2;i<v_len;i++)
				Win->addch(row+i,Width+1,' ');
			Win->addch(row+v_len,Width+1,ACS_DARROW);
		}
		else {
			for(i=1;i<=v_len;i++)
				Win->addch(row+i,Width+1,' ');
		}
		Win->attroff(A_REVERSE);

		Win->hline(Height+1,1,Width);
		Win->attron(A_REVERSE);
		if (h_len >= 2) {
			Win->addch(Height+1,col+1,ACS_LARROW);
			for(i=2;i<h_len;i++)
				Win->addch(Height+1,col+i,' ');
			Win->addch(Height+1,col+h_len,ACS_RARROW);
		}
		else {
			for(i=1;i<=h_len;i++)
				Win->addch(Height+1,col+i,' ');
		}
		Win->attroff(A_REVERSE);

	}





	NCFramedPad::NCFramedPad(NCWindow& win, int lines, int cols,
				 int v_grid, int h_grid)
		: NCPad(lines, (cols ? cols : win.width()-2))
	{
		this->bkgd( win.getbkgd() );
		this->NCPad::setWindow(win, v_grid, h_grid);
		this->NCPad::setSubWindow(*(new NCWindow(win)));
		this->scrollok(true);
		this->getSubWindow()->scrollok( true );
	}

	NCFramedPad::~NCFramedPad()
	{
		delete getSubWindow();
	}

	void NCFramedPad::setWindow(NCWindow& view, int v_grid, int h_grid)  throw(NCException)
	{
		throw NCException("setWindow() is not allowed on NCFramePad");
		// Disable this call; the viewport is already defined
	}

	void NCFramedPad::setSubWindow(NCWindow& sub) throw(NCException)
	{
		throw NCException("setSubWindow() is not allowed on NCFramePad");
	}

} // namespace ncutil
