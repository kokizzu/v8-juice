#include "ncmode.hpp"
#include "ncpalette.hpp" // install_full_palette()

namespace ncutil {

    int NCMode_sentry_count = 0;
    NCMode::NCMode() throw(NCException)
    {
        try
        {
            if( 1 == ++NCMode_sentry_count )
            {
                NCMode::startup();
            }
        }
        catch( const NCException & ex )
        {
            --NCMode_sentry_count;
            throw ex;
        }
        catch( ... )
        {
            --NCMode_sentry_count;
            throw NCException("Unknown exception initializing NCMode sentry." );
        }
    }

    void NCMode::startup() throw(NCException)
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
            catch( const NCException & ex )
            {
                // nothing. maybe someday
            }
            catch( ... )
            {
                // nothing.
            }
        }
    }

} // namespace ncutil
