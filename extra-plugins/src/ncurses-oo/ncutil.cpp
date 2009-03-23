
#include "ncpalette.hpp"
#include "ncutil.hpp"

#include <iostream> // only for a test.
#include <sstream>

// #include <iostream> // only for debuggering
namespace ncutil {





    NCStreamBuffer::NCStreamBuffer( NCWindow & p,
                                    unsigned long curses_attr )
        : m_pnl(&p), m_flags(curses_attr), m_os(0),m_old(0)
    {
        // Tell parent class that we want to call overflow() for each
        // input char:
        this->setp( 0, 0 );
        this->setg( 0, 0, 0 );
        p.scrollok(true);
        p.inch( p.height() -1 , 0 );
    }

    NCStreamBuffer::NCStreamBuffer( NCWindow & p,
                                    std::ostream & os,
                                    unsigned long curses_attr )
        : m_pnl(&p), m_flags(curses_attr), m_os(&os),m_old(os.rdbuf())
    {
        this->setp( 0, 0 );
        this->setg( 0, 0, 0 );
        os.rdbuf( this );
        p.scrollok(true);
        p.inch( p.height() -1 , 0 );
    }

    NCStreamBuffer::~NCStreamBuffer()
    {
        if( this->m_os )
        {
            this->m_os->rdbuf( this->m_old );
        }
    }

    int NCStreamBuffer::overflow( int c )
    { 
        if( ! this->m_pnl ) return EOF;
#if 1
        /**
           Kludge for the JavaScript bindings, so that the stream will
           fail gracefully if/when we fall victim to deletions of
           NCWindow objects from outside of JavaScript's
           visibility. That can happen, e.g., when an NCWindow dtor
           cleans up child objects before the JS GC gets around to
           trying to destroy them.

           If using this code in a purely native context, this check
           can almost certainly be safely eliminated (as C++ scoping
           helps us to keep the lifetimes in order).
        */
        if( ! NCWindow::is_alive( this->m_pnl ) )
        {
            this->m_pnl = 0;
            if( this->m_os )
            {
                this->m_os->rdbuf( this->m_old );
                this->m_os = 0;
            }
            return EOF;
        }
#endif
        int ret = c;
        if( c != EOF )
        {
            if( this->m_flags )
            {
                this->m_pnl->attron( this->m_flags );
                if( ERR == this->m_pnl->addch( (chtype)c ) ) ret = EOF;
                this->m_pnl->attroff( this->m_flags );
            }
            else if( ERR == this->m_pnl->addch( (chtype)c ) ) ret = EOF;
        }
        if( std::isspace(c) || (EOF==c) )
        {
            if( EOF == this->sync() ) ret = EOF;
        }
        return ret;
    }

    int NCStreamBuffer::sync()
    {
        return ( !this->m_pnl || (ERR == this->m_pnl->refresh() ) )
            ? EOF
            :  0;
    }





    char_attr_maps & nc_attr_maps()
    {
        static char_attr_maps bob;
        return bob;
    }

    char_attr_map & nc_attr_map( const std::string & name )
    {
        return nc_attr_maps()[name];
    }


    char_attr_map & nc_current_attr_map()
    {
        return nc_attr_maps()[nc_current_attr_map_name()];
    }


    std::string nc_current_attr_map_name_;
    void nc_current_attr_map_name( const std::string & name )
    {
        nc_current_attr_map_name_ = name;
    }
    std::string nc_current_attr_map_name()
    {
        return nc_current_attr_map_name_;
    }



    void box_and_label( NCWindow & w, const std::string & title )
    {
        w.box();
        w.addstr( 0, 2, title.c_str(), title.size() );
        // w.touchwin();
    }



    void center_window( NCWindow & w )
    {
        w.mvwin( std::min( screen_height()-2, screen_height() / 2 - w.height() / 2 ),
                 std::min( screen_width()-2, screen_width() / 2 - w.width()/2 ) );
        // w.touchwin();
    }

    int screen_width()
    {
        return stdscr->_maxx+1;
    }
    int screen_height()
    {
        return stdscr->_maxy+1;
    }


    int keyhnd_noop( int key )
    {
        return key;
    }

    int keyhnd_zero( int key )
    {
        return 0;
    }

    int read_and_handle_key( const key_handler_map & map, bool retry )
    {
        typedef key_handler_map::const_iterator CIT;
        int key = 0;
        CIT it;
        while( ERR != (key = ::getch()) )
        {
            it = map.find( key );
            if( map.end() == it )
            {
                if( ! retry ) return -1;
                else continue;
            }
            return (*it).second( key );
        }
        return -1;
    }


    void interactively_move_window( NCWindow & win )
    {
        int key;
        bool do_loop = true;
        int x = win.begx();
        int y = win.begy();
        //std::cerr << "start y="<<std::dec<<y<<", x="<<x<<"\n";
        try
        {
            while( do_loop )
            {
                key = win.getch();
                //std::cerr << "key="<<(int)key<<", char="<<(char)key<<"\n";
                if( ERR == key ) break;
                switch( key )
                {
                  case KEY_UP:
                      y -= 1;
                      break;
                  case KEY_DOWN:
                      y += 1;
                      break;
                  case KEY_LEFT:
                      x -= 1;
                      break;
                  case KEY_RIGHT:
                      x += 1;
                      break;
					  
                  default:
                      do_loop = false;
                      break;
                }
                if( ! do_loop ) break;
                //std::cerr << "y="<<std::dec<<y<<", x="<<x<<"\n";
                win.mvwin( y, x );
                win.refresh();
            }
        }
        catch( const NCException & )
        {
            win.refresh();
        }

    }

} // namespace ncutil
