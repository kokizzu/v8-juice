/* auto-generated on Sat Mar  7 19:32:28 CET 2009. Do not edit! */
#define WHEFS_AMALGAMATION_BUILD 1
#if ! defined __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS 1
#endif
/* begin file whio_config.h */
#ifndef WANDERINGHORSE_NET_WHIO_CONFIG_H_INCLUDED
#define WANDERINGHORSE_NET_WHIO_CONFIG_H_INCLUDED 1
/*
  Common configuration options for libwhio. Parts of the
  library can be changed/configured by editing this file.

  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/

#include <stdint.h> /* uintXX_t */
#include <inttypes.h> /* PRIuXX macros */


#ifndef __cplusplus
/* Try to find a usable bool, or make one up ... */
#  ifndef WHIO_HAVE_STDBOOL
#    define WHIO_HAVE_STDBOOL 1
#  endif
#  if defined(WHIO_HAVE_STDBOOL) && !(WHIO_HAVE_STDBOOL)
#    if !defined(bool)
#      define bool char
#    endif
#    if !defined(true)
#      define true 1
#    endif
#    if !defined(false)
#      define false 0
#    endif
#  else /* aha! stdbool.h! C99. */
#    include <stdbool.h>
#  endif /* WHIO_HAVE_STDBOOL */
#endif /* __cplusplus */


#ifdef __cplusplus
extern "C" {
#endif

#if !defined(WHIO_DEBUG_ENABLED)
#  define WHIO_DEBUG_ENABLED 0
#endif

/** @def WHIO_DEBUG

  WHIO_DEBUG is a printf-like macro which is used for internal debugging
  of the whio library. If compiled with WHIO_DEBUG_ENABLED set to 0 then
  all debuggering output is disabled.
*/
#if WHIO_DEBUG_ENABLED
#  include <stdio.h> // printf()
#  define WHIO_DEBUG if(WHIO_DEBUG_ENABLED) printf("WHIO_DEBUG: %s:%d:%s():\n",__FILE__,__LINE__,__func__); if(WHIO_DEBUG_ENABLED) printf
#else
    extern void whio_noop_printf(char const * fmt, ...);
#  define WHIO_DEBUG if(0) whio_noop_printf
#endif


/** @def WHIO_USE_STATIC_MALLOC
   Changing this only has an effect when building this library
   or when building extensions which want to follow these
   conventions...

   If WHIO_USE_STATIC_MALLOC is true then certain operations
   which are normally done via malloc() and free() are instead
   done first via a static list of pre-allocated objects and then
   (if the list is all used up) fall back malloc()/free().
   This i not strictly thread-safe, but for some use cases this isn't
   significant.

   When using this we might not actually save much dynamic memory
   (e.g.  whio_dev is only 12 bytes per object on 32-bit platforms),
   but we potentially save many calls to malloc(). That depends on the
   application, of course, but this idea was implemented for libwhefs,
   where keeping mallocs down is a goal and we create an unusually
   high number of whio_dev objects. In that lib, we were able to cut a
   typical use case from 19 whio-related mallocs down to 1 (and that
   one happend in fopen(), beneath the whio_dev FILE handler, so we
   couldn't avoid it).

   Note that this approach to allocation is inherently not
   thread-safe, so if you need to create/destroy whio_dev objects from
   multiple threads, do not build with this option turned on. If you
   only create and destroy whio_dev objects within a single thread,
   this option can potentially save many calls to malloc() (and should
   perform much better).
*/
#if !defined(WHIO_USE_STATIC_MALLOC)
#  define WHIO_USE_STATIC_MALLOC 0
#endif

#if defined(WHIO_SIZE_T_BITS)
# error "WHIO_SIZE_T_BITS must not be defined before including this file! Edit this file instead!"
#endif

/** @def WHIO_SIZE_T_BITS

    WHIO_SIZE_T_BITS defines the number of bits used by whio's primary
    unsigned interger type. This is configurable so that certain
    client code (*cough* libwhefs *cough*) can use whio without having
    to fudge certain numeric types.
*/
#define WHIO_SIZE_T_BITS 64

/** @def WHIO_SIZE_T_PFMT

    Is is a printf-style specifier, minus the '%' prefix, for
    use with whio_size_t arguments. It can be used like this:

    @code
    whio_size_t x = 42;
    printf("The value of x is %"WHIO_SIZE_T_PFMT".", x );
    @endcode

    Using this constant ensures that the printf-style commands
    work when whio_size_t is of varying sizes.

    @see WHIO_SIZE_T_SFMT
*/

/** @def WHIO_SIZE_T_SFMT

WHIO_SIZE_T_SFMT is the scanf counterpart of WHIO_SIZE_T_PFMT.

@see WHIO_SIZE_T_PFMT
*/

/** typedef some_unsigned_int_type_which_is_WHIO_SIZE_T_BITS_long whio_size_t

whio_size_t is a configurable unsigned integer type specifying the
ranges used by this library. Its exact type depends on the value of
WHIO_SIZE_T_BITS: it will be uintXX_t, where XX is the value of
WHIO_SIZE_T_BITS (8, 16, 32, or 64).

We use a fixed-size numeric type, instead of relying on a standard type
with an unspecified size (e.g. size_t) to help avoid nasty surprises when
porting to machines with different size_t sizes.
*/

#if WHIO_SIZE_T_BITS == 8
#  warning "You're insane! You're just ASKING for overflows!"
#  define WHIO_SIZE_T_PFMT PRIu8
#  define WHIO_SIZE_T_SFMT SCNu8
    typedef uint8_t whio_size_t;
#elif WHIO_SIZE_T_BITS == 16
#  define WHIO_SIZE_T_PFMT PRIu16
#  define WHIO_SIZE_T_SFMT SCNu16
    typedef uint16_t whio_size_t;
#elif WHIO_SIZE_T_BITS == 32
#  define WHIO_SIZE_T_PFMT PRIu32
#  define WHIO_SIZE_T_SFMT SCNu32
    typedef uint32_t whio_size_t;
#elif WHIO_SIZE_T_BITS == 64
#  define WHIO_SIZE_T_PFMT PRIu64
#  define WHIO_SIZE_T_SFMT SCNu64
    typedef uint64_t whio_size_t;
#else
#  error "WHIO_SIZE_T_BITS must be one of: 8, 16, 32, 64"
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WANDERINGHORSE_NET_WHIO_CONFIG_H_INCLUDED */
/* begin file whio_common.h */
#ifndef WANDERINGHORSE_NET_WHIO_COMMON_H_INCLUDED
#define WANDERINGHORSE_NET_WHIO_COMMON_H_INCLUDED
/*
  Common API declarations for the whio API.

  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/

//#include <stdio.h>
//#include <unistd.h> /* off_t on Linux? */
#include <stdint.h> /* uint32_t */
#include <stdarg.h> /* va_list */



#ifdef __cplusplus
extern "C" {
#endif

/** @struct whio_rc_t

   Functions in the api which return an int almost always return a
   value from the whio_rc object. All members of this type must have
   a non-zero value, except for the OK member, which must be 0. The
   values and signedness for all error values is unspecified except
   for SizeTError, which is defined as -1.
*/
typedef struct whio_rc_t
{
    /**
       The non-error value, always equals 0.
    */
    int OK;

    /**
       Error in argument handling (e.g. unexpected arg type, count,
       etc.).
     */
    int ArgError;

    /**
       Read or write error.
    */
    int IOError;

    /**
       Memory allocation error.
    */
    int AllocError;

    /**
       An internal error in the API.
    */
    int InternalError;

    /**
       An out-of-range error. e.g. wrong size or value.
    */
    int RangeError;

    /**
       A requested resource could not be accessed, or write
       permissions are required but denied.
    */
    int AccessError;

    /**
       A data consistency error (or a bug makes the data look
       corrupted even though it is not).
    */
    int ConsistencyError;

    /**
       Indicates that the called routine is not yet implemented.
    */
    int NYIError;

    /**
       Indicates that the requested option or operation is not
       supported.
    */
    int UnsupportedError;

    /**
       Indicates some form of type mismatch or an unexpected type.
    */
    int TypeError;

    /**
       This is equivalent to (whio_size_t)-1, and is used by routines which
       need an error value for a whio_size_t object.
    */
    whio_size_t SizeTError;
} whio_rc_t;

/** @var whio_rc
   A shared instance of whio_rc_t which contains the "official" values
   of the common error codes for the whio API.
*/
extern const whio_rc_t whio_rc;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WANDERINGHORSE_NET_WHIO_COMMON_H_INCLUDED */
/* begin file whprintf.h */
#ifndef WHPRINTF_H_INCLUDED_
#define WHPRINTF_H_INCLUDED_ 1
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif
/**@page whprintf_page_main whprintf: generic printf-like utilities

   This API contains a printf-like implementation which supports
   aribtrary data destinations.

   Authors: many, probably. This code supposedly goes back to the
   early 1980's.

   Current maintainer: Stephan Beal (http://wanderinghorse.net/home/stephan)

   License: Public Domain.

   The primary functions of interest are whprintfv() and whprintf(), which works
   similarly to printf() except that they take a callback function which they
   use to send the generated output to arbitrary destinations. e.g. one can
   supply a callback to output formatted text to a UI widget or a C++ stream
   object.
*/

/**
   @typedef long (*whprintf_appender)( void * arg, char const * data, long n )


   The whprintf_appender typedef is used to provide whprintfv()
   with a flexible output routine, so that it can be easily
   send its output to arbitrary targets.

   The policies which implementations need to follow are:

   - arg is an implementation-specific pointer (may be 0) which is
   passed to vappendf. whprintfv() doesn't know what this argument is
   but passes it to its whprintf_appender. Typically it will be an
   object or resource handle to which string data is pushed or output.

   - The 'data' parameter is the data to append. If it contains
   embedded nulls, this function will stop at the first one. Thus
   it is not binary-safe.

   - n is the number of bytes to read from data. If n<0 then
   strlen(data) should be used.

   - Returns, on success, the number of bytes appended (may be 0).

   - Returns, on error, an implementation-specified negative number.
   Returning a negative error code will cause whprintfv() to stop the
   processing of that string. Note that 0 is a success value (some
   printf format specifiers do not add anything to the output).
*/
typedef long (*whprintf_appender)( void * arg,
				   char const * data,
				   long n );


/**
  This function works similarly to classical printf implementations,
  but instead of outputing somewhere specific, it uses a callback
  function to push its output somewhere. This allows it to be used for
  arbitrary external representations. It can be used, for example, to
  output to an external string, a UI widget, or file handle (it can
  also emulate printf by outputing to stdout this way).

 INPUTS:

 pfAppend : The is a whprintf_appender function which is responsible
 for accumulating the output. If pfAppend returns a negative integer
 then processing stops immediately.

 pfAppendArg : is ignored by this function but passed as the first
 argument to pfAppend. pfAppend will presumably use it as a data
 store for accumulating its string.

 fmt : This is the format string, as in the usual printf().

 ap : This is a pointer to a list of arguments.  Same as in
 vprintf() and friends.


 OUTPUTS:

 The return value is the total number of characters sent to the
 function "func", or a negative number on a pre-output error. If this
 function returns an integer greater than 1 it is in general
 impossible to know if all of the elements were output. As such
 failure can only happen if the callback function returns an error,
 and this type of error is very rare in a printf-like context, this is
 not considered to be a significant problem. (The same is true for any
 classical printf implementations, as far as i'm aware.)


 CURRENT (documented) PRINTF EXTENSIONS:

 %%z works like %%s, but takes a non-const (char *) and vappendf
 deletes the string (using free()) after appending it to the output.

 %%h (HTML) works like %s but converts certain characters (like '<' and '&' to
 their HTML escaped equivalents.

 %%t (URL encode) works like %%s but converts certain characters into a representation
 suitable for use in an HTTP URL. (e.g. ' ' gets converted to %%20)

 %%T (URL decode) does the opposite of %t - it decodes URL-encoded
 strings.

 %%r requires an int and renders it in "ordinal form". That is,
 the number 1 converts to "1st" and 398 converts to "398th".

 %%q quotes a string as required for SQL. That is, '\'' characters get
 doubled.

 %%Q as %%q, but includes the outer '\'' characters and null pointers
 replaced by SQL NULL.

 (The %%q and %%Q specifiers are options inherited from this printf
 implementation's sqlite3 genes.)

 These extensions may be disabled by setting certain macros when
 compiling vappendf.c (see that file for details).
*/
long whprintfv(
  whprintf_appender pfAppend,          /* Accumulate results here */
  void * pfAppendArg,                /* Passed as first arg to pfAppend. */
  const char *fmt,                   /* Format string */
  va_list ap                         /* arguments */
  );

/**
   Identical to whprintfv() but takes a (...) ellipses list instead of a
   va_list.
*/
long whprintf(whprintf_appender pfAppend,
	     void * pfAppendArg,
	     const char *fmt,
	     ... );

/**
   Emulates fprintf() using whprintfv().
*/
//long whprintf_file( FILE * fp, char const * fmt, ... );


/**
   Works like whprintfv(), but appends all output to a
   dynamically-allocated string, expanding the string as necessary to
   collect all formatted data. The returned null-terminated string is
   owned by the caller and it must be cleaned up using free(). If !fmt
   or if the expanded string evaluates to empty, null is returned, not
   a 0-byte string.
*/
char * whprintfv_str( char const * fmt, va_list vargs );

/**
   Equivalent to whprintfv_str(), but takes elipsis arguments instead
   of a va_list.
*/
char * whprintf_str( char const * fmt, ... );

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* WHPRINTF_H_INCLUDED_ */
/* begin file whio_dev.h */
#ifndef WANDERINGHORSE_NET_WHIO_DEV_H_INCLUDED
#define WANDERINGHORSE_NET_WHIO_DEV_H_INCLUDED

#include <stdio.h> /* FILE */
#include <stdint.h> /* uint32_t */
#include <stdarg.h> /* va_list */
#include <stddef.h> /* ??? */
#include <stdarg.h> /* va_list */

#include <sys/types.h> /* off_t on Linux */

/*
   This file contains declarations and documentation for the generic
   whio_dev API. The specific iodevice implementations are declared
   in whio_devs.h.
*/

/** @page page_whio_dev whio_dev I/O API

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: Public Domain

   whio encapsulates an i/o device API. It originally developed as
   parts of two another library, but the parts were found to be generic
   enough (and complementary enough) to fork out into a single library.

   The API provides interfaces for working with random-access devices
   (via the whio_dev interface) and sequential streams (via the
   whio_stream interface).

   Features:

   - Easy to use - modelled after the standard FILE API.

   - Comes with back-end implementations supporting FILE handles,
   dynamic memory buffers (with the option to expand on demand),
   and client-specified memory ranges (similar to mmap()).

   - Utility functions for doing common things, such as finding the
   size of a device, encoding/decoding certain basic types, and doing
   printf-style formatted output to devices.

   - An ioctl-like interface to perform some device-specific operations
   (for those times when genericness just won't do the job).


   Requirements:

   - Some C99 features are used, but those which are are normally
   enabled by compilers even outside of C99 mode. (This of course
   depends on the compiler version.)

   - It unfortunately needs fileno() in one place. fileno() is not
   specified by C89, but is found in most (all?) UNIX environments and
   is specified by POSIX. (It is apparently impossible to truncate
   a file without either the file's name or file handle number, as opposed
   to via a FILE handle.)


   This code has been shown to compile and pass basic sanity checks on:

   - Linux x86/32 with gcc 4.3.x and tcc
   - Linux x86/64 with gcc 4.2.x and tcc
   - Solaris 10 on sparcv9 (v240) with gcc 3.4.3 and SunStudio 12

   Example:
@code
char const * fname = "myfile.out";
whio_dev * dev = whio_dev_for_filename( fname, "r+" );
dev->api->write( dev, "hi, world!\n", 11 ); // 11 = size of the data bytes
dev->api->finalize( dev ); // flush and close device

// Now read it back:
dev = whio_dev_for_filename( fname, "r" );
enum { bufSize = 1024 };
char buf[bufSize]; // input buffer
whio_size_t n = 0;
while( 0 != (n = dev->api->read( dev, buf, bufSize ) ) )
{
    printf("Read %u bytes\n", n );
}
dev->api->finalize(dev);
@endcode

*/

#ifdef __cplusplus
extern "C" {
#endif


struct whio_dev;

/**
   whio_dev_api defines the functions for the whio_dev interface.
   Set the documentation for whio_dev for why this was abstracted
   into a separate class from whio_dev.

   Each whio_dev object has an associated whio_dev_api object. In
   practice, all instances of a specific class of whio_dev share a
   single instance of this class as their 'api' member.

   This type is not intended to be instantiated and used by client
   code.
*/
struct whio_dev_api
{
    /**
       Reads, at most, n bytes from the underlying device and writes
       them to dest. The read number of bytes is returned, or 0 at
       EOF. A short read normally indicates EOF was reached, but can
       also be the result of an i/o error.

       On a non-i/o error (e.g. !dev or !dest), implementations should
       return 0.
    */
    whio_size_t (*read)( struct whio_dev * dev, void * dest, whio_size_t n );

    /**
       Writes n bytes from src to dev, returning the number of
       bytes written. On error, the return value will differ from
       n.

       On a non-i/o error (e.g. !dev or !dest), implementations should
       return 0.
    */
    whio_size_t (*write)( struct whio_dev * dev, void const * src, whio_size_t n );

    /**
       Must close the i/o device, such that reading and writing are
       no longer possible. It is not generically possible to re-open
       a closed device.

       This routine should also deallocate any resources associated
       with the device (but not dev itself). If this routine does not
       (i.e., it defers that to finalize()) then stack-allocated
       whio_dev objects cannot be cleaned up uniformly.

       If dev->client.dtor is not null then this routine must call
       that function and pass it dev->client.data. If it is null then
       dev->client.data must not be modified (the lack of a destructor
       function is a signal that the client owns the object).

       This interface requires that close() must be called from the
       finalize() member and that close() must flush the device (if
       needed), so client code normally does not need to call
       close(). It is provided so that whio_dev objects which are
       created on the stack (which would be unusual but possibly
       useful for some cases) can be properly cleaned up.

       Ownership of the underlying native device is defined by the
       factory function (or similar) which creates the whio_dev
       wrapper.

       This function must return true if it could close the device
       or false if it cannot (e.g. if !dev or it was not opened).
     */
    bool (*close)( struct whio_dev * dev );

    /**
       Must call close() and then free dev. After calling this, dev is
       no longer a valid object.  If passed null, the results are
       undefined (but implementations are incouraged to ignore it,
       possibly emiting a debugging message).
    */
    void (*finalize)( struct whio_dev * dev );

    /**
       Returns 0 if the device is in a good state, else it returns
       an implementation-defined non-zero value. Implementations
       are encourage to use the symbolic values from whio_rc_t,
       but some implementations may be able to return exact
       error codes from the underlying device (e.g. a file handler
       could return a value from ferror() or an errno value related
       to it).
    */
    int (*error)( struct whio_dev * dev );

    /**
       Should clear any error state and return whio_rc.OK on success.
       If this function returns an error (an implementation-defined
       non-zero value), the error state should be assumed
       unrecoverable and the device should not be used.

       For compatibility with the standard FILE API (in particular,
       clearerr()), this routine should be used to clear any
       end-of-file flag (if necessary), though that is not all
       implementations have such a flag to clear.
    */
    int (*clear_error)( struct whio_dev * dev );

    /**
       Returns an implementation-defined non-zero value at eof or 0 at
       not-eof.
     */
    int (*eof)( struct whio_dev * dev );

    /**
       Returns the current position of the device, or
       whio_rc.SizeTError on error. An an example of an error, calling
       truncate() to truncate a device may leave its cursor out of
       bounds, which may be considered an error by the underlying
       implementation (though some allow arbitrary placement but may
       fail to be able to read/write from/to the given position).

       On a non-i/o error (e.g. !dev), whio_rc.SizeTError is returned.
    */
    whio_size_t (*tell)( struct whio_dev * dev );

    /**
       Sets the current position within the stream, following the same
       semantics as fseek() except that:

       - the return value is the new position.

       - If the seek would move the cursor out of bounds, it MAY be
       kept in bounds instead (i.e. at 0 or EOF). This is
       implementation-dependent (e.g. an in-memory back-end may not be
       able to grow). As a general rule, seek() is unbounded, and a
       validity check on the position is deferred until the next read
       or write.

       - Implementations MAY return whio_rc.SizeTError for the
       following cases:

       - !dev
       - whence==SEEK_SET and pos is negative.
       - whence is not one of SEEK_SET, SEEK_CUR, or SEEK_END.
       - seek fails in some other way.

       Alternately, implementations may rely on (largely undocumented)
       behaviour of the underlying implementation. e.g. fseek() does
       not document what happens on SEEK_SET with a negative value, but
       we can hope that it fails in that case.
    */
    whio_size_t (*seek)( struct whio_dev * dev, off_t pos, int whence );

    /**
       Flushes the underying stream (not all streams support/need
       this). On success it must return whio_rc.OK. On error it
       returns an implementation-defined non-zero value. Devices
       which do not use flushing should return 0.
    */
    int (*flush)( struct whio_dev * dev );

    /**
       Changes the size of the device to the new size. On success,
       whio_rc.OK is returned, otherwise an implementation-specified
       non-0 value is returned.

       Whether or not it is possible to increase the size of a device
       using this function is unfortunately implementation-specified.
       See the Linux man pages for ftruncate() for interesting
       details.

       The underlying cursor position must not be changed by calling
       this function.
    */
    int (*truncate)( struct whio_dev * dev, off_t size );

    /**
       ioctl() is a "back door" to give access to certain
       implementation-specific features. Devices need not support
       this, in which case they should return
       whio_rc.UnsupportedError. The second argument specifies the
       requested device-specific operation, which will normally be
       some common enum or macro value. The remaining arguments depend
       100% on the operation, and must be specified in the
       documentation for that operation.

       If the requested operation is supported then whio_rc.OK should
       be returned on success, else some error value OTHER than
       whio_rc.UnsupportedError should be returned. When passing on an
       underlying error code, there may be a code collision with
       whio_rc.UnsupportedError - that is unfortunate but unavoidable
       and we won't lose any sleep over it.

       To pass elipsis-style arguments, use whio_dev_ioctl() (and see
       that function's docs for why this one takes a va_list).
    */
    int (*ioctl)( struct whio_dev * dev, int operation, va_list args );
};
typedef struct whio_dev_api whio_dev_api;

/**
   Implementation details for whio_dev instances. Each instance may
   (and in practice always does does) store instance-specific data
   here. These data are for use only by the functions related to this
   implementation and must be cleaned up via dev->api->finalize(dev).
*/
struct whio_dev_impl
{
    /**
       For use only by concrete implementations, to store any data
       they need for the member function implementations. This data
       should be freed in the owning object's finalize() or close()
       routine.
    */
    void * data;

    /**
       A type identifier for use solely by whio_dev implementations,
       not client code. If the implementation uses this (it is an
       optional component), it must be set by the whio_dev
       initialization routine (typically a factory function).

       This mechanism works by assigning some arbitrary opaque value
       to all instances of a specific whio_dev implementation. The
       implementation funcs can then use that to ensure that they are
       passed the correct type. The typeID need not be public, but may
       be so. e.g. the author of the impl may provide a non-member
       whio_dev-related function which requires a specific type (or
       types), and in that case the types would possibly need to be
       known by the caller.
    */
    void const * typeID;
};
typedef struct whio_dev_impl whio_dev_impl;

/** @struct whio_dev_client

whio_dev_client is an abstraction for tying client-specific
data to a whio_dev object. The data is not used by the public
whio_dev API with one exception: when whio_dev_api::close()
is called, the implementation must clean up this data IFF
the dtor member is not 0. For example:

  @code
  if( my->client.dtor ) my->client.dtor( my->client.data );
  @endcode
   
*/
struct whio_dev_client
{
    /**
       Arbitrary data associated with the device.
    */
    void * data;
    /**
       If this function is not 0 then whio_dev implementations
       MUST call this function, passing the data member to it,
       in their cleanup routines. If it is 0 then they may
       ignore the data member.
    */
    void (*dtor)( void * );
};
typedef struct whio_dev_client whio_dev_client;

/**
   An empty whio_dev_client object for use in initialization
   of whio_dev_client objects.
*/
extern const whio_dev_client whio_dev_client_init;

/**
   Static initializer for whio_dev_client objects.
*/
#define whio_dev_client_init_m {0/*data*/,0/*dtor*/}

/**
   whio_dev is an interface for communicating with an underlying
   random access data store via. It is modelled after the
   POSIX-standard FILE API, and it "should" be able to act as a
   wrapper for any stream type for which we can implement the
   appropriate operations. The most significant limitation is that the
   underlying device type must support random read/write access (if it
   does not, not all operations of this type will be meaningful).

   There is no "default" whio_dev implementation - each underlying
   data store needs a different implementation. This type defines the
   conventions of the interface as specifically as possible, however.

   The member functions of this struct are abstracted into the
   whio_dev_api class (via the 'api' member of this class). The
   primary reason for this is because all instances of a certain class
   of whio_devices, in practice, share a single set of implementation
   functions. By referencing the functions this way we save
   (sizeof(whio_dev_api) - sizeof(whefs_api*)) bytes on each instance
   of whio_dev, and it its place we have a single shared (and static)
   instance of the implementation's API object.

   Thread safety: it is never legal to use any given whio_dev instance
   from more than one thread at a single time, and doing so will
   almost certainly corrupt the internal state of the stream. e.g. its
   read/write position may be moved by another thread, causing a read
   or write to go somewhere other than desired. It is in theory not
   problematic for multiple threads to share one whio_dev instance as
   long as access to the device is strictly serialized via a
   marshaller.
*/
typedef struct whio_dev
{
    /**
       Holds all "member functions" of this interface.  It is never
       legal for api to be NULL, and if a device with a NULL api
       member is used with the whio API then a segfault will certainly
       quickly result.
    */
    whio_dev_api const * api;

    /**
       Holds instance-specific, implementation-dependent
       information. Not for use by client code. The
       implementation-specific close() method should free up this
       memory.
    */
    whio_dev_impl impl;

    /**
       This data is for sole use by whio_dev clients, with one
       important exception: see the docs for whio_dev_client for
       details.
    */
    whio_dev_client client;
} whio_dev;


/**
   Works like malloc(), but beware...

   Creates an empty, non-functional whio_dev object and returns it.
   The object must be populated by the caller and must eventually
   be destroyed by calling whio_dev_free() AFTER the private parts
   of the object are cleaned up (see whio_dev_free() for details).

   Clients of whio_dev objects SHOULD NOT use this routine - it is
   intended for use by whio_dev factory/initialization routines for
   reasons explained below.

   This function is not required to be thread safe nor reentrant, and
   a given implementation may use static data if needed. If it
   dynamically allocates memory for its internal use then it "should"
   clean up that memory via an atexit() handler or custom cleanup
   mechanism beyond the scope of this interface.

   The default implementation simply returns the result of calling
   malloc(sizeof(whio_dev)).

   A side effect of the undefined allocation rules is that devices
   returned from this function may not be valid if used after
   main() exits (e.g. via an atexit() handler) because the underlying
   allocation mechanism might have been cleaned up already.

   Why this function exists:

   As part of a related project i am looking to avoid using malloc()
   when necessary. For that project i would like to provide my own
   allocation pool for whio_dev objects (which are allocated
   relatively often in that framework). So, to that end, the i/o
   device implementations which come with whio use this routine to
   allocate their objects.

   @see whio_dev_free()
*/
whio_dev * whio_dev_alloc();

/**
   BIG FAT HAIRY WARNING:

   - This function must ONLY be passed objects allocated via
   whio_dev_alloc().

   - This function DOES NOT call dev->api->finalize(dev) or
   dev->api->close(dev). Instead, this function is intended to be
   called from one of those routines after the device's private data
   has been cleaned up.

   This function effectively passes the given object to free(), but
   the implementation is free to use a different alloc/dealloc
   method. In any case, clients should treat dev as invalid after this
   call.

   @see whio_dev_alloc()
*/
void whio_dev_free( whio_dev * dev );

/**
   This function is a workaround for the whio_dev::ioctl() interface.

   The public ioctl() function on Linux looks like:

   @code
   int ioctl( int descriptor, int operation, ... );
   @endcode

   But if we give whio_dev::ioctl() that interface, as opposed to
   passing a va_list instead of elipsis, it becomes problematic to
   pass on those arguments to the underlying ioctl() function (or do
   equivalent operations). So we provide the elipsis form here and the
   va_list form in the whio_dev::ioctl() interface.
*/
int whio_dev_ioctl( whio_dev * dev, int operation, ... );

/**
   Returns the size of the given device, or whio_rc.SizeTError if
   !dev or if the device returns that code to signify that it is not
   seekable. The size is calculated by seek()ing to the
   end of the device and using that offset. Thus the device must of
   course support seeking. The original position before the function
   returons.
*/
whio_size_t whio_dev_size( whio_dev * dev );

/**
   Equivalent to dev->api->set( dev, 0, SEEK_SET ), but returns whio_rc.OK
   on success.
*/
int whio_dev_rewind( whio_dev * dev );

/**
   Equivalent to dev->api->write( dev, data, n ).
*/
whio_size_t whio_dev_write( whio_dev * dev, void const * data, whio_size_t n );

/**
   Positions dev to pos and then writes n bytes from data to it. May return
   either whio_rc.SizeTError (if the seek fails) or the result of calling
   dev->api->write( dev, data, n );
*/
whio_size_t whio_dev_writeat( whio_dev * dev, whio_size_t pos, void const * data, whio_size_t n );

/**
   Copies all of src, from the beginning to EOF, to dest, starting
   at dest's current position. Returns whio_rc.OK on success.
*/
int whio_dev_copy( whio_dev * src, whio_dev * dest );

/**
   This enum defines some on-disk sizes for the utility routines
   which encode/decode data to/from whio_dev objects.
*/
enum whio_dev_encoded_sizes {

/** @var whio_dev_sizeof_uint64

   whio_dev_sizeof_uint64 is the length (in bytes) of an encoded uint64 value.
   It is 9: 1 tag byte + 8 data bytes.

   @see whio_dev_uint64_decode()
   @see whio_dev_uint64_encode()
*/
whio_dev_sizeof_uint64 = 9,
/** @var whio_dev_sizeof_uint32

   whio_dev_sizeof_uint32 is the length (in bytes) of an encoded uint32 value.
   It is 5: 1 tag byte + 4 data bytes.

   @see whio_dev_uint32_decode()
   @see whio_dev_uint32_encode()
*/
whio_dev_sizeof_uint32 = 5,

/** @var whio_dev_sizeof_uint16

   whio_dev_sizeof_uint16 is the length (in bytes) of an encoded uint16 value.
   It is 3: 1 tag byte + 2 data bytes.

   @see whio_dev_uint16_decode()
   @see whio_dev_uint16_encode()
*/
whio_dev_sizeof_uint16 = 3,

/** @var whio_dev_sizeof_cstring

   whio_dev_size_cstring is the encoded length of a C-style string,
   NOT INCLUDING the actual string bytes. i.e. this is the header
   size.

   @see whio_dev_cstring_decode()
   @see whio_dev_cstring_encode()
*/
whio_dev_sizeof_cstring = whio_dev_sizeof_uint32 + 1
};

/**
   Encodes a 32-bit integer value into 5 bytes - a leading tag/check
   byte, then the 4 bytes of the number, in big-endian format. Returns
   the number of bytes written, which will be equal to
   whio_dev_sizeof_uint32 on success.

   @see whio_dev_uint32_decode()
*/
size_t whio_dev_uint32_encode( whio_dev * dev, uint32_t i );

/**
   The converse of whio_dev_uint32_encode(), this tries to read an encoded
   32-bit value from the current position of dev. On success it returns
   whio_rc.OK and sets target to that value. On error it returns some
   other value from whio_rc and does not modify tgt.

   Error values include:

   - whio_rc.ArgError = !dev or !tgt

   - whio_rc.IOError = an error while reading the value (couldn't read enough bytes)

   - whio_rc.ConsistencyError = the bytes at the current location were not encoded
   with whio_dev_uint32_encode().

   @see whio_dev_uint32_encode()

*/
int whio_dev_uint32_decode( whio_dev * dev, uint32_t * tgt );

/**
   Similar to whio_dev_uint32_encode(), with the same conventions, but
   works on 16-bit numbers. Returns the number of bytes written, which
   will be equal to whio_dev_sizeof_uint16 on success.

   @see whio_dev_uint16_decode()
*/
size_t whio_dev_uint16_encode( whio_dev * dev, uint16_t i );

/**
   Similar to whio_dev_uint32_decode(), with the same conventions and
   error codes, but works on 16-bit numbers.  On success it returns
   whio_rc.OK and sets target to that value. On error it returns some
   other value from whio_rc and does not modify tgt.

   @see whio_dev_uint16_encode()
*/

int whio_dev_uint16_decode( whio_dev * dev, uint16_t * tgt );


/**
   The 64-bit variant of whio_dev_uint32_encode(). Follows the same
   conventions as that function but handles a uint64_t value instead
   of uint32_t.

   @see whio_dev_uint16_encode()
   @see whio_dev_uint32_encode()
   @see whio_dev_uint64_decode()
*/
size_t whio_dev_uint64_encode( whio_dev * fs, uint64_t i );

/**
   The 64-bit variant of whio_dev_uint32_decode(). Follows the same
   conventions as that function but handles a uint64_t value instead
   of uint32_t.

   @see whio_dev_uint16_decode()
   @see whio_dev_uint32_decode()
   @see whio_dev_uint64_encode()
*/
int whio_dev_uint64_decode( whio_dev * dev, uint64_t * tgt );

/**
   Uses whio_dev_uint32_encode() to write n elements from the given
   array to dev.  Returns whio_rc.OK on success. Returns the number of
   items written.

   @see whio_dev_uint32_encode()
*/
size_t whio_dev_uint32_array_encode( whio_dev * dev, size_t n, uint32_t const * list );

/**
   Reads n consecutive numbers from dev, populating list (which must
   point to at least n uint32_t objects) with the results. Returns the
   number of items read, which will be less than n on error.

   @see whio_dev_uint32_decode()
*/
size_t whio_dev_uint32_array_decode( whio_dev * dev, size_t n, uint32_t * list );

/**
   Encodes a C string into the device by writing a tag byte, the length of
   the string, and then the string bytes. If n is 0 then n is equivalent to
   strlen(s). Zero is also legal string length.

   Returns the number of bytes written, which will be (n +
   whio_dev_size_cstring) on success, 0 if !dev or !s.

   @see whio_dev_cstring_decode()
*/
size_t whio_dev_cstring_encode( whio_dev * dev, char const * s, uint32_t n );

/**
   The converse of whio_dev_cstring_encode(), this routine tries to
   decode a string from the current location in the device.

   On success, tgt is assigned to the new (null-terminated) string
   (allocated via calloc()) and length (if it is not null) is set to
   the length of the string (not counting the terminating null). The
   caller must free the string using free(). If the string has a
   length of 0 then tgt is set to 0, not "", and no memory is
   allocated.

   Neither dev nor tgt may be 0, but length may be 0.

   Returns whio_rc.OK on success.

   On error, neither tgt nor length are modified and some non-OK value
   is returned:

   - whio_rc.ArgError = dev or tgt are 0.

   - whio_rc.ConsistencyError = current position of the device does not
   appear to be an encoded string written by whio_dev_cstring_encode().

   - whio_rc.IOError = some form of IO error.


   Example:

@code
char * str = 0;
size_t len = 0;
int rc = whio_dev_cstring_decode( myDevice, &str, &len );
if( whio_rc.OK != rc ) ... error ...
... use str ...
free(str);
@endcode


   @see whio_dev_cstring_encode()
*/
int whio_dev_cstring_decode( whio_dev * dev, char ** tgt, size_t * length );

/**
   Similar to printf() and friends, this writes a formatted string
   to the given output device. Returns the number of bytes written.
   In the general case it is not possible to know if a given conversion
   fails, so it is not possible to say how many bytes *should* have
   been written.

   The formatted data is not copied, but is instead sent (via proxy
   functions) to the destination device directly, so it may be
   arbitrarily large without any copy penalty. Nonetheless, as with
   most formatted writing, this routine has an extremely high overhead
   compared to unformatted writes.
*/
size_t whio_dev_writefv( whio_dev * dev, const char *fmt, va_list ap );

/**
   Equivalent to whio_dev_writefv() except that it takes an elipsis list
   instead of a va_list.
*/
size_t whio_dev_writef( whio_dev * dev, const char *fmt, ... );

/**
   This enum holds masks for general categories of whio_dev ioctl
   operations.  By convention, the top byte (of 4 bytes) of ioctl
   operation values is reserved for these masks. ioctl values should
   not use the type byte and should instead mask their value against
   one these (this assists in documentation, by making it clear which
   category an ioctl command belongs to).
*/
enum whio_dev_ioctl_categories {

/**
   General-purpose ioctls which may be supported by arbitrary
   devices.
*/
whio_dev_ioctl_mask_GENERAL = 0x01000000,

/**
   ioctl's for FILE devices should have this mask.
*/
whio_dev_ioctl_mask_FILE =   0x02000000,

/**
   ioctl's for in-memory device buffers should have this mask.
*/
whio_dev_ioctl_mask_BUFFER = 0x04000000,

/**
   ioctl's for sub-device operations should have this mask.
*/
whio_dev_ioctl_mask_SUBDEV = 0x10000000,

/**
   ioctl's for locking-related operations should have this mask.
*/
whio_dev_ioctl_mask_LOCKING = 0x20000000,

/**
   ioctl's compatile with fcntl() should have this mask.
*/
whio_dev_ioctl_mask_FCNTL = 0x40000000

};


/**
   Equivalent to dev->api->read(...). If !dev, 0 is returned.
 */
whio_size_t whio_dev_read( whio_dev * dev, void * dest, whio_size_t n );

/**
   Equivalent to dev->api->write(...). If !dev, 0 is returned.
 */
whio_size_t whio_dev_write( whio_dev * dev, void const * src, whio_size_t n );

/**
   Equivalent to dev->api->eof(...). If !dev, whio_rc.ArgError  is returned.
 */
int whio_dev_eof( whio_dev * dev );

/**
   Equivalent to dev->api->tell(...). If !dev, whio_rc.SizeTError is returned.
 */
whio_size_t whio_dev_tell( whio_dev * dev );

/**
   Equivalent to dev->api->seek(...). If !dev, whio_rc.SizeTError is returned.
 */
whio_size_t whio_dev_seek( whio_dev * dev, off_t pos, int whence );

/**
   Equivalent to dev->api->flush(...). If !dev, whio_rc.ArgError  is returned.
 */
int whio_dev_flush( whio_dev * dev );

/**
   Equivalent to dev->api->truncate(...). If !dev, whio_rc.ArgError is returned.
 */
int whio_dev_truncate( whio_dev * dev, off_t size );

/**
   Equivalent to dev->api->finalize(...). If !dev, this function does nothing.
 */
void whio_dev_finalize( whio_dev * dev );

/**
   Equivalent to dev->api->close(...). If !dev, this function returns false.
 */
bool whio_dev_close( whio_dev * dev );


/**
   Result type for whio_dev_fetch(). It is a workaround to enable some
   auto-generated script bindings to use the read() API.  See
   whio_dev_fetch() for details.
*/
typedef struct whio_fetch_result
{
    /**
       malloc()'d memory will be placed here by whio_dev_fetch() and
       whio_dev_fetch_r().  It must be freed by calling
       whio_dev_fetch_free() (if this object is heap-allocated), or by
       passing the data member to free() (if this object is
       stack-allocated).

       It is (char *), as opposed to (void*) or (unsigned char *)
       because SWIG likes it that way.
    */
    char * data;
    /**
       Number of bytes requested in the corresponding
       whio_dev_fetch().
    */
    whio_size_t requested;
    /**
       Number of bytes actually read in the corresponding
       whio_dev_fetch().
    */
    whio_size_t read;
    /**
       Number of bytes allocated to the data member.
     */
    whio_size_t alloced;
} whio_fetch_result;

/**
   This function is a workaround to allow us to use SWIG to
   generate scriptable functions for the whio_dev::read()
   interface. It works like so:

   - it allocates a new whio_fetch_result object.
   - it allocates n bytes of memory.
   - it calls dev->api->read() to fill the memory.

   The returned object contains referneces to the allocated memory,
   the size of the request (the n parameter), the number of bytes
   actually read, and the amount of memory allocated in the obj->data
   member.

   The returned object must be passed to whio_dev_fetch_free(), or
   free(obj->data) and free(obj) must be called (in that order!).

   @see whio_dev_fetch_r()
   @see whio_dev_fetch_free()
   @see whio_dev_fetch_free_data()
*/
whio_fetch_result * whio_dev_fetch( whio_dev * dev, whio_size_t n );

/**
   Similar to whio_dev_fetch(), but the results are written to the tgt
   object (which may not be 0) and the memory buffer of that object is
   re-used if possible (or reallocated, if n is bigger than
   tgt->alloced). On success, whio_rc.OK is returned and tgt is updated.
   On error, tgt may still be updated, and still needs to be freed
   as explained in whio_dev_fetch(), and non-whio_rc.OK is returned:

   - whio_rc.ArgError = !dev or !tgt

   - whio_rc.AllocError = (re)allocation of the memory() buffer
   failed.  tgt->data might still be valid (and need to be freed), but
   could not be expanded to n bytes.

   If tgt comes from C code (as opposed to SWIG-generated script
   bindings) and is a stack-allocated object (not via malloc()), then
   the caller MUST NOT call whio_dev_fetch_free() and must instead
   call free(tgt->data) to free any allocated memory.

   PS: the '_r' suffix on this function name is for "re-use".

   @see whio_dev_fetch()
   @see whio_dev_fetch_free()
   @see whio_dev_fetch_free_data()
*/
int whio_dev_fetch_r( whio_dev * dev, whio_size_t n, whio_fetch_result * tgt );

/**
   Calls free(r->data) then free(r). DO NOT pass this a stack-allocated
   object. To clean up such an object, simply call free(obj->data)
   or call whio_dev_fetch_free_data().

   @see whio_dev_fetch()
   @see whio_dev_fetch_r()
   @see whio_dev_fetch_free_data()
*/
int whio_dev_fetch_free( whio_fetch_result * r );

/**
   Similar to whio_dev_fetch_free(), except that r->data i freed and
   r->alloced set to 0, but r is not deleted. Thus this is legal
   for stack-allocated objects, whereas whio_dev_fetch_free() is not.

   When using the fetch API from scripted code, one needs to be
   careful how the whio_fetch_results are cleaned up. Here's an
   example (in Python) which demonstrates the two ways to handle it:

   @code
import whio
fname = 'bogo.out';
f = whio.whio_dev_for_filename( fname, "w" )
sz = whio.whio_dev_write( f, "Hi, world!", 10 )
whio.whio_dev_finalize(f)
f = whio.whio_dev_for_filename( fname, "r" )
if f is None:
    raise Exception("Could not open "+fname+" for reading!")
# Approach #1: use whio_dev_fetch() to allocate an object:
frc = whio.whio_dev_fetch(f, 5);
if frc is None:
    raise Exception("Fetch failed!")
print('sizes:', frc.requested, frc.read, frc.alloced);
print('data:',frc.data);
# Now clean up the object:
whio.whio_dev_fetch_free(frc);
# Or there's a second approach:
frc = whio.whio_fetch_result(); # allocated new object
rc = whio.whio_dev_fetch_r( f, 30, frc );
rc = whio.whio_dev_fetch_free_data(frc); # note the different cleanup function!
print('fetch_free rc=',rc);
whio.whio_dev_finalize(f)
   @endcode

*/
int whio_dev_fetch_free_data( whio_fetch_result * r );


/**
   This routine only works on devices created with
   whio_dev_subdev_create() (or equivalent). It re-sets the lower and
   upper bounds of the subdevice (as described in
   whio_dev_subdev_create()) and re-sets the cursor to the new lower bound.

   This routine does no bounds checking on the parent device.

   On success, whio_rc.OK is returned, otherwise:

   - whio_rc.ArgError = !dev or upperBound is not 0 but is less than lowerBound.

   - whio_rc.TypeError = dev's type-id does not match (it is not a subdev device).

   - whio_rc.InternalError = dev is not mapped to a parent device (this theoretically
   cannot happen unless client code muddles with dev->impl.data).

   @see whio_dev_subdev_create()
*/
int whio_dev_subdev_rebound( whio_dev * dev, whio_size_t lowerBound, whio_size_t upperBound );

/** @enum whio_dev_ioctls
  This is the collection of "known" ioctl values for use as the second
  argument to whio_dev::ioctl(). The documentation for each entry
  explains what the third and subsuquent arguments to ioctl() must be.
  Type mismatches will lead to undefined results, very possibly
  crashes or memory corruption.

  This enum is updated as new whio_dev implementations are created or
  new ioctl commands are added.

  Here's an example which works with whio_dev objects created
  via whio_dev_for_memmap_XXX() and whio_dev_membuf(), to reveal
  the size of their associated memory buffer:

  @code
  whio_size_t sz = 0;
  int rc = whio_dev_ioctl(dev, whio_dev_ioctl_BUFFER_size, &sz );
  if( whio_rc.OK == rc ) {
    printf("Buffer size = %u\n", sz );
  }
  @endcode
*/
enum whio_dev_ioctls {

/** @var whio_dev_ioctl_GENERAL_size
   whio_dev_file() finds a device's size by calling seek()
   to get to the EOF. While this is generic, it potentially
   requires i/o. Some devices must record their length and
   therefor have constant-time access to it. Such devices should
   support this ioctl.

   The third argument to whio_dev::ioctl() MUST be a pointer
   to a whio_size_t.
*/
whio_dev_ioctl_GENERAL_size = whio_dev_ioctl_mask_GENERAL | 0x01,

/** @var whio_dev_ioctl_GENERAL_name
   Some devices may be able to report a name associated with the
   device. The second parameter must be a (char const **). The pointer
   will be assigned to the name of the device. The string's ownership
   is not generically defined, but will typically be owned by the
   one who opened the device or by the device itself (if it copies the
   name it was opened with). In any case, if the caller wants to keep
   the name, he should copy it immediately after fetching it.
*/
whio_dev_ioctl_GENERAL_name = whio_dev_ioctl_mask_GENERAL | 0x02,

/** @var whio_dev_ioctl_BUFFER_size

   Some whio_dev implementations use a buffer. Some of those can also
   report the buffer size using this ioctl.

   The 3rd parameter to the ioctl call MUST be a pointer to a whio_size_t
   object, in which the size of the buffer is stored.

   The exact meaning of the buffer is implementation dependent. e.g
   the FILE wrapper uses no buffer (though the underlying
   implementation probably does), but the membuf and memmap
   implementations do (but use them in slightly different ways).

   A growable membuf implementation returns the current allocated size
   of the buffer (which may be larger than its reported device size
   and may be changed by write or truncate operations). A non-growable
   membuf will return the fixed allocated size, which does not change
   during the life of the device.

   The memmap device wrapper will return the size of the memory range
   pointed to by the device. This number does not change during the
   life of the device.
*/
whio_dev_ioctl_BUFFER_size = whio_dev_ioctl_mask_BUFFER | 0x01,

/** @var whio_dev_ioctl_FILE_fd

   A FILE-based whio_dev can use this ioctl to return the underlying
   file descriptor number. The third argument to ioctl() MUST be a
   pointer to a signed integer, in which the file descriptor is
   written.
*/
whio_dev_ioctl_FILE_fd = whio_dev_ioctl_mask_FILE | 0x01,

/** @var whio_dev_ioctl_SUBDEV_parent_dev

   Sub-device whio_dev devices interpret this as "return the parent device
   pointer through the third argument", where the third argument must
   be a (whio_dev**).
*/
whio_dev_ioctl_SUBDEV_parent_dev = whio_dev_ioctl_mask_SUBDEV | 0x01,

/** @var whio_dev_ioctl_SUBDEV_bounds_get

   Sub-device whio_dev devices interpret this as "return the
   lower/upper bounds range, relative to the parent device, through
   the third and fourth arguments, respectively", where the third and
   fourth arguments must be a (whio_size_t*). Either may be NULL, in which
   case that argument is ignored.
*/
whio_dev_ioctl_SUBDEV_bounds_get = whio_dev_ioctl_mask_SUBDEV | 0x02,

/** @var whio_dev_ioctl_FCNTL_lock_nowait

   Devices which support locking via fcntl() (or semantically
   compatible) may support this ioctl (and the related ones)
   
   The third argument to the ioctl() call MUST be a non-const (struct
   flock *) which describes the lock/unlock operation.

   whio_dev_ioctl_FCNTL_lock requests a fcntl() non-blocking lock
   (F_SETLK).
*/
whio_dev_ioctl_FCNTL_lock_nowait = whio_dev_ioctl_mask_FCNTL | 0x01,

/** @var whio_dev_ioctl_FCNTL_lock_wait

   whio_dev_ioctl_FCNTL_lock_wait requests a fcntl() blocking lock
   (F_SETLKW).

   See whio_dev_ioctl_FCNTL_lock for more details.
*/
whio_dev_ioctl_FCNTL_lock_wait = whio_dev_ioctl_mask_FCNTL | 0x02,

/** @var whio_dev_ioctl_FCNTL_lock_get

   whio_dev_ioctl_FCNTL_lock_get requests an fcntl() F_GETLK
   operation.

   See whio_dev_ioctl_FCNTL_lock for more details.
*/
whio_dev_ioctl_FCNTL_lock_get = whio_dev_ioctl_mask_FCNTL | 0x04

};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WANDERINGHORSE_NET_WHIO_DEV_H_INCLUDED */
/* begin file whio_devs.h */
#ifndef WANDERINGHORSE_NET_WHIO_DEVS_H_INCLUDED
#define WANDERINGHORSE_NET_WHIO_DEVS_H_INCLUDED

/*
   This file contains declarations and documentation for the concrete
   whio_dev implementations. The generic iodevice API is declared in
   whio_dev.h.
*/
#include <stdio.h> /* FILE */
#include <stdint.h> /* uint32_t */
#include <stdarg.h> /* va_list */

#ifdef __cplusplus
extern "C" {
#endif


/**
   Creates a whio_dev object which will use the underlying FILE
   handle. On success, ownership of f is passed to the returned
   object and the returned object must eventually be finalized with
   dev->api->finalize(dev). Doing so will close f and free the returned
   object.

   For purposes of the whio_dev interface, any parts which have
   implementation-specified behaviour will behave as they do
   for the local FILE-related API. e.g. ftruncate() may or may
   not be able to extend a file, depending on the platform
   and even the underlying filesystem type.

   The takeOwnership argument determines the ownerhsip of f: if the
   function succeeds and takeOwnership is true then f's ownership is
   transfered to the returned object. In all other cases ownership is
   not changed.

   Peculiarities of this whio_dev implementation:

   - ioctl() is very limited. The definition of the pseudo-standard
   ioctl() does not allow us to blindly pass on elipse arguments to
   it, so we cannot simply proxy the calls.

   - See the docs for whio_dev_ioctl_FILE_fd for how to fetch the
   underlying file descriptor.

   @see whio_dev_for_filename()
*/
whio_dev * whio_dev_for_FILE( FILE * f, bool takeOwnership );

/**
   Similar to whio_dev_for_FILE(), but takes a filename and an
   access mode specifier in the same format expected by fopen().
   In addition, the returned object internally uses the lower-level
   read(), write(), lseek(), etc. API instead of fread(), fwrite(),
   fseek(), etc. This is, for some use cases, more performant, and is
   compatible with fcntl()-style locking.

   It is ill advised to mix use of the POSIX file API and the
   lower-level APIs. The returned object uses the lower-level APIs for
   all i/o operations, using the fopen() and fclose() API only for
   opening and closing the file (because fopen()'s mode arguments are
   easier to manage).


   Peculiarities of this whio_dev implementation:

   - Because the lower-level I/O API doesn't have direct equivalents
   of feof(), ferror(), and clearerr(), devices created this way may
   behave slightly differently in some cases (involving error
   reporting) than devices created using whio_dev_for_FILE() (but
   should nonetheless behave as expected).

   - The whio_dev_ioctl_FILE_fd ioctl is supported to fetch the
   underlying file descriptor.

   - It also supports the whio_dev_ioctl_GENERAL_name ioctl() to
   report the name passed to this function. That will only work
   if the name passed to this function is static or otherwise
   stays in scope (and doesn't change addresses) for the life
   of this object.

   - It also supports the whio_dev_ioctl_FCNTL_lock_wait,
   whio_dev_ioctl_FCNTL_lock_nowait, and whio_dev_ioctl_FCNTL_lock_get
   ioctls (from the whio_dev_ioctls enum).

   @see whio_dev_for_FILE()
*/
whio_dev * whio_dev_for_filename( char const * fname, char const * mode );


/**
   Creates a new whio_dev object which wraps an in-memory buffer. The
   initial memory allocated by the buffer is allocated by this call.
   Whether or not the buffer is allowed to be expanded by write() or
   seek() operations is defined by the remaining parameters.

   The expFactor specifies a growth expansion value, as follows. If
   expFactor is 1.0 or less then the buffer will never be allowed to
   expand more than the original size. If it is greater than 1.0, then
   it will be made expandable (that is, write() may cause it to
   grow). Every time its size grows, it will grow by a factor of
   expFactor. e.g. 1.5 means grow by 1.5 times (a common growth factor
   for dynamic memory allocation). Likewise, when a buffer shrinks
   (via truncate()), it will be reallocated if (currentSize/expFactor)
   is greater than the number of bytes being used. For example, if a
   buffer of 1024 bytes with an expFactor of 1.5 is being shrunk, it
   will not release the allocated memory unless doing so will drop it
   below ((1024/1.5)=682) bytes. A very large expFactor (more than
   2.0) is not disallowed, but may not be good for your sanity.

   For purposes of the following text, a membuf device with an
   expFactor of greater than 1.0 is said to be "growable".

   If the buffer is growable then calling write() when we are at (or
   past) EOF will cause the buffer to try to expand to accommodate.
   If it cannot, or if the buffer is not growable, the write operation
   will write as much as it can fit in existing memory, then return a
   short write result.

   It not enough memory can be allocated then 0 is returned. The
   caller owns the returned object and must eventually destroy it by
   calling dev->api->finalize(dev).

   The returned object supports all of the whio_dev interface, with
   the caveat that write() calls will not be allowed to expand out of
   bounds if the device is not growable.

   Regardless of the expansion policies, the truncate() member of the
   returned object can be used to expand the buffer. See below for
   more details.

   Peculiarities of this whio_dev implementation:

   - Whether or not the device is growable (as explained above), seeks past
   EOF are always allowed as long as the range is positive. Non-growable
   buffers will not write there, but growable ones will try to expand
   at the next write. Non-growable buffers *can* be expanded by manually
   calling truncate() on them.

   - truncate() ignores the growth policy! That is by design, to allow
   us to (optionally) manually control the growth without allowing
   rogue seek/write combinations to take up all our memory.

   - When truncate() shrinks a growable buffer: memory may or may not
   be immediately released, depending on the magnitude of the
   change. A truncate() to a size of 0 will always release the memory
   immediately.

   - When truncate() shrinks a non-growable buffer: the memory is not
   released at all because the buffer could then not be expanded
   later. When truncating a non-expanding buffer to a smaller size,
   writes() made past new EOF will cause it to expand, but only up to
   the original size given to this function. e.g. if we have a non-growable
   buffer with 1024 bytes, we can truncate() it to 10 bytes, then write
   (expanding the size) up until we reach 1024 bytes, at which point
   we cannot write any more.

   - seek() will only fail if the offset would cause the position
   counter to overflow its numeric range or would set it before the
   start of the buffer. seek() does not change the object's size, and
   writing after an out of bounds seek will cause the object to grow
   starting at that new position (if it is growable) or it will fail
   (if the buffer is not growable or cannot grow).

   - flush() is a harmless no-op.

   - ioctl(): all of the ictl operations listed in the whio_dev_ioctls
   enum and marked with whio_dev_ioctl_mask_BUFFER are supported, as
   documented in that enum. The whio_dev_ioctl_GENERAL_size ioctl
   is also supported (from the whio_dev_ioctls enum).
*/
whio_dev * whio_dev_for_membuf( whio_size_t size, float expFactor );

/**
   Creates a new read/write whio_dev wrapper for an existing memory
   range.  For read-only access, use whio_dev_for_memmap_ro() instead.

   The arguments:

   - mem = the read/write memory the device will wrap. Ownership is
   not changed.  May not be 0. If mem's address changes during the
   lifetime of the returned object (e.g. via a realloc), results are
   undefined an almost certainly ruinous.

   - size = the size of the mem buffer. It may not be 0. It is the
   caller's responsibility to ensure that the buffer is at least that
   long. This object will not allow i/o operations outside of that
   bound. It is good practice to ensure that the memory is zeroed out
   before passing it here, to avoid unpredictable artifacts.

   On success a new whio_dev is returned. On error (invalid
   arguments or allocation error), 0 is returned.

   Peculiarities of the memmap whio_dev wrapper:

   - trunc() operations will work, but only up to the size passed to
   this function.  That is, one can "shrink" the device by truncating
   it, then grow it back, but never larger than the original size.

   - seek() accepts past-EOF ranges, but will return
   whio_rc.SizeTError on a numeric over/underflow. Writing past EOF
   range will of course not be allowed.

   - write() on a read-only device returns 0, as opposed to whio_rc.SizeTError.

   - Supports the same ioctl() arguments as described for
   whio_dev_for_membuf().

   @see whio_dev_for_memmap_ro()
   @see whio_dev_for_membuf()
*/
whio_dev * whio_dev_for_memmap_rw( void * mem, whio_size_t size );

/**
   This is equivalent to whio_dev_for_memmap_rw() except that it
   creates a read-only device and ownership of mem is not changed by
   calling this function.

   @see whio_dev_for_memmap_rw()
   @see whio_dev_for_membuf()
*/
whio_dev * whio_dev_for_memmap_ro( const void * mem, whio_size_t size );

/**
   Creates a new whio_dev object which acts as a proxy for a specified
   range of another device (that is, the object acts as a
   "subdevice"). All read/write/seek/tell operations on the returned
   object act on a range which is relative to the parent object. The
   object will not allow read/write operations outside of the given
   range.

   The arguments are:

   - parent = the parent i/o device. It must outlive the returned
   object, and ownership of parent is not changed by calling this
   function.

   - lowerBound is the logical begining-of-file for the returned
   device, relative to the coordinates of the parent.

   - upperBound is the logical "hard" EOF for the returned device -
   writes will not be allowed past this point. If 0 is passed it is
   treated as "unlimited". upperBound must be 0 or greater than
   lowerBound.

   The bounds are not checked when the subdevice is created, as some
   devices will allow one to write past EOF to extend it. If the
   bounds are not legal at read/write time then the error will be triggered
   there.  If parent is ever sized such that the given bounds are
   not longer legal, read/write errors may occur.

   Subdevices can be used to partition a larger underlying device into
   smaller pieces. By writing to/reading from the subdevices, one can
   assure that each logical block remains separate and that any
   operation in one block will not affect any other subdevices which
   have their own blocks (barring any bugs in this code). Results are
   of course unpredictable if multiple devices map overlapping ranges
   in the parent device (then again, maybe that's what you want, e.g. as
   a communication channel).

   Most operations are proxied directly through the parent device,
   with some offsets added to accomodate for the bounds defined, so
   the returned object will inherit any peculiarities of the parent
   implementation (e.g. flush() requirements).

   On success a new whio_dev object is returned, which must eventually be
   destroyed by calling dev->api->finalize(dev).

   On error 0 is returned.

   Peculiarities of this whio_dev implementation:

   - read() and write() will reposition the parent cursor to the
   subdevice's position before reading/writing and re-set it when
   complete. Thus other code using the parent device need not worry
   about the subdevice hosing the cursor position. reads/writes do of
   course track the internal cursor used by the subdevice, so (for
   example) one need not manually reposition the cursor when doing
   sequential reads or writes. Likewise, seek() and tell() work independently
   of the parent device (with one exception, described below).

   - The behaviour of seek(dev,n,SEEK_END) depends partially on
   upperBound. If upperBound==0 (i.e. "no bounds") then SEEK_END will
   use the EOF of the parent device, not the subdevice. In any case,
   the value returned from tell() or seek() will be relative to the
   subdevice.

   - truncate() does not function; it simply returns
   whio_rc.UnsupportedError.  It would be feasible to handle
   truncating like the memmap whio_dev does - allow shrinking and
   growing back up to the original maximum size, but no further than
   that. This might be added at some point, but it doesn't currently
   seem to be useful enough to warrant the effort.

   - Some calls (e.g. flush(), clear_error()) are simply passed on to the
   parent device. Though it is not technically/pedantically correct to
   do so in all cases.

   - ioctl(): this type accepts ioctls described in the
   whio_dev_ioctls enum having the name prefix
   whio_dev_ioctl_SUBDEV_. See those docs for details.

   - It is theoretically possible to nest subdevices, and it may be
   even useful in some cases, but i haven't tried it.


   Example:


   @code
    char const * fname = "subdev.iodev";
    whio_dev * parent = whio_dev_for_filename( fname, "w+" );
    assert(parent);
    parent->api->write( parent, "!", 1 );
    parent->api->seek( parent, 99, SEEK_SET );
    parent->api->write( parent, "!", 1 );
    // parent is now 100 bytes long.
    whio_dev * sub = whio_dev_subdev_create( parent, 10, 43 );
    assert(sub);
    MARKER("Subdevice in place for file [%s]...\n", fname );

    size_t i = 0;
    whio_size_t szrc = 0;
    for( ; i < 5; ++i )
    {
        szrc = sub->api->write( sub, "0123456789", 10 );
        MARKER("Write length = %u\n", szrc );
        if( szrc != 10 ) break;
    }
    sub->api->finalize(sub);
    parent->api->finalize(parent);
    // Now bytes in the inclusive range 10..42 (not 43 - that's sub's
    // hard EOF) will be filled with whatever was written to sub.
   @endcode
*/
whio_dev * whio_dev_subdev_create( whio_dev * parent, whio_size_t lowerBound, whio_size_t upperBound );


/**
   A type used to represent sequential block IDs for the whio_blockdev API.
   It is not hard-coded because for my intended uses it will changed to
   other types depending on compilation options.

   FIXME: make this type macro-configurable, so it can play nice with
   whefs's configurable sizes. It'll work as-is for <=32 bit whefs.
*/
typedef uint32_t whio_blockdev_id;

/** @struct whio_blockdev

   whio_blockdev is a helper intended to assist in
   partitioning of a larger i/o device. It is intended to
   be used in conjunction with a "master" i/o device
   which has logical partitions made up of fixed-sized
   records. Instead of providing a low-level i/o API, its
   API works at the "record" level, where each record
   is a block of a size specified when the object
   is initialized.

   whio_blockdev objects are initialized via whio_blockdev_setup()
   and clean up (depending on how they were created) using
   whio_blockdev_clean() or whio_blockdev_finalize().

   Though this type's internals are publically viewable,
   they should not be used by client code. Use the
   whio_blockdev family of functions instead.

   @see whio_blockdev_alloc()
   whio_blockdev_setup()
   whio_blockdev_cleanup()
   whio_blockdev_free()
   whio_blockdev_in_range()
   whio_blockdev_write()
   whio_blockdev_read()
   whio_blockdev_wipe()
*/
typedef struct whio_blockdev
{
    /**
       Info about the blocks this object manages.
    */
    struct blocks
    {
	/**
	   Size of each block. MUST NOT be changed after setting up
	   this object, and doing so may lead to errors.
	*/
	whio_size_t size;
	/**
	   Number of blocks. MUST NOT be changed after setting up
	   this object, and doing so may lead to errors.
	*/
	whio_blockdev_id count;
	/**
	   Must be null or valid memory at least 'size' bytes
	   long. When a block is "wiped", this memory is copied over
	   that block.

	   The contents may be changed after setting up this object,
	   so long as the address stays valid (or is changed to
	   accommodate) and stays at least 'size' bytes long.
	*/
	void const * prototype;
    } blocks;
    /**
       Implementation details which the client should neither touch
       not look at.
    */
    struct impl
    {
	/**
	   This object's i/o device. It is created via
	   whio_blockdev_setup() and cleaned up by
	   whio_blockdev_clean(). It is a whio_dev subdevice
	   which fences i/o operations on the parent device
	   to the range defined by whio_blockdev_setup().
	*/
	whio_dev * fence;
    } impl;
} whio_blockdev;

/**
   A whio_blockdev object for cases where static initialization is necessary
   (e.g. member whio_blockdev objects).
*/
#define whio_blockdev_init_m {\
	{ /* blocks */ \
	    0 /*size*/,\
	    0 /*count*/,\
	    0 /*prototype*/\
	},\
	{ /* impl */ \
	    0 /*fence*/ \
	}\
    }

/**
   Empty initialization object.
*/
extern const whio_blockdev whio_blockdev_init;


/**
   Allocates and returns a new whio_blockdev, which the caller owns,
   or 0 on OOM.  Because this function might be configured to use a
   memory source other than malloc, the object must be destroyed using
   whio_blockdev_free() instead of free().

   @see whio_blockdev_free()
*/
whio_blockdev * whio_blockdev_alloc();

/**
   Initializes the given whio_blockdev object. bdev will use
   parent_store as its storage device, but will only access the device
   range [parent_offset,(block_size * block_count)). None of the
   parameters may be 0 except prototype. If prototype is not null then
   it must be valid memory at least block_size bytes long. When a
   block is "wiped" (see whio_blockdev_wipe()), this prototype object
   is written to it.

   The parent_store object must outlive bdev. Perforing any i/o on
   bdev after the parent i/o device is invalidated will lead to
   undefined results.

   On success, a call to whio_blockdev_cleanup() or
   whio_blockdev_free() must eventually be made for bdev to free up
   the internally allocated resources. See those functions for details
   on which to use.

   If bdev is passed to this function multiple times without a
   corresponding call to whio_blockdev_cleanup(), it will leak
   resources.

   Returns whio_rc.OK on success, some other value on error.
*/
int whio_blockdev_setup( whio_blockdev * bdev, whio_dev * parent_store, whio_size_t parent_offset,
			 whio_size_t block_size, whio_blockdev_id block_count, void const * prototype );
/**
   Cleans up internal memory owned by bdev but does not free bdev
   itself. After this, bdev may be passed to whio_blockdev_setup() to
   re-initialize it if needed.

   Returns true on success, false on error.

   If bdev is a default-initialized object then this function will
   likely attempt to free memory from an undefined memory region,
   leading to undefined behaviour.

   @see whio_blockdev_free()
   @see whio_blockdev_setup()
*/
bool whio_blockdev_cleanup( whio_blockdev * bdev );

/**
   Destroys bdev and any internal memory it owns. ONLY pass this
   object created using whio_blockdev_alloc() or malloc(). DO NOT pass
   this an object which was created on the stack, as that will lead to
   a segfault. For stack-allocated objects using
   whio_blockdev_cleanup() instead.

   @see whio_blockdev_cleanup()
   @see whio_blockdev_alloc()
*/
void whio_blockdev_free( whio_blockdev * bdev );

/**
   Returns true if id is a valid block ID for bdev, else false.
*/
bool whio_blockdev_in_range( whio_blockdev * bdev, whio_blockdev_id id );

/**
   Writes the contents of src to bdev at the underlying device
   position corresponding to the given block id.  On success it
   returns whio_rc.OK. It returns an error code if bdev or src are
   null, id is not in bounds, or on an i/o error. src must be valid
   memory at least bdev->blocks.size bytes long.
*/
int whio_blockdev_write( whio_blockdev * bdev, whio_blockdev_id id, void const * src );
/**
   Reads bdev->blocks.size bytes of memory from the block with the
   given id from bdev and copies it to dest. dest must be valid memory
   at least bdev->blocks.size bytes long. Returns whio_rc.OK on success.
*/
int whio_blockdev_read( whio_blockdev * bdev, whio_blockdev_id id, void * dest );

/**
   If a block prototype object was passed to whio_blockdev_setuo()
   then that object is written to the given block of bdev, otherwise
   whio_rc.ArgError is returned. Returns whio_rc.OK on success.
*/
int whio_blockdev_wipe( whio_blockdev * bdev, whio_blockdev_id id );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WANDERINGHORSE_NET_WHIO_DEVS_H_INCLUDED */
/* begin file whio_stream.h */
#ifndef WANDERINGHORSE_NET_WHIO_STREAM_H_INCLUDED
#define WANDERINGHORSE_NET_WHIO_STREAM_H_INCLUDED 1

/*
   This file contains declarations and documentation for the generic
   whio_stream API. The specific stream implementations are declared
   in whio_streams.h.
*/

/** @page page_whio_stream whio_stream C Stream API

  The whio_stream API provides an abstract interface for sequential
  streams which are either read-only or write-only. In practice, this
  type of stream is often the only type an application has access to
  for certain operations (as opposed to a full-fledged i/o device with
  random access, as modelled by the whio_dev API). This API is similar
  to that of whio_dev, but is somewhat smaller (because sequential
  streams have fewer requirements than random-access streams do).

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: Public Domain

 */
#include <stdarg.h> /* va_list */

#ifdef __cplusplus
extern "C" {
#endif

struct whio_stream;
/** @struct whio_stream_api

   whio_stream_api defines the "member functions" of the whio_stream
   class. It is an abstract interface for sequential streams.

   @see whio_stream
*/
struct whio_stream_api
{
    /**
       read() must read (at most) count bytes from its underlying
       source and write them to dest. It may read more than count
       (e.g. buffering) but must not write more than count bytes to
       dest, nor may it actually consume more bytes than that.

       It must return the number of bytes read, or 0 on EOF or error.
    */
    whio_size_t (*read)( struct whio_stream * self, void * dest, whio_size_t count );

    /**
       write() tries to write count bytes from src to its underlying
       destination. Returns the number of bytes written, 0 if it cannot
       write, or a number smaller than count on error.       
    */
    whio_size_t (*write)( struct whio_stream * self, void const * src, whio_size_t count );

    /**
       Close must close the stream, such that further reads or writes will
       fail. It must also free any resources owned by the instance, but must
       not free the self object.

       The interface requires that finalize() call close(), so normally
       client code does not need to call this. It is provided to allow
       for stack-allocated stream objects which otherwise could not
       be cleaned up uniformly.

       This function should returned false if !self or if the stream is
       not opened.
    */
    bool (*close)( struct whio_stream * self );

    /**
       finalize() must call close() and then free the self object.
       After finalize() returns, self is not a valid
       object.

       The proper way to destroy a whio_stream object is:

       @code
       theStream->api->finalize(theStream);
       @endcode

       Implementations of this function must ensure that they meet
       that expectation.

       DO NOT call this on a stack-allocated object - use close()
       instead (which is provided primarily for stack-allocated
       objects).
    */
    void (*finalize)( struct whio_stream * self );

    /**
       Flushes the write buffer (for write streams). On success it
       must return whio_rc.OK. On error it returns an
       implementation-defined non-zero value.
    */
    int (*flush)( struct whio_stream * self );

    /**
       isgood() returns whether or not self is in a valid use state.
       It should return true on eof, as eof is not strictly an error.
       To report EOF it should return 0 from the read()
       implementation.
    */
    bool (*isgood)( struct whio_stream * self );
};

typedef struct whio_stream_api whio_stream_api;

/**
   Hold implementation details for whio_stream instances. These are
   for use only by the functions related to a specific implementation
   of whio_stream implementation, and not client use.
*/
struct whio_stream_impl
{
    /**
       data is SOLELY for use by concrete implementations of
       whio_stream.
       
       data can be used to store private data required by the
       implementation functions. Each instance may have its own
       information (which should be cleaned up via the finalize()
       member function, assuming the stream owns the data).
    */
    void * data;

    /**
       A type identifier for use solely by whio_stream implementations,
       not client code. If the implementation uses this (it is an
       optional component), it must be set by the whio_stream
       initialization routine (typically a factory function).

       This mechanism works by assigning some arbitrary opaque
       value (unique per class) to all instances of a specific
       whio_stream implementation. The implementation funcs can
       then use that to ensure that they are passed the correct
       type. The typeID need not be public, but may be so. e.g.
       the author of the impl may provide a non-member
       whio_stream-related function which requires a specific type
       (or types), and in that case the typeID would possibly need
       to be known by the caller.
    */
    void const * typeID;
};
typedef struct whio_stream_impl whio_stream_impl;


/** @struct whio_stream

   whio_stream is an abstract interface for sequential streams. There
   is no default implementation - custom implementations must be
   provided which can handle specific stream types, e.g. FILE handles,
   an in-memory buffer, or a socket.

   The proper way to create any stream instance is from a factory
   function. The function may take any arguments necessary for
   constructing a new stream (or connecting to an existing one). For
   example, to create a stream for a FILE handle we might do:

   @code
   whio_stream * theStream = whio_stream_for_FILE(stdin,false);
   @endcode

   The public API of a stream object is accessed like:

   @code
   theStream->api->write( theStream, ... );
   @endcode

   (The first parameter as the equivalent of a "this" pointer,
   so we can get at instance-specific data.)

   For an explanation of why the "extra" api member exists, see the
   documentation for the whio_dev interface, which uses this same
   technique.

   The proper way to destroy a whio_stream object is:

   @code
   theStream->api->finalize(theStream);
   @endcode

   Implementations are responsible for properly implementing the
   finalize() member. Ownership of the underlying native stream is
   defined by the factory function which creates the stream.

   For examples of creating concrete implementations of this type,
   see the files whio_stream_FILE.c and whio_stream_dev.c.
*/
struct whio_stream
{
    const whio_stream_api * api;
    whio_stream_impl impl;
};
/**
   Convenience typedef.
*/
typedef struct whio_stream whio_stream;

/**
   Empty initialization object for whio_streams.
*/
extern const whio_stream whio_stream_init;

/**
   An initializer macro for use in whio_stream subclass struct
   initializers.

   The arguments correspond to the five member functions,
   followed by the type ID (which may be set to 0 if factory
   code will later set it to a valid value).
*/
#define whio_stream_init_m(Read,Write,Destroy,Flush,IsGood,TypeID)	\
    { { Read, Write, Destroy, Flush, IsGood, { 0, TypeID } }

/**
   Equivalent to whio_dev_writefv() except that it takes a whio_stream
   object instead of a whio_dev.
*/
whio_size_t whio_stream_writefv(whio_stream * stream, char const * fmt, va_list args );

/**
   Equivalent to whio_stream_writefv() except that it takes an (...) 
   elipses list instead of a va_list.
*/
whio_size_t whio_stream_writef( whio_stream * stream, char const * fmt, ... );

/**
   Convenience function to read the next character from a whio_stream. If tgt
   is not 0 then it is assigned to the value of the character.
   Returns true if it reads a character, else false.

   Example:

   @code
   char x = 0;
   if( whio_stream_getchar( myStream, &x ) ) {  ... }
   @endcode
*/
bool whio_stream_getchar( whio_stream * stream, char * tgt );


/**
   Consumes stream to the first \\n character.  It appends that data, minus the newline,
   to dest. Returns the number of characters appended to dest, or 0 at EOF or on a read
   error.

   Note that the stream is consumed and the trailing newline character
   (if any) is effectively lost.
*/
//whio_size_t whio_stream_readln_membuf(whio_stream * stream, struct memblob * dest );

/**
   Functionally identical to whio_stream_readln_membuf() except that the
   line is returned as a null-termined string which the caller must
   clean up using free(). On error or EOF 0 is returned.
*/
//char * whio_stream_readln(whio_stream * stream);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // WANDERINGHORSE_NET_WHIO_STREAM_H_INCLUDED
/* begin file whio_streams.h */
#ifndef WANDERINGHORSE_NET_WHIO_STREAMS_H_INCLUDED
#define WANDERINGHORSE_NET_WHIO_STREAMS_H_INCLUDED 1
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/


/*
   This file contains declarations and documentation for the concrete
   whio_stream implementations. The generic stream API is declared in
   whio_stream.h.
*/
#ifdef __cplusplus
extern "C" {
#endif

/**
   Creates a stream object which wraps the given whio_dev object.
   If takeOwnership is true then ownership of dev is passed to the
   new object. dev must outlive the returned object or results
   are undefined.

   Returns 0 if !dev or on an alloc error, otherwise it returns
   a new object which the caller must eventually free using
   str->api->finalize(str). If the stream owns dev then dev will be
   destroyed at that point, too.
*/
whio_stream * whio_stream_for_dev( whio_dev * dev, bool takeOwnership );

/**
   Creates a whio_stream wrapper around the given FILE handle. If fp
   was opened in read mode, it is illegal to use the stream in a write
   context (but this routine cannot check that). Likewise, if it was
   open for write mode, it is illegal to use the stream in a read
   context (again, this code cannot check that).

   The takeOwnership argument determines the ownerhsip of fp: if the
   function succeeds and takeOwnership is true then fp's ownership is
   transfered to the returned object. In all other cases ownership is
   not changed.

   The returned stream object must be destroyed by calling
   stream->destroy(stream). If the stream owns the FILE handle then
   that will close the FILE handle.

   If you want to write to stdout, simply use:

   @code
   whio_stream * out = whio_stream_for_FILE(stdout, false);
   @endcode

   And similar for stdin or stderr.
*/
whio_stream * whio_stream_for_FILE( FILE * fp, bool takeOwnership );

/**
   Works like whio_stream_for_FILE(), except that it accepts a
   filename and a file open mode argument (the same as expected by
   fopen()), and the stream takes over ownership of the underlying
   FILE handle.

   If allocation of the new stream or fopen() fails then 0 is returned.

   For output streams, for mode you will normally want mode "a" (if
   you want to keep the old contents) or "w" (if you want to lose the
   old contents). For input streams, use mode "r". Optionally, you can
   append the letter 'b' to the mode string for platforms which treat
   binary and text streams differently (POSIX platforms don't and
   probably ignore the 'b').

   The returned stream object must be destroyed by calling
   stream->destroy(stream).
*/
whio_stream * whio_stream_for_filename( char const * src, char const * mode );

/**
   Similar to whio_stream_for_FILE() but it takes an existing/open
   file handle number and uses fdopen() to try to open it. On success,
   a new whio_stream object is returned which wraps that FILE
   object. On error (fdopen() or a malloc() fails) 0 is returned.

   See the man page for fdopen() for details of how this might or might not
   behave the exact same as opening a FILE handle.

   The man pages say that the open mode (e.g "r", "r+", "w+", etc.) 
   "must be compatible with" the one used for opening the file
   descriptor in the first place. They do not say what "compatible"
   means, though (e.g. are "w" and "w+" compatible?). Because of this,
   this function may or may not be able to associate a FILE handle
   with the descriptor, as we cannot know the exact flags used to open
   that stream.
*/
whio_stream * whio_stream_for_fileno( int fileno, bool writeMode );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // WANDERINGHORSE_NET_WHIO_STREAMS_H_INCLUDED
/* begin file whio_zlib.h */
#if !defined(WANDERINGHORSE_NET_WHIO_ZLIB_H_INCLUDED)
#define WANDERINGHORSE_NET_WHIO_ZLIB_H_INCLUDED 1

#if ! defined(WHIO_ENABLE_ZLIB)
#  define WHIO_ENABLE_ZLIB 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
   Compresses src to dest using gzip compression of the level specified
   by the level parameter (3 is an often-used choice). zlib provides
   several constants for this value:

   Z_NO_COMPRESSION, Z_BEST_SPEED, Z_BEST_COMPRESSION, and
   Z_DEFAULT_COMPRESSION.

   If level is not in the bounds defined by those constants, it will
   be adjusted up (if too low) or down (if too high) to the minimum or
   maximum compression level.

   src must be a readable stream and dest must be writeable. They may
   not be the same object.

   If whio is not compiled with WHIO_ENABLE_ZLIB defined to a true value,
   this function does nothing and returned whio_rc.UnsupportedError.

   Returns whio_rc.OK on success, else some error value from one of
   zlib routines (a non-zero error code defined in zlib.h). If
   !src or !dest or (src==dest) then whio_rc.ArgError is returned.

   The compressed data is decompressable by gzip-compatible tools.

   Note that because a whio_stream instance can be created for any
   whio_dev device (see whio_stream_for_dev()), it is possible to use
   this routine to compress any i/o device to another.  However,
   random access with transparent compression/decompression is not
   supported (very few people have every managed to code that).

   On error, any number of bytes may or may not have been read from src
   or written to dest.

   @see whio_stream_gunzip()
   @see whio_stream_for_dev()
 */
int whio_stream_gzip( whio_stream * src, whio_stream * dest, int level );

/**
   Assumes src contains gzipped data and decompresses it to dest.

   src must be a readable stream and dest must be writeable. They may
   not be the same object.

   If whio is not compiled with WHIO_ENABLE_ZLIB defined to a true value,
   this function does nothing and returned whio_rc.UnsupportedError.

   Returns whio_rc.OK on success, else some error value from one of
   zlib routines (a non-zero error code defined in zlib.h). If !src or
   !dest or (src==dest) then whio_rc.ArgError is returned.

   On error, any number of bytes may or may not have been read from src
   or written to dest.

   @see whio_stream_gzip()
   @see whio_stream_for_dev()
*/
int whio_stream_gunzip( whio_stream * src, whio_stream * dest );

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* WANDERINGHORSE_NET_WHIO_ZLIB_H_INCLUDED */
