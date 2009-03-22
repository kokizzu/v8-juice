/* auto-generated on Sun Mar 22 01:12:37 CET 2009. Do not edit! */
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
#ifndef WANDERINGHORSE_NET_WHPRINTF_H_INCLUDED
#define WANDERINGHORSE_NET_WHPRINTF_H_INCLUDED 1
#include <stdarg.h>
#include <stdio.h> /* FILE handle */
#ifdef __cplusplus
extern "C" {
#endif
/** @page whprintf_page_main whprintf printf-like API

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
long whprintf_file( FILE * fp, char const * fmt, ... );


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
#endif /* WANDERINGHORSE_NET_WHPRINTF_H_INCLUDED */
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
   parts of two other libraries, but the parts were found to be generic
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
    printf("Read %"WHIO_SIZE_T_PFMT" bytes\n", n );
}
dev->api->finalize(dev);
@endcode

Note the use of WHIO_SIZE_T_PFMT instead of "%%u", in the printf call,
so that the code will work correctly with varying sizes of
whio_size_t.
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
   (and in practice always does) store instance-specific data
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
       in their cleanup routines. If it is 0 then they must
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
   (sizeof(whio_dev_api) - sizeof(whio_dev_api*)) bytes on each instance
   of whio_dev, and in its place we have a single shared (and static)
   instance of the implementation's API object.

   Thread safety: it is never legal to use any given whio_dev instance
   from more than one thread at a single time, and doing so will
   almost certainly corrupt the internal state of the stream. e.g. its
   read/write position may be moved by another thread, causing a read
   or write to go somewhere other than desired. It is in theory not
   problematic for multiple threads to share one whio_dev instance as
   long as access to the device is strictly serialized via a
   marshaller and device positioning operations are written to take
   that into account.
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
   Returns the size of the given device, or whio_rc.SizeTError if !dev
   or if the device returns that code to signify that it is not
   seekable. The size is calculated by seek()ing to the end of the
   device and using that offset. Thus the device must of course
   support seeking. The device is positioned to its pre-call position
   before the function returns.
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
   not use the top byte and should instead mask their value against
   one of these (this assists in documentation, by making it clear
   which category an ioctl command belongs to).
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
   handle. On success, ownership of f is defined by the takeOwnership
   argument (see below) and the returned object must eventually be finalized with
   dev->api->finalize(dev).

   For purposes of the whio_dev interface, any parts which have
   implementation-specified behaviour will behave as they do
   for the local FILE-related API. e.g. ftruncate() may or may
   not be able to extend a file, depending on the platform
   and even the underlying filesystem type.

   The takeOwnership argument determines the ownerhsip of f: if the
   function succeeds and takeOwnership is true then f's ownership is
   transfered to the returned object. In all other cases ownership is
   not changed. If the device owns f then closing the device
   will also close f. If the device does not own f then f must
   outlive the returned device.

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

   It not enough memory can be allocated for the intitial buffer then
   this function returns 0. On success, the caller owns the returned
   object and must eventually destroy it by calling
   dev->api->finalize(dev).

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
   range. For read-only access, use whio_dev_for_memmap_ro() instead.

   The arguments:

   - mem = the read/write memory the device will wrap. Ownership is
   not changed.  May not be 0. If mem's address changes during the
   lifetime of the returned object (e.g. via a realloc), results are
   undefined an almost certainly ruinous. If you want this device to
   free the memory when it is destroyed, set dev->client.data to the
   mem parameter and set dev->client.dtor to an appropriate destructor
   function (free() would suffice for memory allocated via alloc()).

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

   - write() on a read-only memory buffer returns 0, as opposed to
   whio_rc.SizeTError.

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
   be assured that each logical block remains separate and that any
   operation in one block will not affect any other subdevices which
   have their own blocks (barring any bugs in this code or overlapping
   byte rangees). Results are of course unpredictable if multiple
   devices map overlapping ranges in the parent device (then again,
   maybe that's what you want, e.g. as a communication channel).

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


/**
   A type used to represent sequential block IDs for the whio_blockdev
   API.  It is not hard-coded because for my intended uses it will be
   changed to other types depending on compilation options.

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
   and cleaned up (depending on how they were created) using
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
   a segfault. For stack-allocated objects use whio_blockdev_cleanup()
   instead.

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
/* begin file whglob.h */
#if !defined(WANDERINGHORSE_NET_WHGLOB_H_INCLUDED)
#define WANDERINGHORSE_NET_WHGLOB_H_INCLUDED 1
/** @page whglob_page_main whglob Globbing Functions
   
    This is a small API for doing string matching using glob or SQL
    LIKE patterns. The code was originally part of the sqlite3
    (http://sqlite.org) project but was found to be easy to extract
    and genericize, so here it is. They are encapsulated in the
    whglob_matches() and whglob_matches_like() functions.

*/
#ifdef __cplusplus
extern "C" {
#endif

    /**
       Checks str to see if it matches the given pattern, which is
       assumed to follow standard globbing pattern rules. On success,
       non-0 is returned.

       Example:

       @code
       if( whglob_matches( "*.c", __FILE__ ) ) {
           puts("This is a .c file!");
       }
       @endcode

       Globbing rules:

       - '*' Matches any sequence of zero or more characters.
       - '?' Matches exactly one character.
       - [...] Matches one character from the enclosed list of characters.
       - [^...]  Matches one character not in the enclosed list.

       With the [...] and [^...] matching, a ']' character can be
       included in the list by making it the first character after '['
       or '^'.  A range of characters can be specified using '-'.
       Example: "[a-z]" matches any single lower-case letter.  To
       match a '-', make it the last character in the list.

       This routine is usually quick, but can be N**2 in the worst case.

       Hints: to match '*' or '?', put them in "[]".  Like this:

        abc[*]xyz        Matches "abc*xyz" only
    */
    int whglob_matches( char const * pattern, char const * str );

    /**
       Checks str to see if it matches the given pattern, which must
       follow SQL's LIKE pattern rules (where the wildcard character
       is a percent sign). On success, non-0 is returned. If
       caseSensitive is non-0 then a case-sensitive search is done
       (ANSI SQL-92 specifies that LIKE is case-insensitive).

       @code
       if( whglob_matches_like( "%.c", __FILE__ ) ) {
           puts("This is a .c file!");
       }
       @endcode

    */
    int whglob_matches_like( char const * pattern, char const * str, char caseSensitive );

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif
/* begin file whbits.h */
#ifndef WANDERINGHORSE_NET_WHBITS_H_INCLUDED
#define WANDERINGHORSE_NET_WHBITS_H_INCLUDED 1

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int whbits_count_t;
/**
   A type for managing bitsets.
*/
typedef struct whbits
{
    whbits_count_t sz_bytes;
    whbits_count_t sz_bits;
    unsigned char * bytes;
} whbits;
/**
   Static initializer for whbits objects.
*/
#define WHBITS_INIT { 0, 0, 0 }
/**
   Empty initializer for whbits objects.
*/
extern const whbits whbits_init_obj;

/**
   Sets up a new bitset array for the given number of bits. Each byte
   of the array will get the value initialState.

   If tgt contains any bytes and the new bitCount is smaller than the old one,
   it re-uses the old memory.

   On success, 0 is returned and tgt is populated with a bitset
   (potentially allocated using malloc()). The object may be freely
   copied after that, but ONE those copies must eventually be passed
   to whbits_free_bits() to release the memory.
*/
int whbits_init( whbits * tgt, whbits_count_t bitCount, unsigned char initialState );

/**
   Frees any memory associated with b and clears b's state, but does
   not free() b.
*/
void whbits_free_bits( whbits * b );

#define WHBITS_BYTEFOR(B,BIT) ((B)->bytes[ BIT / 8 ])
#define WHBITS_SET(B,BIT) ((WHBITS_BYTEFOR(B,BIT) |= (0x01 << (BIT%8))),0x01)
#define WHBITS_UNSET(B,BIT) ((WHBITS_BYTEFOR(B,BIT) &= ~(0x01 << (BIT%8))),0x00)
#define WHBITS_GET(B,BIT) ((WHBITS_BYTEFOR(B,BIT) & (0x01 << (BIT%8))) ? 0x01 : 0x00)
#define WHBITS_TOGGLE(B,BIT) (WHBITS_GET(B,BIT) ? (WHBITS_UNSET(B,BIT)) : (WHBITS_SET(B,BIT)))


/**
   Sets bit number bitNum in b and returns non-zero on success.
*/
char whbits_set( whbits* b, whbits_count_t bitNum );

/**
   Unsets bit number bitNum in b and returns non-zero on success.
*/
char whbits_unset( whbits * b, whbits_count_t bitNum );

/**
   Gets the value of the bit number bitNum in b.
*/
char whbits_get( whbits const * b, whbits_count_t bitNum );


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* WANDERINGHORSE_NET_WHBITS_H_INCLUDED */
/* begin file whdbg.h */
#ifndef WANDERINGHORSE_NET_WHDBG_H_INCLUDED
#define WANDERINGHORSE_NET_WHDBG_H_INCLUDED 1

/** @page whdbg_page_main whdbg: Debugging Library for C

@section whdbg_sec_about About whdbg

Authors: Stephan Beal (http://wanderinghorse.net/home/stephan/)

License: the core code is in the public domain

Example usage:
@code
// in main():
whdbg_set_stream( stderr );
whdbg_set_flags( whdbg_get_flags() | WDBG_INFO );

// ... in an arbitrary function ...
int x = 4;
WHDBG(WHDBG_INFO)("x=%d", x);
@endcode

That will send something like the following to the specified debug stream:

@verbatim
whdbg(WHDBG_INFO):myfile.c:392:my_function():
    x=42
@endverbatim

The flags passed to WHDBG() and whdbg_set_flags(), and returned by
whdbg_get_flags(), need not come from the predefined WHDBG_xxx flags -
it they can be an arbitrary bitmask. The exceptions are that the value
0 is reserves for "never output" and WHDBG_ALWAYS is reserved for
"always output".

If this library is compiled with WHDBG_CONFIG_ENABLE set to a false
value then debugging is disabled in a way such that all calls to
WHDBG() "should" be optimized out by the compiler.

*/

#include <stdarg.h>
#include <stdio.h> /* FILE */

#ifdef __cplusplus
extern "C" {
#endif



/** @def WHDBG_CONFIG_ENABLE

   If WHDBG_CONFIG_ENABLE is set to a true value then whdbg()
   and friends are activated. If it is a false value then whdbg()
   (and friends) become no-ops, and calls to WHDBG() resolve to
   complete no-ops (no function calls) and should be optimized by the
   compiler out altogether.
*/
#if !defined(WHDBG_CONFIG_ENABLE)
#  define WHDBG_CONFIG_ENABLE 1
#endif

/** @enum whdbg_flags

The whdbg_flags enum contains a bitmask of logging/debugging flags,
for use with whdbg() and friends.
*/
enum whdbg_flags {

/**
   Never log any debug message.
*/
WHDBG_NEVER = 0x00000000,

/**
   General error mask.
*/
WHDBG_ERROR = 0x10000000,

/**
   Log allocation events. This is inherently
   dangerous, as logging can cause an alloc.
*/
WHDBG_ALLOC_ERR = WHDBG_ERROR | 0x02,

/**
   Log error messages.
*/
WHDBG_IO_ERROR = WHDBG_ERROR | 0x0001,

/**
   FIXME markers
 */
WHDBG_FIXME =  WHDBG_ERROR | 0x02,

/**
   General warning mask.
*/
WHDBG_WARNING = 0x20000000,
WHDBG_NYI =    WHDBG_WARNING | 0x01,

WHDBG_INFO = 0x40000000,
/**
   "For your information..." or "To whom it may concern..."
*/
WHDBG_FYI = WHDBG_INFO,
/**

   Log for allocation actions.
*/
WHDBG_ALLOC = WHDBG_INFO | 0x0001,

/**
   Log deallocation events.
*/
WHDBG_DEALLOC =  WHDBG_INFO | 0x0002,

/**
   For apps with a "verbose" flag.
*/
WHDBG_VERBOSE =  0x00010000,
/**
   Factory-related messages (e.g. registration).
 */
WHDBG_FACTORY =  0x00020000,

/**
   Log any memory-related events.
*/
WHDBG_MEMORY = WHDBG_DEALLOC | WHDBG_ALLOC_ERR | WHDBG_ALLOC,

/**
   This range is reserved for client application use, as are
   variables/enum entries named WHDBG_APP_xxx.
*/
WHDBG_APP = 0x7F000000,

/**
   Default debug level.
*/
WHDBG_DEFAULT = WHDBG_WARNING | WHDBG_ERROR | WHDBG_APP,

/**
   Log anything except WHDBG_NEVER.
*/
WHDBG_ALWAYS = 0x7fffffff
};



/** @internal

   Internal typedef. Part of the WHDBG macro definition, and not
   to be used outside of that context. Used like fprintf(), with
   the stream being that set via whdbg_set_stream(). If no stream
   is set, no output is generated.
*/
typedef void (*whdbg_PRIVATE_f)( char const * fmt, ... );

/** @internal

   Internal function. Part of the WHDBG macro definition, and not
   to be used outside of that context.

   This routine is not thread-safe, and there may be collisions in the
   logger if multiple threads use this API at once. In a best
   case you'd get wrong file location information. In a worst case,
   a crash.
*/
whdbg_PRIVATE_f whdbg_PRIVATE_phase1( unsigned int condition,
					    char const * condString,
					    char const * file,
					    unsigned int line,
					    char const * funcName );

/**
   WHDBG() is a much simplified way of calling whdbg(). It is used like this:

   @code
   WHDBG(WHDBG_WHATEVER|WHDBG_WHATEVER_ELSE)("num=%d val=%s",num,val);
   @endcode

   Note the extra set of parenthesis.

   As a log message it will use the string form of the flags instead
   of the numeric form.

   If WHDBG_CONFIG_ENABLE is set to a false value this macro evaluates
   to an if(false) block and "should" be compiled out by compilers.

*/
#define WHDBG(COND) if(WHDBG_CONFIG_ENABLE) whdbg_PRIVATE_phase1( COND, # COND, __FILE__, __LINE__, __func__ )

/**
   WHDBG_FLAGS() is a simpler way to pass the required arguments to
   whdbg(). Instead of:

   @code
   whdbg(WHDBG_WARNING,WHDBG_SOURCE_INFO,"...",...);
   @endcode

   Try:

   @code
   whdbg(WHDBG_FLAGS(WHDBG_WARNING),"...",...);
   @endcode

   Or a bit friendlier:

   @code
   WHDBG(WHDBG_WARNING)("...", ...);
   @endcode

*/
#define WHDBG_FLAGS(CONDITION) (CONDITION),WHDBG_SOURCE_INFO

/**
   Convenience macro to log allocations. Takes a size (unsigned int) argument.
*/
#define WHDBGME_ALLOC(SIZE) WHDBG(WHDBG_ALLOC)("alloc of %u bytes",(SIZE))

/**
   Convenience macro to log allocations. Takes a typename argument.
*/
#define WHDBGME_ALLOCT(TYPE) WHDBG(WHDBG_ALLOC)("alloc of %s object: %u bytes",# TYPE, sizeof(TYPE))

/**
   Convenience macro to log allocations. Takes a typename argument followed by
   a pointer to an object of that type.
*/
#define WHDBGME_ALLOCTP(TYPE,PTR) WHDBG(WHDBG_ALLOC)("alloc of %s object @%p: %u bytes",# TYPE, (PTR), sizeof(TYPE))

/**
   De-alloc form of WHDBGME_ALLOC.
*/
#define WHDBGME_DEALLOC(SIZE) WHDBG(WHDBG_DEALLOC)("dealloc of %u bytes",(SIZE))

/**
   De-alloc form of WHDBGME_ALLOCT.
*/
#define WHDBGME_DEALLOCT(TYPE) WHDBG(WHDBG_DEALLOC)("dealloc of %s object: %u bytes",# TYPE, sizeof(TYPE))

/**
   De-alloc form of WHDBGME_ALLOCTP.
*/
#define WHDBGME_DEALLOCTP(TYPE,PTR) WHDBG(WHDBG_DEALLOC)("dealloc of %s object @%p: %u bytes",# TYPE, (PTR), sizeof(TYPE))

/**
   Gets the current debug mask. See the whdbg_flags enum for
   the ones provided byt his library.

   @see whdbg_set_flags()
*/
unsigned int whdbg_get_flags();

/**
   Sets the current debug mask and returns the previous one. See the
   whdbg_flags enum for details.

   The flags passed to WHDBG() and whdbg_set_flags(), and returned by
   whdbg_get_flags(), need not come from the predefined WHDBG_xxx
   flags - it they can be an arbitrary bitmask. The exceptions are
   that the value 0 is reserves for "never output" and WHDBG_ALWAYS is
   reserved for "always output".

    @see whdbg_get_flags()
*/
unsigned int whdbg_set_flags(unsigned int newflags);

/**
   Sets the debug output stream to f. Setting it to 0 completely
   disables debug output sent via whdbg() and friends.

   Does not transfer ownership of f, and f must outlive all
   debugging output or be replaced before being closed.
*/
void whdbg_set_stream( FILE * f );

/** @def WHDBG_SOURCE_INFO
   This macro should always be pass as the 2nd argument
   to whdbg() and whdbgv().
*/
#define WHDBG_SOURCE_INFO __FILE__,__LINE__,__func__

/** @def WHDBG_SOURCE_INFO_PARAMS_DECL
   For use by whdbgv() and friends.
*/
#define WHDBG_SOURCE_INFO_PARAMS_DECL char const *_file, unsigned int _line, char const *_func

/**
   If (condition & whdbg_get_flags()) and a debug stream has been
   set via whdbg_set_stream(), then the formatted string, followed
   by a newline character is sent to the debug channel, otherwise
   nothing happens.

   Always pass the macro WHDBG_SOURCE_INFO as the second argument.

   As a special case, if either condition or whdbg_get_flags() are
   equal to WHDBG_ALWAYS then the message is always sent regardless
   of the current debug mask (provided there's a stream to send it
   to).

   The whdbg API internally uses whdbg() extensively to send
   messages.  This function isn't *really* for client-side use, but it
   interesting for debuggering purposes and for add-on whdbg code (like
   the i/o support), so its part of the public API.
*/
void whdbgv( unsigned int condition,
		WHDBG_SOURCE_INFO_PARAMS_DECL,
		char const * fmt,
		va_list vargs );
/**
   Identical to whdbgv() but takes an ellipses list instead of
   a va_list.
*/
void whdbg( unsigned int condition,
	       WHDBG_SOURCE_INFO_PARAMS_DECL,
	       char const * fmt,
		... );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WANDERINGHORSE_NET_WHDBG_H_INCLUDED */
/* begin file whefs_config.h */
#if !defined(WANDERINGHORSE_NET_WHEFS_CONFIG_H_INCLUDED)
#define WANDERINGHORSE_NET_WHEFS_CONFIG_H_INCLUDED

/*
  This file contains the compile-time-configurable parts of
  libwhefs. Versions of whefs compiled using different options will
  not be compatible, as these options can affect data sizes and
  ranges, as well as the VFS filesystem layout.

  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/

#if defined(WHEFS_ID_TYPE_BITS)
#  error "WHEFS_ID_TYPE_BITS must not be defined before including this file! See the docs for WHEFS_ID_TYPE_BITS for details!"
#endif

#if ! defined __STDC_FORMAT_MACROS
/* The Linux stdint.h says:

"The ISO C99 standard specifies that these macros must only be
 defined if explicitly requested."
*/
#  define __STDC_FORMAT_MACROS 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @typedef whefs_id_type

   whefs_id_type is the numeric type used to store block and inode IDs
   in a VFS, as well as any count/size values which refer to such
   items.. The library supports building with different sizes of this
   type, largely as a way to conserve memory, and this typedef helps
   support that. See the documentation for WHEFS_ID_TYPE_BITS for full
   details.
*/

/** @def WHEFS_ID_TYPE_BITS

WHEFS_ID_TYPE_BITS tells us how many bits to use for inode and data
block IDs. The supported values are 8, 16, 32, or 64, and libraries
compiled with different values will not be compatible (nor can they
read the others' format, though i'm looking into ways to be able to
enable that).

The number of bits here is significant because:

- We are limited to (2^WHEFS_ID_TYPE_BITS-2) inodes and blocks
(but for all practical purposes, this isn't a limitation even with
16 bits).

- The layout of the filesystem is dependent on the number of bits in
this type, as the number of bytes used for certain data fields changes.

- When/if inode or block caching is added, the cache size will incrase
by (WHEFS_ID_TYPE_BITS/8) for each ID field stored in the cache
(that's a minimum of 2 IDs per object, and maybe up to 6 per inode
once directory support is added). The number grows quickly.

In practice, a 16-bit ID type is completely sufficient, but the
library was originally created with size_t as the identifier type, so
it supports 32-bit IDs as well. The switch to 16 as the default was
made to help save memory when/if inode caching gets added to the
library. In any case, the filesystem's minimalistic implementation is
not scalable to tens of thousands of files, and so 16 bits is a very
realistic limit.

In theory, 64-bit IDs are also okay, but (A) for this particular use
case that is way overkill, (B) it's a huge waste of memory (8 bytes
where we realistically don't need more than 2), and (C) the filesystem
model itself is not scalable to that level of use (e.g. millions of
files). inode/block IDs are always sequential, starting at 1, and by
the time we hit that number of blocks or inodes, the computer's memory
would almost certainly be filled.

i would strongly prefer to have WHEFS_ID_TYPE_BITS as an enum constant
instead of a macro value, but we unfortunately need some conditional
compilation based on the bit count.

If this constant is changed, all whefs client code which depends on
it must be recompiled and all filesystems written using the old value
will not be readable. That is the reason it is not set up with an
ifndef guard, so clients cannot blithely change it. If you are copying
whefs directly into another project, feel free to change the value
all you want, but be aware of the compatibility ramifications. Doing so
may also screw up any printf() (or similar) commands which have a hard-coded
format specifier which no longer matches after changing this value.
*/
#define WHEFS_ID_TYPE_BITS 16

/** @def WHEFS_ID_TYPE_PFMT

    WHEFS_ID_TYPE_PFMT is a helper string for writing portable
    printf-style format strings in conjunction with whefs_id_type. It
    works by using C99-standard macros defined in inttypes.h. It
    selects which specifier to use based on the value of
    WHEFS_ID_TYPE_BITS.  The system's inttypes.h will select a
    platform/bitness-dependent value.

    @see WHEFS_ID_TYPE_SFMT
*/

/** @def WHEFS_ID_TYPE_SFMT

    WHEFS_ID_TYPE_SFMT is the scanf counterpart of WHEFS_ID_TYPE_PFMT.

    @see WHEFS_ID_TYPE_PFMT
*/


/** @var whefs_fs_magic_bytes

    whefs_fs_magic_bytes is the whefs "internal" magic cookie, an
    array of numeric values used for sanity checking and confirming
    that input files are really whefs VFS files. This is independent
    of the client cookie, and dependent on the library version. When
    the core library's binary expectations change, in particular how
    the FS objects are stored change, this number will change. That
    will inherently make all other versions of whefs (both older and
    newer!) incompatible with VFSs created by this version.

    The convention is to store YYYY-MM-DD-WHEFS_ID_TYPE_BITS, using the
    date of the change as the value. Conventions aside, the only hard
    requirement (if you're customizing this) is that the array be
    terminated by a 0, as some routines may rely on that so they don't
    have to know the length of the array in advance.

    Maintenance reminder: When the version number is changed,
    be sure to change WHEFS_MAGIC_STRING_PREFIX as well!

    @see WHEFS_MAGIC_STRING_PREFIX WHEFS_MAGIC_STRING
*/
static const uint32_t whefs_fs_magic_bytes[] = { 2009, 1, 18, WHEFS_ID_TYPE_BITS, 0 };
/** @def WHEFS_MAGIC_STRING_PREFIX

    WHEFS_MAGIC_STRING_PREFIX is an internal helper macro to avoid
    some code repetition.

    Maintenance reminder: keep this value in sync with that defined
    in whefs_fs_magic_bytes!

    @see whefs_fs_magic_bytes WHEFS_MAGIC_STRING
*/
#define WHEFS_MAGIC_STRING_PREFIX "whefs version 20090118 with "

#if WHEFS_ID_TYPE_BITS == 8
/* for very, very limited filesystems. There's lots of room for overflows here! */
#  define WHEFS_MAGIC_STRING WHEFS_MAGIC_STRING_PREFIX"8-bit IDs"
#  define WHEFS_ID_TYPE_PFMT PRIu8
#  define WHEFS_ID_TYPE_SFMT SCNu8
    typedef uint8_t whefs_id_type;
#elif WHEFS_ID_TYPE_BITS == 16
/* the most realistic value, IMO. */
#  define WHEFS_MAGIC_STRING WHEFS_MAGIC_STRING_PREFIX"16-bit IDs"
#  define WHEFS_ID_TYPE_PFMT PRIu16
#  define WHEFS_ID_TYPE_SFMT SCNu16
    typedef uint16_t whefs_id_type;
#elif WHEFS_ID_TYPE_BITS == 32
#  define WHEFS_MAGIC_STRING WHEFS_MAGIC_STRING_PREFIX"32-bit IDs"
#  define WHEFS_ID_TYPE_PFMT PRIu32
#  define WHEFS_ID_TYPE_SFMT SCNu32
    typedef uint32_t whefs_id_type;
#elif WHEFS_ID_TYPE_BITS == 64
#  define WHEFS_ID_TYPE_PFMT PRIu64
#  define WHEFS_ID_TYPE_SFMT SCNu64
#  define WHEFS_MAGIC_STRING WHEFS_MAGIC_STRING_PREFIX"64-bit IDs"
    typedef uint64_t whefs_id_type;
#else
#  error "WHEFS_ID_TYPE_BITS must be one of: 8, 16, 32, 64"
#endif

/** @def WHEFS_MAGIC_STRING

   The default magic cookie string used by the library.
*/

/** @enum whefs_constants

    This enum contains some important compile-time constants
    for whefs.
*/
enum whefs_constants {
/**
   WHEFS_MAX_FILENAME_LENGTH defines the hard maximum string length
   for filenames in a vfs, not including the null terminator. This
   does not include the names of any parent directories.

   whefs_fs_options::filename_length must not be greater than this
   number.

   This value plays a big part in the in-memory size of a vfs inode,
   and should be kept to a reasonable length. While 1024 sounds
   reasonable, setting it that high will drastically increase the
   memory costs of certain operations.
 */
WHEFS_MAX_FILENAME_LENGTH = 128,

/**
   On-disk encoded size of whefs_id_type objects. This will always be
   one of 2, 4, or 8, depending on the size of whefs_id_type, plus 1
   tag byte used by the encoding/decoding bits for consistency
   checking.

   Maintenance reminder: we use (WHEFS_ID_TYPE_BITS/8) here instead of
   sizeof(whefs_id_type), because we need a specific guaranteed size
   for each supported value of WHEFS_ID_TYPE_BITS, and sizeof()
   doesn't give us that.
*/
whefs_sizeof_encoded_id_type = (WHEFS_ID_TYPE_BITS/8+1),

/**
   The maximum value of a whefs_id_type object, used for
   overflow checks.
*/
whefs_id_type_max_val = (whefs_id_type)-1,

/**
   The length of the whefs_fs_magic_bytes array, not including the
   tailing 0 entry.
*/
whefs_fs_magic_bytes_len = (sizeof(whefs_fs_magic_bytes)
			    /sizeof(whefs_fs_magic_bytes[0]) - 1)

};

/** @def WHIO_FS_USE_FCNTL

If WHIO_FS_USE_FCNTL is true then some features for doing
POSIX-style fcntl() advisory locks are added. The locking
code is far from complete and should not be relied upon
for concurrency purposes.

Once it is complete, the fcntl() locking will only work for file-based
base VFSes, not memory-based ones. When it is opened the VFS tries
to determine if it is using a file and if it is then locks will be
used for certain operations.

Maintenance reminder: if this is true, add, be sure to include
fcntl.h in the files which need it.

*/
#define WHIO_FS_USE_FCNTL 1


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* WANDERINGHORSE_NET_WHEFS_CONFIG_H_INCLUDED */
/* begin file whefs.h */
#ifndef WANDERINGHORSE_NET_WHEFS_H_INCLUDED
#define WANDERINGHORSE_NET_WHEFS_H_INCLUDED

#if ! defined __STDC_FORMAT_MACROS
/* The Linux stdint.h says:

"The ISO C99 standard specifies that these macros must only be
 defined if explicitly requested."
*/
#  define __STDC_FORMAT_MACROS 1
#endif
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

//Doxygen won't allow us to have both @page and @mainpage, which is problematic
//when we re-use headers in different projects which also use doxygen.
//@page page_whefs_main whefs: Embedded/Virtual Filesystem
/** 
    @mainpage whefs: Embedded/Virtual Filesystem

   ACHTUNG: this is beta software!

   whefs is a C library implementing an embedded/virtual filesystem
   (VFS).  It works by creating a so-called container file. This API
   can then treat that container similarly to a filesystem. In essence
   this is similar to conventional archives (e.g. zip files), except
   that this library provides random-access read/write support to the
   fs via an API similar to fopen(), fread(), fwrite() and
   friends. Also, a VFS container is more like a real filesystem than
   (e.g.) a zip file because the container's size is fixed when it is
   created.

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: Public Domain

   You can find MUCH more information about this library on its
   home page:

   http://fossil.wanderinghorse.net/repos/whefs/

*/

/** @page whefs_naming_conventions whefs API Naming conventions

The whefs API uses the following API naming conventions:

Functions and types all begin with the prefix "whefs_". Function
are always (or nearly always) declared in the form:

whefs_NOUN_VERB( NOUN theObject, ... )

The NOUN part(s) refer to an object or a member of an object, and the
VERB part is the action (or actions) to perform on the given object.

For example, the function to set an inode's name is
whefs_inode_name_set().  This is arguably less intuitive than the more
conventional-sounding whefs_inode_set_name() or
whefs_set_inode_name(), but this approach was chosen because:

- The ordering is natural if one thinks in an OO
mindset. e.g. inode.name.set(foo).

- When the API docs are generated in sorted order, it's easy to find
all operations for a given noun (object type). e.g. whefs_inode_name_set()
and whefs_inode_name_get() will be grouped together.

- Experience has shown it to be easily and consistently applicable,
and simple to remember because it follows a simple rule. "Was that
foo_set_name() or set_foo_name()?" Just remember that the noun comes
first. When there are multiple nouns (e.g. 'foo' and 'name'), they are
in descending order based on their parentage/ownership/API
significance.

The public whefs API is all declared in whefs.h, but the library
internally uses several headers which declare the private API -
functions and types which should never be used by client code. These
include all functions or types with the following prefixes:

- whefs_inode
- whefs_block
- whefs_fs, UNLESS it is declared in whefs.h (in which case it is public)

The internal API is subject to change at any time, and should never
be relied upon as a stable interface.

whefs is based off of another library, whio, and some of those details
"leak through" the whefs abstraction and are visible in the public
whefs API. While whio can be considered to be a private implementation
detail, it is nonetheless fully open to public use because it is an
independent API. whefs' usage of whio is compatible with the whio
public API and conventions, and there is no problem in mixing the APIs
in client code. For example, it is possible to use whio routines which
are not part of the whefs API on whefs-supplied i/o devices (e.g. the
whio gzip compression routines can be used on whefs-supplied i/o
devices).

*/
#ifdef __cplusplus
extern "C" {
#endif

/** @page page_whefs_file_format whefs container file format

The file format is described briefly (and informally) below...

(Begin file format...)

[CORE_MAGIC_BYTES] 4 integer values which must match the magic bytes expected
by a particular version. The 4th value of these bytes tells us whether we
use 16- or 32-bit inode and block IDs. Libraries compiled with a different
ID bit size will not be compatible.

[FILE_SIZE] 1 integer value. This provides a good sanity check when
opening an existing vfs.

[CLIENT_MAGIC_LENGTH] length (in bytes) of the following sequence...

[CLIENT_MAGIC_BYTES] "magic cookie" for this vfs. Determined by the client.

FS OPTIONS:
    - [BLOCK_SIZE] byte size of each block
    - [BLOCK_COUNT] number of blocks
    - [INODE_COUNT] number of "inodes" (filesystem entries)
    - [FILE_NAME_LENGTH]

[INODE_NAMES_TABLE]

Each inode name is composed of:

    - [TAG_BYTE] consistency-checking byte
    - [ID] stored only for consistency/error checking
    - [HASH] a hash code which will eventually be used to speed up lookups. The
    hash routine used is unspecified and may change over time.
    - [STRING_SIZE] number of used bytes encoded as a uint16_t.
    - The string data. Encoding is left to the client.

[INODE_LIST]

The inode list, 1 node per entry, starting with ID 1. ID 0 is
reserved for invalid/uninitialized inodes. ID 1 is reserved for a root
directory entry.

Each inode is stored as:

    - [TAG_BYTE] a consistency-checking byte
    - [ID]
    - [FIRST_BLOCK_ID]
    - [FLAGS]
    - [MODIFICATION_TIME]
    - [DATA_SIZE] the size of the associated pseudofile

[DATA BLOCKS] fixed-length blocks. Each block is stored as:

   - [TAG_BYTE] a sanity-checking byte.

   - [BLOCK_ID] not strictly necessary (we can calculate it), but it may
   be useful for error checking/correction. Starts at 1, not 0, as ID 0
   is reserved for invalid blocks.

   - [FLAGS] internal flags

   - [NEXT_BLOCK_ID] id of the next data block. Updated when a write()
   overflows a block or a file shrinks (thereby dropping blocks).

   - [BLOCK OF BYTES], a block for the data bytes of a pseudofile.
   The size is a property of the vfs object.

[EOF]

(...end file format)

For portability, numbers are stored in a platform-independent
manner. They are stored as char arrays, with a tag byte as the first
byte and the rest of the number stored in the following N bytes in
big-endian format, where N is the number of bytes in the numeric type
(e.g. uint32_t is stored as (1+4)=5 bytes and uint8_t is stored as two
bytes). The tag byte is used for data consistency checking, and each
encoded type has its own tag byte identifier which, by convention,
has a value greater than 127 (to help avoid false hits on ASCII input).

Reminder to self: storing the numbers little-ending might actually
make more sense because then we could use the decoding routines for
smaller data types on encoded data from larger data without having to
do any byte skipping or extra range checking. This is similar to the reason
which little-endian machines have fewer problems porting up to higher
bitness than big-endian machines have.


Limitations:

- We are stuck with 32 bit support only for now. i do not have a 64-bit
machine to play on. Also, we are currently using fseek() in some code,
and that is restricted to signed long values, so we have got a 2GB
container size limitation.

- File names are internally (unsigned char const *). Encoding/code
page is up to the user.

*/


/** @struct whefs_rc_t
   Functions in the api which return an int almost always return a
   value from the whefs_rc object. All members of this type must have
   a non-zero value, except for the OK member, which must be 0. The
   values and signedness for all error values is unspecified except
   for SizeTError, which is -1.
*/
typedef struct whefs_rc_t
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
       Error parsing vfs magic bytes.
    */
    int BadMagicError;

    /**
       An internal error in the API.
    */
    int InternalError;

    /**
       An out-of-range error. e.g. wrong size or value.
    */
    int RangeError;

    /**
       Signals that a vfs is filled, either it has run out
       of inodes or free blocks.
    */
    int FSFull;

    /**
       The requested resource could not be accessed, or write
       permissions are required but denied.
    */
    int AccessError;

    /**
       The data in the vfs appears to be corrupted (or a bug
       in a read routine makes the data look corrupted even
       though it is not).
    */
    int ConsistencyError;

    /**
       Indicates that the called routine is not yet implemented.
    */
    int NYIError;

    /**
       Indicates that some unsupported operation was requested.
    */
    int UnsupportedError;
    /**
       This is equivalent to (size_t)-1, and is used by routines which
       need an error value for a size_t object.
    */
    size_t SizeTError;
} whefs_rc_t;
/**
   A shared instance of whefs_rc_t which contains the "official"
   values of the common error codes for the whefs API.
*/
extern const whefs_rc_t whefs_rc;



/**
   Each vfs is tagged with a "magic cookie". This cookie is normally
   a descriptive string, such as "my vfs version 0.0.1", but may be
   arbitrary bytes.

   The value of having a custom magic cookie is arguable. i cannot
   personally see a real need for it, but it's easy enough to
   support and may be useful later. Theoretically, client apps
   could use the cookie space to store app-specific data, within
   the limitation that the cookie must have a constant number of
   bytes.
*/
typedef struct whefs_magic
{
    /**
       The length of this object's data member. The length
       must be greater than 0, and is conventionally quite
       small (e.g. 10-20 bytes).
    */
    uint16_t length;
    /**
       Must point to at least length bytes of data. The first
       length bytes are used as a magic cookie header.
    */
    char const * data;
} whefs_magic;

/**
   The default magic cookie used by whefs.
*/
extern const whefs_magic whefs_magic_default;


/** @struct whefs_fs_options

   whefs_fs_options defines the major parameters of a vfs. Once they
   are set, the must not be changed for the life of the vfs, as doing
   so invalidates the vfs and will lead to data corruption. (The public
   API provides no way for the client to change these options after
   a VFS is initialized.)

   Normally this type is used by clients only when creating a new vfs
   container file (e.g. via whefs_mkfs()). When opening an existing
   vfs container, the options will be read in from the container.
*/
struct whefs_fs_options
{
    /**
       The magic cookie of the vfs.
    */
    whefs_magic magic;
    /**
       The size of each data block in the vfs.
    */
    uint32_t block_size;
    /**
       The number of blocks in the VFS. The implementation
       requires that this number be at least as large as
       inode_count. Future versions may allow this value to
       grow during the operation of the VFS.
    */
    whefs_id_type block_count;
    /**
       Number of "inodes" in the vfs. That is, the maximum number
       of files or directories. Each non-0-byte file/directory also
       takes up at least one data block.
    */
    whefs_id_type inode_count;
    /**
       Must be greater than 0 and less than or equal to
       WHEFS_MAX_FILENAME_LENGTH.
    */
    uint16_t filename_length;
};
typedef struct whefs_fs_options whefs_fs_options;

/**
   Initializer macro for inlined whefs_magic objects. Sets the magic
   to the given string.
*/
#define WHEFS_MAGIC_INIT(STR) { sizeof(STR) -1, STR }

/**
   The default magic cookie used by the library.
*/
#define WHEFS_MAGIC_DEFAULT WHEFS_MAGIC_INIT(WHEFS_MAGIC_STRING)

/**
   A static initializer for an empty whefs_magic object.
*/
#define WHEFS_MAGIC_NIL { 0, "\0" }

/**
   A static initializer for whefs_fs_options objects. It defaults
   the block_count to INODE_COUNT, which means to use the same value as
   inode_count.
*/
#define WHEFS_FS_OPTIONS_INIT(BLOCK_SIZE,INODE_COUNT,FN_LEN) \
    { WHEFS_MAGIC_DEFAULT, BLOCK_SIZE, INODE_COUNT, INODE_COUNT, FN_LEN }
/**
   Static initializer for whefs_fs_options object, using
   some rather arbitrary defaults.
*/
#define WHEFS_FS_OPTIONS_DEFAULT { \
    WHEFS_MAGIC_DEFAULT, \
    1024 * 8, /* block_size */ \
    128, /* block_count */ \
    128, /* node_count */ \
    64 /* filename_length */ \
    }
/**
   Static initializer for whefs_fs_options object, with
   all values set to 0.
*/
#define WHEFS_FS_OPTIONS_NIL { \
    WHEFS_MAGIC_NIL, \
    0, /* block_size */ \
    0, /* block_count */ \
    0, /* node_count */ \
    0 /* filename_length */ \
    }

/**
   A default configuration for whefs_fs_options. Can be used as a
   starting point for custom options.
*/
extern const whefs_fs_options whefs_fs_options_default;

/**
   A default configuration for whefs_fs_options with all values
   set to 0.
*/
extern const whefs_fs_options whefs_fs_options_nil;

/** @struct whefs_fs

   An opaque handle type for a virtual filesystem. This is the master
   handle to a vfs container.

   These objects are created using whefs_mkfs() or whefs_openfs().
   They are destroyed using whefs_fs_finalize().

   It is illegal to use any given whefs_fs object from multiple
   threads, whether concurrently or not.
*/
struct whefs_fs;
typedef struct whefs_fs whefs_fs;

/** @struct whefs_file

   whefs_file is an opaque handle type to a pseudo-file within
   a vfs container. They are created using whefs_fopen() and
   destroyed using whefs_fclose().

   Example:

   @code
   whefs_file * f = whefs_fopen( myFS, "my.file", "r+" );
   whefs_fwritef( f, "Hi, %s!", "world" );
   whefs_fclose( f );
   @endcode

*/
struct whefs_file;
typedef struct whefs_file whefs_file;


/**
   Opens a "pseudofile" within the given filesystem.

   mode is similar to the fopen() mode parameter, only the following
   are currently recognized:

   - "r" = read-only mode

   - "r+" = read/write mode, positioned at the start of the file.
   Currently, 'r+' will cause the file to be created if it does
   not exist, but this behaviour may change to more closely match
   that of fopen() (where 'w+' takes that role).

   Any other characters are currently ignored. There are no plans
   to support the various "apppend" options.

   To avoid potential "misinteractions", the VFS will not allow any
   given file to be opened in write mode more than once at a
   time. That is, once a file is opened in write mode, all other
   attempts to open it in write mode will fail. Multiple readers are
   allowed.

   On success, a new file handle is returned. On error, 0 is returned,
   but to discover the nature of the problem you'll have to use a
   debugger. (Hint: it's likely that the requested file wasn't found,
   could not be created (e.g. VFS full), is already opened for write
   mode, or any of 37 other potential errors. Just pick one.)

   Potential errors are:

   - allocation error while setting up the internal data.
   - (mode=="r") and no such entry is found
   - (mode=="r+"), the file is not found, and no free inode could be
   found (FS is full), or the file is already opened in write mode.
   - A general i/o error
   - The filesystem is opened read-only and read/write access was requested.

   Design notes:

   The main purpose of whefs_file is to provide an API which is close
   enough to the C FILE API that we can easily swap them out. That
   said, whefs_file is basically a very thin wrapper around a whio_dev
   specialization which is designed to work with whefs_fs. See whefs_fdev()
   for more information on that.

   Bugs:

   This routine is exceedingly inefficient - the lack of inode name
   caching means it must do an O(N) search, where N =
   whefs_fs_options_get(fs)->inode_count.

   @see whefs_fdev()
*/
whefs_file * whefs_fopen( whefs_fs * fs, char const * name, char const * mode );

/**
   Similar to whefs_fopen(), but returns a whio_dev instead of a
   whefs_file.  On success the caller owns the returned object, which
   must be destroyed by calling dev->finalize(dev). The device will be
   positioned at the beginning of the file.

   The returned object has the same overall behaviours as a whefs_file,
   and can be used in place of a whefs_file wherever it is convenient
   to do so (personally i prefer the whio_dev interface).

   If the filesystem is opened read-only then this operation will fail
   if writeMode is true.

   Peculiarities of the returned whio_dev object include:

   - write() updates the mtime and size of the associated inode, but
   does not flush that info to disk until flush() is called on the i/o
   device. Closing the device will automatically flush it. A read-only
   device will never update the on-disk state of the inode and shares
   a copy of the inode with all other files/devices opened for that
   inode.

   - write()ing past EOF will allocate new VFS data blocks, if
   needed. If such an operation fails because it fills the filesystem,
   the device may need to be manually truncated to free up VFS space.
   This could convievably be considered to be a bug.

   @see whefs_fdev()
   @see whefs_fopen()
   @see whefs_dev_close()
*/
whio_dev * whefs_dev_open( whefs_fs * fs, char const * name, bool writeMode );

/**
   Returns the i/o device associated with f, or 0 if !f. 

   The returned object is owned by f and will be destroyed when f is
   closed.

   Each whefs_file has an associated i/o device to abstract away the
   interaction with the underlying whefs_fs object. This is a
   "pseudodevice" which indirectly uses the underlying storage of the
   vfs but has its own storage range (internal to the vfs) and is
   designed to not read or write outside of the bounds of that
   range. In general, reading and writing and such are simpler (IMO)
   via the whio_dev interface, but the whefs_fXXXX() API will be more
   familiar to long-time C users.

   @see whefs_dev_open()
   @see whefs_fopen().
*/
whio_dev * whefs_fdev( whefs_file * f );


/**
   Returns the file's current cursor position, or whefs_rc.SizeTError
   if !f or the combination of pos and whence is not legal. See
   whio_dev::seek() for the full documentation and possible error
   conditions.

   Note that it is legal to seek past EOF, but the size of the file
   will not be changed unless a write actually happens past EOF.
*/
size_t whefs_fseek( whefs_file * f, size_t pos, int whence );

/**
   "Rewinds" the file pointer back to its starting position.
   Returns whefs_rc.OK on success, else some other value.
*/
int whefs_frewind( whefs_file * f );

/**
   Returns the current size of the given file, or whefs_rc.SizeTError
   if !f or on an internal error (which "shouldn't happen").

   It is actually more efficient (faster) to use
   whio_dev_size(whefs_fdev(f)), but this implementation is different
   in that it respects the constness of f.
*/
size_t whefs_fsize( whefs_file const * f );

/**
   Truncates the given pseudofile to the given length.

   On success, whefs_rc.OK is returned, otherwise some other value is
   returned. Read-only files cannot be truncated and will result in
   whio_rc.AccessError being returned.

   If the file grows then all bytes between the file's previous EOF
   (not it's current position) and the new EOF are zeroed out.

   If the file shrinks then all data after the new EOF, but which
   previously belonged to the file, are also zeroed out.
*/
int whefs_ftrunc( whefs_file * f, size_t newLen );

/**
   Closes f, freeing its resources. After calling this, f is an invalid object.
   Returns whefs_rc.OK on success, or whefs_rc.ArgError if (!f).
 */
int whefs_fclose( whefs_file * f );

/**
   Similar to whefs_fclose() but closes a device opened with whefs_dev_open().

   DO NOT pass this function a device which was fetched using
   whefs_fdev()! Use whefs_fclose() to close the file instead. Failing to
   follow this rule may lead to undefined results when the associated file
   handle dangles around and a double-delete if the file closed afterwards.

   ONLY use this for devices opened with whefs_dev_open().

   For example:

   @code
   // The proper way:
   whio_dev * dev1 = whefs_dev_open(...);
   ...
   whefs_dev_close( dev1 ); // Correct!

   // And when using whefs_file handles:
   whefs_file * f = whefs_fopen(...);
   whio_dev * dev2 = whefs_fdev(f);
   ...
   //whefs_dev_close(dev2); // WRONG!!! This will lead to problems! Instead do:
   whefs_fclose( f ); // CORRECT! dev2 is owned by f and is cleaned up by this.
   @endcode

   @see whefs_dev_open()
*/
int whefs_dev_close( whio_dev * dev );

/**
   Unlinks (deletes) the given file and its associated inode. It there
   are open handles to the file the deletion will fail.

   For data security/integrity reasons, this also zeroes out all data
   on all blocks owned by the inode, which makes unlinking an O(N)
   operation, with N being a function of the block count and their
   size.

   On success whefs_rc.OK is returned. If the file is opened then
   whefs_rc.AccessError is returned. Any other error would be i/o
   related or caused by inconsistent internal VFS state.

   Bugs:

   The choice to not allow deletion when there are open handles is to
   avoid some potential errors. We should (maybe) simply defer the
   unlink until all handles are closed. The problem with that is if
   the handles are not properly closed (e.g. due to a crash or the
   client doesn't close them for some reason), the deferred unlink
   won't ever happen.
*/
int whefs_unlink_filename( whefs_fs * fs, char const * fname );

/**
   Equivalent the fwrite() C function, but works on whefs_file handles.

   Writes n objects from src, each of bsize bytes, to f. Returns the number
   of successfully written, which will e less than n on error.

   When writing a single block of memory (e.g. a string), it is
   significantly more efficient to pass the data length as the bsize
   parameter and n to 1 than it is to set n to the data length and
   bsize to 1. The end effect is the same, but the overhead of the
   latter is much higher than the former.

   @see whefs_fread()
   @see whefs_file_write()
   @see whefs_file_read()
*/
size_t whefs_fwrite( whefs_file * f, size_t bsize, size_t n, void const * src );

/**
   Writes a printf-style formatted string to f. Returs the number of
   bytes written. It is, in general case, impossible to know if all
   data was written correctly.
*/
size_t whefs_fwritev( whefs_file * f, char const * fmt, va_list vargs );

/**
   Equivalent to whefs_fwritev() except that it takes elipsis arguments
   instead of a va_list.
 */
size_t whefs_fwritef( whefs_file * f, char const * fmt, ... );

/**
   Reads n objects, each of bsize bytes, from f and writes them to dest.
   Returns the number of objects successfully written, which will be
   less than n on error.

   @see whefs_fwrite()
   @see whefs_file_write()
   @see whefs_file_read()
*/
size_t whefs_fread( whefs_file * f, size_t bsize, size_t n, void * dest );

/**
   Similar to whefs_fwrite(), but takes arguments in the same
   convention as the write() system call (which incidentally is the
   same convention used by whio_dev). Returns the number of bytes
   written, which will be less than n on error.

   @see whefs_fwrite()
   @see whefs_file_read()
*/
size_t whefs_file_write( whefs_file * f, void const * src, size_t n  );

/**
   Similar to whefs_fread(), but takes arguments in the same convention
   as the read() system call (which incidentally is the same convention
   used by whio_dev). Returns the number of bytes read, which will be
   less than n on error.

   @see whefs_file_write()
   @see whefs_fread()
*/
size_t whefs_file_read( whefs_file * f, void * dest, size_t n  );

/**
   Returns the vfs associated with the given file, or 0 if !f.
*/
whefs_fs * whefs_file_fs( whefs_file * f );

/**
   Flushes the underlying i/o device. Flushing a whefs_file will also
   flush the VFS, so calling this is normally not needed.

   Returns whefs_rc.ArgError if !fs or fs has no backing store, else
   the result is the same as calling whio_dev::flush() on the
   underlying i/o device.

   If/when the VFS caches any information (e.g. to synchronize inode
   use across multiple open handles) then this function will also
   write that cache out to disk.
*/
int whefs_fs_flush( whefs_fs * fs );

/**
   Creates a new vfs using the given filename and options. The resulting
   vfs handle is assigned to tgt.

   The contents of filename will be mercilessly overwritten. As a
   special case, the filename ":memory:" (case-sensitive) will instead
   cause an in-memory buffer to be allocated. In that case, we must be
   able to allocate whefs_fs_calculate_size() bytes for the backing
   storage (plus memory for the vfs-related objects) or this routine
   will fail.

   On success, whefs_rc.OK is returned and tgt is assigned to the new
   whefs_fs object, which the caller must eventually destroy by calling
   whefs_fs_finalize().

   The new vfs object uses a copy of the opt object, so the lifetime
   of the opt object is not significant, nor will changes to that
   object affect the vfs.

   On error, tgt is not modified and one of these whefs_rc error codes
   is returned:

   - ArgError = either filename, opt, or tgt are 0.

   - RangeError = the values in the opt object are out of range. See
   below.

   - AllocError = an allocation error occured (out of memory).

   - AccessError = the specifies file could not be opened for writing.

   - IOError = an i/o error happened while writing the vfs. The
   resulting file is almost certainly corrupt (or incomplete) and
   should be deleted (this code does not delete it, however).

   A RangeError signifies that some value in the opt object is out of range.
   The currently allowed ranges are:

   - block_size >= 32 (which was arbitrarily chosen). Except when
   storing only very small data, a block size of 4k-32k is more
   realistic. Very small block sizes are probably only useful for
   testing the file-pos-to-data-block mapping in the i/o code.

   - inode_count >= 2, as 1 inode is reserved for the root directory, and
   a filesystem without at least 1 free inode is effectively full.

   - block_count must be at least as big as inode_count, because inodes
   which can never be mapped to a block are fairly useless (they can only
   be used as 0-byte files).

   All other values must be non-0.

   It is possible to calculate the size of a vfs container before creating
   it by calling whefs_fs_calculate_size() and passing it the whefs_fs_options
   object which you intend to pass to whefs_mkfs().
*/
int whefs_mkfs( char const * filename, whefs_fs_options const * opt, whefs_fs ** );

/**
   Equivalent to whefs_mkfs() but takes an i/o device as the
   target. The device must be writable and must be able to grow to the
   required size. The existing contents will be destroyed.

   If takeDev is true then on success ownership of dev is passed to
   the object assigned to to tgt, and dev will be destroyed when tgt
   is destroyed. On failure, ownership of dev does not change and tgt
   is not changed.
*/
int whefs_mkfs_dev( whio_dev * dev, whefs_fs_options const * opt, whefs_fs ** tgt, bool takeDev );

/**
   Opens an existing vfs container file.

   On success, whefs_rc.OK is returned and tgt is assigned to the new
   whefs_fs object, which the caller must eventually destroy by calling
   whefs_fs_finalize().

   If writeMode is false then the underlying file is opened read-only.
*/
int whefs_openfs( char const * filename, whefs_fs ** tgt, bool writeMode );

/**
   Similar to whefs_openfs(), but works on an existing i/o device. The
   device must contain a whefs filesystem.

   On success, whefs_rc.OK will be returned and tgt is assigned to the
   newly-created whefs_fs object. Ownership of tgt is then transfered to
   the caller, who must eventually finalize it by passing it to
   whefs_fs_finalize().

   On error, non-whefs_rc.OK wil be returned, tgt will not be
   modified, and ownership of dev does not change.

   If takeDev is true then on success ownership of dev is transfered to
   tgt, such that dev will be finalized when tgt is destroyed.

   Error conditions:

   - !dev or !tgt

   - dev does not contain a readable whefs.

   If dev shares storage with any other objects (be they whio_dev
   objects or otherwise), in particular if more than one have write
   access, the filesystem will in all likelyhood be quickly corrupted.
*/
int whefs_openfs_dev( whio_dev * dev, whefs_fs ** tgt, bool takeDev );

/**
   Cleans up all resources associated with fs. After calling this,
   fs will be an invalid pointer.
*/
void whefs_fs_finalize( whefs_fs * fs );

/**
   Returns the options associated with fs, or 0 if !fs.
*/
whefs_fs_options const * whefs_fs_options_get( whefs_fs const * fs );

/**
   A shorter name for whefs_fs_options_get().
*/
whefs_fs_options const * whefs_fs_opt( whefs_fs const * fs );

/**
   A debug-only function for showing some information about a
   vfs. It's only for my debugging use, not client-side use.
*/
void whefs_fs_dump_info( whefs_fs const * fs, FILE * out );

/**
   Calculates the size of a vfs container based on the given
   options. This does not account for any internal bookkeeping memory,
   just the size of the container image. This can be used, e.g., to
   allocate a buffer for an in-memory whio_dev implementation, or to
   ensure that enough filesystem space (or memory) is available.

   The size of a container is based on:

   - the number of inodes
   - the maximum length of inode names
   - the size of each data block
   - the size of the magic cookie
   - a few internal bookkeeping and consistency checking details

   Once a container is created its size must stay constant. If it is
   changed it will effectively corrupt the vfs.

   If !fs then 0 is returned (0 is never a valid size of a vfs).
*/
size_t whefs_fs_calculate_size( whefs_fs_options const * opt );

/**
   A debuggering routine which dumps fs to the given FILE. This is
   mainly intended for dumping in-memory vfs containers to a file for
   examination or persistency purposes.

   On success it returns whefs_rc.OK. On error may return:

   whefs_rc.ArgError = !fs or !outstr, or fs is not attached to a device.

   whefs_rc.IOError = copying to outstr failed.

   Ownership of outstr is not changed, and this routine does not close
   the stream.
*/
int whefs_fs_dump_to_FILE( whefs_fs * fs, FILE * outstr );

/**
   Equivalent to whefs_fs_dump_to_FILE() except that it takes a file name
   instead of a FILE handle. In addition to the return codes from that
   routine, it may return whefs_rc.AccessError if it cannot open the
   file for writing.
*/
int whefs_fs_dump_to_filename( whefs_fs * fs, char const * filename );


/**
   Test/debug routine which inserts some entries into the inode table.
*/
int whefs_test_insert_dummy_files( whefs_fs * fs );


/**
   A type for reporting certain whefs_file statistics.

   @see whefs_fstat().
*/
typedef struct whefs_file_stats
{
    /**
       Size of the file, in bytes.
    */
    size_t bytes;

    /**
       inode number.
    */
    size_t inode;

    /**
       Number of blocks used by the file.
    */
    size_t blocks;
} whefs_file_stats;


/**
   Populates st (which may not be null) with some statistics for f
   (which also may not be null but may be read-only).

   Returns whefs_rc.OK on success. On error some other value
   is returned and st's contents are in an undefined state.

   Example usage:

   @code
   whefs_file_stats st;
   if( whefs_rc.OK == whefs_fstate( myFile, &st ) ) {
       ... use st ...
   }
   @endcode
*/
int whefs_fstat( whefs_file const * f, whefs_file_stats * st );

/**
   Renames the given file, which must be opened in read/write mode, to
   the new name. On success, whefs_rc.OK is returned, else some other value
   is returned.

   Errors include:

   - whefs_rc.ArgError = either f or newName are null or newName is
   empty.

   - whefs_rc.AccessError = f is not opened in write mode.

   - whefs_rc.RangeError = newName is longer than the smaller of
   whefs_fs_options_get(whefs_file_fs(f))->filename_length or
   WHEFS_MAX_FILENAME_LENGTH.

   - Some other error = something weird happened.
*/
int whefs_file_set_name( whefs_file * f, char const * newName );

/**
   Returns the given file's current name, or 0 if !f or on some weird
   internal error. The memory for the string is owned by the vfs
   internals and will be deallocated when the last handle to the file
   is closed (or possibly before then, if the internals are
   reallocated for some reason). The caller is advised to copy it if
   there is any question at all about its lifetime.

   The returned string is guaranteed to be no longer than
   whefs_fs_options_get(whefs_file_fs(f))->filename_length
   characters (not including the trailing null).
*/
char const * whefs_file_get_name( whefs_file const * f );

/**
   A type for reporting certain vfs metrics.
*/
typedef struct whefs_fs_stats
{
    /**
       Size of the vfs container.
    */
    size_t size;
    /**
       Number of uses inodes.
    */
    size_t used_inodes;
    /**
       Number of used blocks.
    */
    size_t used_blocks;
    /**
       Number of used bytes (not necessarily whole blocks).
    */
    size_t used_bytes;
} whefs_fs_stats;

/**
   Not yet implemented.

   Calculates some statistics for fs and returns them via
   the st parameter.

   Returns whefs_rc.OK on success. On error some other value
   is returned and st's contents are in an undefined state.
*/
int whefs_fs_stats_get( whefs_fs * fs, whefs_fs_stats * st );

/**
   Returns a static string pointing to the whefs home page.
*/
char const * whefs_home_page_url();

/**
   Imports all contents from src into the VFS pseudofile named fname.
   If overwrite is true, any existing entry will be overwritten,
   otherwise an existing file with that name will cause this function
   to return whefs_rc.AccessError.

   Ownership of src is not changed. This routine will re-set src's
   cursor to its pre-call position before returning.

   On success, whefs_rc.OK is returned.

   On error, any number of different codes might be returned, both
   from whefs_rc and whio_rc (some of which overlap or conflict).
   If the import fails and the entry did not exist before the import
   started, it is removed from the filesystem. If it did exist then
   exactly what happens next depends on a few factors:

   - If the import failed because the existing file could not be
   expanded to its new size then it is kept intact, with its old
   size. This step happens first, before any writing is done.

   - If the import failed during the copy process then the destination
   file is left in an undefined state and it should probably be
   unlinked.

*/
int whefs_import( whefs_fs * fs, whio_dev * src, char const * fname, bool overwrite );

/**
   Returns a static string containing the file format version
   supported by this version of the library.
*/
char const * whefs_data_format_version_string();

/**
   Sets the library's debugging stream (which may be 0). This only works if
   the lib is compiled with the debugging support enabled.
*/
void whefs_setup_debug( FILE * ostream );

/** @typedef uint16_t whefs_string_size_t
   
whefs_string_size_t represents the maximum length of strings in
whefs. In practice 8 bits is probably enough, but until whefs gets
support for directories, longer strings may sometimes be needed.
*/
typedef uint16_t whefs_string_size_t;

/** @struct whefs_string
   whefs_string is a type for storing strings. Routines in the whefs
   library which perform string operations may use this type, as
   opposed to a simple string, so because it allows us to re-use the
   string's bytes at times instead of using malloc for every string.

   One way to free the memory owned by these objects is
   whefs_string_clear(). Alternately, one may pass the string member
   to free(), then set the length and alloced members to 0. If the object
   is part of a chain, the caller must free each object in the chain.
*/
struct whefs_string
{
    /**
       The string data for this object. The memory must be freed
       by whomever owns this object.
     */
    char * string;
    /**
       The length of the string member.
    */
    whefs_string_size_t length;
    /**
       Number of bytes allocated for the string member.
    */
    whefs_string_size_t alloced;
    /**
       Pointer to the next string in the list. Used by
       routines which return multiple strings.
    */
    struct whefs_string * next;
};
typedef struct whefs_string whefs_string;

/**
   Empty whefs_string initialization macro for places where static
   initialization is required (e.g. member whefs_string objects).
*/
#define whefs_string_init_m { 0, 0, 0, 0 }

/**
   Empty initialization object.
*/
extern const whefs_string whefs_string_init;

/**
   Copies the null-terminated string str into tgt. Neither src nor tgt
   may be null, but src may be a length-zero string.

   If tgt->alloced is greater than str's length then tgt's allocated
   memory is re-used for the copy, otherwise tgt->allocated is adjusted
   to fit using realloc().

   On success, whefs_rc.OK is returned and tgt is modified. On error,
   some other value is returned and tgt is not modified. tgt->next is
   never modified by this routine.

   Example usage:

   @code
   whefs_string str = whefs_string_init; // Important! See below!
   whefs_string_copy_cstring( "Hi, world!!", &str );
   ...
   whefs_string_copy_cstring( "Bye, world!", &str );
   // ^^^^ Re-uses str->string's memory
   ...
   whefs_string_clear( &str, false );
   @endcode

   BE CAREFUL not to pass a pointer to an uninitialized whefs_string
   objects. If the members of the object have uninitialized values
   this function may attempt to malloc() a huge amount of memory, use
   an uninitialized string, or otherwise misbehave (been there, done
   that!). Use the whefs_string_init object to initialize new
   whefs_string objects, as shown in the example above.

*/
int whefs_string_copy_cstring( whefs_string * tgt, char const * str );

/**
   Clears tgt's contents, freeing up any used memory, but does not
   destroy tgt.

   If clearChain is true then tgt->next is recursively cleared as
   well.
*/
int whefs_string_clear( whefs_string * tgt, bool clearChain );

/**
   Similar to whefs_string_clear(), but also frees tgt. If clearChain
   is true then tgt->next is recursively finalized as well.

   Passing a tgt of null will result in a harmless non-OK return
   value.

   NEVER pass this an object which was allocated on the stack,
   as free() (or some equivalent) will be called on it.

*/
int whefs_string_finalize( whefs_string * tgt, bool clearChain );

/**
   Like the 'ls' Unix program, this function fetches a list of
   filenames matching the given pattern. If no matches are found, null
   is returned, otherwise a string list is returned and count (if it
   is not null) is set to the number of items in the list.

   A pattern of null or an empty string is equivalent to a pattern of
   "*" (but much faster, since it doesn't need to be compared).

   The returned list must be freed by calling whefs_string_finalize()
   and passing true as the second parameter.

   Example usage:

   @code
    whefs_string * ls = whefs_ls( myFS, "*.c", 0 );
    whefs_string * head = ls;
    while( ls )
    {
        printf("%s\n", ls->string );
        ls = ls->next;
    }
    whefs_string_finalize( head, true );
   @endcode

   BUG:

   This routine has no way of reporting an error. If some sort of I/O
   error happens after an entry has been fetched, the entries matched
   so far are returned but there is no way of knowing how many entries
   would have been returned if all could have been read. That said, if
   the first entry is read successfully, it is "reasonable to assume"
   that the remaining entries could be read as well. In fact, since
   only "used" entries are considered for matching, any entries
   returned here must have been read previously (to build the
   used-items cache), and only a true I/O error (or corruption of the
   VFS container) should cause a failure here.
*/
whefs_string * whefs_ls( whefs_fs * fs, char const * pattern, whefs_id_type * count );

/**
   Returns an array of numbers which represent this library's file
   format version number. The bytes are conventionally { YYYY, MM, DD,
   WHEFS_ID_TYPE_BITS }, where YYYY, MM, and DD are the date of the
   format change (and WHEFS_ID_TYPE_BITS is documented elsewhere).

   The returned result is guaranteed to not be null and to have a value
   of 0 as the end-of-array entry.
*/
const uint32_t * whefs_get_core_magic();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WANDERINGHORSE_NET_WHEFS_H_INCLUDED */
/* begin file whefs_encode.h */
#if !defined(WANDERINGHORSE_NET_WHEFS_ENCODE_H_INCLUDED)
#define WANDERINGHORSE_NET_WHEFS_ENCODE_H_INCLUDED 1
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/

  License: Public Domain
*/

#include <stdint.h>
#include <stddef.h> /* size_t on my box */
/**
   This file contains the whefs_inode parts of the whefs
   private/internal API.
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
   This enum defines some on-disk sizes for the utility routines
   which encode/decode data to/from whio_dev objects.
*/
enum whefs_sizeof_encoded {

/** @var whefs_sizeof_encoded_uint64

   whefs_sizeof_encoded_uint64 is the length (in bytes) of an encoded uint64 value.
   It is 9: 1 tag byte + 8 data bytes.

   @see whefs_uint64_decode()
   @see whefs_uint64_encode()
*/
whefs_sizeof_encoded_uint64 = 9,
/** @var whefs_sizeof_encoded_uint32

   whefs_sizeof_encoded_uint32 is the length (in bytes) of an encoded uint32 value.
   It is 5: 1 tag byte + 4 data bytes.

   @see whefs_uint32_decode()
   @see whefs_uint32_encode()
*/
whefs_sizeof_encoded_uint32 = 5,

/** @var whefs_sizeof_encoded_uint16

   whefs_sizeof_encoded_uint16 is the length (in bytes) of an encoded uint16 value.
   It is 3: 1 tag byte + 2 data bytes.

   @see whefs_uint16_decode()
   @see whefs_uint16_encode()
*/
whefs_sizeof_encoded_uint16 = 3,

/** @var whefs_size_cstring

   whefs_size_cstring is the encoded length of a C-style string,
   NOT INCLUDING the actual string bytes. i.e. this is the header
   size.

   @see whefs_cstring_decode()
   @see whefs_cstring_encode()
*/
whefs_sizeof_encoded_cstring = 1 + whefs_sizeof_encoded_uint32,

/**
   The on-disk size of an inode record, not including the
   inode name.
*/
whefs_sizeof_encoded_inode = 1 /* tag byte */
        + whefs_sizeof_encoded_id_type /* id */
        + whefs_sizeof_encoded_uint32 /* flags */
	+ whefs_sizeof_encoded_uint32 /* mtime */
	+ whefs_sizeof_encoded_uint32 /* data_size */
        + whefs_sizeof_encoded_id_type /* first_block */,

/**
   This is the on-disk size of the HEADER for an inode name. The
   actual length is this number plus the associated
   whefs_fs_options::filename_length.
*/
whefs_sizeof_encoded_inode_name_header = 1 /* tag byte */
        + whefs_sizeof_encoded_id_type /* id */
        + whefs_sizeof_encoded_uint64 /* hash */
        + whefs_sizeof_encoded_uint16 /* length */

};

/**
   Encodes a 32-bit integer value into 5 bytes - a leading tag/check
   byte, then the 4 bytes of the number, in big-endian format. Returns
   the number of bytes written, which will be equal to
   whefs_sizeof_encoded_uint32 on success.

   dest must be valid memory at least whefs_sizeof_encoded_uint32 bytes long.

   @see whefs_uint32_decode()
*/
size_t whefs_uint32_encode( unsigned char * dest, uint32_t i );

/**
   The converse of whefs_uint32_encode(), this tries to read an
   encoded 32-bit value from the given memory. On success it returns
   whefs_rc.OK and sets tgt (if not null) to that value. On error it
   returns some other value from whefs_rc and does not modify tgt.

   src must be valid memory at least whefs_sizeof_encoded_uint32 bytes
   long.

   Error values include:

   - whefs_rc.ArgError = !src

   - whefs_rc.ConsistencyError = the bytes at the current location
   were not encoded with whefs_uint32_encode().

   @see whefs_uint32_encode()

*/
int whefs_uint32_decode( unsigned char const * src, uint32_t * tgt );

/**
   Similar to whefs_uint32_encode(), with the same conventions, but
   works on 16-bit numbers. Returns the number of bytes written, which
   will be equal to whefs_sizeof_encoded_uint16 on success.

   dest must be valid memory at least whefs_sizeof_encoded_uint16
   bytes long.

   @see whefs_uint16_decode()
*/
size_t whefs_uint16_encode( unsigned char * dest, uint16_t i );

/**
   Similar to whefs_uint32_decode(), with the same conventions and
   error codes, but works on 16-bit numbers.  On success it returns
   whefs_rc.OK and sets target to that value. On error it returns some
   other value from whefs_rc and does not modify tgt.

   src must be valid memory at least whefs_sizeof_encoded_uint16 bytes
   long.


   @see whefs_uint16_encode()
*/

int whefs_uint16_decode( unsigned char const * src, uint16_t * tgt );


/**
   The 64-bit variant of whefs_uint32_encode(). Follows the same
   conventions as that function but handles a uint64_t value instead
   of uint32_t.

   @see whefs_uint16_encode()
   whefs_uint32_encode()
   whefs_uint64_decode()
*/
size_t whefs_uint64_encode( unsigned char * dest, uint64_t i );

/**
   The 64-bit variant of whefs_uint32_decode(). Follows the same
   conventions as that function but handles a uint64_t value instead
   of uint32_t.

   @see whefs_uint16_decode()
   whefs_uint32_decode()
   whefs_uint64_encode()
*/
int whefs_uint64_decode( unsigned char const * src, uint64_t * tgt );

/**
   Uses whefs_uint32_encode() to write n elements from the given
   array to dev.  Returns whefs_rc.OK on success. Returns the number of
   items written.

   @see whefs_uint32_encode()
*/
size_t whefs_uint32_array_encode( unsigned char * dest, size_t n, uint32_t const * list );

/**
   Reads n consecutive numbers from src, populating list (which must
   point to at least n uint32_t objects) with the results. Returns the
   number of items read, which will be less than n on error.

   @see whefs_uint32_decode()
*/
size_t whefs_uint32_array_decode( unsigned char const * src, size_t n, uint32_t * list );

/**
   Encodes a C string into the destination by writing a tag byte, the length of
   the string, and then the string bytes. If n is 0 then n is equivalent to
   strlen(s). Zero is also legal string length.

   Returns the number of bytes written, which will be (n +
   whefs_sizeof_encoded_cstring) on success, 0 if !dev or !s.

   dest must be at least (n + whefs_sizeof_encoded_cstring) bytes long,
   and on success exactly that many bytes will be written. The null
   terminator (if any) is not stored and not counted in the length.
   s may contain null characters.

   @see whefs_cstring_decode()
*/
size_t whefs_cstring_encode( unsigned char * dest, char const * s, uint32_t n );

/**
   The converse of whefs_cstring_encode(), this routine tries to
   decode a string from the current location in the device.

   src must contain at least (whefs_sizeof_encoded_cstring + N) bytes,
   where N is the number which is encoded in the first part of the data.
   On success exactly that many bytes will be read from src. The null
   terminator (if any) is not stored and not counted in the length.
   s may contain null characters.



   On success, tgt is assigned to the new (null-terminated) string
   (allocated via calloc()) and length (if it is not null) is set to
   the length of the string (not counting the terminating null). The
   caller must free the string using free(). If the string has a
   length of 0 then tgt is set to 0, not "", and no memory is
   allocated.

   Neither dev nor tgt may be 0, but length may be 0.

   Returns whefs_rc.OK on success.

   On error, neither tgt nor length are modified and some non-OK value
   is returned:

   - whefs_rc.ArgError = dev or tgt are 0.

   - whefs_rc.ConsistencyError = src does not contain a string written
   by whefs_cstring_encode().

   Example:

@code
char * str = 0;
size_t len = 0;
int rc = whefs_cstring_decode( mySource, &str, &len );
if( whio_rc.OK != rc ) ... error ...
... use str ...
free(str);
@endcode


   @see whefs_cstring_encode()
*/
int whefs_cstring_decode( unsigned char const * src, char ** tgt, size_t * length );

/**
   Generates a hash code for the first n bytes of the given memory,
   or 0 if n is 0 or data is null.

   The exact hash routine is unspecified, and may change from version
   to version if a compelling reason to do so is found. The hash code
   is intended to be used in ways that will not cause an
   imcompatibility in whefs file format if the hash implementation
   changes.
 */
uint64_t whefs_bytes_hash( void const * data, uint32_t n );
#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* WANDERINGHORSE_NET_WHEFS_ENCODE_H_INCLUDED */
/* begin file whefs_inode.h */
#if !defined(WANDERINGHORSE_NET_WHEFS_INODE_H_INCLUDED)
#define WANDERINGHORSE_NET_WHEFS_INODE_H_INCLUDED 1
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/

  License: Public Domain
*/

/**
   This file contains the whefs_inode parts of the whefs
   private/internal API.
*/

#ifdef __cplusplus
extern "C" {
#endif


/** @struct whefs_block

whefs_block holds the metadata for the data blocks of a VFS, but do
not actually hold the data.
*/
struct whefs_block
{
    /**
       Sequential id number. Value of 0 is reserved for "invalid block".
    */
    whefs_id_type id;
    /**
       Internal flags.
    */
    uint32_t flags;
    /* id of next block */
    whefs_id_type next_block;
};
typedef struct whefs_block whefs_block;

/** @def whefs_block_init_m

    Empty static initializer for whefs_block objects.
 */
#define whefs_block_init_m  \
    { \
    0, /* id */ \
    0, /* flags */ \
    0 /* next_block */ \
    }

/** Empty initialization object for whefs_block objects. */
extern const whefs_block whefs_block_init;

/** @def WHEFS_INODE_RELATIVES

This is going away - don't use it.

WHEFS_INODE_RELATIVES determines whether the underlying support for
directories/subdirectories is turned on. This changes the filesystem
layout and the in-memory size of inode entries.

*/
#define WHEFS_INODE_RELATIVES 0


typedef struct whefs_block_list
{
    whefs_block * list;
    whefs_id_type alloced;
    whefs_id_type count;
} whefs_block_list;

/**
   Convenience macro for places where a whefs_block_list
   object must be statically initialized.
*/
#define whefs_block_list_init_m {0,0,0}
/**
   Empty initialization object.
*/
extern const whefs_block_list whefs_block_list_init;

/** @struct whefs_inode

For most intents and purposes, a whefs_inode object can be considered
to be a file entry in a VFS.
*/
typedef struct whefs_inode
{
    /**
       Sequential id number. Value of 0 is reserved for "invalid
       inode" and ID 1 is reserved for the root node entry of a VFS.
    */
    whefs_id_type id;

    /**
       Flags from the whefs_flags enum. Persistant.
    */
    uint32_t flags;

    /**
       ID of first block. Persistant.
    */
    whefs_id_type first_block;

    /**
       EOF position (i.e. size of the associated data)
    */
    uint32_t data_size;

    /**
       Timestamp of last write/change to the inode. This type may
       change at some point (to uint64_t). Persistant.
     */
    uint32_t mtime;

    /** Used by the open filehandle tracker. Transient. */
    uint16_t open_count;
    /** Is used to mark write ownership of an inode. Transient. */
    void const * writer;
    /**
       This is used by whefs_block_for_pos() (the heart of the i/o
       routines) to keep the block list for an opened inode in
       memory. This saves boatloads of i/o for common use cases.
       Transient.
    */
    whefs_block_list blocks;
    /** Transient string used only by opened nodes. */
    whefs_string name;
} whefs_inode;

#define whefs_inode_init_m { \
	0, /* id */ \
	WHEFS_FLAG_Unused, /* flags */ \
        0, /* first_block */ \
        0, /* data_size */ \
        0, /* mtime */ \
        0, /* open_count */ \
        0, /* writer */ \
	whefs_block_list_init_m /*blocks */, \
        whefs_string_init_m /* name */ \
    }


//static const whefs_inode whefs_inode_init = whefs_inode_init_m;
/** Empty initialization object. */
extern const whefs_inode whefs_inode_init;

typedef struct whefs_inode_list
{
    whefs_inode inode;
    struct whefs_inode_list * next;
    struct whefs_inode_list * prev;
} whefs_inode_list;
#define whefs_inode_list_init_m { whefs_inode_init_m, 0, 0 }
extern const whefs_inode_list whefs_inode_list_init;
typedef struct whefs_inode_cache
{
    whefs_inode ** list;
    whefs_id_type size;
    whefs_id_type alloced;
} whefs_inode_cache;

#define whefs_inode_cache_init_m { 0, 0, 0 }



/**
   Reads a whefs_inode's metadata from disk. Neither n nor n->id may
   be 0, but the remaining contents of n are irrelevant (they will be
   overwritten). On success, whefs_rc.OK is returned and n is updated
   to the on-disk state, otherwise some other value is returned and n
   is left in an undefined state (that is, possibly partially
   populated).

   Ownership of n is not changed by calling this function.

   Typical usage:

   @code
   whefs_inode ino = whefs_inode_init;
   ino.id = 42;
   whefs_inode_read( fs, &ino );
   @endcode
*/
int whefs_inode_read( whefs_fs * fs, whefs_inode * n );

/**
   Reads the flags field of the given inode and assigns the flags
   argument to their value. fs may not be null and nid must be a valid
   inode id for fs or the routine will fail. flags may be 0, in which
   case this is simply a very elaborate way to tell if an inode is
   valid.

   On success flags is modified (if it is not null) and whefs_rc.OK is
   returned. On failure flags is not modified and non-OK is returned.
   
   This routine returns the fs i/o device to its original position
   when it is done, so calling this behaves "as if" the cursor has not
   moved. The one exception is if the seek to the correct inode fails,
   in which case the cursor position is in an undefined state and
   error recovery must begin (writing at that point may corrupt the
   vfs).

   If the given inode ID is currently opened, the flags are taken
   from the opened copy and no i/o is necessary.
*/
int whefs_inode_read_flags( whefs_fs * fs, whefs_id_type nid, uint32_t * flags );

/**
   Opens an inode for concurrent (but NOT multi-threaded!) access,
   such that the node will be shared by open file and devive handles.

   This should be the only function the remaining API uses to "open
   up" an inode.

   On success whefs_rc.OK is returned tgt is set to a pointer to the
   shared inode. The pointer is owned by fs but must be closed by
   calling whefs_inode_close().

   On error tgt is not modified and some other value is
   returned. Errors can be:
   
   - nodeID is not a valid node.
   - could not allocate space for shared inode.
   - !fs or !tgt

   If the inode is to be opened read-only, pass 0 for the writer argument.
   If the inode is to be opened with write mode enabled, writer must be
   an opaque value which uniquely identifies the writer (e.g. the owning
   object). An attempt to open an inode for a second or subsequent writer
   will fail with whefs_rc.AccessError.

   @see whefs_inode_close()
*/
int whefs_inode_open( whefs_fs * fs, whefs_id_type nodeID, whefs_inode ** tgt, void const * writer );

/**
   The converse of whefs_inode_open(), this unregisters a reference to
   the given inode. src and writer MUST be the same pointers as
   returned from resp. passed to whefs_inode_open() (NOT addresses to
   copied objects) or this operation will fail and client code may end
   up leaking one opened inode handle.

   On success, whefs_rc.OK is returned. The src object was allocated
   by whefs_inode_open() and will be cleaned up here.

   If (writer != 0) and (writer == src->writer) then the inode is
   flushed to disk as part of the closing process.

   @see whefs_inode_open()
*/
int whefs_inode_close( whefs_fs * fs, whefs_inode * src, void const * writer );


/**
   Writes n to disk. If any arguments are 0 then whefs_rc.ArgError
   is returned. If n->id is 0 or more than fs->options.inode_count then
   whefs_rc.RangeError is returned.

   On success, whefs_rc.OK is returned.

   Note that this does not flush the inode's name, as that is handled
   separately. See whefs_inode_name_set().
*/
int whefs_inode_flush( whefs_fs * fs, whefs_inode const * n );

/**
   Searches for the next free inode and assigns tgt to that value. If markUsed
   is true then the inode is also marked as used, otherwise a susequent call
   to this function might return the same inode.

   On success it updates tgt and returns whefs_rc.OK. On error it
   returns some other value and does not update tgt.
*/
int whefs_inode_next_free( whefs_fs * fs, whefs_inode * tgt, bool markUsed );

/**
   Searches fs for an inode with the given name. On success, tgt is updated to
   that inode's state and whefs_rc.OK is returned, otherwise some other value
   is returned.

   Bugs:

   - Directory structures are not yet supported.
*/
int whefs_inode_by_name( whefs_fs * fs, char const * name, whefs_inode * tgt );


/**
   Returns true if n is "valid" - has a non-zero id in a range legal
   for the given fs object.
*/
bool whefs_inode_is_valid( whefs_fs const * fs, whefs_inode const * n );

/**
   Equivalent to whefs_inode_is_valid(), but takes an inode ID instead
   of an inode object.
*/
bool whefs_inode_id_is_valid( whefs_fs const * fs, whefs_id_type nid );

/**
   Creates a new i/o device associated with the given inode (which
   must be a valid inode). The device allows reading and writing
   from/to the inode as if it were a normal i/o device, using
   fs's data blocks as the underlying data store. The capability
   allows such a device to itself be the host of a whefs_fs filesystem
   That is, it makes it possible to embed one VFS with another, to
   an arbitrary depth.

   On success a new device is returned, which eventualy must be
   destroyed via dev->finalize(dev). On error, 0 is returned.

   The given inode must be a fully populated object. The returned
   device will take over doing updates of that object's on-disk
   data. That is, changes to the inode made via the device
   cannot be tracked via the passed-in inode pointer.

   The returned object conforms as closely as possible to the
   whio_dev API specifications, but there may still be an
   outstanding corner case or three.

   whio_dev::ioctl(): the returned object supports the
   whio_dev_ioctl_GENERAL_size ictl to return the current size of the
   device.
*/
whio_dev * whefs_dev_for_inode( whefs_fs * fs, whefs_id_type nodeID, bool writeMode );

#if 0 // no longer needed
/**
   Returns the on-disk size of an inode INCLUDING opt->filename_length.
   Semantics of the library will soon (as of 20090115) change to remove
   the string length from the inode itself.
*/
size_t whefs_fs_sizeof_inode( whefs_fs_options const * opt );
#endif

/**
   Returns the on-disk position of the given inode, which must be a
   valid inode id for fs. fs must be opened and initialized. On error
   (!fs or !nid, or nid is out of range), 0 is returned. Does not require
   any i/o.
*/
size_t whefs_inode_pos( whefs_fs const * fs, whefs_id_type nid );

/**
   Seeks to the given inode's on-disk position. Returns whefs_rc.OK
   on success.
*/
int whefs_inode_seek( whefs_fs * fs, whefs_inode const * ino );

/**
   Equivalent to whefs_inode_seek(), but takes an inode ID.
*/
int whefs_inode_id_seek( whefs_fs * fs, whefs_id_type id );

/**
   Sets n's name to name. Returns whefs_rc.OK on success, or some
   other value on error. If the name is longer than the smaller of
   whefs_fs_options_get(fs)->filename_length or
   WHEFS_MAX_FILENAME_LENGTH, whefs_rc.RangeError is returned and n is
   not modified.

   This function does NOT change n->mtime but does update the on-disk
   record. If n is currently opened (that is, n->open_count is not 0)
   then its in-memory record (whefs_inode::name) is also updated, otherwise
   n->name is NOT updated (because it would probably not get cleaned
   up).
*/
int whefs_inode_name_set( whefs_fs * fs, whefs_inode * n, char const * name );

/**
   Loads the name for the given inode id into the given target string
   (which may not be null). If tgt has memory allocated to it, it may
   be re-used (or realloc()'d) by this function, so the caller must
   copy it beforehand if it will be needed later. If the read-in
   string has a length of 0 and tgt->alloced is not 0 (i.e. tgt
   already has content), then the string's memory is kept but is
   zeroed out and tgt->length will be set to 0.

   Returns whefs_rc.OK on success.
*/
int whefs_inode_name_get( whefs_fs * fs, whefs_id_type id, whefs_string * tgt );


/**
   If !n then it returns whefs_rc.ArgError, otherwise it returns
   whefs_rc.OK and updates n->mtime to the current time. It does not
   write the changes to disk.
*/
int whefs_inode_update_mtime( whefs_fs * fs, whefs_inode * n );

/**
   Marks the given inode as unused and wipes all associated data
   blocks. The inode need not be fully populated - only its
   ID is considered.

   The the inode is currently opened, unlinking will not be allowed
   and whefs_rc.AccessError will be returned.

   Returns whefs_rc.OK on success. On error, we cannot say how much of
   the unlink worked before we aborted (e.g. not all blocks might have
   been cleared).

   Ownership of inode is not transfered but the inode object will be
   wiped clean of all state except its ID.
  
*/
int whefs_inode_unlink( whefs_fs * fs, whefs_inode * inode );

/**
   Equivalent to whefs_inode_unlink(), but takes an inode ID.
*/
int whefs_inode_id_unlink( whefs_fs * fs, whefs_id_type nid );


/**
   Searches the table of opened inodes for the given ID. If an entry
   is found, tgt is assigned to a pointer to that object and
   whefs_rc.OK is returned. If none is found, or some other error
   happens (e.g. fs or nodeID are invalid) then some other value is
   returned and tgt is not modified.

   WARNING:

   This routine allows the caller to bypass the single-writer
   restriction on inodes, and should be used with care. It is in the
   semi-public API only to support whefs_file_rename().

*/
int whefs_inode_search_opened( whefs_fs * fs, whefs_id_type nodeID, whefs_inode ** tgt );

/**
   Encodes src (which may not be null) to dest, which must be valid memory
   of at least whefs_sizeof_encoded_inode bytes long. On succes whefs_rc.OK
   is returned and whefs_sizeof_encoded_inode bytes of dest are written.
   The only error conditions are that neither src nor dest are null, so if
   you are certain you're passing non-nulls then you can ignore the error
   check.
*/
int whefs_inode_encode( unsigned char * dest, whefs_inode const * src );

/**
   Encodes dest (which may not be null) from src, which must be valid memory
   at least whefs_sizeof_encoded_inode bytes long and containing an encoded
   inode (see whefs_inode_encode()). On succes whefs_rc.OK
   is returned and whefs_sizeof_encoded_inode bytes are read from src dest.

   Unlike whefs_inode_encode(), this routine has many potential points
   of failure (as there are several values to be decoded), so checking
   the return value is a must. On success, whefs_rc.OK is returned.
   If src or dest are null then whefs_rc.ArgError is returned. If
   decoding fails then whefs_rc.ConsistencyError is returned.
*/
int whefs_inode_decode( unsigned char const * src, whefs_inode * dest );

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* WANDERINGHORSE_NET_WHEFS_INODE_H_INCLUDED */
