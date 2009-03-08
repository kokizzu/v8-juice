#include "ncmode.hpp"
#include "nccolors.hpp" // install_full_palette()

namespace v8 {
namespace juice {
namespace nc {

    int NCMode_sentry_count = 0;
    NCMode::NCMode()
    {
	try
	{
	    if( 1 == ++NCMode_sentry_count )
	    {
		NCMode::startup();
	    }
	}
	catch( ... )
	{
	    --NCMode_sentry_count;
	    throw;
	}
    }

    void NCMode::startup()
    {
	::initscr();
	if (::has_colors()) {
	    ::start_color();
	    install_full_palette();
	}
	// todo: do we want this?	::leaveok(0,true);
	/**
	   todo: report bug in ncurses docs which say that
	   first arg to meta() and keypad() is ignored.  i get
	   the behaviour i want IF i pass ::stdscr here,
	   otherwise my arrow keys and F-keys behave
	   erratically.
	*/
	::meta(::stdscr,true);
	::keypad(::stdscr,true);
	::cbreak();
	::noecho();
    }


    bool NCMode::isActive() throw()
    {
	return 0 > NCMode_sentry_count;
    }

    void NCMode::shutdown() throw()
    {
	if(  ::stdscr )
	{
	    try
	    {
		::echo();
		// 		::nocbreak();
		::clear();
		::endwin();
	    }
	    catch( ... )
	    {
		// std::cerr << "NCMode::shutdown() an caught exception.\n";
	    }
	}
	NCMode_sentry_count = 0;
    }

    NCMode::~NCMode() throw()
    {
		
	if( NCMode_sentry_count
	    &&
	    (0 == --NCMode_sentry_count )
	    )
	{
	    try
	    {
		shutdown();
	    }
	    catch( ... )
	    {
		// nothing.
	    }
	}
    }

}}} // namespaces
