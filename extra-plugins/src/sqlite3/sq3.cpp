#if 0
#  ifndef COUT
#    include <iostream>
#    define COUT std::cerr << "SQ3:"<<__FILE__ << ":" << std::dec<<__LINE__ << ": "
#  endif
#endif

#include "sq3.hpp"
#include <vector>
#include <sstream>

#if SQ3_USE_WCHAR
#include <cwchar>
#endif
#include <cstring>

/**
   If DO_CURSOR_LIFETIME_HACK is true then the statement/cursor classes
   get an extra relationship to try to avoid a cursor stepping on
   a dead statement. In pure C++ this is not a problem because scoping
   is our friend, but this hack was added when this code was used as the
   basis for a JavaScript/sqlite3 binding.
*/
#define DO_CURSOR_LIFETIME_HACK 1

namespace sq3 {

    //typedef refcount::rcptr<sqlite3,sqlite3_finalizer> sqlite3_rcptr;
    //typedef refcount::rcptr<statement,statement_reset_finalizer> statement_reset_rcptr;
    //typedef refcount::rcptr<sqlite3_stmt,sqlite3_stmt_finalizer> stmt_rcptr;

    void statement_reset_finalizer::operator()( ::sq3::statement * & st )
    {
        if( st )
        {
            st->reset();
            st = 0;
        }
    }

    void sqlite3_stmt_reset_finalizer::operator()( sqlite3_stmt * & st )
    {
        if( st )
        {
            sqlite3_reset( st );
            st = 0;
        }
    }

    void sqlite3_stmt_finalizer::operator()( sqlite3_stmt * & st )
    {
        if( st )
        {
            sqlite3_finalize( st );
            st = 0;
        }
    }

    void sqlite3_finalizer::operator()( sqlite3 * & s )
    {
        if( s )
        {
            sqlite3_close( s );
            s = 0;
        }
    }


    bool rc_is_okay( int rc )
    {
        return ((SQLITE_DONE==rc) || (SQLITE_OK==rc) || (SQLITE_ROW==rc));
    }


    int statement::prepare( std::string const & sql )
    {
        return this->prepare( sql.c_str(), static_cast<int>(sql.size()) );
    }

    int statement::prepare( char const * sql, int len )
    {
        // FIXME: make this function clean up any existing sqlite3_stmt.
        const char *tail=NULL;
        if( 0 > len ) len = static_cast<int>(std::strlen(sql));
        sqlite3_stmt * st = 0;
		
        int rc = 
#if (SQLITE_VERSION_NUMBER >= 3003009)
            sqlite3_prepare_v2
#else
            sqlite3_prepare
#endif
            (this->m_db.handle(), sql, len, &st, &tail);
		
        if( SQLITE_OK == rc )
        {
            this->m_argc = sqlite3_column_count(st);
            this->m_stmt.take( st );
        }
        else
        {
            this->finalize();
        }
        return rc;
    }

#if SQ3_USE_WCHAR
    int statement::prepare( sqlite3_wstring_t const sql, int byteCount )
    {
        void const * tail = NULL;
        if( 0 > byteCount ) byteCount = ????;
        sqlite3_stmt * st = 0;
        int rc = 
#if SQLITE_VERSION_NUMBER >= 3003009
            sqlite3_prepare16_v2
#else
            sqlite3_prepare16
#endif
            (this->m_db.handle(), sql, byteCount, &st, &tail);
        if( SQLITE_OK == rc )
        {
            this->m_argc = sqlite3_column_count(st);
            this->m_stmt.take(st);
        }
        else
        {
            this->finalize();
        }
        return rc;

    }
#endif // SQ3_USE_WCHAR


    int statement::finalize()
    {
#if DO_CURSOR_LIFETIME_HACK
        CursorSet curs;
        curs.swap( this->m_curs );
        CursorSet::iterator it = curs.begin();
        bool took = false;
        for( ; curs.end() != it; ++it )
        {
            if( 0 && ! took )
            {
                took = true;
                //(*it)->m_stmt.takeover();
                // ^^^ removes this object from all cursor::RCPtr ownership
                continue;
            }
            (*it)->m_stmt.take(0);
        }
#endif
        int rc = SQLITE_ERROR;
        if( this->m_stmt.get() )
        {
            rc = SQLITE_OK; // we'll fudge a bit here.
            //sqlite3_finalize(this->m_stmt.get());
            this->m_stmt.take(0); // give up ownership.
        }
        return rc;
    }


    statement::statement( database & db )
        : m_db(db), m_stmt(0), m_argc(0)
    {
    }

    statement::statement( database & db, std::string const & sql )
        : m_db(db), m_stmt(0), m_argc(0)
    {
        this->prepare( sql );
    }
    statement::statement( database & db, char const * sql, int byteCount )
        : m_db(db), m_stmt(0), m_argc(0)
    {
        this->prepare( sql, byteCount );
    }

#if SQ3_USE_WCHAR
    statement::statement( database & db, std::wstring const & sql )
        : m_db(db), m_stmt(0), m_argc(0)
    {
        this->prepare( sql );
    }
    statement::statement( database & db, wchar_t const * sql, int byteCount )
        : m_db(db), m_stmt(0), m_argc(0)
    {
        this->prepare( sql, byteCount );
    }
#endif // SQ3_USE_WCHAR


    bool statement::is_prepared() const
    {
        return 0 != this->m_stmt.get();
    }
    statement::~statement()
    {
        this->finalize();
    }
    sqlite3_stmt * statement::handle() const
    {
        return this->m_stmt.get();
    }

    int statement::bind( int index )
    {
        return sqlite3_bind_null( this->m_stmt.get(), index);
    }
    int statement::bind( int index, int data )
    {
        return sqlite3_bind_int( this->m_stmt.get(), index, data );
    }
    int statement::bind( int index, int64_t data )
    {
        return sqlite3_bind_int64( this->m_stmt.get(), index, data );
    }
    int statement::bind( int index, double data )
    {
        return sqlite3_bind_double( this->m_stmt.get(), index, data );
    }
    int statement::bind( int index, char const * data, int len )
    {
        if( 0 > len ) len = static_cast<int>(strlen(data));
        return sqlite3_bind_text( this->m_stmt.get(), index, data, len, SQLITE_TRANSIENT );
    }
    int statement::bind( int index, void const * data, int len )
    {
        return sqlite3_bind_blob( this->m_stmt.get(), index, data, len, SQLITE_TRANSIENT );
    }
    //int statement::bind( int index, wchar_t const * data, int len );
    int statement::bind( int index, std::string const & data )
    {
        return this->bind( index, data.c_str(), static_cast<int>(data.size() ) );
    }
    //int statement::bind( int index, std::wstring const & data );

    int statement::bind( char const * index )
    {
        return sqlite3_bind_null( this->m_stmt.get(),
                                  sqlite3_bind_parameter_index( this->m_stmt.get(), index ) );
    }
    int statement::bind( char const * index, int data )
    {
        return sqlite3_bind_int( this->m_stmt.get(),
                                 sqlite3_bind_parameter_index( this->m_stmt.get(), index ), data );
    }
    int statement::bind( char const * index, int64_t data )
    {
        return sqlite3_bind_int64( this->m_stmt.get(),
                                   sqlite3_bind_parameter_index( this->m_stmt.get(), index ), data );
    }
    int statement::bind( char const * index, double data )
    {
        return sqlite3_bind_double( this->m_stmt.get(),
                                    sqlite3_bind_parameter_index( this->m_stmt.get(), index ), data );
    }
    int statement::bind( char const * index, char const * data, int len )
    {
        if( 0 > len ) len = static_cast<int>(strlen(data) );
        return sqlite3_bind_text( this->m_stmt.get(),
                                  sqlite3_bind_parameter_index( this->m_stmt.get(), index ) , data, len, SQLITE_TRANSIENT );
    }
    int statement::bind( char const * index, void const * data, int len )
    {
        return sqlite3_bind_blob( this->m_stmt.get(),
                                  sqlite3_bind_parameter_index( this->m_stmt.get(), index ) , data, len, SQLITE_TRANSIENT );
    }

    int statement::bind( char const * index, std::string const & data )
    {
        return this->bind( index, data.c_str(), static_cast<int>(data.size() ) );
    }


    cursor statement::get_cursor()
    {
        return cursor(*this);
    }

    int statement::execute()
    {
        return this->get_cursor().step();
    }

#define STATEMENT_EXECUTE_1ARG_IMPL             \
    cursor rd( this->get_cursor() );            \
        int rc = rd.step();                     \
        if( SQLITE_ROW == rc )                  \
        {                                       \
            rc = rd.get(0,tgt);                 \
        }                                       \
        return rc;

    int statement::execute( int & tgt )
    {
        STATEMENT_EXECUTE_1ARG_IMPL;
    }
    int statement::execute( int64_t & tgt )
    {
        STATEMENT_EXECUTE_1ARG_IMPL;
    }
    int statement::execute( double & tgt )
    {
        STATEMENT_EXECUTE_1ARG_IMPL;
    }
    int statement::execute( std::string & tgt )
    {
        STATEMENT_EXECUTE_1ARG_IMPL;
    }
#undef STATEMENT_EXECUTE_1ARG_IMPL

    int statement::execute( sqlite3_text_char_t const ** tgt, int & len )
    {
        cursor rd( this->get_cursor() );
        int rc = rd.step();
        if( SQLITE_ROW == rc )
        {
            rc = rd.get(0,tgt,len);
        }
        return rc;
    }
    int statement::execute( void const ** tgt, int & len )
    {
        cursor rd( this->get_cursor() );
        int rc = rd.step();
        if( SQLITE_ROW == rc )
        {
            rc = rd.get(0,tgt,len);
        }
        return rc;
    }


    int statement::reset()
    {
        return this->m_stmt.get()
            ? sqlite3_reset( this->m_stmt.get() )
            : SQLITE_ERROR;
    }

    int statement::clear_bindings()
    {
        return this->m_stmt.get()
            ? sqlite3_clear_bindings( this->m_stmt.get() )
            : SQLITE_ERROR;
    }

    int statement::colcount()
    {
        return this->m_stmt.get()
            ? this->m_argc
            : -1;
    }

    char const * statement::colname( int index )
    {
        int count = this->colcount();
        if( -1 == count ) return 0;
        if( (index < 0) || (index >= count) ) return 0;
        return sqlite3_column_name(this->m_stmt.get(), index);
    }

    int statement::colname( int index, char const ** cn )
    {
        char const * c = this->colname( index );
        if( c ) *cn = c;
        return c ? SQLITE_OK : SQLITE_ERROR;
    }


    cursor::cursor() : m_stmt(0),m_cn(0)
    {
    }
    cursor::cursor( cursor const & cp ) : m_stmt(0),m_cn(0)
    {
        this->copy(cp);
    }

    cursor::cursor( statement & st ) : m_stmt(&st),m_cn(0)
    {
        //this->m_stmt.take(&st);
#if DO_CURSOR_LIFETIME_HACK
        st.add_cursor(this);
#endif
    }

    cursor & cursor::operator=( cursor const & cp )
    {
        if( &cp == this ) return *this;
        this->copy(cp);
        return *this;
    }

    void cursor::copy( cursor const & rhs )
    {
        if( &rhs == this ) return;
        this->close();
        this->m_stmt = rhs.m_stmt;
#if DO_CURSOR_LIFETIME_HACK
        if( this->m_stmt.get() )
        {
            this->m_stmt->add_cursor( this );
        }
#endif
        if( rhs.m_cn )
        {
            this->m_cn = new NameToIndexMap(*rhs.m_cn);
        }
    }

    cursor::~cursor()
    {
        this->close();
    }

    int cursor::step()
    {
        sqlite3_stmt * st =
            this->m_stmt.get()
            ? (this->m_stmt->m_stmt.get()
               ? this->m_stmt->m_stmt.get()
               : 0)
            : 0;
        return st
            ? sqlite3_step(st)
            : SQLITE_ERROR;
    }

    int cursor::reset()
    {
        delete this->m_cn;
        this->m_cn = 0;
        return this->m_stmt.get()
            ? this->m_stmt->reset()
            : SQLITE_ERROR;
    }
    void cursor::close()
    {
#if DO_CURSOR_LIFETIME_HACK
        if( this->m_stmt.get() )
        {
            this->m_stmt->remove_cursor(this);
        }
#endif
        this->m_stmt.take(0);
        if( this->m_cn )
        {
            delete this->m_cn;
            this->m_cn = 0;
        }
    }

    int cursor::colcount()
    {
        return this->m_stmt.get()
            ? this->m_stmt->colcount()
            : -1;
    }

#define CURSOR_CHECK_INDEX                                              \
    if( ! this->m_stmt.get() ) return SQLITE_ERROR;			\
	if( (index)>(this->m_stmt->m_argc-1)) return SQLITE_ERROR;

    int cursor::isnull( int index, bool & tgt )
    {
        CURSOR_CHECK_INDEX;
        tgt = sqlite3_column_type( this->m_stmt->m_stmt.get(), index) == SQLITE_NULL;
        return SQLITE_OK;
    }

    int cursor::get( int index, int & tgt )
    {
        CURSOR_CHECK_INDEX;
        tgt = sqlite3_column_int( this->m_stmt->m_stmt.get(), index );
        return SQLITE_OK;
    }
    int cursor::get( int index, int64_t & tgt )
    {
        CURSOR_CHECK_INDEX;
        tgt = sqlite3_column_int64( this->m_stmt->m_stmt.get(), index );
        return SQLITE_OK;
    }
    int cursor::get( int index, double & tgt )
    {
        CURSOR_CHECK_INDEX;
        tgt = sqlite3_column_double( this->m_stmt->m_stmt.get(), index );
        return SQLITE_OK;
    }
    int cursor::get( int index, std::string & tgt )
    {
        CURSOR_CHECK_INDEX;
        char const * x = (const char*)sqlite3_column_text(this->m_stmt->m_stmt.get(), index);
        int sz = sqlite3_column_bytes(this->m_stmt->m_stmt.get(), index);
        if( 0 < sz )
        {
            tgt = std::string( x, x+sz );
        }
        else
        {
            tgt = std::string();
        }
        return SQLITE_OK;
    }
    int cursor::get( int index, sqlite3_text_char_t const ** tgt, int & sz )
    {
        CURSOR_CHECK_INDEX;
        sz = sqlite3_column_bytes(this->m_stmt->m_stmt.get(), index);
        if( 0 < sz )
        {
            *tgt = sqlite3_column_text( this->m_stmt->m_stmt.get(), index);
        }
        else
        {
            tgt = 0;
        }
        return SQLITE_OK;
    }
    int cursor::get( int index, void  const ** tgt, int & sz )
    {
        CURSOR_CHECK_INDEX;
        sz = sqlite3_column_bytes(this->m_stmt->m_stmt.get(), index);
        if( 0 < sz )
        {
            *tgt = sqlite3_column_blob(this->m_stmt->m_stmt.get(), index);
        }
        return SQLITE_OK;
    }


    /**
       CURSOR_GET_STRING_IMPL2 is the implementation for the get(string,xxx) family of funcs
       which take 2 arguments.
    */
#define CURSOR_GET_STRING_IMPL2(vaR,targetNamE)                         \
    if( ! this->m_stmt.get() ) return SQLITE_ERROR;                     \
        if( 0 == this->index_colnames() ) return SQLITE_ERROR;          \
        NameToIndexMap::const_iterator n2iit = this->m_cn->find( vaR ); \
        return ( this->m_cn->end() == n2iit ) ? SQLITE_ERROR : this->get( (*n2iit).second, targetNamE );

    /**
       CURSOR_GET_STRING_IMPL3 is the implementation for the get(string,xxx) family of funcs
       which take 3 arguments. It is *almost* identicle to CURSOR_GET_STRING_IMPL2.
    */
#define CURSOR_GET_STRING_IMPL3(vaR,targetNamE,sizeNamE)                \
    if( ! this->m_stmt.get() ) return SQLITE_ERROR;                     \
        if( 0 == this->index_colnames() ) return SQLITE_ERROR;          \
        NameToIndexMap::const_iterator n2iit = this->m_cn->find( vaR ); \
        return ( this->m_cn->end() == n2iit ) ? SQLITE_ERROR : this->get( (*n2iit).second, targetNamE, sizeNamE );


    int cursor::get( std::string const & key, int & tgt )
    {
        CURSOR_GET_STRING_IMPL2(key,tgt);
    }
    int cursor::get( std::string const & key, int64_t & tgt )
    {
        CURSOR_GET_STRING_IMPL2(key,tgt);
    }
    int cursor::get( std::string const & key, double & tgt )
    {
        CURSOR_GET_STRING_IMPL2(key,tgt);
    }
    int cursor::get( std::string const & key, std::string & tgt )
    {
        CURSOR_GET_STRING_IMPL2(key,tgt);
    }
    int cursor::get( std::string const & key, sqlite3_text_char_t const ** tgt, int & sz )
    {
        CURSOR_GET_STRING_IMPL3(key,tgt,sz);
    }
    int cursor::get( std::string const & key, void  const ** tgt, int & sz )
    {
        CURSOR_GET_STRING_IMPL3(key,tgt,sz);
    }


    int cursor::colname( int index, std::string & tgt )
    {
        char const * cn = 0;
        int rc = this->colname( index, &cn );
        if( SQLITE_OK == rc )
        {
            tgt = cn ? cn : "";
        }
        return rc;
    }

    int cursor::colname( int index, char const ** cn )
    {
        return this->m_stmt->colname( index, cn );
    }


#undef CURSOR_CHECK_INDEX
#undef CURSOR_GET_STRING_IMPL2
#undef CURSOR_GET_STRING_IMPL3

    sqlite3 * database::handle() const
    {
        return this->m_dbh.get();
    }
    database::database() : m_dbh(0), m_name()
    {
    }


    database::database( std::string const & dbname )
        : m_dbh(0), m_name(dbname)
    {
        this->open( dbname );
    }

    database::~database()
    {
        this->close();
    }




    bool database::is_open() const
    {
        return 0 != this->m_dbh.get();
    }
    std::string database::name() const
    {
        return this->m_name;
    }

    sqlite3 * database::take_handle()
    {
        return this->m_dbh.takeover();
    }

    void database::take_handle( sqlite3 * dbh, std::string const & name )
    {
        if( this->m_dbh.get() == dbh ) return;
        this->close();
        this->m_name = name;
        this->m_dbh.take( dbh );
    }

    std::string database::errormsg() const
    {
        char const * m = this->m_dbh.get() ? sqlite3_errmsg( this->m_dbh.get() ) : 0;
        return m ? m : "";
    }

    int database::open( char const * dbn, long flags )
    {
        if( ! dbn ) return SQLITE_ERROR;
        int rc = 0;
        if( this->m_dbh.get() )
        {
            rc = this->close();
            if( 0 != rc ) return rc;
        }
        this->m_name = dbn;
        sqlite3 * sq = 0;
#if (SQLITE_VERSION_NUMBER >= 3005001)
        if( ! flags )
        {
            rc = sqlite3_open(dbn, &sq);
        }
        else
        {
            rc = sqlite3_open_v2( dbn, &sq, flags, NULL );
        }
#else
        { int bogus; bogus = flags; } // avoid "unused variable: flags" warning from gcc
        rc = sqlite3_open(dbn, &sq);
#endif // sqlite3 >= 3.5.1
        if( SQLITE_OK == rc )
        {
            this->m_dbh.take( sq );
            rc = this->on_open();
        }
        if( SQLITE_OK != rc )
        {
            this->close(); // ingore any close() failure in this case
        }
        return rc;
    }

    int database::open( std::string const & dbn, long flags )
    {
        return this->open( dbn.c_str(), flags );
    }

    int database::on_open()
    {
        return SQLITE_OK;
    }

#if SQ3_USE_WCHAR
    int database::open( sqlite3_wstring_t dbn )
    {
        //std::wcerr << L"database::open(wchar_t " << dbn << L")\n";
        this->close();
        int rc = sqlite3_open16(dbn, &this->m_db);
        sqlite3 * sq = 0;
        sqlite3_open(dbn, &sq);
        if( SQLITE_OK == rc )
        {
            this->m_dbh.take( sq );
            rc = this->on_open();
        }
        if( SQLITE_OK != rc )
        {
            this->close();
        }
        return rc;
    }

    int database::open( std::wstring const & dbn )
    {
        return this->open( dbn.c_str() );
    }
#endif // SQ3_USE_WCHAR

    int database::close( bool force )
    {
        if(0 == this->m_dbh.get()) return SQLITE_ERROR;
        if( force )
        { // do immediately close:
            return sqlite3_close( this->m_dbh.takeover() /* transfer ownership to sqlite3 */ );
        }
        else
        { // --reference_count and queue up the close:
            this->m_dbh.take(0); // drop existing handle.
            return SQLITE_OK;
        }
    }

    int64_t database::insertid()
    {
        return this->m_dbh.get()
            ? sqlite3_last_insert_rowid(this->m_dbh.get())
            : -1;
    }

    int database::changes()
    {
        return this->m_dbh.get()
            ? sqlite3_changes(this->m_dbh.get())
            : -1;
    }

    int database::setbusytimeout( int ms )
    {
        return this->m_dbh.get()
            ? sqlite3_busy_timeout(this->m_dbh.get(), ms)
            : SQLITE_ERROR;
    }

    int database::execute(const std::string &sql)
    {
        return statement( *this, sql ).execute();
    }

    int database::execute(char const * sql )
    {
        return statement( *this, sql, -1 ).execute();
    }

    int database::execute(char const * sql, int & tgt)
    {
        return statement( *this, sql ).execute( tgt );
    }

    int database::execute(std::string const & sql, int & tgt)
    {
        return statement( *this, sql ).execute( tgt );
    }

    int database::execute(char const * sql, int64_t & tgt)
    {
        return statement( *this, sql, -1 ).execute( tgt );
    }

    int database::execute(std::string const & sql, int64_t & tgt)
    {
        return statement( *this, sql ).execute( tgt );
    }

    int database::execute(char const * sql, double & tgt)
    {
        return statement( *this, sql, -1 ).execute( tgt );
    }

    int database::execute(std::string const & sql, double & tgt)
    {
        return statement( *this, sql ).execute( tgt );
    }

    int database::execute(char const * sql, std::string & tgt)
    {
        return statement( *this, sql ).execute( tgt );
    }

    int database::execute(std::string const & sql, std::string & tgt)
    {
        return statement( *this, sql ).execute( tgt );
    }

    int database::execute(char const * sql, sqlite3_text_char_t const ** tgt, int & len )
    {
        return statement( *this, sql ).execute( tgt, len );
    }

    int database::execute(std::string const & sql, sqlite3_text_char_t const ** tgt, int & len )
    {
        return statement( *this, sql ).execute( tgt, len );
    }

    // 		int database::execute(char const * sql, std::wstring & tgt);
    // 		int database::execute(std::string const & sql, std::wstring & tgt);
    int database::execute(char const * sql, void const ** tgt, int & len )
    {
        return statement( *this, sql ).execute( tgt, len );
    }
    int database::execute(std::string const & sql, void const ** tgt, int & len )
    {
        return statement( *this, sql ).execute( tgt, len );
    }

    int database::execute( std::string const & sql, sqlite3_callback callback, void * data, std::string & errmsg )
    {
        return this->execute( sql.c_str(), callback, data, errmsg );
    }

    int database::execute( char const * sql, sqlite3_callback callback, void * data, std::string & errmsg )
    {
        char * cerrmsg = 0;
        int ret = 0;
        try
        {
            // allow callback to safely throw.
            ret = sqlite3_exec( this->m_dbh.get(), sql, callback, data, &cerrmsg );
        }
        catch( ... )
        {
            if( cerrmsg )
            {
                errmsg = cerrmsg;
                sqlite3_free( cerrmsg );
            }
            throw;
        }
        if( cerrmsg )
        {
            errmsg = cerrmsg;
            sqlite3_free( cerrmsg );
        }
        return ret;
    }

    int database::execute( std::string const & sql, sqlite3_callback callback, void * data )
    {
        std::string ignored;
        return this->execute( sql, callback, data, ignored );
    }

    int database::execute( char const * sql, sqlite3_callback callback, void * data )
    {
        std::string s( sql ? sql : "" );
        std::string ignored;
        return this->execute( s, callback, data, ignored );
    }


    int database::pragma( char const * code )
    {
        std::ostringstream os;
        os << "pragma " << code;
        std::string sql( os.str() );
        return this->execute( sql.c_str() );
    }

    int database::vacuum()
    {
        return this->execute( "vacuum" );
    }

    bool database::interrupt()
    {
        return (0 == this->m_dbh.get())
            ? false
            : (sqlite3_interrupt(this->m_dbh.get()), true);
    }

    int database::clear()
    {
        if( ! this->is_open() )
        {
            return SQLITE_ERROR;
        }
        char const * parts[] = { "view", "trigger", "table", 0 };
        std::string name;
        typedef std::vector<std::string> CmdList;
        CmdList list;
        int rc = SQLITE_OK;
        for( int i = 0; i < 3; ++i )
        {
            statement master(*this,"select name from sqlite_master where type=? and name not like 'sqlite_%'");
            rc = master.bind( 1, parts[i] );
            if( ! rc_is_okay(rc) ) return rc;
            cursor cur(master.get_cursor());
            while( SQLITE_ROW == cur.step() )
            {
                name = "";
                rc = cur.get(0, name);
                if( ! rc_is_okay(rc) ) return rc;
                list.push_back( std::string("drop ") + parts[i] + "'" + name + "'" );
            }
        }
        CmdList::const_iterator it = list.begin();
        CmdList::const_iterator et = list.end();
        for( ; et != it; ++it )
        {
            std::string cmd = *it;
            rc = this->execute( cmd );
        }
        return rc;
    }









    transaction::transaction( database & db, bool start )
        : m_db(db), m_intrans(false)
    {
        if( start ) this->begin();
    }

    transaction::transaction( database & db,
                              transaction::TransactionModes m,
                              bool start )
        : m_db(db), m_intrans(false), m_mode(m)
    {
        if( start ) this->begin();
    }

    transaction::~transaction()
    {
        this->rollback();
    }

    int transaction::begin()
    {
        if( this->m_intrans )
        {
            return SQLITE_ERROR;
        }
        std::ostringstream buf;
        buf << "begin";
        switch( this->m_mode )
        {
          case Exclusive:
              buf << " exclusive"; break;
          case Deferred:
              buf << " deferred"; break;
          case Immediate:
              buf << " immediate"; break;
          default:
              break;
        };
        buf << ';';
        int rc = this->m_db.execute(buf.str().c_str());
        this->m_intrans = (SQLITE_DONE == rc) || (SQLITE_OK == rc);
        return rc;
    }

    int transaction::commit()
    {
        if( ! this->m_intrans )
        {
            return SQLITE_ERROR;
        }
        int rc = this->m_db.execute("commit");
        if( SQLITE_BUSY != rc )
        {
            // According to the sqlite3 docs, if a COMMIT fails with BUSY
            // then the transaction is still open.
            this->m_intrans = false;
        }
        return rc;
    }
    int transaction::rollback()
    {
        if( ! this->m_intrans )
        {
            return SQLITE_ERROR;
        }
        this->m_intrans = false;
        return this->m_db.execute("rollback");
    }

    int cursor::index_colnames()
    {
        if( ! this->m_cn )
        {
            this->m_cn = new NameToIndexMap;
        }
        else
            if( ! this->m_cn->empty() )
            {
                // We're using a cached result
                return -1;
            }
        char const * cname;
        int cc = this->colcount();
        int pos = 0;
        for( int i = 0; i < cc; ++i, ++pos )
        {
            cname = 0;
            if( SQLITE_OK != this->colname( i, &cname ) )
                //if( ! (cname = this->m_stmt->colname( i ) ) )
            {
                break;
            }
            (*this->m_cn)[std::string(cname ? cname : "")] = i;
        }
        return pos;
    }


} // namespace
