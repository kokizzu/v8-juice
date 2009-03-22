#ifndef teeny_NCUTIL_HPP_INCLUDED
#define teeny_NCUTIL_HPP_INCLUDED 1

#include <ncurses.h>
#include "ncpanel.hpp"

#include <string>
#include <map>
#include <iostream>

/************************************************************************
The KEY_CTRL macro is taken from curses++ sources. It was in a header
named internal.h, so it's usage is probably not portable, but it is
terribly useful and appears to work okay on my linux box.
************************************************************************/
#define KEY_CTRL(X) ((X) & 0x1f)

/**
   The ncutil namespace contains some reusable algorithms and classes
   for working with ncurses.

   All/most of the code was developed in conjunction with the Teeny
   toolkit, and thus some of it may not be very reusable/generic, but
   "if it uses curses, but doesn't rely on any app-specific API" then
   it's in this namespace instead of the teeny namespace. The
   intention is to re-use ncutil in other projects, or potentially
   even make a subproject out of it or submit it to the ncurses
   maintainers.

   The following classes were stolen from the ncurses++ 5.4 source
   tree in February 2005 and significantly refactored for use as a
   basis for ncutil's widgets:

   - NCursesWindow (now NCWindow).

   - NCursesPad (now NCPad).

   - NCursesFramedPad (now NCFramedPad).

   Those classes have non-restrictive custom licenses, as detailed in
   their source files.

   The remainder of the code, except where noted, was *probably*
   written by stephan@s11n.net, and is released into the Public
   Domain. The author disclaims all copyrights. (That said, patches
   and feedback are happily received.)

*/
namespace ncutil {

	/**
	   A functor to help consolidate code to commonly-signatured
	   curses functions like refresh(), top(), bottom(), etc.

	   BaseWindowT is normaly NCWindow, but you may need access
	   to, e.g., the NCPanel API (e.g., top()).

	   It is intended for use with std::for_each() and the like.
	*/
	template <typename BaseWindowT = NCWindow,typename ReturnT = int>
	struct WindowMemberFunc
	{
		typedef ReturnT result_type;
		typedef BaseWindowT window_type;
 		typedef std::mem_fun_t<ReturnT,BaseWindowT> panel_functor;
 		typedef result_type (BaseWindowT::*panel_member_function)();
		panel_functor func;
		WindowMemberFunc( panel_functor f ) : func(f)
		{}

		WindowMemberFunc( panel_member_function f ) : func(f)
		{}
		
		/**
		   Calls this->func(w). If w is null then
		   result_type() is returned.
		*/
		result_type operator()( window_type * w )
		{
			if( ! w ) return result_type();
			return this->func( w );
		}
	};



	/**
	   Intended to hold curses color attribute info for
	   characters, with the assumption that all instances of a
	   given character in a given "attribute space" share the same
	   attributes.

	   This type evolved from client-side code in which mapping
	   attributes on a per-char basis was called for. It may or
	   may not be useful in other contexts, but here it is anyway.
	*/
	typedef std::map<char,unsigned long> char_attr_map;

	/**
	   Returns a shared instance of a map with the given name.
	   Passing a name of a non-existant map returns a
	   newly-initialized, empty map.

	   These maps are intended to be used as colormaps (attribute
	   maps, really), and can be de/serialized if you've got
	   a library or algo for doing so (tip: libs11n).

	   This function does not invoke/require curses mode.

	   These maps are not used by this library, but are provided
	   as a place for client code to store rudimentary curses
	   information on a char-by-char basis.
	*/
	char_attr_map & nc_attr_map( const std::string & name );

	/**
	   Allows apps to share a common map without a) having to know
	   it's name and b) having to activate curses mode.

	   Returns the same as nc_attr_map( nc_current_attr_map_name() ).
	*/
	char_attr_map & nc_current_attr_map();

	/**
	   Sets the name of the current global attributes map.
	*/
	void nc_current_attr_map_name( const std::string & name );

	/**
	   Gets the name of the current global attributes map, which
	   is empty unless set by client code.
	*/
	std::string nc_current_attr_map_name();

	/**
	   A map of maps, intended to hold named attribute maps
	   for client-side use.
	*/
	typedef std::map<std::string,char_attr_map> char_attr_maps;

	/**
	   Returns all maps which nc_attr_map() could possible return.

	   This function does not invoke/require curses mode.
	*/
	char_attr_maps & nc_attr_maps();

	/**
	   Similar to NCPanel::frame(), but usable on
	   NCWindows (which don't have frame()).
	*/
	void box_and_label( NCWindow & w, const std::string & title );

	/**
	   Centers w on the screen.
	*/
	void center_window( NCWindow & w );


	/**
	   Returns the width of stdscr, or 0 before curses mode is startdd
	*/
	int screen_height();

	/**
	   Returns the height of stdscr, or 0 before curses mode is startdd
	*/
	int screen_width();



	void interactively_move_window( NCWindow & win );

	/**
	   A very basic streambuffer which sends all of it's input to
	   a given NCPanel. It does no buffering at all, sending
	   it's input byte-by-byte. It is intended to be used as a
	   replacement for the streambuffer owned by
	   eshell::ostream(), such that eshell client code which uses
	   that stream (it all should, instead of using cout) will
	   interact nicely with the curses UI.

	   Note that using two streambuffers on the same stream will
	   not work: only the last one installed will send any output,
	   and there's NO TELLING what sending output to the other
	   will do.

	   Sending more than one stream to one NCWindow is fine, and
	   unsigned long argument to the ctors is intended just for this case.
	*/
	class NCStreamBuffer : public std::streambuf
	{
	private:
		NCWindow * m_pnl;
		unsigned long m_flags;
		std::ostream * m_os;
		std::streambuf * m_old;
	public:
		/**
		   Implants this object as os's rdbuf(). When this
		   object is destroyed, the previous rdbuf() is
		   restored.

		   p and os must both outlive this object.

		   If curses_attr is non-zero then those
		   attributes are set on p before each write
		   and turned off after each write. This is useful
		   when mapping multiple streams to one pad.

		   p.scrollok(true) is called because:

		   a) that is (almost?) always the desired behaviour
		   for this class.

		   b) if p.addch() ever fails, as it "might" when
		   scrolling is disabled (untested), then os will be
		   marked as "done" by the iostreams framework and
		   won't be usable for output any more.

		*/
		NCStreamBuffer( NCWindow & p,
				      std::ostream & os,
				      unsigned long curses_attr = 0 );

		/**
		   Identical to the 3-arg constructor except
		   that it has no assosciated stream from
		   which it will receive input. The client should
		   call:

<pre>
		   std::streambuf * old = mystream.rdbuf();
		   mystream.rdbuf( myNCStreamBuffer );
</pre>

		   to rediect output sent to mystream to p, and should
		   call mystream.rdbuf(old) to restore the stream's
		   old buffer when the client is finished. This is
		   tedious and error-prone, and is provided mainly so
		   that classes like NCStreamPad can use this class
		   more sensibly by managing the reassigment of
		   this streambuffer themselves.
		*/
		NCStreamBuffer( NCWindow & p,
				      unsigned long curses_attr = 0 );


		/**
		   If this object was constructed with a target
		   stream, this restores that stream's rdbuf() to its
		   pre-ctor state. If no stream was assigned then
		   this function does nothing.
		*/
		virtual ~NCStreamBuffer();
		/**
		   Sends c to this object's panel.

		   If c is EOF or std::isspace(c) then sync() is called.

		   Potential TODO: write io manipulators
		   to toggle curses attributes??? Would be
		   cool, but sounds tedious.

		   Returns c on success and EOF if sync() returns EOF
		   or if NCWindow::addch() returns ERR.
		*/
		virtual int overflow( int c );

		/**
		   Calls refresh() on this object's window.  Returns 0
		   unless refresh() returns curses' ERR, in which case
		   this function returns EOF.
		*/
		virtual int sync();
	};



} // namespace ncutil


#endif // teeny_NCUTIL_HPP_INCLUDED
