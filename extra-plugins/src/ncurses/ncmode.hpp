#ifndef WANDERINGHORSE_NET_GOOGLE_V8_NCMODE_H_INCLUDED
#define WANDERINGHORSE_NET_GOOGLE_V8_NCMODE_H_INCLUDED 1

namespace v8 {
namespace juice {
namespace nc {

	/**
	   NCMode is intended to help us avoid having to really
	   know if we're in curses mode or not.

	   The idea is that each routine which needs curses mode
	   should create one of these objects in the scope which fits
	   the intended duration of the use of the curses screen.

	   curses is started with noecho() and cbreak() enabled. We
	   also set up color mode and call install_full_palette(). You
	   DO want COLOR, don't you???
	*/
	class NCMode
	{
	public:
		/**
		   Starts curses mode, if it isn't already started (as
		   determined by THIS class, which might not be 100%
		   accurate!).

		   It may throw an NCException during
		   initialization if this sentry object is one which
		   triggers curses mode activation.
		*/
		NCMode();

		/**
		   Ends curses mode (::endwin()) if the
		   NCMode count drops to zero.
		*/
		~NCMode() throw();

		/**
		   Client code should not normally call this.
		   Instantiating an NCMode object will do so if it
		   needs doing.
		*/
		static void startup();

		/**
		   Returns true if any CurseModeSentry objects
		   are currently active. If the app only uses
		   NCMode to control the start/stop of curses
		   mode then this will reflect the state of the
		   terminal (curses or non-curses).
		*/
		static bool isActive() throw();

		/**
		   Force-invalidate all sentries and shut down curses
		   mode. Do not simply call this because it sounds
		   like a good idea! Doing so invalidates ALL sentries,
		   which means that sentries in still-living objects
		   become useless. It is provided mainly for use in
		   signal handlers which want to exit() the app
		   and want to make sure curses is gone,
		   or similar cases.
		*/
		static void shutdown() throw();

	};

}}} // namespaces


#endif // WANDERINGHORSE_NET_GOOGLE_V8_NCMODE_H_INCLUDED
