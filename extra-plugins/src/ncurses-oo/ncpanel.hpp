// * This makes emacs happy -*-Mode: C++;-*-
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
 *   Author: Juergen Pfeifer, 1997                                          *
 ****************************************************************************/

#ifndef ncutil_NCPANEL_HPP_INCLUDED
#define ncutil_NCPANEL_HPP_INCLUDED 1

// $Id: ncpanel.hpp,v 1.2 2005/03/15 10:53:31 sgbeal Exp $

#include "ncwindow.hpp"

extern "C" {
#  include <panel.h> // C-level panel lib
}


namespace ncutil {

    class  NCPanel : public NCWindow {
    protected:
        PANEL *p;
        static NCPanel *dummy;

    private:

        void init() throw(NCException);                       // Initialize the panel object

    protected:
  
        void OnError (int err) const throw(NCException)
        {
            if (err==ERR)
            {
                throw NCException( "Default NCPanel::OnError() implementation got ERR." );
            }
        }

        // Get a keystroke. Default implementation calls getch()
        virtual int getKey(void);

    public:
        NCPanel(int lines,
                int cols,
                int begin_y = 0,
                int begin_x = 0) throw(NCException);
        // Create a panel with this size starting at the requested position.

        NCPanel() throw(NCException);
        // This constructor creates the default Panel associated with the
        // ::stdscr window. (What happens if we create more than one???)

        virtual ~NCPanel();
  
        // basic manipulation
        inline void hide() {
            OnError (::hide_panel(p));
        }
        // Hide the panel. It stays in the stack but becomes invisible.

        inline void show() {
            OnError (::show_panel(p));
        }
        // Show the panel, i.e. make it visible.

        inline void top() {
            OnError (::top_panel(p));
        }
        // Make this panel the top panel in the stack.
  
        inline void bottom() {
            OnError (::bottom_panel(p));
        }
        // Make this panel the bottom panel in the stack.
        // N.B.: The panel associated with ::stdscr is always on the bottom. So
        // actually bottom() makes the panel the first above ::stdscr.
  
        virtual int mvwin(int y, int x) {
            OnError(::move_panel(p, y, x));
            return OK;
        }
  
        inline bool hidden() const {
            return (::panel_hidden (p) ? TRUE : FALSE);
        }
        // Return TRUE if the panel is hidden, FALSE otherwise.

        /* The functions panel_above() and panel_below() are not reflected in
           the NCPanel class. The reason for this is, that we cannot
           assume that a panel retrieved by those operations is one wrapped
           by a C++ class. Although this situation might be handled, we also
           need a reverse mapping from PANEL to NCPanel which needs some
           redesign of the low level stuff. At the moment, we define them in the
           interface but they will always produce an error. */
        inline NCPanel& above() const {
            OnError(ERR);
            return *dummy;
        }

        inline NCPanel& below() const {
            OnError(ERR);
            return *dummy;
        }

        // Those two are rewrites of the corresponding virtual members of
        // NCWindow
        virtual int refresh();
        // Propagate all panel changes to the virtual screen and update the 
        // physical screen.

        virtual int noutrefresh();
        // Propagate all panel changes to the virtual screen.

        static void redraw();
        // Redraw all panels.
 
        // decorations
        virtual void frame(const char* title=NULL, 
                           const char* btitle=NULL);
        // Put a frame around the panel and put the title centered in the top line
        // and btitle in the bottom line.

        virtual void boldframe(const char* title=NULL,
                               const char* btitle=NULL);
        // Same as frame(), but use highlighted attributes.

        virtual void label(const char* topLabel,
                           const char* bottomLabel);
        // Put the title centered in the top line and btitle in the bottom line.

        virtual void centertext(int row,const char* label);
        // Put the label text centered in the specified row.
    };

} // namespace ncutil

#endif // ncutil_NCPANEL_HPP_INCLUDED
