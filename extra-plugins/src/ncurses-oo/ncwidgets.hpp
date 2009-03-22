#ifndef teeny_WIDGETS_HPP_INCLUDED
#define teeny_WIDGETS_HPP_INCLUDED 1

////////////////////////////////////////////////////////////////////////
// ncwidgets.hpp
// defines widgets-related types for ncutil.
////////////////////////////////////////////////////////////////////////

#include <ncurses.h>
#include <string>
#include <iostream>
#include <map>

#include "ncmode.hpp"
#include "ncpanel.hpp"

namespace ncutil {

	/**
	   A very basic base type for "dialog boxes". Intended
	   mainly for "here's some text, now press a key" types
	   of user interaction/interruption.
	*/
	class NCDialog : public NCPanel
	{

	public:
		/**
		   Creates a new (hidden) dialog with the given number
		   of rows and columns. Clients should not modify this
		   window object's content, but should instead use
		   getViewWidgets() to get at the actual "content"
		   widget. Note that the content widget will be 2 rows
		   and 2 columns smaller than this widget, to
		   accomodate decorations for this window.
		*/
		NCDialog( int rows, int cols );
		virtual ~NCDialog();

		/**
		   This is the interface for triggering this
		   dialog. The exact meaning of the number returned
		   may be subclass/client-specific. The convention is
		   to return the key pressed by the user which causes
		   this window to popdown (i.e., causes popup() to
		   stop processing and return).

		   This function calls the virtual functions
		   decorate() to set up this window's view, and then
		   doPopup() to do anything else. It returns the same
		   value returned by doPopup(). This function also
		   takes care of showing and hiding the window, but
		   does not refresh() the window before doPopup()
		   because it is assumed that doPopup() will want to
		   make screen updates and therefor need to do a
		   refresh().  The window is of course refresh()ed
		   after hiding it.

		   Bug: metad keys may not be returned with the
		   proper metaness.
		*/
		int popup() throw(NCException);

		/**
		   Gets this window's title text.
		*/
		std::string title() const;

		/**
		   Sets this window's title text.
		*/
		void title(const std::string & );

		/**
		   Gets this window's body text.
		*/
		std::string text() const;

		/**
		   Sets this window's body text. Subclasses need not
		   use the text - it is mainly provided here so that
		   this class is functionally complete and usable, as
		   opposed to abstract.
		*/
		void text(const std::string & );
		
		/**
		   Returns the widget used to hold the dialog's
		   content. Ownership does not change by calling this
		   function: this object owns the widget and it will
		   be destroyed when this object destructs.

		   Note that this class does lazy instantiation of the
		   view, and does not create the child widget until
		   this function is called. Normally it is called by
		   decorate() and/or doPopup(), and not directly from
		   client code, but it may be used by client code for
		   drawing to this dialog. Do any curses writes to
		   the view window, not this NCDialog object, so you
		   don't have to account for dialog decorations and
		   whatnot when drawing.
		*/
		NCWindow * getViewWidget();

	protected:
		/**
		   Subclasses should override this. It is called from
		   popup() after making the dialog visible.  The
		   default implementation simply prints text() to the
		   window and waits for a keypress. It returns the key
		   pressed by the user.

		   If this window needs to be redrawn, it should
		   be done from this function: popup() does not
		   do this.

		   Subclasses which reimplement this probably
		   should not also call this default implementation, because
		   it's exact behaviours may change in the future. That is,
		   if you reimplement, completely reimplement it (the default
		   impl is only 3 lines of code).

		   See popup() for more info regarding this type's
		   popup conventions.
		*/
		virtual int doPopup();


		/**
		   Sets up this object's "look". 

		   The default implementation boxes the window, draws
		   the title at the top/left, and writes a simple
		   flashing prompt at the bottom.

		   Subclasses which reimplement this function should
		   not draw window content from here, only modify the
		   look of the dialog window (and possibly it's
		   buttons, if any). It is called by the non-virtual
		   popup() function so that the dialog may decorate
		   itself.

		   decorate() need not call refresh(), as one is
		   assumed to come from doPopup(), which is called
		   after decorate().
		*/
		virtual void decorate();

	private:
		std::string m_title;
		std::string m_text;
		NCWindow * m_ch;
	};


	/**
	   Pops up an extremely simple dialog box which displays some
	   text and waits for a keypress. Returns the result of the
	   dialog's popup(), which is conventionally the key pressed
	   by the user to dismiss the dialog.

	   If rows or cols are -1 then we try to auto-determine them,
	   but the very basic heuristic is not yet well-proven, so
	   don't rely on it.

	   If either of y or x are -1 then center_window()
	   is used to place the dialog.

	   The geometry units are given for the display area: the
	   actual size of the windows will be two units greater in
	   width/height to accomodate a border around the dialog.
	*/
	int popup_dialog( const std::string & title,
			  const std::string & text,
			  int rows = -1, int cols = -1,
			  int y = -1, int x = -1 ) throw(NCException);




	/*****
	      NCStreamPad takes a stream and hijacks it, inserting
	      it's own read buffer, such that output to the stream
	      gets sent out the pad instead. Finally, we can easiliy
	      keep std::cerr output from hosing our curses modes. :)
	      The pad provides a scrollback buffer, which can be
	      navigated via the inherited requestOp() member function.

	******/
	class NCStreamPad : public NCFramedPad
	{
	public:
		/**
		   For the life of this object, os.rdbuf() is replaced
		   with a buffer of our choosing. It is replaced when
		   this object destructs. It is expected that users
		   will pass some "shared" stream like, e.g.,
		   std::cout to the ctor.

		   The linebuffer parameter specifies the number of
		   scroll lines to keep. If this is smaller than
		   towrap.height(), no telling what'll happen.

		   Passing 0 for linebuffer means to use the rather
		   arbitrarily chosen default value of 400.

		   cols is the number of columns. Must be "some
		   minimum size" or it won't be drawn (in my
		   experience).

		   Passing 0 cols means to use (towrap().width()-2),
		   which means it will accomodate the window
		   decorations. This appears to work well, and is
		   quite human-readable, vis-a-vis line-wrapping.

		   See NCStreamBuffer for infoon the curses_attr
		   parameter.
		   
		   As a Major Kludge, the viewport is scrolled to the
		   bottom at startup, so that scrolling works as expected
		   as stream output comes in.
		*/
		NCStreamPad( NCWindow &towrap,
			     std::ostream & os,
			     int linebuffer = 0,
			     int cols = 0,
			     unsigned long curses_attr = 0
			     ) throw(NCException);

		/**
		   Uses a built-in bogus stream, which we are only
		   interested in as a carrier for our redirecting
		   streambuffer. Use ostream() to get at it.
		*/
		NCStreamPad( NCWindow &towrap,
			     int linebuffer = 0,
			     int cols = 0,
			     unsigned long curses_attr = 0
			     ) throw(NCException);

		/**
		   Restores this object's stream's
		   pre-this-object's-ctor streambuffer. (Got that?)
		*/
		virtual ~NCStreamPad();

		/**
		   Returns this object's ostream, which provides a
		   roundabout way of getting text into this object's
		   pad.
		*/
		std::ostream & ostream();

		// WTF do i need this for downstream code to see it???
 		//using NCKeyConsumer::consumeKey;

		/**
		   Clients may want to plug more than one stream
		   into this view. The way to do that is
		   get this streambuffer and call rdbuf(streambuffer)
		   on the stream you want to hijack.

		   Ownership does not change by calling this function.
		   If this object dies before a hijacked stream is
		   released of this buffer, you're just asking for a
		   segfault. Likewise, if this object's stream
		   destructs before this object does... bye bye!

		   Only use this function is you must, and prefer to
		   use the NCStreamBuffer directly if you need
		   to send output from N streams to one pad.
		*/
		std::streambuf * rdbuf();

		/**
		   Sets this object's curses flags, as explained in
		   the ctor docs.
		*/
		void cursesAttr( unsigned long f );

	private:
		std::ostream * m_os;
// 		std::streambuf * m_oldbuf;
		std::streambuf * m_buf;
		bool m_ownos;
		void init( int lines, unsigned long curses_attr );

	};


	/**
	   NCFileViewer is a simple class for loading input
	   from files or streams.
	*/
	class NCFileViewer : public NCPanel
	{
	public:
		/**
		   Creates a new file view of the given size.
		*/
		NCFileViewer( int lines, int cols, int aty, int atx ) throw(NCException);
		/**
		   Sets this panel to the same size and background
		   attributes as the given window.
		*/
		NCFileViewer( const NCWindow & inherit ) throw(NCException);
		virtual ~NCFileViewer();

		/**
		   Loads the viewer content from the given
		   stream. Returns true on success, false on error.
		   May show a dialog box on error if dialogOnError is
		   true.

		   The viewer's internal scroller must be recreated
		   each time this is called, in order to set it's
		   buffer size (which can only be set in the ctor :(
		   ).

		   It will throw if curses cannot allocate enough
		   screen space for the whole input.
		*/
		bool load( std::istream & is, bool dialogOnError = true ) throw(NCException);

		/**
		   Same as load(istream), but loads from a named file.
		*/
		bool load( const std::string & fname, bool dialogOnError = true ) throw(NCException);

		/**
		   Switches input focus to the internal scroll view,
		   allowing user to browse the input file, if any.
		   Use the NCFramedPad request keys to manipulate it (F10
		   exits, unless that key has been reassigned).
		*/
		void doInputLoop();

		/**
		   Reimplemented to write file name and scroll
		   positionto screen.
		*/
		virtual int noutrefresh();

		/**
		   Reimplemented to clear out internal info.
		*/
		virtual int clear();
	private:
		void drawLabels();
		std::string m_fname;
		NCStreamPad * m_pad;
	};


} // namespace ncutil


#endif // teeny_WIDGETS_HPP_INCLUDED
