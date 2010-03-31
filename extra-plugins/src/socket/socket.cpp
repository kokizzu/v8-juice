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
      case PF_UNIX: {
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
      case PF_INET: {
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
      case PF_INET6: {
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
          result->Set(cv::CastToJS<int>(0), cv::CastToJS<char const *>(addr_un->sun_path));
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
                        
          result->Set(0U, cv::CastToJS(buf));
          result->Set(1, cv::CastToJS( ntohs(addr_in6->sin6_port)));
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
          result->Set(0U, cv::CastToJS(buf));
          result->Set(1, cv::CastToJS(ntohs(addr_in->sin_port)));
          return result;
      } break;
    }
    return v8::Undefined();
}

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
    /** JS-side 'this' object. Set by v8::juice::cw::WeakWrap<JSSocket>::Wrap(). */
    v8::Handle<v8::Object> jsSelf;
private:
    void init(int family, int type, int proto, int socketFD )
    {
        this->family = family;
        this->proto = proto;
        this->type = type;
        if( ! socketFD )
        {
            this->fd = socket( family, type, proto );
        }
        else
        {
            this->fd = socketFD;
        }
        if( this->fd <= 0 )
        {
            cv::StringBuffer msg;
            msg << "socket("<<family<<", "<<type<<", "<<proto<<") failed. errno="<<errno<<'.';
            throw std::runtime_error( msg.Content().c_str() );
        }
    }
public:
#define DBGOUT if(JSSocket::enableDebug) CERR

    JSSocket(int family = AF_INET, int type = SOCK_DGRAM, int proto = 0, int socketFD = 0 )
        : fd(-1), family(0), proto(0),type(0),
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
       where == host address

       FIXME: support where==hostname
    */
    int bind( char const * where, int port )
    {
        if( ! where || !*where )
        {
            v8::ThrowException(JSTR("Address may not be empty!"));
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
                msg << "Malformed address: ["<<where<<"]";
            }
        }
        else
        {
            {
                v8::Unlocker unl;
                rc = ::bind( this->fd, (sockaddr *)&addr, len );
            }
            if( 0 != rc )
            {
                msg << "Bind failed with rc "<<rc<<" ("<<strerror(errno)<<")!";
            }
        }
        if( 0 != rc )
        {
            v8::ThrowException( msg );
        }
        return rc;
    }


    int listen( int backlog )
    {
        CERR << "WARNING: UNTESTED CODE!\n";
        int rc = ::listen( this->fd, backlog );
        if( 0 != rc )
        {
            cv::StringBuffer msg;
            msg << "connect() failed: errno="<<strerror(errno);
            v8::ThrowException(msg);
        }
        return rc;
    }

    int listen()
    {
        return this->listen( 5 );
    }

    /**
       accept(2)'s a connection and creates a new socket
       for it. The returned object is owned by v8.
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
        PF_INET is assumed. */
    static v8::Handle<v8::Value> addressToName( char const * addy, int family )
    {
        if( ! addy || !*addy )
        {
            return v8::ThrowException(JSTR("'address' parameter may not be empty!"));
        }
        if( ! family ) family = PF_INET;
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
        {
            v8::Unlocker unl;
            rc = ::connect( this->fd, (sockaddr *)&addr, len );
        }
        if( 0 != rc )
        {
            cv::StringBuffer msg;
            msg << "connect() failed: errno="<<strerror(errno);
            v8::ThrowException(msg);
            return rc;
        }
        else
        {
            DBGOUT << "Connected to ["<<where<<':'<<port<<"]\n";
        }
        return 0;
    }
    
    bool connect( char const * addr )
    {
        // TODO: do not require port # for AF_UNIX?
        return false;
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
       as a string (thus binary data will not work
       properly!). Special return values:

       - (val===null) means reading was interrupted
       by a timeout and no bytes were read before the
       timeout expired.

       - (val===undefined) means that a read error
       occured, but this function will throw
       a v8 exception in that case.

       When the length of the returned string is less than n, it
       could mean any of:

       - Timeout was reached after reading some bytes.

       - EOF
    */
    v8::Handle<v8::Value> read( unsigned int n )
    {
        typedef std::vector<char> VT;
        VT vec(n+1,'\0');
        //vec.reserve(n+1);
        ssize_t rc = 0;
        sock_addr_t addr;
        memset( &addr, 0, sizeof(sock_addr_t) );
        socklen_t len = 0;
        {
            v8::Unlocker unl;
#if 1
            rc = ::recvfrom(this->fd, &vec[0], n, 0, (sockaddr *) &addr, &len);
#else
            rc = ::read(this->fd, &vec[0], n);
#endif
        }
        if( (ssize_t)-1 == rc )
        {
            if( (EAGAIN==errno) || (EWOULDBLOCK==errno) )
            { /* Presumably interrupted by a timeout. */
                return v8::Null();
            }
            else
            {
                cv::StringBuffer msg;
                msg << "socket read() failed! errno="<<errno
                    << " ("<<strerror(errno)<<")";
                v8::ThrowException( msg );
                return v8::Undefined();
            }
        }
        else
        {
            // FIXME: provide a way to return binary data as an array of ints.
            if(SOCK_DGRAM == this->type)
            {
                // i'm not quite sure what this is for. It's from the original implementation.
                this->jsSelf->Set( JSTR(socket_strings.fieldPeer), create_peer( (sockaddr*)&addr) );
            }
            return cv::CastToJS( &vec[0] );
        }
    }

    unsigned int write( char const * src, unsigned int n )
    {
        ssize_t rc = 0;
        {
            v8::Unlocker unl;
            // ^^^ this _might_ not be legal, since src is owned by v8
            // (but it's ArgCaster container is valid until this func
            // returns).
            rc = ::write(this->fd, src, n );
        }
        if( (ssize_t)-1 == rc )
        {
            if( (EAGAIN==errno) || (EWOULDBLOCK==errno) )
            { /* Presumably interrupted by a timeout. */
                rc = 0;
            }
            else
            {
                cv::StringBuffer msg;
                msg << "socket write() failed! errno="<<errno
                    << " ("<<strerror(errno)<<")";
                v8::ThrowException( msg );
            }
        }
        return (unsigned int)rc;
    }
    unsigned int write( char const * src )
    {
        return src
            ? this->write( src, strlen(src) )
            : 0;
    }

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
                msg << "getpeername() failed! errno="<<errno
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
    
}/*JSSocket*/;


/**
   Not yet complete.
*/
class JSByteArray
{
public:
    typedef std::vector<unsigned char> BufferType;
    BufferType vec;
    JSByteArray( unsigned int size = 0 )
        : vec( size, 0 )
    {
    }
    ~JSByteArray()
    {
    }
    uint32_t length() const
    {
        CERR << "length()\n";
        return this->vec.size();
    }
    /** toString() for JS. */
    std::string toString() const;

    static v8::Handle<v8::Value> indexedPropertyGetter(uint32_t index, const v8::AccessorInfo &info);
    static v8::Handle<v8::Value> indexedPropertySetter(uint32_t index, v8::Local< v8::Value > value, const v8::AccessorInfo &info);
    static v8::Handle<v8::Boolean> indexedPropertyQuery(uint32_t index, const v8::AccessorInfo &info);
    static v8::Handle<v8::Boolean> indexedPropertyDeleter(uint32_t index, const v8::AccessorInfo &info);
    static v8::Handle<v8::Array> indexedPropertyEnumerator(const v8::AccessorInfo &info);
    static void SetupBindings( v8::Handle<v8::Object> dest );
};

#include <v8/juice/ClassWrap_TwoWay.h>
////////////////////////////////////////////////////////////////////////
// Set up our ClassWrap policies...
namespace v8 { namespace juice { namespace cw
{
   
    template <> struct DebugLevel<JSSocket> : Opt_Int<3> {};

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
           Installs the bindings for PathFinder into the given object.
        */
        static void SetupBindings( ::v8::Handle< ::v8::Object> target )
        {
            JSSocket::SetupBindings(target);
        }
    };

    template <>
    struct ToNative_SearchPrototypesForNative<JSByteArray>
        : Opt_Bool<true>
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
            cv::CtorForwarder1<JSByteArray,unsigned int>
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

JSSocket * JSSocket::accept()
{
    CERR << "WARNING: UNTESTED CODE!\n";
    int rc = ::accept( this->fd, NULL, NULL );
    if( -1 == rc )
    {
        cv::StringBuffer msg;
        msg << "connect() failed: errno="<<strerror(errno);
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
    v8::Handle<v8::Object> jobj = CW::Instance().NewInstance(4,argv);
    return cv::CastFromJS<JSSocket*>( jobj );
}

v8::Handle<v8::Value> JSByteArray::indexedPropertyGetter(uint32_t index, const v8::AccessorInfo &info)
{
    CERR << "index getter: "<<index<<'\n';
    JSByteArray * ar = cv::CastFromJS<JSByteArray*>( info.This() );
    if( ! ar ) return v8::ThrowException(JSTR("Native 'this' not found!"));
    if( index >= ar->vec.size() ) return v8::Undefined();
    else
    {
        return cv::CastToJS<int>( ar->vec[index] );
    }
}
v8::Handle<v8::Value> JSByteArray::indexedPropertySetter(uint32_t index, v8::Local< v8::Value > value, const v8::AccessorInfo &info)
{
    CERR << "index setter: "<<index<<'\n';
    v8::Handle<v8::Value> rv;
    JSByteArray * ar = cv::CastFromJS<JSByteArray*>( info.This() );
    if( ! ar ) return v8::ThrowException(JSTR("Native 'this' not found!"));
    if( index >= ar->vec.size() )
    {
        ar->vec.reserve( index+1 );
        CERR << "size = "<<ar->vec.size()<<'\n';
    }
    if( index >= ar->vec.size() )
    {
        return rv;
    }
    else
    {
        return cv::CastToJS<uint16_t>( ar->vec[index] = static_cast<unsigned char>( cv::CastFromJS<uint16_t>(value) ) );
    }
}

v8::Handle<v8::Boolean> JSByteArray::indexedPropertyQuery(uint32_t index, const v8::AccessorInfo &info)
{
    JSByteArray * ar = cv::CastFromJS<JSByteArray*>( info.This() );
    if( ! ar ) return v8::Handle<v8::Boolean>();
    else
    {
        return (index < ar->vec.size())
            ? v8::True()
            : v8::False()
            ;
    }
}
v8::Handle<v8::Boolean> JSByteArray::indexedPropertyDeleter(uint32_t index, const v8::AccessorInfo &info)
{
    //CERR << "marker!\n";
    return v8::False();
}
v8::Handle<v8::Array> JSByteArray::indexedPropertyEnumerator(const v8::AccessorInfo &info)
{
    CERR << "marker!\n";
    v8::Handle<v8::Array> rv;
    JSByteArray * ar = cv::CastFromJS<JSByteArray*>( info.This() );
    if( ! ar )
    {
        v8::ThrowException(JSTR("Native 'this' not found!"));
        return rv;
    }
    rv = v8::Handle<v8::Array>( v8::Array::New() );
    for( uint32_t i = 0; i < ar->vec.size(); ++i )
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
       << ']';
    return os.str();
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


    cw.BindGetter<uint32_t,&N::length>("length");
    cw.Set( "toString", ICM::M0::Invocable<std::string,&N::toString> );

    v8::Handle<v8::FunctionTemplate> ctorTmpl = cw.CtorTemplate();
    ctorTmpl->InstanceTemplate()->SetIndexedPropertyHandler( JSByteArray::indexedPropertyGetter,
                                                             JSByteArray::indexedPropertySetter,
                                                             JSByteArray::indexedPropertyQuery,
                                                             JSByteArray::indexedPropertyDeleter,
                                                             JSByteArray::indexedPropertyEnumerator
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
   - int setTimeout( unsigned int seconds, unsigned int microseconds )

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
    JSByteArray::SetupBindings( dest );
    using namespace v8;
    using namespace v8::juice;
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
            cv::InvocableMemFunc1<N,unsigned int,char const *,&N::write>,
            cv::InvocableMemFunc2<N,unsigned int,char const *,unsigned int,&N::write>
        > OverloadsWrite;
    typedef tmp::TypeList<
    //cv::InvocableMemFunc2<N,v8::Handle<v8::Value>,int,unsigned int,&N::getOpt>
        cv::InvocableMemFunc1<N,v8::Handle<v8::Value>,int,&N::getOpt>
        > OverloadsGetOpt;
    typedef tmp::TypeList<
        cv::InvocableMemFunc0<N,int,&N::listen>,
        cv::InvocableMemFunc1<N,int,int,&N::listen>
        > OverloadsListen;

    cw
        .Set( "close", CW::DestroyObject )
        .Set( "toString", ICM::M0::Invocable<std::string,&N::toString> )
        .Set( "bind", ICM::M2::Invocable<int,char const *,int,&N::bind> )
        .Set( "listen", cv::OverloadInvocables<OverloadsListen>::Invocable )
        .Set( "connect", ICM::M2::Invocable<int,char const *, int,&N::connect> )
        .Set( "nameToAddress", ICC::F1::Invocable< v8::Handle<v8::Value> , const char *,&N::nameToAddress> )
        .Set( "addressToName", ICC::F1::Invocable< v8::Handle<v8::Value> , const char *,&N::addressToName> )
        .Set( "read", ICM::M1::Invocable< v8::Handle<v8::Value>, unsigned int,&N::read> )
        .Set( "write", cv::OverloadInvocables<OverloadsWrite>::Invocable )
        //.Set( "peerInfo", ICM::M0::Invocable< v8::Handle<v8::Value>, &N::peerInfo > )
        .Set( "setTimeout", ICM::M2::Invocable< int, unsigned int,unsigned int, &N::setTimeout> )
        //.Set( "setOpt", ICM::M2::Invocable< int, int, int, &N::setOpt > )
        //.Set( "getOpt", convert::OverloadInvocables<OverloadsGetOpt>::Invocable )
        .Set( socket_strings.fieldPeer, cv::BoolToJS(false) )
        ;
    cw.BindMemVarRO<int,&N::family>( "family" );
    cw.BindMemVarRO<int,&N::proto>( "proto" );
    cw.BindMemVarRO<int,&N::type>( "type" );
    cw.BindGetter< v8::Handle<v8::Value>, &N::peerInfo >( "peerInfo" );
    cw.BindGetter<std::string,&N::hostname>("hostname");

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
