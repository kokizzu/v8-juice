#ifndef s11n_net_SQ3_HPP_INCLUDED
#define s11n_net_SQ3_HPP_INCLUDED 1

#include <string>
#include <map>
#include <set>
#include "sqlite3.h"

#ifndef SQ3_USE_WCHAR
#  define SQ3_USE_WCHAR 0
#endif

// Enable WCHAR support when it's there. Thanks to Artem Gr <artem@bizlink.ru>
// for this. However, sqlite3's wchar code requires that a wchar is 16-bit, which
// is not the case on gcc/linux (32-bit).
#ifndef SQ3_USE_WCHAR
#  ifdef _GLIBCXX_USE_WCHAR_T
#    define SQ3_USE_WCHAR 1
#  elif defined(UNICODE) // Windows uses this
#    define SQ3_USE_WCHAR 1
#  else
#    define SQ3_USE_WCHAR 0 // default
#  endif
#endif

#if SQ3_USE_WCHAR
#  warning SQ3_USE_WCHAR: INCOMPLETE/BROKEN code is enabled!
#endif


// #ifndef COUT
// #include <iostream>
// #define COUT std::cerr << "SQ3:"<<__FILE__ << ":" << std::dec<<__LINE__ << ": "
// #endif

// The refcount API provides reference-counted pointers for sqlite3
// handles. It doesn't belong in the public API but is required in this
// header because classes which use it will require a complete type.
#include "refcount.hpp"



/**
   The sq3 namespace encapsulates an OO sqlite3 API very similar to
   the sqlite3x API, but this one uses no exception handling (i.e., it
   doesn't throw on errors). It is intended to be an alternative for
   platforms/projects where exceptions are not desired or not
   available.

   Wide-char support does not currently work. (On my platform
   (gcc/linux) wchar_t is 4 bytes and sqlite3 wants 2-bytes wide chars
   for UTF16 text.)

   This code was written by stephan beal (stephan@s11n.net) and is
   released into the Public Domain. It was modelled heavily after code
   written by Cory Nelson, but was reimplemented from scratch for use
   on a PocketPC platform where throwing exceptions wasn't allowed.

   This code's home page is:

   http://s11n.net/sqlite/

   Change history (only "significant" changes documented here):

   

   - 2007.02.26: integrated isnull() patch from  
   Xose Anton Otero Ferreira <xoseotero at gmail com>

   - 2007.01.22: sq3::reader class renamed to sq3::cursor.
   Renamed cursor::read() to cursor::step().

   - 2007.01.27: Added the cursor::get(std::string const&,...)
   family of functions to enable get-by-string-index. Added
   database::clear().

   - 2007.01.28: rcptr<> shared pointer class introduced to enable
   reasonable copy semantics for the major underlying data structures.
   sq3::database and sq3::statement can now be shallowly copied.

   - 2007.02.14: added statement::bind(char const *,...) functions.

*/
namespace sq3 {

    /**
       The type used for signed 64-bit integer operations.
    */
    typedef sqlite_int64 int64_t;
    /**
       The type used for unsigned 64-bit integer operations.
    */
    typedef sqlite_uint64 uint64_t;

    // 	/**
    // 	   The published result codes from the sqlite3 API.
    // 	*/
    // 	enum Sqlite3ResultCodes {
    // 	ResultOK = SQLITE_OK,
    // 	ResultERROR = SQLITE_ERROR,
    // 	ResultINTERNAL = SQLITE_INTERNAL,
    // 	ResultPERM = SQLITE_PERM,
    // 	ResultABORT = SQLITE_ABORT,
    // 	ResultBUSY = SQLITE_BUSY,
    // 	ResultLOCKED = SQLITE_LOCKED,
    // 	ResultNOMEM = SQLITE_NOMEM,
    // 	ResultREADONLY = SQLITE_READONLY,
    // 	ResultINTERRUPT = SQLITE_INTERRUPT,
    // 	ResultIOERR = SQLITE_IOERR,
    // 	ResultCORRUPT = SQLITE_CORRUPT,
    // 	ResultNOTFOUND = SQLITE_NOTFOUND,
    // 	ResultFULL = SQLITE_FULL,
    // 	ResultCANTOPEN = SQLITE_CANTOPEN,
    // 	ResultPROTOCOL = SQLITE_PROTOCOL,
    // 	ResultEMPTY = SQLITE_EMPTY,
    // 	ResultSCHEMA = SQLITE_SCHEMA,
    // 	ResultTOOBIG = SQLITE_TOOBIG,
    // 	ResultCONSTRAINT = SQLITE_CONSTRAINT,
    // 	ResultMISMATCH = SQLITE_MISMATCH,
    // 	ResultMISUSE = SQLITE_MISUSE,
    // 	ResultNOLFS = SQLITE_NOLFS,
    // 	ResultAUTH = SQLITE_AUTH,
    // 	ResultROW = SQLITE_ROW,
    // 	ResultDONE = SQLITE_DONE
    // 	};

    /**
       A char type used by some of the sqlite3 API to represent
       text data. This is really annoying, but sqlite3's API
       explicitely uses UNSIGNED char arrays for a couple of
       strings, while using signed char arrays for almost
       everything else.
    */
    typedef char unsigned sqlite3_text_char_t;


    class statement; // unfortunate fwd decl

    /**
       A specialized dtor to close sqlite3 handles, for use
       with rcptr<sqlite3,sqlite3_finalizer >.
    */
    struct sqlite3_finalizer
    {
        /**
           Calls sqlite3_close(t) and assigns t to 0.
        */
        void operator()( sqlite3 * & t );
    };

    /**
       A specialized dtor to reset (not close)
       sq3::statement objects, for use with
       rcptr<sqlite3,statement_reset_finalizer>.
    */
    struct statement_reset_finalizer
    {
        /**
           Calls t->reset() and assigns t to 0.
        */
        void operator()( ::sq3::statement * & t );
    };

    /**
       A specialized dtor to call reset sqlite3_stmt
       handles(), for use with
       rcptr<sqlite3,sqlite3_stmt_reset_finalizer>.
    */
    struct sqlite3_stmt_reset_finalizer
    {
        /**
           Calls sqlite3_reset(t) and assigns t to 0.
        */
        void operator()( sqlite3_stmt * & t );
    };


    /**
       A specialized dtor to finalize sqlite3_stmt
       handles, for use with
       rcptr<sqlite3,sqlite3_stmt_finalizer>.
    */
    struct sqlite3_stmt_finalizer
    {
        /**
           Calls sqlite3_finalize(t) and assigns t to 0.
        */
        void operator()( sqlite3_stmt * & t );
    };

    /**
       rc_is_okay() is an easy way to check if rc is one of
       SQLITE_OK, SQLITE_ROW, or SQLITE_DONE.  This function
       returns true if rc is one of those values, else false.
       (Code which accepts arbitrary SQL from a user often has to
       accept any of those three result codes as success.)
    */
    bool rc_is_okay( int rc );


    /**
       Encapsulates a connection to an sqlite database.

       This type is virtual/subclassable so that clients can add
       initialization routines to all of their databases, such as
       adding sqlite-builtin functions and collating sequences.
       Simply do such initializations in your subclass ctors.

       A note on the lack of proper constness for much of this
       API...  Potentially, in the face of triggers, multiple
       threads, and whatnot, it is not generically possible to
       100% ensure that any given SQL statement does *not* modify
       the database in at least *some* way. To reflect this
       underlying state of flux, "it was decided" (it really was)
       that very little, if any, of the members of this class
       would be const. Only those which could 100% guaranty proper
       constness.

       Notes about copying:

       Copying a db object is actually shallow copying. All copies
       of this type will refer to the same underlying (sqlite3*)
       db handle until/unless:

       - When open() is called, the object on which it was
       called may separate itself from the rcptr relationship with
       the older db handle and start a new one.

       - close(false) affects only the calling db. When the
       reference count for the (sqlite3*) handle drops to zero,
       then sqlite3_close() will be called. close(true) closes the
       db handle immediately, affecting all copies of this object.

       - When take_handle() is
       called then ownership of the underlying db handle is removed from ALL
       copies of this object. They will still refer to the handle, but using it
       is not legal.
    */
    class database
    {
    private:
        friend class statement;
        typedef rcptr<sqlite3,sqlite3_finalizer> RCPtr;
        mutable RCPtr m_dbh;
        std::string m_name;
    public:

        /**
           The low-level handle to the sqlite db.
           NEVER close this handle. It is permissible
           to use it to run queries, add functions
           to the db, etc.

           This object retains ownership of the returned
           handle.
        */
        sqlite3 * handle() const;

        /**
           Creates an unopened database. Use open()
           to open a file or take_handle() to transfer
           an existing db handle to this object.
        */
        database();

        /**
           dbh is assumed to be an opened, valid db handle.
           This function transfers ownership of dbh to this
           object. Specifically, dbh will be closed when the
           last database with the same db handle goes out of
           scope or is closed. The name is simply informative,
           and may or may not be the same actually used for
           opening dbh.

           Note that this function does not call the protected
           this->on_open() because, quite frankly, i'm not yet
           sure if it really makes sense to do so.
        */
        void take_handle( sqlite3 * dbh, std::string const & name = "" );

        /**
           Transfers ownership of this->handle() to the
           caller.  ALL copies of this object which point to
           the handle, except for this copy, still have a
           pointer to that handle but will not call the handle
           dtor when they go out of scope or are closed.
        */
        sqlite3 * take_handle();

        /**
           Opens/creates the given db file. Use is_open() to
           see if the opening worked.

           Subclasses should not call this from a ctor
           initialization list because this function may
           eventually call on_open(), which is virtual, but
           the subclass part of the class may not be in place
           to catch that virtual call. So subclasses should
           initialize with the no-arg parent class ctor
           and should then call open(filename) themselves.
        */
        explicit database( std::string const & filename );

        /**
           Closes this db.
        */
        virtual ~database();

        /**
           Returns true if this db is opened.
           Does not detect errors such as opening
           a non-db file.
        */
        bool is_open() const;

        /**
           Returns the name of the db file.
        */
        std::string name() const;

        /**
           Returns the last error message from
           sqlite, or an empty string if this object
           is not opened. Unfortunately, sqlite3
           returns the literal string "not an error"
           on non-errors, instead of returning an empty
           string.
        */
        std::string errormsg() const;

        /**
           Creates/opens the given db file.

           The flags parameter is only used if this code is
           compiled against sqlite3 >= 3.5.1, and can
           theoretically take any values defined in the
           SQLITE_OPEN_xxx family of macros. The sqlite3
           documentation only describes the use of
           SQLITE_OPEN_READONLY, SQLITE_OPEN_READWRITE, and
           SQLITE_OPEN_CREATE, thus other values may or may
           not work. If (0==flags) then
           (SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE) is
           assumed.  See sqlite3_open_v2() for the exact
           semantics.

           On success it returns SQLITE_OK, or some other
           value on error.

           Calling open() will implicitly call close() on any
           existing connection. If that close() fails then
           this open() will also fail. If the open() succeeds,
           it does NOT affect other (shallow) copies of this
           object: they will still refer to the older db
           handle.
		   
           Note that sqlite3 supports the special db name
           ":memory:" to represent an in-memory database. Such
           databases cannot be saved directly to disk and are
           lost when this object closes the db.

           Windows users beware: according to the sqlite3
           documentation, the db name MUST be in UTF8 format,
           regardless of the current codepage used by Windows,
           and it is the caller's responsibility to perform
           any conversion, if needed.

           Internal notes:

           If the underlying sqlite3_open() succeeds, the
           protected member on_open() in called. If it returns
           a value other than SQLITE_OK then this->close() is
           called and the value returned from on_open() is
           returned from this function.

           Subclasses which override this function and do not
           want to call the base implementation should call
           on_open() when done to allow subclasses to
           initialize the database if they like. If on_open()
           fails then this->close() should be called to free
           up the resources and mark this object as unopened.

           The flags argument was added 20071018, suggested by
           Joaquim Campos Salles (Joaquim.Salles at
           br.unisys.com).

        */
        virtual int open( char const *, long flags = 0 );

        /**
           Functionally identical to open(char const*,long).
           This function calls that one, so subclasses wishing
           to change open()'s behaviour need only reimplement
           that one.
        */
        int open( std::string const &, long flags = 0 );

        /**
           "Closes" this db. That actually means that
           it queues it to be closed when the last
           database object which is using that db handle
           closes or goes out of scope.

           The force parameter changes the handling of those
           sqlite3_close():

           If force is false, this function always returns
           SQLITE_OK unless this object is already closed, in
           which case SQLITE_ERROR is returned but can almost
           certainly be safely ignored. Unfortunately, due to
           the asynchronous nature of this operation, we can't
           return the value from the actual sqlite3_close()
           call (if any) when force is set to false.

           If force is true then the internal reference
           counting is anulled and the db handle is closed
           immediately. This affects all copies of this
           object, so use with care (but use if you must).  In
           this case, the value of sqlite3_close() is
           returned, but the the exact state of the underlying
           database handle is ambiguously defined in the
           sqlite3 docs. So... if that happens then the
           underlying db handle is assumed to be invalid, since
           the "test*.c" files which come with sqlite3 seem
           to treat it as such.
        */
        int close( bool force = false );



        /**
           Returns the rowid of the most recently inserted row
           on this db.
        */
        int64_t insertid();

        /**
           Returns the number of database rows that were
           changed (or inserted or deleted) by the most recently
           completed INSERT, UPDATE, or DELETE statement.

           SQLite implements the command "DELETE FROM table"
           without a WHERE clause by dropping and recreating
           the table. To get an accurate count of the number
           of rows deleted, use "DELETE FROM table WHERE 1"
           instead.
        */
        int changes();

        /**
           See sqlite3_busy_timeout().
        */
        int setbusytimeout( int ms );
		
        /**
           Functionally identical to execute(char const *).
        */
        int execute(const std::string &sql);

        /**
           Executes a statement which is assumed to have
           a single step and a void result. Returned result
           is that of an underlying call to sqlite3_step(),
           which means that SQLITE_DONE or SQLITE_ROW evaluate
           to success.
        */
        int execute(char const * sql);

        /**
           Executes a statement which is assumed to have
           a single result step and an integer result.
           On success, tgt will be set to the query's
           value. Typically one of SQLITE_ROW or SQLITE_DONE
           are returned on success.
        */
        int execute(std::string const & sql, int & tgt);

        /**
           See execute( std::string, int & );
        */
        int execute(char const * sql, int & tgt);

        /**
           See execute( std::string, int & );
        */
        int execute(char const * sql, int64_t & tgt);

        /**
           See execute( std::string, int & );
        */
        int execute(std::string const & sql, int64_t & tgt);

        /**
           See execute( std::string, int & );
        */
        int execute(char const * sql, double & tgt);
        /**
           See execute( std::string, int & );
        */
        int execute(std::string const & sql, double & tgt);

        /**
           See execute( std::string, int & );
        */
        int execute(char const * sql, std::string & tgt);
        /**
           See execute( std::string, int & );
        */
        int execute(std::string const & sql, std::string & tgt);

        /**
           See execute( std::string, int & );

           sql is executed and the string result is written to
           tgt and the length of the result string (in bytes)
           is written to len. The text bytes are owned by
           sqlite and will likely become invalid on the next
           db cursor operation, so copy them if you need them.

           Returns the result of stepping through a result
           set, which is typically one of SQLITE_ROW or
           SQLITE_DONE.
        */
        int execute(char const * sql, sqlite3_text_char_t const ** tgt, int & len );
        /**
           See execute( char const *, sqlite3_text_char_t const **, int & ).
           This function is identical.
        */
        int execute(std::string const & sql, sqlite3_text_char_t const ** tgt, int & len );


        /**
           See execute( char const *, sqlite3_text_char_t **, int & ).
           This function is identical except that tgt is a
           void pointer.
        */
        int execute(std::string const & sql, void const ** tgt, int & sz );
        /**
           See execute( char const *, sqlite3_text_char_t **, int & ).
           This function is identical except that tgt is a
           void pointer.
        */
        int execute(char const * sql, void const ** tgt, int & sz );


        /**
           Executes the given query, calling the given callback function for each
           row of the result set. The data pointer is passed on as-is to the callback.
           Any error string is written to errmsg.

           Return value is that of an underlying sqlite3_exec() call.
        */
        int execute( std::string const & sql, sqlite3_callback callback, void * data, std::string & errmsg );
        /**
           Identical to the execute(std::string,sqlite3_callback,void*,std::string&).
        */
        int execute( char const * sql, sqlite3_callback callback, void * data, std::string & errmsg );

        /**
           Convenience overload which has a default data value
           of 0 and ignores any error string passed back by
           sqlite3_exec().
        */
        int execute( std::string const & sql, sqlite3_callback callback, void * data = 0 );
        /**
           Convenience overload which has a default data value
           of 0 and ignores any error string passed back by
           sqlite3_exec().
        */
        int execute( char const * sql, sqlite3_callback callback, void * data = 0 );

        /**
           This is a convenience wrapper for execute( "pragma ..." ).
           Return value is that of the underlying execute() call.

           code should be a pragma key or key/value string, such as
           "temp_store=MEMORY" or "synchronous=OFF"
        */
        int pragma( char const * code );

        /**
           Convenience wrapper around execute("vacuum").  The
           vacuum operation attempts to free up any unused
           disk space in the database.
        */
        int vacuum();

#if SQ3_USE_WCHAR
        int open( wchar_t const * dbname );
        int open( std::wstring const & dbname );
        // 		int execute(char const * sql, std::wstring & tgt);
        // 		int execute(std::string const & sql, std::wstring & tgt);
#endif // SQ3_USE_WCHAR

        /**
           Looks through sqlite_master for a list of views,
           triggers, and tables, and drops them all (in that
           order). Subclasses are welcomed to reimplement it
           to do less destructive cleansing, such as just
           dropping data from a certain table.

           Returns SQLITE_OK on success.

           If you need to free up the newly-freed space, be
           sure to call vacuum(), or else the file size may
           not actually shrink.

           Also remember that any code which is expecting data
           to be in this database will not work after this function
           is done!
        */
        virtual int clear();

        /**
           If this db is closed, the function returns false, otherwise
           it calls sqlite3_interupt() and returns true.
        */
        bool interrupt();

    protected:
        /**
           This function is called when open() succeeds. The
           default implementation does nothing and always
           returns SQLITE_OK, but subclasses may wish to do
           something here.  If this function returns any value
           other than SQLITE_OK then this->close() is called
           before open() returns.

        */
        virtual int on_open();

	
    };

    /**
       This type represents a transaction block in an SQL
       session. Note that as of this writing, sqlite3 does
       not support nested transactions.
    */
    class transaction
    {
    private:
        transaction & operator=( transaction const & ); // unimplemented
        transaction(  transaction const & ); // unimplemented
        database & m_db;
        bool m_intrans;
        int m_mode;
    public:
        /**
           Enumeration of supported transaction modes.
        */
        enum TransactionModes {
        Exclusive = 0x01,
        Deferred = 0x02,
        Immediate = 0x03
        };

        /**
           Creates a transaction for the given db. If start
           is true (the default) then this->begin() is called.
        */
        transaction( database & db, bool start = true );

        /**
           Creates a transaction for the given db, using the
           given transaction mode. If start is true (the
           default) then this->begin() is called.
        */
        transaction( database & db, TransactionModes m, bool start = true );

        /**
           Calls this->rollback()
        */
        ~transaction();
        /**
           Starts the transaction. Return value is the result
           of calling sqlite3_exec(db,"begin;"). If SQLITE_OK
           is returned then this object considers itself to be
           active, such that calling commit() and rollback()
           should be able to succeed.

           The behaviour of sqlite's db locking of the
           transaction can be changed by passing a transaction
           mode to the constructor. See the sqlite3
           documentation for complete details. The defaul is
           to not specify a locking preference.
        */
        int begin();
        /**
           Commits the active transaction. Returns SQLITE_OK
           on success and any other value on error.  Return
           value is that of the underlying sqlite3_exec()
           call.

           After calling this function, this object is considered to
           NOT be in a transaction unless SQLITE_BUSY is returned.
           If that happens, the transaction is still open and commit()
           should be called later to close the transaction.
        */
        int commit();
        /**
           Initiates a rollback and returns the result of
           executing a rollback command. If this object is not
           active (begin() has not been called) then this
           function returns SQLITE_ERROR and has no
           side-effects. Return value is that of the
           underlying sqlite3_exec() call.
        */
        int rollback();
    };

    /**
       This type is for stepping through a db query result.
       Clients do not normally create cursors directly, but
       through the statement::get_cursor()
       function.

       cursor objects are copied shallowly - each copy points
       back to a single original statement object. That statement
       is reset when the last of these copies goes out of scope
       or is finalized.

    */
    class cursor
    {
    private:

        typedef rcptr<statement,statement_reset_finalizer> RCPtr;
        RCPtr m_stmt;
        /**
           And internal helper type for fetching data sets by
           string lookups instead of integer indexes.
        */
        typedef std::map<std::string,int> NameToIndexMap;
        NameToIndexMap * m_cn; // maps column names to column indexes for use with get().


        /**
           If rhs is this object, this function does nothing,
           otherwise it copies rhs.
        */
        void copy( cursor const & rhs );
        /**
           Hashes the colname-to-index mapping.
           Returns:

           -1: result was cached before and will stay cached until
           this object expires.

           0: this result set has no columns. This is likely an error.

           1 or higher: the number of column indexes.

           As a side-effect, this function initializes this->m_cn
           if that object has not already been created.

           If the return value is anything other than this->colcount()
           then an error occurred during the collection of the
           column names. This is exceedingly unlikely to happen.
        */
        int index_colnames();
        friend class statement;
    public:
        /**
           Creates an empty cursor, whose only valid use is to
           assign it from another cursor.
        */
        cursor();
        /**
           Copies rhs. This object and rhs now point to the
           same underlying sqlite3 structures, and modifying
           one of these objects modifies the other, in effect.
        */
        cursor( cursor const & rhs );
        /**
           See the copy ctor.
        */
        cursor & operator=( cursor const & );

        /**
           Identical to calling st.get_cursor().
        */
        cursor( statement & st );

        /**
           A curious side-effect which one needs to be
           aware of but very rarely is an issue:

           When cursors are created they *always* have an
           associated prepared statement. When the last cursor
           with a reference to that same statement goes out of
           scope or is close()ed then the underlying statement
           object is reset(). That sounds curious, but is
           indeed the desired behaviour for this class, and
           breaks some common client code constructs when the
           underlying statement is not automatically reset.
           Without this "feature", client code could not run,
           for exaple, myStatement.execute( myInt ), two times
           in a row because the second time around the
           statement would be at its end point and need to be
           manually reset. Thus client code should never mix
           the use of a cursor object and the non-cursor
           statement API on the same statement.

           See statement::close() for more details.
        */
        ~cursor();

        /**
           Uses sqlite3_step() to step through this object's
           data set by one step. Returns the result of
           sqlite3_step(), which means: SQLITE_ROW if it
           read, SQLITE_DONE at the end of a data set,
           and any other value on error.
        */
        int step();
        /**
           This is functionally the same as calling reset on
           the underlying prepared statement object to which
           this cursor is tied. Use with care, as it affects
           all cursors which point to the same statement
           object.

           returns SQLITE_OK on success, else another sqlite3
           error code.
        */
        int reset();

        /**
           "Disconnects" this object from the underlying
           result set, making this object useless for anything
           but as the target of an assignment.

           It is normally not necessary to call this function,
           but it may be in some special cases.
        */
        void close();

        /**
           Returns the column count of the underlying
           prepared statement. May return 0 for queries which
           has no return value (e.g. UPDATE). Returns -1
           on error.
        */
        int colcount();

        /**
           If column index (0-based) is in bounds then this
           function check if the value of the given column index
           is NULL and assigns tgt to the result of this
           comprobation. On success, SQLITE_OK is returned.
           On any other return value, tgt is not modifed.
        */
        int isnull( int index, bool & tgt );

        /**
           If column index (0-based) is in bounds then this
           function assigns tgt to the value of the given
           column index. On success, SQLITE_OK is returned.
           On any other return value, tgt is not modifed.
        */
        int get( int index, int & tgt );
        /**
           See get(int,int&).
        */
        int get( int index, int64_t & tgt );
        /**
           See get(int,int&).
        */
        int get( int index, double & tgt );
        /**
           See get(int,int&).
        */
        int get( int index, std::string & tgt );
        /**
           If index (0-based) is in bounds, this function
           gets the (char unsigned const *) data at that
           column index and assigns tgt to that value
           and sz to the size of the data.

           tgt is written to by this func but ownership
           of the underlying data remains with sqlite.
           That is, the caller does not need to free
           the memory pointed to by tgt, but may need to
           copy it if he wants to use it later.
        */
        int get( int index, sqlite3_text_char_t const ** tgt, int & sz );
        /**
           See get(int,char const **, int&). Only the tgt
           type is different.
        */
        int get( int index, void const ** tgt, int & sz );


        /**
           This is fundamentally identical to get(int,int &)
           except that the key type is a string, which must
           exactly (case-sensitively) match a column name from
           this result set. On success, SQLITE_OK is returned
           and tgt is modified. On error, some other code is
           returned and tgt is not modified.

           Note that fetching by string index is much less
           efficient than looking up by integer index, but of
           course also a lot more convenient. If you're
           looking for the most speed, go with
           get(int,...). If you're looking for flexibility and
           convenience, at the cost of a few extra cyles and a
           tiny bit of extra memory usage per cursor, then use
           string-based keys.
        */
        int get( std::string const & key, int & tgt );
        /**
           See get(std::string const &,int&).
        */
        int get( std::string const & key, int64_t & tgt );
        /**
           See get(std::string const &,int&).
        */
        int get( std::string const & key, double & tgt );
        /**
           See get(std::string const &,int&).
        */
        int get( std::string const & key, std::string & tgt );
        /**
           If indexis in bounds, this function gets the (char
           unsigned const *) data at that column index and
           assigns tgt to that value and sz to the size of the
           data.

           tgt is written to by this func but ownership
           of the underlying data remains with sqlite.
           That is, the caller does not need to free
           the memory pointed to by tgt, but may need to
           copy it if he wants to use it later.
        */
        int get( std::string const & key, sqlite3_text_char_t const ** tgt, int & sz );
        /**
           See get(std::string const &,char const **, int&). Only the tgt
           type is different.
        */
        int get( std::string const & key, void const ** tgt, int & sz );


        /**
           Sets str to the column name as the given index
           (0-based). Returns SQLITE_OK on success, else
           SQLITE_ERROR and str is not modified.
        */
        int colname( int index, std::string & str );
        /**
           Points str to the nul-terminated column name at the
           given index (0-based), or 0 on error. This
           overload avoids an extra copy of the column name,
           but sqlite owns the string and clients will need to
           make a copy of it if they want to continue to use
           it beyond the lifetime of this object's underlying
           prepared statement.

           Returns SQLITE_OK if str is set, otherwise
           SQLITE_ERROR.
        */
        int colname( int index, char const ** str );
        //int colname( int index, std::wstring & );
    };


    /**
       This class represents a prepared database statement.

       statement objects are copied shallowly - each copy points
       back to a single original sqlite3_stmt object. That sqlite3_stmt
       is finalized with the last of these copies goes out of scope
       or is finalized.

       Sample usage:
       \code
       // Reading data:
       statement st(mydb, "select * from sqlite_master");
       if( ! st.is_prepared() )
       {
       ... error ...
       }
       cursor cur( st.get_cursor() );
       while( SQLITE_ROW == cur.step() )
       {
       ... do something with each row ...
       }


       // Or:
       statement st(mydb, "select count(*) from mytable" );
       int val = 0;
       int rc = st.execute( val );
       if( ! rc_is_okay( rc ) ) { ... error ... }
       std::cout << "count(*) == " << val << '\n';

       // Writing data:
       statement st( mydb, "insert into mytable values(?,?)" );
       st.bind( 1, "a value" );
       st.bind( 2, someIntValue );
       int rc = st.execute();
       if( ! rc_is_okay( rc ) )  { ... error ... }

       \endcode


       Note about copying: copying a statement object produces a
       shallow copy. All copies of this type will refer to the
       same underlying (sqlite3_stmt*) handle. The handle will be
       closed when the last instance of this class which points to
       that statement goes out of scope or is finalized.
    */
    class statement
    {
    private:
        database & m_db;
        typedef rcptr<sqlite3_stmt,sqlite3_stmt_finalizer> RCPtr;
        RCPtr m_stmt;
        int m_argc;
        friend class cursor;
        /**
           Unfortunate: in order to ensure no crashes when a cursor
           resets its statement on destruction AFTER the statement has
           been destroyed, we register each cursor and call
           cursor::m_stmt.take() to keep the cursor from resetting it.
           This is never a problem in C++, but it is in the JavaScript
           bindings built on this API, as they create on the heap and
           the garbage collection is unpredictable.
        */
        typedef std::set<cursor *> CursorSet;
        CursorSet m_curs;
        void add_cursor( cursor * c ) { m_curs.insert(c); }
        void remove_cursor( cursor * c ) { m_curs.erase(c); }
    public:
        /**
           Initializes a prepared statement without a
           query. Use prepare() to prepare the statement.
        */
        statement( database & db );

        /**
           Initializes a statement with the given sql.
           Use is_prepared() to determine if the sql compiled
           successfully.
        */
        statement( database & db, std::string const & sql );
        /**
           Initializes a statement with the given sql.
           Use is_prepared() to determine if the sql compiled
           successfully. byteCount is the length of sql, in bytes.
           If set to -1 then strlen() is used to determine the size
           of sql.
        */
        statement( database & db, char const * sql, int byteCount = -1 );

        /**
           Calls this->finalize()
        */
        ~statement();

        /**
           (Re-)prepares an SQL statement. Return code is that
           of sqlite3_prepare(). If any value other than
           SQLITE_OK is returned then preparation failed and
           this object is not ready to be used.
        */
        int prepare( std::string const & sql );
        /**
           Same as prepare(std::string) but the len parameter
           specifies the length of sql. If byteCount is -1 then
           strlen(sql) is used to find the length.
        */
        int prepare( char const * sql, int byteCount = -1 );

#if SQ3_USE_WCHAR
        //statement( database & db, std::wstring const & sql );
        statement( database & db, wchar_t const * sql, int byteCount = -1 );
        /**
         */
        //int prepare( std::wstring const & sql );
        /**
         */
        int prepare( sqlite3_wstring_t const sql, int byteCount = -1 );
        //int execute( wchar_t * tgt, int & len );
        //int execute( std::wchar & tgt );
        //int bind( int index, wchar_t const * data, int len );
        //int bind( int index, std::wstring const & data );
#endif // SQ3_USE_WCHAR

        /**
           Binds NULL to the given placeholder index (1-based,
           not 0-based!).

           Placeholders are added to SQL code with question
           marks, like this:

           \code
           INSERT INTO MyTable(a,b) VALUES(?,?);
           \endcode

           In this case we have two placeholders at indexes 1
           and 2.

           Note that all bind()-related indexes are 1-based,
           but cursor::get() uses 0-based indexes. This
           inconsistency is an artefact of the sqlite3 API
           (and may even have a longer history).
        */
        int bind( int index );
        /**
           Binds data to the given placeholder index (1-based,
           not 0-based!).
        */
        int bind( int index, int data );
        /**
           Binds data to the given placeholder index (1-based,
           not 0-based!).
        */
        int bind( int index, int64_t data );
        /**
           Binds data to the given placeholder index (1-based,
           not 0-based!).
        */
        int bind( int index, double data );
        /**
           Binds data to the given placeholder index (1-based,
           not 0-based!). len must be the length of data, in bytes.
        */
        int bind( int index, char const * data, int len );
        /**
           Binds data to the given placeholder index (1-based,
           not 0-based!). len must be the length of data, in bytes.
        */
        int bind( int index, void const * data, int len );

        /**
           Binds data to the given placeholder index (1-based,
           not 0-based!).
        */
        int bind( int index, std::string const & data );

        /**
           Binds NULL to the given placeholder index. Note
           that binding by string index is notably less
           efficient than binding by integer index.

           Named placeholders are embedded in SQL similar
           to:

           \code
           INSERT INTO MyTable (a,b) VALUES(:A,:B);
           \endcode

           In that string we have two named bound arguments:
           ":A" and ":B", at indexes 1 and 2, respectively.
           Note that the leading colon is considered to be part
           of the name.
        */
        int bind( char const * index );
        /**
           Binds data to the given placeholder index. See
           bind(char const *) for more info.
        */
        int bind( char const * index, int data );
        /**
           Binds data to the given placeholder index. See
           bind(char const *) for more info.
        */
        int bind( char const * index, int64_t data );
        /**
           Binds data to the given placeholder index. See
           bind(char const *) for more info.
        */
        int bind( char const * index, double data );
        /**
           Binds data to the given placeholder index. len must
           be the length of data, in bytes. See
           bind(char const *) for more info.
        */
        int bind( char const * index, char const * data, int len );
        /**
           Binds data to the given placeholder index. len must
           be the length of data, in bytes. See bind(char
           const *) for more info.
        */
        int bind( char const * index, void const * data, int len );

        /**
           Binds data to the given placeholder index. See
           bind(char const *) for more info.
        */
        int bind( char const * index, std::string const & data );

        /**
           Returns a cursor object ready to step over
           the result set from this object.

           Note that due to low-level design details, it is
           unwise to mix the execute() functions and
           get_cursor() on the same statement. All cursors
           created from this statement (and all copies of
           those cursors) relate back to *this* statement
           object and when the last cursor goes out of scope
           the underlying prepared statement is
           reset. Additionally, the execute() family of
           functions are all actually implemented in terms of
           get_cursor(). Mis-interactions between a mixture of
           get_cursor() and execute() on the same client-side
           statement object cannot be ruled out.

           See the ~cursor destructor for more details.
        */
        cursor get_cursor();

        /**
           Assumes this object's SQL statement is a single
           statement. Executes that statement and returns the
           value from an underlying sqlite3_step() call. Thus
           SQLITE_ROW or SQLITE_DONE will be returned on
           success, depending on the underlying query.
        */
        int execute();

        /**
           Executes this statement and saves the return value
           of that statement in tgt. If this function returns
           any other value than SQLITE_OK then tgt is not
           modified. Note that the value of this object's
           first field must be lexically convertible to tgt's
           type or else tgt will be set to some unspecified
           value.
        */
        int execute( int & tgt );

        /** See execute(int&). */
        int execute( int64_t & tgt );

        /** See execute(int&). */
        int execute( double & tgt );

        /** See execute(int&). */
        int execute( std::string & tgt );

        /**
           See execute(int&). The length of the "returned"
           string is saved in len (in bytes). Ownership of the
           string remains with sqlite3, and the client should
           copy it if he wants to ensure that he has it for
           later. The string's exact lifetime is unspecified
           in the sqlite3 documentation, but in theory it is
           valid until this statement object is finalized or
           a cursor object steps through the result set of this
           statement.
        */
        int execute( sqlite3_text_char_t const ** tgt, int & len );

        /**
           See execute(sqlite3_text_char_t const
           **,int&). This is similar but is used to fetch blob
           data. The blob is "returned" by passinging tgt to
           it.  The length of the blob (in bytes) is saved in
           len. Ownership of the blob data remains with
           sqlite3, and the client should copy it if he wants
           to ensure that he has it for later.  The blob's
           exact lifetime is unspecified in the sqlite3
           documentation, but in theory it is
           valid until this statement object is finalized or
           a cursor object steps through the result set of this
           statement.
        */
        int execute( void const ** tgt, int & len );

        /**
           Finizalizes the underlying prepared statement,
           freeing its resources. Any cursor objects created
           through this->get_cursor() now points to stale
           data and must not be used.

           Return value is the result of calling sqlite3_finalize(),
           or SQLITE_ERROR if finalization cannot take place (e.g.
           finalize() was already called).
        */
        int finalize();

        /**
           Use after construction to ensure that a statement
           was compiled. Returns true if the statement was
           compiled, else false. Returning false typically
           means that the supplied SQL has a syntax error,
           refers to non-existing fields, etc.
        */
        bool is_prepared() const;

        /**
           Calls sqlite3_reset() on the underlying statement
           handle and returns the result.
        */
        int reset();

        /**
           Calls sqlite3_clear() on the underlying statement
           handle and returns the result.
        */
        int clear_bindings();

        /** Returns the column count of this prepared
            statement, or -1 on error.  May return 0 for
            queries which has no return value (e.g. UPDATE).
        */
        int colcount();

        /**
           On success, it returns the null-terminated column
           name of the given column. On error it returns
           0. The returned string is owned by sqlite3 and is
           not guaranteed to be valid longer than the lifetime
           of this statement, so copy it if you need it.
        */
        char const * colname( int index );

        /**
           On success, assigns cn to the null-terminated column
           name at the given index and returns SQLITE_OK. On
           failure cn is not modified and some other value is
           returned. The column name string is not guaranteed
           to be valid longer than this preparation of this
           statement object, so copy it immediately if you will
           need it later.
        */
        int colname( int index, char const ** cn );

       };

} // namespace sq3


#endif // s11n_net_SQ3_HPP_INCLUDED
