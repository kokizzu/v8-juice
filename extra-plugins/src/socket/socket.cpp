/************************************************************************
Author: Stephan Beal (http://wanderinghorse.net/home/stephan)

License: New BSD

Based very heavily on:

http://code.google.com/p/v8cgi/source/browse/trunk/src/lib/socket/socket.cc

by Ondrej Zara


************************************************************************/
#if !defined _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200112L
#endif

#include <v8/juice/ClassWrap.h>
#include <v8/juice/forwarding.h>
#include <v8/juice/plugin.h>
#include <v8/juice/juice.h>

#include <stdio.h> // FILE and friends
#include <errno.h>
#include <sstream>
#include <vector>

#ifdef windows
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  define close(s) closesocket(s)
#else
#  include <unistd.h>
#  include <sys/socket.h>
#  include <sys/un.h>
#  include <sys/param.h>
#  include <arpa/inet.h>
#  include <netinet/in.h>
#  include <netdb.h>
#endif 


#ifndef CERR
#include <iostream> /* only for debuggering */
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

using namespace v8::juice::tmp;
namespace cv = v8::juice::convert;
#define JSTR(X) v8::String::New(X)



#ifndef MAXHOSTNAMELEN
#  define MAXHOSTNAMELEN 64
#endif


namespace {

typedef union sock_addr {
    struct sockaddr_in in;
#ifndef windows
    struct sockaddr_un un;
#endif
    struct sockaddr_in6 in6;
} sock_addr_t;

static struct 
{
    /** Name of JSSocket-internal field for holding connection peer information. */
    char const * fieldPeer;
} socket_strings =
    {
    "dgramPeer"
    };
} /* anon namespace */



/**
 * Universal address creator.
 * @param {char *} address Stringified address
 * @param {int} port Port number
 * @param {int} family Address family
 * @param {sock_addr_t *} result Target data structure
 * @param {socklen_t *} len Result length
 */
static int create_addr(char const * address, int port, int family, sock_addr_t * result, socklen_t * len)
{
    unsigned int length = strlen(address);
    switch (family) {
#ifndef windows
      case AF_UNIX: {
          struct sockaddr_un *addr = (struct sockaddr_un*) result;
          memset(addr, 0, sizeof(sockaddr_un));
          
          if (length >= sizeof(addr->sun_path)) {
              v8::ThrowException( JSTR("Unix socket path too long"));
              return 1;
          }
          addr->sun_family = AF_UNIX;
          memcpy(addr->sun_path, address, length);
          addr->sun_path[length] = '\0';
          *len = length + (sizeof(*addr) - sizeof(addr->sun_path));
      } break;
#endif
      case AF_INET: {
          struct sockaddr_in *addr = (struct sockaddr_in*) result;
          memset(result, 0, sizeof(*addr)); 
          addr->sin_family = AF_INET;

#ifdef windows
          int size = sizeof(struct sockaddr_in);
          int pton_ret = WSAStringToAddress(address, AF_INET, NULL, (sockaddr *) result, &size);
          if (pton_ret != 0) { return 1; }
#else 
          int pton_ret = inet_pton(AF_INET, address, & addr->sin_addr.s_addr);
          if (pton_ret == 0) { return 1; }
#endif

          addr->sin_port = htons((short)port);
          *len = sizeof(*addr);
      } break;
      case AF_INET6: {
          struct sockaddr_in6 *addr = (struct sockaddr_in6*) result;
          memset(addr, 0, sizeof(*addr));
          addr->sin6_family = AF_INET6;

#ifdef windows
          int size = sizeof(struct sockaddr_in6);
          int pton_ret = WSAStringToAddress(address, AF_INET6, NULL, (sockaddr *) result, &size);
          if (pton_ret != 0) { return 1; }
#else 
          int pton_ret = inet_pton(AF_INET6, address, addr->sin6_addr.s6_addr);
          if (pton_ret == 0) { return 1; }
#endif                  
                        
          addr->sin6_port = htons((short)port);
          *len = sizeof(*addr);
      } break;
    }
    return 0;
}
/**
 * Returns JS array with values describing remote address.
 * For UNIX socket, only one item is present. For AF_INET and
 * AF_INET6, array contains [address, port].
 * Returns v8::Undefined() on error.
 */
static v8::Handle<v8::Value> create_peer(sockaddr * addr)
{
    switch (addr->sa_family) {
#ifndef windows
      case AF_UNIX: {
          v8::Handle<v8::Array> result = v8::Array::New(1);
          sockaddr_un * addr_un = (sockaddr_un *) addr;
          result->Set( 0U, cv::CastToJS<char const *>(addr_un->sun_path));
          return result;
      } break;
#endif
      case AF_INET6: {
          v8::Handle<v8::Array> result = v8::Array::New(2);
          char buf[INET6_ADDRSTRLEN+1];
          memset( buf, 0, INET_ADDRSTRLEN+1 );
          sockaddr_in6 * addr_in6 = (sockaddr_in6 *) addr;

#ifdef windows
          DWORD len = INET6_ADDRSTRLEN;
          WSAAddressToString(addr, sizeof(struct sockaddr), NULL, buf, &len);
#else                   
          inet_ntop(AF_INET6, addr_in6->sin6_addr.s6_addr, buf, INET6_ADDRSTRLEN);
#endif
                        
          result->Set( 0U, cv::CastToJS(buf));
          result->Set( 1U, cv::CastToJS( ntohs(addr_in6->sin6_port)));
          return result;
      } break;
      case AF_INET: {
          v8::Handle<v8::Array> result = v8::Array::New(2);
          char buf[INET_ADDRSTRLEN+1];
          memset( buf, 0, INET_ADDRSTRLEN+1 );
          sockaddr_in * addr_in = (sockaddr_in *) addr;

#ifdef windows
          DWORD len = INET_ADDRSTRLEN;
          WSAAddressToString(addr, sizeof(struct sockaddr), NULL, buf, &len);
#else                   
          inet_ntop(AF_INET, & addr_in->sin_addr.s_addr, buf, INET_ADDRSTRLEN);
#endif
          result->Set( 0U, cv::CastToJS(buf));
          result->Set( 1U, cv::CastToJS(ntohs(addr_in->sin_port)));
          return result;
      } break;
    }
    return v8::Undefined();
}

/**
   Not yet complete.
*/
class JSByteArray
{
public:
    typedef std::vector<unsigned char> BufferType;
    BufferType vec;
    JSByteArray()
        :vec()
    {
    }
    /**
       JS Usage:

       new ByteArray( [string data = null [, int length=data.length]] )
    */
    JSByteArray( v8::Handle<v8::Value> const & val, unsigned int len = 0 );
    ~JSByteArray()
    {
        //CERR "~JSByteArray@"<<this<<'\n';
    }
    /** Returns the current length of the byte array. */
    uint32_t length() const
    {
        //CERR << "length()\n";
        return this->vec.size();
    }
    /** Sets the length of the byte array. Throws if sz is "too
        long." Returns the new number of items. Any new entries (via
        growing the array) are filled with zeroes.
     */
    uint32_t length( uint32_t sz );

    /** toString() for JS. */
    std::string toString() const;    

    /**
       Returns array contents as a std::string.
    */
    std::string stringValue() const
    {
        return this->vec.empty()
            ? std::string()
            : std::string( this->vec.begin(), this->vec.end() );
    }
    
//     std::string asString() const;
//     std::string asString( unsigned int fromOffset ) const;
//     std::string asString( unsigned int fromOffset, unsigned int len ) const;
    
    static v8::Handle<v8::Value> indexedPropertyGetter(uint32_t index, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> indexedPropertySetter(uint32_t index, v8::Local< v8::Value > value, const v8::AccessorInfo &info);
    static v8::Handle<v8::Integer> indexedPropertyQuery(uint32_t index, const v8::AccessorInfo &info);
    static v8::Handle<v8::Boolean> indexedPropertyDeleter(uint32_t index, const v8::AccessorInfo &info);
    static v8::Handle<v8::Array> indexedPropertyEnumerator(const v8::AccessorInfo &info);
    static void SetupBindings( v8::Handle<v8::Object> dest );
};


/**
   A thin wrapper around socket() and friends, providing
   a C++/JS binding for the v8-juice framework.
*/
class JSSocket
{
public:
    static bool enableDebug;
    int fd;
    int family;
    int proto;
    int type;
    bool hitTimeout;
    /** JS-side 'this' object. Set by v8::juice::cw::WeakWrap<JSSocket>::Wrap(). */
    v8::Handle<v8::Object> jsSelf;
private:
    void init(int family, int type, int proto, int socketFD )
    {
        this->family = family;
        this->proto = proto;
        this->type = type;
        if( socketFD < 0 )
        {
            this->fd = socket( family, type, proto );
        }
        else
        {
            this->fd = socketFD;
        }
        if( this->fd < 0 )
        {
            cv::StringBuffer msg;
            msg << "socket("<<family<<", "<<type<<", "<<proto<<") failed. errno="<<errno<<'.';
            throw std::runtime_error( msg.Content().c_str() );
        }
    }
public:
#define DBGOUT if(JSSocket::enableDebug) CERR

    /**
       Like socket(family,type,proto). If socketFD is >=0 then it is
       assumed to be an opened socket with the same family, type, and
       protocol. If socketFD is (>=0) and the constructor succeeds,
       this object takes ownership of the socket and will close it
       when it destructs. If socketFD is (>=0) and the ctor fails
       (throws), the caller owns the socket.
    */
    JSSocket(int family = AF_INET,
             int type = SOCK_DGRAM,
             int proto = 0,
             int socketFD = -1 )
        : fd(-1), family(0), proto(0),type(0),
          hitTimeout(false),
          jsSelf()
    {
        this->init( family, type, proto, socketFD );
    }

    /** Closes the socket. */
    virtual ~JSSocket()
    {
        this->close();
    }

    /**
       Closes the socket (if it is opened).
    */
    void close()
    {
        if( this->fd >= 0 )
        {
            DBGOUT << "JSSocket@"<<(void const *)this<<"->close()\n";
            ::close( this->fd );
            this->fd = -1;
        }
    }

    /** toString() for JS. */
    std::string toString() const;

    /**
       Installs this class' bindings into dest.
    */
    static void SetupBindings( v8::Handle<v8::Object> dest );

    /**
       JS Usage:

       int bind( string hostNameOrAddress, int port )
    
       Throws a JS exception on error, so the return value
       is largely irrelevant. But 0==success, if you must
       know.       
    */
    int bind( char const * where, int port )
    {
        if( ! where || !*where )
        {
            v8::ThrowException(JSTR("Address argument may not be empty!"));
            return -1;
        }
        sock_addr_t addr;
        memset( &addr, 0, sizeof( sock_addr_t ) );
        socklen_t len = 0;
        int rc = 0;
        {
            v8::Unlocker unl;
            rc = create_addr( where, port, this->family, &addr, &len );
        }
        cv::StringBuffer msg;
        if( 0 != rc )
        { // By-address failed, so we'll try by hostname...
            //CERR << "Bind to address "<<where<<" failed. Trying by name...\n";
            v8::Handle<v8::Value> const ip = nameToAddress( where );
            if( ! ip.IsEmpty() )
            {
                std::string const & ips = cv::JSToStdString(ip);
                v8::Unlocker unl;
                rc = create_addr( ips.c_str(), port, this->family, &addr, &len );
            }
            if( 0 != rc )
            {
                msg << "Malformed address: ["<<where<<"] ";
            }
        }
        if( 0 == rc )
        {
            CERR << "bind()ing...\n";
            {
                v8::Unlocker unl;
                rc = ::bind( this->fd, (sockaddr *)&addr, len );
                CERR << "bind() rc="<<rc<<'\n';
            }
            if( 0 != rc )
            {
                msg << "Bind failed! errno="<<errno<<" ("<<strerror(errno)<<")!";
            }
        }
        if( 0 != rc )
        {
            v8::ThrowException( msg );
        }
        return rc;
    }


    /**
       JS usage:

       int listen( [int backlog] )

       Throws a JS exception on error. Returns 0 on success.
    */
    int listen( int backlog )
    {
        CERR << "WARNING: listen() IS UNTESTED!\n";
        int rc = ::listen( this->fd, backlog );
        // ^^^^ does socket timeout value apply here???
        if( 0 != rc )
        {
            cv::StringBuffer msg;
            msg << "listen() failed: errno="<<errno
                << " ("<<strerror(errno)<<')'
                ;
            v8::ThrowException(msg);
        }
        return rc;
    }

    /** this->listen( someUnspecifiedValue ). */
    int listen()
    {
        return this->listen( 5 );
    }

    /**
       accept(2)'s a connection and creates a new socket
       for it. The returned object is owned by v8.

       Returns NULL if:

       - accept()ing would block a non-blocking socket.

       - Possibly if a timeout occurs.

       On error: in addition to returning NULL, a JS-side exception is
       thrown if accept(2) returns an error or if construction of the
       new socket object fails for some reason.
    */
    JSSocket * accept();

    static int getProtoByName( char const * name )
    {
        struct protoent const * r = ::getprotobyname(name);
        return r ? r->p_proto : 0;
    }

    /** Converts name to canonical address. */
    static v8::Handle<v8::Value> nameToAddress( char const * name, int family )
    {
        if( ! name || !*name )
        {
            return v8::ThrowException(JSTR("'name' parameter may not be empty!"));
        }
        struct addrinfo hints, * servinfo = NULL;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = family;
        int rc = 0;
        {
            v8::Unlocker unl;
            rc = getaddrinfo(name, NULL, &hints, &servinfo);
        }
        if (rc != 0)
        {
            cv::StringBuffer msg;
            msg << gai_strerror(rc);
            return v8::ThrowException(msg);
        }
                
        v8::Handle<v8::Value> rv = create_peer(servinfo->ai_addr);
        freeaddrinfo(servinfo);
        if( rv.IsEmpty() ) return rv; // pass on exception
        v8::Local<v8::Object> item;
        if( rv->IsObject() )
        {
            item = rv->ToObject();
        }
        if( ! item.IsEmpty() )
        {
            return item->Get(cv::CastToJS<int>(0));
        }
        else
        {
            return v8::Undefined();
        }
    }
    static v8::Handle<v8::Value> nameToAddress( char const * name )
    {
        return nameToAddress( name, 0 );
    }

    /** Converts canonical address addy to hostname. if family==0 then
        AF_INET is assumed. */
    static v8::Handle<v8::Value> addressToName( char const * addy, int family )
    {
        if( ! addy || !*addy )
        {
            return v8::ThrowException(JSTR("'address' parameter may not be empty!"));
        }
        if( ! family ) family = AF_INET;
        char hn[NI_MAXHOST+1];
        memset( hn, 0, sizeof(hn) );
        sock_addr_t addr;
        socklen_t len = 0;
        memset( &addr, 0, sizeof(sock_addr_t) );
        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        int rc = 0;
        {
            v8::Unlocker unl;
            rc = create_addr( addy, 0, family, &addr, &len );
        }
        if( 0 != rc )
        {
            cv::StringBuffer msg;
            msg << "Malformed address: "<<addy;
            return v8::ThrowException(msg);
        }
        rc = ::getnameinfo( (sockaddr*) &addr, len, hn, NI_MAXHOST, NULL, 0, 0 );
        if( 0 != rc )
        {
            cv::StringBuffer msg;
            msg << "getnameinfo() failed with rc "<<rc <<
                " ("<<gai_strerror(rc)<<")";
            return v8::ThrowException(msg);
        }
        else
        {
            return cv::CastToJS(hn);
        }
    }

    static v8::Handle<v8::Value> addressToName( char const * addy )
    {
        return addressToName( addy, 0 );
    }


    /**
       JS usage:

       int connect( string nameOrAddress, int port )

       Throws a JS exception on error.
    */
    int connect( char const * where, int port )
    {
        if( ! where || !*where )
        {
            v8::ThrowException( JSTR("Address may not be empty!") );
            return -1;
        }
        sock_addr_t addr;
        memset( &addr, 0, sizeof(sock_addr_t) );
        socklen_t len = 0;
        int rc = 0;
        {
            v8::Unlocker unl;
            rc = create_addr(where, port, this->family, &addr, &len);
        }
        if( 0 != rc )
        {
            v8::Handle<v8::Value> ip = nameToAddress( where );
            if( ! ip.IsEmpty() )
            {
                std::string const ips = cv::JSToStdString(ip);
                v8::Unlocker unl;
                rc = create_addr( ips.c_str(), port, this->family, &addr, &len );
            }
            if( 0 != rc )
            {
                cv::StringBuffer msg;
                msg << "Malformed address: "<<where;
                v8::ThrowException(msg);
                return rc;
            }
        }
        int errNo = 0;
        {
            v8::Unlocker unl;
            rc = ::connect( this->fd, (sockaddr *)&addr, len );
            errNo = errno;
        }
        if( 0 != rc )
        {
            cv::StringBuffer msg;
            msg << "connect() failed: errno="<<errNo
                << " ("<<strerror(errNo)<<')';
            v8::ThrowException(msg);
            return rc;
        }
        else
        {
            DBGOUT << "Connected to ["<<where<<':'<<port<<"]\n";
        }
        return 0;
    }
    
    int connect( char const * addr )
    {
        // TODO: do not require port # for AF_UNIX?
        return -1;
    }

    static std::string hostname()
    {
        enum { MaxLen = 129 };
        char buf[MaxLen];
        memset( buf, 0, MaxLen );
        gethostname(buf, MaxLen-1 );
        return buf;
    }

    /**
       Reads n bytes from the socket and returns them
       as described below.

       Special return values:

       - (val===null) means reading was interrupted
       by a timeout and no bytes were read before the
       timeout expired.

       - (val===undefined) means that a read error
       occured, but this function will throw
       a v8 exception in that case.

       If binary is true then the returned value is a JSByteArray
       object. If binary is false then the data is returned as a
       string (which will fail with undefined behaviour if the string
       is not encoded in a way which v8 requires).
       
       When the length of the returned data is less than n, it
       could mean any of:

       - Timeout was reached after reading some bytes.

       - EOF

       There is unfortunately currently no way to distinguish.
       We may need to add an error id property to the class so
       that clients can tell the difference.
    */
    v8::Handle<v8::Value> read( unsigned int n, bool binary );

    /** this->read( n, false ). */
    v8::Handle<v8::Value> read( unsigned int n )
    {
        return this->read( n, false );
    }

    /**
       Writes n bytes from src to this socket. Returns the number
       of bytes written or:

       - 0 if a timeout was hit before sending any bytes.

       - It return 0 and throws a JS exception on a write error other
       than timeout-before-send.

       Return of (<n) _may_ be due to a timeout during write.
    */
    unsigned int write2( char const * src, unsigned int n );

    /** this->write2( src, strlen(src) ), or 0 if !src or !*src. */
    unsigned int write1( char const * src )
    {
        return (src && *src)
            ? this->write2( src, strlen(src) )
            : 0
            ;
    }

    /**
       JS usage:

       int write( ByteArray|string src,  [, int len=src.length] )

       Returns number of bytes written. Return of 0 "should" signify
       that a timeout was encountered before any bytes were written. A
       short write _might_ signify that a timeout was encountered
       after some bytes had been written.

       Throws a JS-side exception on write errors other than timeouts.

       argv.This() must be-a JSSocket or it throws a JS-side
       exception.
    */
    static v8::Handle<v8::Value> writeN( v8::Arguments const & argv );

    /**
       JS Usage:

       int sendTo( string hostNameOrAddress, ByteArray|string src [, int len=src.length] );
    */
    static v8::Handle<v8::Value> sendTo( v8::Arguments const & argv );

    
    /**
       JS Usage:

       Array peerInfo()

       Returns [address,port] for AF_INET and AF_INET6 sockets.
       For AF_UNIX sockets, [socketFilePath]. Returns undefined
       for other types.

       This value is only valid after a connection is established,
       and it throws if getpeername(2) fails.
    */    
    v8::Handle<v8::Value> peerInfo()
    {
        if( ! this->jsSelf->Get(JSTR(socket_strings.fieldPeer))->IsTrue() )
        {
            sock_addr_t addr;
            memset( &addr, 0, sizeof(sock_addr_t) );
            socklen_t len = sizeof(sock_addr_t);
            int rc = 0;
            {
                v8::Unlocker unl;
                rc = getpeername(this->fd, (sockaddr *) &addr, &len);
            }
            if (0 == rc)
            {
                this->jsSelf->Set(JSTR(socket_strings.fieldPeer), create_peer( (sockaddr*) &addr ) );
            }
            else
            {
                cv::StringBuffer msg;
                msg << "getpeername("<<this->fd<<",...) failed! errno="<<errno
                    << " ("<<strerror(errno)<<")!";
                return v8::ThrowException(msg);
            }
        }
        return this->jsSelf->Get(JSTR(socket_strings.fieldPeer));
    }

    /**
       Don't use. See getOpt() for why.
    */
    int setOpt( int key, int val )
    {
        int rc = setsockopt( this->fd, SOL_SOCKET, key, (void const *)&val, sizeof(int) );
        if( 0 != rc )
        {
            rc = errno;
            cv::StringBuffer msg;
            msg << "setsockopt("<<key<<", "<<val<<") failed! errno="<<errno
                << " ("<<strerror(errno)<<")"
                ;
            v8::ThrowException(msg);
        }
        return rc;
    }

    /**
       Don't use this until we can confirm which options we can
       support here.
    */
    v8::Handle<v8::Value> getOpt( int key )
    {
        int buf = 0;
        socklen_t len = sizeof(buf);
        int rc = getsockopt( this->fd, SOL_SOCKET, key, (void *)&buf, &len );
        if( 0 != rc )
        {
            cv::StringBuffer msg;
            msg << "getsockopt("<<key<<") failed! errno="<<rc
                << " ("<<strerror(errno)<<")"
                ;
            return v8::ThrowException(msg);
        }
        else
        {
            return cv::CastToJS(buf);
        }
    }


    /**
       Don't use this: the returned string is not going to be encoded
       properly.
    */
    v8::Handle<v8::Value> getOpt( int key, unsigned int len )
    {
        typedef std::vector<char> VT;
        VT vec( len, '\0' );
        socklen_t slen = len;
        int rc = getsockopt( this->fd, SOL_SOCKET, key, (void *)&vec[0], &slen );
        if( 0 != rc )
        {
            cv::StringBuffer msg;
            msg << "getsockopt("<<key<<") failed! errno="<<rc
                << " ("<<strerror(errno)<<")"
                ;
            return v8::ThrowException(msg);
        }
        else
        {
            return cv::CastToJS(&vec[0]);
        }
    }

    /**
       Sets the send/receive timeouts to the given number of
       seconds/microseconds. Returns 0 on success or an errno value on
       error.
    */
    int setTimeout( unsigned int seconds, unsigned int usec )
    {
        struct timeval t;
        t.tv_sec = seconds;
        t.tv_usec = usec;
        socklen_t len = sizeof(timeval);
        int rc = setsockopt( this->fd, SOL_SOCKET, SO_SNDTIMEO, &t, len );
        if( 0 == rc )
        {
            rc = setsockopt( this->fd, SOL_SOCKET, SO_RCVTIMEO, &t, len );
        }
        if( rc != 0 )
        {
            rc = errno;
        }
        return rc;
    }
    int setTimeout( unsigned int seconds )
    {
        return this->setTimeout( seconds, 0 );
    }
    
}/*JSSocket*/;



#include <v8/juice/ClassWrap_TwoWay.h>
////////////////////////////////////////////////////////////////////////
// Set up our ClassWrap policies...
namespace v8 { namespace juice { namespace cw
{
   
    template <> struct DebugLevel<JSSocket> : Opt_Int<1> {};

    template <>
    struct ToNative_SearchPrototypesForNative<JSSocket>
        : Opt_Bool<true>
    {};

    template <>
    struct AllowCtorWithoutNew<JSSocket>
        : Opt_Bool<false>
    {};

    template <>
    struct InternalFields<JSSocket>
        : InternalFields_Base<JSSocket,1,0>
    {};

    template <>
    struct Factory<JSSocket>
        : Factory_CtorForwarder<JSSocket,
                                v8::juice::tmp::TypeList<
            cv::CtorForwarder0<JSSocket>,
            cv::CtorForwarder1<JSSocket,int>,
            cv::CtorForwarder2<JSSocket,int,int>,
            cv::CtorForwarder3<JSSocket,int,int,int>,
            /* for internal use only! */
            cv::CtorForwarder4<JSSocket,int,int,int,int>
        >
        >
    {};

    template <>
    struct Extract< JSSocket > :
        TwoWayBind_Extract< JSSocket > {};

    template <>
    struct ToNative< JSSocket > :
        ToNative_Base< JSSocket > {};
    
    template <>
    struct ToJS< JSSocket > :
        TwoWayBind_ToJS< JSSocket > {};

    template <>
    struct ClassName< JSSocket >
    {
        static char const * Value()
        {
            return "Socket";
        }
    };
    
    template <>
    struct WeakWrap< JSSocket >
    {
        typedef JSSocket Type;
        typedef JSSocket * NativeHandle;
        typedef TwoWayBind_WeakWrap<JSSocket> WB;
        static void Wrap( v8::Persistent<v8::Object> const & jsSelf, NativeHandle nativeSelf )
        {
            WB::Wrap( jsSelf, nativeSelf );
            nativeSelf->jsSelf = jsSelf;
            return;
        }
        static void Unwrap( v8::Handle<v8::Object> const & jsSelf, NativeHandle nativeSelf )
        {
            WB::Unwrap( jsSelf, nativeSelf );
            nativeSelf->jsSelf = v8::Handle<v8::Object>();
            return;
        }
    };

    template <>
    struct Installer<JSSocket>
    {
    public:
        /**
           Installs the bindings for JSSocket into the given object.
        */
        static void SetupBindings( ::v8::Handle< ::v8::Object> target )
        {
            JSSocket::SetupBindings(target);
        }
    };

    template <>
    struct ToNative_SearchPrototypesForNative<JSByteArray>
        : Opt_Bool<false>
    {};

    template <>
    struct AllowCtorWithoutNew<JSByteArray>
        : Opt_Bool<false>
    {};

    template <>
    struct InternalFields<JSByteArray>
        : InternalFields_Base<JSByteArray,1,0>
    {};

    template <>
    struct Factory<JSByteArray>
        : Factory_CtorForwarder<JSByteArray,
                                v8::juice::tmp::TypeList<
            cv::CtorForwarder0<JSByteArray>,
            cv::CtorForwarder1<JSByteArray, v8::Handle<v8::Value> const & >,
            cv::CtorForwarder2<JSByteArray, v8::Handle<v8::Value> const &, unsigned int >
            > >
    {};

    template <>
    struct ToNative< JSByteArray > :
        ToNative_Base< JSByteArray > {};
    
    template <>
    struct ClassName< JSByteArray >
    {
        static char const * Value()
        {
            return "ByteArray";
        }
    };

    template <>
    struct Installer<JSByteArray>
    {
    public:
        /**
           Installs the bindings for JSByteArray into the given object.
        */
        static void SetupBindings( ::v8::Handle< ::v8::Object> target )
        {
            JSByteArray::SetupBindings(target);
        }
    };

    
} } } // v8::juice::cw

namespace v8 { namespace juice { namespace convert
{

    template <>
    struct JSToNative< JSSocket > : v8::juice::cw::JSToNativeImpl< JSSocket >
    {};
    
    template <>
    struct NativeToJS< JSSocket > : v8::juice::cw::NativeToJSImpl< JSSocket >
    {};

    template <>
    struct JSToNative< JSByteArray > : v8::juice::cw::JSToNativeImpl< JSByteArray >
    {};

    
} } } // v8::juice::convert

bool JSSocket::enableDebug = v8::juice::cw::DebugLevel<JSSocket>::Value > 2;

/************************************************************************
   Down here is where the runtime setup parts of the bindings take place...
************************************************************************/

std::string JSSocket::toString() const
{
    std::ostringstream os;
    os << "[object "
       << v8::juice::cw::ClassName<JSSocket>::Value()
       << "@"<<(void const *)this
       << ']';
    return os.str();
}

v8::Handle<v8::Value> JSSocket::sendTo( v8::Arguments const & argv )
{
    if( (argv.Length()<3) || (argv.Length()>4) )
    {
        return v8::ThrowException(JSTR("write() requires 3-4 arguments!"));
    }
    JSSocket * so = cv::CastFromJS<JSSocket>( argv.This() );
    if( ! so )
    {
        cv::StringBuffer msg;
        msg << "Could not find native 'this' argument for "
            << v8::juice::cw::ClassName<JSByteArray>::Value()
            << " object!";
        return v8::ThrowException(msg);
    }
    std::string where = cv::JSToStdString(argv[0]);
    int port = cv::JSToInt32(argv[1]);
    unsigned int len = 0;
    sock_addr_t addr;
    memset( &addr, 0, sizeof( sock_addr_t ) );
    socklen_t alen = 0;
    int rc = 0;
    {
        v8::Unlocker unl;
        rc = create_addr(where.c_str(), port, so->family, &addr, &alen);
    }
    if( 0 != rc )
    {
        v8::Handle<v8::Value> ip = JSSocket::nameToAddress( where.c_str() );
        if( ! ip.IsEmpty() )
        {
            where = cv::JSToStdString(ip);
            v8::Unlocker unl;
            rc = create_addr( where.c_str(), port, so->family, &addr, &alen );
        }
        if( 0 != rc )
        {
            cv::StringBuffer msg;
            msg << "Malformed address: "<<where;
            return v8::ThrowException(msg);
        }
    }

    void const * buf = NULL;
    std::string sval;
    if( argv[2]->IsString() )
    {
        typedef cv::TMemFuncForwarder<JSSocket,1> FF;
        sval = cv::JSToStdString(argv[2]);
        if( argv.Length() > 3 )
        {
            len = cv::JSToUInt32(argv[3]);
            if( len > sval.size() ) len = sval.size();
        }
        else len = sval.size();
        buf = sval.c_str();
    }
    else
    {
        JSByteArray * ba = cv::CastFromJS<JSByteArray>( argv[2] );
        if( ! ba )
        {
            cv::StringBuffer msg;
            msg << "The second argument must be a String or "
                << v8::juice::cw::ClassName<JSByteArray>::Value()
                << '!';
            return v8::ThrowException(msg);
        }
        if( argv.Length() > 3 )
        {
            len = cv::JSToUInt32(argv[3]);
            if( len > ba->length() ) len = ba->length();
        }
        else len = ba->length();
        buf = &ba->vec[0];
    }
    ssize_t sendToRC = -1;
    {
        v8::Unlocker unl;
        sendToRC = ::sendto( so->fd, buf, len, 0, (sockaddr *)&addr, alen );
    }
    if( -1 == sendToRC )
    {
        cv::StringBuffer msg;
        msg << "::sendto("<<so->fd<<"["<<where<<":"<<port<<"], <buffer>, "<<len<<",...) failed: errno="<<errno
            << " ("<<strerror(errno)<<")";
        return v8::ThrowException( msg );
    }
    return cv::CastToJS( rc );
}
v8::Handle<v8::Value> JSSocket::writeN( v8::Arguments const & argv )
{
    if( argv.Length() > 2 )
    {
        return v8::ThrowException(JSTR("write() requires 1 or 2 arguments!"));
    }
    JSSocket * so = cv::CastFromJS<JSSocket>( argv.This() );
    if( ! so )
    {
        cv::StringBuffer msg;
        msg << "Could not find native 'this' argument for "
            << v8::juice::cw::ClassName<JSByteArray>::Value()
            << " object!";
        return v8::ThrowException(msg);
    }
    unsigned int len = 0;
    if( argv[0]->IsString() )
    {
        typedef cv::TMemFuncForwarder<JSSocket,1> FF;
        std::string const val = cv::JSToStdString(argv[0]);
        if( argv.Length() > 1 )
        {
            len = cv::JSToUInt32(argv[1]);
            if( len > val.size() ) len = val.size();
        }
        else len = val.size();
        return cv::CastToJS( so->write2( val.c_str(), len ) );
    }
    else
    {
        JSByteArray * ba = cv::CastFromJS<JSByteArray>( argv[0] );
        if( ! ba )
        {
            cv::StringBuffer msg;
            msg << "The first argument must be a String or "
                << v8::juice::cw::ClassName<JSByteArray>::Value()
                << '!';
            return v8::ThrowException(msg);
        }
        if( argv.Length() > 1 )
        {
            len = cv::JSToUInt32(argv[1]);
            if( len > ba->length() ) len = ba->length();
        }
        else len = ba->length();
        return cv::CastToJS( so->write2( (char const *)&ba->vec[0], len ) );
    }
}
unsigned int JSSocket::write2( char const * src, unsigned int n )
{
    this->hitTimeout = false;
    ssize_t rc = 0;
    {
        v8::Unlocker unl;
        rc = ::write(this->fd, src, n );
        // reminder: ^^^^ affected by socket timeout reminder: through
        // src technically came from v8, it actually lives in a
        // std::string object (as a side-effect of the bindings' type
        // conversions) and therefore is not subject to v8 lifetime
        // issues during our unlocked time. i hope.
    }
    if( (ssize_t)-1 == rc )
    {
        if( (EAGAIN==errno) || (EWOULDBLOCK==errno) )
        { /* Presumably interrupted by a timeout. */
            this->hitTimeout = true;
            rc = 0;
        }
        else
        {
            cv::StringBuffer msg;
            msg << "socket write() failed! errno="<<errno
                << " ("<<strerror(errno)<<")";
            v8::ThrowException( msg );
            rc = 0;
        }
    }
    return (unsigned int)rc;
}


v8::Handle<v8::Value> JSSocket::read( unsigned int n, bool binary )
{
    this->hitTimeout = false;
    typedef std::vector<unsigned char> VT;
    VT vec(n,'\0');
    ssize_t rc = 0;
    sock_addr_t addr;
    memset( &addr, 0, sizeof(sock_addr_t) );
    socklen_t len = 0;
    {
        v8::Unlocker unl;
        //CERR << "read("<<n<<", "<<binary<<")...\n";
#if 1
        rc = ::recvfrom(this->fd, &vec[0], n, 0, (sockaddr *) &addr, &len);
#else
        rc = ::read(this->fd, &vec[0], n);
#endif
        //CERR << "read("<<n<<", "<<binary<<") == "<<rc<<"\n";
        if( 0 == rc ) /*EOF*/ return v8::Undefined();
    }
    if( (ssize_t)-1 == rc )
    {
#if 1
        if( (EAGAIN==errno) || (EWOULDBLOCK==errno) )
        { /* Presumably interrupted by a timeout. */
            this->hitTimeout = true;
            return v8::Undefined();
        }
#endif
        cv::StringBuffer msg;
        msg << "socket read() failed! errno="<<errno
            << " ("<<strerror(errno)<<")";
        v8::ThrowException( msg );
        return v8::Undefined();
    }
    else
    {
        if( (unsigned int) rc < n )
        {
            // dammit... we cannot distinguish timeout from EOF here....
            // this->hitTimeout = true;
        }
#if 1
        if(SOCK_DGRAM == this->type)
        {
            // i'm not quite sure what this is for. It's from the original implementation.
            this->jsSelf->Set( JSTR(socket_strings.fieldPeer),
                               create_peer( (sockaddr*)&addr) );
        }
#endif
        vec.resize( rc );
        if( binary )
        {
            typedef v8::juice::cw::ClassWrap<JSByteArray> CW;
            v8::Handle<v8::Object> jba =
                CW::Instance().NewInstance( 0, NULL );
            JSByteArray * ba = cv::CastFromJS<JSByteArray>( jba );
            if( ! ba )
            {
                cv::StringBuffer msg;
                msg << "Creation of "<<v8::juice::cw::ClassName<JSByteArray>::Value()
                    << " object failed!";
                return v8::ThrowException( msg );
            }
            //ba->length( vec.size() );
            //ba->vec.assign( vec.begin(), vec.end() );
            // ^^^ swap() won't work because of different signedness
            ba->vec.swap( vec );
            return jba;
        }
        else
        {
            return v8::String::New( (char const *)&vec[0], static_cast<int>( vec.size() ) );
        }
    }
}

    
JSSocket * JSSocket::accept()
{
    CERR << "WARNING: accept() is UNTESTED CODE!\n";
    int rc = 0;
    {
        v8::Unlocker unlocker;
        rc = ::accept( this->fd, NULL, NULL );
        // ^^^^ does socket timeout value apply here???
    }
    if( -1 == rc )
    {
        if( (errno == EAGAIN)
            || (errno == EWOULDBLOCK) )
        { /** presumably(?) a timeout (or blocking on a non-blocking socket) */
            return NULL;
        }
        cv::StringBuffer msg;
        msg << "accept() failed: errno="<<errno
            << " ("<<strerror(errno)<<')';
        v8::ThrowException(msg);
        return NULL;
    }
    typedef v8::juice::cw::ClassWrap<JSSocket> CW;
    v8::Handle<v8::Value> argv[] =
        {
        cv::CastToJS( this->family ),
        cv::CastToJS( this->type ),
        cv::CastToJS( this->proto ),
        cv::CastToJS( rc )        
        };
    v8::Handle<v8::Object> jobj =
        CW::Instance().NewInstance( sizeof(argv)/sizeof(argv[0]), argv );
    return cv::CastFromJS<JSSocket*>( jobj );
}

JSByteArray::JSByteArray( v8::Handle<v8::Value> const & val, unsigned int len )
    : vec()
{
    if( !val.IsEmpty()
        && !val->IsNull()
        && !val->IsUndefined()
        )
    {
        if( val->IsNumber() )
        {
            const int32_t x = cv::JSToInt32( val );
            if( x < 0 )
            {
                std::ostringstream msg;
                msg << v8::juice::cw::ClassName<JSByteArray>::Value()
                    << " ctor argument may not be a negative number.";
                throw std::runtime_error( msg.str().c_str() );
            }
            this->length( (unsigned int)x );
        }
        else
        {
            std::string const x = cv::JSToStdString( val );
            if( ! len ) len = x.size();
            if( len > x.size() ) len = x.size();
            if( len )
            {
                this->length( len );
                this->vec.assign( x.begin(), x.end() );
            }
        }
    }
}

v8::Handle<v8::Value> JSByteArray::indexedPropertyGetter(uint32_t index, const v8::AccessorInfo &info)
{
    //CERR << "indexed getter: "<<index<<'\n';
    JSByteArray * ar = cv::CastFromJS<JSByteArray*>( info.This() );
    if( ! ar ) return v8::ThrowException(JSTR("Native 'this' not found!"));
    if( index >= ar->length() ) return v8::Undefined();
    else
    {
        return cv::CastToJS<int>( ar->vec[index] );
    }
}

v8::Handle<v8::Value> JSByteArray::indexedPropertySetter(uint32_t index, v8::Local< v8::Value > value, const v8::AccessorInfo &info)
{
    //CERR << "indexed setter: "<<index<<'\n';
    v8::Handle<v8::Value> rv;
    JSByteArray * ar = cv::CastFromJS<JSByteArray*>( info.This() );
    if( ! ar ) return v8::ThrowException(JSTR("Native 'this' not found!"));
#if 0
    if( index >= ar->length() )
    {
        ar->length( index+1 );
        //CERR << "capacity = "<<ar->vec.capacity()<<'\n';
        //CERR << "size = "<<ar->vec.size()<<'\n';
    }
#endif
    if( index >= ar->length() )
    {
#if 1
        cv::StringBuffer msg;
        msg << "Index "<<index<<" is out of bounds for "
            <<v8::juice::cw::ClassName<JSByteArray>::Value()
            << " of length "<<ar->length()<<'!';
        return v8::ThrowException(msg);
#else
        return rv;
#endif
    }
    else
    {
        return cv::CastToJS<uint16_t>( ar->vec[index] = static_cast<unsigned char>( cv::CastFromJS<uint16_t>(value) ) );
    }
}

v8::Handle<v8::Integer> JSByteArray::indexedPropertyQuery(uint32_t index, const v8::AccessorInfo &info)
{
    CERR << "indexed query "<<index<<'\n';
    JSByteArray * ar = cv::CastFromJS<JSByteArray*>( info.This() );
    if( ! ar ) return v8::Handle<v8::Integer>();
    else
    {
#if 0
        return (index < ar->length())
            ? v8::True()
            : v8::False()
            ;
#else
        return v8::Integer::New(0);
        /*
          The return type of this function changed from Boolean
          sometime in 2010 and the bastards didn't document what the
          new semantics are. They write only that the integer "encodes
          information about the property." Poking around in v8's
          sources seems to imply that any non-empty integer handle is
          treated as "true" here.
         */
#endif
    }
}
v8::Handle<v8::Boolean> JSByteArray::indexedPropertyDeleter(uint32_t index, const v8::AccessorInfo &info)
{
    //CERR << "indexed deleter "<<index<<'\n';
    //CERR << "marker!\n";
    return v8::False();
}
v8::Handle<v8::Array> JSByteArray::indexedPropertyEnumerator(const v8::AccessorInfo &info)
{
    //CERR << "indexed enumerator\n";
    v8::Handle<v8::Array> rv;
    JSByteArray * ar = cv::CastFromJS<JSByteArray*>( info.This() );
    if( ! ar )
    {
        v8::ThrowException(JSTR("Native 'this' not found!"));
        return rv;
    }
    rv = v8::Handle<v8::Array>( v8::Array::New() );
    for( uint32_t i = 0; i < ar->length(); ++i )
    {
        rv->Set( i, cv::CastToJS(i) );
    }
    return rv;
}
std::string JSByteArray::toString() const
{
    std::ostringstream os;
    os << "[object "
       << v8::juice::cw::ClassName<JSByteArray>::Value()
       << "@"<<(void const *)this
       << ", length="<<this->length()
       << ']';
    return os.str();
}

uint32_t JSByteArray::length( uint32_t sz )
{
    //CERR << "length("<<sz<<")!\n";
    if( sz > this->vec.max_size() )
    {
        cv::StringBuffer msg;
        msg << v8::juice::cw::ClassName<JSByteArray>::Value()
            << " length "<<sz << " is too large to store "
            << "in std::vector! Max size is "<< this->vec.max_size()<<".";
        throw std::runtime_error( msg.Content().c_str() );
    }
    if( sz != this->vec.size() )
    {
        this->vec.resize(sz,'\0');
    }
    return this->vec.size();
}

    
void JSByteArray::SetupBindings( v8::Handle<v8::Object> dest )
{
    using namespace v8;
    using namespace v8::juice;
    //HandleScope scope;
    typedef JSByteArray N;
    typedef cw::ClassWrap<N> CW;
    CW & cw( CW::Instance() );
    DBGOUT <<"Binding class "<<CW::ClassName::Value()<<"...\n";
    typedef convert::InvocationCallbackCreator ICC;
    typedef convert::MemFuncInvocationCallbackCreator<N> ICM;

    if( cw.IsSealed() )
    {
        cw.AddClassTo( dest );
        return;
    }

    cw.BindGetterSetter<uint32_t,&N::length,uint32_t,uint32_t,&N::length>("length");
    cw.BindGetter<std::string,&N::stringValue>("stringValue");
    cw
        .Set( "toString", ICM::M0::Invocable<std::string,&N::toString> )
        .Set( "destroy", CW::DestroyObject )
        ;

    v8::Handle<v8::FunctionTemplate> ctorTmpl = cw.CtorTemplate();
    ctorTmpl->InstanceTemplate()->SetIndexedPropertyHandler( JSByteArray::indexedPropertyGetter,
                                                             JSByteArray::indexedPropertySetter,
                                                             JSByteArray::indexedPropertyQuery,
                                                             NULL/*JSByteArray::indexedPropertyDeleter*/,
                                                             NULL/*JSByteArray::indexedPropertyEnumerator*/
                                                             );
    ctorTmpl->Set( JSTR("WTF"), cv::CastToJS("wtf is going on here?") );

    v8::Handle<v8::Function> ctor = cw.Seal();
    cw.AddClassTo( dest );

    DBGOUT <<"Binding done.\n";
    return;
}

/**

   Classes:

   - Socket

   Socket ctor properties:
   
   - int AF_INET
   - int AF_INET6
   - int AF_UNIX
   - int AF_UNSPEC
   - int SOCK_DGRAM
   - int SOCK_STREAM
   - int SOCK_SEQPACKET
   - int SOCK_RAW
   
   Socket class functions:

   - new Socket( [int family=AF_INET [, int type=SOCK_DGRAM [, int protocol=0]]] )
   - void close()
   - string toString()
   - int bind( string hostNameOrAddress, int port )
   - int connect( string hostNameOrAddress, int port )
   - string nameToAddress(string host)
   - string addressToName(string address)
   - string read(unsigned int length)
   - string write(string data [,int length=data.length])
   - int setTimeout( unsigned int seconds[, unsigned int microseconds=0] )

   Most of the functions throw on error.

   Socket instance properties:

   - Array[address,port] peerInfo, only valid after a connection is
   established.
   
   
   Socket constructor properties:

   - int getProtoByName( string protocolName )
   - string nameToAddress( string hostname )
   - string addressToName( string address )
   
*/
void JSSocket::SetupBindings( v8::Handle<v8::Object> dest )
{
    using namespace v8;
    using namespace v8::juice;
    cw::Installer<JSByteArray>::SetupBindings( dest );
    HandleScope scope;
    typedef JSSocket N;
    typedef cw::ClassWrap<N> CW;
    CW & cw( CW::Instance() );
    DBGOUT <<"Binding class "<<CW::ClassName::Value()<<"...\n";
    typedef convert::InvocationCallbackCreator ICC;
    typedef convert::MemFuncInvocationCallbackCreator<N> ICM;

    if( cw.IsSealed() )
    {
        cw.AddClassTo( dest );
        return;
    }


    typedef tmp::TypeList<
    cv::InvocableMemFunc1<N,v8::Handle<v8::Value>,unsigned int,&N::read>,
        cv::InvocableMemFunc2<N,v8::Handle<v8::Value>,unsigned int,bool,&N::read>
        > OverloadsRead;
    typedef tmp::TypeList<
        cv::InvocableMemFunc0<N,int,&N::listen>,
        cv::InvocableMemFunc1<N,int,int,&N::listen>
        > OverloadsListen;
    typedef tmp::TypeList<
        cv::InvocableMemFunc2<N,int,unsigned int,unsigned int,&N::setTimeout>,
        cv::InvocableMemFunc1<N,int,unsigned int,&N::setTimeout>
        > OverloadsSetTimeout;
#if 0
    typedef tmp::TypeList<
    //cv::InvocableMemFunc1<N,unsigned int,char const *,&N::write1>,
    //cv::InvocableMemFunc2<N,unsigned int,char const *,unsigned int,&N::write2>
    cv::InvocableCallback< -1, &JSSocket::writeN >
        > OverloadsWrite;
    typedef tmp::TypeList<
    //cv::InvocableMemFunc2<N,v8::Handle<v8::Value>,int,unsigned int,&N::getOpt>
        cv::InvocableMemFunc1<N,v8::Handle<v8::Value>,int,&N::getOpt>
        > OverloadsGetOpt;
#endif
    cw
        .Set( "close", CW::DestroyObject )
        .Set( "toString", ICM::M0::Invocable<std::string,&N::toString> )
        .Set( "bind", ICM::M2::Invocable<int,char const *,int,&N::bind> )
        .Set( "accept", ICM::M0::Invocable<JSSocket*,&N::accept> )
        .Set( "listen", cv::OverloadInvocables<OverloadsListen>::Invocable )
        .Set( "connect", ICM::M2::Invocable<int,char const *, int,&N::connect> )
        .Set( "nameToAddress", ICC::F1::Invocable< v8::Handle<v8::Value> , const char *,&N::nameToAddress> )
        .Set( "addressToName", ICC::F1::Invocable< v8::Handle<v8::Value> , const char *,&N::addressToName> )
        .Set( "read", cv::OverloadInvocables<OverloadsRead>::Invocable )
        .Set( "write",
              //cv::OverloadInvocables<OverloadsWrite>::Invocable
              cv::InvocableCallback< -1, &JSSocket::writeN >::Invocable
              )
        .Set( "sendTo", cv::InvocableCallback< -1, &JSSocket::sendTo >::Invocable )
        .Set( "setTimeout", cv::OverloadInvocables<OverloadsSetTimeout>::Invocable )
        //.Set( "setOpt", ICM::M2::Invocable< int, int, int, &N::setOpt > )
        //.Set( "getOpt", convert::OverloadInvocables<OverloadsGetOpt>::Invocable )
        .Set( socket_strings.fieldPeer, v8::False() )
        ;
    cw.BindMemVarRO<int,&N::family>( "family" );
    cw.BindMemVarRO<int,&N::proto>( "proto" );
    cw.BindMemVarRO<int,&N::type>( "type" );
    cw.BindMemVarRO<bool,&N::hitTimeout>( "timeoutBeforeIO" );
    cw.BindGetter< v8::Handle<v8::Value>, &N::peerInfo >( "peerInfo" );
    cw.BindGetter< std::string, &N::hostname >("hostname");

    v8::Handle<v8::Function> ctor = cw.Seal();
#define SETInt(K) ctor->Set( JSTR(#K), v8::Integer::New(K) )
    SETInt(AF_INET);
    SETInt(AF_INET6);
    SETInt(AF_UNIX);
    SETInt(AF_UNSPEC);
    SETInt(SOCK_DGRAM);
    SETInt(SOCK_STREAM);
    SETInt(SOCK_SEQPACKET);
    SETInt(SOCK_RAW);
#undef SETInt

    v8::InvocationCallback cb; cb = NULL;
#define JF v8::FunctionTemplate::New(cb)->GetFunction()
#define F(X) ctor->Set( JSTR(X), JF )

    //cb = ICC::F0::Invocable<std::string,&N::hostname>;
    ctor->Set( JSTR("hostname"), cv::CastToJS( N::hostname()), v8::ReadOnly );

    cb = ICC::F1::Invocable<int,char const *,&N::getProtoByName>;
    F("getProtoByName");

    //cb = ICC::F2::Invocable< v8::Handle<v8::Value>,char const *,int,&N::nameToAddress>;
    //F("nameToAddress");

    typedef tmp::TypeList<
        convert::InvocableFunction1<v8::Handle<v8::Value>,char const *,&N::nameToAddress>,
        convert::InvocableFunction2<v8::Handle<v8::Value>,char const *,int,&N::nameToAddress>
        > GetAddrInfoList;
    cb = convert::OverloadInvocables<GetAddrInfoList>::Invocable;
    F("nameToAddress");

    typedef tmp::TypeList<
        convert::InvocableFunction1<v8::Handle<v8::Value>,char const *,&N::addressToName>,
        convert::InvocableFunction2<v8::Handle<v8::Value>,char const *,int,&N::addressToName>
        > AddressToName;
    cb = convert::OverloadInvocables<AddressToName>::Invocable;
    F("addressToName");

    cw.AddClassTo( dest );
    DBGOUT <<"Binding done.\n";
    return;
}
#undef DBGOUT
#undef JSTR


static v8::Handle<v8::Value> JSSocket_plugin_init( v8::Handle<v8::Object> dest )
{
    v8::juice::cw::Installer<JSSocket>::SetupBindings( dest );
    return dest;
}

V8_JUICE_PLUGIN_STATIC_INIT(JSSocket_plugin_init);
