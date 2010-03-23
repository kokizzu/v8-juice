/* auto-generated on Tue Mar 23 15:14:49 CET 2010. Do not edit! */
#line 1 "whio_amalgamation.c"
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200112L /* needed for ftello() and friends */
#endif
#if !defined(_ISOC99_SOURCE)
#define _ISOC99_SOURCE 1 /* needed for snprintf() */
#endif
#include "whio_amalgamation.h"
/* begin file src/whio.c */
#line 8 "src/whio.c"
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/

#include <stdlib.h>
#include <assert.h>
#include <memory.h>


const whio_mutex whio_mutex_empty = whio_mutex_empty_m;


/**
   Maintenance reminder: keep the member-name comments in the same
   order as declared in the class!
*/
const whio_rc_t whio_rc =
    {
    0, /* OK */
    /* -1 is reserved to avoid potential confusion with SizeTError. */
    -2, /* ArgError */
    -3, /* IOError */
    -4, /* AllocError */
    -5, /* InternalError */
    -6, /* RangeError */
    -7, /* InterruptedError */
    -8, /* AccessError */
    -9, /* ConsistencyError */
    -10, /* NYIError */
    -11, /* UnsupportedError */
    -12, /* TypeError */
    -13, /* DeviceFullError */
    -14, /* LockingError */
    -15, /* HashingError */
    -16, /* NotFoundError */
    -17, /* WTFError */
    (whio_size_t)-1 /* SizeTError */
    };


char const * whio_rc_string( int rc )
{
    /* can't use switch(rc) here b/c we can't use
       whio_rc.XXX in a case label.
    */
    if( ! rc ) return "whio_rc.OK";
#define C(X) else if( whio_rc.X == rc ) return "whio_rc."#X
    C(ArgError);
    C(IOError);
    C(AllocError);
    C(InternalError);
    C(RangeError);
    C(InterruptedError);
    C(AccessError);
    C(ConsistencyError);
    C(NYIError);
    C(UnsupportedError);
    C(TypeError);
    C(DeviceFullError);
    C(LockingError);
    C(HashingError);
    C(NotFoundError);
    C(WTFError);
#undef C
    else return NULL;
}

void whio_noop_printf(char const * fmt, ...)
{
}
static void * whio_realloc_default( void * m, unsigned int n, void * state )
{
    return realloc( m, n );
}

const whio_allocator whio_allocator_stdalloc = {whio_realloc_default,NULL};
whio_allocator whio_memory_source = {whio_realloc_default,NULL};
void * whio_realloc( void * m, unsigned int n )
{
    return whio_memory_source.realloc( m, n, whio_memory_source.state );
}
void * whio_malloc( unsigned int n )
{
    return whio_realloc( NULL, n );
}

void whio_free( void * m )
{
    if(NULL != m) whio_realloc( m, 0 );
    return;
}
/* end file src/whio.c */
/* begin file src/whio_common.c */
#line 8 "src/whio_common.c"
#include <string.h> /* strchr() */

const whio_client_data whio_client_data_empty = whio_client_data_empty_m;
const whio_impl_data whio_impl_data_empty = whio_impl_data_empty_m;

short whio_mode_to_iomode( char const * mode )
{
    if( ! mode ) return -1;
    else if( (0 != strchr( mode, 'w' )) )
    { 
        return 1;
    }
    else if( 0 != strchr( mode, 'r' ) )
    {
	if( 0 != strchr( mode, '+' ) )
        {
            return 1;
        }
	else return 0;
    }
    else return -1;
}
/* end file src/whio_common.c */
/* begin file src/whio_dev.c */
#line 8 "src/whio_dev.c"
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/

#if defined(__cplusplus) && !defined(__STDC_LIMIT_MACROS)
#  define __STDC_LIMIT_MACROS
/* A comment from the Linux stdint.h:

   The ISO C99 standard specifies that in C++ implementations these
   macros should only be defined if explicitly requested.
*/
#endif
#include <stdint.h>



#include <string.h> /* memset() */
#include <stdlib.h> /* calloc() and friends */
#include <inttypes.h> /* PRIuXX */

const whio_dev whio_dev_empty = whio_dev_empty_m;
const whio_lock_request whio_lock_request_empty = whio_lock_request_empty_m;

const whio_lock_request whio_lock_request_test_r =
    { whio_lock_TYPE_READ,
      whio_lock_CMD_TEST,
      SEEK_SET,
      0, 1};
const whio_lock_request whio_lock_request_test_w =
    { whio_lock_TYPE_WRITE,
      whio_lock_CMD_TEST,
      SEEK_SET,
      0, 1};
const whio_lock_request whio_lock_request_set_w =
    { whio_lock_TYPE_WRITE,
      whio_lock_CMD_SET_NOWAIT,
      SEEK_SET,
      0, 0};
const whio_lock_request whio_lock_request_set_r =
    { whio_lock_TYPE_READ,
      whio_lock_CMD_SET_NOWAIT,
      SEEK_SET,
      0, 0};
const whio_lock_request whio_lock_request_setw_w =
    { whio_lock_TYPE_WRITE,
      whio_lock_CMD_SET_WAIT,
      SEEK_SET,
      0, 0};
const whio_lock_request whio_lock_request_setw_r =
    { whio_lock_TYPE_READ,
      whio_lock_CMD_SET_WAIT,
      SEEK_SET,
      0, 0};
const whio_lock_request whio_lock_request_set_u =
    { whio_lock_TYPE_UNLOCK,
      whio_lock_CMD_SET_NOWAIT,
      SEEK_SET,
      0, 0};
const whio_lock_request whio_lock_request_setw_u =
    { whio_lock_TYPE_UNLOCK,
      whio_lock_CMD_SET_WAIT,
      SEEK_SET,
      0, 0};


#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whio_dev_alloc_slots object.
*/
whio_dev_alloc_count = 15
};
struct
{
    whio_dev devs[whio_dev_alloc_count]; /* Flawfinder: ignore  this is intentional. */
    char used[whio_dev_alloc_count]; /* Flawfinder: ignore  this is intentional. */
    whio_size_t next;
} whio_dev_alloc_slots = { {whio_dev_empty_m}, {0}, 0 };
#endif

whio_dev * whio_dev_alloc()
{
    whio_dev * dev = 0;
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = whio_dev_alloc_slots.next;
    for( ; i < whio_dev_alloc_count; ++i )
    {
	if( whio_dev_alloc_slots.used[i] ) continue;
	whio_dev_alloc_slots.next = i+1;
	whio_dev_alloc_slots.used[i] = 1;
	dev = &whio_dev_alloc_slots.devs[i];
	//WHIO_DEBUG("Allocated device #%u @0x%p\n", i, (void const *)dev );
	break;
    }
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! dev ) dev = (whio_dev *) whio_malloc( sizeof(whio_dev) );
    if( dev ) *dev = whio_dev_empty;
    return dev;
}

void whio_dev_free( whio_dev * dev )
{
    if( dev ) *dev = whio_dev_empty;
    else return;	
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (dev < &whio_dev_alloc_slots.devs[0]) ||
	(dev > &whio_dev_alloc_slots.devs[whio_dev_alloc_count-1]) )
    { /* doesn't belong to us. */
	whio_free(dev);
	return;
    }
    else
    {
	size_t ndx = dev - &whio_dev_alloc_slots.devs[0];
	if( 0 )
	{
	    WHIO_DEBUG("Address range = 0x%p to 0x%p, ndx=%u\n",
		       (void const *)&whio_dev_alloc_slots.devs[0],
		       (void const *)&whio_dev_alloc_slots.devs[whio_dev_alloc_count-1],
		       ndx
		       );
	    WHIO_DEBUG("Freeing object @0x%p from static pool index %u (@0x%p)\n",
		       (void const *)dev,
		       ndx,
		       (void const *)&whio_dev_alloc_slots.devs[ndx] );
	}
	whio_dev_alloc_slots.used[ndx] = 0;
	if( ndx < whio_dev_alloc_slots.next ) whio_dev_alloc_slots.next = ndx;
	return;
    }
#else
    whio_free(dev);
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
}


int whio_dev_ioctl( whio_dev * dev, int operation, ... )
{
    if( ! dev ) return whio_rc.ArgError;
    va_list vargs;
    va_start( vargs, operation );
    int rc = dev->api->ioctl( dev, operation, vargs );
    va_end(vargs);
    return rc;
}

whio_size_t whio_dev_write( whio_dev * dev, void const * data, whio_size_t n )
{
    return dev->api->write( dev, data, n );
}

whio_size_t whio_dev_writeat( whio_dev * dev, whio_size_t pos, void const * data, whio_size_t n )
{
    if( ! dev || ! data || !n ) return 0;
    //WHIO_DEBUG("Writing %u bytes at pos %u\n", n, pos );
    whio_size_t rc = dev->api->seek( dev, pos, SEEK_SET );
    return (whio_rc.SizeTError == rc)
	? rc
	: whio_dev_write( dev, data, n );
}

whio_size_t whio_dev_readat( whio_dev * dev, whio_size_t pos, void * data, whio_size_t n )
{
    if( ! dev || ! data || !n ) return 0;
    //WHIO_DEBUG("Writing %u bytes at pos %u\n", n, pos );
    whio_size_t rc = dev->api->seek( dev, pos, SEEK_SET );
    return (whio_rc.SizeTError == rc)
	? rc
	: whio_dev_read( dev, data, n );
}


whio_size_t whio_dev_size( whio_dev * dev )
{
    if( ! dev ) return whio_rc.SizeTError;
    whio_size_t pos = dev->api->tell( dev );
    if( whio_rc.SizeTError == pos ) return pos;
    whio_size_t rc = dev->api->seek( dev, 0L, SEEK_END );
    dev->api->seek( dev, pos, SEEK_SET );
    return rc;
}

int whio_dev_rewind( whio_dev * dev )
{
    if( ! dev ) return whio_rc.ArgError;
    return (whio_rc.SizeTError != dev->api->seek( dev, 0, SEEK_SET ))
	? whio_rc.OK
	: whio_rc.IOError;
}

int whio_dev_copy( whio_dev * src, whio_dev * dest )
{
    if( ! src || ! dest ) return whio_rc.ArgError;
    int rc = whio_rc.OK;
    enum { bufSize = (1024 * 4) };
    unsigned char buf[bufSize];  /* Flawfinder: ignore This is intentional and used correctly in the loop below. */
    whio_size_t rlen = 0;
    if( whio_rc.SizeTError == src->api->seek( src, 0L, SEEK_SET ) )
    {
	return whio_rc.RangeError;
    }
    while( (rlen = src->api->read( src, buf /*Flawfinder: ignore  This is safe in conjunction with bufSize*/, bufSize ) ) )
    {
	if( rlen != dest->api->write( dest, buf, rlen ) )
	{
	    rc = whio_rc.IOError;
	    break;
	}
    }
    return rc;
}


static long whio_dev_printf_appender( void * arg, char const * data, long n )
{
    if( ! arg || !data || (n<1) ) return -1;
    size_t sz = n;
    if( n < sz ) return -1; /* negative n */
    whio_dev * dev = (whio_dev*)arg;
    sz = dev->api->write( dev, data, sz );
    return (sz == whio_rc.SizeTError) ? 0 : (long) sz; // FIXME: check for overflow!
}

size_t whio_dev_writefv( whio_dev * dev, const char *fmt, va_list ap )
{
    long rc = whprintfv( whio_dev_printf_appender, dev, fmt, ap );
    return (rc < 0) ? 0 : (size_t)rc;
}

size_t whio_dev_writef( whio_dev * dev, const char *fmt, ... )
{
    va_list vargs;
    va_start( vargs, fmt );
    size_t rc = whio_dev_writefv( dev, fmt, vargs );
    va_end(vargs);
    return rc;
}

whio_size_t whio_dev_read( whio_dev * dev, void * dest, whio_size_t n )
{
    return dev ? dev->api->read( dev, dest, n ) : 0; /*Flawfinder: ignore  Bounds check is done in proxied function (cannot be done here). */
}

int whio_dev_eof( whio_dev * dev )
{
    return dev ? dev->api->eof( dev ) : whio_rc.ArgError;
}

whio_size_t whio_dev_tell( whio_dev * dev )
{
    return dev ? dev->api->tell( dev ) : whio_rc.SizeTError;;
}

whio_size_t whio_dev_seek( whio_dev * dev, whio_off_t pos, int whence )
{
    return dev ? dev->api->seek( dev, pos, whence ) : whio_rc.SizeTError;
}

int whio_dev_flush( whio_dev * dev )
{
    return dev ? dev->api->flush( dev ) : whio_rc.ArgError;
}

int whio_dev_truncate( whio_dev * dev, whio_off_t size )
{
    return dev ? dev->api->truncate( dev, size ) : whio_rc.ArgError;
}

void whio_dev_finalize( whio_dev * dev )
{
    if( dev ) dev->api->finalize( dev );
    return;
}

bool whio_dev_close( whio_dev * dev )
{
    return dev ? dev->api->close( dev ) : false;
}

int whio_dev_lock( whio_dev * dev, whio_lock_request * lock )
{
    if( ! dev || !lock ) return whio_rc.ArgError;
    /** This check is _also_ left up to device impls because
        they might get locks requested through a different
        interface (i.e. directly over ioctl()).

        We perform this check _here_ mainly as a fail-fast case before
        delving into ioctl().
    */
    else if( (lock->type == whio_lock_TYPE_WRITE)
             && (dev->api->iomode(dev) == 0)
              /* (iomode<0) _might_ be writable - we can't know without trying. */
             )
    {
        return whio_rc.AccessError;
    }
    return whio_dev_ioctl( dev, whio_dev_ioctl_WHIO_LOCK, lock );
}

whio_fetch_result * whio_dev_fetch( whio_dev * dev, whio_size_t n )
{
    if( ! dev ) return 0;
    whio_fetch_result * rc = (whio_fetch_result*)whio_malloc(sizeof(whio_fetch_result));
    if( ! rc ) return 0;
    rc->alloced = 0;
    rc->requested = n;
    rc->read = 0;
    if( ! n ) return rc;
    const whio_size_t sza = n+1; /* the +1 is necessary so we can ensure nulls for script-side strings. */
    rc->data = (char *)malloc(sza);
    if( ! rc->data )
    {
	whio_free(rc);
	return 0;
    }
    rc->alloced = sza;
    memset( rc->data, 0, sza );
    rc->read = dev->api->read( dev, rc->data, n ); /*Flawfinder: ignore rc->data will always be longer than (see above). */
    return rc;
}

int whio_dev_fetch_r( whio_dev * dev, whio_size_t n, whio_fetch_result * tgt )
{
    if( ! dev || !tgt ) return whio_rc.ArgError;
    if( ! n )
    {
	tgt->requested = tgt->read = n;
	return whio_rc.OK;
    }
    tgt->read = 0;
    if( !tgt->data || (tgt->alloced < n) )
    {
	whio_size_t sza = n + 1;
	void * p = realloc( tgt->data, sza );
	if( ! p ) return whio_rc.AllocError;
	memset( p, 0, sza );
	tgt->data = p;
	tgt->alloced = sza;
    }
    tgt->requested = n;
    memset( tgt->data, 0, n );
    tgt->read = dev->api->read( dev, tgt->data, n );  /*Flawfinder: ignore  tgt->data will always be  longer than n as long as tgt->allocated is set properly. */
    return whio_rc.OK;
}

int whio_dev_fetch_free( whio_fetch_result * r )
{
    if( r )
    {
        whio_dev_fetch_free_data(r);
	whio_free(r);
	return whio_rc.OK;
    }
    return whio_rc.ArgError;
}
int whio_dev_fetch_free_data( whio_fetch_result * r )
{
    if( r )
    {
        if( r->data )
        {
            free(r->data);
            r->alloced = 0;
            r->data = 0;
        }
	return whio_rc.OK;
    }
    return whio_rc.ArgError;
}


const whio_blockdev whio_blockdev_empty = whio_blockdev_empty_m;
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whio_blockdev_alloc_slots object.
*/
whio_blockdev_alloc_count = 10
};
#define whio_blockdev_alloc_slots_whio_blockdev_INIT {0 /* FILL THIS OUT FOR whio_blockdev OBJECTS! */}
static struct
{
    whio_blockdev objs[whio_blockdev_alloc_count]; /* Flawfinder: ignore This is intentional. */
    char used[whio_blockdev_alloc_count]; /* Flawfinder: ignore This is intentional. */
} whio_blockdev_alloc_slots = { { whio_blockdev_empty_m }, {0} };
#endif

whio_blockdev * whio_blockdev_alloc()
{
    whio_blockdev * obj = 0;
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whio_blockdev_alloc_count; ++i )
    {
	if( whio_blockdev_alloc_slots.used[i] ) continue;
	whio_blockdev_alloc_slots.used[i] = 1;
	obj = &whio_blockdev_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_blockdev *) whio_malloc( sizeof(whio_blockdev) );
    if( obj ) *obj = whio_blockdev_empty;
    return obj;
}

void whio_blockdev_free( whio_blockdev * obj )
{
    if( ! obj ) return;
    whio_blockdev_cleanup( obj );
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_blockdev_alloc_slots.objs[0]) ||
	(obj > &whio_blockdev_alloc_slots.objs[whio_blockdev_alloc_count-1]) )
    { /* it does not belong to us */
	whio_free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whio_blockdev_alloc_slots.objs[0]);
	whio_blockdev_alloc_slots.objs[ndx] = whio_blockdev_empty;
	whio_blockdev_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    whio_free(obj);
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
}

bool whio_blockdev_cleanup( whio_blockdev * self )
{
    if( ! self ) return false;
    if( self->impl.fence )
    {
        if( self->blocks.count )
        {
            self->impl.fence->api->finalize( self->impl.fence );
        }
        // else fence was pointing back to the parent device.
	self->impl.fence = 0;
    }
    *self = whio_blockdev_empty;
    return true;
}


int whio_blockdev_setup( whio_blockdev * self, whio_dev * parent, whio_size_t parent_offset,
                         whio_size_t block_size, whio_size_t count, void const * prototype )
{
    if( ! self || ! parent || ! count || ! block_size ) return whio_rc.ArgError;
    *self = whio_blockdev_empty;
    self->impl.fence = whio_dev_subdev_create( parent, parent_offset, parent_offset + (count * block_size) );
    if( ! self->impl.fence ) return whio_rc.AllocError;
    self->blocks.prototype = prototype;
    self->blocks.size = block_size;
    self->blocks.count = count;
    return whio_rc.OK;
}

int whio_blockdev_setup2( whio_blockdev * self, whio_dev * parent, whio_size_t block_size, void const * prototype )
{
    if( ! self || ! parent || ! block_size ) return whio_rc.ArgError;
    *self = whio_blockdev_empty;
    self->impl.fence = parent;
    self->blocks.prototype = prototype;
    self->blocks.size = block_size;
    self->blocks.count = 0;
    return whio_rc.OK;
}

int whio_blockdev_wipe( whio_blockdev * self, whio_size_t id )
{
    return (self && self->blocks.prototype)
        ? whio_blockdev_write( self, id, self->blocks.prototype )
        : whio_rc.ArgError;
}

bool whio_blockdev_in_range( whio_blockdev const * self, whio_size_t id )
{
    return !self
	? false
	: (self->blocks.count ? (id < self->blocks.count) : true);
}

/**
   Returns the on-disk position of the given block ID, relative to
   self, or whio_rc.SizeTError if !self or if id is not in range for
   self.
*/
static whio_size_t whio_block_offset_for_id( whio_blockdev * self, whio_size_t id )
{
    return ( ! self || !whio_blockdev_in_range( self, id ) )
	? whio_rc.SizeTError
	: (id * self->blocks.size);
}

int whio_blockdev_write( whio_blockdev * self, whio_size_t id, void const * src )
{
    if( ! src ) return whio_rc.ArgError;
    whio_size_t pos = whio_block_offset_for_id( self, id );
    if( whio_rc.SizeTError == pos )
    {
	WHIO_DEBUG("id #%"WHIO_SIZE_T_PFMT" is not valid for this whio_blockdev. block count=%"WHIO_SIZE_T_PFMT"\n",id,self->blocks.count);
	return whio_rc.RangeError;
    }
    if( ! src ) return false;
    if( pos != self->impl.fence->api->seek( self->impl.fence, pos, SEEK_SET ) ) return whio_rc.IOError;
    return (self->blocks.size == self->impl.fence->api->write( self->impl.fence, src, self->blocks.size ))
	? whio_rc.OK
	: whio_rc.IOError;
}

int whio_blockdev_read( whio_blockdev * self, whio_size_t id, void * dest )
{
    whio_size_t pos = whio_block_offset_for_id( self, id );
    if( whio_rc.SizeTError == pos ) return whio_rc.RangeError;
    if( pos != self->impl.fence->api->seek( self->impl.fence, pos, SEEK_SET ) ) return whio_rc.IOError;
    return (self->blocks.size == self->impl.fence->api->read( self->impl.fence, dest, self->blocks.size ))  /*Flawfinder: ignore  Bounds check is mostly done in self->impl.fence->api->read(). Bounds of dest must be ensured by the caller. */
	? whio_rc.OK
	: whio_rc.IOError;
}
/* end file src/whio_dev.c */
/* begin file src/whio_dev_FILE.c */
#line 8 "src/whio_dev_FILE.c"
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/
/************************************************************************
Implementations for a whio_dev type for wrapping a FILE handle.
It complies with the whio_dev interface, and all
implementation-specified behaviours of the interface are documented
along with the factory functions for creating the device objects.
************************************************************************/

#if !defined(_POSIX_C_SOURCE)
/* required for for fileno(), ftello(), maybe others */
#  define _POSIX_C_SOURCE 200112L
//#  define _POSIX_C_SOURCE 199309L
//#  define _POSIX_C_SOURCE 199506L
#endif

#include <unistd.h> /* ftruncate() */
#include <stdlib.h> /* malloc()/free() */
#include <string.h> /* memset() */
//#include <stdio.h>
#if defined(__GNUC__) || defined(__TINYC__)
#if !defined(GCC_VERSION) || (GCC_VERSION < 40100)
/* i don't actually know which versions need this, but 4.0.2 does. */
    extern int ftruncate(int , off_t);
    extern int fsync(int fd);
//#  warning "Kludging ftruncate() and fsync() declartions."
//#else
//#  warning "Hoping ftruncate() and fsync() are declared."
#endif
#endif /* __GNUC__ */


#if WHIO_CONFIG_USE_FCNTL
#  include <fcntl.h>
#endif

/**
   Internal implementation details for the whio_dev FILE wrapper.
*/
typedef struct whio_dev_FILE
{
    /**
       Underlying FILE handle. Owned by this
       object.
    */
    FILE * fp;
    int fileno;
    /**
       Flags whether we need to do a flush (i.e. if any writes
       have been called since the last flush).
     */
    short needsFlush;
    bool ownsFile;
    short iomode;
    /** We use this to allow us to combine two mallocs into one. */
    whio_dev dev;
} whio_dev_FILE;


/**
   Initialization object for whio_dev_FILE objects. Also used as
   whio_dev::typeID for such objects.
*/
#define WHIO_DEV_FILE_INIT { \
    0, /* fp */ \
    0, /* fileno */ \
    0, /* needsFlush */ \
    0, /* ownsFile */                       \
   -1 /*iomode*/,                      \
    whio_dev_empty_m/*dev*/ \
    }
static const whio_dev_FILE whio_dev_FILE_meta_init = WHIO_DEV_FILE_INIT;

#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whio_dev_FILE_alloc_slots object.
*/
whio_dev_FILE_alloc_count = 5
};
struct
{
    whio_dev_FILE objs[whio_dev_FILE_alloc_count];
    char used[whio_dev_FILE_alloc_count];
} whio_dev_FILE_alloc_slots = { {WHIO_DEV_FILE_INIT}, {0} };
#endif

#undef WHIO_DEV_FILE_INIT

static whio_dev_FILE * whio_dev_FILE_alloc()
{
    whio_dev_FILE * obj = 0;
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whio_dev_FILE_alloc_count; ++i )
    {
	if( whio_dev_FILE_alloc_slots.used[i] ) continue;
	whio_dev_FILE_alloc_slots.used[i] = 1;
	obj = &whio_dev_FILE_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_FILE *) whio_malloc( sizeof(whio_dev_FILE) );
    if( obj ) *obj = whio_dev_FILE_meta_init;
    return obj;
}

static void whio_dev_FILE_free( whio_dev_FILE * obj )
{
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_dev_FILE_alloc_slots.objs[0]) ||
	(obj > &whio_dev_FILE_alloc_slots.objs[whio_dev_FILE_alloc_count-1]) )
    { /* it does not belong to us */
	whio_free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whio_dev_FILE_alloc_slots.objs[0]);
	whio_dev_FILE_alloc_slots.objs[ndx] = whio_dev_FILE_meta_init;
	whio_dev_FILE_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    whio_free(obj);
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
}


/**
   A helper for the whio_dev_FILE API. Requires that the 'dev'
   parameter be-a whio_dev and that that device is-a whio_dev_FILE.
 */
#define WHIO_FILE_DECL(RV) whio_dev_FILE * f = (dev ? (whio_dev_FILE*)dev->impl.data : 0); \
    if( !f || !f->fp || ((void const *)&whio_dev_FILE_meta_init != dev->impl.typeID) ) return RV

static whio_size_t whio_dev_FILE_read( whio_dev * dev, void * dest, whio_size_t n )
{
    WHIO_FILE_DECL(whio_rc.SizeTError);
    if( f->needsFlush ) dev->api->flush(dev);
    return (dev && dest)
	? fread( dest, sizeof(char), n, f->fp )
	: 0;
}

static whio_size_t whio_dev_FILE_write( whio_dev * dev, void const * src, whio_size_t n )
{
    WHIO_FILE_DECL(0);
    f->needsFlush = (n ? 1 : 0);
    const whio_size_t rc = (dev && src && n)
	? fwrite( src, sizeof(char), n, f->fp )
	: 0;
    if( rc == n ) f->iomode = 1;
    return rc;
}

static int whio_dev_FILE_error( whio_dev * dev )
{
    WHIO_FILE_DECL(whio_rc.ArgError);
    return ferror(f->fp);
}

static int whio_dev_FILE_clear_error( whio_dev * dev )
{
    WHIO_FILE_DECL(whio_rc.ArgError);
    clearerr(f->fp);
    return whio_rc.OK;
}

static int whio_dev_FILE_eof( whio_dev * dev )
{
    WHIO_FILE_DECL(whio_rc.ArgError);
    return feof(f->fp);
}

static whio_size_t whio_dev_FILE_tell( whio_dev * dev )
{
    WHIO_FILE_DECL(whio_rc.SizeTError);
    off_t rc = ftello(f->fp);
    return (rc>=0) ? (whio_size_t)rc : whio_rc.SizeTError;
}

static whio_size_t whio_dev_FILE_seek( whio_dev * dev, whio_off_t pos, int whence )
{
    WHIO_FILE_DECL(whio_rc.SizeTError);
    if( 0 == fseeko( f->fp, pos, whence ) )
    {
	off_t t = ftello( f->fp );
	return (t >= 0) ? (whio_size_t)t : whio_rc.SizeTError;
    }
    return whio_rc.SizeTError;
}

static int whio_dev_FILE_flush( whio_dev * dev )
{
    WHIO_FILE_DECL(whio_rc.ArgError);
    if( 0 == f->iomode ) return whio_rc.AccessError;
    else
    {
        f->needsFlush = 0;
        return (0 == fflush( f->fp ))
            ? whio_rc.OK
            : whio_rc.IOError;
    }
}

static int whio_dev_FILE_trunc( whio_dev * dev, whio_off_t len )
{
    WHIO_FILE_DECL(whio_rc.ArgError);
    if( 0 == f->iomode ) return whio_rc.AccessError;
    else
    {
        return (0 == ftruncate( f->fileno, len ))
            ? whio_rc.OK
            : whio_rc.IOError;
        /** ^^^ is there a way to truncate a FILE handle without using fileno()? */
    }
}


// internal func defined and documented in whio_dev_fileno.c:
extern int whio_dev_ioctl_fcntl_proxy( short iomode, int fileno, int ctl, void * lockObj );

static int whio_dev_FILE_ioctl( whio_dev * dev, int arg, va_list vargs )
{
    WHIO_FILE_DECL(whio_rc.ArgError);
    /**
       The standard ioctl() looks like:

       int ioctl(int d, int request, ...);

       which means there's no way for us to pass our ... directly to
       it.  So it appears to be impossible to emulate the system's
       ioctl() without literally checking every possible ioctl and
       casting the first ... arg to the proper type (which is likely
       platform-dependent).
    */
    int rc = whio_rc.UnsupportedError;
    switch( arg )
    {
      case whio_dev_ioctl_FILE_fd:
	  rc = whio_rc.OK;
	  *(va_arg(vargs,int*)) = f->fileno;
	  break;
#if WHIO_CONFIG_USE_FCNTL
      case whio_dev_ioctl_mask_FCNTL_LOCKING:
      case whio_dev_ioctl_mask_WHIO_LOCKING:
          rc = whio_rc.OK;
          break;          
      case whio_dev_ioctl_FCNTL_lock_nowait:
      case whio_dev_ioctl_FCNTL_lock_wait:
      case whio_dev_ioctl_FCNTL_lock_get:
      case whio_dev_ioctl_WHIO_LOCK:
          rc = whio_dev_ioctl_fcntl_proxy( f->iomode, f->fileno, arg, va_arg(vargs,void *) );
          break;
#endif
      default: break;
    };
    return rc;
}

short whio_dev_FILE_iomode( whio_dev * dev )
{
    WHIO_FILE_DECL(-1);
    return f->iomode;
}

static bool whio_dev_FILE_close( whio_dev * dev )
{
    WHIO_FILE_DECL(false);
    dev->api->flush(dev);
    if( dev->client.dtor ) dev->client.dtor( dev->client.data );
    /**
       Reminder: we want it to be (and stay) legal for client.data to
       refer to f->fp and client.dtor to destroy f->fp IFF
       !f->ownsFile. This allows us to attach (e.g.) popen()-opened
       handles as the client data of the device and clean them up
       properly when the device is closed.

       So... don't touch f->fp below unless f->ownsFile.
    */
    dev->client = whio_client_data_empty;
    dev->impl.data = 0;
    if( f->fp && f->ownsFile ) fclose( f->fp );
    f->fileno = 0;
    return true;
}

static void whio_dev_FILE_finalize( whio_dev * dev )
{
    WHIO_FILE_DECL();
    dev->api->close( dev );
    whio_dev_FILE_free( f );
}
#undef WHIO_FILE_DECL

static const whio_dev_api whio_dev_FILE_api =
    {
    whio_dev_FILE_read,
    whio_dev_FILE_write,
    whio_dev_FILE_close,
    whio_dev_FILE_finalize,
    whio_dev_FILE_error,
    whio_dev_FILE_clear_error,
    whio_dev_FILE_eof,
    whio_dev_FILE_tell,
    whio_dev_FILE_seek,
    whio_dev_FILE_flush,
    whio_dev_FILE_trunc,
    whio_dev_FILE_ioctl,
    whio_dev_FILE_iomode
    };

static const whio_dev whio_dev_FILE_init =
    {
    &whio_dev_FILE_api,
    { /* impl */
    0, /* data. Must be-a (whio_dev_FILE*) */
    (void const *)&whio_dev_FILE_meta_init /* typeID */
    }
    };

whio_dev * whio_dev_for_FILE( FILE * F, bool takeOwnership )
{
#if 0
    // TODO: test this, and enable it if it really does what i think it should do:
    if( (off_t)-1 == lseek( fileno(F), 0L, SEEK_CUR ) )
    {/* device does not seem to be seekable (not random-access). */
	return 0;
    }
#endif
    whio_dev_FILE * meta = whio_dev_FILE_alloc();
    if( ! meta ) return NULL;
    *meta = whio_dev_FILE_meta_init;
    whio_dev * dev = &meta->dev;
    *dev = whio_dev_FILE_init;
    dev->impl.data = meta;
    meta->fp = F;
    meta->ownsFile = takeOwnership;
    meta->fileno = fileno(F);
    meta->iomode = -1;
    return dev;
}

#if 0 /* now implemented in whio_dev_fileno.c, but this may be interesting for later. */
whio_dev * whio_dev_for_filename( char const * fname, char const * mode )
{
    if( ! fname || !mode ) return 0;
    FILE * f = fopen( fname, mode );
    if( ! f ) return 0;
    whio_dev * d = whio_dev_for_FILE( f, true );
    if( ! d )
    {
	fclose(f);
        return 0;
    }
    whio_dev_FILE * meta = (whio_dev_FILE*)d->impl.data;
    meta->iomode = whio_mode_to_iomode( mode );
    return d;
}
#endif

/* end file src/whio_dev_FILE.c */
/* begin file src/whio_dev_fileno.c */
#line 8 "src/whio_dev_fileno.c"
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain

  Many thanks to Lew Pitcher for his tips on implementing this:

  http://groups.google.com/group/comp.unix.programmer/browse_thread/thread/9ffb66c1d0a4f7f3/7c28cd32b63d99a4

*/
/************************************************************************
Implementations for a whio_dev type for wrapping a file descriptor
handle.  This is almost a 100% copy/paste of the code from
whio_dev_FILE.c, but it uses the lower-level read()/write() API
instead of the fXXX(FILE*,...) API. Simple tests in libwhefs show this
to provide dramatic speed increases.
************************************************************************/

#if !defined(_POSIX_C_SOURCE)
/* required for for fileno(), ftello(), fdatasync(), maybe others */
#  define _POSIX_C_SOURCE 200112L
//#  define _POSIX_C_SOURCE 199309L
//#  define _POSIX_C_SOURCE 199506L
#endif
#include <assert.h>
#include <stdlib.h> /* malloc()/free() */
#include <string.h> /* memset() */
#include <unistd.h> /* ftruncate(), fdatasync() */
#include <errno.h>

#if defined(__GNUC__) || defined(__TINYC__)
#if !defined(GCC_VERSION) || (GCC_VERSION < 40100)
/* i don't actually know which versions need this, but 4.0.2 does. */
    extern int ftruncate(int, off_t);
    extern int fsync(int fd);
//#  warning "Kludging ftruncate() and fsync() declartions."
//#else
//#  warning "Hoping ftruncate() and fsync() are declared."
#endif
#endif /* __GNUC__ */


#if WHIO_CONFIG_USE_FCNTL
#  include <fcntl.h>
#endif

/**
   Internal implementation details for the whio_dev file descriptor
   wrapper.
*/
typedef struct whio_dev_fileno
{
    /**
       Underlying FILE handle. Owned by this
       object.
    */
    FILE * fp;
    int fileno;
    char * filename;
    bool atEOF;
    int errstate;
    short iomode;
    /** We use this to allow us to combine two mallocs into one. */
    whio_dev * dev;
    whio_dev devMem;
} whio_dev_fileno;


/**
   Initialization object for whio_dev_fileno objects. Also used as
   whio_dev::typeID for such objects.
*/
#define whio_dev_fileno_empty_m { \
    0, /* fp */ \
    -1, /* fileno */ \
    0, /* filename */ \
    false, /* atEOF */ \
    0, /* errstate */                       \
    -1 /*iomode*/,                      \
        NULL/*dev*/,                            \
        whio_dev_empty_m/*devMem*/              \
    }
static const whio_dev_fileno whio_dev_fileno_meta_empty = whio_dev_fileno_empty_m;


/**
   Allocates (sizeof(whio_dev_fileno)+plus) bytes. If plus is not 0
   then the 'filename' member if the allocated object is pointed to
   that memory, which lies just beyond the whio_dev_fileno object.
*/
static whio_dev_fileno * whio_dev_fileno_alloc( whio_size_t plus )
{
    unsigned char * m = (unsigned char *)whio_malloc( sizeof(whio_dev_fileno) + plus );
    whio_dev_fileno * obj = 0;
    if( m )
    {
        obj = (whio_dev_fileno*)m;
        *obj = whio_dev_fileno_meta_empty;
        if( plus )
        {
            obj->filename = (char *) (m+sizeof(whio_dev_fileno));
        }
    }
    return obj;
}

static void whio_dev_fileno_free( whio_dev_fileno * obj )
{
    whio_free(obj);
}


/**
   A helper for the whio_dev_fileno API. Requires that the 'dev'
   parameter be-a whio_dev and that that device is-a whio_dev_fileno.
 */
#define WHIO_fileno_DECL(RV) whio_dev_fileno * f = (dev ? (whio_dev_fileno*)dev->impl.data : 0); \
    if( !f || (f->fileno<0) || ((void const *)&whio_dev_fileno_meta_empty != dev->impl.typeID) ) return RV

static whio_size_t whio_dev_fileno_read( whio_dev * dev, void * dest, whio_size_t n )
{
    WHIO_fileno_DECL(whio_rc.SizeTError);
    if( ! dest || !n ) return 0;
    ssize_t rc = read( f->fileno, dest, n );
    if( 0 == rc )
    {
	f->atEOF = true;
    }
    else if( (ssize_t)-1 == rc )
    {
	f->errstate = errno;
	rc = 0;
    }
    return (whio_size_t)rc;
}

static whio_size_t whio_dev_fileno_write( whio_dev * dev, void const * src, whio_size_t n )
{
    WHIO_fileno_DECL(0);
    if( ! src || !n ) return 0;
    ssize_t rc = write( f->fileno, src, n );
    if( (ssize_t)-1 == rc )
    {
	f->errstate = errno;
	rc = 0;
    }
    return rc;
}

static int whio_dev_fileno_error( whio_dev * dev )
{
    WHIO_fileno_DECL(whio_rc.ArgError);
    /**
      ferror(f->fp) is not likely to be valid b/c we're
      using the low-level i/o API, but what the heck...
    */
    //return ferror(f->fp);
    return f->errstate;
}

static int whio_dev_fileno_clear_error( whio_dev * dev )
{
    WHIO_fileno_DECL(whio_rc.ArgError);
    /** Because we use the low-level read/write() API instead of fread()/fwrite()
	and friends, using clearerr(f->fp) isn't really going to give us anything.
	We'll go ahead and call it and assume the best.
    */
    //clearerr(f->fp);
    f->errstate = 0;
    f->atEOF = false;
    return whio_rc.OK;
}

static int whio_dev_fileno_eof( whio_dev * dev )
{
    WHIO_fileno_DECL(whio_rc.ArgError);
    return  f->atEOF ? 1 : 0;
}

static whio_size_t whio_dev_fileno_tell( whio_dev * dev )
{
    WHIO_fileno_DECL(whio_rc.SizeTError);
    off_t rc = lseek( f->fileno, 0L, SEEK_CUR );
    return (rc>=0) ? (whio_size_t)rc : whio_rc.SizeTError;
}

static whio_size_t whio_dev_fileno_seek( whio_dev * dev, whio_off_t pos, int whence )
{
    WHIO_fileno_DECL(whio_rc.SizeTError);
    off_t rc = lseek( f->fileno, pos, whence );
    if( pos == rc )
    {
	/**
	   The man page for fseek() says (on my system):

	   "A successful call to the fseek() function clears the
	   end-of-file indicator for the stream."
	*/
	f->atEOF = false;
    }
    return (rc>=0) ? (whio_size_t) rc : whio_rc.SizeTError;
}

static int whio_dev_fileno_flush( whio_dev * dev )
{
    WHIO_fileno_DECL(whio_rc.ArgError);
    if( 0 == f->iomode ) return whio_rc.AccessError;
    else
    {
        int rc = fsync( f->fileno );
        /* reminder: i realy want fdatasync(), but some platforms
         *cough* Solaris *cough* don't appear to have it. */
        if( 0 != rc ) rc = whio_rc.IOError;
        return rc;
    }
}

static int whio_dev_fileno_trunc( whio_dev * dev, whio_off_t len )
{
    WHIO_fileno_DECL(whio_rc.ArgError);
    if( 0 == f->iomode ) return whio_rc.AccessError;
    else
    {
        int rc = whio_rc.OK;
        if( 0 == ftruncate( f->fileno, len ) )
        {
            whio_dev_fileno_flush( dev );
        }
        else
        {
            rc = whio_rc.IOError;
        }
        return rc;
    }
}

short whio_dev_fileno_iomode( whio_dev * dev )
{
    WHIO_fileno_DECL(-1);
    return f->iomode;
}

/** @internal


    Helper for use by whio_dev impls which use fcntl() locks. This
    function implements the whio_dev_ioctl_FCNTL family of locking
    ioctls, as well as whio_dev_ioctl_WHIO_LOCKING (by translating the
    request to an fcntl() lock).

    If WHIO_CONFIG_USE_FCNTL is false then this function does nothing
    and returns whio_rc.UnsupportedError.
    
    The args:
    
    - iomode is the whio_dev::iomode()-compatible value describing the
    over-lying whio_dev object's i/o mode. whio_rc.AccessError is
    returned if iomode is zero and a write lock is
    attempted. (iomode<0) is treated as writable b/c FILE proxy
    devices (and possibly others) cannot know their access mode, and
    they have a negative iomode.

    - ctl is the whio_dev ioctl command. Only the following are
    supported:
    
    whio_dev_ioctl_WHIO_LOCK, whio_dev_ioctl_FCNTL_lock_nowait,
    whio_dev_ioctl_FCNTL_lock_wait, whio_dev_ioctl_FCNTL_lock_get.

    - fileno is the file descriptor, which is presumably associated
    with dev.

    - lockObj must be either a populated (whio_lock_request*) if
    (ctl==whio_dev_ioctl_WHIO_LOCK), else it must be a populated
    ((struct flock *), for all other ctl values). It may not be null.

    Returns zero on success, or one of several different errors on error:

    - whio_rc.LockingError = a lock was attempted but fcntl() returned
    an error code.

    - whio_rc.ArgError = one or more of the arguments is not correct (e.g. lockObj
    is null or ctl is out of range).

    - whio_rc.AccessError = iomode is 0 and the request is for a write
    lock.

    If the lock request is a F_GETLK/whio_lock_CMD_TEST and it
    succeeds, the lockObj may be modified:

    - flock::l_type will be changed to F_UNLCK to signify that the
    lock _could_ have been placed but wansn't.

    - whio_lock::command will be changed to whio_lock_CMD_TEST_PASSED
    to signify that the lock _could_ have been placed but wansn't.

    - whio_rc.InterruptedError if EINTR is returned from fcntl() (lock
    was interrupted by a signal).

    Maintenance reminder: this function is used by the whio_dev_FILE
    implementation as well.
*/
int whio_dev_ioctl_fcntl_proxy( short iomode, int fileno, int ctl, void * lockObj )
{
#if ! WHIO_CONFIG_USE_FCNTL
    return whio_rc.UnsupportedError;
#else
    whio_lock_request * wli = NULL;
    struct flock * fl = NULL;
    if( ! lockObj )
    {
        return whio_rc.ArgError;
    }
    else if( whio_dev_ioctl_WHIO_LOCK == ctl )
    {
        wli = (whio_lock_request*)lockObj;
    }
    else if(
            (ctl == whio_dev_ioctl_FCNTL_lock_nowait)
            || (ctl == whio_dev_ioctl_FCNTL_lock_wait)
            || (ctl == whio_dev_ioctl_FCNTL_lock_get)
            )
    {
#if 0
        if( ! fileno )
        {
            return whio_rc.ArgError;
        }
#endif
        fl = (struct flock *)lockObj;
    }
    else
    {
        //assert(0 && "Shouldn't ever happen.");
        return whio_rc.ArgError;
    }
    int rc = 0;
    do
    {
        struct flock flo /* only used if (wli). */;
        int lockCmd = 0;
        if( wli )
        {
            fl = &flo;
            memset(&flo,0,sizeof(flo));
            flo.l_type = (whio_lock_TYPE_UNLOCK == wli->type)
                ? F_UNLCK
                : ((wli->type==whio_lock_TYPE_WRITE) ? F_WRLCK : F_RDLCK)
                ;
            flo.l_start = (off_t)wli->start;
            flo.l_len = (off_t)wli->length;
            flo.l_whence = (short)wli->whence;
            lockCmd = (whio_lock_CMD_TEST == wli->command)
                ? F_GETLK
                : ((wli->command==whio_lock_CMD_SET_WAIT) ? F_SETLKW : F_SETLK);
            if( (wli->whence == SEEK_SET)
                && (wli->start < 0) )
            {
                rc = whio_rc.RangeError;
                break;
            }
        }
        else
        {
            lockCmd = (whio_dev_ioctl_FCNTL_lock_get == ctl)
                ? F_GETLK
                : ((whio_dev_ioctl_FCNTL_lock_nowait == ctl)
                   ? F_SETLK
                   : F_SETLKW
                   )
                ;
        }
        // almost there...
        if( (iomode == 0) /* (iomode<0) _might_ be writable - we can't know without trying. */
            && (F_WRLCK == fl->l_type)
            )
        {
            rc = whio_rc.AccessError;
            break;
        }
        // Finally...
        //retry_lock:
        errno = 0;
        rc = fcntl( fileno, lockCmd, fl );
        if( 0 != rc )
        {
            if( EINTR == errno)
            {
                rc = whio_rc.InterruptedError;
                // arguable: goto retry_lock;
            }
            else if( EINVAL == errno )
            {
                rc = whio_rc.RangeError;
            }
            else
            {
                rc = whio_rc.LockingError;
            }
        }
        else if( wli
                 && (whio_lock_CMD_TEST == wli->command)
                 && (F_UNLCK == fl->l_type) )
        {
            wli->command = whio_lock_CMD_TEST_PASSED;
        }
    } while(0);
    return rc;
#endif /* WHIO_CONFIG_USE_FCNTL */
}

static int whio_dev_fileno_ioctl( whio_dev * dev, int arg, va_list vargs )
{
    WHIO_fileno_DECL(whio_rc.ArgError);
    /**
       The standard ioctl() looks like:

       int ioctl(int d, int request, ...);

       which means there's no way for us to pass our ... directly to
       it.  So it appears to be impossible to emulate the system's
       ioctl() this without literally checking every possible ioctl
       and casting the first ... arg to the proper type (which is
       likely platform-dependent).
    */
    int rc = whio_rc.UnsupportedError;
    switch( arg )
    {
      case whio_dev_ioctl_FILE_fd:
	  rc = whio_rc.OK;
	  *(va_arg(vargs,int*)) = f->fileno;
	  break;
      case whio_dev_ioctl_GENERAL_name:
	  do
	  {
	      char const ** cpp = (va_arg(vargs,char const **));
	      if( cpp )
	      {
		  rc = whio_rc.OK;
		  *cpp = f->filename;
	      }
	      else
	      {
		  rc = whio_rc.ArgError;
	      }
	  } while(0);
	  break;
      case whio_dev_ioctl_FILE_REMOVE:
          if( f->filename && *f->filename )
          {
              errno = 0;
              rc = remove(f->filename);
              if(rc)
              {
                  switch(errno)
                  {
                    case ENOTDIR:
                    case ELOOP:
                    case ENOENT:
                    case ENAMETOOLONG: rc = whio_rc.RangeError;
                        break;
                    case EISDIR:
                    case EPERM:
                    case EROFS:
                    case EACCES: rc = whio_rc.AccessError;
                        break;
                    case EBUSY: rc = whio_rc.LockingError;
                        break;
                    case EIO: rc = whio_rc.IOError;
                        break;
                    case ENOMEM: rc = whio_rc.AllocError;
                        break;
                    default:
                        rc = whio_rc.InternalError;
                        break;
                  };
              }
          }
          else
          {
              rc = whio_rc.UnsupportedError;
          }
          break;
#if WHIO_CONFIG_USE_FCNTL
      case whio_dev_ioctl_mask_FCNTL_LOCKING:
      case whio_dev_ioctl_mask_WHIO_LOCKING:
          rc = whio_rc.OK;
          break;
      case whio_dev_ioctl_FCNTL_lock_nowait:
      case whio_dev_ioctl_FCNTL_lock_wait:
      case whio_dev_ioctl_FCNTL_lock_get:
      case whio_dev_ioctl_WHIO_LOCK:
          rc = whio_dev_ioctl_fcntl_proxy( f->iomode,
                                           f->fileno,
                                           arg,
                                           va_arg(vargs,void *) );
          break;
#endif
      default: break;
    };
    return rc;
}

static bool whio_dev_fileno_close( whio_dev * dev )
{
    WHIO_fileno_DECL(false);
    dev->api->flush(dev);
    if( dev->client.dtor ) dev->client.dtor( dev->client.data );
    *dev = whio_dev_empty;
    if( f->fp )
    {
        fclose( f->fp );
        f->fp = NULL;
        f->fileno = -1;
    }
    else if( f->fileno >= 0 )
    {
        close( f->fileno );
        f->fileno = -1;
    }
    if( dev != &f->devMem )
    { /* user supplied dev to the factory. Free only our parts. */
        whio_dev_fileno_free(f);
        *dev = whio_dev_empty;
    }
    else
    {
        /* we will Free it in finalize(). */
    }
    return true;
}

static void whio_dev_fileno_finalize( whio_dev * dev )
{
    WHIO_fileno_DECL();
    whio_dev const * old = &f->devMem;
    dev->api->close( dev );
    if( old == dev )
    { /** dev is allocated as part of f.  */
        whio_dev_fileno_free(f);
    }
    else
    {
        /* if the client is following the API, he'll call close(),
           which will free f.
        */
    }
          
}

#undef WHIO_fileno_DECL

static const whio_dev_api whio_dev_fileno_api =
    {
    whio_dev_fileno_read,
    whio_dev_fileno_write,
    whio_dev_fileno_close,
    whio_dev_fileno_finalize,
    whio_dev_fileno_error,
    whio_dev_fileno_clear_error,
    whio_dev_fileno_eof,
    whio_dev_fileno_tell,
    whio_dev_fileno_seek,
    whio_dev_fileno_flush,
    whio_dev_fileno_trunc,
    whio_dev_fileno_ioctl,
    whio_dev_fileno_iomode
    };

static const whio_dev whio_dev_fileno_empty =
    {
    &whio_dev_fileno_api,
    { /* impl */
    0, /* data. Must be-a (whio_dev_fileno*) */
    (void const *)&whio_dev_fileno_meta_empty /* typeID */
    }
    };

/**
   Implementation for whio_dev_for_fileno() and whio_dev_for_filename().

   If fname is 0 or empty then fdopen(fileno,mode) is used, otherwise
   fopen(fname,mode) is used.
*/
static whio_dev * whio_dev_for_file_impl( char const * fname, int filenum, char const * mode )
{
    if( ((!fname || !*fname) && (filenum<1)) || (!mode || !*mode) )
    {
        return NULL;
    }
    const whio_size_t nameLen = (fname && *fname)
        ? strlen(fname)
        : 0;
    whio_dev_fileno * meta = whio_dev_fileno_alloc( nameLen ? (nameLen+2) : 0 );
    if( ! meta ) return NULL;
    /** Maintenance reminder:

        i would like to move the alloc to below the fopen(), but if we
        open the file first and alloc fails then we have to check
        whether we created the file, and delete it if we did not.
    */
    whio_dev * dev = meta->dev = &meta->devMem;
    FILE * f = nameLen ? fopen(fname,mode) : fdopen( filenum, mode );
    if( ! f )
    {
        whio_dev_fileno_free(meta);
	return NULL;
    }
    *dev = whio_dev_fileno_empty;
    dev->impl.data = meta;
    meta->fp = f;
    meta->fileno = fileno(f);
    if( nameLen )
    {
        assert( meta->filename && "meta->filename should have been set by allocator!" );
        memcpy( meta->filename, fname, nameLen );
        meta->filename[nameLen] = 0;
    }
    meta->iomode = whio_mode_to_iomode( mode );
    return dev;
}

whio_dev * whio_dev_for_fileno( int fileno, char const * mode )
{
    return whio_dev_for_file_impl( 0, fileno, mode );
}

#if 1 /* there is a separate implementation in whio_dev_FILE.c, but the API
	 docs describe this one. */
whio_dev * whio_dev_for_filename( char const * fname, char const * mode )
{
    return (fname && *fname)
        ? whio_dev_for_file_impl( fname, -1, mode )
        : NULL;
}
#endif


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int whio_dev_posix_open2( whio_dev ** tgt, char const * fname,
                          enum whio_iomodes flags, short filePerms )
{
    if( !tgt || !fname || !*fname
        || (WHIO_MODE_INVALID == flags)
        )
    {
        return whio_rc.ArgError;
    }
    else if( ! filePerms )
    {
        filePerms = 0644;
    }
    const whio_size_t nameLen = strlen(fname);
    whio_dev_fileno * meta = whio_dev_fileno_alloc( (nameLen+2) );
    if( ! meta ) return whio_rc.AllocError;
    /** Maintenance reminder:

        i would like to move the alloc to below the fopen(), but if we
        open the file first and alloc fails then we have to check
        whether we created the file, and delete it if we did not.
    */
    int omode = 0;
    if( flags & WHIO_MODE_RW )
    {
        
        omode |= O_RDWR;
        meta->iomode = 1;
    }
    else if( flags & WHIO_MODE_WO )
    {
        
        omode |= O_WRONLY;
        meta->iomode = 1;
    }
    else
    {
        omode |= O_RDONLY;
        meta->iomode = 0;
    }

    if( meta->iomode < 0 )
    {
        whio_dev_fileno_free(meta);
        return whio_rc.RangeError;
    }
    
    if( flags & WHIO_MODE_FLAG_FAIL_IF_EXISTS )
    {
        omode |= O_EXCL;
    }
    if( flags & WHIO_MODE_FLAG_TRUNCATE )
    {
        omode |= O_TRUNC;
    }
    if( flags & WHIO_MODE_FLAG_CREATE )
    {
        omode |= O_CREAT;
    }
    whio_dev * dev = meta->dev = (*tgt ? *tgt : &meta->devMem);
    *dev = whio_dev_fileno_empty;
    dev->impl.data = meta;
    int fn = open( fname, omode, filePerms );
    if( fn < 0 )
    {
        whio_dev_fileno_free(meta);
        switch( errno )
        {
          case EACCES:
          case EEXIST:
          case EFAULT:
          case EISDIR:
          case EPERM:
          case EROFS:
          case ETXTBSY:
          case ENOTDIR:
              return whio_rc.AccessError;
          case EWOULDBLOCK:
              return whio_rc.LockingError; /* arguably AccessError */
          case EINTR:
              return whio_rc.InterruptedError;
          case ELOOP:
          case EMFILE:
          case ENFILE:
          case EOVERFLOW:
          case EFBIG: /* linux pre-2.6.24 */
          case ENAMETOOLONG:
              return whio_rc.RangeError;
          case ENODEV:
          case ENXIO:
          case ENOENT:
              return whio_rc.NotFoundError;
          case ENOMEM:
              return whio_rc.AllocError;
          default:
              return whio_rc.IOError;
        };
    }
    meta->fileno = fn;
    assert( meta->filename && "meta->filename should have been set by allocator!" );
    memcpy( meta->filename, fname, nameLen );
    meta->filename[nameLen] = 0;
    if( ! *tgt ) *tgt = dev;
    return whio_rc.OK;
}

#undef whio_dev_fileno_empty_m
/* end file src/whio_dev_fileno.c */
/* begin file src/whio_dev_mem.c */
#line 8 "src/whio_dev_mem.c"
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/
/************************************************************************
Implementations for whio_dev types:

- one for a dynamically allocated in-memory buffer, with the option to
grow the buffer on demand.

- one for existing memory ranges, with read-only or read-write access.

They comply with the whio_dev interface, and all
implementation-specified behaviours of the interface are documented
along with the factory functions for creating the device objects.

************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memcpy() */
#include <stdint.h>

/**
   Internal implementation details for the whio_dev memory
   buffer wrapper.
*/
typedef struct whio_dev_membuf_meta
{
    /**
       Size of the buffer (i.e. position of EOF).
    */
    whio_size_t size;
    /**
       Bytes allocated in the buffer. May be larger than size.
    */
    whio_size_t alloced;
    /**
       The buffer itself.
    */
    unsigned char * buffer; /* is unsigned necessary here? */
    /**
       Current position within the buffer.
    */
    whio_size_t pos;
    /**
       If true, the buffer will be grown as needed.
    */
    bool expandable;
    /**
       By how much do we want to expand when we grow?

       values <= 1.0 mean do not expand.

       values >1.0 mean:

       - when expanding, expand by (amount * factor).

       - when shrinking, only release memory if (amount / factor) is
       greater than the amount requested (that is, if we shrunk
       enough).
    */
    float expfactor;
} whio_dev_membuf_meta;


#define WHIO_DEV_MEMBUF_META_INIT { \
    0, /* size */ \
    0, /* alloced */ \
    0, /* buffer */ \
    0, /* pos */ \
    false, /* expandable */ \
    1.5 /* expfactor */ \
    }
/**
   Initialization object for new whio_dev_membuf objects.
*/
static const whio_dev_membuf_meta whio_dev_membuf_meta_empty = WHIO_DEV_MEMBUF_META_INIT;

#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whio_dev_membuf_meta_alloc_slots object.
*/
whio_dev_membuf_meta_alloc_count = 5
};
static struct
{
    whio_dev_membuf_meta objs[whio_dev_membuf_meta_alloc_count];
    char used[whio_dev_membuf_meta_alloc_count];
} whio_dev_membuf_meta_alloc_slots = { {WHIO_DEV_MEMBUF_META_INIT}, {0} };
#endif

static whio_dev_membuf_meta * whio_dev_membuf_meta_alloc()
{
    whio_dev_membuf_meta * obj = 0;
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whio_dev_membuf_meta_alloc_count; ++i )
    {
	if( whio_dev_membuf_meta_alloc_slots.used[i] ) continue;
	whio_dev_membuf_meta_alloc_slots.used[i] = 1;
	whio_dev_membuf_meta_alloc_slots.objs[i] = whio_dev_membuf_meta_empty;
	obj = &whio_dev_membuf_meta_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_membuf_meta *) whio_malloc( sizeof(whio_dev_membuf_meta) );
    return obj;
}

static void whio_dev_membuf_meta_free( whio_dev_membuf_meta * obj )
{
    if( ! obj ) return;
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_dev_membuf_meta_alloc_slots.objs[0]) ||
	(obj > &whio_dev_membuf_meta_alloc_slots.objs[whio_dev_membuf_meta_alloc_count-1]) )
    { /* it does not belong to us */
	whio_free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whio_dev_membuf_meta_alloc_slots.objs[0]);
	whio_dev_membuf_meta_alloc_slots.objs[ndx] = whio_dev_membuf_meta_empty;
	whio_dev_membuf_meta_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    whio_free(obj);
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
}


/**
   A helper for the whio_dev_membuf API. Requires that the 'dev'
   parameter be-a whio_dev and that that device is-a whio_dev_membuf.
 */
#define WHIO_MEMBUF_DECL(RV) whio_dev_membuf_meta * mb = (dev ? (whio_dev_membuf_meta*)dev->impl.data : 0); \
    if( !mb  || ((void const *)&whio_dev_api_membuf != dev->impl.typeID) ) return RV

static whio_size_t whio_dev_membuf_read( whio_dev * dev, void * dest, whio_size_t n )
{
    WHIO_MEMBUF_DECL(0); //whio_rc.SizeTError);
    if( ! dest ) return 0; //whio_rc.SizeTError;
    if( mb->pos >= mb->size ) return 0;
    whio_size_t rlen = n;
    if( ((mb->pos + n) >= mb->size )
	|| ((mb->pos + n) < mb->pos)
	)
    {
	rlen = mb->size - mb->pos;
    }
    if( rlen )
    {
	memcpy( dest, mb->buffer + mb->pos, rlen );
	mb->pos += rlen;
    }
    return rlen;
}

static whio_size_t whio_dev_membuf_write( whio_dev * dev, void const * src, whio_size_t n )
{
    WHIO_MEMBUF_DECL(0);
    if( ! n || !src ) return 0;
    whio_size_t newEnd = mb->pos + n;
    if( newEnd < mb->pos ) return 0; /* overflow! fixme: write as much as we can */
    whio_size_t wlen = n;

    if( newEnd >= mb->size )
    {
	//WHIO_DEBUG("write of %u bytes to pos %u would go out of bounds (%u). Expanding==%d\n",n, mb->pos, mb->size,mb->expandable);
	if( mb->expandable )
	{
	    dev->api->truncate( dev, newEnd );
	    /* ignore possible trunc failure and do a short write. */
	}
	else
	{
	    mb->size = (newEnd > mb->alloced)
		? mb->alloced
		: newEnd;
	}
    }

    if( mb->pos >= mb->size )
    {
	/*
	  We likely got seek()ed out of bounds. Behave like FILE does
	  on my Linux box and resize now...
	*/
	//WHIO_DEBUG("pos(%u) > size(%u). Expanding to %u = %s\n", mb->pos, mb->size, newEnd, mb->expandable ? "yes" : "no");
	if( ! mb->expandable ) return 0;
	//WHIO_DEBUG("Seems we've been truncated from %u to %u\n",mb->size, mb->pos);
	mb->size = newEnd;
    }

    if( (newEnd >= mb->size ) /* will overflow current EOF. */
	//|| (newEnd < mb->pos ) /* overflow in mb->pos+n. */
	)
    {
	/* write as much as we can, to EOF. */
	wlen = mb->size - mb->pos;
    }
    if( wlen )
    {
	memcpy( mb->buffer + mb->pos, src, wlen );
	mb->pos += wlen;
	if( mb->size < mb->pos ) mb->size = mb->pos;
    }
    return wlen;
}

static int whio_dev_membuf_error( whio_dev * dev )
{
    WHIO_MEMBUF_DECL(whio_rc.ArgError);
    return (mb->pos <= mb->size)
	? whio_rc.OK
	: whio_rc.RangeError;
}

static int whio_dev_membuf_clear_error( whio_dev * dev )
{
    return whio_rc.OK;
}

static int whio_dev_membuf_eof( whio_dev * dev )
{
    WHIO_MEMBUF_DECL(whio_rc.ArgError);
    return (mb->pos < mb->size)
	? 0
	: 1;
}

static whio_size_t whio_dev_membuf_tell( whio_dev * dev )
{
    WHIO_MEMBUF_DECL(whio_rc.SizeTError);
    return mb->pos;
}

static whio_size_t whio_dev_membuf_seek( whio_dev * dev, whio_off_t pos, int whence )
{
    WHIO_MEMBUF_DECL(whio_rc.SizeTError);
    whio_size_t too = mb->pos;
    switch( whence )
    {
      case SEEK_SET:
	  if( pos < 0 ) return whio_rc.SizeTError;
	  too = (whio_size_t)pos;
	  break;
      case SEEK_END:
	  too = mb->size + pos;
	  if( (pos>0) && (too < mb->size) )  /* overflow! */ return whio_rc.SizeTError;
	  else if( (pos<0) && (too > mb->size) )  /* underflow! */ return whio_rc.SizeTError;
	  break;
      case SEEK_CUR:
	  too += pos;
	  if( (pos>0) && (too < mb->pos) )  /* overflow! */ return whio_rc.SizeTError;
	  else if( (pos<0) && (too > mb->pos) )  /* underflow! */ return whio_rc.SizeTError;
	  break;
      default:
	  return whio_rc.SizeTError;
	  break;
    };
    /** We defer any actual expansion until the next write. */
    return (mb->pos = too);
}

static int whio_dev_membuf_flush( whio_dev * dev )
{
    return whio_rc.OK;
}

static int whio_dev_membuf_trunc( whio_dev * dev, whio_off_t _len )
{
    WHIO_MEMBUF_DECL(whio_rc.ArgError);
    if( _len < 0 ) return whio_rc.RangeError;
    whio_size_t ulen = (whio_size_t)_len;
    if( 0 == ulen )
    {
#if 1 /* arguable. Hmmm. */
	if( mb->expandable )
	{
	    /**
	       We only do this for expanding buffers because otherwise
	       we could no longer write to the buffer (as we can't expand
	       it).
	    */
	    free( mb->buffer );
	    mb->buffer = 0;
	    mb->alloced = 0;
	}
#endif
	mb->size = 0;
	return 0;
    }
    if( !mb->alloced || (ulen > mb->alloced) )
    { /* try to grow */
	whio_size_t alen = ulen;
	if( mb->expandable )
	{ /* see how much to expand by. */
	    alen = (whio_size_t)(mb->alloced * (mb->expfactor+0.01));
	    /* ^^^ that +0.01 kludge is to work around (100*1.8)==179 and (100*1.9)==189 */
	    if( alen < ulen ) alen = ulen;
	}
	void * b = realloc( mb->buffer, alen );
	if( ! b ) return whio_rc.AllocError;
	mb->buffer = b;
	//WHIO_DEBUG("Grew buffer from %u to %u bytes\n", mb->alloced, alen);
	if( mb->alloced < alen )
	{   /* clean up new memory to avoid RAM artifacts. */
	    memset( WHIO_VOID_PTR_ADD(b,mb->alloced), 0, alen - mb->alloced );
	}
	mb->alloced = alen;
	mb->size = ulen;
	return whio_rc.OK;
    }
    if( mb->expandable && (mb->alloced > ulen) )
    {	/**
	   Try to shrink...

	   We only do this for expanding buffers because otherwise
	   we could no longer write to the buffer (as we can't expand
	   it).
	*/
	//const whio_size_t oldAlloc = mb->alloced;
	//WHIO_DEBUG("oldAlloc=%u mb->alloced=%u ulen=%u\n",oldAlloc,mb->alloced,ulen);
	whio_size_t alen = ulen;
	if( alen < (mb->alloced/mb->expfactor) )
	{
	    void * b = realloc( mb->buffer, alen );
	    if( b )
	    {
		mb->buffer = b;
		mb->alloced = alen;
		//WHIO_DEBUG("Shrunk buffer from %u to %u bytes\n", oldAlloc, mb->alloced);
	    }
	    /* ignore realloc failure if we're shrinking - just keep the old block. */
	}
    }
    mb->size = ulen;
    return whio_rc.OK;
}

short whio_dev_membuf_iomode( whio_dev * dev )
{
    WHIO_MEMBUF_DECL(-1);
    return 1;
}

static int whio_dev_membuf_ioctl( whio_dev * dev, int arg, va_list vargs )
{
    int rc = whio_rc.UnsupportedError;
    WHIO_MEMBUF_DECL(rc);
    whio_size_t * x = 0;
    unsigned char const ** cp = 0;
    switch( arg )
    {
      case whio_dev_ioctl_BUFFER_uchar_ptr:
          cp = va_arg(vargs,unsigned char const **);
          if( cp )
          {
              rc = whio_rc.OK;
              *cp = mb->buffer;
          }
          else
          {
              rc = whio_rc.ArgError;
          }
	  break;
      case whio_dev_ioctl_GENERAL_size:
          x = va_arg(vargs,whio_size_t*);
          if( x )
          {
              rc = whio_rc.OK;
              *x = mb->size;
          }
          else
          {
              rc = whio_rc.ArgError;
          }
	  break;
      case whio_dev_ioctl_BUFFER_size:
          x = va_arg(vargs,whio_size_t*);
          if( x )
          {
              rc = whio_rc.OK;
              *x = mb->alloced;
          }
          else
          {
              rc = whio_rc.ArgError;
          }
	  break;
      default:
          break;
    };
    return rc;
}

static bool whio_dev_membuf_close( whio_dev * dev );
static void whio_dev_membuf_finalize( whio_dev * dev );
    
const whio_dev_api whio_dev_api_membuf =
    {
    whio_dev_membuf_read,
    whio_dev_membuf_write,
    whio_dev_membuf_close,
    whio_dev_membuf_finalize,
    whio_dev_membuf_error,
    whio_dev_membuf_clear_error,
    whio_dev_membuf_eof,
    whio_dev_membuf_tell,
    whio_dev_membuf_seek,
    whio_dev_membuf_flush,
    whio_dev_membuf_trunc,
    whio_dev_membuf_ioctl,
    whio_dev_membuf_iomode
    };

#define WHIO_DEV_MEMBUF_INIT { \
    &whio_dev_api_membuf, \
    { /* impl */ \
    0, /* data. Must be-a (whio_dev_membuf*) */ \
    (void const *)&whio_dev_api_membuf /* typeID */ \
    } }

static const whio_dev whio_dev_membuf_empty = WHIO_DEV_MEMBUF_INIT;

static bool whio_dev_membuf_close( whio_dev * dev )
{
    if( dev )
    {
	if( dev->client.dtor ) dev->client.dtor( dev->client.data );
	dev->client = whio_client_data_empty;
	whio_dev_membuf_meta * f = (whio_dev_membuf_meta*)dev->impl.data;
	if( f )
	{
	    dev->impl.data = 0;
	    free(f->buffer);
	    /**f = whio_dev_membuf_meta_empty;*/
	    whio_dev_membuf_meta_free( f );
	    return true;
	}
        *dev = whio_dev_membuf_empty;
    }
    return false;
}

static void whio_dev_membuf_finalize( whio_dev * dev )
{
    if( dev )
    {
	dev->api->close(dev);
	whio_dev_free(dev);
    }
}


#undef WHIO_MEMBUF_DECL

whio_dev * whio_dev_for_membuf( whio_size_t size, float expFactor )
{
    /*
      FIXME: add a whio_dev object to mb and return a pointer to that
      object, to save us an allocation.
    */
    whio_dev * dev = whio_dev_alloc();
    if( ! dev ) return 0;
    *dev = whio_dev_membuf_empty;
    whio_dev_membuf_meta * mb = whio_dev_membuf_meta_alloc();
    if( !mb )
    {
	whio_dev_free(dev);
	return 0;
    }
    *mb = whio_dev_membuf_meta_empty;
    dev->impl.data = mb;
    //mb->alloc_policy = expandable ? whio_dev_membuf_alloc_policy_133 : 0;
    mb->expandable = (expFactor >= 1.0);
    mb->expfactor = expFactor;
    int rc = dev->api->truncate( dev, size );
    if( whio_rc.OK != rc )
    {
	dev->api->finalize( dev );
	dev = 0;
    }
    //WHIO_DEBUG( "membuf @%p, buffer @%p: size=%u\n", (void const *)dev, (void const *)mb->buffer, mb->size );
    return dev;
}


/**
   Internal implementation details for the whio_dev memmap wrapper. It
   wraps a client-supplied memory range in the whio_dev interface.
*/
typedef struct whio_dev_memmap
{
    /**
       Size of the buffer (i.e. position of EOF).
    */
    whio_size_t size;
    /**
       The maximum size of the buffer. This starts out the same as
       size, but truncate() can shrink the effective size of the
       buffer.  We keep the largest size so that we can re-truncate()
       after a truncate() shrinks the range (but we can't grow larger
       than this value).
    */
    whio_size_t maxsize;

    /**
       Current position within the buffer.
    */
    whio_size_t pos;

    /**
       The memory buffer itself, in read/write form.
       For read-only streams this is 0.
    */
    void * rw;

    /**
       For read/write streams, this is the same as rw but const.  For
       read-only streams it is the start of the wrapped memory range.
    */
    void const * ro;
    /**
       We have this in order to save us one malloc() in the
       whio_dev_for_memmap() factories.
    */
    whio_dev devMem;
} whio_dev_memmap;

/**
   Initialization object for new whio_dev_memmap objects.
*/
#define WHIO_DEV_MEMMAP_INIT { \
    0, /* size */ \
    0, /* maxsize */ \
    0, /* pos */ \
    0, /* rw */ \
    0, /* ro */                             \
    whio_dev_empty_m/*devMem*/ \
    }

static const whio_dev_memmap whio_dev_memmap_empty = WHIO_DEV_MEMMAP_INIT;


#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whio_dev_memmap_alloc_slots object.
*/
whio_dev_memmap_alloc_count = 5
};
static struct
{
    whio_dev_memmap objs[whio_dev_memmap_alloc_count];
    char used[whio_dev_memmap_alloc_count];
} whio_dev_memmap_alloc_slots = { {WHIO_DEV_MEMMAP_INIT}, {0} };
#endif

static whio_dev_memmap * whio_dev_memmap_alloc()
{
    whio_dev_memmap * obj = 0;
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whio_dev_memmap_alloc_count; ++i )
    {
	if( whio_dev_memmap_alloc_slots.used[i] ) continue;
	whio_dev_memmap_alloc_slots.used[i] = 1;
	whio_dev_memmap_alloc_slots.objs[i] = whio_dev_memmap_empty;
	obj = &whio_dev_memmap_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_memmap *) whio_malloc( sizeof(whio_dev_memmap) );
    return obj;
}

static void whio_dev_memmap_free( whio_dev_memmap * obj )
{
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_dev_memmap_alloc_slots.objs[0]) ||
	(obj > &whio_dev_memmap_alloc_slots.objs[whio_dev_memmap_alloc_count-1]) )
    { /* it does not belong to us */
	whio_free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whio_dev_memmap_alloc_slots.objs[0]);
	whio_dev_memmap_alloc_slots.objs[ndx] = whio_dev_memmap_empty;
	whio_dev_memmap_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    whio_free(obj);
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
}


/**
   A helper for the whio_dev_memmap API. Requires that the 'dev'
   parameter be-a whio_dev and that that device is-a
   whio_dev_memmap. Declares the local variable (whio_dev_memmap * mb)
   - the internal device data. On error (null argument or type
   mismatch) it calls (return RV).
 */
#define WHIO_MEMMAP_DECL(RV) whio_dev_memmap * mb = (dev ? (whio_dev_memmap*)dev->impl.data : 0); \
    if( !mb  || ((void const *)&whio_dev_api_memmap != dev->impl.typeID) ) return RV

static whio_size_t whio_dev_memmap_read( whio_dev * dev, void * dest, whio_size_t n )
{
    WHIO_MEMMAP_DECL(0); //whio_rc.SizeTError);
    if( ! dest || !mb->ro ) return 0; //whio_rc.SizeTError;
    if( mb->pos >= mb->size ) return 0;
    whio_size_t rlen = n;
    if( ((mb->pos + n) >= mb->size )
	|| ((mb->pos + n) < mb->pos)
	)
    {
	rlen = mb->size - mb->pos;
    }
    if( rlen )
    {
	memcpy( dest, WHIO_VOID_CPTR_ADD(mb->ro,mb->pos), rlen );
	mb->pos += rlen;
    }
    return rlen;
}

static whio_size_t whio_dev_memmap_write( whio_dev * dev, void const * src, whio_size_t n )
{
    WHIO_MEMMAP_DECL(0);
    if( ! n || !src || !mb->rw ) return 0;
    if( mb->pos >= mb->size )
    {
	//WHIO_DEBUG("write would go out of bounds.\n");
	return 0;
    }
    const whio_size_t newEnd = mb->pos + n;
    whio_size_t wlen = n;
    if( (newEnd >= mb->size ) /* would overflow EOF. */
	|| (newEnd < mb->pos ) /* overflow in mb->pos+n. */
	)
    {
	/* write as much as we can, to EOF. */
	wlen = mb->size - mb->pos;
    }
    if( wlen )
    {
	memcpy( WHIO_VOID_PTR_ADD(mb->rw,mb->pos), src, wlen );
	mb->pos += wlen;
    }
    return wlen;
}

static int whio_dev_memmap_error( whio_dev * dev )
{
    WHIO_MEMMAP_DECL(whio_rc.ArgError);
    return (mb->ro && (mb->pos <= mb->size))
	? whio_rc.OK
	: whio_rc.RangeError;
}

static int whio_dev_memmap_clear_error( whio_dev * dev )
{
    return whio_rc.OK;
}


static int whio_dev_memmap_eof( whio_dev * dev )
{
    WHIO_MEMMAP_DECL(whio_rc.ArgError);
    return (mb->pos < mb->size)
	? 0
	: 1;
}

static whio_size_t whio_dev_memmap_tell( whio_dev * dev )
{
    WHIO_MEMMAP_DECL(whio_rc.SizeTError);
#if 0
    return (mb->pos <= mb->size )
	? mb->pos
	: whio_rc.SizeTError;
#else
    return mb->pos;
#endif
}

static whio_size_t whio_dev_memmap_seek( whio_dev * dev, whio_off_t pos, int whence )
{
    WHIO_MEMMAP_DECL(whio_rc.SizeTError);
    whio_size_t too = mb->pos;
    switch( whence )
    {
      case SEEK_SET:
	  if( pos < 0 ) return whio_rc.SizeTError;
	  too = (whio_size_t)pos;
	  break;
      case SEEK_END:
	  too = mb->size + pos;
	  if( (pos>0) && (too < mb->size) )  /* overflow! */ return whio_rc.SizeTError;
	  else if( (pos<0) && (too > mb->size) )  /* underflow! */ return whio_rc.SizeTError;
	  break;
      case SEEK_CUR:
	  too += pos;
	  if( (pos>0) && (too < mb->pos) )  /* overflow! */ return whio_rc.SizeTError;
	  else if( (pos<0) && (too > mb->pos) )  /* underflow! */ return whio_rc.SizeTError;
	  break;
      default:
	  return whio_rc.SizeTError;
	  break;
    };
    return (mb->pos = too);
}

static int whio_dev_memmap_flush( whio_dev * dev )
{
    return whio_rc.OK;
}

static int whio_dev_memmap_trunc( whio_dev * dev, whio_off_t _len )
{
    WHIO_MEMMAP_DECL(whio_rc.ArgError);
    if( _len < 0 ) return whio_rc.RangeError;
    whio_size_t ulen = (whio_size_t)_len;
    if( ulen > mb->maxsize )
    {
	return whio_rc.RangeError;
    }
    mb->size = ulen;
    return whio_rc.OK;
}

short whio_dev_memmap_iomode( whio_dev * dev )
{
    WHIO_MEMMAP_DECL(-1);
    return (NULL == mb->rw) ? 0 : 1;
}

static int whio_dev_memmap_ioctl( whio_dev * dev, int arg, va_list vargs )
{
    int rc = whio_rc.UnsupportedError;
    WHIO_MEMMAP_DECL(rc);
    whio_size_t * x = 0;
    switch( arg )
    {
      case whio_dev_ioctl_GENERAL_size:
          x = va_arg(vargs,whio_size_t*);
          if( x )
          {
              rc = whio_rc.OK;
              *x = mb->size;
          }
          else
          {
              rc = whio_rc.ArgError;
          }
	  break;
      case whio_dev_ioctl_BUFFER_size:
          x = va_arg(vargs,whio_size_t*);
          if( x )
          {
              rc = whio_rc.OK;
              *x = mb->maxsize;
          }
          else
          {
              rc = whio_rc.ArgError;
          }
	  break;
      default:
          break;
    };
    return rc;
}

static bool whio_dev_memmap_close( whio_dev * dev )
{
    if( dev )
    {
	if( dev->client.dtor ) dev->client.dtor( dev->client.data );
	dev->client = whio_client_data_empty;
	whio_dev_memmap * f = (whio_dev_memmap*)dev->impl.data;
	if( f )
	{
	    dev->impl.data = 0;
	    *f = whio_dev_memmap_empty;
            /* dev lives IN f, so we free it in finalize() */ 
	    return true;
	}
    }
    return false;
}

static void whio_dev_memmap_finalize( whio_dev * dev )
{
    if( dev )
    {
	whio_dev_memmap * f = (whio_dev_memmap*)dev->impl.data;
	dev->api->close( dev );
        if( f )
	{
            /** dev lives in f */
            whio_dev_memmap_free(f);
        }
    }
}



int whio_dev_memmap_remap( whio_dev * dev, void * rw, void const * ro, whio_size_t size )
{
    if( ! dev ) return whio_rc.ArgError;
    else if( rw && ro && (ro != rw) ) return whio_rc.RangeError;
    else
    {
        WHIO_MEMMAP_DECL(whio_rc.TypeError);
        mb->rw = rw;
        mb->ro = ro;
        if( !ro && !rw ) size = 0;
        mb->size = size;
        return 0;
    }
}


#undef WHIO_MEMMAP_DECL

const whio_dev_api whio_dev_api_memmap =
    {
    whio_dev_memmap_read,
    whio_dev_memmap_write,
    whio_dev_memmap_close,
    whio_dev_memmap_finalize,
    whio_dev_memmap_error,
    whio_dev_memmap_clear_error,
    whio_dev_memmap_eof,
    whio_dev_memmap_tell,
    whio_dev_memmap_seek,
    whio_dev_memmap_flush,
    whio_dev_memmap_trunc,
    whio_dev_memmap_ioctl,
    whio_dev_memmap_iomode
    };

static const whio_dev whio_dev_memmap_dev_empty =
    {
    &whio_dev_api_memmap,
    { /* impl */
    0, /* data. Must be-a (whio_dev_memmap*) */
    (void const *)&whio_dev_api_memmap /* typeID */
    }
    };

/**
   Creates a new whio_dev wrapper for an existing memory range. The arguments:

   - rw = the read/write memory the device will wrap. Ownership is not changed.
   May be 0, but only if ro is not 0.

   - ro = the read-only memory the device will wrap. Ownership is not changed.
   May be 0, but only if rw is not 0.

   - size = the size of the rw or ro buffer. It is the caller's
   responsibility to ensure that the buffer is at least that
   long. This object will not allow i/o operations outside of that
   bound.

   If both ro and rw are not 0 then they must have the same address. If rw is 0 then
   the device is read-only, and any write operations will fail.

   On success a new whio_dev is returned. On error (invalid arguments,
   alloc error), 0 is returned.

   See whio_dev_for_memmap_rw() and whio_dev_for_memmap_ro() for more details
   about the returned object.
*/
static whio_dev * whio_dev_for_memmap( void * rw, void const * ro, whio_size_t size )
{
    if( (!rw && !ro) || ! size ) return NULL;
    if( (rw && ro) && ( ro != rw ) ) return NULL;
    else
    {
        /*
          FIXME: add a whio_dev object to mb and return a pointer to that
          object, to save us an allocation.
        */
        whio_dev_memmap * mb = whio_dev_memmap_alloc();
        whio_dev * dev = NULL;
        if( !mb )
        {
            return NULL;
        }
        dev = &mb->devMem;
        *mb = whio_dev_memmap_empty;
        *dev = whio_dev_memmap_dev_empty;
        dev->impl.data = mb;
        mb->size = mb->maxsize = size;
        mb->rw = rw;
        mb->ro = ro ? ro : rw;
        //WHIO_DEBUG( "memmap @%p, buffer @%p: size=%u\n", (void const *)dev, (void const *)mb->ro, mb->size );
        return dev;
    }
}

whio_dev * whio_dev_for_memmap_rw( void * mem, whio_size_t size )
{
    return whio_dev_for_memmap( mem, mem, size );
}

whio_dev * whio_dev_for_memmap_ro( const void * mem, whio_size_t size )
{
    return whio_dev_for_memmap( 0, mem, size );
}
/* end file src/whio_dev_mem.c */
/* begin file src/whio_dev_subdev.c */
#line 8 "src/whio_dev_subdev.c"
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/
/************************************************************************
Implementations for a whio_dev type for wrapping a FILE handle.
It complies with the whio_dev interface, and all
implementation-specified behaviours of the interface are documented
along with the factory functions for creating the device objects.
************************************************************************/

#if !defined(_POSIX_C_SOURCE)
/* required for for fileno(), ftello(), maybe others */
#  define _POSIX_C_SOURCE 200112L
//#  define _POSIX_C_SOURCE 199309L
//#  define _POSIX_C_SOURCE 199506L
#endif

#include <stdlib.h>
#include <stdio.h>

#if WHIO_CONFIG_USE_FCNTL
#  include <fcntl.h>
#endif

/**
   Internal implementation details for the whio_dev subdev wrapper.
*/
typedef struct whio_dev_subdev_meta
{
    /**
       Underlying FILE handle. Owned by this
       object.
    */
    whio_dev * dev;
    /**
       Lower bound of device, relative to parent BOF.
    */
    whio_size_t lower;
    /**
       Upper bound of device, relative to parent BOF. Use 0
       for "no bound".

       In hindsight, we should probably store the length instead of
       the upper bound.
    */
    whio_size_t upper;
    /**
       Current cursor pos, relative to parent BOF.

       In hindsight, this should probably be stored relative to the
       subdevice.
    */
    whio_size_t pos;
} whio_dev_subdev_meta;

/**
   Initialization object for whio_dev_subdev objects. Also used as
   whio_dev::typeID for such objects.
*/
#define WHIO_DEV_SUBDEV_META_INIT { \
    0, /* dev */ \
    0, /* lower */ \
    0, /* upper */ \
    0 /* pos */ \
    }
static const whio_dev_subdev_meta whio_dev_subdev_meta_empty = WHIO_DEV_SUBDEV_META_INIT;

#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whio_dev_subdev_meta_alloc_slots object.
*/
whio_dev_subdev_meta_alloc_count = 6
};
static struct
{
    whio_dev_subdev_meta objs[whio_dev_subdev_meta_alloc_count];
    char used[whio_dev_subdev_meta_alloc_count];
} whio_dev_subdev_meta_alloc_slots = { {WHIO_DEV_SUBDEV_META_INIT}, {0} };
#endif

whio_dev_subdev_meta * whio_dev_subdev_meta_alloc()
{
    whio_dev_subdev_meta * obj = 0;
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whio_dev_subdev_meta_alloc_count; ++i )
    {
	if( whio_dev_subdev_meta_alloc_slots.used[i] ) continue;
	whio_dev_subdev_meta_alloc_slots.used[i] = 1;
	whio_dev_subdev_meta_alloc_slots.objs[i] = whio_dev_subdev_meta_empty;
	obj = &whio_dev_subdev_meta_alloc_slots.objs[i];
	//WHIO_DEBUG("Allocated device #%u @0x%p\n", i, (void const *)obj );
	break;
    }
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_subdev_meta *) whio_malloc( sizeof(whio_dev_subdev_meta) );
    return obj;
}

void whio_dev_subdev_meta_free( whio_dev_subdev_meta * obj )
{
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_dev_subdev_meta_alloc_slots.objs[0]) ||
	(obj > &whio_dev_subdev_meta_alloc_slots.objs[whio_dev_subdev_meta_alloc_count-1]) )
    { /* it does not belong to us */
	whio_free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whio_dev_subdev_meta_alloc_slots.objs[0]);
	if( 0 )
	{
	    WHIO_DEBUG("Address range = 0x%p to 0x%p, ndx=%u\n",
		       (void const *)&whio_dev_subdev_meta_alloc_slots.objs[0],
		       (void const *)&whio_dev_subdev_meta_alloc_slots.objs[whio_dev_subdev_meta_alloc_count-1],
		       ndx
		       );
	    WHIO_DEBUG("Freeing object @0x%p from static pool index %u (@0x%p)\n",
		       (void const *)obj,
		       ndx,
		       (void const *)&whio_dev_subdev_meta_alloc_slots.objs[ndx] );
	}

	whio_dev_subdev_meta_alloc_slots.objs[ndx] = whio_dev_subdev_meta_empty;
	whio_dev_subdev_meta_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    whio_free(obj);
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
}


/**
   A helper for the whio_dev_subdev API. Requires that the 'dev'
   parameter be-a whio_dev and that that device is-a whio_dev_subdev.
 */
#define WHIO_subdev_DECL(RV) whio_dev_subdev_meta * sub = (dev ? (whio_dev_subdev_meta*)dev->impl.data : 0); \
    if( !sub || ((void const *)&whio_dev_subdev_meta_empty != dev->impl.typeID) || (!sub->dev)) return RV


static whio_size_t whio_dev_subdev_read( whio_dev * dev, void * dest, whio_size_t n )
{
    WHIO_subdev_DECL(whio_rc.SizeTError);
    if( (sub->pos < sub->lower) || (sub->pos >= sub->upper ) ) return 0;
    const whio_size_t opos = sub->dev->api->tell( sub->dev );
    if( whio_rc.SizeTError == opos ) return 0;
    if( sub->pos != sub->dev->api->seek( sub->dev, sub->pos, SEEK_SET ) ) return 0;
    whio_size_t rend = sub->pos + n;
    if( rend > sub->upper )
    {
	rend = sub->upper;
    }
    whio_size_t rlen = rend - sub->pos;
    whio_size_t rc = 0;
    if( rlen )
    {
	rc = sub->dev->api->read( sub->dev, dest, rlen );
    }
    sub->pos += rc;
    sub->dev->api->seek( sub->dev, opos, SEEK_SET );
    return rc;
}

static whio_size_t whio_dev_subdev_write( whio_dev * dev, void const * src, whio_size_t n )
{
    WHIO_subdev_DECL(0);
    if( (sub->pos < sub->lower) || (sub->pos >= sub->upper ) ) return 0;
    const whio_size_t opos = sub->dev->api->tell( sub->dev );
    if( whio_rc.SizeTError == opos ) return 0;
    if( sub->pos != sub->dev->api->seek( sub->dev, sub->pos, SEEK_SET ) ) return 0;
    whio_size_t rend = sub->pos + n;
    if( rend > sub->upper )
    {
	rend = sub->upper;
    }
    whio_size_t wlen = rend - sub->pos;
    whio_size_t rc = 0;
    if( wlen )
    {
	rc = sub->dev->api->write( sub->dev, src, wlen );
    }
    sub->pos += rc;
    sub->dev->api->seek( sub->dev, opos, SEEK_SET );
    return rc;
}

static int whio_dev_subdev_error( whio_dev * dev )
{
    WHIO_subdev_DECL(whio_rc.ArgError);
    return sub->dev->api->error( sub->dev );
}

static int whio_dev_subdev_clear_error( whio_dev * dev )
{
    WHIO_subdev_DECL(whio_rc.ArgError);
    return sub->dev->api->clear_error( sub->dev );
}

static int whio_dev_subdev_eof( whio_dev * dev )
{
    WHIO_subdev_DECL(whio_rc.ArgError);
    return sub->pos >= sub->upper;
}

static whio_size_t whio_dev_subdev_tell( whio_dev * dev )
{
    WHIO_subdev_DECL(whio_rc.SizeTError);
    if( sub->pos < sub->lower ) return whio_rc.SizeTError;
    else return sub->pos - sub->lower;
}

static whio_size_t whio_dev_subdev_seek( whio_dev * dev, whio_off_t pos, int whence )
{
    WHIO_subdev_DECL(whio_rc.SizeTError);
    whio_size_t too = sub->pos;
    whio_size_t ppos = sub->dev->api->tell( sub->dev );
    whio_size_t top = sub->upper;
#define OVERFLOW return whio_rc.SizeTError
#define UNDERFLOW return whio_rc.SizeTError
    switch( whence )
    {
      case SEEK_SET:
	  if( pos < 0 ) return whio_rc.SizeTError;
	  too = sub->lower + (whio_size_t)pos;
	  if( too < sub->lower ) OVERFLOW;
	  break;
      case SEEK_END:
	  /* This reeks of special-case handling... */
	  if( ! sub->upper )
	  {
	      top = sub->dev->api->seek( sub->dev, 0, SEEK_END );
	      sub->dev->api->seek( sub->dev, ppos, SEEK_SET );
	      if( whio_rc.SizeTError == top ) return top;
	  }
	  else
	  {
	      top = sub->upper;
	  }
	  too = top + pos;
	  if( (pos < 0) && (too > sub->upper ) ) UNDERFLOW;
	  else if( (pos>0) && (too < sub->upper) ) OVERFLOW;
	  break;
      case SEEK_CUR:
	  too += pos;
	  if( (pos < 0) && (too > sub->pos ) ) UNDERFLOW;
	  else if( (pos > 0) && (too < sub->pos) ) OVERFLOW;
	  break;
      default:
	  return whio_rc.SizeTError;
	  break;
    };
#undef OVERFLOW
#undef UNDERFLOW
    const whio_size_t upos = sub->dev->api->seek( sub->dev, (sub->pos = too), SEEK_SET );
    return (upos == sub->pos)
	? (sub->pos - sub->lower)
	: whio_rc.SizeTError;
}

static int whio_dev_subdev_flush( whio_dev * dev )
{
    WHIO_subdev_DECL(whio_rc.ArgError);
    return sub->dev ? sub->dev->api->flush( sub->dev ) : whio_rc.ArgError;
}

short whio_dev_subdev_iomode( whio_dev * dev )
{
    WHIO_subdev_DECL(-1);
    return sub->dev->api->iomode( sub->dev );
}

static int whio_dev_subdev_trunc( whio_dev * dev, whio_off_t len )
{
    return whio_rc.UnsupportedError;
    //WHIO_subdev_DECL(whio_rc.ArgError);
    //return sub->dev->api->truncate( sub->dev, len );
}

static int whio_dev_subdev_ioctl( whio_dev * dev, int arg, va_list vargs )
{
    WHIO_subdev_DECL(whio_rc.ArgError);
    int rc = whio_rc.UnsupportedError;
    whio_size_t * sz = 0;
    switch( arg )
    {
#if WHIO_CONFIG_USE_FCNTL
      case whio_dev_ioctl_FCNTL_lock_nowait:
      case whio_dev_ioctl_FCNTL_lock_wait:
      case whio_dev_ioctl_FCNTL_lock_get:
	  do
	  {
              rc = whio_rc.OK;
	      struct flock * fl = fl = (va_arg(vargs,struct flock *));
              if( ! fl )
              {
                  rc = whio_rc.ArgError;
                  break;
              }
              const int whence = fl->l_whence;
              switch( whence )
              {
                case SEEK_SET:
                    fl->l_start += sub->lower;
                    // FIXME: check for overflow!
                    break;
                case SEEK_CUR:
                    fl->l_whence = SEEK_SET;
                    fl->l_start += sub->pos;
                    // FIXME: check for overflow!
                    break;
                case SEEK_END:
                    if( sub->upper )
                    {
                        fl->l_whence = SEEK_SET;
                        fl->l_start += sub->upper;
                        // FIXME: check for overflow!
                    }
                    else
                    {
                        /**
                           This case might not work properly when
                           sub->dev is another subdevice. ???
                        */
                    }
                    break;
                default:
                    return whio_rc.RangeError;
              };
              /**
                 Reminder: fl->l_len might lead us past sub->upper,
                 but i'm willing to accept that for compatibility with
                 POSIX fcntl() locks, which allows bytes past the end
                 to be locked. However, such behaviour is not 100%
                 appropriate (nor semantically legal) for all
                 subdevices, and it is up to the client to be sure he
                 doesn't unduly lock neighboring subdevices if his use
                 case relies on that for correct behaviour.
              */
              if( (SEEK_SET == fl->l_whence)
                  && (fl->l_start < (whio_off_t)sub->lower )
                  )
              {
                  // not legal to lock before BOF.
                  rc = whio_rc.RangeError;
                  break;
              }
              rc = whio_dev_ioctl( sub->dev, arg, fl );
	  } while(0);
	  break;
#endif /*WHIO_CONFIG_USE_FCNTL*/
      case whio_dev_ioctl_WHIO_LOCK:
          /**
             Adjust lock request range and pass on the request
             to the parent device...
          */
	  do
	  {
              whio_lock_request * wli = va_arg(vargs,whio_lock_request *);
              if( ! wli )
              {
                  rc = whio_rc.ArgError;
                  break;
              }
              if( sub->upper )
              { /**
                   Check the start pos/length and adjust length if
                   necessary to keep it in the bounds of sub.

                   FIXME: i removed the subdev range checking/trimming
                   because it got too complex for me to follow once
                   i added SEEK_CUR/SEEK_END support. We need to:

                   a) ensure the start of the lock request does not lie
                   before sub->lower.

                   b) trim wli->length to fit in the subdevice.
                */
              }
              switch( wli->whence )
              {
                case SEEK_SET:
                    wli->start += sub->lower;
                    // FIXME: check for overflow!
                    break;
                case SEEK_CUR:
                    wli->whence = SEEK_SET;
                    wli->start += sub->pos;
                    // FIXME: check for overflow!
                    break;
                case SEEK_END:
                    if( sub->upper )
                    {
                        wli->whence = SEEK_SET;
                        wli->start += sub->upper;
                        // FIXME: check for overflow!
                    }
                    else
                    {
                        /**
                           We don't know enough here to give an end
                           position, so we'll pass on SEEK_END.
                        */
                    }
                    break;
                default:
                    return whio_rc.RangeError;
              };
              if( (SEEK_SET == wli->whence)
                  && (wli->start < (whio_off_t)sub->lower )
                  )
              {
                  // not legal to lock before BOF.
                  rc = whio_rc.RangeError;
                  break;
              }
              rc = whio_dev_ioctl( sub->dev, arg, wli );
	  } while(0);
	  break;
      case whio_dev_ioctl_SUBDEV_parent_dev:
	  rc = whio_rc.OK;
	  *(va_arg(vargs,whio_dev**)) = sub->dev;
	  break;
      case whio_dev_ioctl_SUBDEV_bounds_get:
	  rc = whio_rc.OK;
	  sz = (va_arg(vargs,whio_size_t*));
	  if( sz ) *sz = sub->lower;
	  sz = (va_arg(vargs,whio_size_t*));
	  if( sz ) *sz = sub->upper;
	  break;
      default:
	  rc = sub->dev->api->ioctl( sub->dev, arg, vargs );
	  break;
    };
    return rc;
}

static bool whio_dev_subdev_close( whio_dev * dev )
{
    WHIO_subdev_DECL(false);
    dev->api->flush(dev);
    if( dev->client.dtor ) dev->client.dtor( dev->client.data );
    dev->client = whio_client_data_empty;
    *sub = whio_dev_subdev_meta_empty;
    whio_dev_subdev_meta_free( sub );
    dev->impl = whio_impl_data_empty;
    return true;
}

static void whio_dev_subdev_finalize( whio_dev * dev )
{
    if( dev )
    {
	dev->api->close(dev);
	whio_dev_free(dev);
    }
}

#undef WHIO_subdev_DECL

static const whio_dev_api whio_dev_api_subdev =
    {
    whio_dev_subdev_read,
    whio_dev_subdev_write,
    whio_dev_subdev_close,
    whio_dev_subdev_finalize,
    whio_dev_subdev_error,
    whio_dev_subdev_clear_error,
    whio_dev_subdev_eof,
    whio_dev_subdev_tell,
    whio_dev_subdev_seek,
    whio_dev_subdev_flush,
    whio_dev_subdev_trunc,
    whio_dev_subdev_ioctl,
    whio_dev_subdev_iomode
    };

static const whio_dev whio_dev_subdev_empty =
    {
    &whio_dev_api_subdev,
    { /* impl */
    0, /* implData. Must be-a (whio_dev_subdev_meta*) */
    (void const *)&whio_dev_subdev_meta_empty /* typeID */
    }
    };

whio_dev * whio_dev_subdev_create( whio_dev * parent, whio_size_t lowerBound, whio_size_t upperBound )
{
    /* Reminder: this function _should_, in hindsight, take a range in
       the format (lowerBound,length) instead of (lower,upper). */
    /*
      FIXME: add a whio_dev object to meta and return a pointer to that, to save
      us an allocation.
    */
    if( ! parent || (upperBound && (upperBound <= lowerBound)) ) return 0;
    whio_dev * dev = whio_dev_alloc();
    if( ! dev ) return 0;
    whio_dev_subdev_meta * meta = whio_dev_subdev_meta_alloc();
    if( ! meta )
    {
	whio_dev_free(dev);
	return 0;
    }
    *dev = whio_dev_subdev_empty;
    *meta = whio_dev_subdev_meta_empty;
    dev->impl.data = meta;
    meta->dev = parent;
    meta->lower = lowerBound;
    meta->pos = lowerBound;
    meta->upper = upperBound;
    return dev;
}

int whio_dev_subdev_rebound( whio_dev * dev, whio_size_t lowerBound, whio_size_t upperBound )
{
    if( !dev || (upperBound && (upperBound <= lowerBound)) ) return whio_rc.ArgError;
    if( (void const *)&whio_dev_subdev_meta_empty != dev->impl.typeID ) return whio_rc.TypeError;
    whio_dev_subdev_meta * sub = (whio_dev_subdev_meta*)dev->impl.data;
    if( ! sub || !sub->dev ) return whio_rc.InternalError;
    sub->lower = sub->pos = lowerBound;
    sub->upper = upperBound;
    return whio_rc.OK;
}
/* end file src/whio_dev_subdev.c */
/* begin file src/whio_stream.c */
#line 8 "src/whio_stream.c"
#include <stdlib.h> /* free/malloc */



#ifdef __cplusplus
#define ARG_UNUSED(X)
extern "C" {
#else
#define ARG_UNUSED(X) X
#endif

bool whio_stream_default_isgood( whio_stream * ARG_UNUSED(self) )
{
    return false;
}

whio_size_t whio_stream_default_read( whio_stream * ARG_UNUSED(self),
				 void * ARG_UNUSED(dest),
				 whio_size_t ARG_UNUSED(max) )
{
    return 0;
}

whio_size_t whio_stream_default_write( whio_stream * ARG_UNUSED(self),
				void const * ARG_UNUSED(src),
				whio_size_t ARG_UNUSED(len) )
{
    return 0;
}

int whio_stream_default_flush( whio_stream * ARG_UNUSED(self) )
{
    return whio_rc.ArgError;
}

bool whio_stream_default_close( whio_stream * ARG_UNUSED(self) )
{
    if( self->client.dtor ) self->client.dtor( self->client.data );
    self->client = whio_client_data_empty;
    return false;
}

void whio_stream_default_finalize( whio_stream * ARG_UNUSED(self) )
{
    if(self)
    {
	self->api->close( self );
	//C11N_LOGME_DEALLOCT(whio_stream);
	free(self);
    }
}

const whio_stream_api whio_stream_api_empty = 
    {
    whio_stream_default_read,
    whio_stream_default_write,
    whio_stream_default_close,
    whio_stream_default_finalize,
    whio_stream_default_flush,
    whio_stream_default_isgood
    };

const whio_stream whio_stream_empty = 
    {
    &whio_stream_api_empty,
    whio_impl_data_empty_m,
    whio_client_data_empty_m
    };

bool whio_stream_getchar( whio_stream * self, char * tgt )
{
    if( ! self ) return false;
    char x = 0;
    if( 1 != self->api->read( self, &x, 1 ) ) return false;
    if( tgt ) *tgt = x;
    return true;
}


/** @implements whprintf_appender

   gprintf_appender implementation which appends all input to
   a whio_stream. Requires arg to be-a whio_stream. n bytes from
   the data argument are written to that stream. On success, the number
   of bytes written is returned.
*/
static long whio_stream_printf_appender( void * arg, char const * data, long n )
{
    if( ! arg || !data ) return -1;
    whio_size_t sz = n;
    if( n < sz ) return -1; /* negative n */
    whio_stream * str = (whio_stream*)arg;
    sz = str->api->write( str, data, sz );
    return (sz == whio_rc.SizeTError) ? 0 : (long) sz; // FIXME: check for overflow!
}

whio_size_t whio_stream_writefv( whio_stream * str, const char *fmt, va_list ap )
{
    long rc = whprintfv( whio_stream_printf_appender, str, fmt, ap );
    return (rc < 0) ? 0 : (whio_size_t)rc;
}

whio_size_t whio_stream_writef( whio_stream * str, const char *fmt, ... )
{
    va_list vargs;
    va_start( vargs, fmt );
    whio_size_t rc = whio_stream_writefv( str, fmt, vargs );
    va_end(vargs);
    return rc;
}

int whio_stream_copy( whio_stream * restrict istr, whio_stream * restrict ostr )
{
    if( istr == ostr ) return 0;
    whio_size_t rdrc = 0;
    whio_size_t wrc = 0;
    enum { bufSize = 1024 * 4 };
    unsigned char buf[bufSize];
    do
    {
        rdrc = istr->api->read( istr, buf, bufSize );
        if( ! rdrc ) return whio_rc.IOError;
        wrc = ostr->api->write( ostr, buf, rdrc );
        if( rdrc != wrc ) return whio_rc.IOError;
    } while( bufSize == rdrc );
    return whio_rc.OK;
}


#ifdef __cplusplus
} /* extern "C" */
#endif
/* end file src/whio_stream.c */
/* begin file src/whio_stream_dev.c */
#line 8 "src/whio_stream_dev.c"
#include <stdlib.h> /* free/malloc */

/*
  Implementation for a whio_stream wrapper for a whio_dev object.
*/
#ifdef __cplusplus
#define ARG_UNUSED(X)
extern "C" {
#else
#define ARG_UNUSED(X) X
#endif

static bool whio_stream_dev_isgood( whio_stream * self );
static whio_size_t whio_stream_dev_read( whio_stream * self, void * dest, whio_size_t max );
static whio_size_t whio_stream_dev_write( whio_stream * self, void const * src, whio_size_t len );
static int whio_stream_dev_flush( whio_stream * ARG_UNUSED(self) );
static bool whio_stream_dev_close( whio_stream * self );
static void whio_stream_dev_finalize( whio_stream * self );
static short whio_stream_dev_iomode( whio_stream * self );
/** whio_dev::impl::typeID value for whio_stream_dev objects. */

const whio_stream_api whio_stream_api_dev = 
    {
    whio_stream_dev_read,
    whio_stream_dev_write,
    whio_stream_dev_close,
    whio_stream_dev_finalize,
    whio_stream_dev_flush,
    whio_stream_dev_isgood,
    whio_stream_dev_iomode
    };

const whio_stream whio_stream_dev =
    {
    &whio_stream_api_dev,
    { /* impl */
    0, /* data */
    &whio_stream_api_dev /* typeID */
    }
    };


typedef struct whio_stream_dev_meta
{
    whio_dev * dev;
    bool ownsDev;
} whio_stream_dev_meta;

static const whio_stream_dev_meta whio_stream_dev_meta_empty =
    {
    0, /* dev */
    0 /* ownsDev */
    };

/**
   Helper macro for the whio_stream_dev_xxx() API.
*/
#define WHIO_STR_DEV_DECL(RV) whio_stream_dev_meta * meta = (self && (self->impl.typeID==&whio_stream_api_dev)) ? (whio_stream_dev_meta*)self->impl.data : 0; \
    if( ! meta ) return RV

bool whio_stream_dev_isgood( whio_stream * self )
{
    WHIO_STR_DEV_DECL(false);
    return meta->dev ? (0 == meta->dev->api->error(meta->dev)) : false;
}

whio_size_t whio_stream_dev_read( whio_stream * self, void * dest, whio_size_t max )
{
    WHIO_STR_DEV_DECL(0);
    return meta->dev ? meta->dev->api->read(meta->dev, dest, max) : 0;
}

whio_size_t whio_stream_dev_write( whio_stream * self, void const * src, whio_size_t len )
{
    WHIO_STR_DEV_DECL(0);
    return meta->dev ? meta->dev->api->write(meta->dev, src, len) : 0;
}

int whio_stream_dev_flush( whio_stream * ARG_UNUSED(self) )
{
    WHIO_STR_DEV_DECL(whio_rc.ArgError);
    return meta->dev->api->flush( meta->dev );
}

bool whio_stream_dev_close( whio_stream * self )
{
    whio_stream_dev_meta * meta = (self ? (whio_stream_dev_meta*)self->impl.data : 0);
    if( ! meta ) return false;
    self->impl.data = 0;
    if( meta->dev )
    {
        if( meta->dev->api->iomode(meta->dev) > 0 )
        {
            meta->dev->api->flush( meta->dev );
        }
    }
    if( self->client.dtor ) self->client.dtor( self->client.data );
    self->client = whio_client_data_empty;
    if( meta->dev && meta->ownsDev )
    {
        meta->dev->api->finalize( meta->dev );
    }
    whio_free( meta );
    return true;
}


static short whio_stream_dev_iomode( whio_stream * self )
{
    WHIO_STR_DEV_DECL(-1);
    return meta->dev->api->iomode( meta->dev );
}


void whio_stream_dev_finalize( whio_stream * self )
{
    if( self )
    {
	self->api->close( self );
	whio_free( self );
    }
}

whio_stream * whio_stream_for_dev( whio_dev * dev, bool takeOwnership )
{
    if( ! dev ) return 0;
    /*
      FIXME: add a whio_stream object to meta and return a pointer to
      that object, to save an allocation.
    */
    whio_stream * str = (whio_stream *) whio_malloc( sizeof(whio_stream) );
    if( ! str ) return 0;
    whio_stream_dev_meta * meta = (whio_stream_dev_meta *) whio_malloc( sizeof(whio_stream_dev_meta) );
    if( ! meta )
    {
	whio_free(str);
	return 0;
    }
    *str = whio_stream_dev;
    *meta = whio_stream_dev_meta_empty;
    str->impl.data = meta;
    meta->dev = dev;
    meta->ownsDev = takeOwnership;
    return str;
}



#ifdef __cplusplus
} /* extern "C" */
#endif
/* end file src/whio_stream_dev.c */
/* begin file src/whio_stream_FILE.c */
#line 8 "src/whio_stream_FILE.c"

#if !defined(_POSIX_C_SOURCE)
/* required for for fileno(), ftello(), maybe others */
#  define _POSIX_C_SOURCE 200112L
#endif

/*
  whio_stream wrapper implementation for FILE handles.
*/
#include <stdlib.h> /* malloc()/free() */
#include <string.h> /* strstr() */
#include <stdio.h> /* (FILE*) */

static bool whio_stream_FILE_isgood( whio_stream * self );
static whio_size_t whio_stream_FILE_read( whio_stream * self, void * dest, whio_size_t max );
static whio_size_t whio_stream_FILE_write( whio_stream * self, void const * src, whio_size_t len );
static int whio_stream_FILE_flush( whio_stream * self );
static void whio_stream_FILE_finalize( whio_stream * self );
static bool whio_stream_FILE_close( whio_stream * self );
static short whio_stream_FILE_iomode( whio_stream * self );

const whio_stream_api whio_stream_api_FILE = 
    {
    whio_stream_FILE_read,
    whio_stream_FILE_write,
    whio_stream_FILE_close,
    whio_stream_FILE_finalize,
    whio_stream_FILE_flush,
    whio_stream_FILE_isgood,
    whio_stream_FILE_iomode
    };

const whio_stream whio_stream_FILE_init = 
    {
    &whio_stream_api_FILE,
    { /* impl */
    0, /* data */
    &whio_stream_api_FILE /* typeID */
    }
    };


typedef struct whio_stream_FILEINFO
{
    /**
       File handle this object proxies.
    */
    FILE * fp;
    /**
       fileno(fp)
    */
    int fileno;
    /**
       If this object owns its FILE pointer (it opened it itself)
       then api->finalize() will fclose() it.
     */
    bool ownsFile;
    short iomode;
    void * memToFree;
} whio_stream_FILEINFO;
static const whio_stream_FILEINFO whio_stream_FILEINFO_init =
    {
    0, /* fp */
    0, /* fileno */
    false, /* ownsFile */
    -1 /* iomode */,
    NULL/* memToFree*/
    };

whio_stream * whio_stream_for_FILE( FILE * fp, bool takeOwnership )
{
    if( ! fp ) return 0;
    void * mem = whio_malloc( sizeof(whio_stream) + sizeof(whio_stream_FILEINFO) );
    if( ! mem ) return NULL;
    whio_stream * st = (whio_stream *) mem;
    whio_stream_FILEINFO * meta = (whio_stream_FILEINFO*) WHIO_VOID_PTR_ADD(mem,sizeof(whio_stream) );
    *meta = whio_stream_FILEINFO_init;
    *st = whio_stream_FILE_init;
    st->impl.data = meta;
    meta->memToFree = mem;
    meta->ownsFile = takeOwnership;
    meta->fp = fp;
    meta->fileno = fileno(fp);
    meta->iomode = -1;
    return st;
}

whio_stream * whio_stream_for_filename( char const * src, char const * mode )
{
    if( ! src || ! mode ) return 0;
    FILE * fp = fopen( src, mode );
    if( ! fp ) return 0;
    whio_stream * st = whio_stream_for_FILE(fp, true);
    if( ! st )
    {
	fclose(fp);
    }
    else
    {
        whio_stream_FILEINFO * meta = (whio_stream_FILEINFO*)st->impl.data;
        meta->iomode = whio_mode_to_iomode( mode );
    }
    return st;
}

whio_stream * whio_stream_for_fileno( int fileno, bool writeMode )
{
    FILE * fp = fdopen( fileno, writeMode ? "wb" : "r+b" );
    if( ! fp ) return 0;
    whio_stream * st = whio_stream_for_FILE(fp, true);
    if( ! st )
    {
	fclose(fp);
    }
    else
    {
        whio_stream_FILEINFO * meta = (whio_stream_FILEINFO*)st->impl.data;
        meta->iomode = writeMode ? 1 : 0;
    }
    return st;
}

/**
   Helper macro for the whio_stream_FILE_xxx() API.
*/
#define WHIO_STR_FILE_DECL(RV) whio_stream_FILEINFO * meta = (self && (self->impl.typeID==&whio_stream_api_FILE)) ? (whio_stream_FILEINFO*)self->impl.data : 0; \
    if( ! meta ) return RV


/**
   whio_stream_api.isgood implementation for whio_stream_FILE.
*/
static bool whio_stream_FILE_isgood( whio_stream * self )
{
    if( self && self->impl.data )
    {
	WHIO_STR_FILE_DECL(false);
	return meta->fp && (0 == ferror(meta->fp));
    }
    return false;
}

static short whio_stream_FILE_iomode( whio_stream * self )
{
	WHIO_STR_FILE_DECL(-1);
        return meta->iomode;
}

/**
   whio_stream_api.read implementation for whio_stream_FILE.
*/
static whio_size_t whio_stream_FILE_read( whio_stream * self, void * dest, whio_size_t max )
{
    WHIO_STR_FILE_DECL(0);
    if( ! self || !max || !dest	) return 0;
    else return fread( dest, sizeof(char), max, meta->fp );
}

/**
   whio_stream_api.write implementation for whio_stream_FILE.
*/
static whio_size_t whio_stream_FILE_write( whio_stream * self, void const * src, whio_size_t len )
{
    WHIO_STR_FILE_DECL(false);
    if( ! self ) return false;
    else return fwrite( src, sizeof(char), len, meta->fp );
}

static int whio_stream_FILE_flush( whio_stream * self )
{
    WHIO_STR_FILE_DECL(whio_rc.ArgError);
    return (meta->fp)
	? fflush(meta->fp)
	: whio_rc.InternalError;
}

static bool whio_stream_FILE_close( whio_stream * self )
{
    WHIO_STR_FILE_DECL(false);
    if( meta->iomode ) self->api->flush( self );
    if( self->client.dtor ) self->client.dtor( self->client.data );
    self->client = whio_client_data_empty;
    self->impl.data = 0;
    if( meta->fp && meta->ownsFile )
    {
        fclose( meta->fp );
    }
    *meta = whio_stream_FILEINFO_init;
    // meta's memory was allocated as part of self, and will be cleaned up later.
    return true;
}

/**
   whio_stream_api.destroy implementation for whio_stream_FILE.
*/
static void whio_stream_FILE_finalize( whio_stream * self )
{
    WHIO_STR_FILE_DECL();
    void * mem = meta->memToFree;
    self->api->close( self );
    *self = whio_stream_FILE_init;
    whio_free(mem);
}

#undef WHIO_STR_FILE_DECL
/* end file src/whio_stream_FILE.c */
/* begin file src/whio_zlib.c */
#line 8 "src/whio_zlib.c"
/*
  Implementations for whio gzip support.

  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/

#if WHIO_ENABLE_ZLIB
#include <zlib.h>
#endif /* WHIO_ENABLE_ZLIB */

int whio_stream_gzip( whio_stream * restrict src, whio_stream * restrict dest, int level )
{
#if ! WHIO_ENABLE_ZLIB
    return whio_rc.UnsupportedError;
#else
    if( !src || !dest || (src == dest) ) return whio_rc.ArgError;
    if( level != Z_DEFAULT_COMPRESSION )
    {
	if( level < Z_NO_COMPRESSION ) level = Z_NO_COMPRESSION;
	else if (level > Z_BEST_COMPRESSION) level = Z_BEST_COMPRESSION;
    }
    /* Code taken 99% from http://zlib.net/zlib_how.html */
    int ret;
    int flush;
    size_t have;
    z_stream strm;
    enum { bufSize  = 1024 * 8 };
    unsigned char in[bufSize];
    unsigned char out[bufSize];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = //deflateInit(&strm, level)
	deflateInit2( &strm, level, Z_DEFLATED, 16+MAX_WBITS, 8, Z_DEFAULT_STRATEGY )
	;
    if (ret != Z_OK)
    {
	WHIO_DEBUG("defaultInit() failed with rc %d\n", ret );
        return ret;
    }

    /* compress until end of file */
    do
    {
	size_t iosize = src->api->read( src, in, bufSize );
	strm.avail_in = iosize;
        if( ! src->api->isgood(src)  )
	{
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = (iosize < bufSize) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
	/* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do
	{
	    strm.avail_out = bufSize;
            strm.next_out = out;
	    ret = deflate(&strm, flush);    /* no bad return value */
	    if( Z_STREAM_ERROR == ret )
	    {
		WHIO_DEBUG("deflate() returned Z_STREAM_ERROR!\n");
                (void)deflateEnd(&strm);
		return Z_STREAM_ERROR;
	    }
	    have = bufSize - strm.avail_out;
	    if( have )
	    {
		iosize = dest->api->write( dest, out, have );
		if( (iosize != have)
		    || !dest->api->isgood(dest) )
		{
		    WHIO_DEBUG("Write of %u bytes failed - wrote only %u.\n", have, iosize );
		    (void)deflateEnd(&strm);
		    return Z_ERRNO;
		}
            }
	} while (strm.avail_out == 0);
	if( strm.avail_in != 0)
	{
	    WHIO_DEBUG("Not all input was consumed! %u byte(s) remain!", strm.avail_in );
	    (void)deflateEnd(&strm);
	}
        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    //assert(ret == Z_STREAM_END);        /* stream will be complete */
    /* clean up and return */
    (void)deflateEnd(&strm);
    return (ret == Z_STREAM_END) ? Z_OK : ret;
#endif /* WHIO_ENABLE_ZLIB */

}


int whio_stream_gunzip( whio_stream * restrict src, whio_stream * restrict dest )
{
#if ! WHIO_ENABLE_ZLIB
    return whio_rc.UnsupportedError;
#else
    if( !src || !dest || (src == dest) ) return whio_rc.ArgError;
    /* Code taken 99% from http://zlib.net/zlib_how.html */
    int ret;
    size_t have;
    z_stream strm;
    enum { bufSize  = 1024 * 8 };
    unsigned char in[bufSize];
    unsigned char out[bufSize];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = //inflateInit( &strm )
	inflateInit2( &strm, 16+MAX_WBITS )
	;
    if (ret != Z_OK)
    {
	WHIO_DEBUG("Initialization of z_stream failed with rc %d!\n", ret );
        return ret;
    }
    do
    {
	size_t iosize = src->api->read( src, in, bufSize );
	strm.avail_in = iosize;
	if( ! src->api->isgood( src ) )
	{
            (void)inflateEnd( &strm );
	    WHIO_DEBUG("!src->isgood()!\n" );
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;
        /* run inflate() on input until output buffer not full */
        do
	{
            strm.avail_out = bufSize;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            switch (ret)
	    {
	      case Z_NEED_DICT:
		  WHIO_DEBUG("inflate() says Z_NEED_DICT\n");
		  ret = Z_DATA_ERROR; /* and fall through */
	      case Z_STREAM_ERROR:
		  WHIO_DEBUG("Z_STREAM_ERROR\n");
	      case Z_DATA_ERROR:
		  WHIO_DEBUG("Z_DATA_ERROR\n");
	      case Z_MEM_ERROR:
		  WHIO_DEBUG("inflate() returned unwanted value %d!\n", ret );
		  (void)inflateEnd(&strm);
		  return ret;
	      default:
		  break;
            }
            have = bufSize - strm.avail_out;
	    if( have )
	    {
		iosize = dest->api->write( dest, out, have );
		if ( (iosize != have)
		     || !dest->api->isgood(dest) )
		{
		    WHIO_DEBUG("write failed or !dest->isgood()! Wrote %u of %u bytes?\n", iosize, have );
		    (void)inflateEnd(&strm);
		    return Z_ERRNO;
		}
	    }
	} while (strm.avail_out == 0);
	/* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);
    (void)inflateEnd( &strm );
    return (ret == Z_STREAM_END) ? Z_OK : Z_DATA_ERROR;
#endif /* WHIO_ENABLE_ZLIB */
}
/* end file src/whio_zlib.c */
/* begin file src/whio_encode.c */
#line 8 "src/whio_encode.c"
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/

  License: Public Domain
*/

/* Note from linux stdint.h:

   The ISO C99 standard specifies that in C++ implementations these
   should only be defined if explicitly requested. 
*/
#if defined __cplusplus && ! defined __STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS
#endif
#include <stdlib.h> /* calloc() */
#include <string.h> /* memset() */
static const unsigned char whio_uint64_tag_char = 0x80 | 64;
size_t whio_encode_uint64( unsigned char * dest, uint64_t i )
{
    if( ! dest ) return 0;
    static const uint64_t mask = UINT64_C(0x00ff);
    size_t x = 0;
    dest[x++] = whio_uint64_tag_char;
    dest[x++] = (unsigned char)((i>>56) & mask);
    dest[x++] = (unsigned char)((i>>48) & mask);
    dest[x++] = (unsigned char)((i>>40) & mask);
    dest[x++] = (unsigned char)((i>>32) & mask);
    dest[x++] = (unsigned char)((i>>24) & mask);
    dest[x++] = (unsigned char)((i>>16) & mask);
    dest[x++] = (unsigned char)((i>>8) & mask);
    dest[x++] = (unsigned char)(i & mask);
    return whio_sizeof_encoded_uint64;
}


int whio_decode_uint64( unsigned char const * src, uint64_t * tgt )
{
    if( ! src || ! tgt ) return whio_rc.ArgError;
    if( whio_uint64_tag_char != src[0] )
    {
	return whio_rc.ConsistencyError;
    }
#define SHIFT(X) ((uint64_t)src[X])
    uint64_t val = (SHIFT(1) << UINT64_C(56))
	+ (SHIFT(2) << UINT64_C(48))
	+ (SHIFT(3) << UINT64_C(40))
	+ (SHIFT(4) << UINT64_C(32))
	+ (SHIFT(5) << UINT64_C(24))
	+ (SHIFT(6) << UINT64_C(16))
	+ (SHIFT(7) << UINT64_C(8))
	+ (SHIFT(8));
#undef SHIFT
    *tgt = val;
    return whio_rc.OK;
}


static const unsigned char whio_int64_tag_char = 0x80 | 65;
size_t whio_encode_int64( unsigned char * dest, int64_t i )
{
    if( ! dest ) return 0;
    static const int64_t mask = INT64_C(0x00ff);
    size_t x = 0;
    dest[x++] = whio_int64_tag_char;
    dest[x++] = (unsigned char)((i>>56) & mask);
    dest[x++] = (unsigned char)((i>>48) & mask);
    dest[x++] = (unsigned char)((i>>40) & mask);
    dest[x++] = (unsigned char)((i>>32) & mask);
    dest[x++] = (unsigned char)((i>>24) & mask);
    dest[x++] = (unsigned char)((i>>16) & mask);
    dest[x++] = (unsigned char)((i>>8) & mask);
    dest[x++] = (unsigned char)(i & mask);
    return whio_sizeof_encoded_int64;
}


int whio_decode_int64( unsigned char const * src, int64_t * tgt )
{
    if( ! src || ! tgt ) return whio_rc.ArgError;
    if( whio_int64_tag_char != src[0] )
    {
	return whio_rc.ConsistencyError;
    }
#define SHIFT(X) ((int64_t)src[X])
    int64_t val = (SHIFT(1) << INT64_C(56))
	+ (SHIFT(2) << INT64_C(48))
	+ (SHIFT(3) << INT64_C(40))
	+ (SHIFT(4) << INT64_C(32))
	+ (SHIFT(5) << INT64_C(24))
	+ (SHIFT(6) << INT64_C(16))
	+ (SHIFT(7) << INT64_C(8))
	+ (SHIFT(8));
#undef SHIFT
    *tgt = val;
    return whio_rc.OK;
}


static const unsigned char whio_uint32_tag_char = 0x80 | 32;
size_t whio_encode_uint32( unsigned char * dest, uint32_t i )
{
    if( ! dest ) return 0;
    static const unsigned short mask = 0x00ff;
    size_t x = 0;
    /** We tag all entries with a prefix mainly to simplify debugging
	of the vfs files (it's easy to spot them in a file viewer),
	but it incidentally also gives us a sanity-checker at
	read-time (we simply confirm that the first byte is this
	prefix).
    */
    dest[x++] = whio_uint32_tag_char;
    dest[x++] = (unsigned char)(i>>24) & mask;
    dest[x++] = (unsigned char)(i>>16) & mask;
    dest[x++] = (unsigned char)(i>>8) & mask;
    dest[x++] = (unsigned char)(i & mask);
    return whio_sizeof_encoded_uint32;
}

int whio_decode_uint32( unsigned char const * src, uint32_t * tgt )
{
    if( ! src ) return whio_rc.ArgError;
    if( whio_uint32_tag_char != src[0] )
    {
	//WHIO_DEBUG("read bytes are not an encoded integer value!\n");
	return whio_rc.ConsistencyError;
    }
    uint32_t val = (src[1] << 24)
	+ (src[2] << 16)
	+ (src[3] << 8)
	+ (src[4]);
    if( tgt ) *tgt = val;
    return whio_rc.OK;
}

static const unsigned char whio_int32_tag_char = 0x80 | 33;
size_t whio_encode_int32( unsigned char * dest, int32_t i )
{
    if( ! dest ) return 0;
    static const unsigned short mask = 0x00ff;
    size_t x = 0;
    /** We tag all entries with a prefix mainly to simplify debugging
	of the vfs files (it's easy to spot them in a file viewer),
	but it incidentally also gives us a sanity-checker at
	read-time (we simply confirm that the first byte is this
	prefix).
    */
    dest[x++] = whio_int32_tag_char;
    dest[x++] = (unsigned char)(i>>24) & mask;
    dest[x++] = (unsigned char)(i>>16) & mask;
    dest[x++] = (unsigned char)(i>>8) & mask;
    dest[x++] = (unsigned char)(i & mask);
    return whio_sizeof_encoded_int32;
}

int whio_decode_int32( unsigned char const * src, int32_t * tgt )
{
    if( ! src ) return whio_rc.ArgError;
    if( whio_int32_tag_char != src[0] )
    {
	//WHIO_DEBUG("read bytes are not an encoded integer value!\n");
	return whio_rc.ConsistencyError;
    }
    int32_t val = (src[1] << 24)
	+ (src[2] << 16)
	+ (src[3] << 8)
	+ (src[4]);
    if( tgt ) *tgt = val;
    return whio_rc.OK;
}


static const unsigned char whio_uint16_tag_char = 0x80 | 16;
size_t whio_encode_uint16( unsigned char * dest, uint16_t i )
{
    if( ! dest ) return 0;
    static const uint16_t mask = 0x00ff;
    uint8_t x = 0;
    dest[x++] = whio_uint16_tag_char;
    dest[x++] = (unsigned char)(i>>8) & mask;
    dest[x++] = (unsigned char)(i & mask);
    return whio_sizeof_encoded_uint16;
}

int whio_decode_uint16( unsigned char const * src, uint16_t * tgt )
{
    if( ! src ) return whio_rc.ArgError;
    if( whio_uint16_tag_char != src[0] )
    {
	return whio_rc.ConsistencyError;
    }
    uint16_t val = + (src[1] << 8)
	+ (src[2]);
    *tgt = val;
    return whio_rc.OK;
}

static const unsigned char whio_int16_tag_char = 0x80 | 17;
size_t whio_encode_int16( unsigned char * dest, int16_t i )
{
    if( ! dest ) return 0;
    static const int16_t mask = 0x00ff;
    int8_t x = 0;
    dest[x++] = whio_int16_tag_char;
    dest[x++] = (unsigned char)(i>>8) & mask;
    dest[x++] = (unsigned char)(i & mask);
    return whio_sizeof_encoded_int16;
}

int whio_decode_int16( unsigned char const * src, int16_t * tgt )
{
    if( ! src ) return whio_rc.ArgError;
    if( whio_int16_tag_char != src[0] )
    {
	return whio_rc.ConsistencyError;
    }
    int16_t val = + (src[1] << 8)
	+ (src[2]);
    *tgt = val;
    return whio_rc.OK;
}


static const unsigned char whio_uint8_tag_char = 0x80 | 8;
size_t whio_encode_uint8( unsigned char * dest, uint8_t i )
{
    if( ! dest ) return 0;
    dest[0] = whio_uint8_tag_char;
    dest[1] = i;
    return whio_sizeof_encoded_uint8;
}

int whio_decode_uint8( unsigned char const * src, uint8_t * tgt )
{
    if( ! src ) return whio_rc.ArgError;
    if( whio_uint8_tag_char != src[0] )
    {
	return whio_rc.ConsistencyError;
    }
    *tgt = src[1];
    return whio_rc.OK;
}

static const unsigned char whio_int8_tag_char = 0x80 | 8;
size_t whio_encode_int8( unsigned char * dest, int8_t i )
{
    if( ! dest ) return 0;
    dest[0] = whio_int8_tag_char;
    dest[1] = i;
    return whio_sizeof_encoded_int8;
}

int whio_decode_int8( unsigned char const * src, int8_t * tgt )
{
    if( ! src ) return whio_rc.ArgError;
    if( whio_int8_tag_char != src[0] )
    {
	return whio_rc.ConsistencyError;
    }
    *tgt = src[1];
    return whio_rc.OK;
}


size_t whio_encode_uint32_array( unsigned char * dest, size_t n, uint32_t const * list )
{
    size_t i = (dest && n && list) ? 0 : n;
    size_t rc = 0;
    for( ; i < n; ++i, ++rc )
    {
	if( whio_sizeof_encoded_uint32 != whio_encode_uint32( dest, *(list++) ) )
	{
	    break;
	}
    }
    return rc;
}

size_t whio_decode_uint32_array( unsigned char const * src, size_t n, uint32_t * list )
{
    size_t i = (src && n && list) ? 0 : n;
    size_t rc = 0;
    for( ; i < n; ++i, ++rc )
    {
	if( whio_rc.OK != whio_decode_uint32( src, &list[i] ) )
	{
	    break;
	}
    }
    return rc;
}

static const unsigned char whio_cstring_tag_char = 0x80 | '"';
size_t whio_encode_cstring( unsigned char * dest, char const * s, uint32_t n )
{
    if( ! dest || !s ) return 0;
    if( ! n )
    {
	char const * x = s;
	loop: if( x && *x ) { ++x; ++n; goto loop; }
	//for( ; x && *x ; ++x, ++n ){}
    }
    *(dest++) = whio_cstring_tag_char;
    size_t rc = whio_encode_uint32( dest, n );
    if( whio_sizeof_encoded_uint32 != rc ) return rc;
    dest += rc;
    rc = 1 + whio_sizeof_encoded_uint32;
    size_t i = 0;
    for( ; i < n; ++i, ++rc )
    {
	*(dest++) = *(s++);
    }
    *dest = 0;
    return rc;
}

int whio_decode_cstring( unsigned char const * src, char ** tgt, size_t * length )
{
    if( !src || ! tgt ) return whio_rc.ArgError;

    if( whio_cstring_tag_char != *src )
    {
	return whio_rc.ConsistencyError;
    }
    ++src;
    uint32_t slen = 0;
    int rc = whio_decode_uint32( src, &slen );
    if( whio_rc.OK != rc ) return rc;
    if( ! slen )
    {
	*tgt = 0;
	if(length) *length = 0;
	return whio_rc.OK;
    }
    char * buf = (char *)calloc( slen + 1, sizeof(char) );
    if( ! buf ) return whio_rc.AllocError;
    if( length ) *length = slen;
    *tgt = buf;
    size_t i = 0;
    src += whio_sizeof_encoded_uint32;
    for(  ; i < slen; ++i )
    {
	*(buf++) = *(src++);
    }
    *buf = 0;
    return whio_rc.OK;
}

/**
   tag byte for encoded whio_id_type objects.
*/
static const unsigned int whio_size_t_tag_char = 0x08 | 'p';
whio_size_t whio_encode_size_t( unsigned char * dest, whio_size_t v )
{
#if WHIO_SIZE_T_BITS == 64
    return whio_encode_uint64( dest, v );
#elif WHIO_SIZE_T_BITS == 32
    return whio_encode_uint32( dest, v );
#elif WHIO_SIZE_T_BITS == 16
    return whio_encode_uint16( dest, v );
#elif WHIO_SIZE_T_BITS == 8
    return whio_encode_uint8( dest, v );
#else
#error "whio_size_t size (WHIO_SIZE_T_BITS) is not supported!"
#endif
}

int whio_decode_size_t( unsigned char const * src, whio_size_t * v )
{
#if WHIO_SIZE_T_BITS == 64
    return whio_decode_uint64( src, v );
#elif WHIO_SIZE_T_BITS == 32
    return whio_decode_uint32( src, v );
#elif WHIO_SIZE_T_BITS == 16
    return whio_decode_uint16( src, v );
#elif WHIO_SIZE_T_BITS == 8
    return whio_decode_uint8( src, v );
#else
#error "whio_size_t is not a supported type!"
#endif
}

whio_size_t whio_dev_encode_size_t( whio_dev * dev, whio_size_t v )
{
    unsigned char buf[whio_sizeof_encoded_size_t];
    whio_encode_size_t( buf, v );
    return dev->api->write( dev, &buf, whio_sizeof_encoded_size_t );
}

int whio_dev_decode_size_t( whio_dev * dev, whio_size_t * tgt )
{
    unsigned char buf[whio_sizeof_encoded_size_t]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_size_t is the correct size. */
    memset( buf, 0, whio_sizeof_encoded_size_t );
    size_t rc = dev->api->read( dev, buf  /*Flawfinder: ignore  This is safe in conjunction with whio_sizeof_encoded_xxx*/, whio_sizeof_encoded_size_t );
    return ( whio_sizeof_encoded_size_t == rc )
        ? whio_decode_size_t( buf, tgt )
        : whio_rc.IOError;
}


size_t whio_dev_encode_uint64( whio_dev * dev, uint64_t i )
{
    unsigned char buf[whio_sizeof_encoded_uint64]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint64 is the correct size. */
    size_t const x = whio_encode_uint64( buf, i );
    return ( whio_sizeof_encoded_uint64 == x )
        ? whio_dev_write( dev, buf, whio_sizeof_encoded_uint64 )
        : 0;
}

int whio_dev_decode_uint64( whio_dev * dev, uint64_t * tgt )
{
    if( ! dev || ! tgt ) return whio_rc.ArgError;
    unsigned char buf[whio_sizeof_encoded_uint64]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint64 is the correct size. */
    memset( buf, 0, whio_sizeof_encoded_uint64 );
    size_t rc = dev->api->read( dev, buf  /*Flawfinder: ignore  This is safe in conjunction with whio_sizeof_encoded_uint64*/, whio_sizeof_encoded_uint64 );
    return ( whio_sizeof_encoded_uint64 == rc )
        ? whio_decode_uint64( buf, tgt )
        : whio_rc.IOError;
}


size_t whio_dev_encode_uint32( whio_dev * dev, uint32_t i )
{
    unsigned char buf[whio_sizeof_encoded_uint32]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint32 is the correct size. */
    size_t const x = whio_encode_uint32( buf, i );
    return ( whio_sizeof_encoded_uint32 == x )
        ? whio_dev_write( dev, buf, whio_sizeof_encoded_uint32 )
        : 0;
}

int whio_dev_decode_uint32( whio_dev * dev, uint32_t * tgt )
{
    if( ! dev || ! tgt ) return whio_rc.ArgError;
    unsigned char buf[whio_sizeof_encoded_uint32]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint32 is the correct size. */
    memset( buf, 0, whio_sizeof_encoded_uint32 );
    size_t rc = dev->api->read( dev, buf  /*Flawfinder: ignore  This is safe in conjunction with whio_sizeof_encoded_uint32*/, whio_sizeof_encoded_uint32 );
    return ( whio_sizeof_encoded_uint32 == rc )
        ? whio_decode_uint32( buf, tgt )
        : whio_rc.IOError;
}


static const unsigned char whio_dev_uint16_tag_char = 0x80 | 16;
size_t whio_dev_encode_uint16( whio_dev * dev, uint16_t i )
{
    unsigned char buf[whio_sizeof_encoded_uint16]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint16 is the correct size. */
    size_t const x = whio_encode_uint16( buf, i );
    return ( whio_sizeof_encoded_uint16 == x )
        ? whio_dev_write( dev, buf, whio_sizeof_encoded_uint16 )
        : 0;
}

int whio_dev_decode_uint16( whio_dev * dev, uint16_t * tgt )
{
    if( ! dev || ! tgt ) return whio_rc.ArgError;
    unsigned char buf[whio_sizeof_encoded_uint16]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint16 is the correct size. */
    memset( buf, 0, whio_sizeof_encoded_uint16 );
    size_t rc = dev->api->read( dev, buf  /*Flawfinder: ignore  This is safe in conjunction with whio_sizeof_encoded_uint16*/, whio_sizeof_encoded_uint16 );
    return ( whio_sizeof_encoded_uint16 == rc )
        ? whio_decode_uint16( buf, tgt )
        : whio_rc.IOError;
}

size_t whio_dev_encode_uint32_array( whio_dev * dev, size_t n, uint32_t const * list )
{
    size_t i = (dev && n && list) ? 0 : n;
    size_t rc = 0;
    for( ; i < n; ++i, ++rc )
    {
	if( whio_sizeof_encoded_uint32 != whio_dev_encode_uint32( dev, *(list++) ) )
	{
	    break;
	}
    }
    return rc;
}

size_t whio_dev_decode_uint32_array( whio_dev * dev, size_t n, uint32_t * list )
{
    size_t i = (dev && n && list) ? 0 : n;
    size_t rc = 0;
    for( ; i < n; ++i, ++rc )
    {
	if( whio_rc.OK != whio_dev_decode_uint32( dev, &list[i] ) )
	{
	    break;
	}
    }
    return rc;
}

static const unsigned char whio_dev_cstring_tag_char = 0x80 | '"';
whio_size_t whio_dev_encode_cstring( whio_dev * dev, char const * s, uint32_t n )
{
    if( ! dev || !s ) return 0;
    if( ! n )
    {
	char const * x = s;
	loop: if( x && *x ) { ++x; ++n; goto loop; }
	//for( ; x && *x ; ++x, ++n ){}
    }
    whio_size_t rc = dev->api->write( dev, &whio_dev_cstring_tag_char, 1 );
    if( 1 != rc ) return rc;
    rc += whio_dev_encode_uint32( dev, n );
    if( (1 + whio_sizeof_encoded_uint32) != rc ) return rc;
    return rc + dev->api->write( dev, s, (whio_size_t)n );
}

int whio_dev_decode_cstring( whio_dev * dev, char ** tgt, uint32_t * length )
{
    if( !dev || ! tgt ) return whio_rc.ArgError;

    { /* check tag byte */
	unsigned char tagbuf[1] = {0}; /* Flawfinder: ignore  This is intended and safe. */
	whio_size_t const sz = dev->api->read( dev, tagbuf, 1 );/*Flawfinder: ignore  This is safe used safely.*/
	if( (1 != sz) || (whio_dev_cstring_tag_char != tagbuf[0]) )
	{
	    return sz ? whio_rc.ConsistencyError : whio_rc.IOError;
	}
    }
    uint32_t slen = 0;
    int rc = whio_dev_decode_uint32( dev, &slen );
    if( whio_rc.OK != rc ) return rc;
    if( ! slen )
    {
	*tgt = 0;
	if(length) *length = 0;
	return whio_rc.OK;
    }
    char * buf = (char *)calloc( slen + 1, sizeof(char) );
    if( ! buf ) return whio_rc.AllocError;
    if( slen != dev->api->read( dev, buf /*Flawfinder: ignore  This is safe in conjunction with slen. See above. */, slen ) )
    {
	free( buf );
	return whio_rc.IOError;
    }
    *tgt = buf;
    if( length ) *length = slen;
    return whio_rc.OK;
}

static const unsigned char whio_encode_pack_tag = 0xF0 | 'P';
size_t whio_encode_pack_calc_size( char const * fmt, size_t *itemCount )
{
    if( ! fmt || !*fmt ) return 0;
    char const * at = fmt;
    size_t rc = 1 + whio_sizeof_encoded_uint8;
    size_t exp = 0;
    for( ;at && *at; ++at )
    {
        exp =0;
        switch( *at )
        {
          case ' ':
          case '+':
          case '-': continue;
          case '1': exp = whio_sizeof_encoded_uint8;
              break;
          case '2': exp = whio_sizeof_encoded_uint16;
              break;
          case '4': exp = whio_sizeof_encoded_uint32;
              break;
          case '8': exp = whio_sizeof_encoded_uint64;
              break;
          case 'S': exp = whio_sizeof_encoded_size_t;
              break;
          default: return 0;
        }
        if( ! exp ) return 0;
        if( itemCount ) ++(*itemCount);
        rc += exp;
    }
    return rc;
}

size_t whio_encode_pack( void * dest, size_t * itemsWritten, char const * fmt, ... )
{
    va_list va;
    size_t rc = 0;
    va_start(va,fmt);
    rc = whio_encode_pack_v( dest, itemsWritten, fmt, va );
    va_end(va);
    return rc;
}

size_t whio_encode_pack_v( void * dest_, size_t * itemCount, char const * fmt, va_list va )
{

    if( ! fmt || !dest_ ) return 0;
    size_t rc = 0;
    size_t shouldItems = 0;
    size_t const shouldBytes = whio_encode_pack_calc_size( fmt, &shouldItems );
    unsigned char * dest = (unsigned char *)dest_;
    char const * at = fmt;
    *(dest++) = whio_encode_pack_tag;
    unsigned char * countPos = dest;
    dest += whio_sizeof_encoded_uint8;
    rc = (dest - (unsigned char const *)dest_);
    bool isSigned = false;
    size_t ck;
    size_t count = 0;
    size_t exp = 0;
    for( ;at && *at; ++at )
    {
        ck = 0;
        exp = 0;
        switch( *at )
        {
          case ' ': continue;
          case '+':
          case '-': isSigned = true;
              continue;
#define CASE(TAG,STYPE,UTYPE) if( isSigned ) {       \
                  isSigned = false; \
                  ck = whio_encode_##TAG( dest, va_arg(va,STYPE)); \
                  exp = whio_sizeof_encoded_##TAG; \
              } else { \
                  ck = whio_encode_u##TAG( dest, va_arg(va,UTYPE));  \
                  exp = whio_sizeof_encoded_u##TAG;                     \
              } break
          case '1':
              CASE(int8,int,int);
              // ^^^ gcc complaints about type promotion if i pass a uint8_t there.
              // It threatens to abort() if the code is run that way.
          case '2':
              CASE(int16,int,int); // <<--- see uint8_t notes above
          case '4':
              CASE(int32,int32_t,uint32_t);
          case '8':
              CASE(int64,int64_t,uint64_t);
              break;
          case 'S':
              isSigned = false;
              ck = whio_encode_size_t( dest, va_arg(va,whio_size_t));
              exp = whio_sizeof_encoded_size_t;
              break;
          default: continue;
#undef CASE
        }
        if( ! ck || (ck != exp) ) return rc;
        ++count;
        rc += ck;
        dest += ck;
    }
    if( itemCount ) *itemCount = count;
    if( shouldBytes == rc )
    {
        whio_encode_uint8( countPos, count );
    }
    return rc;
}


int whio_decode_pack( void const * src, size_t * itemCount, char const * fmt, ... )
{
    va_list va;
    int rc = 0;
    va_start(va,fmt);
    rc = whio_decode_pack_v( src, itemCount, fmt, va );
    va_end(va);
    return rc;
}

int whio_decode_pack_v( void const * src_, size_t * itemCount, char const * fmt, va_list va )
{

    if( ! fmt || !src_ ) return 0;
    int rc = 0;
    size_t shouldItems = 0;
    size_t const shouldBytes = whio_encode_pack_calc_size( fmt, &shouldItems );
    unsigned char const * src = (unsigned char const *)src_;
    char const * at = fmt;
    if( *(src++) != whio_encode_pack_tag )
    {
        return whio_rc.ConsistencyError;
    }
    uint8_t shouldPackedItems = 0;
    rc = whio_decode_uint8( src, &shouldPackedItems );
    if( (whio_rc.OK != rc) || !shouldPackedItems )
    {
        return whio_rc.ConsistencyError;
    }
    src += whio_sizeof_encoded_uint8;
    bool isSigned = false;
    size_t count = 0;
    size_t exp = 0;
    for( ;at && *at; ++at )
    {
        rc = 0;
        exp = 0;
        switch( *at )
        {
          case ' ': continue;
          case '+':
          case '-': isSigned = true;
              continue;
#define CASE(TAG) if( isSigned ) {       \
                  isSigned = false; \
                  rc = whio_decode_##TAG( src, va_arg(va,TAG##_t*)); \
                  exp = whio_sizeof_encoded_##TAG; \
              } else { \
                  rc = whio_decode_u##TAG( src, va_arg(va,u##TAG##_t*));  \
                  exp = whio_sizeof_encoded_u##TAG;                     \
              } break
          case '1':
              CASE(int8);
          case '2':
              CASE(int16);
          case '4':
              CASE(int32);
          case '8':
              CASE(int64);
              break;
          case 'S':
              rc = whio_decode_size_t( src, va_arg(va,whio_size_t*));
              exp = whio_sizeof_encoded_size_t;
              break;
          default: continue;
#undef CASE
        }
        if( rc != whio_rc.OK ) return rc;
        if( ! exp ) return whio_rc.RangeError;
        ++count;
        src += exp;
    }
    if( itemCount ) *itemCount = count;
    if( whio_rc.OK == rc )
    {
        if( shouldBytes != (src-(unsigned char const*)src_) )
        {
            rc = whio_rc.ConsistencyError;
        }
        else if( shouldPackedItems != count )
        {
            rc = whio_rc.ConsistencyError;
        }
    }
    return rc;
}
/* end file src/whio_encode.c */
/* begin file src/whprintf.c */
#line 8 "src/whprintf.c"
/************************************************************************
The printf-like implementation in this file is based on the one found
in the sqlite3 distribution is in the Public Domain.

This copy was forked for use with the clob API in Feb 2008 by Stephan
Beal (http://wanderinghorse.net/home/stephan/) and modified to send
its output to arbitrary targets via a callback mechanism. Also
refactored the %X specifier handlers a bit to make adding/removing
specific handlers easier.

All code in this file is released into the Public Domain.

The printf implementation (whprintfv()) is pretty easy to extend
(e.g. adding or removing %-specifiers for whprintfv()) if you're
willing to poke around a bit and see how the specifiers are declared
and dispatched. For an example, grep for 'etSTRING' and follow it
through the process of declaration to implementation.

See below for several WHPRINTF_OMIT_xxx macros which can be set to
remove certain features/extensions.
************************************************************************/

#if !defined(_ISOC99_SOURCE)
#define _ISOC99_SOURCE 1 /* needed for snprintf() */
#endif

#include <stdio.h> /* FILE */
#include <string.h> /* strlen() */
#include <stdlib.h> /* free/malloc() */
#include <ctype.h>
#include <stdint.h>
typedef long double LONGDOUBLE_TYPE;

/*
   If WHPRINTF_OMIT_FLOATING_POINT is defined to a true value, then
   floating point conversions are disabled.
*/
#ifndef WHPRINTF_OMIT_FLOATING_POINT
#  define WHPRINTF_OMIT_FLOATING_POINT 0
#endif

/*
   If WHPRINTF_OMIT_SIZE is defined to a true value, then
   the %n specifier is disabled.
*/
#ifndef WHPRINTF_OMIT_SIZE
#  define WHPRINTF_OMIT_SIZE 0
#endif

/*
   If WHPRINTF_OMIT_SQL is defined to a true value, then
   the %q and %Q specifiers are disabled.
*/
#ifndef WHPRINTF_OMIT_SQL
#  define WHPRINTF_OMIT_SQL 0
#endif

/*
   If WHPRINTF_OMIT_HTML is defined to a true value then the %h (HTML
   escape), %t (URL escape), and %T (URL unescape) specifiers are
   disabled.
*/
#ifndef WHPRINTF_OMIT_HTML
#  define WHPRINTF_OMIT_HTML 0
#endif

/*
Most C compilers handle variable-sized arrays, so we enable
that by default. Some (e.g. tcc) do not, so we provide a way
to disable it: set WHPRINTF_HAVE_VARARRAY to 0

One approach would be to look at:

  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)

but some compilers support variable-sized arrays even when not
explicitly running in c99 mode.
*/
#if !defined(WHPRINTF_HAVE_VARARRAY)
#  if defined(__TINYC__)
#    define WHPRINTF_HAVE_VARARRAY 0
#  else
#    define WHPRINTF_HAVE_VARARRAY 1
#  endif
#endif

/**
WHPRINTF_CHARARRAY is a helper to allocate variable-sized arrays.
This exists mainly so this code can compile with the tcc compiler.
*/
#if WHPRINTF_HAVE_VARARRAY
#  define WHPRINTF_CHARARRAY(V,N) char V[N+1]; memset(V,0,N+1);
#  define WHPRINTF_CHARARRAY_FREE(V)
#else
#  define WHPRINTF_CHARARRAY(V,N) char * V = (char *)malloc(N+1); memset(V,0,N+1);
#  define WHPRINTF_CHARARRAY_FREE(V) free(V)
#endif

/*
   Conversion types fall into various categories as defined by the
   following enumeration.
*/
enum PrintfCategory {etRADIX = 1, /* Integer types.  %d, %x, %o, and so forth */
		     etFLOAT = 2, /* Floating point.  %f */
		     etEXP = 3, /* Exponentional notation. %e and %E */
		     etGENERIC = 4, /* Floating or exponential, depending on exponent. %g */
		     etSIZE = 5, /* Return number of characters processed so far. %n */
		     etSTRING = 6, /* Strings. %s */
		     etDYNSTRING = 7, /* Dynamically allocated strings. %z */
		     etPERCENT = 8, /* Percent symbol. %% */
		     etCHARX = 9, /* Characters. %c */
/* The rest are extensions, not normally found in printf() */
		     etCHARLIT = 10, /* Literal characters.  %' */
#if !WHPRINTF_OMIT_SQL
		     etSQLESCAPE = 11, /* Strings with '\'' doubled.  %q */
		     etSQLESCAPE2 = 12, /* Strings with '\'' doubled and enclosed in '',
                          NULL pointers replaced by SQL NULL.  %Q */
		     etSQLESCAPE3 = 16, /* %w -> Strings with '\"' doubled */
#endif /* !WHPRINTF_OMIT_SQL */
		     etPOINTER = 15, /* The %p conversion */
		     etORDINAL = 17, /* %r -> 1st, 2nd, 3rd, 4th, etc.  English only */
#if ! WHPRINTF_OMIT_HTML
                     etHTML = 18, /* %h -> basic HTML escaping. */
                     etURLENCODE = 19, /* %t -> URL encoding. */
                     etURLDECODE = 20, /* %T -> URL decoding. */
#endif
		     etPLACEHOLDER = 100
};

/*
   An "etByte" is an 8-bit unsigned value.
*/
typedef unsigned char etByte;

/*
   Each builtin conversion character (ex: the 'd' in "%d") is described
   by an instance of the following structure
*/
typedef struct et_info {   /* Information about each format field */
  char fmttype;            /* The format field code letter */
  etByte base;             /* The base for radix conversion */
  etByte flags;            /* One or more of FLAG_ constants below */
  etByte type;             /* Conversion paradigm */
  etByte charset;          /* Offset into aDigits[] of the digits string */
  etByte prefix;           /* Offset into aPrefix[] of the prefix string */
} et_info;

/*
   Allowed values for et_info.flags
*/
enum et_info_flags { FLAG_SIGNED = 1,    /* True if the value to convert is signed */
		     FLAG_EXTENDED = 2,  /* True if for internal/extended use only. */
		     FLAG_STRING = 4     /* Allow infinity precision */
};

/*
  Historically, the following table was searched linearly, so the most
  common conversions were kept at the front.

  Change 2008 Oct 31 by Stephan Beal: we reserve an array or ordered
  entries for all chars in the range [32..126]. Format character
  checks can now be done in constant time by addressing that array
  directly.  This takes more static memory, but reduces the time and
  per-call overhead costs of whprintfv().
*/
static const char aDigits[] = "0123456789ABCDEF0123456789abcdef";
static const char aPrefix[] = "-x0\000X0";
static const et_info fmtinfo[] = {
/**
   If WHPRINTF_FMTINFO_FIXED is 1 then we use the original
   implementation: a linear list of entries. Search time is linear. If
   WHPRINTF_FMTINFO_FIXED is 0 then we use a fixed-size array which
   we index directly using the format char as the key.
*/
#define WHPRINTF_FMTINFO_FIXED 0
#if WHPRINTF_FMTINFO_FIXED
  {  'd', 10, FLAG_SIGNED, etRADIX,      0,  0 },
  {  's',  0, FLAG_STRING, etSTRING,     0,  0 },
  {  'g',  0, FLAG_SIGNED, etGENERIC,    30, 0 },
  {  'z',  0, FLAG_STRING, etDYNSTRING,  0,  0 },
  {  'c',  0, 0, etCHARX,      0,  0 },
  {  'o',  8, 0, etRADIX,      0,  2 },
  {  'u', 10, 0, etRADIX,      0,  0 },
  {  'x', 16, 0, etRADIX,      16, 1 },
  {  'X', 16, 0, etRADIX,      0,  4 },
  {  'i', 10, FLAG_SIGNED, etRADIX,      0,  0 },
#if !WHPRINTF_OMIT_FLOATING_POINT
  {  'f',  0, FLAG_SIGNED, etFLOAT,      0,  0 },
  {  'e',  0, FLAG_SIGNED, etEXP,        30, 0 },
  {  'E',  0, FLAG_SIGNED, etEXP,        14, 0 },
  {  'G',  0, FLAG_SIGNED, etGENERIC,    14, 0 },
#endif /* !WHPRINTF_OMIT_FLOATING_POINT */
  {  '%',  0, 0, etPERCENT,    0,  0 },
  {  'p', 16, 0, etPOINTER,    0,  1 },
  {  'r', 10, (FLAG_EXTENDED|FLAG_SIGNED), etORDINAL,    0,  0 },
#if ! WHPRINTF_OMIT_SQL
  {  'q',  0, FLAG_STRING, etSQLESCAPE,  0,  0 },
  {  'Q',  0, FLAG_STRING, etSQLESCAPE2, 0,  0 },
  {  'w',  0, FLAG_STRING, etSQLESCAPE3, 0,  0 },
#endif /* !WHPRINTF_OMIT_SQL */
#if ! WHPRINTF_OMIT_HTML
  {  'h',  0, FLAG_STRING, etHTML, 0, 0 },
  {  't',  0, FLAG_STRING, etURLENCODE, 0, 0 },
  {  'T',  0, FLAG_STRING, etURLDECODE, 0, 0 },
#endif /* !WHPRINTF_OMIT_HTML */
#if !WHPRINTF_OMIT_SIZE
  {  'n',  0, 0, etSIZE,       0,  0 },
#endif
#else /* WHPRINTF_FMTINFO_FIXED */
  /*
    These entries MUST stay in ASCII order, sorted
    on their fmttype member!
  */
  {' '/*32*/, 0, 0, 0, 0, 0 },
  {'!'/*33*/, 0, 0, 0, 0, 0 },
  {'"'/*34*/, 0, 0, 0, 0, 0 },
  {'#'/*35*/, 0, 0, 0, 0, 0 },
  {'$'/*36*/, 0, 0, 0, 0, 0 },
  {'%'/*37*/, 0, 0, etPERCENT, 0, 0 },
  {'&'/*38*/, 0, 0, 0, 0, 0 },
  {'\''/*39*/, 0, 0, 0, 0, 0 },
  {'('/*40*/, 0, 0, 0, 0, 0 },
  {')'/*41*/, 0, 0, 0, 0, 0 },
  {'*'/*42*/, 0, 0, 0, 0, 0 },
  {'+'/*43*/, 0, 0, 0, 0, 0 },
  {','/*44*/, 0, 0, 0, 0, 0 },
  {'-'/*45*/, 0, 0, 0, 0, 0 },
  {'.'/*46*/, 0, 0, 0, 0, 0 },
  {'/'/*47*/, 0, 0, 0, 0, 0 },
  {'0'/*48*/, 0, 0, 0, 0, 0 },
  {'1'/*49*/, 0, 0, 0, 0, 0 },
  {'2'/*50*/, 0, 0, 0, 0, 0 },
  {'3'/*51*/, 0, 0, 0, 0, 0 },
  {'4'/*52*/, 0, 0, 0, 0, 0 },
  {'5'/*53*/, 0, 0, 0, 0, 0 },
  {'6'/*54*/, 0, 0, 0, 0, 0 },
  {'7'/*55*/, 0, 0, 0, 0, 0 },
  {'8'/*56*/, 0, 0, 0, 0, 0 },
  {'9'/*57*/, 0, 0, 0, 0, 0 },
  {':'/*58*/, 0, 0, 0, 0, 0 },
  {';'/*59*/, 0, 0, 0, 0, 0 },
  {'<'/*60*/, 0, 0, 0, 0, 0 },
  {'='/*61*/, 0, 0, 0, 0, 0 },
  {'>'/*62*/, 0, 0, 0, 0, 0 },
  {'?'/*63*/, 0, 0, 0, 0, 0 },
  {'@'/*64*/, 0, 0, 0, 0, 0 },
  {'A'/*65*/, 0, 0, 0, 0, 0 },
  {'B'/*66*/, 0, 0, 0, 0, 0 },
  {'C'/*67*/, 0, 0, 0, 0, 0 },
  {'D'/*68*/, 0, 0, 0, 0, 0 },
  {'E'/*69*/, 0, FLAG_SIGNED, etEXP, 14, 0 },
  {'F'/*70*/, 0, 0, 0, 0, 0 },
  {'G'/*71*/, 0, FLAG_SIGNED, etGENERIC, 14, 0 },
  {'H'/*72*/, 0, 0, 0, 0, 0 },
  {'I'/*73*/, 0, 0, 0, 0, 0 },
  {'J'/*74*/, 0, 0, 0, 0, 0 },
  {'K'/*75*/, 0, 0, 0, 0, 0 },
  {'L'/*76*/, 0, 0, 0, 0, 0 },
  {'M'/*77*/, 0, 0, 0, 0, 0 },
  {'N'/*78*/, 0, 0, 0, 0, 0 },
  {'O'/*79*/, 0, 0, 0, 0, 0 },
  {'P'/*80*/, 0, 0, 0, 0, 0 },
  {'Q'/*81*/, 0, FLAG_STRING, etSQLESCAPE2, 0, 0 },
  {'R'/*82*/, 0, 0, 0, 0, 0 },
  {'S'/*83*/, 0, 0, 0, 0, 0 },
  {'T'/*84*/,  0, FLAG_STRING, etURLDECODE, 0, 0 },
  {'U'/*85*/, 0, 0, 0, 0, 0 },
  {'V'/*86*/, 0, 0, 0, 0, 0 },
  {'W'/*87*/, 0, 0, 0, 0, 0 },
  {'X'/*88*/, 16, 0, etRADIX,      0,  4 },
  {'Y'/*89*/, 0, 0, 0, 0, 0 },
  {'Z'/*90*/, 0, 0, 0, 0, 0 },
  {'['/*91*/, 0, 0, 0, 0, 0 },
  {'\\'/*92*/, 0, 0, 0, 0, 0 },
  {']'/*93*/, 0, 0, 0, 0, 0 },
  {'^'/*94*/, 0, 0, 0, 0, 0 },
  {'_'/*95*/, 0, 0, 0, 0, 0 },
  {'`'/*96*/, 0, 0, 0, 0, 0 },
  {'a'/*97*/, 0, 0, 0, 0, 0 },
  {'b'/*98*/, 0, 0, 0, 0, 0 },
  {'c'/*99*/, 0, 0, etCHARX,      0,  0 },
  {'d'/*100*/, 10, FLAG_SIGNED, etRADIX,      0,  0 },
  {'e'/*101*/, 0, FLAG_SIGNED, etEXP,        30, 0 },
  {'f'/*102*/, 0, FLAG_SIGNED, etFLOAT,      0,  0},
  {'g'/*103*/, 0, FLAG_SIGNED, etGENERIC,    30, 0 },
  {'h'/*104*/, 0, FLAG_STRING, etHTML, 0, 0 },
  {'i'/*105*/, 10, FLAG_SIGNED, etRADIX,      0,  0},
  {'j'/*106*/, 0, 0, 0, 0, 0 },
  {'k'/*107*/, 0, 0, 0, 0, 0 },
  {'l'/*108*/, 0, 0, 0, 0, 0 },
  {'m'/*109*/, 0, 0, 0, 0, 0 },
  {'n'/*110*/, 0, 0, etSIZE, 0, 0 },
  {'o'/*111*/, 8, 0, etRADIX,      0,  2 },
  {'p'/*112*/, 16, 0, etPOINTER, 0, 1 },
  {'q'/*113*/, 0, FLAG_STRING, etSQLESCAPE,  0, 0 },
  {'r'/*114*/, 10, (FLAG_EXTENDED|FLAG_SIGNED), etORDINAL,    0,  0},
  {'s'/*115*/, 0, FLAG_STRING, etSTRING,     0,  0 },
  {'t'/*116*/,  0, FLAG_STRING, etURLENCODE, 0, 0 },
  {'u'/*117*/, 10, 0, etRADIX,      0,  0 },
  {'v'/*118*/, 0, 0, 0, 0, 0 },
  {'w'/*119*/, 0, FLAG_STRING, etSQLESCAPE3, 0, 0 },
  {'x'/*120*/, 16, 0, etRADIX,      16, 1  },
  {'y'/*121*/, 0, 0, 0, 0, 0 },
  {'z'/*122*/, 0, FLAG_STRING, etDYNSTRING,  0,  0},
  {'{'/*123*/, 0, 0, 0, 0, 0 },
  {'|'/*124*/, 0, 0, 0, 0, 0 },
  {'}'/*125*/, 0, 0, 0, 0, 0 },
  {'~'/*126*/, 0, 0, 0, 0, 0 },
#endif /* WHPRINTF_FMTINFO_FIXED */
};
#define etNINFO  (sizeof(fmtinfo)/sizeof(fmtinfo[0]))

#if ! WHPRINTF_OMIT_FLOATING_POINT
/*
   "*val" is a double such that 0.1 <= *val < 10.0
   Return the ascii code for the leading digit of *val, then
   multiply "*val" by 10.0 to renormalize.
**
   Example:
       input:     *val = 3.14159
       output:    *val = 1.4159    function return = '3'
**
   The counter *cnt is incremented each time.  After counter exceeds
   16 (the number of significant digits in a 64-bit float) '0' is
   always returned.
*/
static int et_getdigit(LONGDOUBLE_TYPE *val, int *cnt){
  int digit;
  LONGDOUBLE_TYPE d;
  if( (*cnt)++ >= 16 ) return '0';
  digit = (int)*val;
  d = digit;
  digit += '0';
  *val = (*val - d)*10.0;
  return digit;
}
#endif /* !WHPRINTF_OMIT_FLOATING_POINT */

/*
   On machines with a small(?) stack size, you can redefine the
   WHPRINTF_BUF_SIZE to be less than 350.  But beware - for smaller
   values some %f conversions may go into an infinite loop.
*/
#ifndef WHPRINTF_BUF_SIZE
#  define WHPRINTF_BUF_SIZE 350  /* Size of the output buffer for numeric conversions */
#endif

#if ! defined(__STDC__) && !defined(__TINYC__)
#ifdef WHPRINTF_INT64_TYPE
  typedef WHPRINTF_INT64_TYPE int64_t;
  typedef unsigned WHPRINTF_INT64_TYPE uint64_t;
#elif defined(_MSC_VER) || defined(__BORLANDC__)
  typedef __int64 int64_t;
  typedef unsigned __int64 uint64_t;
#else
  typedef long long int int64_t;
  typedef unsigned long long int uint64_t;
#endif
#endif

#if 0
/   Not yet used. */
enum PrintfArgTypes {
TypeInt = 0,
TypeIntP = 1,
TypeFloat = 2,
TypeFloatP = 3,
TypeCString = 4
};
#endif


#if 0
/   Not yet used. */
typedef struct whprintf_spec_handler_def
{
	char letter; /   e.g. %s */
	int xtype; /* reference to the etXXXX values, or fmtinfo[*].type. */
	int ntype; /* reference to PrintfArgTypes enum. */
} spec_handler;
#endif

/**
   whprintf_spec_handler is an almost-generic interface for farming
   work out of whprintfv()'s code into external functions.  It doesn't
   actually save much (if any) overall code, but it makes the whprintfv()
   code more manageable.


   REQUIREMENTS of implementations:

   - Expects an implementation-specific vargp pointer.
   whprintfv() passes a pointer to the converted value of
   an entry from the format va_list. If it passes a type
   other than the expected one, undefined results.

   - If it calls pf then it must return the return value
   from that function.

   - If it calls pf it must do: pf( pfArg, D, N ), where D is
   the data to export and N is the number of bytes to export.
   It may call pf() an arbitrary number of times

   - If pf() successfully is called, the return value must be the
   accumulated totals of its return value(s), plus (possibly, but
   unlikely) an imnplementation-specific amount.

   - If it does not call pf() then it must return 0 (success)
   or a negative number (an error) or do all of the export
   processing itself and return the number of bytes exported.


   SIGNIFICANT LIMITATIONS:

   - Has no way of iterating over the format string,
   so handling precisions and such here can't work too
   well.
*/
typedef long (*whprintf_spec_handler)( whprintf_appender pf,
				       void * pfArg,
				       void * vargp );


/**
  whprintf_spec_handler for etSTRING types. It assumes that varg is a
  null-terminated (char [const] *)
*/
static long spech_string( whprintf_appender pf,
			  void * pfArg,
			  void * varg )
{
	char const * ch = (char const *) varg;
	return ch ? pf( pfArg, ch, strlen(ch) ) : 0;
}

/**
  whprintf_spec_handler for etDYNSTRING types.  It assumes that varg
  is a non-const (char *). It behaves identically to spec_string() and
  then calls free() on that (char *).
*/
static long spech_dynstring( whprintf_appender pf,
			     void * pfArg,
			     void * varg )
{
  long ret = spech_string( pf, pfArg, varg );
  free( (char *) varg );
  return ret;
}

#if !WHPRINTF_OMIT_HTML
static long spech_string_to_html( whprintf_appender pf,
                                  void * pfArg,
                                  void * varg )
{
    char const * ch = (char const *) varg;
    long ret = 0;
    if( ! ch ) return 0;
    ret = 0;
    for( ; *ch; ++ch )
    {
        switch( *ch )
        {
          case '<': ret += pf( pfArg, "&lt;", 4 );
              break;
          case '&': ret += pf( pfArg, "&amp;", 5 );
              break;
          default:
              ret += pf( pfArg, ch, 1 );
              break;
        };
    }
    return ret;
}

static int httpurl_needs_escape( int c )
{
    /*
      Definition of "safe" and "unsafe" chars
      was taken from:

      http://www.codeguru.com/cpp/cpp/cpp_mfc/article.php/c4029/
    */
    return ( (c >= 32 && c <=47)
             || ( c>=58 && c<=64)
             || ( c>=91 && c<=96)
             || ( c>=123 && c<=126)
             || ( c<32 || c>=127)
             );
}

/**
   The handler for the etURLENCODE specifier.

   It expects varg to be a string value, which it will preceed to
   encode using an URL encoding algothrim (certain characters are
   converted to %XX, where XX is their hex value) and passes the
   encoded string to pf(). It returns the total length of the output
   string.
 */
static long spech_urlencode( whprintf_appender pf,
                             void * pfArg,
                             void * varg )
{
    char const * str = (char const *) varg;
    long ret = 0;
    char ch = 0;
    char const * hex = "0123456789ABCDEF";
#define xbufsz 10
    char xbuf[xbufsz];
    int slen = 0;
    if( ! str ) return 0;
    memset( xbuf, 0, xbufsz );
    ch = *str;
#define xbufsz 10
    slen = 0;
    for( ; ch; ch = *(++str) )
    {
        if( ! httpurl_needs_escape( ch ) )
        {
            ret += pf( pfArg, str, 1 );
            continue;
        }
        else {
            slen = snprintf( xbuf, xbufsz, "%%%c%c",
                             hex[((ch>>4)&0xf)],
                             hex[(ch&0xf)]);
            ret += pf( pfArg, xbuf, slen );
        }
    }
#undef xbufsz
    return ret;
}

/* 
   hexchar_to_int():

   For 'a'-'f', 'A'-'F' and '0'-'9', returns the appropriate decimal
   number.  For any other character it returns -1.
    */
static int hexchar_to_int( int ch )
{
    if( (ch>='a' && ch<='f') ) return ch-'a'+10;
    else if( (ch>='A' && ch<='F') ) return ch-'A'+10;
    else if( (ch>='0' && ch<='9') ) return ch-'0';
    return -1;
}

/**
   The handler for the etURLDECODE specifier.

   It expects varg to be a ([const] char *), possibly encoded
   with URL encoding. It decodes the string using a URL decode
   algorithm and passes the decoded string to
   pf(). It returns the total length of the output string.
   If the input string contains malformed %XX codes then this
   function will return prematurely.
 */
static long spech_urldecode( whprintf_appender pf,
                             void * pfArg,
                             void * varg )
{
    char const * str = (char const *) varg;
    if( ! str ) return 0;
    long ret = 0;
    char ch = 0;
    char ch2 = 0;
    char xbuf[4];
    int decoded;
    ch = *str;
    while( ch )
    {
        if( ch == '%' )
        {
            ch = *(++str);
            ch2 = *(++str);
            if( isxdigit(ch) &&
                isxdigit(ch2) )
            {
                decoded = (hexchar_to_int( ch ) * 16)
                    + hexchar_to_int( ch2 );
                //printf("DECODED: %s, %d, %c\n", xbuf, decoded, (char) decoded );
                xbuf[0] = (char)decoded;
                xbuf[1] = 0;
                ret += pf( pfArg, xbuf, 1 );
                ch = *(++str);
                continue;
            }
            else
            {
                xbuf[0] = '%';
                xbuf[1] = ch;
                xbuf[2] = ch2;
                xbuf[3] = 0;
                ret += pf( pfArg, xbuf, 3 );
                ch = *(++str);
                continue;
            }
        }
        else if( ch == '+' )
        {
            xbuf[0] = ' ';
            xbuf[1] = 0;
            ret += pf( pfArg, xbuf, 1 );
            ch = *(++str);
            continue;
        }
        xbuf[0] = ch;
        xbuf[1] = 0;
        ret += pf( pfArg, xbuf, 1 );
        ch = *(++str);
    }
    return ret;
}

#endif /* !WHPRINTF_OMIT_HTML */


#if !WHPRINTF_OMIT_SQL
/**
   Quotes the (char *) varg as an SQL string 'should'
   be quoted. The exact type of the conversion
   is specified by xtype, which must be one of
   etSQLESCAPE, etSQLESCAPE2, or etSQLESCAPE3.

   Search this file for those constants to find
   the associated documentation.
*/
static long spech_sqlstring_main( int xtype,
				  whprintf_appender pf,
				  void * pfArg,
				  void * varg )
{
        int i, j, n, ch, isnull;
        int needQuote;
        char q = ((xtype==etSQLESCAPE3)?'"':'\'');   /* Quote character */
        char const * escarg = (char const *) varg;
	char * bufpt = 0;
        isnull = escarg==0;
        if( isnull ) escarg = (xtype==etSQLESCAPE2 ? "NULL" : "(NULL)");
        for(i=n=0; (ch=escarg[i])!=0; i++){
          if( ch==q )  n++;
        }
        needQuote = !isnull && xtype==etSQLESCAPE2;
        n += i + 1 + needQuote*2;
	// FIXME: use a static buffer here instead of malloc()! Shame on you!
	bufpt = (char *)malloc( n );
	if( ! bufpt ) return -1;
        j = 0;
        if( needQuote ) bufpt[j++] = q;
        for(i=0; (ch=escarg[i])!=0; i++){
          bufpt[j++] = ch;
          if( ch==q ) bufpt[j++] = ch;
        }
        if( needQuote ) bufpt[j++] = q;
        bufpt[j] = 0;
	long ret = pf( pfArg, bufpt, j );
	free( bufpt );
	return ret;
}

static long spech_sqlstring1( whprintf_appender pf,
			      void * pfArg,
			      void * varg )
{
	return spech_sqlstring_main( etSQLESCAPE, pf, pfArg, varg );
}

static long spech_sqlstring2( whprintf_appender pf,
			      void * pfArg,
			      void * varg )
{
	return spech_sqlstring_main( etSQLESCAPE2, pf, pfArg, varg );
}

static long spech_sqlstring3( whprintf_appender pf,
			      void * pfArg,
			      void * varg )
{
	return spech_sqlstring_main( etSQLESCAPE3, pf, pfArg, varg );
}

#endif /* !WHPRINTF_OMIT_SQL */

				      

/*
   The root printf program.  All variations call this core.  It
   implements most of the common printf behaviours plus (optionally)
   some extended ones.

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
   function "func".  Returns -1 on a error.

   Note that the order in which automatic variables are declared below
   seems to make a big difference in determining how fast this beast
   will run.

   Much of this code dates back to the early 1980's, supposedly.

   Known change history (most historic info has been lost):

   10 Feb 2008 by Stephan Beal: refactored to remove the 'useExtended'
   flag (which is now always on). Added the whprintf_appender typedef to
   make this function generic enough to drop into other source trees
   without much work.

   31 Oct 2008 by Stephan Beal: refactored the et_info lookup to be
   constant-time instead of linear.
*/
long whprintfv(
  whprintf_appender pfAppend,          /* Accumulate results here */
  void * pfAppendArg,                /* Passed as first arg to pfAppend. */
  const char *fmt,                   /* Format string */
  va_list ap                         /* arguments */
){
    /**
       HISTORIC NOTE (author and year unknown):

       Note that the order in which automatic variables are declared below
       seems to make a big difference in determining how fast this beast
       will run.
    */

#if WHPRINTF_FMTINFO_FIXED
  const int useExtended = 1; /* Allow extended %-conversions */
#endif
  long outCount = 0;          /* accumulated output count */
  int pfrc = 0;              /* result from calling pfAppend */
  int c;                     /* Next character in the format string */
  char *bufpt = 0;           /* Pointer to the conversion buffer */
  int precision;             /* Precision of the current field */
  int length;                /* Length of the field */
  int idx;                   /* A general purpose loop counter */
  int width;                 /* Width of the current field */
  etByte flag_leftjustify;   /* True if "-" flag is present */
  etByte flag_plussign;      /* True if "+" flag is present */
  etByte flag_blanksign;     /* True if " " flag is present */
  etByte flag_alternateform; /* True if "#" flag is present */
  etByte flag_altform2;      /* True if "!" flag is present */
  etByte flag_zeropad;       /* True if field width constant starts with zero */
  etByte flag_long;          /* True if "l" flag is present */
  etByte flag_longlong;      /* True if the "ll" flag is present */
  etByte done;               /* Loop termination flag */
  uint64_t longvalue;   /* Value for integer types */
  LONGDOUBLE_TYPE realvalue; /* Value for real types */
  const et_info *infop = 0;      /* Pointer to the appropriate info structure */
  char buf[WHPRINTF_BUF_SIZE];       /* Conversion buffer */
  char prefix;               /* Prefix character.  "+" or "-" or " " or '\0'. */
  etByte errorflag = 0;      /* True if an error is encountered */
  etByte xtype = 0;              /* Conversion paradigm */
  char * zExtra = 0;              /* Extra memory used for etTCLESCAPE conversions */
#if ! WHPRINTF_OMIT_FLOATING_POINT
  int  exp, e2;              /* exponent of real numbers */
  double rounder;            /* Used for rounding floating point values */
  etByte flag_dp;            /* True if decimal point should be shown */
  etByte flag_rtz;           /* True if trailing zeros should be removed */
  etByte flag_exp;           /* True to force display of the exponent */
  int nsd;                   /* Number of significant digits returned */
#endif


  /* WHPRINTF_RETURN, WHPRINTF_CHECKERR, and WHPRINTF_SPACES
     are internal helpers.
  */
#define WHPRINTF_RETURN if( zExtra ) free(zExtra); return outCount;
#define WHPRINTF_CHECKERR(FREEME) if( pfrc<0 ) { WHPRINTF_CHARARRAY_FREE(FREEME); WHPRINTF_RETURN; } else outCount += pfrc;
#define WHPRINTF_SPACES(N) \
if(1){				       \
    WHPRINTF_CHARARRAY(zSpaces,N);		      \
    memset( zSpaces,' ',N);			      \
    pfrc = pfAppend(pfAppendArg, zSpaces, N);	      \
    WHPRINTF_CHECKERR(zSpaces);			      \
    WHPRINTF_CHARARRAY_FREE(zSpaces);		      \
}

  length = 0;
  bufpt = 0;
  for(; (c=(*fmt))!=0; ++fmt){
    if( c!='%' ){
      int amt;
      bufpt = (char *)fmt;
      amt = 1;
      while( (c=(*++fmt))!='%' && c!=0 ) amt++;
      pfrc = pfAppend( pfAppendArg, bufpt, amt);
      WHPRINTF_CHECKERR(0);
      if( c==0 ) break;
    }
    if( (c=(*++fmt))==0 ){
      errorflag = 1;
      pfrc = pfAppend( pfAppendArg, "%", 1);
      WHPRINTF_CHECKERR(0);
      break;
    }
    /* Find out what flags are present */
    flag_leftjustify = flag_plussign = flag_blanksign = 
     flag_alternateform = flag_altform2 = flag_zeropad = 0;
    done = 0;
    do{
      switch( c ){
        case '-':   flag_leftjustify = 1;     break;
        case '+':   flag_plussign = 1;        break;
        case ' ':   flag_blanksign = 1;       break;
        case '#':   flag_alternateform = 1;   break;
        case '!':   flag_altform2 = 1;        break;
        case '0':   flag_zeropad = 1;         break;
        default:    done = 1;                 break;
      }
    }while( !done && (c=(*++fmt))!=0 );
    /* Get the field width */
    width = 0;
    if( c=='*' ){
      width = va_arg(ap,int);
      if( width<0 ){
        flag_leftjustify = 1;
        width = -width;
      }
      c = *++fmt;
    }else{
      while( c>='0' && c<='9' ){
        width = width*10 + c - '0';
        c = *++fmt;
      }
    }
    if( width > WHPRINTF_BUF_SIZE-10 ){
      width = WHPRINTF_BUF_SIZE-10;
    }
    /* Get the precision */
    if( c=='.' ){
      precision = 0;
      c = *++fmt;
      if( c=='*' ){
        precision = va_arg(ap,int);
        if( precision<0 ) precision = -precision;
        c = *++fmt;
      }else{
        while( c>='0' && c<='9' ){
          precision = precision*10 + c - '0';
          c = *++fmt;
        }
      }
    }else{
      precision = -1;
    }
    /* Get the conversion type modifier */
    if( c=='l' ){
      flag_long = 1;
      c = *++fmt;
      if( c=='l' ){
        flag_longlong = 1;
        c = *++fmt;
      }else{
        flag_longlong = 0;
      }
    }else{
      flag_long = flag_longlong = 0;
    }
    /* Fetch the info entry for the field */
    infop = 0;
#if WHPRINTF_FMTINFO_FIXED
    for(idx=0; idx<etNINFO; idx++){
      if( c==fmtinfo[idx].fmttype ){
        infop = &fmtinfo[idx];
        if( useExtended || (infop->flags & FLAG_EXTENDED)==0 ){
          xtype = infop->type;
        }else{
	    WHPRINTF_RETURN;
        }
        break;
      }
    }
#else
#define FMTNDX(N) (N - fmtinfo[0].fmttype)
#define FMTINFO(N) (fmtinfo[ FMTNDX(N) ])
    infop = ((c>=(fmtinfo[0].fmttype)) && (c<fmtinfo[etNINFO-1].fmttype))
	? &FMTINFO(c)
	: 0;
    //fprintf(stderr,"char '%c'/%d @ %d,  type=%c/%d\n",c,c,FMTNDX(c),infop->fmttype,infop->type);
    if( infop ) xtype = infop->type;
#undef FMTINFO
#undef FMTNDX
#endif /* WHPRINTF_FMTINFO_FIXED */
    zExtra = 0;
    if( (!infop) || (!infop->type) ){
	WHPRINTF_RETURN;
    }


    /* Limit the precision to prevent overflowing buf[] during conversion */
    if( precision>WHPRINTF_BUF_SIZE-40 && (infop->flags & FLAG_STRING)==0 ){
      precision = WHPRINTF_BUF_SIZE-40;
    }

    /*
       At this point, variables are initialized as follows:
    **
         flag_alternateform          TRUE if a '#' is present.
         flag_altform2               TRUE if a '!' is present.
         flag_plussign               TRUE if a '+' is present.
         flag_leftjustify            TRUE if a '-' is present or if the
                                     field width was negative.
         flag_zeropad                TRUE if the width began with 0.
         flag_long                   TRUE if the letter 'l' (ell) prefixed
                                     the conversion character.
         flag_longlong               TRUE if the letter 'll' (ell ell) prefixed
                                     the conversion character.
         flag_blanksign              TRUE if a ' ' is present.
         width                       The specified field width.  This is
                                     always non-negative.  Zero is the default.
         precision                   The specified precision.  The default
                                     is -1.
         xtype                       The class of the conversion.
         infop                       Pointer to the appropriate info struct.
    */
    switch( xtype ){
      case etPOINTER:
        flag_longlong = sizeof(char*)==sizeof(int64_t);
        flag_long = sizeof(char*)==sizeof(long int);
        /* Fall through into the next case */
      case etORDINAL:
      case etRADIX:
        if( infop->flags & FLAG_SIGNED ){
          int64_t v;
          if( flag_longlong )   v = va_arg(ap,int64_t);
          else if( flag_long )  v = va_arg(ap,long int);
          else                  v = va_arg(ap,int);
          if( v<0 ){
            longvalue = -v;
            prefix = '-';
          }else{
            longvalue = v;
            if( flag_plussign )        prefix = '+';
            else if( flag_blanksign )  prefix = ' ';
            else                       prefix = 0;
          }
        }else{
          if( flag_longlong )   longvalue = va_arg(ap,uint64_t);
          else if( flag_long )  longvalue = va_arg(ap,unsigned long int);
          else                  longvalue = va_arg(ap,unsigned int);
          prefix = 0;
        }
        if( longvalue==0 ) flag_alternateform = 0;
        if( flag_zeropad && precision<width-(prefix!=0) ){
          precision = width-(prefix!=0);
        }
        bufpt = &buf[WHPRINTF_BUF_SIZE-1];
        if( xtype==etORDINAL ){
	    /** i sure would like to shake the hand of whoever figured this out: */
          static const char zOrd[] = "thstndrd";
          int x = longvalue % 10;
          if( x>=4 || (longvalue/10)%10==1 ){
            x = 0;
          }
          buf[WHPRINTF_BUF_SIZE-3] = zOrd[x*2];
          buf[WHPRINTF_BUF_SIZE-2] = zOrd[x*2+1];
          bufpt -= 2;
        }
        {
          const char *cset;
          int base;
          cset = &aDigits[infop->charset];
          base = infop->base;
          do{                                           /* Convert to ascii */
            *(--bufpt) = cset[longvalue%base];
            longvalue = longvalue/base;
          }while( longvalue>0 );
        }
        length = &buf[WHPRINTF_BUF_SIZE-1]-bufpt;
        for(idx=precision-length; idx>0; idx--){
          *(--bufpt) = '0';                             /* Zero pad */
        }
        if( prefix ) *(--bufpt) = prefix;               /* Add sign */
        if( flag_alternateform && infop->prefix ){      /* Add "0" or "0x" */
          const char *pre;
          char x;
          pre = &aPrefix[infop->prefix];
          if( *bufpt!=pre[0] ){
            for(; (x=(*pre))!=0; pre++) *(--bufpt) = x;
          }
        }
        length = &buf[WHPRINTF_BUF_SIZE-1]-bufpt;
        break;
      case etFLOAT:
      case etEXP:
      case etGENERIC:
        realvalue = va_arg(ap,double);
#if ! WHPRINTF_OMIT_FLOATING_POINT
        if( precision<0 ) precision = 6;         /* Set default precision */
        if( precision>WHPRINTF_BUF_SIZE/2-10 ) precision = WHPRINTF_BUF_SIZE/2-10;
        if( realvalue<0.0 ){
          realvalue = -realvalue;
          prefix = '-';
        }else{
          if( flag_plussign )          prefix = '+';
          else if( flag_blanksign )    prefix = ' ';
          else                         prefix = 0;
        }
        if( xtype==etGENERIC && precision>0 ) precision--;
#if 0
        /* Rounding works like BSD when the constant 0.4999 is used.  Wierd! */
        for(idx=precision, rounder=0.4999; idx>0; idx--, rounder*=0.1);
#else
        /* It makes more sense to use 0.5 */
        for(idx=precision, rounder=0.5; idx>0; idx--, rounder*=0.1){}
#endif
        if( xtype==etFLOAT ) realvalue += rounder;
        /* Normalize realvalue to within 10.0 > realvalue >= 1.0 */
        exp = 0;
#if 1
	if( (realvalue)!=(realvalue) ){
	    /* from sqlite3: #define sqlite3_isnan(X)  ((X)!=(X)) */
	    /* This weird array thing is to avoid constness violations
	       when assinging, e.g. "NaN" to bufpt.
	    */
	    static char NaN[4] = {'N','a','N','\0'};
	    bufpt = NaN;
          length = 3;
          break;
        }
#endif
        if( realvalue>0.0 ){
          while( realvalue>=1e32 && exp<=350 ){ realvalue *= 1e-32; exp+=32; }
          while( realvalue>=1e8 && exp<=350 ){ realvalue *= 1e-8; exp+=8; }
          while( realvalue>=10.0 && exp<=350 ){ realvalue *= 0.1; exp++; }
          while( realvalue<1e-8 && exp>=-350 ){ realvalue *= 1e8; exp-=8; }
          while( realvalue<1.0 && exp>=-350 ){ realvalue *= 10.0; exp--; }
          if( exp>350 || exp<-350 ){
            if( prefix=='-' ){
		static char Inf[5] = {'-','I','n','f','\0'};
		bufpt = Inf;
            }else if( prefix=='+' ){
		static char Inf[5] = {'+','I','n','f','\0'};
		bufpt = Inf;
            }else{
		static char Inf[4] = {'I','n','f','\0'};
		bufpt = Inf;
            }
            length = strlen(bufpt);
            break;
          }
        }
        bufpt = buf;
        /*
           If the field type is etGENERIC, then convert to either etEXP
           or etFLOAT, as appropriate.
        */
        flag_exp = xtype==etEXP;
        if( xtype!=etFLOAT ){
          realvalue += rounder;
          if( realvalue>=10.0 ){ realvalue *= 0.1; exp++; }
        }
        if( xtype==etGENERIC ){
          flag_rtz = !flag_alternateform;
          if( exp<-4 || exp>precision ){
            xtype = etEXP;
          }else{
            precision = precision - exp;
            xtype = etFLOAT;
          }
        }else{
          flag_rtz = 0;
        }
        if( xtype==etEXP ){
          e2 = 0;
        }else{
          e2 = exp;
        }
        nsd = 0;
        flag_dp = (precision>0) | flag_alternateform | flag_altform2;
        /* The sign in front of the number */
        if( prefix ){
          *(bufpt++) = prefix;
        }
        /* Digits prior to the decimal point */
        if( e2<0 ){
          *(bufpt++) = '0';
        }else{
          for(; e2>=0; e2--){
            *(bufpt++) = et_getdigit(&realvalue,&nsd);
          }
        }
        /* The decimal point */
        if( flag_dp ){
          *(bufpt++) = '.';
        }
        /* "0" digits after the decimal point but before the first
           significant digit of the number */
        for(e2++; e2<0 && precision>0; precision--, e2++){
          *(bufpt++) = '0';
        }
        /* Significant digits after the decimal point */
        while( (precision--)>0 ){
          *(bufpt++) = et_getdigit(&realvalue,&nsd);
        }
        /* Remove trailing zeros and the "." if no digits follow the "." */
        if( flag_rtz && flag_dp ){
          while( bufpt[-1]=='0' ) *(--bufpt) = 0;
          /* assert( bufpt>buf ); */
          if( bufpt[-1]=='.' ){
            if( flag_altform2 ){
              *(bufpt++) = '0';
            }else{
              *(--bufpt) = 0;
            }
          }
        }
        /* Add the "eNNN" suffix */
        if( flag_exp || (xtype==etEXP && exp) ){
          *(bufpt++) = aDigits[infop->charset];
          if( exp<0 ){
            *(bufpt++) = '-'; exp = -exp;
          }else{
            *(bufpt++) = '+';
          }
          if( exp>=100 ){
            *(bufpt++) = (exp/100)+'0';                /* 100's digit */
            exp %= 100;
          }
          *(bufpt++) = exp/10+'0';                     /* 10's digit */
          *(bufpt++) = exp%10+'0';                     /* 1's digit */
        }
        *bufpt = 0;

        /* The converted number is in buf[] and zero terminated. Output it.
           Note that the number is in the usual order, not reversed as with
           integer conversions. */
        length = bufpt-buf;
        bufpt = buf;

        /* Special case:  Add leading zeros if the flag_zeropad flag is
           set and we are not left justified */
        if( flag_zeropad && !flag_leftjustify && length < width){
          int i;
          int nPad = width - length;
          for(i=width; i>=nPad; i--){
            bufpt[i] = bufpt[i-nPad];
          }
          i = prefix!=0;
          while( nPad-- ) bufpt[i++] = '0';
          length = width;
        }
#endif /* !WHPRINTF_OMIT_FLOATING_POINT */
        break;
#if !WHPRINTF_OMIT_SIZE
      case etSIZE:
        *(va_arg(ap,int*)) = outCount;
        length = width = 0;
        break;
#endif
      case etPERCENT:
        buf[0] = '%';
        bufpt = buf;
        length = 1;
        break;
      case etCHARLIT:
      case etCHARX:
        c = buf[0] = (xtype==etCHARX ? va_arg(ap,int) : *++fmt);
        if( precision>=0 ){
          for(idx=1; idx<precision; idx++) buf[idx] = c;
          length = precision;
        }else{
          length =1;
        }
        bufpt = buf;
        break;
      case etSTRING:
      case etDYNSTRING: {
	  bufpt = va_arg(ap,char*);
	  whprintf_spec_handler spf = (xtype==etSTRING)
              ? spech_string : spech_dynstring;
	  pfrc = spf( pfAppend, pfAppendArg, bufpt );
	  WHPRINTF_CHECKERR(0);
	  length = 0;
	  if( precision>=0 && precision<length ) length = precision;
	}
        break;
#if ! WHPRINTF_OMIT_HTML
      case etHTML:
	  bufpt = va_arg(ap,char*);
	  pfrc = spech_string_to_html( pfAppend, pfAppendArg, bufpt );
	  WHPRINTF_CHECKERR(0);
	  length = 0;
        break;
      case etURLENCODE:
	  bufpt = va_arg(ap,char*);
	  pfrc = spech_urlencode( pfAppend, pfAppendArg, bufpt );
	  WHPRINTF_CHECKERR(0);
	  length = 0;
        break;
      case etURLDECODE:
          bufpt = va_arg(ap,char *);
	  pfrc = spech_urldecode( pfAppend, pfAppendArg, bufpt );
	  WHPRINTF_CHECKERR(0);
          length = 0;
          break;
#endif /* WHPRINTF_OMIT_HTML */
#if ! WHPRINTF_OMIT_SQL
      case etSQLESCAPE:
      case etSQLESCAPE2:
      case etSQLESCAPE3: {
	      whprintf_spec_handler spf =
		      (xtype==etSQLESCAPE)
		      ? spech_sqlstring1
		      : ((xtype==etSQLESCAPE2)
			 ? spech_sqlstring2
			 : spech_sqlstring3
			 );
	      bufpt = va_arg(ap,char*);
	      pfrc = spf( pfAppend, pfAppendArg, bufpt );
	      WHPRINTF_CHECKERR(0);
	      length = 0;
	      if( precision>=0 && precision<length ) length = precision;
      }
#endif /* !WHPRINTF_OMIT_SQL */
    }/* End switch over the format type */
    /*
       The text of the conversion is pointed to by "bufpt" and is
       "length" characters long.  The field width is "width".  Do
       the output.
    */
    if( !flag_leftjustify ){
      int nspace;
      nspace = width-length;
      if( nspace>0 ){
	      WHPRINTF_SPACES(nspace);
      }
    }
    if( length>0 ){
      pfrc = pfAppend( pfAppendArg, bufpt, length);
      WHPRINTF_CHECKERR(0);
    }
    if( flag_leftjustify ){
      int nspace;
      nspace = width-length;
      if( nspace>0 ){
	      WHPRINTF_SPACES(nspace);
      }
    }
    if( zExtra ){
      free(zExtra);
      zExtra = 0;
    }
  }/* End for loop over the format string */
  WHPRINTF_RETURN;
} /* End of function */


#undef WHPRINTF_SPACES
#undef WHPRINTF_CHECKERR
#undef WHPRINTF_RETURN
#undef WHPRINTF_OMIT_FLOATING_POINT
#undef WHPRINTF_OMIT_SIZE
#undef WHPRINTF_OMIT_SQL
#undef WHPRINTF_BUF_SIZE
#undef WHPRINTF_OMIT_HTML

long whprintf(whprintf_appender pfAppend,          /* Accumulate results here */
	    void * pfAppendArg,                /* Passed as first arg to pfAppend. */
	    const char *fmt,                   /* Format string */
	    ... )
{
	va_list vargs;
	va_start( vargs, fmt );
	long ret = whprintfv( pfAppend, pfAppendArg, fmt, vargs );
	va_end(vargs);
	return ret;
}


long whprintf_FILE_appender( void * a, char const * s, long n )
{
	FILE * fp = (FILE *)a;
	if( ! fp ) return -1;
	long ret = fwrite( s, sizeof(char), n, fp );
	return (ret >= 0) ? ret : -2;
}


long whprintf_file( FILE * fp, char const * fmt, ... )
{
	va_list vargs;
	va_start( vargs, fmt );
	int ret = whprintfv( whprintf_FILE_appender, fp, fmt, vargs );
	va_end(vargs);
	return ret;
}

/**
   Internal implementation details for whprintfv_appender_stringbuf.
*/
typedef struct whprintfv_stringbuf
{
    /** dynamically allocated buffer */
    char * buffer;
    /** bytes allocated to buffer */
    size_t alloced;
    /** Current position within buffer. */
    size_t pos;
} whprintfv_stringbuf;
static const whprintfv_stringbuf whprintfv_stringbuf_init = { 0, 0, 0 };

/**
   A whprintfv_appender implementation which requires arg to be a
   (whprintfv_stringbuf*). It appends n bytes of data to the
   whprintfv_stringbuf object's buffer, reallocating it as
   needed. Returns less than 0 on error, else the number of bytes
   appended to the buffer. The buffer will always be null terminated.
*/
static long whprintfv_appender_stringbuf( void * arg, char const * data, long n )
{
    whprintfv_stringbuf * sb = (whprintfv_stringbuf*)arg;
    if( ! sb || (n<0) ) return -1;
    if( ! n ) return 0;
    size_t npos = sb->pos + n;
    if( npos >= sb->alloced )
    {
	const size_t asz = (npos * 1.5) + 1;
	if( asz < npos ) return -1; /* overflow */
	char * buf = realloc( sb->buffer, asz );
	if( ! buf ) return -1;
	memset( buf + sb->pos, 0, (npos + 1 - sb->pos) ); // the +1 adds our \0 for us
	sb->buffer = buf;
	sb->alloced = asz;
    }
    long rc = 0;
    for( ; rc < n; ++rc, ++sb->pos )
    {
	sb->buffer[sb->pos] = data[rc];
    }
    return rc;
}


char * whprintfv_str( char const * fmt, va_list vargs )
{
    if( ! fmt ) return 0;
    whprintfv_stringbuf sb = whprintfv_stringbuf_init;
    long rc = whprintfv( whprintfv_appender_stringbuf, &sb, fmt, vargs );
    if( rc <= 0 )
    {
	free( sb.buffer );
	sb.buffer = 0;
    }
    return sb.buffer;
}

char * whprintf_str( char const * fmt, ... )
{
    va_list vargs;
    va_start( vargs, fmt );
    char * ret = whprintfv_str( fmt, vargs );
    va_end( vargs );
    return ret;
}
/* end file src/whprintf.c */
/* begin file src/whalloc_amalgamation.c */
#line 8 "src/whalloc_amalgamation.c"
/* auto-generated on Wed Mar 17 23:25:53 CET 2010. Do not edit! */
#line 1 "whalloc_amalgamation.c"
/* begin file whalloc.c */
#line 8 "whalloc.c"
#if !defined(__STDC_FORMAT_MACROS)
#  define __STDC_FORMAT_MACROS 1 /* for PRIxNN specifiers*/
#endif
#include <stdlib.h>
#include <assert.h>
#include <string.h> /* memset() */

/************************************************************************
************************************************************************/
#if defined (_MSC_VER)
/* warning C4116: unnamed type definition in parentheses */ 
#  pragma warning (disable : 4116)
#endif




/**
   Implements WHALLOC_API(realloc_f)() and returns realloc(m,n).
*/
static void * WHALLOC_API(allocator_realloc_proxy)( void * m, unsigned int n, void * state )
{
    return realloc(m, n);
}

const WHALLOC_API(fallback) WHALLOC_API(fallback_empty) = whalloc_fallback_empty_m;
const WHALLOC_API(fallback) WHALLOC_API(fallback_stdalloc) = {realloc,free};
const WHALLOC_API(allocator) WHALLOC_API(allocator_realloc3) = {WHALLOC_API(allocator_realloc_proxy),NULL};
const WHALLOC_API(allocator) WHALLOC_API(allocator_empty) = {NULL,NULL};
const WHALLOC_API(allocator_base) WHALLOC_API(allocator_base_empty) = whalloc_allocator_base_empty_m;
const WHALLOC_API(rc_t) WHALLOC_API(rc) =
    {
    0/*OK*/,
    1/*RangeError*/,
    2/*ArgError*/,
    3/*InternalError*/,
    4/*HashingError*/,
    5/*AllocError*/,
    6/*UsageError*/,
    7/*ConsistencyError*/,
    8/*LockingError*/,
    (WHALLOC_API(size_t))-1/*HashCodeError*/
    };

typedef char static_assert[
                           sizeof(uintptr_t) == sizeof(void*) ? 1 : -1
                           ];

const WHALLOC_API(mutex) WHALLOC_API(mutex_empty) = whalloc_mutex_empty_m;
const WHALLOC_API(mutex) WHALLOC_API(mutex_trace) = whalloc_mutex_trace_m;

int WHALLOC_API(mutex_lock_trace)( void * p )
{
    fprintf(stderr,"WHALLOC_API(mutex)::lock(): mutex @%p\n",p);
    return 0;
}
int WHALLOC_API(mutex_unlock_trace)( void * p )
{
    fprintf(stderr,"WHALLOC_API(mutex)::unlock(): mutex @%p\n",p);
    return 0;
}



int whalloc_calc_mask( WHALLOC_API(size_t) number,
                       uint8_t * bits,
                       WHALLOC_API(size_t) * mask )
{
    uint8_t b;
    WHALLOC_API(size_t) i;
    if( !number || ! bits ) return WHALLOC_API(rc).ArgError;
    b = 0;
    i = 1;
    for( ; i < number; i*=2 )
    {
        ++b;
    }
    if( b > (WHALLOC_BITNESS-1) )
    {
        return WHALLOC_API(rc).RangeError;
    }
    *bits = b;
    if( mask ) *mask = (WHALLOC_MASK >> (WHALLOC_BITNESS - b));
    return WHALLOC_API(rc).OK;
}

int whalloc_calc_mask2( WHALLOC_API(size_t) size,
                        WHALLOC_API(size_t) blockSize,
                        uint8_t * bits,
                        WHALLOC_API(size_t) * mask,
                        WHALLOC_API(size_t) *blocks )
{
    if( !size || !blockSize || ! bits || (size<(blockSize*2)) )
    {
        return WHALLOC_API(rc).ArgError;
    }
    else
    {
        WHALLOC_API(size_t) n;
        n = size/blockSize;
        if( blocks ) *blocks = n;
        return whalloc_calc_mask( n, bits, mask );
    }
}



/**
   @internal

   Returns a hash value for the given offset (in bytes) in
   self->uspace, which must be in the range [0,self->usize).

   If( off >= self->usize ) WHALLOC_API(rc).HashCodeError is returned, else
   the hash value is returned.
*/
WHALLOC_API(size_t) whalloc_allocator_base_hash_offset( WHALLOC_API(allocator_base) const * self, WHALLOC_API(size_t) off )
{
#if 1
    return ( off >= self->usize )
        ? WHALLOC_API(rc).HashCodeError
        : ((off/self->blockSize)&self->hashMask)
        ;
#else
    return ((off)/self->blockSize) & self->hashMask;
#endif
}
/**
   @internal

   Returns the hashcode for the given address. If addr
   is outside of self's range then WHALLOC_API(rc).HashCodeError
   is returned.
*/
WHALLOC_API(size_t) whalloc_allocator_base_hash_addr( WHALLOC_API(allocator_base) const * self, void const * addr_ )
{
    unsigned char const * addr;
    addr = (unsigned char const *) addr_;
    if( !addr || !self
        || (addr < self->uspace)
        || (addr >= self->end)
        )
    {
#if 0
        LOGBASE(("bad addr @%p/%u. uspace=%p/%u, limit=%p/%u\n",addr,(size_t)addr,
                 self->uspace,(size_t)self->uspace,self->end,(size_t)self->end));
#endif
        return
            WHALLOC_API(rc).HashCodeError
            ;
    }
    else
    {
        return whalloc_allocator_base_hash_offset( self, (WHALLOC_API(size_t))(addr - self->uspace) );
    }
}


uint8_t WHALLOC_API(bitness)()
{
    return WHALLOC_BITNESS;
}
/* end file whalloc.c */
/* begin file whalloc_bt.c */
#line 8 "whalloc_bt.c"
#include <assert.h>
#include <string.h> /* memset() */

/* Defined/documented in whalloc.c */
extern WHALLOC_API(size_t) whalloc_allocator_base_hash_offset( WHALLOC_API(allocator_base) const * self, WHALLOC_API(size_t) off );

/* Defined/documented in whalloc.c */
extern WHALLOC_API(size_t) whalloc_allocator_base_hash_addr( WHALLOC_API(allocator_base) const * self, void const * addr_ );

#if !defined(NDEBUG)
#  define LOGBASE(mp_exp) if( self && self->log ) \
    { self->log("%s:%d:%s(): ",__FILE__,__LINE__,__func__); \
        self->log mp_exp; \
    } else (void)0
#  define LOGSELF(mp_exp) if( self && self->base.log )   \
    { self->base.log("%s:%d:%s(): ",__FILE__,__LINE__,__func__); \
        self->base.log mp_exp; \
    } else (void)0
#define MARKER(mp_exp) printf("MARKER: %s:%d:%s(): ",__FILE__,__LINE__,__func__); printf mp_exp
#else
#  define LOGSELF(mp_exp) (void)0
#  define LOGBASE(mp_exp) (void)0
#  define MARKER(mp_exp) ((void)0)
#endif

#define LOCK_OR(RC) if( self->base.mutex.lock ) if( 0 != self->base.mutex.lock(self->base.mutex.state) ) return RC

#define UNLOCK if( self->base.mutex.unlock ) self->base.mutex.unlock( self->base.mutex.state )

#define whalloc_bt_hash_offset(S,O) whalloc_allocator_base_hash_offset(&(S)->base,(O))
#define whalloc_bt_hash_addr(S,O) whalloc_allocator_base_hash_addr(&(S)->base,(O))

const WHALLOC_API(bt) WHALLOC_API(bt_empty) = whalloc_bt_empty_m;

#define BYTES_BYTEFOR(A,BIT) ((A)[ BIT / 8 ])
#define BYTES_SET(A,BIT) ((BYTES_BYTEFOR(A,BIT) |= (0x01 << (BIT%8))),0x01)
#define BYTES_UNSET(A,BIT) ((BYTES_BYTEFOR(A,BIT) &= ~(0x01 << (BIT%8))),0x00)
#define BYTES_GET(A,BIT) ((BYTES_BYTEFOR(A,BIT) & (0x01 << (BIT%8))) ? 0x01 : 0x00)
#define BYTES_TOGGLE(A,BIT) (BYTES_GET(A,BIT) ? (BYTES_UNSET(A,BIT)) : (BYTES_SET(B,BIT)))

#define BITMAP_BYTEOF(ARRAY,BIT) ((self->bits.ARRAY)[ BIT / 8 ])
#define BITMAP_IS_USED(BIT) BYTES_GET(self->bits.usage,(BIT))
#define BITMAP_SET_USAGE(BIT,VAL) ((VAL)?BYTES_SET(self->bits.usage,BIT):BYTES_UNSET(self->bits.usage,BIT))
#define BITMAP_USE(BIT) BITMAP_SET_USAGE(BIT,1)
#define BITMAP_UNUSE(BIT) BITMAP_SET_USAGE(BIT,0)

#define BITMAP_IS_LINKED(BIT) BYTES_GET(self->bits.links,(BIT))
#define BITMAP_SET_LINK(BIT,VAL) ((VAL)?BYTES_SET(self->bits.links,BIT):BYTES_UNSET(self->bits.links,BIT))
#define BITMAP_LINK(BIT) BITMAP_SET_LINK(BIT,1)
#define BITMAP_UNLINK(BIT) BITMAP_SET_LINK(BIT,0)

/**
   Clears the internal bits table. Returns 0 on success.
*/
static int WHALLOC_API(bt_clear_table)( WHALLOC_API(bt) * const self )
{
    if( ! self ) return WHALLOC_API(rc).ArgError;
    else
    {
        if( self->bits.usage != self->bits.cache )
        {
            LOGSELF(("Clearing out %"WHALLOC_SIZE_T_PFMT" bytes from client-side cache.\n",self->bits.byteCount));
            memset(self->bits.usage, 0, self->bits.byteCount );
        }
        else
        {
            LOGSELF(("Clearing out %"WHALLOC_SIZE_T_PFMT" bytes from internal cache.\n",sizeof(self->bits.cache)));
            memset(self->bits.cache, 0, sizeof(self->bits.cache) );
        }
        self->base.allocCount=0;
        self->base.allocBlockCount=0;
        self->base.freeIndexHint = 0;
        return WHALLOC_API(rc).OK;
    }
}

int WHALLOC_API(bt_init)( WHALLOC_API(bt) * const self,
                         void * mem,
                         WHALLOC_API(size_t) size,
                         WHALLOC_API(size_t) blockSize )
{
    static const WHALLOC_API(size_t) MaxSize = (WHALLOC_MASK);
    static const WHALLOC_API(size_t) MinSize = 64;
    WHALLOC_API(size_t) blockCount;
    unsigned char * bUsed;
    unsigned char * bLink;
    WHALLOC_API(size_t) tblBits;
    WHALLOC_API(size_t) mbBytes;
    int rc;
    if( ! blockSize ) blockSize = 8;
    if( ! self || ! mem || (size<(blockSize*2)) )
    {
        return WHALLOC_API(rc).ArgError;
    }
    if( (size > MaxSize) || (size < MinSize) )
    {
        LOGSELF(("Size %"WHALLOC_SIZE_T_PFMT" must be in the range (%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT")\n",size, MinSize, MaxSize));
        return WHALLOC_API(rc).ArgError;
    }

    blockCount = 0;
    rc = whalloc_calc_mask2( size, blockSize, &self->base.bitCount, &self->base.hashMask, &blockCount );
    LOGSELF(("rc=%d from whalloc_calc_mask2(%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT",...) blockCount=%"WHALLOC_SIZE_T_PFMT"\n",rc,size, blockSize,blockCount));
    if( rc )
    {
        LOGSELF(("Error %d from whalloc_calc_mask2(%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT",...)\n",rc,self->base.usize, blockSize));
        return rc;
    }
    self->base.mem = (unsigned char *) mem;
    self->base.size = self->base.usize = size;
    self->base.uspace = self->base.mem;
    self->base.end = self->base.mem + self->base.size;
    self->base.blockSize = blockSize;
    bUsed = 0;
    bLink = 0;
    tblBits = blockCount;
    mbBytes = (tblBits/8)*2;/*  2==1 bit for is-used, 1 bit for is-linked */
    /* if( ! mbBytes ) mbBytes = 2; */
    if( mbBytes && (tblBits%8) ) ++mbBytes;
    if( mbBytes%2 ) ++mbBytes; /*  round up to 2-byte boundary */
    mbBytes += 2;
    /** ^^^^^ maintenance reminder We need to add a padding byte
        between each bitmap to ensure that the bitmaps do not align
        directly with each other or the end of the bitmap. If the
        bitmap has been perfectly sized, and all bits are used,
        operations which crawl linked blocks can actually crawl into
        the data bytes by mistake if the bitmaps are edge-to-edge,
        causing corruption. Been there, done that.

        20100304: now that the bitsets have been moved to the end
        of the memory, we really only need 1 padding byte between
        the is-used and is-linked bitsets.
    */
    if( (WHALLOC_API(bt_CacheLength))
        && ((sizeof(self->bits.cache)>2) && (mbBytes < (sizeof(self->bits.cache))))
        )
    {
        /**
           If the bits will fit in our internal cache, use the cache space.
           
           TODO: if the cache is long enough to hold one of the bitsets,
           relocate only one bitset into the reserved memory.
        */
        LOGSELF(("Using internal bits cache for blockCount=%"WHALLOC_SIZE_T_PFMT", tblBits=%"WHALLOC_SIZE_T_PFMT", mbBytes=%"WHALLOC_SIZE_T_PFMT"! Stealing no memory!\n",
                 blockCount, tblBits, mbBytes ));
        bUsed = self->bits.cache;
        bLink = self->bits.cache + (mbBytes/2);
        self->bits.byteCount = 0;
    }
    else
    {
        self->base.usize -= mbBytes;
        /*  adjust memory buffer size and re-calculate our bitmask... */
        rc = whalloc_calc_mask2( self->base.usize, blockSize, &self->base.bitCount, &self->base.hashMask, &blockCount );
        LOGSELF(("rc=%d from whalloc_calc_mask2(%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT",...) blockCount=%"WHALLOC_SIZE_T_PFMT"\n",rc,size, blockSize,blockCount));
        if( rc )
        {
            LOGSELF(("Error %d from whalloc_calc_mask2(%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT",...)\n",rc,self->base.usize, blockSize));
            return rc;
        }
#if 1
        /** Changed 20100304:

            Locate the flag bits at the END of self->base.mem so that
            we can ensure predictable alignment of allocated blocks.
        */
        self->base.uspace = self->base.mem;
        bUsed = self->base.mem + self->base.usize + 1 /*+1 need as a 0-pad byte, as explained above.*/;
        bLink = bUsed + (mbBytes/2);
        self->bits.byteCount = mbBytes;
#else
        /** Store flag bits at the start of self->base.mem. This is
            theoretically safer but makes it more difficult to ensure
            predictable alignment of allocated blocks.
        */
        self->base.uspace = self->base.mem + mbBytes;
        bUsed = self->base.mem;
        bLink = self->base.mem + (mbBytes/2);
        self->bits.byteCount = mbBytes;
#endif
        LOGSELF(("blockCount=%"WHALLOC_SIZE_T_PFMT", usize=%"WHALLOC_SIZE_T_PFMT", tblBits=%"WHALLOC_SIZE_T_PFMT", mbBytes=%"WHALLOC_SIZE_T_PFMT", bits.byteCount=%"WHALLOC_SIZE_T_PFMT"\n",
                 blockCount,self->base.usize,tblBits,mbBytes,self->bits.byteCount));
    }
    self->base.blockCount = self->bits.end = blockCount;
    self->bits.usage = bUsed;
    self->bits.links = bLink;
    LOGSELF(("size=%"WHALLOC_SIZE_T_PFMT", usize=%"WHALLOC_SIZE_T_PFMT" (diff=-%"WHALLOC_SIZE_T_PFMT"), blockSize=%"WHALLOC_SIZE_T_PFMT", "
             "maxBlocks=%"WHALLOC_SIZE_T_PFMT", tblBits=%"WHALLOC_SIZE_T_PFMT" (%"WHALLOC_SIZE_T_PFMT" per set), "
             "mbBytes=%"WHALLOC_SIZE_T_PFMT" (fits %"WHALLOC_SIZE_T_PFMT" bits/set), self->bits.end\n",
             size, self->base.usize, (size- self->base.usize), blockSize,
             blockCount, tblBits, self->bits.end,
             mbBytes, mbBytes*4));
    LOGSELF(("bitCount=%"WHALLOC_SIZE_T_PFMT", hashMask=%08x\n",
             self->base.bitCount, self->base.hashMask));
    LOGSELF(("\nself->base.mem\t%p\nself->base.uspace\t%p\nDiff\t%"WHALLOC_SIZE_T_PFMT" bytes\nself->base.end\t%p\n",
             self->base.mem, self->base.uspace, (uint32_t)(self->base.uspace - self->base.mem), self->base.end ));
    /*  Test the hashing algo for no dupes: */
    if( 1 )
    {
        WHALLOC_API(size_t) at;
        unsigned char const * offset;
        WHALLOC_API(size_t) hash;

        at = 0;
        offset = self->base.uspace;
        hash = 0;
        memset( mem, 0, size );
        WHALLOC_API(bt_clear_table)(self);
        for( ; at < self->bits.end; ++at, offset += self->base.blockSize )
        {
            hash =
                /* whalloc_bt_hash_addr( self, offset ) */
                whalloc_bt_hash_offset( self, at * self->base.blockSize )
                ;
            assert( hash == whalloc_bt_hash_offset( self, at * self->base.blockSize ) );
            assert( hash == whalloc_bt_hash_addr( self, offset ) );
            if( BITMAP_IS_USED(hash) )
            {
                LOGSELF(("Unexpected is-used while testing hashcodes (possibly hash collision): Offset %p at=%"WHALLOC_SIZE_T_PFMT" hash=%"WHALLOC_SIZE_T_PFMT"\n",offset,at,hash));
                assert( 0 && "BITMAP_IS_USED(hash)" );
                return WHALLOC_API(rc).ConsistencyError;
            }
            if( hash != at )
            {

#if 0
                LOGSELF(("Unexpected mismatch while testing hashcodes: Offset %p at=%"WHALLOC_SIZE_T_PFMT" hash=%"WHALLOC_SIZE_T_PFMT"\n",offset,at,hash));
#else
                LOGSELF(("Warning: (hash %"WHALLOC_SIZE_T_PFMT"!=pos %"WHALLOC_SIZE_T_PFMT") we ran out of hashcodes at %"WHALLOC_SIZE_T_PFMT" of %"WHALLOC_SIZE_T_PFMT" expected entries. "
                         "Possibly caused by too-small of a blockSize (%"WHALLOC_SIZE_T_PFMT") for this size (%"WHALLOC_SIZE_T_PFMT").\n",
                         hash,at,at,self->bits.end,self->base.blockSize, self->base.size));
#endif
                assert( hash == at );
                return WHALLOC_API(rc).HashingError;
            }
            BITMAP_USE(at);
        }
        offset += self->base.blockSize;
        hash =
            whalloc_bt_hash_addr( self, offset )
            /* whalloc_bt_hash_offset( self, at ) */
            ;
        if( hash < self->bits.end )
        {
            MARKER(("Unexpected final hash code: offset %p end=%p hash=%"WHALLOC_SIZE_T_PFMT"\n",offset,self->base.end,hash));
            assert( (hash >= self->bits.end) );
            return WHALLOC_API(rc).HashingError;
        }
    }
    memset( mem, 0, size );
    WHALLOC_API(bt_clear_table)(self);
    return WHALLOC_API(rc).OK;
}

/** @internal
   Returns a pointer the memory associated with the given block index,
   or null if !self or at is out of bounds.
*/
static unsigned char *
WHALLOC_API(bt_mem_for_hash)( WHALLOC_API(bt) const * const self,
                             WHALLOC_API(size_t) at )
{
    return !self
        ? NULL
        : ( (at >= self->bits.end)
           ? NULL
            : (self->base.uspace + ( self->base.blockSize * at )) )
        ;
}

/** @internal

Tries to find an unused range of memory in self. On success it marks
the all blocks of the span as in-use and links all but the last one.

startIndex = self->ht.head index to start at. It is a hint. If that
block is in use then it will be skipped over.

size = size, in bytes, to allocate. It must be greater than 0.

self is assumed to already be locked - this function does
not lock it.

On success, returns the index of the chunk containing the memory
record. On error it returns a value equal to or greater than
self->bits.end (WHALLOC_API(rc).HashCodeError if !self or !size).  All
blocks in the chain except the last one will be marked as in-use and
linked. The last block will be marked as in-use and not linked.

This function adjusts self->base.allocBlockCount and
self->base.allocCount. It also possibly updates self->freeIndexHint.
*/
static
WHALLOC_API(size_t) WHALLOC_API(bt_mark_range)( WHALLOC_API(bt) * const self,
                                      WHALLOC_API(size_t) startIndex,
                                      WHALLOC_API(size_t) size )
{
    WHALLOC_API(size_t) pos;
    WHALLOC_API(size_t) endex /* one-past-end */;
    WHALLOC_API(size_t) stride;
    if( ! self || ! size ) return WHALLOC_API(rc).HashCodeError;
    LOGSELF(("trying to mark range of %"WHALLOC_SIZE_T_PFMT" bytes starting at index %"WHALLOC_SIZE_T_PFMT"...\n",
             size, startIndex));
    /**
       Overall algorithm:
    
       endex = startIndex + (size/self->base.blockSize)

       If endex>=self->bits.end return error

       If endex is in-use:

       - startIndex = endex+1
       - If startIndex>=self->bits.end, return error
       - Else try again

       If endex is unused:

       - Mark all blocks between (startIndex,endex) as used.
    */
    if( startIndex >= self->bits.end )
    {
        return self->bits.end;
    }
    endex = self->bits.end;
    stride = (size/self->base.blockSize);
    if( !stride || (size%self->base.blockSize) ) ++stride;
    LOGSELF(("trying to mark range of %"WHALLOC_SIZE_T_PFMT" bytes starting at index %"WHALLOC_SIZE_T_PFMT", stride=%"WHALLOC_SIZE_T_PFMT"...\n",
             size, startIndex, stride));
    do
    {
        WHALLOC_API(size_t) check;
        WHALLOC_API(size_t) lastFree;
        endex = startIndex + stride;
        LOGSELF(("Checking range [%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT")"
                 ", self->bits.end=%"WHALLOC_SIZE_T_PFMT"\n",
                 startIndex, endex, self->bits.end));
        if( endex > self->bits.end )
        {
            return self->bits.end;
        }
        else if( endex != self->bits.end )
        {            
            if( BITMAP_IS_USED(endex) )
            {
                LOGSELF(("[%"WHALLOC_SIZE_T_PFMT" is used. Skipping.\n",startIndex,endex));
                startIndex = endex + 1;
                continue;
            }
        }
        else
        {
            /* It looks like we're eking out the last block. */
        }
        /*  ensure the whole range is not in use: */
        /*
            FIXME: when startIndex is on a byte boundary relative to
            the bitsets, we can check a whole block of 8 (or 16, or
            32) at once via a single mask check. If it's not on a byte
            boundary, we'd have to shift that mask around quite trickily.
        */
        check = endex-1;
        lastFree = check;
        for( ; (check != startIndex)
                 && !BITMAP_IS_USED(check);
             --check, --lastFree )
        {
        }
        if( check == startIndex ) break; /*  all blocks between (start,end) are free :-D */
        startIndex = lastFree+1;
        /*  try again. */
    } while(1);
    if( (endex > self->bits.end) || (startIndex >= self->bits.end) )
    {
        return self->bits.end;
    }
    LOGSELF(("Using range [%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT")\n",startIndex,endex));
    for( pos = startIndex ; pos < endex; ++pos )
    {
        /*
          FIXME: as above when checking ranges, we can,
          if pos is on a byte boundary, mask whole
          sets of 8 at once.
         */
        LOGSELF(("Marking #%"WHALLOC_SIZE_T_PFMT" as used.\n",pos));
        BITMAP_USE(pos);
        assert( BITMAP_IS_USED(pos) );
        if( (endex-1) != pos )
        {
            BITMAP_LINK(pos);
            assert( BITMAP_IS_LINKED(pos) );
        }
        else
        {
            LOGSELF(("Not linking last item, #%"WHALLOC_SIZE_T_PFMT".\n",pos));
        }
        ++self->base.allocBlockCount;
    }
    ++self->base.allocCount;
    if( self->base.freeIndexHint == startIndex )
    {
        self->base.freeIndexHint = endex;
    }
    return startIndex;
}

/**@ internal
   The converse of WHALLOC_API(bt_mark_range)().

   Clears a range of blocks for re-use.

   startIndex must refer to an in-use block. That block,
   plus any others to its right associated via a multi-block
   allocation, are marked as free.

   Returns 0 on success. If it returns WHALLOC_API(rc).InternalError
   then the pool state is probably corrupt or unexpected. It returns
   WHALLOC_API(rc).ArgError if !self and WHALLOC_API(rc).RangeError if
   startIndex is not less than self->ht.end. 

   This function adjusts self->base.allocBlockCount and
   self->base.allocCount. It also possibly updates
   self->freeIndexHint.
*/
static
int WHALLOC_API(bt_clear_range)( WHALLOC_API(bt) * const self,
                                 WHALLOC_API(size_t) startIndex )
{
    WHALLOC_API(size_t) pos;
    char hadLink;
    unsigned char * mem;
    if( ! self )
    {
        return WHALLOC_API(rc).ArgError;
    }
    else if( startIndex >= self->bits.end)
    {
        return WHALLOC_API(rc).RangeError;
    }
    pos = startIndex;
    LOGSELF(("Clearing range starting at %"WHALLOC_SIZE_T_PFMT"...\n",pos));
    hadLink = 0;
    for( ; pos < self->bits.end; ++pos )
    {
        /**
           TODO:

           We can check the whole byte at once for used-ness and/or
           linkedness. But we need to be careful to mask only the
           pos-plus-right bits in the current byte of
           self->bits.usage, and we have the special case of the final
           block in a chain being used but not linked.
        */
        if( startIndex == pos )
        {
            if( ! BITMAP_IS_USED(pos) )
            {
                LOGSELF(("Internal error: we were expecting that entry #%"WHALLOC_SIZE_T_PFMT" was marked as used!\n",
                         pos ));
                return WHALLOC_API(rc).InternalError;
            }
        }
        else
        {
            if( ! BITMAP_IS_USED(pos) )
            {
                break; /*  we've reached the next block. */
                LOGSELF(("Expecting a used entry at #%"WHALLOC_SIZE_T_PFMT"!\n",pos));
                return WHALLOC_API(rc).InternalError;
            }
        }
        LOGSELF(("Marking entry #%"WHALLOC_SIZE_T_PFMT" as unused.\n",pos));
        BITMAP_UNUSE(pos);
        hadLink = BITMAP_IS_LINKED(pos);
        BITMAP_UNLINK(pos);
        --self->base.allocBlockCount;
        if( ! hadLink )
        { /*  end of the block */
            break;
        }
    }
    --self->base.allocCount;
    mem = WHALLOC_API(bt_mem_for_hash)( self, startIndex );
    assert(mem && "Internal memory reference error!");
    if( self->base.freeIndexHint > startIndex )
    {
        self->base.freeIndexHint = startIndex;
    }
    return WHALLOC_API(rc).OK;    
}

static WHALLOC_API(size_t) WHALLOC_API(bt_count_chain)( WHALLOC_API(bt) * const self,
                                       WHALLOC_API(size_t) startIndex )
{
    WHALLOC_API(size_t) pos;
    if( ! self )
    {
        return 0;
    }
    else if( startIndex >= self->bits.end)
    {
        return 0;
    }
    pos = startIndex;
    for( ; pos < self->bits.end; ++pos )
    {
        if( startIndex == pos )
        {
            if( ! BITMAP_IS_USED(pos) )
            {
                LOGSELF(("Internal error: we were expecting that entry #%"WHALLOC_SIZE_T_PFMT" was marked as used!\n",
                         pos ));
                return 0;
            }
        }
        else
        {
            if( ! BITMAP_IS_USED(pos) )
            {
                LOGSELF(("Expecting a used entry at #%"WHALLOC_SIZE_T_PFMT"!\n",pos));
                break; /*  we've reached the next block. */
            }
        }
        if( ! BITMAP_IS_LINKED(pos) )
        { /*  end of the block */
            break;
        }
    }
    return (pos - startIndex)+1;
}

/** @internal

   Internal impl for WHALLOC_API(bt_alloc)() and WHALLOC_API(bt_realloc)().
   If doLocking is true then self->mutex is respected, otherwise it
   is assumed to already be locked.
*/
static void*
WHALLOC_API(bt_alloc_impl)( WHALLOC_API(bt) * const self, WHALLOC_API(size_t) size, char doLocking  )
{
    WHALLOC_API(size_t) hash;
    unsigned char * mem;
    void * fb;
    if( ! self ) return 0;
    if( doLocking )
    {
        LOCK_OR(NULL);
    }
    fb = 0;
#define FBALLOC if(self->base.fallback.realloc) do { fb = self->base.fallback.realloc(0,size); if(doLocking) UNLOCK; return fb; } while(0)
    if (! size)
    {
        size = self->base.blockSize;
    }
    LOGSELF(("pool=@%p, alloc request size=%"WHALLOC_SIZE_T_PFMT"\n",(void const *)self, size));

    if( (self->base.allocBlockCount>=self->bits.end /*we're full*/)
        )
    {
        LOGSELF(("pool=@%p cannot allocate %"WHALLOC_SIZE_T_PFMT" bytes.%s\n",
                 (void const *)self,
                 size,
                 (self->base.fallback.realloc ? " Trying fallback." : "")
                 ));
        FBALLOC;
        if(doLocking) UNLOCK;
        return 0;
    }
  
    hash = 0;
    hash = self->base.freeIndexHint;
    LOGSELF(("self->base.freeIndexHint=%"WHALLOC_SIZE_T_PFMT"\n",hash));
    if( hash >= self->bits.end ) hash = 0;
    hash = WHALLOC_API(bt_mark_range)( self, hash, size );/*  FIXME: size+ALIGNMENT? */
    if( hash >= self->bits.end )
    {
        LOGSELF(("Could not allocate %"WHALLOC_SIZE_T_PFMT" bytes!\n",size));
        FBALLOC;
        if(doLocking) UNLOCK;
        return 0;
    }
    mem = WHALLOC_API(bt_mem_for_hash)( self, hash );
    LOGSELF(("Allocated %"WHALLOC_SIZE_T_PFMT" bytes @%p\n",size,mem));
    if(doLocking) UNLOCK;
    assert( mem && "Internal memory handling error!" );
#undef FBALLOC
    return mem;
}

void *  WHALLOC_API(bt_realloc)( WHALLOC_API(bt) * const self, void * m, WHALLOC_API(size_t) size )
{
    WHALLOC_API(size_t) hash;
    unsigned char * mem;
    unsigned char * re;
    WHALLOC_API(size_t) oldlen;
    WHALLOC_API(size_t) bcount;
    WHALLOC_API(size_t) blocksNeeded;
    if( ! self ) return NULL;
    else if( ! m ) return WHALLOC_API(bt_alloc)( self, size );
    else if( ! size )
    {
        return (0 == WHALLOC_API(bt_free)( self, m ))
            ? NULL
            : m;
    }
    LOCK_OR(NULL);
#define FBALLOC if(self->base.fallback.realloc) do { void * fb = self->base.fallback.realloc(m,size); UNLOCK; return fb; } while(0)
    hash = whalloc_bt_hash_addr( self, m );
    if( hash >= self->bits.end )
    {
        FBALLOC;
        UNLOCK;
        assert( 0 && "WHALLOC_API(bt_realloc)() was asked to realloc memory it doesn't manage!");
        return NULL;
    }
    mem = WHALLOC_API(bt_mem_for_hash)( self, hash );
    assert( mem && "Internal error getting starting memory address for hash code.");
    bcount = WHALLOC_API(bt_count_chain)( self, hash );
    if( ! bcount )
    {
        UNLOCK;
        assert( 0 && "Internal error counting block chain length.");
        return NULL;
    }
    blocksNeeded = (size/self->base.blockSize);
    if( !blocksNeeded || (size%self->base.blockSize) ) ++blocksNeeded;
    oldlen = (bcount * self->base.blockSize);
    if( (size <= oldlen) || (blocksNeeded<=bcount) )
    {
#if 1
        if( blocksNeeded < bcount )
        { /*
            Shrink if needed. This could be optimized, but this approach
            is easy...
          */
            WHALLOC_API(size_t) mark;
            if( 0 != WHALLOC_API(bt_clear_range)( self, hash ) )
            {
                UNLOCK;
                assert(0 && "WHALLOC_API(bt_realloc)() internal error while clearing memory range!");
                return NULL;
            }
            mark = WHALLOC_API(bt_mark_range)( self, hash, size );
            assert( (mark == hash) && "WHALLOC_API(bt_realloc)() internal error: bt_mark_range() returned unexpected result!" );
            mem = WHALLOC_API(bt_mem_for_hash)( self, mark );
        }
#endif
        if( self->base.freeIndexHint >= hash)
        {
            self->base.freeIndexHint = hash+blocksNeeded;
        }
        UNLOCK;
        return mem;
    }
    re = 0;
    if(1) do
    {
        /**
           See if we can expand in the neighboring blocks
           before falling back to WHALLOC_API(bt_alloc)().
        */
        WHALLOC_API(size_t) endStart;
        WHALLOC_API(size_t) endex /* at-end position, not one-past */;
        WHALLOC_API(size_t) goal;
        WHALLOC_API(size_t) x;
        char isUsed;
        isUsed = 0;
        endStart= hash + bcount;
        endex= endStart;
        goal = hash + blocksNeeded - 1;
        while( (endex < self->bits.end)
               && (endex < goal)
               &&  !(isUsed=BITMAP_IS_USED(endex)))
        {
            ++endex;
        }
        if( (endex >= self->bits.end) || isUsed )
        { /* we can't fit. Fall back to WHALLOC_API(bt_alloc)() for the new block...*/
            break;
        }
        assert( endex == goal );
        /* mark [endStart,endex] as used/linked
           and endex as used/unlinked.
        */
        if(1)
        {
            LOGSELF(("Expanding memory @%p range [%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT"] to "
                     "[%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT"] "
                     "= %"WHALLOC_SIZE_T_PFMT" blocks for %"WHALLOC_SIZE_T_PFMT" bytes.\n",
                     mem, hash, (hash+bcount-1),
                     hash, endex, blocksNeeded, size));
        }
        if( self->base.allocBlockCount ) --self->base.allocBlockCount;/*account for re-linking of previous tail item. */
        for( x = endStart-1; x <= endex; ++x )
        {
            ++self->base.allocBlockCount;
            if( endex == x )
            {
                BITMAP_UNLINK(x);
            }
            else
            {
                BITMAP_LINK(x);
            }
            BITMAP_USE(x);
        }
        if( (self->base.freeIndexHint >= hash)
            && (self->base.freeIndexHint <= endex)
            )
        {
            self->base.freeIndexHint = endex+1;
        }
        UNLOCK;
        return mem;
    } while(0);
    if( ! re ) re = WHALLOC_API(bt_alloc_impl)( self, size, 0 );
    if( re && (mem != re) )
    {
        /*WHALLOC_API(size_t) end;*/
        memcpy( re, mem, oldlen );
        if(0) memset( mem, 0, oldlen ); /* arguable */
#if 1
        WHALLOC_API(bt_clear_range)( self, hash );
#else
        assert( self->base.allocCount );
        if( self->base.allocBlockCount ) --self->base.allocCount;
        end = (hash+bcount);
        for( ; hash < end; ++hash )
        {
            --self->base.allocBlockCount;
            BITMAP_UNUSE(hash);
            BITMAP_UNLINK(hash);
        }
#endif
    }
#undef FBALLOC
    UNLOCK;
    return re;
}

void * WHALLOC_API(bt_alloc)( WHALLOC_API(bt) * const self, WHALLOC_API(size_t) size  )
{
    return WHALLOC_API(bt_alloc_impl)( self, size, 1 );
}

int WHALLOC_API(bt_free)( WHALLOC_API(bt) * const self, void * m )
{
    unsigned char const * ucm;
    WHALLOC_API(size_t) hash;
    int drc;
    if( ! self || !m ) return WHALLOC_API(rc).ArgError;
#define FBFREE if(self->base.fallback.free && self->base.fallback.realloc) { self->base.fallback.free(m); UNLOCK; return WHALLOC_API(rc).OK; }

    LOCK_OR(WHALLOC_API(rc).LockingError);
    ucm = (unsigned char const *)m;
    hash = whalloc_bt_hash_addr( self, m );
    LOGSELF(("delloc @%p, hash=%"WHALLOC_SIZE_T_PFMT"\n",m,hash));
    if( hash >= self->bits.end )
    {
        LOGSELF(("Cannot dealloc memory @%p.%s\n",
                 m,(self->base.fallback.free ? " Trying fallback..." : "")));
        FBFREE;
        UNLOCK;
        assert(0 && "WHALLOC_API(bt_free)() was asked to release memory it does not own!");
        return WHALLOC_API(rc).UsageError;
    }
    assert( ucm == WHALLOC_API(bt_mem_for_hash)( self, hash ) );
    drc = WHALLOC_API(bt_clear_range)( self, hash );
    if( WHALLOC_API(rc).OK != drc )
    {
        LOGSELF(("Deallocation failed with rc %d!\n",drc));
    }
    UNLOCK;
#undef FBFREE
    return drc;
}

/* Implements WHALLOC_API(realloc_f)() interface and requires that
   allocState be-a (WHALLOC_API(bt)*).
*/
static void * WHALLOC_API(bt_allocator_realloc)( void * mem, unsigned int n, void * allocState )
{
    return allocState
        ? WHALLOC_API(bt_realloc)( (WHALLOC_API(bt) *)allocState, mem, n )
        : NULL;    
}

int WHALLOC_API(bt_allocator)( WHALLOC_API(bt) * b, WHALLOC_API(allocator) * a )
{
    if( ! b || !a ) return WHALLOC_API(rc).ArgError;
    else
    {
        a->realloc = WHALLOC_API(bt_allocator_realloc);
        a->state = b;
        return 0;
    }
}


int WHALLOC_API(bt_drain)( WHALLOC_API(bt) * const self )
{
    if( ! self ) return WHALLOC_API(rc).ArgError;
    LOCK_OR(WHALLOC_API(rc).LockingError);
    memset( self->base.mem, 0, self->base.size );
    WHALLOC_API(bt_clear_table)( self );
    LOGSELF(("WHALLOC_API(bt_drain)(%p) {}\n",(void const*)self));
    UNLOCK;
    return 0;
}

int WHALLOC_API(bt_dump_debug)( WHALLOC_API(bt) const * const self, FILE * out )
{
    WHALLOC_API(size_t) at;
    WHALLOC_API(size_t) pos;
    WHALLOC_API(size_t) count;
    char isL;
    char isU;
    WHALLOC_API(size_t) inNode;
    int rc;
    if( ! self ) return WHALLOC_API(rc).ArgError;
    LOCK_OR(WHALLOC_API(rc).LockingError);
#define NUM "%"WHALLOC_SIZE_T_PFMT
    fprintf(out, "%s(): self=%p (sizeof="NUM")\n", __func__, (void const *)self, (unsigned int)sizeof(WHALLOC_API(bt)) );
    fprintf(out,"Allocator:\n");
    fprintf(out,"\trealSize\t"NUM"\n\tusableSize\t"NUM"\n"
            "\tblockSize\t"NUM"\n\thashBits\t"NUM"\n\thashMask\t%08x\n"
            "\tbitmap is stealing (%1.2f%%) ("NUM" bytes) of the storage.\n",
            self->base.size, self->base.usize,
            self->base.blockSize, self->base.bitCount, self->base.hashMask,
            ((1.0*(self->bits.byteCount))/(1.0*self->base.size))*100,
            self->base.size - self->base.usize
            );
    fprintf(out,
            "\tself->base.mem\t%p\n"
            "\tself->base.uspace\t%p\n"
            "\tself->base.end\t%p\n",
            self->base.mem,
            self->base.uspace,
            self->base.end
            );
    fprintf(out, "Bitmap:\n\tbytes reserved\t"NUM"\n\tmax entries\t"NUM"\n"
            "\tWHALLOC_API(bt_CacheLength)="NUM" bytes (enough for "NUM" entries)\n",
            self->bits.byteCount, self->bits.end,
            WHALLOC_API(bt_CacheLength), WHALLOC_API(bt_CacheLength)*4
            );

    fprintf(out, "Used entries:\n"
            "\t"NUM" of "NUM" block(s) alloced using "NUM" bytes from "NUM" allocation(s).\n",
            self->base.allocBlockCount, self->bits.end, 
            self->base.allocBlockCount * self->base.blockSize,
            self->base.allocCount );
    fprintf(out, "Dump of used bitmap entries:\n");
    at = 0;
    count = 0;
    inNode = WHALLOC_API(rc).HashCodeError;
    for( ; at < self->bits.end; ++at )
    {
        isL = BITMAP_IS_LINKED(at);
        isU = BITMAP_IS_USED(at);
        if( ! isL && !isU )
        {
            inNode = WHALLOC_API(rc).HashCodeError;
            pos = 0;
            continue;
        }
        if( WHALLOC_API(rc).HashCodeError == inNode )
        {
            if( isU )
            {
                if( isL )
                {
                    inNode = at;
                }
                fprintf(out, "\t%s block #"NUM": addr=%p\n",
                        (isL ? "Start" : "Single"),
                        at,  WHALLOC_API(bt_mem_for_hash)(self,at) );
                pos = 1;
                ++count;
                continue;
            }
        }
        else
        {
            ++count;
            ++pos;
            if( isL )
            {
                fprintf(out, "\t\tspan> #"NUM": addr=%p\n",
                        at, WHALLOC_API(bt_mem_for_hash)(self,at) );
            }
            else
            {
                fprintf(out, "\t\t end> #"NUM": addr=%p (%"WHALLOC_SIZE_T_PFMT" blocks)\n",
                        at, WHALLOC_API(bt_mem_for_hash)(self,at), pos );
            }
            if( ! isL )
            {
                inNode = WHALLOC_API(rc).HashCodeError;
            }
        }
    }
    if( count != self->base.allocBlockCount )
    {
        fprintf(out,"ERROR: allocated block count mismatch: expected "
                NUM" but got "NUM"!\n", self->base.allocBlockCount, count );
        rc = WHALLOC_API(rc).InternalError;
    }
#undef NUM
    assert( count == self->base.allocBlockCount );
    UNLOCK;
    return rc;
}


#undef BITMAP_BYTE
#undef BITMAP_IS_LINKED
#undef BITMAP_IS_USED
#undef BITMAP_LINK
#undef BITMAP_SET_LINK
#undef BITMAP_SET_USAGE
#undef BITMAP_UNLINK
#undef BITMAP_UNSET
#undef BITMAP_UNUSE
#undef BITMAP_USE
#undef BYTES_BYTEFOR
#undef BYTES_GET
#undef BYTES_SET
#undef BYTES_TOGGLE
#undef BYTES_UNSET
#undef LOGSELF
#undef MARKER
#undef whalloc_bt_hash_offset
#undef whalloc_bt_hash_addr
#undef UNLOCK
#undef LOCK_OR
/* end file whalloc_bt.c */
/* begin file whalloc_ht.c */
#line 8 "whalloc_ht.c"
#include <assert.h>
#include <string.h> /* memset() */


/* Defined/documented in whalloc.c */
extern WHALLOC_API(size_t) whalloc_allocator_base_hash_offset( WHALLOC_API(allocator_base) const * self, WHALLOC_API(size_t) off );

/* Defined/documented in whalloc.c */
extern WHALLOC_API(size_t) whalloc_allocator_base_hash_addr( WHALLOC_API(allocator_base) const * self, void const * addr_ );

const WHALLOC_API(ht_entry) WHALLOC_API(ht_entry_empty) = whalloc_ht_entry_empty_m;
const WHALLOC_API(ht) WHALLOC_API(ht_empty) = whalloc_ht_empty_m;

#include <stdio.h>
#if !defined(NDEBUG)
#  define LOGBASE(mp_exp) if( self && self->log ) \
    { self->log("%s:%d:%s(): ",__FILE__,__LINE__,__func__); \
        self->log mp_exp; \
    } else (void)0
#  define LOGSELF(mp_exp) if( self && self->base.log )   \
    { self->base.log("%s:%d:%s(): ",__FILE__,__LINE__,__func__); \
        self->base.log mp_exp; \
    } else (void)0
#define MARKER(mp_exp) printf("MARKER: %s:%d:%s(): ",__FILE__,__LINE__,__func__); printf mp_exp
#else
#  define LOGSELF(mp_exp) (void)0
#  define LOGBASE(mp_exp) (void)0
#  define MARKER(mp_exp) ((void)0)
#endif

#define LOCK_OR(RC) if( self->base.mutex.lock ) if( 0 != self->base.mutex.lock(self->base.mutex.state) ) return RC
#define UNLOCK if( self->base.mutex.unlock ) self->base.mutex.unlock( self->base.mutex.state )

/**
   Internal masking macros:

   WHALLOC_MASK is the complete mask, with all usable bits set set.
   Bits to the left of the usable space must explicitly be zeroed.
   
   WHALLOC_LENGTH_BITS = the number of bits which can be used for
   storing the length of an allocated block. Normally
   (WHALLOC_BITNESS-1). All non-length bits are assumed to be
   flag bits.

   WHALLOC_FLAG_MASK is a mask of only the bits used as flags.
   Currently the only flag is the in-use bit. Each additional flag
   halves the maximum size of an allocation block, and
   WHALLOC_LENGTH_BITS must be reduced by one for each flag bit.
   Flag bits are assumed to be the left-most bits. All non-flag
   bits are assumed to be length bits.

   WHALLOC_LEN_MASK is a mask of only the bit used as length
   data. Length bits are assumed to be the right-most bits.

   WHALLOC_FLAG_USED is a mask of only the in-use bit.
*/
#if 8 == WHALLOC_BITNESS
#   define WHALLOC_LENGTH_BITS 7
#   define WHALLOC_FLAG_MASK 0x80
#elif 16 == WHALLOC_BITNESS
#   define WHALLOC_LENGTH_BITS 15
#   define WHALLOC_FLAG_MASK (WHALLOC_MASK & 0x8000)
#elif 32 == WHALLOC_BITNESS
#   define WHALLOC_LENGTH_BITS 31
#   define WHALLOC_FLAG_MASK (0x01 << WHALLOC_LENGTH_BITS)
#elif 64 == WHALLOC_BITNESS
#   error "UNTESTED CODE!"
#   define WHALLOC_LENGTH_BITS 48
#   define WHALLOC_FLAG_MASK (((uint64_t)0x0ffff) << WHALLOC_LENGTH_BITS)
#else
#  error "All other values are untested!"
#endif

/* Maintenance reminder: we need a per-BITNESS def of WHALLOC_FLAG_USED if we add more flag bits */
#define WHALLOC_FLAG_USED WHALLOC_FLAG_MASK
#define WHALLOC_LEN_MASK (WHALLOC_MASK & ~(WHALLOC_FLAG_MASK))

/** Returns the LENGTH parts of integer I. */
#define N_TO_ENTRYLEN(I) (((WHALLOC_API(size_t))(I)) & WHALLOC_LEN_MASK)
/** Returns the LENGTH value of the (WHALLOC_API(ht_entry)*) E */
#define ENTRY_GETLEN(E) N_TO_ENTRYLEN((E)->lenfl)
/** Returns the FLAG parts of integer I. */
#define N_TO_ENTRYFL(I) ((I) & WHALLOC_FLAG_MASK)
/** Returns the FLAG value of the (WHALLOC_API(ht_entry)*) E */
#define ENTRY_GETFL(E) N_TO_ENTRYFL((E)->lenfl)
#define ENTRY_IS_USED(E) ((E)->lenfl & WHALLOC_FLAG_USED)
/**
   Returns the given LENGTH value and FLAGS
   values, encoded into a single value. LENGTH
   and FLAGS must be fully masked.
*/
#define ENTRY_ENCODE(I,F) (N_TO_ENTRYLEN(I) | N_TO_ENTRYFL(F))
#define ENTRY_SET_USE_ALLOC(E,I) (E)->lenfl = ENTRY_ENCODE(I,WHALLOC_FLAG_USED)


#define whalloc_ht_hash_offset(S,O) whalloc_allocator_base_hash_offset(&(S)->base,(O))
#define whalloc_ht_hash_addr(S,O) whalloc_allocator_base_hash_addr(&(S)->base,(O))


/** @internal

  Returns the address of the (not-necessarily-aligned) memory refered
  to by the given hashcode (block index). at must be a valid hashcode
  for self. Returns 0 on error (!self or 'at' out of range).
*/
static unsigned char * WHALLOC_API(ht_mem_for_hash)( WHALLOC_API(ht) const * const self,
                                           WHALLOC_API(size_t) at )
{
    return !self
        ? NULL
        : ( (at >= self->ht.end)
           ? NULL
            : (self->base.uspace + ( self->base.blockSize * at )) )
        ;
}

int WHALLOC_API(ht_dump_debug)( WHALLOC_API(ht) const * const self, FILE * out )
{
    WHALLOC_API(size_t) end;
    WHALLOC_API(size_t) at;
    WHALLOC_API(size_t) i;
    WHALLOC_API(size_t) len;
    WHALLOC_API(ht_entry) const * e;
    if( ! self ) return WHALLOC_API(rc).ArgError;
    end = self->ht.end;
    fprintf(out, "%s(): self=%p (sizeof=%"WHALLOC_SIZE_T_PFMT")\n", __func__, (void const *)self,(unsigned int)sizeof(WHALLOC_API(ht)) );
    fprintf(out,"Allocator:\n\trealSize\t%"WHALLOC_SIZE_T_PFMT"\n\tusableSize\t%"WHALLOC_SIZE_T_PFMT""
            "\n\tblockSize\t%"WHALLOC_SIZE_T_PFMT"\n\thashBits\t%"WHALLOC_SIZE_T_PFMT"\n\thashMask\t%08x\n",
            self->base.size, self->base.usize,
            self->base.blockSize, self->base.bitCount, self->base.hashMask);
    fprintf(out, "Hashtable:\n\tbytes\t%"WHALLOC_SIZE_T_PFMT"\n\tend\t%"WHALLOC_SIZE_T_PFMT" (%"WHALLOC_SIZE_T_PFMT" allocated)\n"
            "\tis-initial entries\t%"WHALLOC_SIZE_T_PFMT"\n"
            "\talloced blocks\t%"WHALLOC_SIZE_T_PFMT"\n"
            "\tmemory usage (%1.2f%%)\n"
            "Used entries:\n",
            self->ht.sizeOf, end, self->ht.len,
            self->base.allocCount,
            self->base.allocBlockCount,
            ((1.0*(self->ht.sizeOf))/(1.0*self->base.size))*100
            );
    at = 0;
    i = 0;
    len = 0;
    e = 0;
    for( ; at < self->ht.end; ++at, ++i )
    {
        e = &self->ht.head[at];
        if( ! ENTRY_IS_USED(e) ) continue;
        len = ENTRY_GETLEN(e);
        fprintf(out, "\t%s #%"WHALLOC_SIZE_T_PFMT": encoded=%08x hash=%"WHALLOC_SIZE_T_PFMT" size=%"WHALLOC_SIZE_T_PFMT", addr=%p\n",
                (len ? "Entry" : "    span>"), i, e->lenfl, at, len,
                WHALLOC_API(ht_mem_for_hash)(self,at) );
    }
    return WHALLOC_API(rc).OK;
}

int WHALLOC_API(ht_init)( WHALLOC_API(ht) * const self,
              void* buffer,
              WHALLOC_API(size_t) size,
              WHALLOC_API(size_t) blockSize
             )
{
    static const WHALLOC_API(size_t) MaxSize = (WHALLOC_MASK);
    static const WHALLOC_API(size_t) MinSize = 64/*arbitrary!*/;
    WHALLOC_API(size_t) i;
    WHALLOC_API(size_t) maxItems;
    size_t check;
    WHALLOC_API(size_t) x;
    int rc;
    WHALLOC_API(ht_entry) * ht;
    LOGSELF(("initializing...\n"));
   
    LOGSELF(("WHALLOC_BITNESS=%"WHALLOC_SIZE_T_PFMT" WHALLOC_FLAG_MASK=%x WHALLOC_LEN_MASK=%x\n",
             WHALLOC_BITNESS,WHALLOC_FLAG_MASK,WHALLOC_LEN_MASK));
    LOGSELF(("WHALLOC_FLAG_USED=%04x/%"WHALLOC_SIZE_T_PFMT"\n",
            WHALLOC_FLAG_USED,WHALLOC_FLAG_USED));
    if( (size > MaxSize) || (size < MinSize) )
    {
        LOGSELF(("Size %"WHALLOC_SIZE_T_PFMT" must be in the range %"WHALLOC_SIZE_T_PFMT" .. %"WHALLOC_SIZE_T_PFMT"\n",size, MinSize, MaxSize));
        return WHALLOC_API(rc).ArgError;
    }


    if( ! blockSize ) blockSize = 8;
    if( blockSize >= size )
    {
        LOGSELF(("Block size %"WHALLOC_SIZE_T_PFMT" must be smaller than the buffer size (%"WHALLOC_SIZE_T_PFMT")\n",blockSize,size));
        return WHALLOC_API(rc).ArgError;
    }
    else if( blockSize < (sizeof(WHALLOC_API(ht_entry))) )
    {
        LOGSELF(("Block size of %"WHALLOC_SIZE_T_PFMT" is too small for this allocator.\n",blockSize));
        return WHALLOC_API(rc).RangeError;
    }
    maxItems = 0;
    {
        /**
           Here we guestimate the maximum number of objects we can
           handle.  The heuristic isn't 100% accurate - it can waste a
           block or two - but does fairly well for most cases.
        */
#if 8 == WHALLOC_BITNESS 
        maxItems = (size / blockSize);
#elif 16 == WHALLOC_BITNESS 
        if( 4 == blockSize )
        {
            maxItems = (WHALLOC_API(size_t))((size_t)size/6)+2;
            /* works fairly well: (WHALLOC_API(size_t))((size_t)size/6)+blockSize;*/
        }
        else /* this works well for 2/8/16+, but not 4:*/
        {
            maxItems = (size / (blockSize+2))+2;
        }
#elif 32 == WHALLOC_BITNESS 
        if( (4 == blockSize) )
        {
            maxItems = (WHALLOC_API(size_t))((size_t)size/8)+2;
        }
        else
        {
            /* i haven't yet found one which works really well. This case
               loses relatively many entries.
            */
            maxItems = (size / (blockSize+2))+2;
        }
#else
#  error "Unhandled WHALLOC_BITNESS value!"
#endif
    }
    /* FIXME: refactor to use whalloc_calc_mask2(). See WHALLOC_API(bt_init)() for example. */
    self->base.blockSize = blockSize;
    self->base.bitCount = 0;
    check = 1;
    for( check = 1; check <= ((size_t)maxItems);
         (check = check*2) )
    {
        ++self->base.bitCount;
    }
    if( self->base.bitCount > (WHALLOC_BITNESS-1) )
    {
        LOGSELF(("Some value is too high: self->base.bitCount(=%"WHALLOC_SIZE_T_PFMT") was pushed above the maximum value of %d!\n",
                self->base.bitCount,(WHALLOC_BITNESS-1)));
        return WHALLOC_API(rc).RangeError;
    }
    self->base.hashMask = WHALLOC_MASK >> (WHALLOC_BITNESS - self->base.bitCount);
    self->base.mem = buffer;
    self->base.size = size;
    self->base.end = self->base.mem + size;
    self->ht.head = (WHALLOC_API(ht_entry)*)self->base.mem;
    self->ht.len = maxItems;
    self->ht.sizeOf = sizeof(WHALLOC_API(ht_entry)) * self->ht.len;
    self->base.uspace = self->base.mem + self->ht.sizeOf;
    self->base.usize = size - self->ht.sizeOf;
    self->base.freeIndexHint = 0;
    LOGSELF(("size=%"WHALLOC_SIZE_T_PFMT", maxItems=%"WHALLOC_SIZE_T_PFMT", blockSize=%"WHALLOC_SIZE_T_PFMT", check=%"WHALLOC_SIZE_T_PFMT", self->base.bitCount=%"WHALLOC_SIZE_T_PFMT", hashMask=0x%04x/%d\n",
            size, maxItems, blockSize, check,self->base.bitCount,self->base.hashMask,self->base.hashMask));
    LOGSELF(("mem=%p, uspace=%p, allocatable bytes=%"WHALLOC_SIZE_T_PFMT"\n",
            self->base.mem, self->base.uspace,self->base.usize));
#if 0
    /* FIXME: if we're wasting self->ht entries (that is, some are
     unreachable in the memory range), shrink self->ht until we get
     a nice fit. If we're wasting memory (entries which can never be
     used), we should shrink the list and adjust self->base.uspace/self->base.usize
     accordingly.

     See WHALLOC_API(bt_init)() for how a similar problem is solved there.
    */
#endif
    ht = self->ht.head;
    LOGSELF(("Hashtable@%p: %"WHALLOC_SIZE_T_PFMT" entries using %"WHALLOC_SIZE_T_PFMT" bytes (%1.2f%%).\n",
            (void const *)ht,self->ht.len, self->ht.sizeOf,
            (1.0*self->ht.sizeOf)/(size)*100));
    for( i = 0; i < self->ht.len; ++i, ++ht )
    {
        *ht = WHALLOC_API(ht_entry_empty);
    }
    i = 0;
    rc = 0;
    for( x = 0; (x < self->base.usize); x += blockSize, ++i )
    {
        /**
           Try to hash all block boundaries:

           a) To ensure no hash collisions.

           b) Try to figure out how much space we might be wasting on
           unused hash entries.

           c) To ensure we allocated enough hash entries.
        */
        unsigned char const * addr;
        WHALLOC_API(size_t) hash;
        WHALLOC_API(ht_entry) * e;
        addr = self->base.uspace + x;
        assert( whalloc_ht_hash_offset( self, x ) == whalloc_ht_hash_addr( self, addr ) );

        hash =
            whalloc_ht_hash_offset( self, x )
            /*whalloc_ht_hash_addr( self, addr )*/
            ;
        if( WHALLOC_API(rc).HashCodeError == hash )
        {
            LOGSELF(("Step #%"WHALLOC_SIZE_T_PFMT" Hash of addr offset %"WHALLOC_SIZE_T_PFMT" @ %p got unexpected hash value %"WHALLOC_SIZE_T_PFMT"!. self->ht.end=%"WHALLOC_SIZE_T_PFMT"\n",
                     i, x, addr, hash, self->ht.end ));
            rc = WHALLOC_API(rc).HashingError;
            break;
        }
        e = &self->ht.head[hash];
        if( WHALLOC_FLAG_USED & ENTRY_GETFL(e) )
        {
            LOGSELF(("Step #%"WHALLOC_SIZE_T_PFMT" Hash of addr offset %"WHALLOC_SIZE_T_PFMT" @ %p = %"WHALLOC_SIZE_T_PFMT" got collision!\n",i, x, addr, hash ));
            rc = WHALLOC_API(rc).HashingError;
            break;
        }
        e->lenfl |= WHALLOC_FLAG_USED; /* just for checking for collisions, above. We'll undo this in a moment.*/
        /*LOGSELF(("Step #%"WHALLOC_SIZE_T_PFMT" Hash of addr offset %"WHALLOC_SIZE_T_PFMT" @ %p = %"WHALLOC_SIZE_T_PFMT"\n",i, x, addr, hash ));*/
    }
    self->base.blockCount = self->ht.end = i;
    
    memset( self->ht.head, 0, self->ht.sizeOf ); /* Unset the USED flags we set above for testing purposes*/
    if( i > self->ht.len )
    {
        LOGSELF(("WARNING: overstepped table length by about %"WHALLOC_SIZE_T_PFMT" items.\n",
                 (i-self->ht.len)));
        return WHALLOC_API(rc).HashingError;
    }
    else
    {
        if( i )
        {
            /**
               FIXME: if we have enough space left, squeeze out the
               extra ht entries, adjusting: self->base.uspace, self->base.usize,
               ht->len until ht->len==(ht->end-1) or until we don't
               have another full block.
               
               Or calculate the number of hashtable entries more
               accurately earlier on.
            */
            /*MARKER(("Wasted hash entries: %"WHALLOC_SIZE_T_PFMT".\n",(self->ht.len-i)));*/
        }
    }
    LOGSELF(("ht.end=%"WHALLOC_SIZE_T_PFMT", ht.len=%"WHALLOC_SIZE_T_PFMT", ht.sizeOf=%"WHALLOC_SIZE_T_PFMT"\n",self->ht.end,self->ht.len,self->ht.sizeOf));
#undef HASH
    return rc;
}


/** @internal

Tries to find an unused range of memory. On success it marks the all
blocks of the span as in-use and marks the first element with the size
of the range.


startIndex = self->ht.head index to start at. It is a hint. if it's
in use then it will be skipped over.

size = size, in bytes, to allocate. It must be greater than 0.

self is assumed to already be locked - this function does
not lock it.

On success, returns the index in self->ht.head of the page containing
the memory record. On error it returns a value equal to or greater
than self->ht.end (WHALLOC_API(rc).HashCodeError if !self or !size).
The first record will contain the size parameter, and all subsequent
blocks, if any, will be marked as in-use but having 0 bytes. That
is a signal to the deallocator that those blocks "belong" to another
block somewhere to the left of that block.

This function adjusts self->base.allocBlockCount and
self->base.allocCount. It also possibly updates self->freeIndexHint.
*/
static WHALLOC_API(size_t) WHALLOC_API(ht_mark_range)( WHALLOC_API(ht) * const self,
                                               WHALLOC_API(size_t) startIndex,
                                               WHALLOC_API(size_t) size )
{
    WHALLOC_API(size_t) pos;
    WHALLOC_API(size_t) endex;
    WHALLOC_API(ht_entry) * e1;
    WHALLOC_API(ht_entry) * e2;
    WHALLOC_API(size_t) stride;
    if( ! self || ! size ) return WHALLOC_API(rc).HashCodeError;
    LOGSELF(("trying to mark range of %"WHALLOC_SIZE_T_PFMT" bytes starting at index %"WHALLOC_SIZE_T_PFMT"...\n",
             size, startIndex));
    /**
       Overall algorithm:
    
       endex = startIndex + (size/self->base.blockSize)

       If endex>=self->ht.end return error

       If endex is in-use:

       - startIndex = endex+1
       - If startIndex>=self->ht.end, return error
       - Else try again

       If endex is unused:

       - Mark all blocks between (startIndex,endex) as used.
       - Set self->ht[startIndex.lenfl length part] to size.
    */
    if( startIndex >= self->ht.end )
    {
        return self->ht.end;
    }
    endex = self->ht.end;
    e1 = 0;
    e2 = 0;
    stride = (size/self->base.blockSize);
    if( stride && !(size%self->base.blockSize) ) --stride;
    do
    {
        WHALLOC_API(ht_entry) * check;
        WHALLOC_API(size_t) lastFree;
        endex = startIndex + stride;
        if( endex >= self->ht.end )
        {
            return self->ht.end;
        }
        e1 = &self->ht.head[startIndex];
        e2 = &self->ht.head[endex];
        LOGSELF(("Checking range [%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT"] ...\n",startIndex,endex));
        if( ENTRY_IS_USED(e2) )
        {
            startIndex = endex + 1;
            continue;
        }
        /* ensure the whole range is not in use:*/
        check = e2;
        lastFree = endex;
        for( ; (check != e1) && !ENTRY_IS_USED(check);
             --check, --lastFree )
        {
        }
        if( check == e1 ) break; /* all blocks between [e1,e2] are free :-D*/
        startIndex = lastFree+1;
        /* try again.*/
    } while(1);
    if( (endex >= self->ht.end) || (startIndex >= self->ht.end) )
    {
        return self->ht.end;
    }
    LOGSELF(("Using range [%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT"]\n",startIndex,endex));
    for( pos = startIndex ; pos <= endex; ++pos )
    {
        e1 = &self->ht.head[pos];
        ENTRY_SET_USE_ALLOC(e1,(pos==startIndex) ? size : 0);
        LOGSELF(("Marking #%"WHALLOC_SIZE_T_PFMT" encoded=%08x, e->size=%"WHALLOC_SIZE_T_PFMT"\n",pos, e1->lenfl,ENTRY_GETLEN(e1)));
        ++self->base.allocBlockCount;
    }
    /* e1 = &self->ht.head[startIndex]; */
    ++self->base.allocCount;
    if(1)
    {
        unsigned char const * rawp;
        rawp = WHALLOC_API(ht_mem_for_hash)( self, startIndex );
        assert( rawp && "internal error in WHALLOC_API(ht_mark_range)!" );
    }
    self->base.freeIndexHint = endex+1;
    return startIndex;
}

/**@ internal
   The converse of WHALLOC_API(ht_mark_range)().

   Clears a range of blocks for re-use.

   startIndex must refer to an in-use hashtable entry. That block,
   plus any others to its right associated via a multi-block
   allocation, are marked as free.

   Returns WHALLOC_API(rc).OK on error. If it returns WHALLOC_API(rc).InternalError
   then the pool state is probably corrupt or unexpected. It returns
   WHALLOC_API(rc).ArgError if !self and WHALLOC_API(rc).RangeError if
   startIndex is not less than self->bits.end. 

   This function adjusts self->base.allocBlockCount and
   self->base.allocCount. It also possibly updates
   self->freeIndexHint.

*/
static int WHALLOC_API(ht_clear_range)( WHALLOC_API(ht) * const self,
                                   WHALLOC_API(size_t) startIndex )
{
    WHALLOC_API(size_t) pos;
    WHALLOC_API(ht_entry) * e;
    WHALLOC_API(size_t) xblocks;
    WHALLOC_API(size_t) end;
    if( ! self )
    {
        return WHALLOC_API(rc).ArgError;
    }
    else if( startIndex >= self->ht.end)
    {
        return WHALLOC_API(rc).RangeError;
    }
    pos = startIndex;
    e = &self->ht.head[pos];
    /* WHALLOC_API(size_t) slots = (ENTRY_GETLEN(e) / self->base.blockSize); */
    xblocks = (ENTRY_GETLEN(e) / self->base.blockSize);
    if( xblocks && !(ENTRY_GETLEN(e) % self->base.blockSize) ) --xblocks;
    end = pos + xblocks + 1;
    LOGSELF(("Clearing range (%"WHALLOC_SIZE_T_PFMT",%"WHALLOC_SIZE_T_PFMT"]. Len=%"WHALLOC_SIZE_T_PFMT", blocks=%"WHALLOC_SIZE_T_PFMT", Block Size=%"WHALLOC_SIZE_T_PFMT"...\n",
             pos,end,ENTRY_GETLEN(e),1+xblocks,self->base.blockSize));
    for( ; pos < end; ++pos,
             (e = &self->ht.head[pos]) )
    {
        if( ! ENTRY_IS_USED(e) )
        {
            LOGSELF(("Internal error: we were expecting that hashtable entry #%"WHALLOC_SIZE_T_PFMT" was marked as used. encoded=%08x!\n",
                     pos, e->lenfl ));
            return WHALLOC_API(rc).InternalError;
        }
        if( startIndex == pos )
        {
            if( ! ENTRY_GETLEN(e) )
            {
                LOGSELF(("Expecting non-zero length at entry #%"WHALLOC_SIZE_T_PFMT"!\n",pos));
                return WHALLOC_API(rc).InternalError;
            }
        }
        else
        {
            if( ENTRY_GETLEN(e) )
            {
                LOGSELF(("Expecting zero length at entry #%"WHALLOC_SIZE_T_PFMT"!\n",pos));
                return WHALLOC_API(rc).InternalError;
            }
        }
        LOGSELF(("Marking entry #%"WHALLOC_SIZE_T_PFMT" as unused. It says it owns %"WHALLOC_SIZE_T_PFMT" bytes.\n",
                 pos, ENTRY_GETLEN(e)));
        *e = WHALLOC_API(ht_entry_empty);
        --self->base.allocBlockCount;
    }
    --self->base.allocCount;
    if( self->base.freeIndexHint > startIndex )
    {
        self->base.freeIndexHint = startIndex;
    }
    return WHALLOC_API(rc).OK;    
}

static void*
WHALLOC_API(ht_alloc_ex)(
                 WHALLOC_API(ht) * const self,
                 WHALLOC_API(size_t) size,
                 size_t align /* old artifact - not respected! */
                 ) {
    void * fb = 0;
    WHALLOC_API(size_t) hash;
    unsigned char * rawp;
    if( ! self ) return 0;
    LOCK_OR(NULL);
#define FBALLOC if(self->base.fallback.free && self->base.fallback.realloc) do { fb = self->base.fallback.realloc(0,size); UNLOCK; return fb; } while(0)
    if (! size) {
        size = self->base.blockSize;
    }

    if (! align) {
#if WHALLOC_USE_ALIGN
        align = WHALLOC_ALIGN_MAX;
#endif
    }
    LOGSELF(("pool=@%p, size=%"WHALLOC_SIZE_T_PFMT", align=%"WHALLOC_SIZE_T_PFMT"\n",(void const *)self, size, align));

    if( (self->base.allocCount>=self->ht.end /*we're full*/)
        /* || (self->base.blockSize < size) //FIXME: alloc > blockSize */
        )
    {
        LOGSELF(("pool=@%p cannot allocate %"WHALLOC_SIZE_T_PFMT" bytes. Trying fallback.\n",(void const *)self, size));
        FBALLOC;
        UNLOCK;
        return 0;
    }
  
    hash = 0;
    hash =
        whalloc_ht_hash_offset( self, self->base.freeIndexHint * self->base.blockSize )
        ;
    LOGSELF(("self->freeIndexHint=%"WHALLOC_SIZE_T_PFMT", hash=%"WHALLOC_SIZE_T_PFMT"\n",self->base.freeIndexHint,hash));
    if( hash >= self->ht.end ) hash = 0;
    hash = WHALLOC_API(ht_mark_range)( self, hash, size );/*  FIXME: size+ALIGNMENT? */
    if( hash >= self->ht.end )
    {
        LOGSELF(("Could not allocate %"WHALLOC_SIZE_T_PFMT" bytes!\n",size));
        FBALLOC;
        UNLOCK;
        return 0;
    }
    rawp = WHALLOC_API(ht_mem_for_hash)( self, hash );
    LOGSELF(("Allocating %"WHALLOC_SIZE_T_PFMT" bytes starting at @%p\n",size,rawp));
    UNLOCK;
    return rawp;
#undef FBALLOC
}

void * WHALLOC_API(ht_alloc)( WHALLOC_API(ht) * const self, WHALLOC_API(size_t) size )
{
    return WHALLOC_API(ht_alloc_ex)(self, size, WHALLOC_ALIGN_MAX);
}


int
WHALLOC_API(ht_drain)( WHALLOC_API(ht) * const self )
{
    if( ! self ) return WHALLOC_API(rc).ArgError;
    LOCK_OR(WHALLOC_API(rc).LockingError);
    /* const size_t esz = sizeof(WHALLOC_API(ht_entry))*self->emax; */
    self->base.allocCount = 0;
    memset( self->base.mem, 0, self->base.size );
    LOGSELF(("WHALLOC_API(ht_drain)(%p) {}\n",(void const*)self));
    UNLOCK;
    return 0;
}

int WHALLOC_API(ht_free)( WHALLOC_API(ht) * const self, void * m )
{
    unsigned char const * ucm;
    WHALLOC_API(size_t) hash;
    int drc;
    if( ! self || !m ) return WHALLOC_API(rc).ArgError;
#define FBFREE if(self->base.fallback.free) { self->base.fallback.free(m); UNLOCK; return WHALLOC_API(rc).OK; }

    LOCK_OR(WHALLOC_API(rc).LockingError);
    ucm = (unsigned char const *)m;
    hash = whalloc_ht_hash_addr( self, m );
    LOGSELF(("delloc @%p, hash=%"WHALLOC_SIZE_T_PFMT"\n",m,hash));
    assert( ucm == WHALLOC_API(ht_mem_for_hash)( self, hash ) );
    if( hash >= self->ht.end )
    {
        LOGSELF(("Cannot dealloc memory @%p.%s\n",
                 m,(self->base.fallback.free ? " Trying fallback..." : "")));
        FBFREE;
        UNLOCK;
        assert(0 && "Allocated was asked to release memory it does not own!");
        return WHALLOC_API(rc).UsageError;
    }
    drc = WHALLOC_API(ht_clear_range)( self, hash );
    if( WHALLOC_API(rc).OK != drc )
    {
        LOGSELF(("Deallocation failed with rc %d!\n",drc));
    }
    if( self->base.freeIndexHint > hash )
    {
        self->base.freeIndexHint = hash;
    }
    UNLOCK;
    return drc;
#undef FBFREE
}

#undef ENTRY_ENCODE
#undef ENTRY_GETFL
#undef ENTRY_GETLEN
#undef ENTRY_LENGTH_BITS
#undef LOGSELF
#undef MARKER
#undef N_TO_ENTRYFL
#undef N_TO_ENTRYLEN
#undef WHALLOC_ALIGN_MAX
#undef WHALLOC_DEBUG
#undef WHALLOC_FLAG_MASK
#undef WHALLOC_FLAG_USED
#undef WHALLOC_LEN_MASK
#undef WHALLOC_LENGTH_BITS
#undef UNLOCK
#undef LOCK_OR
/* end file whalloc_ht.c */
/* begin file whalloc_pager.c */
#line 8 "whalloc_pager.c"
#include <assert.h>
#include <string.h> /* memset() */
#include <stdint.h> /* fixed-size integers. */
#include <stdlib.h> /* realloc() */



enum WHALLOC_PAGER_FLAGS
    {
    PAGER_BOOK_AUTO_VACUUM = 0x01
    };

#if !defined(NDEBUG)
#  include <stdio.h> /* for debugging only */
#  define LOGBOOK(B,mp_exp) if( (B) && (B)->log )              \
    { (B)->log("%s:%d:%s(): ",__FILE__,__LINE__,__func__);     \
        (B)->log mp_exp;                                       \
    } else (void)0
#  define MARKER(mp_exp) do{ printf("MARKER: %s:%d:%s(): ",__FILE__,__LINE__,__func__); printf mp_exp; } while(0)
#else
#  define LOGBOOK(B,mp_exp) (void)0
#  define MARKER(mp_exp) ((void)0)
#endif


/* Evalues to the number of bytes needed for N bits. */
#define BITCOUNT_TO_BYTES(N) ( ((N)/8) + (((N)&&(!((N)%8))) ? 0 : 1) )
/* Evalutes to the byte from array A storing the given BIT offset. */
#define BYTES_BYTEFOR(A,BIT) ((A)[ BIT / 8 ])
/* Sets the given bit in array A to 1. Evaluates to 1.*/
#define BYTES_SET(A,BIT) ((BYTES_BYTEFOR(A,BIT) |= (0x01 << (BIT%8))),0x01)
/* Sets the given bit in array A to 0. Evaluates to 0.*/
#define BYTES_UNSET(A,BIT) ((BYTES_BYTEFOR(A,BIT) &= ~(0x01 << (BIT%8))),0x00)
/* Gets the value (1 or 0) of the given BIT in array A. */
#define BYTES_GET(A,BIT) ((BYTES_BYTEFOR(A,BIT) & (0x01 << (BIT%8))) ? 0x01 : 0x00)
/**
   Toggles the given BIT in array A. Evaluates to the new value of
   that bit (1 or 0).
*/
#define BYTES_TOGGLE(A,BIT) (BYTES_GET(A,BIT) ? (BYTES_UNSET(A,BIT)) : (BYTES_SET(B,BIT)))

/*
  #define BOOK_BYTEOF(P,BIT) ((P)->flags[ BIT / 8 ])
*/
#define CHUNK_IS_USED(P,BIT) BYTES_GET((P)->flags,(BIT))
/* Sets the given chunk (BIT) of page P to the given value (1 or 0).
 Evaluates to 1 (if VAL) or 0. */
#define CHUNK_SET_USAGE(P,BIT,VAL) ((VAL)?BYTES_SET((P)->flags,BIT):BYTES_UNSET((P)->flags,BIT))
/* Sets the given chunk-as-used BIT in page P. Evaluates to 1.*/
#define CHUNK_USE(P,BIT) CHUNK_SET_USAGE(P,BIT,1)
/* Clears the given chunk-is-used BIT in page P. Evaluates to 0.*/
#define CHUNK_UNUSE(P,BIT) CHUNK_SET_USAGE(P,BIT,0)


const WHALLOC_API(page) WHALLOC_API(page_empty) = {
0/*length*/,
0/*chunkSize*/,
0/*useCount*/,
0/*nextFree*/,
NULL/*flags*/,
NULL/*pool*/,
NULL/*next*/
};

/** @internal
   Implements WHALLOC_API(realloc_f) interface. It ignores the allocState
   argument and uses realloc(2).
*/
static void * WHALLOC_API(pager_realloc_default)( void * m, unsigned int n, void * allocState );

const WHALLOC_API(book) WHALLOC_API(book_empty) = {
0/*pageLength*/,
0/*chunkSize*/,
0/*flags*/,
NULL/*page*/,
NULL/*log*/,
whalloc_allocator_empty_m/*alloc*/,
whalloc_allocator_empty_m/*pageAlloc*/,
whalloc_mutex_empty_m/*mutex*/
};

#define BOOK_LOCK_OR(B,COND,RC) if( (COND) && (B)->mutex.lock ) { if( 0 != (B)->mutex.lock( (B)->mutex.state ) ) return RC; }
#define BOOK_UNLOCK(B,COND) if( (COND) && (B)->mutex.unlock ) (B)->mutex.unlock( (B)->mutex.state )


void * WHALLOC_API(pager_realloc_default)( void * m, unsigned int n, void * _allocState )
{
    return realloc( m, n );
}

/** @internal

    A realloc(3)-proxying WHALLOC_API(allocator) object.
*/
static const WHALLOC_API(allocator) WHALLOC_API(pager_allocator_default) =
{
WHALLOC_API(pager_realloc_default),
NULL
};

unsigned int WHALLOC_API(page_calc_size)( uint16_t n, uint16_t chunkSize )
{
    if( !n || !chunkSize ) return 0;
    else
    {
        return WHALLOC_API_PAGE_CALC_SIZE( n, chunkSize );
    }
        
}
unsigned int WHALLOC_API(page_sizeof)( WHALLOC_API(page) const * p )
{
    return p
        ? WHALLOC_API(page_calc_size)( p->length, p->chunkSize )
        : 0;
}

WHALLOC_API(page) * WHALLOC_API(page_new2)( uint16_t n, uint16_t chunkSize,
                                           WHALLOC_API(allocator) const * alloc)
{
    const uint16_t sz = WHALLOC_API(page_calc_size)(n, chunkSize);
    unsigned char * mem = NULL;
    WHALLOC_API(page) * page = NULL;
    if( ! sz ) return NULL;
    if( (NULL == alloc) || (NULL == alloc->realloc) )
    {
        alloc = &WHALLOC_API(pager_allocator_default);
    }
    mem = (unsigned char *) alloc->realloc( NULL, sz, alloc->state );
    if( ! mem ) return NULL;
    memset( mem, 0, sz ); /* valgrind bitches without this! */
    page = (WHALLOC_API(page)*) mem;
    *page = WHALLOC_API(page_empty);
    page->alloc = *alloc;
    page->length = n;
    page->chunkSize = chunkSize;
    page->useCount = 0;
    /* FIXME: move flags to end so we can guarany alignment of page->pool. */
    page->flags = mem + sizeof(WHALLOC_API(page));
    page->pool = page->flags + BITCOUNT_TO_BYTES(n);
    page->next = page->prev = NULL;
    return page;    
}
WHALLOC_API(page) * WHALLOC_API(page_new)( uint16_t n, uint16_t chunkSize,
                                           WHALLOC_API(realloc_f) alloc, void * allocState)
{
    WHALLOC_API(allocator) A = WHALLOC_API(allocator_empty);
    A.realloc = alloc;
    A.state = allocState;
    return WHALLOC_API(page_new2)( n, chunkSize, &A );
}

void WHALLOC_API(page_finalize)( WHALLOC_API(page) * p )
{
    if( p )
    {
        void * ignored;
        ignored = p->alloc.realloc( p, 0, p->alloc.state );
    }
    return;
}

void * WHALLOC_API(page_alloc)( WHALLOC_API(page) * p )
{
    if( ! p ) return NULL;
#if 0
    else if( p->useCount >= p->length ) return NULL;
#else
    else if( p->nextFree >= p->length ) return NULL;
#endif
    else
    {
        uint16_t i = p->nextFree;
#if 0
        if( p->nextFree >= 8 )
        {
            if( (p->length > 8) && (p->useCount > 8) )
            { /* Optimization to check if sets of 8
                 (one byte in p->flags) are set. Cuts
                 down the looping on full large pages.

                 But only works as-is if i starts at 0,
                 which invalidates any effect of using
                 p->nextFree. :/
              */
                uint16_t x = 0;
                for( ; x < BITCOUNT_TO_BYTES(p->length); ++x )
                {
                    if( 0xFF == p->flags[x] )
                    {
                        i += 8;
                    }
                    else if( 0xF0 == p->flags[x] )
                    {
                        i += 4;
                    }
                    else break;            
                }
            }
        }
#endif
        for( ; i < p->length; ++i )
        {
            if( CHUNK_IS_USED(p,i) ) continue;
            CHUNK_USE(p,i);
            if( p->nextFree == i )
            {
                ++p->nextFree;
            }
            ++p->useCount;
            return p->pool + (p->chunkSize * i);
        }
        MARKER(("You shouldn't have gotten this far: page@%p, i=%"WHALLOC_SIZE_T_PFMT", p->length=%"WHALLOC_SIZE_T_PFMT", p->used=%"WHALLOC_SIZE_T_PFMT"\n",
                (void const *)p, i, p->length,p->useCount));
        assert( 0 && "This theoretically cannot be reached!" );
        return NULL;
    }
}

char WHALLOC_API(page_owns_mem)( WHALLOC_API(page) const * p, void const * m )
{
    if(!p || !m ) return 0;
    else
    {
        unsigned char const * mc = (unsigned char const *)m;
        if( (mc < p->pool)
            || (mc > ((p->pool + (p->chunkSize * p->length))))
            )
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}

int WHALLOC_API(page_free)( WHALLOC_API(page) * p, void * mem )
{
    unsigned char const * mc = (unsigned char const *)mem;
    /*MARKER(("mc=@%p\n",(void const *)mc));*/
    if( ! p ) return -1;
    else if( ! WHALLOC_API(page_owns_mem)(p,mem) )
    {
        return -1;
    }
    else
    {
        const uint16_t pos = (mc - p->pool) / p->chunkSize;
        /*MARKER(("pos=%u"\n",pos));*/
        CHUNK_UNUSE(p,pos);
        --p->useCount;
        if( p->nextFree > pos )
        {
            p->nextFree = pos;
        }
        return 0;
    }
}

int WHALLOC_API(page_erase)( WHALLOC_API(page) * p )
{
    if( ! p ) return -1;
    else
    {
        memset( p->flags, 0, BITCOUNT_TO_BYTES(p->length) );
#if 0 /* highly arguable. */
        memset( p->pool, 0, p->length * p->chunkSize );
#error "Re-add the API docs in the following comment lines to the public API!"
        /**
           This routine effectively performs a memset() on the page's entire
           contents, and thus has O(N) performance, where N is a function of
           (but not exactly equal to) (p->length * p->chunkSize). This is
           highly arguably, but is there to help avoid mis-use of deallocated
           memory.
         */
#endif
        p->nextFree = p->useCount = 0;
        return 0;
    }
}

char WHALLOC_API(page_is_full)( WHALLOC_API(page) const * p )
{
    return p
        ? ((p->nextFree >= p->length) ? 1 : 0)
        : 0;
}

int WHALLOC_API(page_insert_after)( WHALLOC_API(page) * p, WHALLOC_API(page) * after )
{
    if( !p || !after ) return -1;
    else if( p->prev || p->next ) return 1;
    else
    {
        if( after->next )
        {
            after->next->prev = p;
            p->next = after->next;
        }
        after->next = p;
        p->prev = after;
        return 0;
    }
}

int WHALLOC_API(page_insert_before)( WHALLOC_API(page) * p, WHALLOC_API(page) * before )
{
    if( !p || !before ) return -1;
    else if( p->prev || p->next ) return 1;
    else
    {
        if( before->prev )
        {
            before->prev->next = p;
            p->prev = before->prev;
        }
        before->prev = p;
        p->next = before;
        return 0;
    }
}

WHALLOC_API(page)* WHALLOC_API(page_snip)( WHALLOC_API(page) * p )
{
    if( !p ) return NULL;
    else
    {
        WHALLOC_API(page)* rc = NULL;
        if( p->prev )
        {
            p->prev->next = p->next;
            rc = p->prev;
        }
        if( p->next )
        {
            p->next->prev = p->prev;
            rc = p->next;
        }
        p->prev = p->next = NULL;
        return rc;
    }
}

int WHALLOC_API(page_move_right)( WHALLOC_API(page) * p )
{
    if( !p ) return -1;
    else if( ! p->next ) return 0;
    else
    {
#if !defined(NDEBUG)
        WHALLOC_API(page) const * pr = p->next;
#endif
        WHALLOC_API(page) * s = WHALLOC_API(page_snip)( p );
        assert( s == pr );
        WHALLOC_API(page_insert_after)( p, s );
        return 0;
    }
}

int WHALLOC_API(page_move_left)( WHALLOC_API(page) * p )
{
    if( !p ) return -1;
    else if( ! p->prev ) return 0;
    else
    {
        WHALLOC_API(page) * pr = p->prev;
        WHALLOC_API(page) * ignored;
        ignored = WHALLOC_API(page_snip)( p );
        WHALLOC_API(page_insert_before)( p, pr );
        return 0;
    }
}

WHALLOC_API(book) * WHALLOC_API(book_open2)( uint16_t n, uint16_t chunkSize,
                                             WHALLOC_API(allocator) const * alloc,
                                             WHALLOC_API(allocator) const * pageAlloc )
{
    const size_t sz = sizeof(WHALLOC_API(book));
    WHALLOC_API(book) * b = NULL;
    WHALLOC_API(allocator) const * AB =
        (alloc && alloc->realloc)
        ? alloc
        : &WHALLOC_API(pager_allocator_default);
    WHALLOC_API(allocator) const * AP =
        (pageAlloc && pageAlloc->realloc)
        ? pageAlloc
        : &WHALLOC_API(pager_allocator_default);
    if( !chunkSize || !n ) return NULL;
    b = (WHALLOC_API(book)*) AB->realloc( NULL, sz, AB->state );
    if( ! b ) return NULL;
    memset( b, 0, sz ); /* valgrind bitches without this, even though the next line does the same thing! */
    *b = WHALLOC_API(book_empty);
    b->pageLength = n;
    b->chunkSize = chunkSize;
    b->alloc = *AB;
    b->pageAlloc = *AP;
    return b;
}



WHALLOC_API(book) * WHALLOC_API(book_open)( uint16_t n, uint16_t chunkSize,
                                            WHALLOC_API(realloc_f) alloc, void * allocState,
                                            WHALLOC_API(realloc_f) pageAlloc, void * pageAllocState
                                            )
{
#if 1
    WHALLOC_API(allocator) ab;
    WHALLOC_API(allocator) ap;
    ab.realloc = alloc;
    ab.state = allocState;
    ap.realloc = pageAlloc;
    ap.state = pageAllocState;
    return WHALLOC_API(book_open2)( n, chunkSize, &ab, &ap );
#else
    const size_t sz = sizeof(WHALLOC_API(book));
    WHALLOC_API(book) * b = NULL;
    if( NULL == alloc )
    {
        alloc = WHALLOC_API(pager_realloc_default);
        allocState = NULL;
    }
    if( NULL == pageAlloc )
    {
        pageAlloc = WHALLOC_API(pager_realloc_default);
        pageAllocState = NULL;
    }
    if( !chunkSize || !n ) return NULL;
    b = (WHALLOC_API(book)*) alloc( NULL, sz, allocState );
    if( ! b ) return NULL;
    memset( b, 0, sz ); /* valgrind bitches without this, even though the next line does the same thing! */
    *b = WHALLOC_API(book_empty);
    b->pageLength = n;
    b->chunkSize = chunkSize;
    b->alloc.realloc = alloc;
    b->alloc.state = allocState;
    b->pageAlloc.realloc = pageAlloc;
    b->pageAlloc.state = pageAllocState;
    return b;
#endif
}


int WHALLOC_API(book_close)( WHALLOC_API(book) * b )
{
    if( ! b ) return WHALLOC_API(rc).ArgError;
    else
    {
        WHALLOC_API(mutex) mutex;
        WHALLOC_API(page) * p = NULL;
        void * allocState = NULL;
        WHALLOC_API(realloc_f) balloc = NULL;
        BOOK_LOCK_OR(b,1,WHALLOC_API(rc).LockingError);
        mutex = b->mutex;
        allocState = b->alloc.state;
        balloc = b->alloc.realloc;
        p = b->page;
        *b = WHALLOC_API(book_empty) /* helps avoid mis-use after freeing. */;
        balloc( b, 0, allocState );
        while( p && p->prev ) p = p->prev;
        while( p )
        {
            WHALLOC_API(page) * x = WHALLOC_API(page_snip)( p );
            WHALLOC_API(page_finalize)( p );
            p = x;
        }
        if(mutex.unlock) mutex.unlock(mutex.state);
        return 0;        
    }
}

void * WHALLOC_API(book_open_alloc_reuse)( void * m, unsigned int n, void * allocState )
{
    return n ? allocState : NULL;
}

/** @internal

   Internal impl for WHALLOC_API(book_add_page). If doLock is 1 then
   this function will lock/unlock b->mutex IF b->mutex.lock is
   non-null.  If doLock is 0, the caller is assumed to have locked and
   no locking is done.
*/
static WHALLOC_API(page) * WHALLOC_API(book_add_page_impl)( WHALLOC_API(book) * b, char doLock )
{
    if( ! b ) return NULL;
    else
    {
        /**
           In theory...

           we could move the lock to after the allocation (and only
           lock if allocation succeeds). This implies, however, that:

           a) b->pageAlloc/pageAllocState are not modified (their API
           docs specify that they must not be modified, so doing so is
           blatant mis-use).

           b) b->pageAlloc.realloc does its own locking.

           If in fact b->pageAlloc.realloc does its own locking AND
           its using the same mutex as b, then us indirectly locking
           it here will cause a deadlock if the mutex is not
           recursive.
        */
        WHALLOC_API(page) * np = NULL;
        BOOK_LOCK_OR(b, doLock,NULL);
        np = WHALLOC_API(page_new2)( b->pageLength,
                                     b->chunkSize,
                                     &b->pageAlloc );
        if( np )
        {
            if( b->page )
            {
                WHALLOC_API(page_insert_before)( np, b->page );
            }
            b->page = np;
        }
        LOGBOOK(b,("Added new page @%p to the book.\n",(void const *)np));
        BOOK_UNLOCK(b, doLock);
        return np;
    }
}
WHALLOC_API(page) * WHALLOC_API(book_add_page)( WHALLOC_API(book) * b )
{
    return WHALLOC_API(book_add_page_impl)( b, 1 );
}

void * WHALLOC_API(book_alloc)( WHALLOC_API(book) * b )
{
    if( ! b ) return NULL;
    else
    {
        /**
           Walk each page until we find space.

           Each time we allocate from a page, move it to the right if
           it has more allocations than page->next. The point being to
           keep pages with free slots up front in the list. This
           speeds up allocation but can have the opposite effect on
           deallocation if objects are destructed in the same order as
           they are created (which is not unusual, but not the norm, i
           think).
        */
        void * rc = NULL;
        WHALLOC_API(page) * p = NULL;
        BOOK_LOCK_OR(b,1,NULL);
        p = b->page;
        if( ! p )
        {
            p = WHALLOC_API(book_add_page_impl)( b, 0 );
            if( !p )
            {
                LOGBOOK(b,("Adding new page failed! Out of memory?\n"));
                BOOK_UNLOCK(b,1);
                return NULL;
            }
        }
        while( p )
        {
            /**
               Actually... the next page is guaranteed to be NULL
               or full due to the list-reordering. Thus if the first
               page is full we could skip to the end. However, it'd
               still be O(N).
            */
            if( WHALLOC_API(page_is_full)( p ) )
            {
                LOGBOOK(b,("Skipping full page @%p.\n",(void const *)p));
                p = p->next;
                continue;
            }
            rc = WHALLOC_API(page_alloc)(p);
            break;
        }
        if( !rc )
        { /* (most likely) all pages were full */
            p = WHALLOC_API(book_add_page)( b );
            if( ! p )
            { /* OOM. */
                LOGBOOK(b,("Page allocation failed!\n"));
                BOOK_UNLOCK(b,1);
                return NULL;
            }
            rc = WHALLOC_API(page_alloc)(p);
        }
        if( ! rc )
        {
            LOGBOOK(b,("This shouldn't be able to happen: allocation failed but we have free page slots!\n"));
            BOOK_UNLOCK(b,1);
            assert( rc && "This shouldn't be able to happen!");
            return NULL;
        }
        if( p->next )
        {
            while( p->useCount > p->next->useCount )
            {
                LOGBOOK(b,("Moving page @%p to the right.\n",(void const *)p));
                WHALLOC_API(page_move_right)( p );
            }
        }
        if( p->prev )
        {
            while( p->prev && (p->useCount < p->prev->useCount) )
            {
                LOGBOOK(b,("Moving page @%p to the left.\n",(void const *)p));
                WHALLOC_API(page_move_right)( p->prev );
            }
        }
        while( b->page->prev )
        {
            /* kludge to ensure b->page is the head, until i work out
               some list-organization details. */
            b->page = b->page->prev;
        }
        LOGBOOK(b,("Allocated object @%p from page @%p!\n",(void const *)rc,(void const *)p));
        BOOK_UNLOCK(b,1);
        return rc;
    }
}

int WHALLOC_API(book_free)( WHALLOC_API(book) * b, void * mem )
{
    if( ! b ) return -1;
    else
    {
        
        WHALLOC_API(page) * p = NULL;
        int rc = 0;
        BOOK_LOCK_OR(b,1,WHALLOC_API(rc).LockingError);
        p = b->page;
        while( p && ! WHALLOC_API(page_owns_mem)(p, mem) )
        {
            p = p->next;
        }
        if( ! p )
        {
            BOOK_UNLOCK(b,1);
            return 1;
        }
        rc = WHALLOC_API(page_free)(p, mem);
        if( 0 == rc )
        {
            LOGBOOK(b,("Deallocated object @%p from page @%p!\n",(void const *)mem,(void const *)p));
            if( !p->useCount
                && (b->flags & PAGER_BOOK_AUTO_VACUUM)
                )
            { /** Deallocate p. */
                WHALLOC_API(page) * s = WHALLOC_API(page_snip)( p );
                if( b->page == p )
                {
                    b->page = s ? (s->prev ? s->prev : s) : s;
                }
                LOGBOOK(b,("Auto-vacuuming page @%p.\n",(void const *)p));
                WHALLOC_API(page_finalize)(p);
                p = 0;
            }
            if( p )
            {
                while( p->prev && (p->prev->useCount > p->useCount) )
                {
                    WHALLOC_API( page_move_left(p) );
                }
                if( ! p->prev )
                {
                    b->page = p;
                }
            }
        }
        else
        {
            LOGBOOK(b,("Deallocation of object @%p from page @%p failed!\n",(void const *)mem,(void const *)p));
            assert(0 && "DEallocation of object from page failed!");
        }
        BOOK_UNLOCK(b,1);
        return rc;
#undef UNLOCK
    }
}

/** @internal

   Internal impl of WHALLOC_API(book_vacuum)(). If doLock is 1 then this
   function uses b->mutex (if set) to lock/unlock b. If doLock is 0
   then the caller is assumed to have locked b.

*/
static unsigned int WHALLOC_API(book_vacuum_impl)( WHALLOC_API(book) * b,
                                                   char doLock )
{
    if( ! b || !b->page ) return 0;
    else
    {
        size_t rc = 0;
        WHALLOC_API(page) * p = NULL;
        WHALLOC_API(page) * x = NULL;
        BOOK_LOCK_OR(b,doLock,0);
        p = b->page;;
        while( p )
        {
            if( p->useCount )
            {
                p = p->next;
                continue;
            }
            ++rc;
            x = WHALLOC_API(page_snip)( p );
            LOGBOOK(b,("Vacuuming page @%p\n",(void const *)p));
            if( b->page == p )
            {
                b->page = x;
            }
            WHALLOC_API(page_finalize)( p );
            p = x;
        }
        while( b->page && b->page->prev )
        {
            b->page = b->page->prev;
        }
        BOOK_UNLOCK(b,doLock);
        return rc;
    }
}

unsigned int WHALLOC_API(book_vacuum)( WHALLOC_API(book) * b )
{
    return WHALLOC_API(book_vacuum_impl)( b, 1 );
}

int WHALLOC_API(book_vacuum_auto)( WHALLOC_API(book) * b, char autoVac )
{
    if( ! b ) return -1;
    else
    {
        BOOK_LOCK_OR(b,1,WHALLOC_API(rc).LockingError);
        b->flags |= PAGER_BOOK_AUTO_VACUUM;
        BOOK_UNLOCK(b,1);
        return 0;
    }
}

int WHALLOC_API(book_erase)( WHALLOC_API(book) * b, char alsoDeallocPages )
{
    if( ! b ) return -1;
    else
    {
        WHALLOC_API(page) * p = NULL;
        unsigned int count = 0;
        BOOK_LOCK_OR(b,1,WHALLOC_API(rc).LockingError);
        p = b->page;
        if( NULL != p )
        {
            while( p->prev ) p = p->prev;
            b->page = p;
            for( ; p ;)
            {
                WHALLOC_API(page_erase)( p );
                p = p->next;
                ++count;
            }
            if( alsoDeallocPages )
            {
                const unsigned int check = WHALLOC_API(book_vacuum_impl)( b, 0 );
                if(!check){/*avoid unused var warning.*/}
                assert( (check == count) && ! b->page );
            }
        }
        BOOK_UNLOCK(b,1);
        return 0;
    }
}


WHALLOC_API(book) * WHALLOC_API(book_from_bt)( WHALLOC_API(bt) * src,
                                               uint16_t pageLength,
                                               uint16_t chunkSize )
{

    WHALLOC_API(book) * b = NULL;
    WHALLOC_API(allocator) A = WHALLOC_API(allocator_empty);
    if( ! src || ! pageLength || ! chunkSize ) return 0;
    WHALLOC_API(bt_allocator)( src, &A );
    b = WHALLOC_API(book_open2)( pageLength, chunkSize,
                                 &A, &A );
    if( b && src->base.mutex.lock)
    {
        b->mutex = src->base.mutex;
    }
    return b;
}


#undef LOGBOOK
#undef MARKER

#undef BITCOUNT_TO_BYTES
#undef BYTES_BYTEFOR
#undef BYTES_SET
#undef BYTES_UNSET
#undef BYTES_GET
#undef BYTES_TOGGLE
/*
  #undef BOOK_BYTEOF
*/
#undef CHUNK_IS_USED
#undef CHUNK_SET_USAGE
#undef CHUNK_USE
#undef CHUNK_UNUSE
#undef BOOK_LOCK_OR
#undef BOOK_UNLOCK
/* end file whalloc_pager.c */
/* begin file whalloc_region.c */
#line 8 "whalloc_region.c"
#include <string.h> /* memset() */

const WHALLOC_API(region) WHALLOC_API(region_empty) = {
NULL/*mem*/,
NULL/*pos*/,
0/*length*/
};


int WHALLOC_API(region_init)( WHALLOC_API(region) * r, void * mem, WHALLOC_API(size_t) length )
{
    if( ! r || ! mem || !length ) return WHALLOC_API(rc).ArgError;
    else
    {
        r->mem = r->pos = mem;
        r->end = r->mem + length;
        memset( r->mem, 0, length );
        return 0;
    }
}
    
void * WHALLOC_API(region_alloc)( WHALLOC_API(region) * r, WHALLOC_API(size_t) n )
{
    if( ! r || ! n ) return NULL;
    else if( (r->pos + n) > r->end ) return NULL;
    else
    {
        void * rc = r->pos;
        r->pos += n;
        return rc;
    }
}

int WHALLOC_API(region_free)( WHALLOC_API(region) * r )
{
    if( ! r )
    {
        return WHALLOC_API(rc).ArgError;
    }
    else
    {
        r->pos = r->mem;
        memset( r->pos, 0, r->end - r->pos );
        return 0;
    }
}

/* end file whalloc_region.c */
/* end file src/whalloc_amalgamation.c */
/* begin file pfs/whio_epfs_internal.h */
#line 8 "pfs/whio_epfs_internal.h"
#ifndef WANDERINGHORSE_NET_WHIO_EPFS_INTERNAL_H_INCLUDED
#define WANDERINGHORSE_NET_WHIO_EPFS_INTERNAL_H_INCLUDED

/** @file whio_epfs_internal.h

whio_epfs_internal.h holds the declarations for most of the
whio_epfs-internal API (minus static members). The internal API is not
for use in generic client applications, and is subject to incompatible
changes without notice between versions.

i am considering moving much of this into a "public internal" API,
meaning they would be considered public (and API-stable) but low-level
(=="use at your own risk"). All such functions would be denoted by
the naming prefix whio_epfsi_ (i==internal).
*/

#ifdef __cplusplus
extern "C" {
#endif
    /** @deprecated

       Do not use.
       
       Clears any internal error flag and returns the previous error
       code.

       WARNING: the error code is not consistently used internally.
       Check the return values from API functions instead of relying
       on this.
    */
    int whio_epfs_clearerr( whio_epfs * fs );


    /** @internal

        Encodes val to dest, which must be at least whio_epfs_sizeof_id
        bytes long. Returns whio_epfs_sizeof_id on success.
    */
    whio_size_t whio_epfs_encode_id( unsigned char * dest, whio_epfs_id_t val );
    /** @internal

        Decodes val from src, which must be at least
        whio_epfs_sizeof_id bytes long. On success, val is updated and
        0 is returned. On error val is not modified and non-zero is
        returned.
    */
    int whio_epfs_decode_id( unsigned char const * dest, whio_epfs_id_t * val );

    /** @internal

        Seeks to the given position of fs->dev. Returns pos on
        success.
    */
    whio_size_t whio_epfs_seek( whio_epfs * fs, whio_size_t pos );
    
    /** @internal

        Sets or unsets the is-used flag for ino. Returns 0 on success,
        but it can only fail if !ino. If !u then ino is zeroed out
        except for its id.

        If (u && !ino->mtime) then whio_epfs_inode_touch(ino) is also
        called.

        This function may have to flush ino to storage in order to update
        the free-inode list.

        Returns 0 on success. If an error comes from the free-list handling
        part then the state of the free list is undefined and we're in all
        sorts of crap from then on out. Possibly. Our we might just get
        device-full errors when trying to allocate new inodes even though
        there are free items.
    */
    int whio_epfs_inode_set_used( whio_epfs * fs, whio_epfs_inode * ino, bool u );

    /** @internal

        Encodes ino to dest, which must be at least
        whio_epfs_sizeof_inode bytes long. Returns
        whio_epfs_sizeof_inode on success.
    */
    whio_size_t whio_epfs_inode_encode( whio_epfs_inode const * ino, unsigned char * dest );
    /** @internal

        Decodes src, which must be at least whio_epfs_sizeof_inode
        bytes long, to dest. Returns 0 on success.
    */
    int whio_epfs_inode_decode( whio_epfs_inode * ino, unsigned char const * src );

    /** @internal

        Returns the fs->dev position for the given inode ID, or 0 on
        error (!fs or ino out of range).
    */
    whio_size_t whio_epfs_inode_id_pos( whio_epfs const * fs, whio_epfs_id_t ino );

    /** @internal

        Writes n bytes to fs->dev. Returns n on success.
    */
    whio_size_t whio_epfs_write( whio_epfs * fs, void const * src, whio_size_t n );

    /** @internal

        Writes n bytes to fs->dev position pos. Returns n on success.
    */
    whio_size_t whio_epfs_writeat( whio_epfs * fs, whio_size_t pos, void const * src, whio_size_t n );

    /** @internal

        Reads n bytes from fs->dev. Returns n on success.
    */
    whio_size_t whio_epfs_read( whio_epfs * fs, void * dest, whio_size_t n );

    /** @internal

        Reads n bytes from fs->dev position pos. Returns n on success.
    */
    whio_size_t whio_epfs_readat( whio_epfs * fs, whio_size_t pos, void * dest, whio_size_t n );


    /** @internal

        Writes fs->hints to the storage. Returns 0 on success.
    */
    int whio_epfs_hints_write( whio_epfs * fs );

    /** @internal

        Writes ino to storage. Returns 0 on success.
    */
    int whio_epfs_inode_flush( whio_epfs * fs, whio_epfs_inode const * ino );

    /** @internal

        Tries to read the given inode ID from storage. On success 0 is
        returned and dest is populated with the fetched values. On
        error dest is not modified.
    */
    int whio_epfs_inode_read( whio_epfs * fs,
                              whio_epfs_id_t id,
                              whio_epfs_inode * dest );

    /** @internal

        Returns the client data container object for fs. The client
        may alter its contents (if he is certain what he is doing) but
        must not deallocate the returned whio_client_data object - it
        is owned by either fs or the client.
    */
    whio_client_data * whio_epfs_client_data( whio_epfs * fs );
    
    /** @internal

        Searches the inode table for the next free element. On success
        it returns 0 and dest is populated with a copy of the inode
        data. If markAsUsed is true then the record is marked as used
        and flushed to disk, otherwise the same record might be
        returned again on the next call to this function.

        Error conditions include:

        - !fs or !dest (whio_rc.ArgError).

        - No more free inodes (whio_rc.DeviceFullError).

        - Any potential errors via reading or writing the inode and
        updating the free-inode list.

        - markAsUsed is true but fs is opened for read-only more:
        whio_rc.AccessError.

        As of 20100310, the actual search is an O(1) operation but
        there is (if markAsUsed is true) a small i/o overhead to
        update the inode free-list. If markAsUsed is true it must
        read, at most, 2 additional inodes, update their free-list
        links, and flush them. The ids (and therefore on-storage
        positions) of all inodes involved are known in advance, so
        this routine has to do no search-related i/o. Note that such
        re-linking pays no attention whatsoever to whether an inode or
        its neighbors are opened (and therefore have their inode state
        cached somewhere in memory!). If the API is used properly then
        by the time an inode can be opened, all links to/from the
        inode and the free-list are severed.

        The down-side of the O(1) search guaranty is that if an i/o
        error happens while updating the free-list links, it could
        leave the free-list in an undefined state. That will either
        lead to prematurely "running out" of inodes (when there are
        actually some left) or to this routine taking longer to find
        the next free inode (it double-checks to ensure that the
        search led it to an unused inode). Or both. Probably both.

        TODO: add the above-mentioned potential corruption case to the
        future fsck-like functionality. We could theoretically rebuild
        the free list from scratch by linearly scanning for
        explicitly-marked used inodes.
    */
    int whio_epfs_inode_next_free( whio_epfs * fs, whio_epfs_inode * dest, bool markAsUsed );

    /** @internal

        Updates ino->mtime to the given timestamp, or the current time
        (in GMT) if time==-1. Does not flush ino to disk.

        Returns 0 on success, and the only error condition is when !ino.
    */
    int whio_epfs_inode_touch( whio_epfs_inode * ino, uint32_t time );

    /** @internal

        Encodes bl to dest, which must be at least
        whio_epfs_sizeof_blockMeta bytes long. Returns
        whio_epfs_sizeof_blockMeta on success. On error dest
        might be modified.
    */
    whio_size_t whio_epfs_block_encode( whio_epfs_block const * bl, unsigned char * dest );

    /** @internal

        Decodes bl from src, which must be whio_epfs_sizeof_blockMeta
        bytes long, to dest. Returns 0 on success. It is expected that
        src was encoded using whio_epfs_block_encode().
    */
    int whio_epfs_block_decode( whio_epfs_block * bl, unsigned char const * src );

    /** @internal

        Returns true if fs is not null, bl is not 0, and (!fs->fsopt.maxBlocks or
        bl<=fs->fsopt.maxBlocks).
    */
    bool whio_epfs_block_id_in_bounds( whio_epfs const * fs, whio_epfs_id_t bl );

    /** @internal

        Returns true if fs and bl are not 0 and if bl is less than or
        equal to fs->hints.blockCount.
    */
    bool whio_epfs_block_id_exists( whio_epfs const * fs, whio_epfs_id_t bl );
    
    /** @internal

        Returns true if bl is not null and the WHIO_EPFS_FLAG_IS_USED
        bit in bl->flags is set.
    */
    bool whio_epfs_block_is_used( whio_epfs_block const * bl );

    /** @internal

        Sets or unsets the is-used flag for bl. Returns 0 on success,
        but it can only fail if !bl. If !u then bl is zeroed out
        except for its id. This means that any blocks which bl links
        to MUST be set to unused before bl is, or blocks may be
        orphaned.

        Does not update storage but may update fs->hints.freeBlockList
        and bl->nextFree.

        If bl already has the given use-state, this function has no
        side-effects.
    */
    int whio_epfs_block_set_used( whio_epfs * fs, whio_epfs_block * bl, bool u );

    /** @internal

        Returns the on-storage position of the given block. This is
        where the metadata is stored - the client data follows that.

       Returns 0 on error.

       This function does not care whether the given block actually
       exists - only if the ID is in a legal range.
    */
    whio_size_t whio_epfs_block_meta_pos( whio_epfs const * fs, whio_epfs_id_t id );

    /** @internal

        Returns the on-storage position of the client data part of the
        given block.  It is exactly fs->fsopt.blockSize bytes long,
        and overflowing it via a write will corrupt the EFS by bleeding
        into the next data block (or past the last block).

        Returns 0 on error.
    */
    whio_size_t whio_epfs_block_data_pos( whio_epfs const * fs, whio_epfs_id_t id );

    /** @internal

        Seeks fs->dev to the client area of the given block ID. If pos
        is not NULL then the position of the seek is stored in pos.

        On success whio_rc.OK is returned.
    */
    int whio_epfs_block_data_seek( whio_epfs * fs, whio_epfs_id_t id, whio_size_t * pos );

    /** @internal

        Flushes the given block, which must be a properly populated
        object, to storage. This writes only the metadata part of the
        block, not the client data. Returns 0 on success.
    */
    int whio_epfs_block_flush( whio_epfs * fs, whio_epfs_block const * bl );

    /** @internal

       "Formats" an inclusive range of blocks in fs, destroying their
       contents and adding them to the free-block list. It pays NO
       ATTENTION to whether or not those blocks are in use, and thus
       corrupts any in-use inodes which use those blocks. Thus it
       should only be called as part of the mkfs() process to pre-fill
       any blocks.

       If from is 0 then 1 is used. If to is 0 then
       fs->fsopt.maxBlocks is used. If to and fs->fsopt.maxBlocks are
       both 0, whio_rc.RangeError is returned. whio_rc.RangeError is also
       returned if fs->fsopt.maxBlocks is not 0 and to is higher than
       that value.

       from must be 0 or less than to.

       Returns 0 on success.
    */
    int whio_epfs_blocks_format( whio_epfs * fs, whio_epfs_id_t from, whio_epfs_id_t to );

    /** @internal

        Tries to read the given block ID and store it into dest. On
        success, 0 is returned.

       On error:

       - whio_rc.IOError could indicate that either the block could
       not be read (a true i/o error) or that the block does not yet
       exist on storage.
    */
    int whio_epfs_block_read( whio_epfs * fs, whio_epfs_id_t id, whio_epfs_block * dest );

    /** @internal

        Tries to read the block left->nextBlock and populate next with
        its data.  On success 0 is returned and next is populated. If
        left->nextBlock is 0 then success is returned but next->id
        will be 0, an indication to the caller that left has no
        right-side neighbor.

        On error non-0 is returned and next is not modified.

        left and next may point to the same object.

        next may be null, in which case the block is read (if
        left->nextBlock) and the result of the read is returned. This
        can be used to verify that the block can be read while
        discarding the read-in block data.
    */
    int whio_epfs_block_read_next( whio_epfs * fs, whio_epfs_block const * left, whio_epfs_block * next );

    /** @internal

       Tries to find the next free block in fs. On success it returns 0
       and updates dest with the contents of the block.

       If markAsUsed is true, the block is flag as in-use and created
       if necessary. If it marked as in-use, it is flushed to disk.

       If markAsUsed is false and read of any given block fails
       (because the block does not exist) then this function will
       fail. On success, if markAsUsed is false then calling this
       function again may return the same block information.

       fs->fsopt.maxBlocks is respected, but if it is 0 and markAsUsed
       is true then this function may add a new block.

       On error dest is not modified and non-0 is returned.
    */
    int whio_epfs_block_next_free( whio_epfs * fs, whio_epfs_block * dest, bool markAsUsed );
    
    /** @internal

        Ensures that bl->list is at least count items long,
        reallocating if need. As a special case, if count is 0 the
        list is freed. It may allocate more than count items, and
        bl->alloced will reflect the actual number.

       This function does not update bl->count unless count is 0.

       If count is less than or equal to bl->alloced, this function
       has no side effects.
       
       Returns 0 on success.

       Maintenance reminder:
       
       The fs argument is required only so that we can use
       whio_epfs_malloc() and friends.

    */
    int whio_epfs_block_list_reserve( whio_epfs * fs, whio_epfs_block_list *bl, whio_epfs_id_t count );

    /** @internal

        Appends a copy of bl to bli->list, expanding the list as
        necessary. If bli has blocks already (bli->count>0), bl is set
        as a next block in the chain and the previous block in the
        chain is flushed to disk.
       
       bli is assumed to refer to blocks owned by an an opened
       inode. If it is not, results are undefined.

       Immediately after calling this, bli->list[bli->count-1] will refer
       to the copy of bl, but further calls to whio_block_list_reserve()
       or whio_block_list_append() may relocate or destroy that object,
       so a pointer must not be held to is.

       If, after calling this, bli->count==1 then the inode which has
       bli->list[0] as its first block must be flushed to storage if it
       has not already been flushed.
       
       Returns whefs_rc.OK on success.
    */
    int whio_epfs_block_list_append( whio_epfs * fs, whio_epfs_block_list * bli, whio_epfs_block const * bl );

    /** @internal

        Allocates a whio_epfs_handle in the context of the given fs.

       The returned object is owned by the caller and must be freed
       using whio_epfs_handle_free().

       The fs argument is provided in case the fs impl changes such
       that it can provide an alternate memory allocation mechanism.
    */
    whio_epfs_handle * whio_epfs_handle_alloc(whio_epfs * fs);

    /** @internal

        Frees the memory associated with a whio_epfs_handle_alloc()'d
        handle, including any linked blocks.

        Returns 0 on success. The only error condition is if
        h is NULL.
    */
    int whio_epfs_handle_free( whio_epfs_handle * h );


    /** @internal

        Returns true if h is not null and is was opened for writing.
    */
    bool whio_epfs_handle_is_rw( whio_epfs_handle const * h );

    /** @internal

        "Opens" an inode for access via the i/o API.
        
        ino is the ID to open.
    
        mode must be one of:

        - WHIO_MODE_RO opens the inode for read-only access.

        - WHIO_MODE_RW opens the inode for read/write acces.

        WHIO_MODE_RW may optionally be ORed together with:
        
        - WHIO_MODE_FLAG_CREATE works like RW but will mark the node as
        used if it is not used already.

        Both RO and RW modes will fail if the inode is not in-use (has
        not previously been opened), whereas CREATE will mark the inode
        as used if needed.
        
        tgt may not be null, and must point to null.

        After returning, ownership of *tgt is as follows:

        This function allocates the *tgt object and it must eventually
        be closed using whio_epfs_handle_close(). If it is the first
        handle to open the inode, closing it will not actually free it
        until the final link to that handle is closed.
        
        On error non-0 is returned and tgt is not modified.

        FIXME:

        - Allow *tgt to point to user-supplied memory. In theory that
        works now, and whio_epfs_handle_free() is set up to deal with
        that.  However, they may still be other lifetime issues to
        consider and test before making that part of the interface.
    */
    int whio_epfs_handle_open( whio_epfs * fs, whio_epfs_handle **tgt, whio_epfs_id_t ino, int mode );

    /** @internal

        Closes a handle previously opened with whio_epfs_handle_open().
        Whether or not the memory associated with h is immediately
        freed depends on whether other opened handles link to it. If
        they do, it will be freed with the last handle linking to it
        is closed.

        After this call, h should be considered to be an invalid object.

        Returns 0 on success.
     */
    int whio_epfs_handle_close( whio_epfs_handle * h );

    /** @internal

        Returns h->link (recursively) or h if h->link is null or
        points to h. Returns NULL if !h.

        The intention is to transform h into a pointer to the origin
        handle for multi-opened handles.
    */
    whio_epfs_handle * whio_epfs_handle_origin( whio_epfs_handle * h );
    /** @internal

        Const form of whio_epfs_handle_origin().
    */
    whio_epfs_handle const * whio_epfs_handle_origin_c( whio_epfs_handle const * h );

    /** @internal

        Returns a pointer to whio_epfs_handle_origin(h)->inode, or NULL
        if !h.
    */
    whio_epfs_inode * whio_epfs_handle_inode( whio_epfs_handle * h );
    /** @internal

        Const form of whio_epfs_handle_inode().
    */
    whio_epfs_inode const * whio_epfs_handle_inode_c( whio_epfs_handle const * h );

    /** @internal

        Searches li for h. If it finds it it returns it, else it
        returns 0.  If removeIt is true then h is removed from li,
        shifting all right-side elements to the left, and passes
        ownership of h back to the caller.
    */
    whio_epfs_handle *  whio_epfs_handle_search( whio_epfs_handle_list * li, whio_epfs_handle * h, bool removeIt );
    /** @internal

        Searches li for a record with the given id. If it is found,
        its "origin handle" (the first handle opened with that ID) is
        returned, else NULL is returned.
    */
    whio_epfs_handle *  whio_epfs_handle_search_id( whio_epfs_handle_list * li, whio_epfs_id_t id );
    
    /** @internal

        Ensures that li has at least count items available for
        use. Returns 0 on success. li->count is only modified if
        count==0, in which case li->list is freed and li's contents
        are set to an empty state.

        Note that li need not be fs->handles, but memory allocated
        for li may come from fs' allocator.
    */
    int whio_epfs_handle_list_reserve( whio_epfs * fs, whio_epfs_handle_list *li, whio_epfs_id_t count );

    /** @internal

        Appends h to li, growing li if needed. Ownership of h is
        passed to li.
    */
    int whio_epfs_handle_list_append( whio_epfs_handle_list *li, whio_epfs_handle * h );

    /** @internal

        Loads the current block chain for h. If h is linked to another
        handle, the operation is performed on that handle instead.

        This should be called ONE time to load the list of blocks for
        h.  Aftert that, use the whio_epfs_block_list_append() to add
        blocks, and be sure to flush each _new_ block using
        whio_epfs_block_flush(). Use whio_dev::truncate() (via
        whio_epfs_dev_open()) to remove blocks.

        Returns 0 on success.
    */
    int whio_epfs_block_list_load( whio_epfs * fs, whio_epfs_handle * h );

    /** @internal

        This function is the heart of the pseudofile i/o beast...

        It tries to map a logical file position to a data block
        associated with an inode.

        It starts with the inode's first block and increments blocks
        until the block in which pos would land is found. If the inode
        doesn't have enough blocks, the behaviour is defined by the
        expands parameter, as follows:

        If expands is true then it will add blocks to the inode's
        chain in order to reach the destination pos, if necessary. If
        expands is false and pos is not within the inode's current
        data size then the function fails.

        If h is opened read-only then expands is always treated
        as if it were false.

        On success, tgt is populated with the block associated with
        the given position and inode, and the inode *may* be updated
        (if it had no blocks associated with it beforehand). To figure
        out the proper offset of pos to use within the block use
        (pos%fs->fsopt.blockSize).
    
        This  function never actually changes  the logical size of the
        inode even though it may allocate new blocks.

        On success whefs_rc.OK is returned, else some other error
        value. Some possibilities include:

        - whio_rc.RangeError = pos it past EOF and expands is false, or if the fs has a block cap and that cap would be exceeded.
        - whio_rc.ArgError = !fs, !tgt, or h are not valid

        BIG FAT HAIRY WARNING:

        It is intended that the handle argument be an inode which has
        been opened via whio_epfs_handle_open(), but this function does
        not check that because doing so is relatively costly and this
        routine is called from the i/o device implementation for every
        read and write.

        Because the handle argument *may* be updated, it is imperative
        that if it refers to an opened inode, that the handle argument
        actually be a pointer to that object, as opposed to being a
        copy of it. Failure to follow this may result in
        mis-synchronization of the node's state or a memory
        leak. Specifically, if the inode previously had no blocks, and
        this function adds at least one, then the handle must be
        updated.

        If handle refers to an inode which has been opened multiple
        times, only the original handle is actually updated.


        BIGGER, FATTER, HAIRIER WARNING:

        Because profiling has shown that this function spends a
        significant amount of time validating fs and ino (by calling
        whio_epfs_block_id_in_bounds()), that check has been
        removed. We're relying on the caller to pass only legal
        arguments.
    */
    int whio_epfs_block_for_pos( whio_epfs_handle * h, whio_size_t pos, whio_epfs_block * restrict tgt, bool expand );


    /** @internal

        Wipes client bytes of the given block clean, replacing them
        with nulls. If bl->nextBlock then all blocks to the right of
        bl are wiped.

       This does not change the block metadata, only the client data
       area. If startPos is not 0 then the wiping starts at the given
       offset relative to the bl. Right-handle wiped blocks will be
       wiped in their entirety.
    */
    int whio_epfs_block_wipe_data( whio_epfs * fs, whio_epfs_block * bl, whio_size_t startPos );

    /** @internal

       Zeroes out parts of the given data block. bl must be fully
       populated for this to work.

       If data is true then the block's on-disk data bytes are zeroed out.

       If meta is true then the metadata associated with the block (block
       flags and next block ID) are reset to their default state.

       If deep is true then all linked blocks (starting at bl->nextBlock)
       are also cleaned with the same parameters.

       Returns whefs_rc.OK on success.
       
       WARNINGS:

       #1: This pays no regard to whether bl is part of a
       currently-open block chain.
       
       #2: If (meta && !deep), lost blocks will result if bl has child
       blocks! Lost blocks are are marked used but are no longer
       reachable via a block chain, and thus cannot be directly
       recovered without manually fixing the block relationships.

    */
    int whio_epfs_block_wipe( whio_epfs * fs, whio_epfs_block * bl, bool data, bool meta, bool deep );

    /** @internal

        Sets up a whio_dev object which acts as a proxy for i/o on the
        given handle.

        h must refer to a handle opened via whio_epfs_handle_open().

        dest may not be null, but may point to null. If !*dest then
        on success a new whio_dev is allocated and assigned to *dest.
        If *dest then it must be a cleanly initialized (e.g. via
        assignment from whio_dev_empty or whio_dev_empty_m) object.
        
        On success dest is non-null and ownership of h is
        transfered to dest. Ownership of dest is as follows:

        If !*dest, this function allocated the device and it must be
        destroyed using the whio-standard whio_dev::finalize()
        approach, e.g. dev->api->finalize(dev). If *dest (the caller
        allocated it), it must be either finalized as above (if it was
        allocated dynamically) or be closed using dev->api->close(dev)
        (if it was allocated on the stack or via an alternate allocation
        method).

        On error non-0 is returned, but the only error conditions are
       !h and allocation error.

       FIXME: there's a crash condition in the close() operation if the
       returned object is not closed before h->pfs is closed.
    */
    int whio_epfs_dev_create( whio_epfs_handle * h, whio_dev ** dest );

    /** @internal

       Very internal. Only to be used by EFS open/mkfs routines.
    
       This routine does not need fs to be completely populated - only
       fs->dev and fs's read/write flag have to be set. This is so
       that we can check for locking support as early as possible in
       the openfs/mkfs process (and abort a mkfs before hosing an
       opened EFS).

       If the the library not compiled with storage locking support
       (controlled via the WHIO_EPFS_CONFIG_ENABLE_STORAGE_LOCKING
       macro) then this function does nothing and returns
       whio_rc.UnsupportedError. Otherwise...

       If the lock probe fails then non-zero is returned and locking
       in general is disabled in fs. If it is supported, 0 is returned
       and some internals of fs are touched to enable locking support.

       Note that this is only a shallow probe and does not actually
       try to lock anything.

       The results of this probe are recorded as an internal fs
       setting, and calling it more than once will return the result
       of the first probe.
    */
    int whio_epfs_probe_lockability( whio_epfs * fs );
    
    /**  @internal

        NYI. Might never be used.
    */
    struct whio_epfs_record_lock_data
    {
        /**
           Flags record:

           - inode or block?
           - read or write?
           - wait or nowait?
        */
        uint8_t flags;
        /**
           inode or block ID.
        */
        whio_epfs_id_t id;
    };
    /** Convenience typedef. */
    typedef struct whio_epfs_record_lock_data whio_epfs_record_lock_data;
    /** Empty-initialized whio_epfs_record_lock_data object. */
#define whio_epfs_record_lock_data_empty_m {0U/*flags*/,0U/*id*/}
    
    /**
       Tries to apply the given lock request to fs->dev via
       whio_dev_lock(), and returns the result of that request.

       Returns 0 on success. Errors include:

       - req is of type whio_lock_TYPE_WRITE, but fs is opened for read-only
       access: whio_rc.AccessError

       - The library is compiled without WHIO_EPFS_CONFIG_ENABLE_STORAGE_LOCKING,
       or locking has been disabled for fs: whio_rc.UnsupportedError

       - fs, fs->dev, or req are null: whio_rc.ArgError
    */
    int whio_epfs_storage_lock( whio_epfs * fs, whio_lock_request * req );

    /** @internal

        If the libwhio was not compiled with locking support
        (controlled via the WHIO_EPFS_CONFIG_ENABLE_STORAGE_LOCKING
        macro) then this function does nothing and returns
        whio_rc.UnsupportedError. Otherwise...

        Tries to lock a range of bytes within fs->dev.

        If isWriteLock a write lock is requested, else a read lock is
        requested. If wait is true then locking will wait until a lock
        is acquired, else it will fail (with whio_rc.LockingError) if
        another process is prohibiting the lock.

        start and length are coordinates relative to the start of
        fs->dev.

        Returns 0 on success, or:

        - whio_rc.UnsupportedError if fs does not appear capable of
        supporting locking.

        - whio_rc.LockingError if this routine can explcitely
        determine that a non-waiting lock was blocked by another
        process. It's not clear how reliable that determination is.

        - Some other error code from deep inside the whio_dev_ioctl()
        bits.


        Possible TODOs:

        - Store each lock as an object internally in fs, and return a
        pointer to that object (or an ID referencing it). The
        pointer/ID could then be passed to the unlock routine to
        simplify the unlocking process. Since each inode/block has to
        be locked separately, that would end up eating memory, though.

        @see whio_epfs_storage_lock()
        @see whio_epfs_storage_unlock2()
    */
    int whio_epfs_storage_lock2( whio_epfs * fs,
                                 bool isWriteLock,
                                 whio_size_t start,
                                 whio_size_t length,
                                 bool wait );
    /** @internal

       This function is the converse of whio_epfs_storage_lock2(), and undoes
       a lock previously placed by that routine.

       Returns 0 on success.
       
       Reminder to self: it's not really clear whether read/write
       modes need to match the original lock when unlocking? i assume
       so? It's also not clear whether we will ever need to wait (as
       in F_SETLKW) on an unlock, or whether F_SETLK (non-waiting)
       suffices for that.

       @see whio_epfs_storage_lock2()
       @see whio_epfs_storage_unlock2()
    */
    int whio_epfs_storage_unlock2( whio_epfs * fs,
                                  bool isWriteLock,
                                  whio_size_t start,
                                  whio_size_t length,
                                  bool wait );

    /**
       whio_epfs_probe_lockability() must have been called
       before this function.
    
       This tries to place a lock on the whole fs storage device.
       If wait is true it will wait to obtain a lock, else it
       will fail if a contending lock would block.

       It is a read/write lock if fs is r/w, else it is a read lock.
    */
    int whio_epfs_storage_lock_all( whio_epfs * fs, bool wait );

    /**
       The converse of whio_epfs_storage_unlock_all(), but
       always does a non-waiting unlock (as we do not expect
       _unlocking_ to fail except over network filesystems???).
    */
    int whio_epfs_storage_unlock_all( whio_epfs * fs );
    
    /** @internal

        UNTESTED!

        Tries to apply a read or write (specified by writeLock) lock
        to the inode record with the given ID. If wait is false it will
        fail if it cannot lock immediately, otherwise it will wait until
        it can get a lock.

        Returns 0 on success.
    */
    int whio_epfs_inode_lock( whio_epfs * fs, whio_epfs_id_t id, bool writeLock, bool wait );

    /** @internal

        UNTESTED!

        The converse of whio_epfs_inode_lock(), this tries to unlock
        the given record.

        Returns 0 on success.
    */
    int whio_epfs_inode_unlock( whio_epfs * fs, whio_epfs_id_t id, bool writeLock, bool wait );

    /** @internal

        Tries to apply a read or write (specified by writeLock) lock
        to the block record with the given ID. If wait is false it
        will fail if it cannot lock immediately, otherwise it will
        wait until it can get a lock.

        Returns 0 on success.
    */
    int whio_epfs_block_lock( whio_epfs * fs, whio_epfs_id_t id, bool writeLock, bool wait );

    /** @internal

        UNTESTED!

        The converse of whio_epfs_block_lock(), this tries to unlock
        the given record.

        Returns 0 on success.
    */
    int whio_epfs_block_unlock( whio_epfs * fs, whio_epfs_id_t id, bool writeLock, bool wait );

    /** @internal

        UNTESTED!

        Tries to apply a read or write (specified by writeLock) lock
        to fs' magic bytes. If wait is false it will fail if it cannot
        lock immediately, otherwise it will wait until it can get a
        lock.

        Returns 0 on success.

        @see whio_epfs_storage_lock()
        @see whio_epfs_magic_unlock()
    */
    int whio_epfs_magic_lock( whio_epfs * fs, bool writeLock, bool wait );

    /** @internal

        Tries to unlock a lock set by whio_epfs_magic_lock().

        Returns 0 on success.

        @see whio_epfs_magic_lock()
        @see whio_epfs_storage_lock()
    */
    int whio_epfs_magic_unlock( whio_epfs * fs, bool writeLock, bool wait );
    
    /** @internal

       If whio_epfs_setup_mempool() has been called to set up a memory
       pool for fs, this function tries to allocate n bytes from
       that pool. Otherwise it allocates using malloc().
    */
    void * whio_epfs_malloc( whio_epfs * fs, whio_size_t n );

    /** @internal

       If whio_epfs_setup_mempool() has been called to set up a memory
       pool for fs, this function tries to reallocate m with a size of
       n bytes from that pool. Otherwise it reallocates using realloc().
    */
    void * whio_epfs_mrealloc( whio_epfs * fs, void * mem, whio_size_t n );

    /** @internal

       If whio_epfs_setup_mempool() has been called to set up a memory
       pool for fs, this function tries to free m using that pool,
       otherwise free() is used.

       Returns 0 on success. The only error conditions are:

       A) !fs

       B) If the underlying custom allocation mechanism is fouled
       up. In that case, the error code may come from a different
       library, and the error code may not be one of the whio_rc
       values (even though it may correspond to one of them).

       If this function EVER returns non-0 when given valid arguments
       then there is likely a bug somewhere in the internally-used
       allocator engine.
    */
    int whio_epfs_mfree( whio_epfs * fs, void * m );

    /** @internal

    Searches the opened handle list for the given inode ID, or loads the inode
    from storage if it is not opened.

    On success it returns 0 and updates the inode's client flags to
    the given flags parameter. The inode is then flushed. Any opened handles
    to the inode will use the modified flags.

    Error conditions include:

    - !fs
    - fs is opened in read-only mode.
    - id is out of range.
    - i/o error while reading or flushing the inode.

    ACHTUNG: this has no r/w access checks on the inode, but fs must be
    r/w.
    */
    int whio_epfs_inode_id_client_flags_set( whio_epfs * fs, whio_epfs_id_t id,
                                             uint32_t flags );

    
    /** @internal

    Tries to read the client flags for the given inode id. The list of
    opened inodes is searched first, otherwise the inode is read from
    storage.

    On success 0 is returned and the flags parameter (if it is not NULL)
    is set to the flags of the inode.

    Error conditions include:

    - !fs
    - id is out of range.
    - i/o error while reading the inode.

    If non-0 is returned then the flags parameter is not modified.
    */
    int whio_epfs_inode_id_client_flags_get( whio_epfs * fs, whio_epfs_id_t id,
                                             uint32_t * flags );

    /** @internal
       Sets the client flags field of ino. Returns 0 on success or whio_rc.ArgError
       if !ino.
    */
    int whio_epfs_inode_client_flags_set( whio_epfs_inode * ino, uint32_t flags );

    /** @internal

       Gets the client flags field of ino and sets them in the flags
       parameter (if it is not null). Returns 0 on success or
       whio_rc.ArgError if !ino.
    */
    int whio_epfs_inode_client_flags_get( whio_epfs_inode const * ino, uint32_t * flags );

    /** @internal

       This function won't work any more since some internal refactoring of the
       allocation API on 20100226.
    
       If the library is compiled with memory pool support, this will dump debug
       information about the memory pool to the given FILE. The prefix argument can be
       used to add a label to the top of the output. If full is true then a full dump
       of the pool is done, rather than just statistical information.
    */
    int whio_epfs_mempool_dump( whio_epfs * fs, FILE * dest, char const * prefix, bool full );

    /** @internal

        HIGHLY internal, this function only exists to remove an unsightly
        dependency on the allocator engine in one of the source files.

        Empties the memory pool state. It does NO cleanup of any objects
        in the pool.

        Returns 0 if fs and its memory pool has been set up, else
        whio_arc.ArgError.
    */
    int whio_epfs_mempool_drain( whio_epfs * fs );
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WANDERINGHORSE_NET_WHIO_EPFS_INTERNAL_H_INCLUDED */
/* end file pfs/whio_epfs_internal.h */
/* begin file pfs/block.c */
#line 8 "pfs/block.c"
/************************************************************************
FAR FROM COMPLETE.

This is a work-in-progress, porting over parts of the whefs API into
the whio API...

Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

License: Public Domain
************************************************************************/
#if 1
#define MARKER(X)
#else
#define MARKER(X) printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); printf X
#define WHIO_DEBUG_ENABLED 1
#endif

#include <assert.h>
#include <string.h> // memset() and friends



const whio_epfs_block whio_epfs_block_empty = whio_epfs_block_empty_m;
const whio_epfs_block_list whio_epfs_block_list_empty = whio_epfs_block_list_empty_m;

int whio_epfs_block_list_reserve( whio_epfs * fs, whio_epfs_block_list *bl, whio_epfs_id_t n )
{
    if( ! bl ) return whio_rc.ArgError;
    else if( 0 == n )
    {
        //MARKER(("Freeing block list of %"WHIO_EPFS_ID_T_PFMT" items @%p\n",n, (void const *)bl->list);
        //free( bl->list );
        whio_epfs_mfree( fs, bl->list );
        //void * x; x = realloc( bl->list, 0 );
        *bl = whio_epfs_block_list_empty;
        return whio_rc.OK;
    }
    else if( bl->alloced >= n ) return whio_rc.OK;
    whio_epfs_block * li = (whio_epfs_block *)
        //realloc( bl->list, n * sizeof(whio_epfs_block) );
        whio_epfs_mrealloc( fs,  bl->list, n * sizeof(whio_epfs_block) );
    //MARKER(("(Re)allocated block list of %"WHIO_EPFS_ID_T_PFMT" items @%p\n", n, (void const *)li));
    if( ! li )
    {
        return whio_rc.AllocError;
    }
    bl->alloced = n;
    bl->list = li;
    whio_epfs_id_t i = bl->count;
    for( ; i < n; ++i )
    {
        li[i] = whio_epfs_block_empty;
    }
    return whio_rc.OK;
}

int whio_epfs_block_list_append( whio_epfs * fs, whio_epfs_block_list * bli, whio_epfs_block const * bl )
{
    if( ! fs || !bli || !bl ) return whio_rc.ArgError;
    int rc = 0;
    /**
       allocIncrement is the number of objects to allocate at a
       time. Its value is _almost_ arbitrarily chosen. It _should_,
       for efficiency, be evently dividable into the memory pool block
       size (if using a pool), but i don't want the pool abstraction
       to leak into here.

       i chose not to use a multiplier (typical for expanding allocators)
       because it can end up wasting a lot of memory in our memory
       pool for EFSs with very small block sizes. Instead we suffer a few
       more re-allocations for those cases.

       BUG on 64-bit: if this number is "too low" (e.g. 6) i'm getting
       (at cleanup time) an assertion in whalloc_bt_free() at some
       point because of an invalid address passed into the allocator
       (and i have no idea where it's coming from - everything else
       seems to work). However, if i increase allocIncrement to 10, this
       problem goes away. This needs more testing when i have access
       to a 64-bit machine with a graphical debugger.
    */
    static whio_epfs_id_t allocIncrement = 6;
    if( bli->alloced <= bli->count )
    {
        rc = whio_epfs_block_list_reserve( fs, bli, bli->count + allocIncrement );
        if( whio_rc.OK != rc ) return rc;
    }
    bli->list[bli->count] = *bl;
    if( 0 < bli->count )
    { // append block to the list
        whio_epfs_block * prev = &bli->list[bli->count-1];
        if( ! prev->nextBlock )
        {
            prev->nextBlock = bl->id;
            rc = whio_epfs_block_flush( fs, prev );
            if(rc)
            {
                WHIO_DEBUG("Error: could not flush block #%"WHIO_EPFS_ID_T_PFMT" after appending to whio_epfs_block_list!\n",
                           prev->id );
                return rc;
            }
        }
        else if( prev->nextBlock != bl->id )
        {
            WHIO_DEBUG("Internal error: previous block (#%"WHIO_EPFS_ID_T_PFMT") says its next block is #%"WHIO_EPFS_ID_T_PFMT
                       ", but request was made to append #%"WHIO_EPFS_ID_T_PFMT"!\n",
                       prev->id, prev->nextBlock, bl->id );
            assert( 0 && "Internal consistency error." );
            return whio_rc.InternalError;
        }
    }
    else
    { // set bl as the first block...
        //WHIO_DEBUG("inode attached to bli needs to be updated now!\n");
    }
    ++bli->count;
    //WHEFS_DBG("Appended block #%"WHIO_EPFS_ID_T_PFMT" to chain (of %"WHIO_EPFS_ID_T_PFMT" item(s)) for inode #%"WHIO_EPFS_ID_T_PFMT"[%s].", bl->id, ino->blocks.count, ino->id, ino->name );
    return whio_rc.OK;
}


bool whio_epfs_block_is_used( whio_epfs_block const * bl )
{
    return bl && (bl->flags & WHIO_EPFS_FLAG_IS_USED);
}

int whio_epfs_block_set_used( whio_epfs * fs, whio_epfs_block * bl, bool u )
{
    if( ! bl ) return whio_rc.ArgError;
    else
    {
        if(u)
        {
            if( !(bl->flags & WHIO_EPFS_FLAG_IS_USED) )
            {
                bl->flags |= WHIO_EPFS_FLAG_IS_USED;
                if( fs->hints.freeBlockList == bl->id )
                {
                    fs->hints.freeBlockList = bl->nextFree;
                    bl->nextFree = 0;
                }
            }
        }
        else
        {
            if( bl->flags & WHIO_EPFS_FLAG_IS_USED )
            {
                whio_epfs_id_t id = bl->id;
                if( bl->nextBlock )
                {
                    WHIO_DEBUG("WARNING: un-using block %"WHIO_EPFS_ID_T_PFMT
                               " while it still links to %"WHIO_EPFS_ID_T_PFMT"!\n",
                               id, bl->nextBlock);
                }
                *bl = whio_epfs_block_empty;
                bl->id = id;
                bl->nextFree = fs->hints.freeBlockList;
                fs->hints.freeBlockList = bl->id;
            }
        }
        return whio_rc.OK;
    }
}

static const unsigned char whio_epfs_block_tag_char = 'B';
whio_size_t whio_epfs_block_encode( whio_epfs_block const * bl, unsigned char * dest )
{
    if( ! bl || ! dest ) return 0;
    *(dest++) = whio_epfs_block_tag_char;
    dest += whio_epfs_encode_id( dest, bl->id );
    dest += whio_epfs_encode_id( dest, bl->nextBlock );
    dest += whio_epfs_encode_id( dest, bl->nextFree );
    dest += whio_encode_uint8( dest, bl->flags );
    return whio_epfs_sizeof_blockMeta;
}
int whio_epfs_block_decode( whio_epfs_block * bl, unsigned char const * src )
{
    unsigned char const * at = src;
    if( *(at++) != whio_epfs_block_tag_char )
    {
        return whio_rc.ConsistencyError;
    }
    int rc = whio_epfs_decode_id( at, &bl->id );
    if( rc ) return rc;
    at += whio_epfs_sizeof_id;

    rc = whio_epfs_decode_id( at, &bl->nextBlock );
    if( rc ) return rc;
    at += whio_epfs_sizeof_id;

    rc = whio_epfs_decode_id( at, &bl->nextFree );
    if( rc ) return rc;
    at += whio_epfs_sizeof_id;

    rc = whio_decode_uint8( at, &bl->flags );
    return rc;
}

bool whio_epfs_block_id_in_bounds( whio_epfs const * fs,
                                   whio_epfs_id_t bl )
{
    return (!fs || !bl)
        ? false
        : (( !fs->fsopt.maxBlocks )
           ||
           ( bl <= fs->fsopt.maxBlocks ))
        ;
}

bool whio_epfs_block_id_exists( whio_epfs const * fs, whio_epfs_id_t bl )
{
    return ( ! fs || !bl )
        ? false
        : (bl <= fs->hints.blockCount);
}

whio_size_t whio_epfs_block_meta_pos( whio_epfs const * fs, whio_epfs_id_t id )
{
    return ( !whio_epfs_block_id_in_bounds( fs, id ) )
        ? 0
        : fs->offsets[whio_epfs_index_blockMeta] +
        ((id-1) * (fs->sizes[whio_epfs_index_blockMeta]
                   + fs->fsopt.blockSize))
        ;
}
whio_size_t whio_epfs_block_data_pos( whio_epfs const * fs, whio_epfs_id_t id )
{
    whio_size_t x = whio_epfs_block_meta_pos( fs, id );
    return x
        ? (x + fs->sizes[whio_epfs_index_blockMeta])
        : 0;
}
int whio_epfs_block_data_seek( whio_epfs * fs, whio_epfs_id_t id, whio_size_t * pos )
{
    whio_size_t x = whio_epfs_block_meta_pos( fs, id );
    if( ! x ) return whio_rc.RangeError;
    x += fs->sizes[whio_epfs_index_blockMeta];
    if( pos ) *pos = x;
    return (x == whio_epfs_seek( fs, x ))
        ? 0
        : whio_rc.IOError;
}
#if 0
//static
whio_epfs_block * whio_epfs_block_alloc()
{
    whio_epfs_block * x = (whio_epfs_block*)malloc(sizeof(whio_epfs_block));
    if( x ) *x = whio_epfs_block_empty;
    return x;
}
//static
void whio_epfs_block_free(whio_epfs_block * h)
{
    if( h )
    {
        *h = whio_epfs_block_empty;
        free(h);
    }
}
#endif
                             
int whio_epfs_block_flush( whio_epfs * fs,
                          whio_epfs_block const * bl
                          )
{
    if( ! fs || !bl ) return whio_rc.ArgError;
    if( ! whio_epfs_is_rw(fs) ) return whio_rc.AccessError;
    whio_size_t pos = whio_epfs_block_meta_pos( fs, bl->id );
    if( ! pos ) return whio_rc.RangeError;
    enum { Len = whio_epfs_sizeof_blockMeta };
    unsigned char buf[Len];
    whio_epfs_block_encode( bl, buf );
    whio_size_t wrc = whio_epfs_writeat( fs, pos, buf, Len );
    if( Len != wrc )
    {
        return whio_rc.IOError;
    }
    else
    {
        uint8_t changes = 0;
        if( fs->hints.blockCount < bl->id )
        {
            fs->hints.blockCount = bl->id;
            ++changes;
        }
        if( whio_epfs_block_is_used( bl ) )
        {
            if( fs->hints.maxEverUsedBlock < bl->id )
            {
                fs->hints.maxEverUsedBlock = bl->id;
                ++changes;
            }
        }
#if WHIO_EPFS_CONFIG_AUTO_FLUSH_HINTS
        if( changes )
        {
            int rc = whio_epfs_hints_write( fs );
            if( rc ) return rc;
        }
#endif
        return whio_rc.OK;
    }
}

int whio_epfs_block_wipe_data( whio_epfs * fs, whio_epfs_block * bl, whio_size_t startPos )
{
    if( ! fs || ! bl ) return whio_rc.ArgError;
    int rc;
    {
        if( bl->nextBlock )
        {
            whio_epfs_block nbl = whio_epfs_block_empty;
            rc = whio_epfs_block_read( fs, bl->nextBlock, &nbl );
            if( rc ) return rc;
            rc = whio_epfs_block_wipe_data( fs, &nbl, 0 );
            if( rc ) return rc;
        }
    }
    //rc = whio_epfs_block_flush( fs, bl );
    //if( rc ) return rc;
    whio_size_t fpos = 0;
    const whio_size_t bs = fs->fsopt.blockSize;
    if( startPos >= bs ) return whio_rc.RangeError;
    rc = whio_epfs_block_data_seek( fs, bl->id, &fpos );
    if( rc ) return rc;
    if( (fpos + bs) < fpos /* overflow! */ ) return whio_rc.RangeError;
    fs->dev->api->seek( fs->dev, startPos, SEEK_CUR );
    const whio_size_t count = bs - startPos;
    {
        enum { bufSize = 1024 * 4 };
        static unsigned const char fill = 0;/* should be 0 except for testing purposes.*/
        static unsigned char buf[bufSize] = {1,0};
        if( buf[0] && (buf[0]!=fill) )
        {
            memset( buf+1, fill, bufSize-1 );
            buf[0] = fill;
        }
        whio_size_t wrc = 0;
        whio_size_t total = 0;
        while( total < count )
        {
            const whio_size_t x = count - total;
            const whio_size_t wsz = (bufSize > x) ? x : bufSize;
            wrc = whio_epfs_write( fs, buf, wsz);
            if( ! wrc ) break;
            if( wsz != wrc ) return whio_rc.IOError;
            total += wrc;
        }
        //MARKER(("Wrote %"WHIO_SIZE_T_PFMT" bytes to zero block #%"WHIO_EPFS_ID_T_PFMT". Range=[%"WHIO_SIZE_T_PFMT" .. %"WHIO_SIZE_T_PFMT")", total, bl->id, fpos, fpos+count ));
    }
    return whio_rc.OK;

}

int whio_epfs_block_wipe( whio_epfs * fs, whio_epfs_block * bl,
		      bool wipeData,
		      bool wipeMeta,
		      bool deep )
{
    if( ! whio_epfs_block_id_in_bounds( fs, bl ? bl->id : 0 ) ) return whio_rc.ArgError;
    size_t fpos = 0;
    const size_t bs = fs->fsopt.blockSize;
    if( (fpos + bs) < fpos /* overflow! */ ) return whio_rc.RangeError;
    int rc = 0;
    if( deep && bl->nextBlock )
    {
	whio_epfs_block next = *bl;
	whio_epfs_block xb = *bl;
	bl->nextBlock = 0;
	while( xb.nextBlock )
	{
	    if( whio_rc.OK != (rc = whio_epfs_block_read_next( fs, &xb, &next )) )
	    {
		return rc;
	    }
	    xb = next;
	    next.nextBlock = 0; /* avoid that the next call recurses deeply while still honoring 'deep'. */
	    if( whio_rc.OK != (rc = whio_epfs_block_wipe( fs, &next, wipeData, wipeMeta, deep )) )
	    {
		return rc;
	    }
	}
    }
    if( wipeMeta )
    {
	if( ! deep && bl->nextBlock )
	{
	    //WHIO_DEBUG("Warning: we're cleaning up the metadata without cleaning up children! We're losing blocks!\n");
	}
	const whio_epfs_id_t oid = bl->id;
	*bl = whio_epfs_block_empty;
	bl->id = oid;
        bl->nextFree = fs->hints.freeBlockList;
        fs->hints.freeBlockList = bl->id;
        if(1)
        {
            WHIO_DEBUG("Wiping block #%"WHIO_EPFS_ID_T_PFMT
                       ". freeBlockList=#%"WHIO_EPFS_ID_T_PFMT
                       "->#%"WHIO_EPFS_ID_T_PFMT"\n",
                       bl->id, fs->hints.freeBlockList, bl->nextFree);
        }
	rc = whio_epfs_block_flush( fs, bl );
        if(!rc)
        { /* FIXME? move this somewhere else? */
            rc = whio_epfs_hints_write( fs );
        }
        if( whio_rc.OK != rc )
	{
	    //WHIO_DEBUG("Wiping block #%"WHEFS_ID_TYPE_PFMT" failed: flush failed with error code #%d!\n", bl->id, rc);
	    return rc;
	}
    }
    if( wipeData )
    {
	rc = whio_epfs_block_wipe_data( fs, bl, 0 );
	if( whio_rc.OK != rc )
	{
	    //WHIO_DEBUG("Wiping block #%"WHIO_EPFS_ID_T_PFMT" failed with error code #%d!\n", bl->id, rc);
	    return rc;
	}
    }
    return whio_rc.OK;
}



int whio_epfs_block_read( whio_epfs * fs,
                         whio_epfs_id_t id,
                         whio_epfs_block * dest )
{
    if( ! fs || !dest ) return whio_rc.ArgError;
    if( ! whio_epfs_block_id_in_bounds( fs, id ) )
    {
        return whio_rc.RangeError;
    }
    enum { Len = whio_epfs_sizeof_blockMeta };
    unsigned char buf[Len];
    whio_size_t pos = whio_epfs_block_meta_pos( fs, id );
    if( ! pos ) return whio_rc.InternalError;
    whio_size_t sz = whio_epfs_readat( fs, pos, buf, Len );
    if( sz != Len )
    {
        return whio_rc.IOError;
    }
    whio_epfs_block bl = whio_epfs_block_empty;
    int rc = whio_epfs_block_decode( &bl, buf );
    if( rc ) return fs->err = rc;
    { /* check for fs hint update. */
        int8_t changes = 0;
        if( fs->hints.blockCount < bl.id )
        {
            fs->hints.blockCount = bl.id;
            ++changes;
        }
        if( whio_epfs_block_is_used( &bl ) )
        {
            if( fs->hints.maxEverUsedBlock < bl.id )
            {
                fs->hints.maxEverUsedBlock = bl.id;
                ++changes;
            }
        }
#if WHIO_EPFS_CONFIG_AUTO_FLUSH_HINTS
        if( changes && whio_epfs_is_rw(fs) )
        {
            rc = whio_epfs_hints_write( fs );
            if( rc ) return rc;
        }
#endif
    }
    *dest = bl;
    return whio_rc.OK;
}

int whio_epfs_block_next_free( whio_epfs * fs, whio_epfs_block * dest, bool markAsUsed )
{
    if( ! fs || ! dest ) return whio_rc.ArgError;
    if( markAsUsed && ! whio_epfs_is_rw(fs) ) return whio_rc.AccessError;
    int rc = whio_rc.InternalError;
#if 1
    // FIXME: refactor to use helper funcs for modifying the free list,
    // as is done in the inode class.
    whio_epfs_block bl = whio_epfs_block_empty;
    whio_epfs_id_t id = fs->hints.freeBlockList;
    if( ! id )
    { /* assume we are full and try to add blocks */
        if( ! markAsUsed ) return whio_rc.DeviceFullError;
        do
        {
            id = fs->hints.maxEverUsedBlock+1;
            rc = whio_epfs_block_read( fs, id, &bl );
            if( 0 == rc )
            { /* block exists? */
                if( whio_epfs_block_is_used(&bl) )
                {
                    assert(0 && "next-free-inode got a used inode!");
                }
                return whio_rc.InternalError;
            }
            else
            {
                /** Try to create the block... */
                bl = whio_epfs_block_empty;
                bl.id = id;
#if 0
                rc = whio_epfs_block_set_used( fs, &bl, markAsUsed )
                    /* might update fs->hints.freeBlockList */
                    ;
                if( rc ) return rc;
#endif
                rc = whio_epfs_block_flush( fs, &bl );
                if( rc ) return rc;
                rc = whio_epfs_block_wipe_data( fs, &bl, 0 );
                if( rc ) return rc;
            }
            break;
        } while(true);
    }
    else
    {
        rc = whio_epfs_block_read( fs, id, &bl );
        if( rc ) return rc;
    }
    if( !rc )
    {
        if( markAsUsed )
        {
            whio_epfs_block_set_used( fs, &bl, true );
            rc = whio_epfs_block_flush( fs, &bl );
            if(!rc) rc = whio_epfs_hints_write( fs );
        }
        if( ! rc )
        {
            *dest = bl;
        }
    }
    return rc;
#else /* older, mostly linear impl */
    whio_epfs_id_t id = fs->hints.nextFreeBlock;
    if( ! (id+1) )
    {
        WHIO_DEBUG("Search for next free inode would overflow counter!\n");
        return whio_rc.RangeError;
    }
    else if( fs->fsopt.maxBlocks && (id > fs->fsopt.maxBlocks) )
    {
        /**
           Re-set the counter to try to see if we can find any
           orphaned blocks in here...
         */
        id = fs->hints.nextFreeBlock = 1;
    }
    else if( ! id )
    {
        /* ID 0 is reserved for not-a-block. */
        id = fs->hints.nextFreeBlock = 1;
    }
    if( fs->fsopt.maxBlocks && (id > fs->fsopt.maxBlocks) )
    {
        return whio_rc.DeviceFullError;
    }
    //whio_epfs_id_t const origin = id;
    rc = whio_rc.RangeError;
    whio_epfs_block bl = whio_epfs_block_empty;
    for( ; ; ++id )
    {
        if( (fs->fsopt.maxBlocks && (id >fs->fsopt.maxBlocks))
            || ((whio_epfs_id_t)-1 == id))
        {
            return whio_rc.DeviceFullError;
        }
        rc = whio_epfs_block_read( fs, id, &bl );
        if( !rc && whio_epfs_block_is_used( &bl ) ) continue;
        else if( rc )
        { /* See if we can/need to create one...*/
            if( ! markAsUsed )
            {
                /** Read-only requests fail when the block in not there. */
                return rc;
            }
            if( whio_epfs_block_id_exists( fs, id ) )
            { /* Block is known but read really failed. */
                return rc;
            }
            bl = whio_epfs_block_empty;
            bl.id = id;
        }
        if( markAsUsed )
        {
            whio_epfs_block_set_used( fs, &bl, true );
            rc = whio_epfs_block_flush( fs, &bl );
            if( rc ) return rc;
            /** We wipe the data area for two reasons:

            1) to ensure that recycled blocks behave like
            libc routines when expanding into unwritten space.

            2) to ensure that the data bytes for new blocks actually
            exist, avoiding special-case handling and potentially
            bogus read errors later on.

            */
            rc = whio_epfs_block_wipe_data( fs, &bl, 0 );
            if( rc )
            {
                /**
                   How to gracefully recover from this case?
                */
                return fs->err = rc;
            }
#if 0 // WHIO_EPFS_CONFIG_AUTO_FLUSH_HINTS
            // this is arguably  better done in whio_epfs_block_read/flush()
            int8_t changes = 0;
            if( id >= origin )
            {
                fs->hints.nextFreeBlock = id + 1;
                ++changes;
            }
            if( id > fs->hints.maxEverUsedBlock )
            {
                fs->hints.maxEverUsedBlock = id;
                ++changes;
            }
            if( changes )
            {
                rc = whio_epfs_hints_write( fs );
                if( rc ) return rc;
            }
#endif
        }
        *dest = bl;
        return whio_rc.OK;
    }
#endif
    return rc;
}

int whio_epfs_block_read_next( whio_epfs * fs,
                              whio_epfs_block const * left,
                              whio_epfs_block * next )
{
    if( ! fs || ! left ) return whio_rc.ArgError;
    whio_epfs_block bl = whio_epfs_block_empty;
    if( ! left->nextBlock )
    {
        if( next ) *next = bl;
        return whio_rc.OK;
    }
    int rc = whio_epfs_block_read(fs,left->nextBlock,&bl);
    if( !rc && next ) *next = bl;
    return rc;
}

int whio_epfs_block_list_load( whio_epfs * fs, whio_epfs_handle * h_ )
{
    if( ! fs || ! h_ ) return whio_rc.ArgError;
    whio_epfs_handle * h = whio_epfs_handle_origin(h_);
    whio_epfs_inode * ino = &h->inode;
    if( ! ino->firstBlock )
    {
        return whio_rc.OK;
    }
    if( (h_ == h) && h->blocks.count )
    {
        MARKER(("WARNING: this function shouldn't be called when handle->blocks.count is !0. inode=#%"WHIO_EPFS_ID_T_PFMT".",
                ino->id));
        return whio_rc.OK;
    }
    else if( (h_ != h) && h->blocks.count )
    {
        /* assume the list was loaded by the original handle. */
        return whio_rc.OK;
    }
    whio_epfs_block bl = whio_epfs_block_empty;
    whio_epfs_block_list * bli = &h->blocks;
    int rc = whio_epfs_block_read( fs, ino->firstBlock, &bl );
    if( whio_rc.OK != rc ) return rc;

#if 0
    if( ! bli->list )
    {
        rc = whio_epfs_block_list_reserve( fs, bli, 5 /* arbitrarily chosen */ );
        if( whio_rc.OK != rc ) return rc;
    }
#endif
    
    rc = whio_epfs_block_list_append( fs, bli, &bl );
    if( whio_rc.OK != rc ) return rc;
    if( ! ino->firstBlock )
    {
        ino->firstBlock = bl.id;
        whio_epfs_inode_touch( ino, -1 );
        rc = whio_epfs_inode_flush( fs, ino );
        if( whio_rc.OK != rc ) return rc;
    }
    while( bl.nextBlock )
    {
        rc = whio_epfs_block_read_next( fs, &bl, &bl );
        if( whio_rc.OK != rc ) return rc;
        rc = whio_epfs_block_list_append( fs, bli, &bl );
        if( whio_rc.OK != rc ) return rc;
    }
#if 0
    WHIO_DEBUG("Loaded block chain of %"WHIO_EPFS_ID_T_PFMT" block(s) for inode #%"WHIO_EPFS_ID_T_PFMT".\n",
               bli->count, ino->id );
#endif
    return whio_rc.OK;
}

int whio_epfs_block_for_pos( whio_epfs_handle * h_,
                            whio_size_t pos, whio_epfs_block * restrict tgt, bool expand )
{
    /**
       TODO: consider refactoring to take a (whio_epfs_block ** tgt)
       instead, and copying the pointer directly. That would be a
       couple cycles faster and i can't think of a reason that it
       would cause Grief.
    */
    if( expand && ! whio_epfs_handle_is_rw(h_) ) expand = false;
    whio_epfs_handle * h = whio_epfs_handle_origin(h_);
    whio_epfs * fs = h->fs;
    whio_epfs_block_list * bli = &h->blocks;
    whio_epfs_inode * ino = &h->inode;
    if( (ino->size <= pos) )
    {
        if( !expand )
        {
            return whio_rc.RangeError;
        }
    }
    const whio_size_t bs = fs->fsopt.blockSize;
    const whio_size_t bc = /* how many blocks will we need? */
        1 + (pos/bs);
    if( fs->fsopt.maxBlocks && (bc>fs->fsopt.maxBlocks) )
    {
        return whio_rc.RangeError;
    }
    int rc = whio_rc.OK;
    if( ! bli->list )
    {
        rc = whio_epfs_block_list_load( fs, h );
        if( rc ) return rc;
    }
    if( !expand && (bli->count < bc) )
    { /* cannot grow for this request. */
        return whio_rc.RangeError;
    }
    whio_epfs_block bl = whio_epfs_block_empty;
    whio_epfs_block * blP = 0;
    if( bc <= bli->count )
    {
        blP = &bli->list[bc-1]; /* jump right to it. */
    }
    else
    { /* expand the list */
        if( ! expand )
        {
            return whio_rc.AccessError;
        }
        whio_size_t i = bli->count;
        blP = &bl;
        for( ; i < bc; ++i )
        {
            // FIXME: reserve enough space in bli before this loop,
            // to avoid potentially more mallocs.
            rc = whio_epfs_block_next_free( fs, &bl, true );
            if( ! rc ) rc = whio_epfs_block_list_append( fs, bli, &bl );
            if( !rc && !ino->firstBlock )
            {
                ino->firstBlock = bl.id;
                whio_epfs_inode_touch( ino, -1 );
                rc = whio_epfs_inode_flush( fs, ino );
                if( whio_rc.OK != rc ) return rc;
            }
            if( rc )
            {
                return rc;
                /**
                   We "might" want to truncate the inode back to its
                   previous length here, but why add room for yet another
                   error on top of the one we just encountered?
                */
            }
        }
    }
    if( whio_rc.OK == rc )
    {
        *tgt = *blP;
    }
    return rc;
}

int whio_epfs_blocks_format( whio_epfs * fs, whio_epfs_id_t from, whio_epfs_id_t to )
{
    if( ! fs || (from<to) ) return whio_rc.ArgError;
    else
    {
        whio_epfs_id_t i = 0;
        if( ! from ) from = 1;
        if( ! to )
        {
            to = fs->fsopt.maxBlocks;
            if( ! to ) return whio_rc.RangeError;
        }
        if( fs->fsopt.maxBlocks && (i > fs->fsopt.maxBlocks) )
        {
            return whio_rc.RangeError;
        }
        if( from < to )
        {
            /**
               Reverse free-list link order so that we get
               allocated low-ID's blocks first.
            */
            i = from;
            from = to;
            to = i;
        }
        i = from;
        whio_epfs_block bl = whio_epfs_block_empty;
        int rc = whio_rc.RangeError;
        for( ; i >= to; --i )
        {
            bl.id = i;
            bl.nextFree = (i > 1) ? (i-1) : 0;
            rc = whio_epfs_block_wipe( fs, &bl, true, true, false );
            if( rc )
            {
                //MARKER("Error writing block #%"WHIO_EPFS_ID_T_PFMT"!\n",i);
                break;
            }
        }
        return rc;
    }
}

#undef MARKER
/* end file pfs/block.c */
/* begin file pfs/handle.c */
#line 8 "pfs/handle.c"
//#define WHIO_DEBUG_ENABLED 1
#include <stdlib.h> // malloc() and friends.
#include <string.h> // memmove() and friends
#include <assert.h>

whio_epfs_handle * whio_epfs_handle_alloc(whio_epfs * fs)
{
    /**
       TODO: once i've pulled in whalloc support, allocate

       using that.
    */
    whio_epfs_handle * x = (whio_epfs_handle*)whio_epfs_malloc(fs,sizeof(whio_epfs_handle));
    if(x)
    {
        *x = whio_epfs_handle_empty;
        x->allocStamp = &whio_epfs_handle_empty;
        x->fs = fs;
    }
    return x;
}

int whio_epfs_handle_free(whio_epfs_handle * h)
{
    if( ! h ) return whio_rc.ArgError;
    else
    {
        whio_epfs * fs = h->fs;
        if( h->blocks.list )
        {
            whio_epfs_block_list_reserve( fs, &h->blocks, 0 );
        }
        void const * stamp = h->allocStamp;
        *h = whio_epfs_handle_empty;
        if( &whio_epfs_handle_empty == stamp )
        {
            whio_epfs_mfree( fs, h );
        }
        else
        {
            // Assume it was allocated somewhere else.
            h->allocStamp = stamp;
        }
        return 0;
    }
}

int whio_epfs_handle_list_reserve( whio_epfs * fs, whio_epfs_handle_list *li, whio_epfs_id_t n )
{
    if( ! li ) return whio_rc.ArgError;
    else if( 0 == n )
    {
        /* FIXME: This logic does not take into account closing of
           li->list entries. Doing so requires handling links, and
           that gets a bit kludgy. One significant problem is that
           calling whio_epfs_handle_close() from here will end up
           modifying li as we're traversing it.

           The real fix is probably to rework how we open one inode
           multiple times. However, since i quite like the current
           approach (except that it makes list-closing difficult), i'm
           still looking for another alternative before replacing
           that one.
        */
        whio_epfs_handle * h = 0;
        whio_epfs_id_t i;
        for( i = 0; i < li->count; ++i )
        {
            h = li->list[i];
            whio_epfs_handle_free(h);
        }
        whio_epfs_mfree( fs, li->list );
        *li = whio_epfs_handle_list_empty;
        return whio_rc.OK;
    }
    else if( li->alloced > n )
    {/* Reminder; we always ensure we have at least 1 extra alloced to make certain
        list manipulation legal (where we need to step one past the last item).
     */
        return whio_rc.OK;
    }
    whio_epfs_handle ** reli = (whio_epfs_handle **)whio_epfs_mrealloc( fs, li->list, n * sizeof(whio_epfs_handle*) );
    if( ! reli )
    {
        return whio_rc.AllocError;
    }
    li->list = reli;
    li->alloced = n;
    whio_epfs_id_t i = li->count;
    for( ; i < n; ++i )
    {
        li->list[i] = NULL;
    }
    return whio_rc.OK;
}

/**
   If TRY_SORTED_HANDLE_LIST is true then we will try to keep the handle
   lists sorted, to optimize certain search operations.

   That said, i don't expect opened-handle lists to get long enough
   that this overhead pays off. When only, say, up to 5 handles are open,
   it's cheaper to traverse them linearly.
*/
#define TRY_SORTED_HANDLE_LIST 0
#if TRY_SORTED_HANDLE_LIST
/**
   Compare function for qsort() and whio_epfs_handle_list.

   lhs and rhs must be (whio_epfs_handle **). It sorts only on
   the id field of the referenced inode.
*/
static int whio_epfs_handle_list_compare_id(const void * lhs, const void * rhs)
{
    if( lhs == rhs ) return 0;
    whio_epfs_handle const * l = lhs ? *((whio_epfs_handle const **)lhs) : NULL;
    whio_epfs_handle const * r = rhs ? *((whio_epfs_handle const **)rhs) : NULL;
    whio_epfs_id_t const vl = l ? whio_epfs_handle_inode_c(l)->id : 0U;
    whio_epfs_id_t const vr = r ? whio_epfs_handle_inode_c(r)->id : 0U;
    if( vl == vr ) return 0;
    else if( vl < vr ) return -1;
    else return 1;
}
#endif

/**
   On success, returns 0 and transfers ownership of h to li.
*/
int whio_epfs_handle_list_append( whio_epfs_handle_list *li, whio_epfs_handle * h )
{
    if( ! li || !h ) return whio_rc.ArgError;
    int rc = whio_epfs_handle_list_reserve( h->fs, li, li->count+1 );
    if( rc ) return rc;
    li->list[li->count] = h;
    ++li->count;
#if TRY_SORTED_HANDLE_LIST
    qsort( li->list, li->count, sizeof(li->list[0]), whio_epfs_handle_list_compare_id );
#else
    /*
      FIXME: sort the list by inode ID here to make searching faster
      later. We can do that by simply inserting in the proper spot, or
      qsort()ing the list after appending.
    */
#endif
    return whio_rc.OK;
}

whio_epfs_handle * whio_epfs_handle_origin( whio_epfs_handle * h )
{
    if( ! h ) return NULL;
    else
    {
        for( ; h->link && (h->link != h); h = h->link )
        {}
        return h;
    }
}

whio_epfs_inode * whio_epfs_handle_inode( whio_epfs_handle * h )
{
    return h ? &(whio_epfs_handle_origin(h)->inode) : NULL;
}
whio_epfs_handle const * whio_epfs_handle_origin_c( whio_epfs_handle const * h )
{
    if( ! h ) return NULL;
    else
    {
        for( ; h->link && (h->link != h); h = h->link )
        {}
        return h;
    }
}
whio_epfs_inode const * whio_epfs_handle_inode_c( whio_epfs_handle const * h )
{
    return h ? &(whio_epfs_handle_origin_c(h)->inode) : NULL;
}

/**
   Removes the item at index i from li and shifts all right-side
   entries to the left one position. It does NO CHECKING on the
   validity of li or i.

   Returns 0 unless li is null (==segfault) or i is enough out of
   range to cause a segfault.

   This does not free any memory, close any handles, or whatnot.
*/
static int whio_epfs_handle_list_rm_index( whio_epfs_handle_list * li, whio_epfs_id_t i )
{
    if( li->count ) --li->count;
    const size_t hsz = sizeof(whio_epfs_handle*);
    memmove( &li->list[i], &li->list[i+1], hsz * (li->count-i) );
    return 0;
}

whio_epfs_handle *  whio_epfs_handle_search( whio_epfs_handle_list * li, whio_epfs_handle * h, bool removeIt )
{
    if( ! li || !h ) return NULL;
    // FIXME?: keep the list sorted and use bsearch() here.
    whio_epfs_id_t i = 0;
    whio_epfs_handle * o = 0;
    for( ; i < li->count; ++i )
    {
        o = li->list[i];
        if( o == h ) break;
    }
    if( o != h ) return NULL;
    if( removeIt )
    {
#if 0
        if( li->count ) --li->count;
        const size_t hsz = sizeof(whio_epfs_handle*);
        memmove( &li->list[i], &li->list[i+1], hsz * (li->count-i) );
#elif 1
        whio_epfs_handle_list_rm_index( li, i );
#else
        if( li->count ) --li->count;
        for( ; i <= li->count; ++i )
        {
            li->list[i] = li->list[i+1];
            /** that i+1 is safe ^^^^^^ because of the li->count decrement.

                But valgrind is complaining, saying it's out of bounds.
            */
        }
#endif
    }
    return o;
}

whio_epfs_handle *  whio_epfs_handle_search_id( whio_epfs_handle_list * li, whio_epfs_id_t id )
{
    if( ! li || !id ) return NULL;
    whio_epfs_id_t i = 0;
    whio_epfs_handle * h = 0;
    for( ; i < li->count; ++i )
    {
        h = li->list[i];
        if( ! h )
        {
            assert( 0 && "Internal error: mis-management of whio_epfs_handle_list::list[] data." );
            /* internal error! */ continue;
        }
        else if( h->link && (h->link != h) ) /* we only want origin handles. */ continue; 
#if TRY_SORTED_HANDLE_LIST
        // This relies on the qsort() in whio_epfs_handle_list_append():
        else if( h->inode.id < id ) continue;
        else if( h->inode.id > id ) h = NULL; /* and fall through to break. */
        /** We _could_ use bsearch() in this case, but we don't expect li to
            grow enough that this sorting makes any real difference.
         */
#else
        else if( h->inode.id != id ) continue;
#endif
        break;
    }
    return h;
}

bool whio_epfs_handle_is_rw( whio_epfs_handle const * h )
{
    return h && (h->iomode & WHIO_MODE_RW);
}

int whio_epfs_handle_open( whio_epfs * fs,
                          whio_epfs_handle ** tgt,
                          whio_epfs_id_t id,
                          int mode )
{
    if( !fs || !tgt ) return whio_rc.ArgError;
    else if( !(mode & (WHIO_MODE_READ | WHIO_MODE_WRITE) ) ) return whio_rc.ArgError;
    else if( !(mode & WHIO_MODE_WRITE)
             &&
             (mode & (WHIO_MODE_FLAG_CREATE | WHIO_MODE_FLAG_TRUNCATE)) )
    {
        /** invalid combination */
        return whio_rc.ArgError;
    }
    else if( !id && !(mode & WHIO_MODE_FLAG_CREATE) )
    { /** if id is 0 we require the create flag. */
        return whio_rc.ArgError;
    }
    else if( id && ! whio_epfs_inode_id_in_bounds( fs, id ) )
    {
        return whio_rc.RangeError;
    }
    else if( (mode & (WHIO_MODE_WRITE|WHIO_MODE_FLAG_CREATE) )
             && (!whio_epfs_is_rw(fs)) )
    { /* fs is r/o but caller asked for r/w. */
        return whio_rc.AccessError;
    }
    whio_epfs_inode oni = whio_epfs_inode_empty;
    int rc = 0;
    whio_epfs_handle * h = NULL;
    whio_epfs_handle * origin = NULL;
    whio_epfs_id_t i = 0;
    if(id) for( ; i < fs->handles.count; ++i )
    { // FIXME: keep list sorted on inode id to make this faster.
        h = fs->handles.list[i];
#if 0
        if( !h || (h->inode.id != id ) ) continue;
        if( h->link && (h->link!=h) ) continue;// try to find originals only
        origin = h;//whio_epfs_handle_origin(h);
#else
        origin = whio_epfs_handle_origin(h);
        if( origin->inode.id != id ) continue;
#endif
        break;
    }
    h = 0;
    if( origin )
    {
        if( 0 && (mode == origin->iomode) )
        {// this is soooo arguable... recycle the origin instance.
            ++origin->openCount;
            *tgt = origin;
            return whio_rc.OK;
        }
        // else fall through and link to the original
        if( (origin->openCount+1) < origin->openCount )
        { /* overflow! */
            WHIO_DEBUG("WARNING: continuing would overflow whio_epfs_handle::openCount!\n");
            return whio_rc.RangeError;
        }
    }
    /**
       This ownsHandle business is not being advertised in the API
       docs until i can confirm that client-supplied handles can
       sensibly honor the lifetime guaranteees we need for them.

       Maintenance reminder: we allocate before the reading, despite
       the potential for i/o errors, because the recovery is simpler
       this way. If we do all the read/inode-marking stuff and THEN
       alloc fails, we have to un-do our inode changes. This ordering
       also simplifies locking, though we have to clean up h if
       locking fails.
    */
    const bool ownsHandle = !*tgt;
    h = ownsHandle ? whio_epfs_handle_alloc(fs) : *tgt;
    if( ! h )
    {
        return whio_rc.AllocError;
    }
#define CLEANUP if(ownsHandle) whio_epfs_handle_free(h)
    /*
       FIXME: lock inode here.
    */
    if( id )
    {
        rc = whio_epfs_inode_read( fs, id, &oni );
    }
    else if( WHIO_MODE_FLAG_CREATE & mode )
    {
        rc = whio_epfs_inode_next_free( fs, &oni, true );
    }
    else
    {
        rc = whio_rc.ArgError;
    }
    if( rc )
    {
        CLEANUP;
        /*
          FIXME: need to unlock here on error.
        */
        return rc;
    }
    bool wasUsed = whio_epfs_inode_is_used(&oni);
    if( id && (!wasUsed) && !(mode & WHIO_MODE_FLAG_CREATE) )
    {
        /*
          FIXME: need to unlock here on error.
        */
        CLEANUP;
        return whio_rc.AccessError;//need new error code for this???
    }
    if( ! wasUsed )
    {
        whio_epfs_inode_set_used( fs, &oni, true );
        rc = whio_epfs_inode_flush( fs, &oni );
        if( rc )
        {
            /*
              FIXME: need to unlock here on error.
            */
            CLEANUP;
            return rc;
        }
    }
#undef CLEANUP
    h->openCount = 1;
    if( origin ) ++origin->openCount;
#if 0
    if( mode & WHIO_MODE_WRITE )
    {
        if( origin ) ++origin->writerCount;
        else h->writerCount = 1;
    }
#endif
    h->iomode = mode;
    h->link = origin ? origin : h;
    h->inode = origin
        ? whio_epfs_inode_empty /* we don't want that inode to ever be used. */
        : oni;
    h->fs = fs;
    whio_epfs_handle_list_append( &fs->handles, h );
    if( ownsHandle ) *tgt = h;
#if 0
    whio_epfs_flush( fs )
        /*Ensure recent free-list changes are written, but adds a
          noticable about to runtime to some uses cases (just over 30%
          in my quick tests).
         */
        ;
#endif
    return whio_rc.OK;
}

int whio_epfs_handle_close( whio_epfs_handle * h )
{
    if( ! h ) return whio_rc.ArgError;
#if 1
    const whio_epfs_handle * o = whio_epfs_handle_search( &h->fs->handles, h, false );
    if( o != h )
    {
        return whio_rc.RangeError;
    }
#endif
    whio_epfs_handle * origin = whio_epfs_handle_origin(h);
#define XXX 0
#if XXX
    WHIO_DEBUG("Closing handle: h=%p, origin=%p\n",(void const *)h,(void const *)origin);
    WHIO_DEBUG("h->openCount=%"WHIO_EPFS_ID_T_PFMT", origin->openCount=%"WHIO_EPFS_ID_T_PFMT"\n",
           h->openCount, origin->openCount);
#endif
    --h->openCount;
    const bool isrw = whio_epfs_handle_is_rw(h);
    int rc = 0;
    if( isrw )
    { // fixme: only flush if it's dirty
        rc = whio_epfs_inode_flush( h->fs, &origin->inode );
    }
#if XXX
    WHIO_DEBUG("h->openCount=%"WHIO_EPFS_ID_T_PFMT", origin->openCount=%"WHIO_EPFS_ID_T_PFMT"\n",
           h->openCount, origin->openCount);
#endif
    // TODO: reduce origin->writerCount. When it's zero:
    // unlock if !h->openCount.
    // change to read-lock if h->openCount
    if( ! h->openCount )
    {
#if XXX
        WHIO_DEBUG("Freeing handle #%"WHIO_EPFS_ID_T_PFMT" @%p\n", origin->inode.id, (void const *)h );
#endif
        //whio_epfs * fs = origin->fs;
        whio_epfs_handle_search( &origin->fs->handles, h, true );
        whio_epfs_handle_free(h);
#if 0
        whio_epfs_flush( fs ) /* ensure recent free-list changes are written.*/;
#endif
        rc = whio_rc.OK;
    }
    if( (origin != h) )
    {
        {
#if XXX
            WHIO_DEBUG("Handling free() of origin @%p.\n",(void const *)origin);
#endif
            rc = whio_epfs_handle_close(origin)/*decrement origin->openCount */;
        }
    }
#undef XXX
    return rc;
}

#undef TRY_SORTED_HANDLE_LIST
/* end file pfs/handle.c */
/* begin file pfs/inode.c */
#line 8 "pfs/inode.c"
/************************************************************************
This file contains most of the inode-specific whio_epfs routines.

Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

License: Public Domain
************************************************************************/

#if !defined(_POSIX_C_SOURCE)
/* required for for gmtime_r() */
#  define _POSIX_C_SOURCE 200112L
#endif

#include <assert.h>
#include <time.h> // time(), gmtime(), mktime()

#if 1
#define MARKER(X)
#else
#define MARKER(X) printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); printf X
#endif

bool whio_epfs_inode_is_used( whio_epfs_inode const * ino )
{
    return ino && (ino->flags & WHIO_EPFS_FLAG_IS_USED);
}
/** @internal
   Updates fs->hints.freeInodeList to remove ino.

   ino and its neighbors are flushed.

   Returns 0 on success. On error the free list may be
   in an undefined state.
*/
static int whio_epfs_inode_from_freelist( whio_epfs * fs,
                                          whio_epfs_inode * ino )
{
    whio_epfs_inode on = whio_epfs_inode_empty;
    int rc = 0;
    int i = 0;
    if( !ino->nextFree && !ino->prevFree )
    {
        if( fs->hints.freeInodeList == ino->id )
        {
            fs->hints.freeInodeList = 0;
        }
        return 0;
    }
    for( ; i < 2; ++i )
    {
        whio_epfs_id_t nid = (0==i) ? ino->prevFree : ino->nextFree;
        if( ! nid ) continue;
        rc = whio_epfs_inode_read( fs, nid, &on );
        if( rc ) break;
        if( 0 == i )
        { /* we're on the left. */
            on.nextFree = ino->nextFree;
        }
        else
        { /* we're on the right */
            on.prevFree = ino->prevFree;
        }
        rc = whio_epfs_inode_flush( fs, &on );
        if( rc ) break;
    }
    if( !rc )
    {
        if( fs->hints.freeInodeList == ino->id )
        {
            fs->hints.freeInodeList = ino->prevFree ? ino->prevFree : ino->nextFree;
        }
        
        MARKER(("Removed inode #%"WHIO_EPFS_ID_T_PFMT" from free-list. "
                "fs->hints.freeInodeList=%"WHIO_EPFS_ID_T_PFMT"\n",
                ino->id, fs->hints.freeInodeList ));
        ino->nextFree = ino->prevFree = 0;
        rc = whio_epfs_inode_flush( fs, ino );
    }
    return rc;
}

/** @internal
   Updates fs->hints.freeInodeList to insert ino
   as the first free item. This modifies ino->nextFree
   and ino->prevFree.

   ino and its neighbors are flushed.

   Returns 0 on success. On error the free list may be
   in an undefined state.
*/
static int whio_epfs_inode_to_freelist( whio_epfs * fs, whio_epfs_inode * ino )
{

    int rc = 0;
    if( fs->hints.freeInodeList == ino->id ) return rc;
    else if( fs->hints.freeInodeList )
    {
        whio_epfs_inode on = whio_epfs_inode_empty;
        rc = whio_epfs_inode_read( fs, fs->hints.freeInodeList, &on );
        if( rc ) return rc;
        on.prevFree = ino->id;
        rc = whio_epfs_inode_flush( fs, &on );
        if( rc ) return rc;
        ino->nextFree = on.id;
    }
    else
    { /* This inode is now the head and tail of the free list. */
        ino->nextFree = 0;
    }
    ino->prevFree = 0;
    rc = whio_epfs_inode_flush( fs, ino );
    if( !rc )
    {
        fs->hints.freeInodeList = ino->id;
    }
    return rc;
}

int whio_epfs_inode_set_used( whio_epfs * fs, whio_epfs_inode * ino, bool u )
{
    if( ! ino ) return whio_rc.ArgError;
    else
    {
        if(u)
        {
            if( !(ino->flags & WHIO_EPFS_FLAG_IS_USED) )
            {
                whio_epfs_inode_from_freelist( fs, ino );
                ino->flags|=WHIO_EPFS_FLAG_IS_USED;
                MARKER(("Setting inode #%"WHIO_EPFS_ID_T_PFMT" as used. "
                       "freeInodeList=%"WHIO_EPFS_ID_T_PFMT"\n",
                        ino->id, fs->hints.freeInodeList ));
                if( ! ino->mtime )
                {
                    whio_epfs_inode_touch( ino, -1 );
                }
            }
        }
        else
        {
            if( ino->flags & WHIO_EPFS_FLAG_IS_USED )
            {
                whio_epfs_id_t id = ino->id;
                *ino = whio_epfs_inode_empty;
                ino->id = id;
                whio_epfs_inode_to_freelist( fs, ino );
            }
        }
        return whio_rc.OK;
    }
}


static const unsigned char whio_epfs_inode_tag_char = 'I';
whio_size_t whio_epfs_inode_encode( whio_epfs_inode const * ino, unsigned char * dest )
{
    if( ! ino || ! dest ) return 0;
    *(dest++) = whio_epfs_inode_tag_char;
    dest += whio_epfs_encode_id( dest, ino->id );
    dest += whio_encode_uint8( dest, ino->flags );
    dest += whio_epfs_encode_id( dest, ino->nextFree );
    dest += whio_epfs_encode_id( dest, ino->prevFree );
    dest += whio_epfs_encode_id( dest, ino->firstBlock );
    dest += whio_encode_uint32( dest, ino->cflags );
    dest += whio_encode_size_t( dest, ino->size );
    dest += whio_encode_uint32( dest, ino->mtime );
    return whio_epfs_sizeof_inode;
}
int whio_epfs_inode_decode( whio_epfs_inode * ino, unsigned char const * src )
{
    unsigned char const * at = src;
    if( *(at++) != whio_epfs_inode_tag_char )
    {
        return whio_rc.ConsistencyError;
    }
    else
    {
        int rc = 0;
        rc = whio_epfs_decode_id( at, &ino->id );
        if( rc ) return rc;
        at += whio_epfs_sizeof_id;

        rc = whio_decode_uint8( at, &ino->flags );
        if( rc ) return rc;
        at += whio_sizeof_encoded_uint8;

        rc = whio_epfs_decode_id( at, &ino->nextFree );
        if( rc ) return rc;
        at += whio_epfs_sizeof_id;

        rc = whio_epfs_decode_id( at, &ino->prevFree );
        if( rc ) return rc;
        at += whio_epfs_sizeof_id;       
        
        rc = whio_epfs_decode_id( at, &ino->firstBlock );
        if( rc ) return rc;
        at += whio_epfs_sizeof_id;

        rc = whio_decode_uint32( at, &ino->cflags );
        if( rc ) return rc;
        at += whio_sizeof_encoded_uint32;
        
        rc = whio_decode_size_t( at, &ino->size );
        if( rc ) return rc;
        at += whio_sizeof_encoded_size_t;

        rc = whio_decode_uint32( at, &ino->mtime );
        if( rc ) return rc;
        at += whio_sizeof_encoded_uint32;

        return rc;
    }
}

bool whio_epfs_inode_id_in_bounds( whio_epfs const * fs,
                                 whio_epfs_id_t ino )
{
    return fs && ino && (ino <= fs->fsopt.inodeCount);
}

whio_size_t whio_epfs_inode_id_pos( whio_epfs const * fs,
                                   whio_epfs_id_t id )
{
    return ( !whio_epfs_inode_id_in_bounds( fs, id ) )
        ? 0
        : (fs->offsets[whio_epfs_index_inode] +
           ((id-1) * fs->sizes[whio_epfs_index_inode]));
}

int whio_epfs_inode_touch( whio_epfs_inode * ino, uint32_t t )
{
    if( ! ino ) return whio_rc.ArgError;
    else
    {
#if 0
        time_t tx = time(NULL);
        struct tm gt;
        ino->mtime = (uint32_t)mktime( gmtime_r( &tx, &gt ) );
        // FUCK! gmtime_r() allocates memory! WTF for????
#else
        if( ((uint32_t)-1) != t )
        {
            ino->mtime = t;
        }
        else
        {
            const time_t tx = time(NULL);
            ino->mtime = (uint32_t)mktime( gmtime(&tx) );
            /* In kcachegrind i'm seeing that
               gmtime(), and possibly mktime(),
               are indirectly calling malloc() (at least
               sometimes). :(
             */
        }
#endif        
        return 0;
    }
}

int whio_epfs_inode_next_free( whio_epfs * fs, whio_epfs_inode * dest, bool markAsUsed )
{
    if( ! fs || ! dest ) return whio_rc.ArgError;
    else if( markAsUsed && !whio_epfs_is_rw(fs) )
    {
        return whio_rc.AccessError;
    }
    int rc = 0;
    /**
       This impl would be guaranteed O(1), but the ability to open an
       arbitrary inode number means that our free-list can get
       confused. So we have a potential linear component.
    */
    whio_epfs_inode ino = whio_epfs_inode_empty;
    whio_epfs_id_t id = fs->hints.freeInodeList;
    while(id)
    {
        rc = whio_epfs_inode_read( fs, id, &ino );
        if( rc ) return rc;
        if( whio_epfs_inode_is_used( &ino ) )
        {
            MARKER(("Is-free check of inode #%"WHIO_EPFS_ID_T_PFMT" failed.\n",ino.id));
            if( ino.nextFree )
            {
                MARKER(("Trying ino.nextFree #%"WHIO_EPFS_ID_T_PFMT".\n",ino.nextFree));
                if( fs->hints.freeInodeList == ino.id )
                {
                    fs->hints.freeInodeList = ino.nextFree;
                }
                id = ino.nextFree;
                continue;
            }
            else
            {
                return whio_rc.ConsistencyError/*???*/;
            }
        }
        break;
    }
    if( ! id )
    { /* assume we are full */
        return whio_rc.DeviceFullError;
    }
    if( markAsUsed )
    {
        rc = whio_epfs_inode_set_used( fs, &ino, true );
        //if(!rc) rc = whio_epfs_inode_flush( fs, &ino );
        if(!rc) rc = whio_epfs_hints_write( fs );
    }
    if( ! rc )
    {
        *dest = ino;
    }
    return rc;
}

                             
int whio_epfs_inode_flush( whio_epfs * fs,
                           whio_epfs_inode const * ino )
{
    if( ! fs || !ino ) return whio_rc.ArgError;
    if( ! whio_epfs_is_rw(fs) ) return whio_rc.AccessError;
    whio_size_t pos = whio_epfs_inode_id_pos( fs, ino->id );
    if( ! pos ) return whio_rc.RangeError;
    enum { Len = whio_epfs_sizeof_inode };
    unsigned char buf[Len];
    whio_epfs_inode_encode( ino, buf );
    whio_size_t wrc = whio_epfs_writeat( fs, pos, buf, Len );
    if( Len != wrc )
    {
        return whio_rc.IOError;
    }
    else
    {
        uint8_t changes = 0;
        if( whio_epfs_inode_is_used(ino) )
        {
            if( ino->id > fs->hints.maxEverUsedInode )
            {
                fs->hints.maxEverUsedInode = ino->id;
                ++changes;
            }
        }
#if WHIO_EPFS_CONFIG_AUTO_FLUSH_HINTS
        if( changes )
        {
            int rc = whio_epfs_hints_write( fs );
            if( rc ) return rc;
        }
#endif
        return whio_rc.OK;
    }
}


int whio_epfs_inode_read( whio_epfs * fs,
                          whio_epfs_id_t id,
                          whio_epfs_inode * dest )
{
    if( ! fs || !dest ) return whio_rc.ArgError;
    whio_size_t pos = whio_epfs_inode_id_pos( fs, id );
    if( ! pos )
    {
        return whio_rc.RangeError;
    }
    enum { Len = whio_epfs_sizeof_inode };
    unsigned char buf[Len];
    whio_epfs_inode ino = whio_epfs_inode_empty;
    whio_size_t sz = whio_epfs_readat( fs, pos, buf, Len );
    if( sz != Len )
    {
        return fs->err = whio_rc.IOError;
    }
    int rc = whio_epfs_inode_decode( &ino, buf );
    if( rc ) return fs->err = rc;
    { /* check for fs hint update. */
        int8_t changes = 0;
        if( whio_epfs_inode_is_used(&ino) )
        {
            if( fs->hints.maxEverUsedInode < ino.id )
            {
                fs->hints.maxEverUsedInode = ino.id;
                ++changes;
            }
        }
#if WHIO_EPFS_CONFIG_AUTO_FLUSH_HINTS
        if( changes && whio_epfs_is_rw(fs) )
        {
            rc = whio_epfs_hints_write( fs );
            if( rc ) return rc;
        }
#endif
    }
    *dest = ino;
    return whio_rc.OK;
}

int whio_epfs_foreach_inode_in_range( whio_epfs * fs,
                                      whio_epfs_id_t begin,
                                      whio_epfs_id_t end,
                                      whio_epfs_inode_predicate_f where, void * whereData,
                                      whio_epfs_inode_foreach_f func, void * foreachData )
{
    if( ! fs || !func ) return whio_rc.ArgError;
    if( ! end )
    {
        end = fs->fsopt.inodeCount + 1;
    }
    if( !begin
        || (begin > end)
        || !whio_epfs_inode_id_in_bounds(fs,begin)
        || !whio_epfs_inode_id_in_bounds(fs,end-1)
        )
    {
        return whio_rc.RangeError;
    }
    whio_epfs_id_t i = begin;
    whio_epfs_inode n;;
    int rc = whio_rc.OK;
    for( ; (i < end) && !rc; ++i )
    {
        n = whio_epfs_inode_empty;
	rc = whio_epfs_inode_read( fs, i, &n );
	if( rc ) break;
	if( n.id != i )
	{
	    WHIO_DEBUG("BUG: Node id mismatch after successful whefs_inode_id_read(). Expected %"WHIO_EPFS_ID_T_PFMT" but got %"WHIO_EPFS_ID_T_PFMT".", i, n.id );
	    assert( 0 && "node id mismatch after whio_epfs_inode_read()" );
	    return whio_rc.InternalError;
	}
        if( where )
        {
            if( ! where( fs, &n, whereData ) ) continue;
        }
        else
        {
            if( fs->hints.maxEverUsedInode < n.id )
            {
                /**
                   We know (if all is well, anyway) that no inodes
                   are used past this point, so we can break off
                   this loop.

                   Reminder: maxEverUsedInode never shrinks. If the max
                   inode is removed, we _could_ simply --maxEverUsedInode
                   in that case, but we would then need to check that
                   the inode at that pos is used (ad nauseum, leftwards)
                   to keep maxEverUsedInode correct. So we punt on that problem
                   and simply keep the highest-ever value, even if it leads us
                   to the end.
                */
                break;
            }
            else if( ! whio_epfs_inode_is_used(&n) ) continue;
        }
        rc = func( fs, &n, foreachData );
    }
    return rc;
}

bool whio_epfs_inode_predicate_true( whio_epfs * fs, whio_epfs_inode const * n, void * clientData )
{
    return true;
}

int whio_epfs_foreach_inode( whio_epfs * fs, whio_epfs_inode_predicate_f where, void * whereData,
                             whio_epfs_inode_foreach_f func, void * foreachData )
{
    if( ! fs || !func ) return whio_rc.ArgError;
    return whio_epfs_foreach_inode_in_range( fs, 1, 0,
                                             where, whereData,
                                             func, foreachData );
}

int whio_epfs_inode_client_flags_set( whio_epfs_inode * ino, uint32_t flags )
{
    return ( !ino )
        ? whio_rc.ArgError
        : ((ino->cflags = flags), 0);
}

int whio_epfs_inode_id_client_flags_set( whio_epfs * fs, whio_epfs_id_t id,
                                         uint32_t flags )
{
    if( ! fs ) return whio_rc.ArgError;
    else if( ! whio_epfs_is_rw( fs ) )
    {
        return whio_rc.AccessError;
    }
    whio_epfs_handle * h = whio_epfs_handle_search_id( &fs->handles, id );
    whio_epfs_inode inoBuf = whio_epfs_inode_empty;
    whio_epfs_inode * ino = NULL;
    int rc = 0;
    if( h )
    {
        ino = whio_epfs_handle_inode( h );
    }
    else
    {
        rc = whio_epfs_inode_read( fs, id, &inoBuf );
        if( rc ) return rc;
        ino = &inoBuf;
    }
    rc = whio_epfs_inode_client_flags_set( ino, flags );
    if( ! rc )
    {
        rc = whio_epfs_inode_flush( fs, ino );
    }
    return rc;
}

int whio_epfs_inode_client_flags_get( whio_epfs_inode const * ino, uint32_t * flags )
{
    if( !ino ) return whio_rc.ArgError;
    else
    {
        if( flags ) *flags = ino->cflags;
        return 0;
    }
}

int whio_epfs_inode_id_client_flags_get( whio_epfs * fs, whio_epfs_id_t id,
                                         uint32_t * flags )
{
    if( ! fs ) return whio_rc.ArgError;
    whio_epfs_handle * h = whio_epfs_handle_search_id( &fs->handles, id );
    whio_epfs_inode inoBuf = whio_epfs_inode_empty;
    whio_epfs_inode * ino = NULL;
    int rc = whio_rc.OK;
    if( h )
    {
        ino = whio_epfs_handle_inode( h );
    }
    else
    {
        rc = whio_epfs_inode_read( fs, id, &inoBuf );
        if( rc ) return rc;
        ino = &inoBuf;
    }
    if( !rc )
    {
        rc = whio_epfs_inode_client_flags_get( ino, flags );
    }
    return rc;
}

#undef MARKER
/* end file pfs/inode.c */
/* begin file pfs/iodev.c */
#line 8 "pfs/iodev.c"
/**
   This is a skeleton implementation for whio_dev implementations. To use it:

   - Pick a device type name for your device. For example, "mydev".
   - Globally replace pfs in this file with that chosen name.
   - Search through this code for comment lines and do what they say
*/

#include <assert.h>
#include <string.h> /* memset() */
/**
   Internal implementation details for the whio_dev wrapper.

   Put any device-specific details here.
*/
typedef struct whio_epfs_inode_iodev_meta
{
    /**
       The handle associated with this device.
    */
    whio_epfs_handle * h;
    /**
       The _origin_ inode associated with this device.
       May differ from h->inode if h is opened multiple
       times.
    */
    whio_epfs_inode * ino;
    /**
       Signifies read/write mode.
    */
    bool rw;
    /* Shorthand for h->fs->fsopt.blockSize. */
    whio_size_t bs;
    /**
       Memory this object should free when closed.  We may allocate
       several objects into this, and must be very, very careful how
       we deallocate it.
    */
    void * memToFree;
} whio_epfs_inode_iodev_meta;

/** Used for "zeroing out" whio_epfs_inode_iodev_meta instances. */
static const whio_epfs_inode_iodev_meta whio_epfs_inode_iodev_meta_empty = {
NULL/*h*/,NULL/*ino*/,false/*rw*/,0/*bs*/,NULL/*memToFree*/
};

/**
   A helper for the whio_epfs_inode_iodev API. Requires that the 'dev'
   parameter be-a whio_dev and that that device is-a whio_epfs_inode_iodev.
 */
#define WHIO_EPFS_DECL(RV) whio_epfs_inode_iodev_meta * m = (dev ? (whio_epfs_inode_iodev_meta*)dev->impl.data : 0); \
    if( !m || !m->h || ((void const *)&whio_epfs_inode_iodev_meta_empty != dev->impl.typeID) ) return RV
#define WHIO_EPFS_CDECL(RV) whio_epfs_inode_iodev_meta const * m = (dev ? (whio_epfs_inode_iodev_meta const*)dev->impl.data : 0); \
    if( !m || !m->h || ((void const *)&whio_epfs_inode_iodev_meta_empty != dev->impl.typeID) ) return RV

/**
   Internal implementation of whio_epfs_inode_iodev_read(). All arguments
   are as for whio_dev::read() except keepGoing:

   If this routine must "wrap" over multiple blocks, it will read what it
   can from one block and then set keepGoing to true and return the read
   size. The caller should, if keepGoing is true, call this function
   again, adding the returned size to dest and subtracting it from n.

   None of the pointer arguments may be null.
*/
static whio_size_t whio_epfs_inode_iodev_read_impl( whio_dev * dev,
                                           whio_epfs_inode_iodev_meta * m,
                                           void * dest,
                                           whio_size_t n,
                                           bool * keepGoing )
{
    *keepGoing = false;
    if( ! n ) return 0U;
    else if( m->h->cursor >= m->ino->size ) return 0;
    int rc = 0;
    whio_epfs_block block = whio_epfs_block_empty;
    rc = whio_epfs_block_for_pos( m->h, m->h->cursor, &block, false );
    if( whio_rc.OK != rc )
    {
        WHIO_DEBUG("Error #%d getting block for m->h->cursor=%u. n=%"WHIO_SIZE_T_PFMT", bs=%"WHIO_SIZE_T_PFMT"\n",
                  rc, m->h->cursor, n, m->h->fs->fsopt.blockSize );
        return 0;
    }
    if(0)
    {
        WHIO_DEBUG("inode #%"WHIO_EPFS_ID_T_PFMT" will be using block #%"WHIO_EPFS_ID_T_PFMT" for a read at pos %"WHIO_SIZE_T_PFMT"\n",
                   m->ino->id, block.id, m->h->cursor );
    }
    if( m->h->cursor >= m->ino->size ) return 0;
    const whio_size_t rdpos = (m->h->cursor % m->bs);
    const whio_size_t left = m->bs - rdpos;
    const whio_size_t bdpos = whio_epfs_block_data_pos( m->h->fs, block.id );
    whio_size_t rdlen = ( n > left ) ? left : n;
    if( (rdlen + m->h->cursor) >= m->ino->size )
    {
        rdlen = m->ino->size - m->h->cursor;
    }
    //WHIO_DEBUG("rdpos=%u left=%u bdpos=%u rdlen=%u\n", rdpos, left, bdpos, rdlen );
#if 0
    whio_dev * fd = m->h->fs->dev;
    fd->api->seek( fd, bdpos + rdpos, SEEK_SET );
    const whio_size_t sz = fd->api->read( fd, dest, rdlen );
#else
    const whio_size_t sz = whio_epfs_readat( m->h->fs, bdpos + rdpos, dest, rdlen );
#endif
    if( ! sz ) return 0;
    const whio_size_t szCheck = m->h->cursor + sz;
    if( szCheck > m->h->cursor )
    {
        m->h->cursor = szCheck;
    }
    else
    {
        WHIO_DEBUG("Numeric overflow in read! (pos=%"WHIO_SIZE_T_PFMT" + readLength=%"WHIO_SIZE_T_PFMT") = overflow\n", m->h->cursor, sz );
        return 0;
    }
    //whefs_block_flush( m->h->fs, &block );
    if(0)
    {
        WHIO_DEBUG("Read %"WHIO_SIZE_T_PFMT" of %"WHIO_EPFS_ID_T_PFMT" (n=%"WHIO_SIZE_T_PFMT") bytes "
                   "from inode #%"WHIO_EPFS_ID_T_PFMT"'s block #%"WHIO_EPFS_ID_T_PFMT". "
                   "fs pos=%"WHIO_SIZE_T_PFMT", block offset=%"WHIO_SIZE_T_PFMT" file pos=%"WHIO_SIZE_T_PFMT", file eof=%"WHIO_SIZE_T_PFMT"\n",
                   sz, rdlen, n,
                   m->ino->id, block.id,
                   bdpos, rdpos, m->h->cursor, m->ino->size );
    }
    if( sz < rdlen )
    { /* short write! */
        return sz;
    }
    else if( rdlen < n )
    { /* Wrap to next block and continue... */
        *keepGoing = true;
        return sz;
    }
    else
    { /* got the exact right amount */
        return sz;
    }
}
/**
   whio_dev::read() impl for whio_epfs_inode_iodev.
*/
static whio_size_t whio_epfs_inode_iodev_read( whio_dev * dev, void * dest, whio_size_t n )
{
    WHIO_EPFS_DECL(0);
    bool keepGoing = true;
    whio_size_t total = 0;
    while( keepGoing )
    {
        const whio_size_t sz = whio_epfs_inode_iodev_read_impl( dev, m, WHIO_VOID_PTR_ADD(dest,total), n - total, &keepGoing );
        total += sz;
    }
    return total;
}



/**
   This function's logic and handling of the keepGoing parameter are
   identical to that of whio_epfs_inode_iodev_read_impl(), but apply to
   writes instead of reads.

   None of the pointer arguments may be null.
*/
static whio_size_t whio_epfs_inode_iodev_write_impl( whio_dev * dev,
                                            whio_epfs_inode_iodev_meta * m,
                                            void const * src,
                                            whio_size_t n,
                                            bool * keepGoing )
{
    *keepGoing = false;
    int rc = 0;
    whio_epfs_block block = whio_epfs_block_empty;
    rc = whio_epfs_block_for_pos( m->h, m->h->cursor, &block, true );
    if( whio_rc.OK != rc )
    {
        WHIO_DEBUG("Error #%d getting block for m->h->cursor==%u\n\n", rc, m->h->cursor );
        return 0;
    }
    const whio_size_t wpos = (m->h->cursor % m->bs);
    const whio_size_t left = m->bs - wpos;
    const whio_size_t bdpos = whio_epfs_block_data_pos( m->h->fs, block.id );
    const whio_size_t wlen = ( n > left ) ? left : n;
    //WHIO_DEBUG("wpos=%u left=%u bdpos=%u wlen=%u\n\n", wpos, left, bdpos, wlen );
    //whio_dev * fd = m->h->fs->dev;
    //fd->api->seek( fd, bdpos + wpos, SEEK_SET );
    whio_size_t sz =
        whio_epfs_writeat( m->h->fs, bdpos+wpos, src, wlen );
        //fd->api->write( fd, src, wlen );
    if( wlen != sz ) return sz;
    whio_epfs_inode_touch( m->ino, -1 );
    whio_size_t szCheck = m->h->cursor + sz;
    if( szCheck > m->h->cursor )
    {
        m->h->cursor = szCheck;
    }
    if( m->ino->size < m->h->cursor )
    {
        m->ino->size = m->h->cursor;
        //whio_epfs_inode_flush( m->h->fs, &m->ino ); // we should do this, really.
    }
    if(0) WHIO_DEBUG("Wrote %u of %u (n=%u) bytes "
                    "to inode #%u's block #%u. "
                    "fs pos=%u, block offset=%u file pos=%u, file eof=%u\n\n",
                    sz, wlen, n,
                    m->ino->id, block.id,
                    bdpos, wpos, m->h->cursor, m->ino->size );
    if( sz < wlen )
    { /* short write! */
        return sz;
    }
    else if( wlen < n )
    { /* Wrap to next block and continue... */
        *keepGoing = true;
        return sz;
    }
    else
    {
        return sz;
    }
}
/**
   whio_dev::write() impl for whio_epfs_inode_iodev.
*/
static whio_size_t whio_epfs_inode_iodev_write( whio_dev * dev, void const * src, whio_size_t n )
{
    WHIO_EPFS_DECL(0);
    if( !src || !n || !m->rw )
    {
        return 0;
    }
    bool keepGoing = true;
    whio_size_t total = 0;
    while( keepGoing )
    {
        total += whio_epfs_inode_iodev_write_impl( dev, m, WHIO_VOID_CPTR_ADD(src,total), n - total, &keepGoing );
    }
    return total;
}

static int whio_epfs_inode_iodev_error( whio_dev * dev )
{
    return 0;
}

static int whio_epfs_inode_iodev_clear_error( whio_dev * dev )
{
    return 0;
}

static int whio_epfs_inode_iodev_eof( whio_dev * dev )
{
    WHIO_EPFS_DECL(whio_rc.ArgError);
    return (m->h->cursor < m->ino->size)
        ? 0
        : 1;
}

static whio_size_t whio_epfs_inode_iodev_tell( whio_dev * dev )
{
    WHIO_EPFS_DECL(whio_rc.SizeTError);
    return m->h->cursor;
}

static whio_size_t whio_epfs_inode_iodev_seek( whio_dev * dev, whio_off_t pos, int whence )
{
    WHIO_EPFS_DECL(whio_rc.SizeTError);
    whio_size_t too = m->h->cursor;
    switch( whence )
    {
      case SEEK_SET:
          if( pos < 0 ) return whio_rc.SizeTError;
          too = (whio_size_t)pos;
          break;
      case SEEK_END:
          too = m->ino->size + pos;
          if( (pos>0) && (too < m->ino->size) )  /* overflow! */ return whio_rc.SizeTError;
          else if( (pos<0) && (too > m->ino->size) )  /* underflow! */ return whio_rc.SizeTError;
          break;
      case SEEK_CUR:
          too += pos;
          if( (pos>0) && (too < m->h->cursor) )  /* overflow! */ return whio_rc.SizeTError;
          else if( (pos<0) && (too > m->h->cursor) )  /* underflow! */ return whio_rc.SizeTError;
          break;
      default:
          return whio_rc.SizeTError;
          break;
    };
    /** We defer any actual expansion until the next write. */
    return (m->h->cursor = too);
}

static int whio_epfs_inode_iodev_flush( whio_dev * dev )
{
    WHIO_EPFS_DECL(whio_rc.ArgError);
    whio_epfs_inode_flush( m->h->fs, m->ino );
    whio_epfs_flush( m->h->fs );
    return whio_rc.NYIError; // return whio_rc.OK on success
}

static int whio_epfs_inode_iodev_trunc( whio_dev * dev, whio_off_t len )
{
    WHIO_EPFS_DECL(whio_rc.ArgError);
    if( len < 0 ) return whio_rc.ArgError;
    if( ! m->rw ) return whio_rc.AccessError;
    const whio_size_t off = (whio_size_t)len;
    if( off > len ) return whio_rc.RangeError; /* overflow */
    if( off == m->ino->size ) return whio_rc.OK;

    if( 0 == len )
    { /* special (simpler) case for 0 byte truncate */
        if( m->ino->firstBlock ) 
        {
            whio_epfs_block block = whio_epfs_block_empty;
            int rc = whio_epfs_block_read( m->h->fs, m->ino->firstBlock, &block ); // ensure we pick up whole block chain
            if( whio_rc.OK != rc )
            {
                rc = whio_epfs_block_wipe( m->h->fs, &block, true, true, true );
            }
            if( whio_rc.OK != rc ) return rc;
        }
        m->h->blocks.count = 0;
        m->ino->firstBlock = 0;
        m->ino->size = 0;
        whio_epfs_inode_flush(m->h->fs, m->ino);
        return whio_rc.OK;
    }

    // Now hold on and enjoy the ride...
    int rc = whio_rc.OK;
    //const size_t oldSize = off>m->ino->size;
    const short dir = (off < m->ino->size)
	? -1
	: ((off>m->ino->size) ? 1 : 0);
    assert( (0 != off) && "This shouldn't be able to happen!" );

    /* Update inode metadata... */
    //WHEFS_DBG("truncating from %u to %u bytes\n",m->ino->size, off);
    m->ino->size = off;
    rc = whio_epfs_inode_flush( m->h->fs, m->ino );
    if( whio_rc.OK != rc )
    {
	WHIO_DEBUG("Flush failed for inode #%u. Error code=%d.\n",
                   m->ino->id, rc );
	return rc;
    }
    /* Update block info... */
    whio_epfs_block bl = whio_epfs_block_empty;
    rc = whio_epfs_block_for_pos( m->h, off, &bl, true );
    if( whio_rc.OK != rc )
    {
	WHIO_DEBUG("Could not get block for write position %u of inode #%u. Error code=%d.\n",
                   off, m->ino->id, rc );
	return rc;
    }
    if( dir < 0 )
    { /* we shrunk */
#if 1
	/*
	  We'll be nice and zero the remaining bytes... We do this
	  partially for consistency with how blocks will get removed
	  (they get wiped as well).  Theoretically we don't need this
	  because they get wiped when created and when unlinked, but a
	  failed unlink could leave data lying around, so we clean it
	  here. Maybe we should consider a 'dirty' flag for blocks,
	  wiping only dirty blocks, but that could get messy (no pun
	  intended).
	*/
	const uint32_t bs = whio_epfs_options( m->h->fs )->blockSize;
	rc = whio_epfs_block_wipe_data( m->h->fs, &bl, ( off % bs ) );
	if( whio_rc.OK != rc ) return rc;
#endif
	if( ! bl.nextBlock )
	{ /* Lucky for us! No more work to do! */
	    m->h->blocks.count = 1;
	    return whio_rc.OK;
	}
	whio_epfs_block * blP = &m->h->blocks.list[0];
	whio_epfs_block * nblP = blP + 1;
	uint32_t x = 1;
	for( ; (x < m->h->blocks.count)
		 && (nblP->id != bl.nextBlock)
		 ; ++nblP, ++x )
	{
	    /* Skip to bl.nextBlock */
	}
	if( (x == m->h->blocks.count) || (nblP->id != bl.nextBlock) )
	{
	    WHIO_DEBUG("nblP->id=%u, bl.next_block=%u\n", nblP->id, bl.nextBlock );
	    WHIO_DEBUG("Internal block cache for inode #%u is not as "
                       "long as we expect it to be or is missing entries!\n",
                       m->ino->id );
	    return whio_rc.InternalError;
	}
	blP = nblP - 1;
	m->h->blocks.count = x;
	whio_epfs_block_wipe( m->h->fs, nblP, true, true, true );
	blP->nextBlock = 0;
	return whio_epfs_block_flush( m->h->fs, blP );
    }
    else if( dir > 0 )
    { /* we grew - fill the new bytes with zeroes */
	/*
	  Actually... since we zero these when shrinking and during mkfs(),
          we probably don't need to do this.
	*/
	enum { bufSize = 1024 * 4 };
	unsigned char buf[bufSize];
	memset( buf, 0, bufSize );
	const whio_size_t PosAbs = m->h->cursor;
	const whio_size_t orig = m->ino->size;
	const whio_size_t dest = off;
	dev->api->seek( dev, orig, SEEK_SET );
	whio_size_t wlen = dest - orig;
	whio_size_t iorc = 0;
        whio_size_t wsz = 0;
	do
	{
            wsz = (wlen < bufSize) ? wlen : bufSize;
	    iorc = dev->api->write( dev, buf, wsz );
	    wlen -= iorc;
	}
	while( iorc && (iorc == wsz) );
	iorc = dev->api->seek( dev, PosAbs, SEEK_SET );
	return (iorc == PosAbs)
	    ? whio_rc.OK
	    : whio_rc.IOError;
    }
    else
    {
	/* cannot happen due to special-case handling of truncate(0), above. */
	assert( 0 && "This is impossible!" );
    }
    WHIO_DEBUG("You should never have gotten to this line!");
    return whio_rc.InternalError;
}

static int whio_epfs_inode_iodev_ioctl( whio_dev * dev, int arg, va_list vargs )
{
    /**
       TODO (or at least consider):

       If we want cross-process locking to work for an EPFS embedded
       within another EPFS, we need to forward the
       whio_dev_ictl_WHIO_LOCKING ioctl here.  The catch is that the
       lock range could span blocks, so we'd need to analyse the lock
       request and potentially apply it to more than one block.

       The outer-most EPFS container will (once the locking parts
       are done) be locking access to the embedded EPFS as a whole...
       hmmm... if we forward locking requests we could end up unlocking
       something the outer EPFS locked. Except that the outer EPFS doesn't
       lock any of the block _data_ bytes - it only locks its own records.
       That means the inner EPFS could lock without collisions because all
       of its inode/block records live inside data blocks of the outer EPFS.

       Anyway... things to consider...
    */
    WHIO_EPFS_DECL(whio_rc.ArgError);
    switch(arg)
    {
      case whio_epfs_ioctl_INODE_ID: {
          whio_epfs_id_t * v = va_arg(vargs,whio_epfs_id_t*);
          if(v) *v = m->ino->id;
          return v ? 0 : whio_rc.ArgError;
      }
      case whio_epfs_ioctl_INODE_PTR: {
          whio_epfs_inode ** v = va_arg(vargs,whio_epfs_inode **);
          if(v) *v = m->ino;
          return v ? 0 : whio_rc.ArgError;
      }
      case whio_epfs_ioctl_HANDLE_PTR: {
          whio_epfs_handle ** v = va_arg(vargs,whio_epfs_handle **);
          if(v) *v = m->h;
          return v ? 0 : whio_rc.ArgError;
      }
      default:
          break;
    };
    return whio_rc.UnsupportedError;
}

static short whio_epfs_inode_iodev_iomode( whio_dev * dev )
{
    // return: 0=read-only, >0=read/write, <0=unknown/unknowable
    WHIO_EPFS_DECL(-1);
    return m->rw
        ? 1
        : 0;
}

whio_epfs_id_t whio_epfs_dev_inode_id( whio_dev const * dev )
{
    /** Reminder: we don't use whio_epfs_dev_inode_ioctl() here
        so that we can keep dev const.
    */
    WHIO_EPFS_CDECL(0);
    return m->ino->id;
}

static bool whio_epfs_inode_iodev_close( whio_dev * dev )
{
    WHIO_EPFS_DECL(false);
    if( dev->api->iomode(dev) > 0 )
    {
        dev->api->flush( dev );
    }
    // Required by whio_dev_api::close() interface:
    if( dev->client.dtor ) dev->client.dtor( dev->client.data );
    dev->client = whio_client_data_empty;

    if( dev->impl.data )
    {
        whio_epfs_inode_iodev_meta * m = (whio_epfs_inode_iodev_meta*)dev->impl.data;
        whio_epfs * fs = 0;
        fs = m->h->fs;
        whio_epfs_handle_close( m->h );
        if( m->memToFree == (void const *)m )
        {
            void * x = m->memToFree;
            m->memToFree = 0;
            assert( fs && "fs cannot be null at this point!");
            whio_epfs_mfree( fs, x );
        }
        else
        {
            /**
               m->memToFree contains dev and m, and wil be freed
               in whio_epfs_inode_iodev_finalize().
            */
        }
        dev->impl = whio_impl_data_empty;
    }
    return true;
}

static void whio_epfs_inode_iodev_finalize( whio_dev * dev )
{
    WHIO_EPFS_DECL();
    void * mem = m->memToFree;
    whio_epfs * fs = m->h->fs;
    dev->api->close(dev);
    if( mem == (void const *)dev )
    {
        whio_epfs_mfree( fs, mem );
    }
    else
    {
        /* it was freed via close(). */
    }
}

int whio_epfs_dev_client_flags_get( whio_dev const * dev, uint32_t * flags )
{
    WHIO_EPFS_CDECL(whio_rc.ArgError);
    return whio_epfs_inode_client_flags_get( m->ino, flags );
}

int whio_epfs_dev_client_flags_set( whio_dev * dev, uint32_t flags )
{
    WHIO_EPFS_DECL(whio_rc.ArgError);
    return ( ! m->rw )
        ? whio_rc.AccessError
        : whio_epfs_inode_client_flags_set( m->ino, flags );
}

whio_epfs_inode * whio_epfs_dev_inode( whio_dev * dev )
{
    WHIO_EPFS_DECL(NULL);
    return m->rw
        ? m->ino
        : NULL;
}
whio_epfs_inode const * whio_epfs_dev_inode_c( whio_dev const * dev )
{
    WHIO_EPFS_CDECL(NULL);
    return m->ino;
}
int whio_epfs_dev_touch( whio_dev * dev, uint32_t t )
{
    WHIO_EPFS_DECL(whio_rc.ArgError);
    return m->rw
        ? whio_epfs_inode_touch( m->ino, t )
        : whio_rc.AccessError;
}
int whio_epfs_dev_mtime( whio_dev const * dev, uint32_t * time )
{
    WHIO_EPFS_CDECL(whio_rc.ArgError);
    if( time ) *time = m->ino->mtime;
    return whio_rc.OK;
}

#undef WHIO_EPFS_DECL
#undef WHIO_EPFS_CDECL

/**
   The whio_dev_api implementation for pfs.
*/
static const whio_dev_api whio_epfs_inode_iodev_api =
    {
    whio_epfs_inode_iodev_read,
    whio_epfs_inode_iodev_write,
    whio_epfs_inode_iodev_close,
    whio_epfs_inode_iodev_finalize,
    whio_epfs_inode_iodev_error,
    whio_epfs_inode_iodev_clear_error,
    whio_epfs_inode_iodev_eof,
    whio_epfs_inode_iodev_tell,
    whio_epfs_inode_iodev_seek,
    whio_epfs_inode_iodev_flush,
    whio_epfs_inode_iodev_trunc,
    whio_epfs_inode_iodev_ioctl,
    whio_epfs_inode_iodev_iomode
    };


/**
   whio_epfs_inode_iodev initializer object.
*/
static const whio_dev whio_epfs_inode_iodev_empty =
    {
    &whio_epfs_inode_iodev_api,
    { /* impl */
    0, /* data. Must be-a (whio_epfs_inode_iodev*) */
    (void const *)&whio_epfs_inode_iodev_meta_empty /* typeID */
    }
    };

int whio_epfs_dev_create( whio_epfs_handle * h, whio_dev ** dev_ )
{
    if( ! h || !dev_ ) return whio_rc.ArgError;
    typedef whio_epfs_inode_iodev_meta MetaType;
    bool ownsDev = !*dev_;
    const size_t asz = sizeof(MetaType)
        + (ownsDev ? sizeof(whio_dev) : 0);
    unsigned char * mem = (unsigned char *)whio_epfs_malloc(h->fs,asz);
    if( ! mem ) return whio_rc.AllocError;
    whio_dev * dev = ownsDev ? ((whio_dev*)mem) : *dev_;

    // Set up our internal metadata:
    MetaType * meta = (MetaType*)(mem + (ownsDev ? sizeof(whio_dev) : 0));
    *dev = whio_epfs_inode_iodev_empty;
    *dev_ = dev;
    *meta = whio_epfs_inode_iodev_meta_empty;
    dev->impl.data = meta;
    meta->memToFree = mem;
    meta->h = h;
    meta->ino = whio_epfs_handle_inode(h);
    meta->rw = whio_epfs_handle_is_rw(h);
    meta->bs = h->fs->fsopt.blockSize;
    return 0;
}

int whio_epfs_dev_open( whio_epfs * fs,
                      whio_dev ** dev_,
                      whio_epfs_id_t id,
                      enum whio_iomodes mode )
{
    if( ! fs ) return whio_rc.ArgError;
    else if( !(mode & WHIO_MODE_WRITE)
             &&
             (mode & (WHIO_MODE_FLAG_CREATE | WHIO_MODE_FLAG_TRUNCATE)) )
        return whio_rc.ArgError;

    bool ownsDev = !*dev_;
    whio_epfs_handle * h = 0;
    int rc = whio_epfs_handle_open( fs, &h, id, mode );
    if( rc ) return rc;
    rc = whio_epfs_dev_create( h, dev_ );
    if( rc )
    {
        whio_epfs_handle_close( h );
    }
    else if( mode & WHIO_MODE_FLAG_TRUNCATE )
    {
        whio_dev * dev = *dev_;
        /** Truncate can fail (and has failed) on OOM because it causes
            h's block cache to be loaded.

            In the error case, we also need to unlink it if we created
            it.
         */
        rc = dev->api->truncate(dev,0U);
        if( rc )
        { // Try to recover halfway gracefully.
            const whio_epfs_id_t id = whio_epfs_handle_inode(h)->id;
            const bool created =
                (!h->link && (h->openCount==1) && (mode&WHIO_MODE_FLAG_CREATE))
                /** Reminder: ^^^^^^ that logic will (may?) not
                    work(???) in conjunction with the handle recycling
                    code in whio_epfs_handle_open().
                */
                ;
            if( ownsDev )
            {
                dev->api->finalize(dev);
                *dev_ = 0;
            }
            else
            {
                dev->api->close(dev);
            }
            if( created )
            {
                whio_epfs_unlink( fs, id );
            }
        }
    }
    return rc;
}


/* end file pfs/iodev.c */
/* begin file pfs/locking.c */
#line 8 "pfs/locking.c"
/************************************************************************
This file contains the fcntl-style-locking-related parts of whio_epfs.

Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

License: Public Domain
************************************************************************/

//#include <stdio.h> // only for debugging
//#include <stdlib.h> // malloc() and friends.
#include <string.h> // memset() and friends
#include <assert.h>

int whio_epfs_probe_lockability( whio_epfs * fs )
{
#if ! WHIO_EPFS_CONFIG_ENABLE_STORAGE_LOCKING
    return whio_rc.UnsupportedError;
#else
    assert(fs && fs->dev && "Mis-use of function whio_epfs_probe_lockability()!");
    if( ! fs || !fs->dev )
    {
        return whio_rc.ArgError;
    }
    else if( fs->hints.storageLockingMode >= 0 )
    { /* Was already probed. Re-use the result. */
        return fs->hints.storageLockingMode
            ? 0 /* locking was already enabled */
            : whio_rc.UnsupportedError;
    }
    int rc = whio_dev_ioctl( fs->dev, whio_dev_ioctl_mask_WHIO_LOCKING );
#if 0
    if( ! rc )
    {
        // Try to get a lock...
        whio_lock_request wli = whio_lock_request_empty;
        wli.type = whio_epfs_is_rw(fs)
            ? whio_lock_TYPE_WRITE
            : whio_lock_TYPE_READ;
        wli.command = whio_lock_CMD_TEST;
        wli.start = 0;
        wli.length = 0;
        rc = whio_dev_lock( fs->dev, &wli );
        if( rc )
        {
            if( whio_rc.LockingError == rc )
            {
                /* The locking mechanism must fundamentally
                   work if we got this.
                */
                rc = 0;
            }
        }
    }
#endif
    fs->hints.storageLockingMode = (int8_t)(rc ? 0 : 1);
    return rc;
#endif /* WHIO_EPFS_CONFIG_ENABLE_STORAGE_LOCKING */
}

int whio_epfs_storage_lock( whio_epfs * fs, whio_lock_request * req )
{
#if ! WHIO_EPFS_CONFIG_ENABLE_STORAGE_LOCKING
    return whio_rc.UnsupportedError;
#else
    if( ! fs || !req || !fs->dev )
    {
        return whio_rc.ArgError;
    }
    else if( 0 >= fs->hints.storageLockingMode )
    {
        return whio_rc.UnsupportedError;
    }
    else if( (whio_lock_TYPE_WRITE == req->type)
             && ! whio_epfs_is_rw(fs) )
    {
        return whio_rc.AccessError;
    }
    return whio_dev_lock( fs->dev, req );
#endif
}

int whio_epfs_storage_lock_all( whio_epfs * fs, bool wait )
{
#if ! WHIO_EPFS_CONFIG_ENABLE_STORAGE_LOCKING
    return whio_rc.UnsupportedError;
#else
    if( ! fs ) return whio_rc.ArgError;
    else if( 0 >= fs->hints.storageLockingMode )
    {
        return whio_rc.UnsupportedError;
    }
    whio_lock_request wli = whio_lock_request_empty;
    wli.type = whio_epfs_is_rw(fs)
        ? whio_lock_TYPE_WRITE
        : whio_lock_TYPE_READ;
    wli.command = wait
        ? whio_lock_CMD_SET_WAIT
        : whio_lock_CMD_SET_NOWAIT;
    return whio_epfs_storage_lock( fs, &wli );
#endif
}
int whio_epfs_storage_unlock_all( whio_epfs * fs )
{
#if ! WHIO_EPFS_CONFIG_ENABLE_STORAGE_LOCKING
    return whio_rc.UnsupportedError;
#else
    whio_lock_request wli = whio_lock_request_set_u;
    return whio_epfs_storage_lock( fs, &wli );
#endif
}
/**
   Internal impl for whio_epfs_storage_[un]lock().

   If setLock is true then it performs write lock (if isWriteLock is
   true) or read lock.

   If setLock is true: if wait is true then F_SETLKW is used, else
   F_SETLK is used.

   If setLock is false then F_SETLK[W] is used with F_UNLCK to unlock
   a prior lock.

   start and length are coordinates relative to the start of fs->dev.

   Returns 0 on succcess. If built without
   WHIO_EPFS_CONFIG_ENABLE_STORAGE_LOCKING then it returns
   whio_rc.UnsupportedError.

   If this function returns -1 (which isn't used by whio_rc), it comes
   from deep in whio_dev_ioctl() call, and almost certainly from
   fcntl(). In that case, the cause of the error is likely set in the
   global errno.

   IN THEORY it is OK for fs->dev to be-a whio_dev subdevice, but
   that type's fcntl forwarding support is untested.
*/
static int whio_epfs_storage_lock2_impl( whio_epfs * fs,
                                        bool setLock,
                                        bool isWriteLock,
                                        whio_size_t start,
                                        whio_size_t length,
                                        bool wait )
{
#if ! WHIO_EPFS_CONFIG_ENABLE_STORAGE_LOCKING
    return whio_rc.UnsupportedError;
#else
    if( ! fs || !fs->dev )
    {
        return whio_rc.ArgError;
    }
    else if( 0 == fs->hints.storageLockingMode )
    {
        return whio_rc.UnsupportedError;
    }
    else if( setLock && isWriteLock && ! whio_epfs_is_rw(fs) )
    {
        return whio_rc.AccessError;
    }
    whio_lock_request wli = whio_lock_request_empty;
    wli.type = !setLock
        ? whio_lock_TYPE_UNLOCK
        : (isWriteLock ?  whio_lock_TYPE_WRITE : whio_lock_TYPE_READ)
        ;
    wli.command = wait ? whio_lock_CMD_SET_WAIT : whio_lock_CMD_SET_NOWAIT;
    //wli.setLock = setLock;
    //wli.waitForLock = wait;
    wli.start = start;
    wli.length = length;
    if(0)
    {
        //WHIO_DEBUG
        printf("Locking: cmd=%d, type=%d, start=%"WHIO_OFF_T_PFMT", len=%"WHIO_OFF_T_PFMT"\n",
               wli.command, wli.type, wli.start, wli.length );
    }
    return whio_epfs_storage_lock( fs, &wli );
#endif
}

int whio_epfs_storage_lock2( whio_epfs * fs,
                            bool isWriteLock,
                            whio_size_t start,
                            whio_size_t length,
                            bool wait )
{
    return whio_epfs_storage_lock2_impl( fs,
                                        true,
                                        isWriteLock,
                                        start,
                                        length,
                                        wait );
}

int whio_epfs_storage_unlock2( whio_epfs * fs,
                              bool isWriteLock,
                              whio_size_t start,
                              whio_size_t length,
                              bool wait )
{
    return whio_epfs_storage_lock2_impl( fs,
                                        false,
                                        isWriteLock,
                                        start,
                                        length,
                                        wait );
}


int whio_epfs_inode_lock( whio_epfs * fs, whio_epfs_id_t id, bool writeLock, bool wait )
{
    const whio_size_t pos = whio_epfs_inode_id_pos( fs, id );
    return ( ! pos )
        ? whio_rc.ArgError
        : whio_epfs_storage_lock2( fs, writeLock, pos, whio_epfs_sizeof_inode, wait )
        ;
}

int whio_epfs_inode_unlock( whio_epfs * fs, whio_epfs_id_t id, bool writeLock, bool wait )
{
    const whio_size_t pos = whio_epfs_inode_id_pos( fs, id );
    return ( ! pos )
        ? whio_rc.ArgError
        : whio_epfs_storage_unlock2( fs, writeLock, pos, whio_epfs_sizeof_inode, wait )
        ;
}

int whio_epfs_block_lock( whio_epfs * fs, whio_epfs_id_t id, bool writeLock, bool wait )
{
    const whio_size_t pos = whio_epfs_block_meta_pos( fs, id );
    return ( ! pos )
        ? whio_rc.ArgError
        : whio_epfs_storage_lock2( fs, writeLock, pos, whio_epfs_sizeof_blockMeta, wait )
        ;
}

int whio_epfs_block_unlock( whio_epfs * fs, whio_epfs_id_t id, bool writeLock, bool wait )
{
    const whio_size_t pos = whio_epfs_block_meta_pos( fs, id );
    return ( ! pos )
        ? whio_rc.ArgError
        : whio_epfs_storage_unlock2( fs, writeLock, pos, whio_epfs_sizeof_blockMeta, wait )
        ;
}

int whio_epfs_magic_lock( whio_epfs * fs, bool writeLock, bool wait )
{
    return !fs
        ? whio_rc.ArgError
        : whio_epfs_storage_lock2( fs, writeLock,
                                   fs->offsets[whio_epfs_index_magic],
                                   fs->sizes[whio_epfs_index_magic],
                                   wait )
        ;
}

int whio_epfs_magic_unlock( whio_epfs * fs, bool writeLock, bool wait )
{
    return !fs
        ? whio_rc.ArgError
        : whio_epfs_storage_unlock2( fs, writeLock,
                                     fs->offsets[whio_epfs_index_magic],
                                     fs->sizes[whio_epfs_index_magic],
                                     wait )
        ;
}
/* end file pfs/locking.c */
/* begin file pfs/memory.c */
#line 8 "pfs/memory.c"
/************************************************************************
This file contains parts of whio_epfs dealing with the internal memory
allocation.

Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

License: Public Domain
************************************************************************/

//#define WHIO_DEBUG_ENABLED 1

//#include <stdio.h> // only for debugging
#include <stdlib.h> // malloc() and friends.
#include <string.h> // memset() and friends

#include <assert.h>

#if WHIO_EPFS_CONFIG_ENABLE_MEMPOOL
#endif

#define MARKER printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); printf

#if WHIO_EPFS_CONFIG_ENABLE_MEMPOOL
#define PFS_POOL(FS) ((WHALLOC_API(bt)*)(FS)->pool.mem)
static int alloc_loggerv( char const * fmt, va_list varg )
{
    return vfprintf( stderr, fmt, varg );
}
static int alloc_logger( char const * fmt, ... )
{
    int rc;
    va_list varg;
    va_start(varg,fmt);
    rc = alloc_loggerv( fmt, varg );
    va_end(varg);
    return rc;
}
#endif

int whio_epfs_mempool_drain( whio_epfs * fs )
{
#if ! WHIO_EPFS_CONFIG_ENABLE_MEMPOOL
    return whio_rc.UnsupportedError;
#else
    if( fs && fs->pool.mem )
    {
        WHALLOC_API(bt_drain)( (WHALLOC_API(bt)*)fs->pool.mem );
        return 0;
    }
    else return whio_rc.ArgError;
#endif
}

#if WHIO_EPFS_CONFIG_ENABLE_MEMPOOL
/** Implements the whio_allocator interface and treats state as a
    (WHALLOC_API(bt)*) object.
*/
static void * whio_epfs_allocator_bt( void * m, unsigned int sz, void * state )
{
    return WHALLOC_API(bt_realloc)( (WHALLOC_API(bt) *)state, m, sz );
}
#endif

int whio_epfs_mempool_setup( whio_epfs * fs, void * mem, whio_size_t size, bool fallback )
{
#if ! WHIO_EPFS_CONFIG_ENABLE_MEMPOOL
    return whio_rc.UnsupportedError;
#else
    if( ! fs ) return whio_rc.ArgError;
    if( fs->pool.mem ) return whio_rc.AccessError;
    size_t const btSz = sizeof(WHALLOC_API(bt));
    if( size < (btSz*2) ) return whio_rc.RangeError;
    const whalloc_size_t minBlockSize = 24;
    const whalloc_size_t blockSize =
        (WHIO_EPFS_CONFIG_MEMPOOL_BLOCKSIZE < minBlockSize)
        ? minBlockSize
        : WHIO_EPFS_CONFIG_MEMPOOL_BLOCKSIZE;
    fs->pool.mem = (unsigned char *)mem;
    fs->pool.size = size;
    WHALLOC_API(bt) * p = (WHALLOC_API(bt)*)fs->pool.mem;
    *p = WHALLOC_API(bt_empty);
    int rc = WHALLOC_API(bt_init)(p, fs->pool.mem + btSz, size - btSz, blockSize);
    if( rc )
    {
        return whio_rc.AllocError;
    }
    p->base.log = alloc_logger; // avoid "static function never used" warning
    p->base.log = 0; // but we don't want to log by default
    if( fallback )
    {
        p->base.fallback = whalloc_fallback_stdalloc;
    }
    fs->alloc.realloc = whio_epfs_allocator_bt;
    fs->alloc.state = p;
    return 0;
#endif
}

int whio_epfs_mempool_stats( whio_epfs const * fs, whio_epfs_mempool_stats_t * dest )
{
#if !WHIO_EPFS_CONFIG_ENABLE_MEMPOOL
    return whio_rc.UnsupportedError;
#else
    if( ! fs || ! dest ) return whio_rc.ArgError;
    else if( ! fs->pool.mem ) return whio_rc.UnsupportedError;
    else
    {
        memset( dest, 0, sizeof(whio_epfs_mempool_stats_t) );
        WHALLOC_API(bt) * p = PFS_POOL(fs);
        dest->allocedObjects = p->base.allocCount;
        dest->allocedBlocks = p->base.allocBlockCount;
        dest->blockCount = p->base.blockCount;
        dest->blockSize = p->base.blockSize;
        dest->memorySize = p->base.usize;
        return whio_rc.OK;
    }
#endif
}

int whio_epfs_mempool_dump( whio_epfs * fs, FILE * dest, char const * prefix, bool full )
{
#if ! WHIO_EPFS_CONFIG_ENABLE_MEMPOOL
    return whio_rc.UnsupportedError;
#else
    if( ! fs || ! dest ) return whio_rc.ArgError;
    WHALLOC_API(bt) * p = PFS_POOL(fs);
    fprintf(dest, "%s: whefs_fs@%p:\n",(prefix? prefix:__func__),(void const *)fs);
    if( ! p )
    {
        fputs("Memory pool is not installed. Use whio_epfs_mempool_setup() to install it (and RTFM first!).\n",dest);
        return 0;
    }
    else
    {
        if( full )
        {
            WHALLOC_API(bt_dump_debug)( p, dest );
        }
        else
        {
            fprintf(dest, "Memory pool contains %"WHALLOC_SIZE_T_PFMT
                    " item(s) in %"WHALLOC_SIZE_T_PFMT" block(s) taking up %"WHALLOC_SIZE_T_PFMT
                    " byte(s).\n",
                    p->base.allocCount, p->base.allocBlockCount, (p->base.allocBlockCount * p->base.blockSize) );
        }
        whio_epfs_mempool_stats_t st;
        int rc = whio_epfs_mempool_stats( fs, &st );
        if( rc ) return rc;
        fprintf( dest, "Overall status:\n");
#define X(KEY) fprintf( dest,"\t%s = %"WHIO_SIZE_T_PFMT"\n", #KEY, st.KEY)
        X(memorySize);
        X(allocedObjects);
        X(allocedBlocks);
        X(blockCount);
        X(blockSize);
#undef X
        fprintf( dest, "Current usage: %3.2lf%%\n",
               (1.0*st.allocedBlocks) / st.blockCount * 100
               );
        return 0;
    }
#endif
}



void * whio_epfs_mrealloc( whio_epfs * fs, void * mem, whio_size_t n )
{
    if( !fs ) return NULL;
    else if( fs->alloc.realloc )
    {
        return fs->alloc.realloc( mem, n, fs->alloc.state );
    }
    else
    {
        return realloc( mem, n );
    }
}

void * whio_epfs_malloc( whio_epfs * fs, whio_size_t n )
{
    return whio_epfs_mrealloc( fs, NULL, n );
}

int whio_epfs_mfree( whio_epfs * fs, void * m )
{
    //void const * X =
    whio_epfs_mrealloc( fs, m, 0 );
    //return X ? whio_rc.InternalError : 0;
    return 0;
    /** my man pages say:

    ---
    realloc() returns a pointer to the newly allocated memory, which
    is suitably aligned for any kind of variable and may be different
    from ptr, or NULL if the request fails.  If size was equal to 0,
    either NULL or a pointer suitable to be passed to free() is
    returned.  If realloc() fails the original block is left
    untouched; it is not freed or moved.
    ---

    The point being that realloc() need not return NULL for a
    deallocation. It may return some value "suitable to be passed to
    free()".
    */
}

#if defined(PFS_POOL)
#  undef PFS_POOL
#endif

#undef MARKER
/* end file pfs/memory.c */
/* begin file pfs/pfs.c */
#line 8 "pfs/pfs.c"
/************************************************************************
This is a work-in-progress, porting over parts of the whefs API into
the whio API...

Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

License: Public Domain
************************************************************************/


#include <assert.h>
#include <stdlib.h> // malloc() and friends.
#include <string.h> // memset() and friends
#include <ctype.h> // isxdigit()
//#include <stdio.h> // only for debugging

const whio_epfs_inode whio_epfs_inode_empty = whio_epfs_inode_empty_m;
const whio_epfs_fsopt whio_epfs_fsopt_empty = whio_epfs_fsopt_empty_m;
const whio_epfs_handle_list whio_epfs_handle_list_empty = whio_epfs_handle_list_empty_m;
const whio_epfs whio_epfs_empty = whio_epfs_empty_m;
const whio_epfs_handle whio_epfs_handle_empty = whio_epfs_handle_empty_m;
const whio_epfs_namer whio_epfs_namer_empty = whio_epfs_namer_empty_m;
const whio_epfs_setup_opt whio_epfs_setup_opt_empty = whio_epfs_setup_opt_empty_m;

static void * whio_epfs_allocator_std_realloc( void * m, size_t n, void * data )
{
    return realloc( m, n );
}

const whio_epfs_allocator whio_epfs_allocator_stdalloc = {
whio_epfs_allocator_std_realloc,
NULL
};

#define MARKER printf("MARKER: %s:%d:%s():\t",__FILE__,__LINE__,__func__); printf

const uint16_t whio_epfs_magic_bytes[] =
    {
    2010,
    3,
    10,
    WHIO_SIZE_T_BITS,
    WHIO_EPFS_ID_T_BITS,
    whio_epfs_sizeof_label_payload,
    0 };

/** Tag marking the beginning of the EFS storage. */
static const unsigned char whio_epfs_magic_tag_char = 'P';
/** Tag marking the beginning of whio_epfs::fsopt storage. */
static const unsigned char whio_epfs_fsopt_tag_char = 'O';
/** Tag marking the beginning of whio_epfs::hints storage. */
static const unsigned char whio_epfs_hints_tag_char = 'H';
/** Tag marking the beginning of EFS label storage. */
static const unsigned char whio_epfs_label_tag_char = 'L';

int whio_epfs_flush( whio_epfs * fs )
{
    if(!fs || !fs->dev ) return whio_rc.ArgError;
    else if( !whio_epfs_is_rw(fs) ) return whio_rc.AccessError;
    else
    {
        /* FIXME? put this somewhere else? */
        int rc =
#if 1
            0
#else
            whio_epfs_hints_write(fs)
#endif
            ;
        if(! rc )
        {
            rc = fs->dev->api->flush( fs->dev );
        }
        return rc;
    }
            
}

whio_epfs_fsopt const * whio_epfs_options(whio_epfs const * fs)
{
    return fs
        ? &fs->fsopt
        : 0;
}

whio_size_t whio_epfs_encode_id( unsigned char * dest, whio_epfs_id_t val )
{
#if WHIO_EPFS_ID_T_BITS == 8
    return whio_encode_uint8( dest, val );
#elif WHIO_EPFS_ID_T_BITS == 16
    return whio_encode_uint16( dest, val );
#elif WHIO_EPFS_ID_T_BITS == 32
    return whio_encode_uint32( dest, val );
#elif WHIO_EPFS_ID_T_BITS == 64
    return whio_encode_uint64( dest, val );
#else
#  error "WHIO_EPFS_ID_T_BITS must be one of: 8, 16, 32, 64"
    return fix_me;
#endif
}
int whio_epfs_decode_id( unsigned char const * src, whio_epfs_id_t * val )
{
#if WHIO_EPFS_ID_T_BITS == 8
    return whio_decode_uint8( src, val );
#elif WHIO_EPFS_ID_T_BITS == 16
    return whio_decode_uint16( src, val );
#elif WHIO_EPFS_ID_T_BITS == 32
    return whio_decode_uint32( src, val );
#elif WHIO_EPFS_ID_T_BITS == 64
    return whio_decode_uint64( src, val );
#else
#  error "WHIO_EPFS_ID_T_BITS must be one of: 8, 16, 32, 64"
    return fix_me;
#endif
}

int whio_epfs_clearerr( whio_epfs * fs )
{
    if( ! fs ) return whio_rc.ArgError;
    else
    {
        int rc = fs->err;
        fs->err = 0;
        return rc;
    }
}

whio_epfs_id_t whio_epfs_block_count( whio_epfs const * fs )
{
    return fs ? fs->hints.blockCount : 0;
}

whio_size_t whio_epfs_write( whio_epfs * fs, void const * src, whio_size_t n )
{
    if( ! fs || ! src || !n ) return 0;
    else if( ! whio_epfs_is_rw(fs) ) return 0;
    else return fs->dev->api->write( fs->dev, src, n );
}

whio_size_t whio_epfs_seek( whio_epfs * fs, whio_size_t pos )
{
    return (fs && fs->dev)
        ? fs->dev->api->seek( fs->dev, pos, SEEK_SET )
        : whio_rc.SizeTError;
}

whio_size_t whio_epfs_writeat( whio_epfs * fs, whio_size_t pos,
                              void const * src,
                              whio_size_t n )
{
    if( ! fs || ! src || !n ) return 0;
    else if( ! whio_epfs_is_rw(fs) ) return 0;
    else
    {
        whio_size_t rc = fs->dev->api->seek( fs->dev, pos, SEEK_SET );
        return ( rc != pos )
            ? ((fs->err = whio_rc.IOError),0)
            : whio_epfs_write( fs, src, n );
    }
}

whio_size_t whio_epfs_read( whio_epfs * fs, void * dest, whio_size_t n )
{
    return fs->dev->api->read( fs->dev, dest, n );
}

whio_size_t whio_epfs_readat( whio_epfs * fs, whio_size_t pos,
                             void * dest,
                             whio_size_t n )
{
    whio_size_t rc = fs->dev->api->seek( fs->dev, pos, SEEK_SET );
    return ( rc != pos )
        ? ((fs->err = whio_rc.IOError),0)
        : whio_epfs_read( fs, dest, n );
}
                             
whio_epfs * whio_epfs_alloc()
{
    whio_epfs * x = (whio_epfs *)whio_malloc(sizeof(whio_epfs));
    if( x )
    {
        *x = whio_epfs_empty;
        x->hints.allocStamp = &whio_epfs_empty;
    }
    return x;
}
int whio_epfs_free( whio_epfs * fs )
{
    if( ! fs )
    {
        return whio_rc.ArgError;
    }
    else
    {
        void const * stamp = fs->hints.allocStamp;
        *fs = whio_epfs_empty;
        if( &whio_epfs_empty == stamp )
        {
            whio_free( fs );
        }
        else
        {
            // Assume it was allocated somewhere else.
            fs->hints.allocStamp = stamp;
        }
        return 0;
    }
}
bool whio_epfs_is_rw( whio_epfs const * const fs )
{
    return fs && (fs->flags & WHIO_EPFS_FLAG_RW);
}
int whio_epfs_client_data_set( whio_epfs * fs, void * data, void (*dtor)(void *) )
{
    if( ! fs ) return whio_rc.ArgError;
    else
    {
        fs->client.dtor = dtor;
        fs->client.data = data;
        return 0;
    }
}
void * whio_epfs_client_data_get( whio_epfs * fs )
{
    return fs ? fs->client.data : NULL;
}
whio_client_data * whio_epfs_client_data( whio_epfs * fs )
{
    return fs ? &fs->client : NULL;
}

int whio_epfs_close( whio_epfs *fs )
{
    if( !fs ) return whio_rc.ArgError;
    if( !fs->err && fs->dev && whio_epfs_is_rw(fs) )
    {
        whio_epfs_hints_write(fs);
        whio_epfs_flush(fs);
    }
    if( fs->client.dtor )
    {
        /**
           Reminder: we have to close this early in case the client
           dtor indirectly uses fs.
        */
        fs->client.dtor( fs->client.data );
        fs->client = whio_client_data_empty;
    }
    if( fs->handles.alloced )
    {
        /** The whio_epfs_handle::link bits give us some headaches
            here...
        */
        while( fs->handles.count )
        {
            /**
               Reminder: i don't like this. It is only safe so long as
               fs->handles.list is never realloced during
               whio_epfs_handle_close(). i can't imagine why it would
               be, but i need to make certain that it never does.

               i'd like to refactor the handles to use a inode pointer
               instead of a whole object, as that would eliminate much
               of the handle-linking hassle.
            */
            whio_epfs_handle_list lcp = fs->handles;
            whio_epfs_id_t i;
            int rc = 0;
            for( i = 0; i < lcp.count; ++i )
            {
                //WHIO_DEBUG("Closing whio_epfs_handle @%p.\n", (void const *)lcp.list[i]);
                rc = whio_epfs_handle_close( lcp.list[i] );
                if( whio_rc.OK != rc )
                {
                    /**
                       Reminder: we cannot bail out here because that would leave
                       fs in an undefined state (e.g. fs->client is already gone
                       and the handle list possibly partially cleaned up).
                       If the internal memory allocator is used, any memory associated
                       with a failed-closed object will be cleaned up indirectly
                       when that pool is cleaned up below.
                    */
                    WHIO_DEBUG("Error closing whio_epfs.handle @%p while closing EPFS!\n", (void const *)lcp.list[i]);
                }
            }
        }
        whio_epfs_handle_list_reserve( fs, &fs->handles, 0 );
    }
    if( fs->dev )
    {
        if( !fs->err && whio_epfs_is_rw(fs) )
        {
            fs->dev->api->flush(fs->dev);
        }
        //whio_epfs_storage_unlock_all( fs ) /*ignoring error code.*/;
        /* Reminder: no need to explcitely unlock fs->dev: closing the
           device will, at least for fcntl locks, release all locks.
        */
        if( fs->flags & WHIO_EPFS_FLAG_FS_OWNS_DEV )
        {
            fs->dev->api->finalize(fs->dev);
        }
        fs->dev = 0;
    }
    whio_epfs_mempool_drain( fs )/*ignoring error code*/; /* this needs to go away, in favour of the newer allocation abstraction. */
    {
        void const * allocStamp = fs->hints.allocStamp;
        *fs = whio_epfs_empty;
        fs->hints.allocStamp = allocStamp;
    }
    return whio_rc.OK;
}

int whio_epfs_finalize( whio_epfs *fs )
{
    int rc = whio_epfs_close( fs );
    if( whio_rc.OK == rc )
    {
        whio_epfs_free(fs);
        /** ^^^ ignoring return val so we don't open up the question
            of ownership if close() succeeds but free() does not.  The
            caller doesn't have enough info to be able to make that
            distinction.

            [Much later]: whio_epfs_free() only fails if !fs (can't
            happen here). It arguably should fail if
            fs->hints.allocStamp is not from whio_epfs_alloc(), but
            i'm still not certain about making that change.
        */
    }
    return rc;
}

static int whio_epfs_init_offsz( whio_epfs * fs )
{
#define OKEY(K) whio_epfs_index_##K
#define FOFF(K) fs->offsets[OKEY(K)]
#define OFF(K,V) FOFF(K) = V
#define FSZ(K) fs->sizes[OKEY(K)]
#define SZ(K) FSZ(K) = whio_epfs_sizeof_##K
    SZ(magic);
    SZ(fsopt);
    SZ(hints);
    SZ(label);
    SZ(inode);
    SZ(blockMeta);
    OFF(magic,0);
    OFF(fsopt,FOFF(magic)+FSZ(magic));
    OFF(hints,FOFF(fsopt)+FSZ(fsopt));
    OFF(label,FOFF(hints)+FSZ(hints));
    OFF(inode,FOFF(label)+FSZ(label));
    OFF(blockMeta,FOFF(inode)+(FSZ(inode)*fs->fsopt.inodeCount));

#if 0
#define P(K) WHIO_DEBUG("fs->offsets["#K"]=%u\tf->sizes["#K"]=%u\n",FOFF(K),FSZ(K))
    P(magic);
    P(fsopt);
    P(hints);
    P(inode);
    P(blockMeta);
    WHIO_DEBUG("* %"WHIO_EPFS_ID_T_PFMT" inodes\n",fs->fsopt.inodeCount);
#undef P
#endif

#undef OKEY
#undef OFF
#undef FOFF
#undef FSZ
#undef SZ
    return whio_rc.OK;
}

/**
   Writes magic bytes to fs->dev at offset 0.
*/
static int whio_epfs_magic_write( whio_epfs * fs )
{
    enum { Len = whio_epfs_sizeof_magic };
    unsigned char buf[Len];
    //memset(buf, 0, Len);
    unsigned char * dest = buf;
    *(dest++) = whio_epfs_magic_tag_char;
    whio_size_t sz = 1;
    whio_size_t rc = 0;
    uint16_t const * m = whio_epfs_magic_bytes;
    for( ; *m; ++m )
    {
        rc = whio_encode_uint16( dest, *m );
        sz += rc;
        dest += rc;
    }
    return (sz == whio_epfs_writeat(fs, fs->offsets[whio_epfs_index_magic],
                                   buf, sz ))
        ? 0
        : (fs->err = whio_rc.IOError)
        ;
}

/**
   Reads fs->sizes[whio_epfs_index_magic] bytes of data from
   fs->offsets[whio_epfs_index_magic] and tries to confirm
   that they match the whio_epfs_magic_bytes.

   Returns 0 on success.
*/
static int whio_epfs_magic_read( whio_epfs * fs )
{
    assert( fs && fs->sizes[whio_epfs_index_magic] && "Premature use of whio_epfs_magic_read()!" );
    if( ! fs ) return whio_rc.ArgError;
    uint16_t val;
    uint16_t const * m = whio_epfs_magic_bytes;
    enum { Len = whio_epfs_sizeof_magic };
    unsigned char buf[Len];
    memset(buf,0,Len);
    if( Len != whio_epfs_readat( fs, fs->offsets[whio_epfs_index_magic], buf, Len ) )
    {
        return whio_rc.IOError;
    }
    if( *buf != whio_epfs_magic_tag_char )
    {
        return whio_rc.ConsistencyError;
    }
    unsigned char * at = buf+1;
    int rc = 0;
    for( ; *m; ++m, at += whio_sizeof_encoded_uint16 )
    {
        val = 0;
        rc = whio_decode_uint16( at, &val );
        if( rc ) break;
        if( *m != val )
        {
            return whio_rc.ConsistencyError;
        }
    }
    return rc;
}

/**
   Writes whio_epfs_sizeof_label bytes at position fs->offsets[whio_epfs_index_label]
   of fs->dev.

   The bytes are:

   - If (lbl) then it is used. n must be greater than 0 and less than
   or equal to whio_epfs_sizeof_label. If n is smaller than
   whio_epfs_sizeof_label then the label is padded with nulls.

   - If !lbl then an empty label written, filled with nulls, and n is
   ignored.

   Returns 0 on success.
*/
static int whio_epfs_label_write( whio_epfs * fs, char const * lbl, whio_size_t n )
{
    if( ! fs || !fs->dev ) return whio_rc.ArgError;
    else if( lbl && (n > whio_epfs_sizeof_label_payload) ) return whio_rc.RangeError;
    else if( !whio_epfs_is_rw(fs) )
    {
        return whio_rc.AccessError;
    }
    enum { Len = whio_epfs_sizeof_label };
    unsigned char buf[Len];
    //memset(buf, 0, Len);
    unsigned char * dest = buf;
    *(dest++) = whio_epfs_label_tag_char;
    if( lbl && n )
    {
        memcpy( dest, lbl, n );
        memset( dest+n, 0, whio_epfs_sizeof_label_payload - n );
    }
    else
    {
        memset( dest, 0, whio_epfs_sizeof_label_payload );
    }
    return (Len == whio_epfs_writeat(fs, fs->offsets[whio_epfs_index_label],
                                     buf, (whio_size_t)Len ))
        ? 0
        : (fs->err = whio_rc.IOError)
        ;
}

/**
   Reads whio_epfs_sizeof_label bytes from position
   fs->offsets[whio_epfs_index_label] in fs->dev and copies all but
   the first byte (a tag byte) to tgt. tgt must be valid memory at
   least whio_epfs_sizeof_label_payload bytes long, and on success
   exactly whio_epfs_sizeof_label_payload bytes will be copied to it.
   On failure tgt will not be modified.

   Returns 0 on success.

   Errors include:

   - either arg is null: whio_rc.ArgError

   - Read error: whio_rc.IOError

   - Read succeeded but data is not what we expected:
   whio_rc.ConsistencyError
*/
static int whio_epfs_label_read( whio_epfs * fs, char * tgt )
{
    if( ! fs
        || !fs->dev
        || !tgt )
    {
        return whio_rc.ArgError;
    }
    enum { Len = whio_epfs_sizeof_label };
    unsigned char buf[Len];
    memset( buf, 0, Len );
    if( (Len != whio_epfs_readat(fs, fs->offsets[whio_epfs_index_label],
                                  buf, (whio_size_t)Len ) ) )
    {
        return whio_rc.IOError;
    }
    if( *buf != whio_epfs_label_tag_char )
    {
        return whio_rc.ConsistencyError;
    }
    memcpy( tgt, buf+1/*tag byte*/, whio_epfs_sizeof_label_payload );
    return whio_rc.OK;
}

int whio_epfs_label_get( whio_epfs * fs, char * lbl )
{
    return whio_epfs_label_read( fs, lbl );
}

int whio_epfs_label_set( whio_epfs * fs, char const * lbl, whio_size_t n )
{
    return whio_epfs_label_write( fs, lbl, n );
}


/**
   Writes fs->fsopt to position fs->offsets[whio_epfs_index_fsopt]
   of fs->dev. Returns 0 on success.
*/
static int whio_epfs_fsopt_write( whio_epfs * fs )
{
    enum { Len = whio_epfs_sizeof_fsopt };
    unsigned char buf[Len];
    memset(buf, 0, Len);
    unsigned char * dest = buf;
    *(dest++) = whio_epfs_fsopt_tag_char;
    whio_size_t sz = 1;
    whio_size_t rc = 0;
    rc = whio_epfs_encode_id( dest, fs->fsopt.inodeCount );
    if( rc != whio_epfs_sizeof_id ) return whio_rc.InternalError;
    dest += rc;
    sz += rc;
    rc = whio_epfs_encode_id( dest, fs->fsopt.maxBlocks );
    if( rc != whio_epfs_sizeof_id ) return whio_rc.InternalError;
    dest += rc;
    sz += rc;
    rc = whio_encode_size_t( dest, fs->fsopt.blockSize );
    if( rc != whio_sizeof_encoded_size_t ) return whio_rc.InternalError;
    sz += rc;
    return (sz == whio_epfs_writeat(fs, fs->offsets[whio_epfs_index_fsopt],
                                   buf, sz ))
        ? 0
        : (fs->err = whio_rc.IOError)
        ;
}

int whio_epfs_hints_write( whio_epfs * fs )
{
    /**
       TODO:

       once locking is in place, write a "change number" byte to the
       hints. If the number is not the last change number we wrote,
       assume another process has changed the hints, and re-load them
       here instead of writing them.

       FIXME: don't write them if we don't have to, to avoid
       unnecessarily updating the storage mtime.
    */
    enum { Len = whio_epfs_sizeof_hints };
    unsigned char buf[Len];
    memset(buf, 0, Len);
    unsigned char * dest = buf;
    *(dest++) = whio_epfs_hints_tag_char;
    whio_size_t sz = 1;
    whio_size_t rc = 0;

    rc = whio_epfs_encode_id( dest, fs->hints.maxEverUsedInode );
    dest += rc;
    sz += rc;

    rc = whio_epfs_encode_id( dest, fs->hints.maxEverUsedBlock );
    dest += rc;
    sz += rc;
    
    rc = whio_epfs_encode_id( dest, fs->hints.freeInodeList );
    dest += rc;
    sz += rc;

    rc = whio_epfs_encode_id( dest, fs->hints.freeBlockList );
    dest += rc;
    sz += rc;

    rc = whio_epfs_encode_id( dest, fs->hints.blockCount );
    dest += rc;
    sz += rc;

    if(  sz != fs->sizes[whio_epfs_index_hints] )
    {
        assert( 0 && "EFPS corruption detected." );
        return whio_rc.ConsistencyError;
    }
    return (sz == whio_epfs_writeat(fs, fs->offsets[whio_epfs_index_hints],
                                   buf, sz ))
        ? 0
        : (fs->err = whio_rc.IOError)
        ;
}
/**
   Reads fs->hints from fs->offsets[whio_epfs_index_hints].
   Returns 0 on success.

   fs->offsets must be populated before this is called.
*/
static int whio_epfs_hints_read( whio_epfs * fs )
{
    enum { Len = whio_epfs_sizeof_hints };
    unsigned char buf[Len];
    memset(buf,0,Len);
    if( Len != whio_epfs_readat( fs, fs->offsets[whio_epfs_index_hints], buf, Len ) )
    {
        return whio_rc.IOError;
    }
    if( *buf != whio_epfs_hints_tag_char )
    {
        return whio_rc.ConsistencyError;
    }
    unsigned char * at = buf+1;
    int rc = 0;

    rc = whio_epfs_decode_id( at, &fs->hints.maxEverUsedInode );
    at += whio_epfs_sizeof_id;
    if( rc ) return rc;

    rc = whio_epfs_decode_id( at, &fs->hints.maxEverUsedBlock );
    at += whio_epfs_sizeof_id;
    if( rc ) return rc;
    
    rc = whio_epfs_decode_id( at, &fs->hints.freeInodeList );
    at += whio_epfs_sizeof_id;
    if( rc ) return rc;

    rc = whio_epfs_decode_id( at, &fs->hints.freeBlockList );
    at += whio_epfs_sizeof_id;
    if( rc ) return rc;
    
    rc = whio_epfs_decode_id( at, &fs->hints.blockCount );
    at += whio_epfs_sizeof_id;
    if( rc ) return rc;
    return rc;
}

static int whio_epfs_inodes_table_write( whio_epfs * fs )
{
    whio_epfs_inode ino = whio_epfs_inode_empty;
    int rc = 0;
    whio_size_t i = 1;
    fs->hints.freeInodeList = i;
    for( ; (!rc) && (i <= fs->fsopt.inodeCount); ++i )
    {
        ino.id = i;
        ino.nextFree = (i < fs->fsopt.inodeCount) ? (i+1) : 0;
        ino.prevFree = i-1;
        rc = whio_epfs_inode_flush( fs, &ino );
    }
    return rc;
}

/** @internal
   Internal code-duplication remover. It assumes fs is fully (but just
   recently) initialized, and that sopt is properly set up. It also
   assumes that initialization of fs is at its end phase, and the fs
   object has already succeeded through initialization except for these
   house-keeping bits.

   If sopt->memory.mem is not null then whio_epfs_mempool_setup() is
   called and on success sopt->memory.mem is set to NULL. If mempool
   initialization fails, that error code is returned from this
   function.

   If sopt->storage.takeDevOnSuccess is true then fs has its
   owns-the-device flag set and sopt->storage.dev is set to NULL to
   signify to the caller that ownership was taken.

   Returns non-zero if any part of setup fails, but these parts are
   not considered critical, and should not cause init of the fs to
   fail (i.e., ignore the return code, and the client can check sopt's
   state if he NEEDs to know if certain part failed).

   This routine does not check sopt->storage.enableLocking, since that
   normally needs to happen before the above actions are desired.

   On mis-use, this routine asserts in debug builds and returns
   whio_rc.InternalError in non-debug builds.
*/
static int whio_epfs_apply_sopt( whio_epfs * fs, whio_epfs_setup_opt * sopt )
{
    if( !fs || !sopt || !((sopt->storage.dev) && (fs->dev == sopt->storage.dev)) )
    {
        assert( 0 && "Internal usage error: the arguments passed to whio_epfs_apply_sopt() do not match our criteria." );
        return whio_rc.InternalError;
    }
    int rc = 0;
    if( sopt->storage.dev && sopt->storage.takeDevOnSuccess )
    {
        fs->flags |= WHIO_EPFS_FLAG_FS_OWNS_DEV;
        sopt->storage.dev = NULL;
    }
    if( sopt->memory.mem )
    {
        rc = whio_epfs_mempool_setup( fs, sopt->memory.mem,
                                      sopt->memory.size,
                                      sopt->memory.fallbackOnOOM );
        if( 0 == rc )
        {
            sopt->memory.mem = NULL;
        }
        else
        {
            // ^^^^ ignoring error case - not critical.
        }
    }
    return rc;
}

int whio_epfs_mkfs( whio_epfs ** fs_, whio_epfs_setup_opt * sopt, whio_epfs_fsopt const * fsopt )
{
    if( ! fs_  || !fsopt || ! sopt || !sopt->storage.dev ) return whio_rc.ArgError;
    if( ! sopt->storage.dev->api->iomode(sopt->storage.dev) )
    {
        /** We're going to assume that iomode<0 (unknown) is writable.
            If it is not, the first write operations will soon fail.
        */
        return whio_rc.AccessError;
    }
    const bool ownsFS = !*fs_;
    whio_epfs * fs = ownsFS ? whio_epfs_alloc() : *fs_;
    if( ownsFS && !fs ) return whio_rc.AllocError;
#define RERR(RC) fs->err = RC; if(ownsFS){whio_epfs_finalize(fs);} else {whio_epfs_close(fs);} return RC
#define CHECKRC if(rc) { RERR(rc); }
    //whio_epfs_check_fileno( fs );
    fs->fsopt = *fsopt;
    whio_epfs_init_offsz(fs);
    fs->dev = sopt->storage.dev;
    fs->flags |= WHIO_EPFS_FLAG_RW;
    int rc = 0;
    if( sopt->storage.enableLocking )
    {
        if( 0 != whio_epfs_probe_lockability( fs ) )
        {
            sopt->storage.enableLocking = false;
        }
        else
        {
            //whio_lock_request lock = whio_lock_request_set_w;
            //rc = whio_epfs_storage_lock( fs, &lock );
            rc = whio_epfs_storage_lock_all( fs, false );
            //MARKER("Lock write on EFS? rc=%d\n",rc);
            /**
               We expect this lock to pass unless someone has the EFS open,
               in which case we need to abort the mkfs (as opposed to waiting
               for a lock then hosing the EFS once we get it).
             */
            CHECKRC;
        }
    }
    else
    {
        fs->hints.storageLockingMode = 0;
    }

    {
        rc = fs->dev->api->truncate( fs->dev, 0 );
        /* ^^^ Truncation avoids garbage data at the end the device,
           which the EFS may later try to read it as block
           metadata. (Been there, debugged that.)

           However... not all devices support truncation. Namely
           subdevices, but i plan on using a subdevice to host
           an EPFS inside of a libwhefs filesystem. So i'm going to
           ignore the return value for the sake of subdevices
           for now.
        */
        rc = 0;
    }
    rc = whio_epfs_fsopt_write(fs);
    CHECKRC;
    rc = whio_epfs_hints_write(fs);
    CHECKRC;
    rc = whio_epfs_label_write(fs,0,0);
    CHECKRC;
    rc = whio_epfs_inodes_table_write(fs);
    CHECKRC;

    /** Write the magic bytes last, to help work around the race
        condition that an app opens the EFS while mkfs is still
        writing.
    */
    rc = whio_epfs_magic_write(fs);
    CHECKRC;
#undef CHECKRC
#undef RERR
    if( ownsFS ) *fs_ = fs;
    whio_epfs_apply_sopt( fs, sopt )/*ignoring error code - not critical.*/;
    whio_epfs_flush(fs);
    return rc;
}

int whio_epfs_mkfs2( whio_epfs ** fs_, whio_dev * store, whio_epfs_fsopt const * opt, bool takeStore )
{
    whio_epfs_setup_opt so = whio_epfs_setup_opt_empty;
    so.storage.dev = store;
    so.storage.takeDevOnSuccess = takeStore;
    return whio_epfs_mkfs( fs_, &so, opt );
}

int whio_epfs_mkfs_file( whio_epfs ** fs_,
                         whio_epfs_fsopt const * fsopt,
                         char const * fname,
                         bool allowOverwrite )
{
    if( ! fs_
        || ! fsopt
        || ! fname
        || !*fname
        ) return whio_rc.ArgError;
    else
    {
        /**
           i HATE this dual-open business here, but i want to confirm,
           for the case of mkfs, that we don't overwrite files unless
           we really must.
        */
        int rc = 0;
        whio_dev * dev = whio_dev_for_filename( fname, "r" );
        bool existed = (NULL != dev);
        if( existed )
        {
            dev->api->finalize(dev);
            dev = NULL;
            if( !allowOverwrite )
            {
                return whio_rc.AccessError;
            }
        }
        dev = whio_dev_for_filename( fname, existed ? "r+" : "w+" );
        /**  reminder: ---------------------^^^^

             i'd prefer "w" mode (create or open+truncate) to avoid a
             potential problem of extra garbage at the end of the
             device (on file re-use), but that hoses the device before
             we get a chance to check the locking.

             We cannot truncate() the device to clean it up because
             subdevices don't yet support truncation and i plan on using
             a subdevice in conjunction with whefs.
        */
        if( ! dev )
        {
            return whio_rc.IOError /* we're guessing. */;
        }
        whio_epfs_setup_opt sopt = whio_epfs_setup_opt_empty;
        sopt.storage.dev = dev;
        sopt.storage.takeDevOnSuccess = true;
        rc = whio_epfs_mkfs( fs_, &sopt, fsopt );
        if( rc )
        {
            dev->api->finalize(dev);
        }
        return rc;
    }
}

/**
   Reads whio_epfs_sizeof_fsopt bytes from position
   fs->offsets[whio_epfs_index_fsopt] of fs->dev and populates
   fs->fsopt with the data.

   On success 0 is returned, else non-zero and fs->fsopt is
   in an undefined state.
*/
static int whio_epfs_fsopt_read( whio_epfs * fs )
{
    assert( fs && fs->dev && fs->sizes[whio_epfs_index_magic] && "Premature call of whio_epfs_fsopt_read()!");
    if( ! fs || ! fs->dev ) return whio_rc.ArgError;
    enum { Len = whio_epfs_sizeof_fsopt };
    unsigned char buf[Len];
    int rc = 0;
    memset(buf,0,Len);
    if( Len != whio_epfs_readat( fs, fs->offsets[whio_epfs_index_fsopt], buf, Len ) )
    {
        return whio_rc.IOError;
    }
    if( *buf != whio_epfs_fsopt_tag_char )
    {
        return whio_rc.ConsistencyError;
    }
    unsigned char * at = buf+1;
    rc = whio_epfs_decode_id( at, &fs->fsopt.inodeCount );
    at += whio_epfs_sizeof_id;
    if( rc ) return rc;

    rc = whio_epfs_decode_id( at, &fs->fsopt.maxBlocks );
    at += whio_epfs_sizeof_id;
    if( rc ) return rc;
        
    rc = whio_decode_size_t( at, &fs->fsopt.blockSize );
    at += whio_sizeof_encoded_size_t;
    return rc;
}

/**
   fs must be initialized and fs->dev must already be
   set.

   This function verifies the magic bytes of the PFS, sets up
   fs->offsets and fs->sizes, and reads in fs->fsopt.

   On success 0 is returned. On error the caller
   must clean up/free fs.
*/
static int whio_epfs_openfs_stage2( whio_epfs * fs )
{
    if( ! fs ) return whio_rc.ArgError;
    //whio_epfs_check_fileno( fs );
    fs->offsets[whio_epfs_index_magic] = 0;
    fs->sizes[whio_epfs_index_magic] = whio_epfs_sizeof_magic;
    int rc = whio_epfs_magic_read( fs );
    if( rc ) return rc;
    fs->offsets[whio_epfs_index_fsopt] = fs->offsets[whio_epfs_index_magic]
        + fs->sizes[whio_epfs_index_magic];
    rc = whio_epfs_fsopt_read( fs );
    if( rc ) return rc;
    whio_epfs_init_offsz(fs);
    return whio_epfs_hints_read(fs);
}


int whio_epfs_openfs( whio_epfs ** fs_, whio_epfs_setup_opt * sopt )
{
    if( ! fs_ || ! sopt || !sopt->storage.dev ) return whio_rc.ArgError;
    const bool ownsFS = !*fs_;
    whio_epfs * fs = ownsFS ? whio_epfs_alloc() : *fs_;
    if( ownsFS && !fs ) return whio_rc.AllocError;
#define RERR(RC) fs->err = RC; if(ownsFS){whio_epfs_finalize(fs);} else {whio_epfs_close(fs);} return RC
#define CHECKRC if(rc) { RERR(rc); }
    fs->dev = sopt->storage.dev;
    if( 0 < fs->dev->api->iomode( fs->dev ) )
    {
        fs->flags |= WHIO_EPFS_FLAG_RW;
    }
    int rc = 0;
    if( sopt->storage.enableLocking )
    {
        if( 0 != whio_epfs_probe_lockability( fs ) )
        {
            sopt->storage.enableLocking = false;
        }
        else
        {
            rc = whio_epfs_storage_lock_all( fs, true );
        }
    }
    else
    {
        fs->hints.storageLockingMode = 0;
    }
    if( ! rc )
    {
        rc = whio_epfs_openfs_stage2( fs );
    }
    if( rc )
    {
        fs->err = rc;
        if( ownsFS )
        {
            whio_epfs_finalize( fs );
            *fs_ = NULL;
        }
        else
        {
            whio_epfs_close( fs );
        }
        fs = 0;        
        return rc;
    }
    if( ownsFS ) *fs_ = fs;
    whio_epfs_apply_sopt( fs, sopt )/*ignoring error code - not critical.*/;
    return whio_rc.OK;
#undef RERR
#undef CHECKRC
}

int whio_epfs_openfs2( whio_epfs ** fs_, whio_dev * store, bool takeStore )
{
    if( ! fs_ || ! store ) return whio_rc.ArgError;
    else
    {
        whio_epfs_setup_opt opt = whio_epfs_setup_opt_empty;
        opt.storage.dev = store;
        opt.storage.takeDevOnSuccess = takeStore;
        return whio_epfs_openfs( fs_, &opt );
    }
}


int whio_epfs_openfs_file( whio_epfs ** fs_, char const * fname, bool writeMode )
{
    if( ! fs_ || ! fname || !*fname ) return whio_rc.ArgError;
    else
    {
        whio_dev * dev = whio_dev_for_filename( fname, writeMode ? "r+" : "r" );
        if( ! dev )
        {
            return whio_rc.IOError /* we're guessing. */;
        }
        whio_epfs_setup_opt opt = whio_epfs_setup_opt_empty;
        opt.storage.dev = dev;
        opt.storage.takeDevOnSuccess = true;
        int rc = whio_epfs_openfs( fs_, &opt );
        if( rc )
        {
            dev->api->finalize(dev);
        }
        return rc;
    }
}

int whio_epfs_unlink( whio_epfs * fs, whio_epfs_id_t inodeID )
{
    if( ! fs ) return whio_rc.ArgError;
    if( ! whio_epfs_inode_id_in_bounds( fs, inodeID ) ) return whio_rc.RangeError;
    if( ! whio_epfs_is_rw( fs ) ) return whio_rc.AccessError;
    whio_epfs_handle const * h = whio_epfs_handle_search_id( &fs->handles, inodeID );
    if( h )
    {
        return whio_rc.AccessError;
    }
    whio_epfs_inode ino = whio_epfs_inode_empty;
    int rc = whio_epfs_inode_read( fs, inodeID, &ino );
    if( rc ) return rc;
    if( ! whio_epfs_inode_is_used(&ino) ) return whio_rc.RangeError;
    whio_epfs_id_t firstBlock = ino.firstBlock;
    if( firstBlock )
    {
        whio_epfs_block bl = whio_epfs_block_empty;
        rc = whio_epfs_block_read( fs, firstBlock, &bl );
        if( rc ) return rc;
        rc = whio_epfs_block_wipe( fs, &bl, true, true, true );
        /**
           if rc!=0 then we may be orphaning blocks here. They might
           be left marked as used but not claimed by an inode.

           We could fix such cases later if we stored the inode
           ID in the block.
         */
    }
    if( 0 == rc )
    {
        whio_epfs_inode_set_used( fs, &ino, false );
        rc = whio_epfs_inode_flush( fs, &ino );
        if( rc ) return rc;
    }
    /* FIXME? move this somewhere else? */
    rc = whio_epfs_hints_write( fs );

    return rc;
}

int whio_epfs_parse_id( char const * inp, bool allowExtraChars, whio_epfs_id_t * tgt )
//int whio_epfs_parse_id( char const * inp, whio_epfs_id_t * tgt, char const ** end )
{
    if( ! inp || ! tgt ) return whio_rc.ArgError;
    const bool tryHex = (*inp && ((*(inp+1) == 'x') || (*(inp+1) == 'X')));
    char const * scanFmt = tryHex ? "%"WHIO_EPFS_ID_T_SFMTX : "%"WHIO_EPFS_ID_T_SFMT;
    int rc = sscanf( inp, scanFmt, tgt);
    if( 1!=rc) return whio_rc.RangeError;
    rc = 0;
#if 0
    if( end )
    {
        char const * p = tryHex ? (inp+2) : inp;
        for( ; *p && (tryHex ? isxdigit(*p) : isdigit(*p)); ++p )
        {
        }
        *end = p;
    }
#else
    if( ! allowExtraChars )
    {
        char const * p = tryHex ? (inp+2) : inp;
        for( ; *p && (tryHex ? isxdigit(*p) : isdigit(*p)); ++p )
        {
        }
        if( *p ) rc = whio_rc.RangeError;
    }
#endif
    return rc;
}

int whio_epfs_parse_range( char const * inp,
                           whio_epfs_id_t * begin,
                           whio_epfs_id_t * end )
{
    if( ! inp ) return whio_rc.ArgError;
    whio_epfs_id_t b = 0;
    whio_epfs_id_t e = 0;
    int rc = whio_epfs_parse_id( inp, true,  &b );
    if( rc ) return rc;
    char const * at = inp+1;
    for( ; *at && (*at != '-'); ++at )
    {
    }
    if( ! *at )
    { // only a single number
        if( begin ) *begin = b;
        if( end ) *end = b;
        return 0;
    }
    if( *at != '-' )
    {
        return whio_rc.RangeError;
    }
    ++at;
    rc = whio_epfs_parse_id( at, false, &e );
    if( ! rc )
    {
        if( begin ) *begin = b;
        if( end ) *end = e;
    }
    return rc;
}


#undef MARKER
/* end file pfs/pfs.c */
/* begin file udb/whglob.c */
#line 8 "udb/whglob.c"
#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif


/**
   Globbing implementation extracted from the sqlite3 source tree.

   Original author: D. Richard Hipp (http://sqlite.org)

   Maintainer of this copy: Stephan Beal
   (http://wanderinghorse.net/home/stephan)

   License: Public Domain
*/

typedef unsigned char u8;
#define SQLITE_ASCII 1
/* An array to map all upper-case characters into their corresponding
** lower-case character. 
**
** SQLite only considers US-ASCII (or EBCDIC) characters.  We do not
** handle case conversions for the UTF character set since the tables
** involved are nearly as big or bigger than SQLite itself.
*/
static const unsigned char sqlite3UpperToLower[] = {
#ifdef SQLITE_ASCII
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
     18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
     36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
     54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 97, 98, 99,100,101,102,103,
    104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
    122, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,
    108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,
    126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
    144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,
    162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
    180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,
    198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,
    216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,
    234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,
    252,253,254,255
#endif
#ifdef SQLITE_EBCDIC
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, /* 0x */
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, /* 1x */
     32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, /* 2x */
     48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, /* 3x */
     64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, /* 4x */
     80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, /* 5x */
     96, 97, 66, 67, 68, 69, 70, 71, 72, 73,106,107,108,109,110,111, /* 6x */
    112, 81, 82, 83, 84, 85, 86, 87, 88, 89,122,123,124,125,126,127, /* 7x */
    128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143, /* 8x */
    144,145,146,147,148,149,150,151,152,153,154,155,156,157,156,159, /* 9x */
    160,161,162,163,164,165,166,167,168,169,170,171,140,141,142,175, /* Ax */
    176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191, /* Bx */
    192,129,130,131,132,133,134,135,136,137,202,203,204,205,206,207, /* Cx */
    208,145,146,147,148,149,150,151,152,153,218,219,220,221,222,223, /* Dx */
    224,225,162,163,164,165,166,167,168,169,232,203,204,205,206,207, /* Ex */
    239,240,241,242,243,244,245,246,247,248,249,219,220,221,222,255, /* Fx */
#endif
};
/*
** This lookup table is used to help decode the first byte of
** a multi-byte UTF8 character.
*/
static const unsigned char sqlite3UtfTrans1[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x00, 0x00,
};

/*
** For LIKE and GLOB matching on EBCDIC machines, assume that every
** character is exactly one byte in size.  Also, all characters are
** able to participate in upper-case-to-lower-case mappings in EBCDIC
** whereas only characters less than 0x80 do in ASCII.
*/
#if defined(SQLITE_EBCDIC)
# define sqlite3Utf8Read(A,B,C)  (*(A++))
# define GlogUpperToLower(A)     A = sqlite3UpperToLower[A]
#else
# define GlogUpperToLower(A)     if( A<0x80 ){ A = sqlite3UpperToLower[A]; }
#endif

/*
** Assuming zIn points to the first byte of a UTF-8 character,
** advance zIn to point to the first byte of the next UTF-8 character.
*/
#define SQLITE_SKIP_UTF8(zIn) {                        \
  if( (*(zIn++))>=0xc0 ){                              \
    while( (*zIn & 0xc0)==0x80 ){ zIn++; }             \
  }                                                    \
}

/*
** Translate a single UTF-8 character.  Return the unicode value.
**
** During translation, assume that the byte that zTerm points
** is a 0x00.
**
** Write a pointer to the next unread byte back into *pzNext.
**
** Notes On Invalid UTF-8:
**
**  *  This routine never allows a 7-bit character (0x00 through 0x7f) to
**     be encoded as a multi-byte character.  Any multi-byte character that
**     attempts to encode a value between 0x00 and 0x7f is rendered as 0xfffd.
**
**  *  This routine never allows a UTF16 surrogate value to be encoded.
**     If a multi-byte character attempts to encode a value between
**     0xd800 and 0xe000 then it is rendered as 0xfffd.
**
**  *  Bytes in the range of 0x80 through 0xbf which occur as the first
**     byte of a character are interpreted as single-byte characters
**     and rendered as themselves even though they are technically
**     invalid characters.
**
**  *  This routine accepts an infinite number of different UTF8 encodings
**     for unicode values 0x80 and greater.  It do not change over-length
**     encodings to 0xfffd as some systems recommend.
*/
#define READ_UTF8(zIn, zTerm, c)                           \
  c = *(zIn++);                                            \
  if( c>=0xc0 ){                                           \
    c = sqlite3UtfTrans1[c-0xc0];                          \
    while( zIn!=zTerm && (*zIn & 0xc0)==0x80 ){            \
      c = (c<<6) + (0x3f & *(zIn++));                      \
    }                                                      \
    if( c<0x80                                             \
        || (c&0xFFFFF800)==0xD800                          \
        || (c&0xFFFFFFFE)==0xFFFE ){  c = 0xFFFD; }        \
  }
static int sqlite3Utf8Read(
  const unsigned char *z,         /* First byte of UTF-8 character */
  const unsigned char *zTerm,     /* Pretend this byte is 0x00 */
  const unsigned char **pzNext    /* Write first byte past UTF-8 char here */
){
  int c;
  READ_UTF8(z, zTerm, c);
  *pzNext = z;
  return c;
}


/*
** A structure defining how to do GLOB-style comparisons.
*/
typedef struct sqlite3CompareInfo
{
  u8 matchAll;
  u8 matchOne;
  u8 matchSet;
  u8 noCase;
} sqlite3CompareInfo;


/*
** Compare two UTF-8 strings for equality where the first string can
** potentially be a "glob" expression.  Return true (1) if they
** are the same and false (0) if they are different.
**
** Globbing rules:
**
**      '*'       Matches any sequence of zero or more characters.
**
**      '?'       Matches exactly one character.
**
**     [...]      Matches one character from the enclosed list of
**                characters.
**
**     [^...]     Matches one character not in the enclosed list.
**
** With the [...] and [^...] matching, a ']' character can be included
** in the list by making it the first character after '[' or '^'.  A
** range of characters can be specified using '-'.  Example:
** "[a-z]" matches any single lower-case letter.  To match a '-', make
** it the last character in the list.
**
** This routine is usually quick, but can be N**2 in the worst case.
**
** Hints: to match '*' or '?', put them in "[]".  Like this:
**
**         abc[*]xyz        Matches "abc*xyz" only
*/
static int patternCompare(
  const u8 *zPattern,              /* The glob pattern */
  const u8 *zString,               /* The string to compare against the glob */
  const sqlite3CompareInfo *pInfo, /* Information about how to do the compare */
  const int esc                    /* The escape character */
){
  int c, c2;
  int invert;
  int seen;
  u8 matchOne = pInfo->matchOne;
  u8 matchAll = pInfo->matchAll;
  u8 matchSet = pInfo->matchSet;
  u8 noCase = pInfo->noCase; 
  int prevEscape = 0;     /* True if the previous character was 'escape' */

  if( ! zPattern || !zString ) return 0;
  while( (c = sqlite3Utf8Read(zPattern,0,&zPattern))!=0 ){
    if( !prevEscape && c==matchAll ){
      while( (c=sqlite3Utf8Read(zPattern,0,&zPattern)) == matchAll
               || c == matchOne ){
        if( c==matchOne && sqlite3Utf8Read(zString, 0, &zString)==0 ){
          return 0;
        }
      }
      if( c==0 ){
        return 1;
      }else if( c==esc ){
        c = sqlite3Utf8Read(zPattern, 0, &zPattern);
        if( c==0 ){
          return 0;
        }
      }else if( c==matchSet ){
	  //assert( esc==0 );         /* This is GLOB, not LIKE */
	  //assert( matchSet<0x80 );  /* '[' is a single-byte character */
	  if( (esc==0) || (matchSet<0x80) ) return 0;
        while( *zString && patternCompare(&zPattern[-1],zString,pInfo,esc)==0 ){
          SQLITE_SKIP_UTF8(zString);
        }
        return *zString!=0;
      }
      while( (c2 = sqlite3Utf8Read(zString,0,&zString))!=0 ){
        if( noCase ){
          GlogUpperToLower(c2);
          GlogUpperToLower(c);
          while( c2 != 0 && c2 != c ){
            c2 = sqlite3Utf8Read(zString, 0, &zString);
            GlogUpperToLower(c2);
          }
        }else{
          while( c2 != 0 && c2 != c ){
            c2 = sqlite3Utf8Read(zString, 0, &zString);
          }
        }
        if( c2==0 ) return 0;
        if( patternCompare(zPattern,zString,pInfo,esc) ) return 1;
      }
      return 0;
    }else if( !prevEscape && c==matchOne ){
      if( sqlite3Utf8Read(zString, 0, &zString)==0 ){
        return 0;
      }
    }else if( c==matchSet ){
      int prior_c = 0;
      //assert( esc==0 );    /* This only occurs for GLOB, not LIKE */
      if( esc == 0 ) return 0;
      seen = 0;
      invert = 0;
      c = sqlite3Utf8Read(zString, 0, &zString);
      if( c==0 ) return 0;
      c2 = sqlite3Utf8Read(zPattern, 0, &zPattern);
      if( c2=='^' ){
        invert = 1;
        c2 = sqlite3Utf8Read(zPattern, 0, &zPattern);
      }
      if( c2==']' ){
        if( c==']' ) seen = 1;
        c2 = sqlite3Utf8Read(zPattern, 0, &zPattern);
      }
      while( c2 && c2!=']' ){
        if( c2=='-' && zPattern[0]!=']' && zPattern[0]!=0 && prior_c>0 ){
          c2 = sqlite3Utf8Read(zPattern, 0, &zPattern);
          if( c>=prior_c && c<=c2 ) seen = 1;
          prior_c = 0;
        }else{
          if( c==c2 ){
            seen = 1;
          }
          prior_c = c2;
        }
        c2 = sqlite3Utf8Read(zPattern, 0, &zPattern);
      }
      if( c2==0 || (seen ^ invert)==0 ){
        return 0;
      }
    }else if( esc==c && !prevEscape ){
      prevEscape = 1;
    }else{
      c2 = sqlite3Utf8Read(zString, 0, &zString);
      if( noCase ){
        GlogUpperToLower(c);
        GlogUpperToLower(c2);
      }
      if( c!=c2 ){
        return 0;
      }
      prevEscape = 0;
    }
  }
  return *zString==0;
}

int whglob_matches( char const * pattern, char const * str )
{
    static const sqlite3CompareInfo cinfo = { '*', '?', '[', 0 };
    return patternCompare( (unsigned char *)pattern, (unsigned char *) str, &cinfo, '\\' );
}

int whglob_matches_like( char const *pattern, char const * str, char caseSensitive )
{
    /* The correct SQL-92 behavior is for the LIKE operator to ignore
    ** case.  Thus  'a' LIKE 'A' would be true. */
    static const sqlite3CompareInfo likeInfoNorm = { '%', '_',   0, 1 };
    /* If SQLITE_CASE_SENSITIVE_LIKE is defined, then the LIKE operator
    ** is case sensitive causing 'a' LIKE 'A' to be false */
    static const sqlite3CompareInfo likeInfoAlt = { '%', '_',   0, 0 };
    return patternCompare( (unsigned char *)pattern, (unsigned char *) str,
			   caseSensitive ? &likeInfoAlt : &likeInfoNorm,
			   '%' );
}


#undef SQLITE_ASCII
#undef SQLITE_SKIP_UTF8
#undef READ_UTF8
#if defined(GlogUpperToLower)
#  undef GlogUpperToLower
#endif
#if defined(sqlite3Utf8Read)
#  define sqlite3Utf8Read
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* end file udb/whglob.c */
/* begin file udb/whio_udb.c */
#line 8 "udb/whio_udb.c"
#include <assert.h>
#include <string.h> /* memcmp() */
#include <strings.h> /* strncasecmp() */
#include <stdlib.h> /* malloc()/free() */
#include <ctype.h> /* tolower() */


#define WHIO_UDB_STATIC_ASSERT(NAME,COND) static const char whio_udb_static_assert_##NAME[ (COND) ? 1 : -1 ] = {'!'}

#if WHIO_UDB_ENABLE_METRICS
#  define UDB_METRIC_INC(DB,FIELD) ++(DB)->metrics.FIELD
#  define UDB_METRIC_DEC(DB,FIELD) --(DB)->metrics.FIELD
#else
#  define UDB_METRIC_INC(DB,FIELD) (void)0
#  define UDB_METRIC_DEC(DB,FIELD) (void)0
#endif

/**
   REC_SMALL_BUF(Name) declares local array
   Name[whio_udb_sizeof_record_header], which is suitable for use as a
   decode/encode buffer for "small" record reads/writes (those
   reads/writes which only do the non-key/data parts of the record).
*/
#define REC_SMALL_BUF(Name) unsigned char Name[whio_udb_sizeof_record_header]

/**
   Ensures that WHIO_UDB_MAGIC_STRING is not empty.
*/
WHIO_UDB_STATIC_ASSERT(WHIO_UDB_MAGIC_STRING_must_have_positive_size,(sizeof(WHIO_UDB_MAGIC_STRING)>1));


// Documented in the header.
/*
  whio_udb_record TODOs:

  - Refactor the chaining so that not only free blocks are chained,
  but also un-free blocks. We can re-use the prevFree/nextFree members
  for this but must first change other code which unlinks/re-links
  objects, and some which relies on 0 values for non-free records.
*/
struct whio_udb_record
{
    /** Record ID, starting with 1. The value 0 is reserved
        for "not a record" or "invalid record".
    */
    whio_size_t id;
    /**
       The record's hash code. The stored value
       isn't actually used by the main API, and the
       field is primarily here so we can:

       a) Do even more internal consistency checking. (Yeah!)

       b) So we could potentially rebuild the entry if there is
       any corruption which unlinks it from its hash collision
       chain.
    */
    whio_udb_hash_t hash;
    /**
       The ID of the next record with the same hash code as this
       one, for chaining records with hashcode collisions.  The
       first item inserted with a given hash code is the head of
       the collision list, and the list may grow arbitrarily long.

       TODO: re-use nextFree for this, and use (keyLen==0) to
       differentiate between the uses.
    */
    whio_size_t nextCollision;

    /**
       Points to the previous free record, or 0 if this
       record is in use.
    */
    whio_size_t prevFree;
    /**
       Points to the next free record, or 0 if this record is in
       use.
    */
    whio_size_t nextFree;

    /** Length of the key bytes. May be less than
        the key length of the underlying db. */
    whio_size_t keyLen;

    /** Length of the data bytes. May be less than the data length
        of the underlying db. */
    whio_size_t dataLen;

    /**
       Pointer to the key bytes, which is keyLen bytes long.
       Lifetime and ownership are defined by the routines which
       use this class. Be careful!
    */
    unsigned char const * key;

    /**
       Pointer to the data bytes, which is dataLen bytes long.
       Lifetime and ownership are defined by the routines which
       use this class. Be careful!
    */
    unsigned char const * data;

    /** The very secret internals of this class.  This data must
        not be touched by client code.
    */
    struct whio_udb_record_internals
    {
        unsigned char * buffer;
    } iobuf;
};
#define whio_udb_record_empty_m {               \
        0/*id*/,0/*hash*/,0/*nextCollision*/,   \
            0/*prevFree*/,0/*nextFree*/,        \
            0/*keyLen*/,0/*dataLen*/,           \
            NULL/*key*/,NULL/*data*/,           \
        {/*iobuf*/ NULL/*buffer*/}              \
    }


const whio_udb_opt whio_udb_opt_empty = whio_udb_opt_empty_m;
const whio_udb_funcs whio_udb_funcs_empty = whio_udb_funcs_empty_m;
const whio_udb_funcs whio_udb_funcs_strings = whio_udb_funcs_strings_m;
const whio_udb_funcs whio_udb_funcs_strings_nocase = whio_udb_funcs_strings_nocase_m;
const whio_udb_hints whio_udb_hints_empty = whio_udb_hints_empty_m;
const whio_udb_metrics whio_udb_metrics_empty = whio_udb_metrics_empty_m;
const whio_udb whio_udb_empty = whio_udb_empty_m;
static const whio_udb_record whio_udb_record_empty = whio_udb_record_empty_m;

#if 1 /* debuggering stuff */
#include <stdio.h>
static int whio_udb_dbgoutv(char const * fmt, va_list vargs )
{
    return vfprintf( stderr, fmt, vargs );
}
static int whio_udb_dbgout(char const * fmt, ...)
{
    //if(0) whio_udb_dbgout(0);
    va_list vargs;
    int rc;
    va_start(vargs,fmt);
    rc = whio_udb_dbgoutv( fmt, vargs );
    va_end(vargs);
    return rc;
}
#  define UDBG(mp_exp) do { whio_udb_dbgout("UDBG: %s:%d:%s():\t",__FILE__,__LINE__,__func__); whio_udb_dbgout mp_exp; whio_udb_dbgout("\n");} while(0)
#else
#  define UDBG(mp_exp)
#endif




static whio_udb_hash_t whio_udb_hash_default_impl( void const * obj, whio_size_t len, bool caseSens )
{
    /* "djb2" algo code taken from: http://www.cse.yorku.ca/~oz/hash.html */
    if( ! obj || !len ) return 0U;
    else
    {
        static whio_udb_hash_t seed = 5381;
        char const * vstr = (char const *)obj;
        whio_udb_hash_t hash = seed;
        unsigned int c = 0;
        whio_size_t i = 0;
        for( ; (i<len)
#if 0
                 && *vstr
                 &&(c = (caseSens ? *vstr : tolower(*vstr)))
#endif
                 ; ++vstr, ++i )
        {
            c = (caseSens ? *vstr : tolower(*vstr));
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        }
        return hash ? hash : seed;
    }
}

whio_udb_hash_t whio_udb_hash_default( void const * obj, whio_size_t len )
{
    return whio_udb_hash_default_impl( obj, len, true );
}

static whio_udb_hash_t whio_udb_hash_str_impl( void const * obj, whio_size_t len, bool caseSens )
{
    char const * str = (char const *)obj;
    const whio_size_t sl = str ? strlen(str) : 0;
    return sl ? whio_udb_hash_default_impl( str, sl, caseSens ) : 0;
}

whio_udb_hash_t whio_udb_hash_str( void const * obj, whio_size_t len )
{
    return whio_udb_hash_str_impl( obj, len, true );
}


whio_udb_hash_t whio_udb_hash_str_nocase( void const * obj, whio_size_t len )
{
    return whio_udb_hash_str_impl( obj, len, false );
}


whio_size_t whio_udb_length_str( void const * obj )
{
    return obj ? strlen((char const *)obj) : 0;
}

int whio_udb_key_cmp_str( void const * s1, void const * s2, size_t len )
{
    if( !s1 ) return s2 ? -1 : 0;
    else if( !s2 ) return s1 ? 1 : 0;
    else if( !len ) return 0;
    else return strncmp( (char const *)s1, (char const *)s2, len );
}

int whio_udb_key_cmp_str_nocase( void const * s1, void const * s2, size_t len )
{
    if( !s1 ) return s2 ? -1 : 0;
    else if( ! s2 ) return s1 ? 1 : 0;
    else
    {
        size_t i = 0;
        char const * l = (char const *)s1;
        char const * r = (char const *)s2;
        char lv, rv;
        for( ; i<len; ++l, ++r, ++i )
        {
            lv = tolower(*l);
            rv = tolower(*r);
            if( !lv ) return rv ? -1 : 0;
            else if( !rv ) return 1;
            else if( lv < rv ) return -1;
            else if( lv > rv ) return 1;
        }
        return 0;
    }
        //return strncasecmp( (char const *)s1, (char const *)s2, len );
}

whio_size_t whio_udb_hashtable_length( whio_udb const * db )
{
    return db ? db->opt.hashSize : 0;
}

whio_size_t whio_udb_record_buffer_size( whio_size_t keyLen, whio_size_t dataLen )
{
    //return db ? db->osz[whio_udb_osz_sizeof_record] : 0;
    return (!keyLen || !dataLen)
        ? 0
        : whio_udb_sizeof_record_header
        + keyLen +1/*NULL byte*/
        + dataLen +1/*NULL byte*/
        ;
}

whio_size_t whio_udb_sizeof_record( whio_udb const * db )
{
    return db
        ? whio_udb_record_buffer_size( db->opt.keyLen, db->opt.dataLen )
        : 0;    
}

/**
   Locks/unlocks db->mutex, depending on lockIt. Returns 0 on un/lock
   success or if db->mutex.unlock and db->mutex.lock are NULL.
   Returns mutex-specified non-0 on error.  Returns whio_rc.ArgError
   if either, but not both, of db->mutex.lock and db->mutex.unlock are
   NULL.
*/
static int whio_udb_mlock( whio_udb * db, bool lockIt )
{
    if( ! db ) return whio_rc.ArgError;
    else if( ! db->mutex.lock )
    {
        if( !db->mutex.unlock ) return 0;
        else return whio_rc.ArgError;
    }
    else
    {
        return lockIt
            ? db->mutex.lock( db->mutex.state )
            : db->mutex.unlock( db->mutex.state );
    }
}
/** Internal helper macro: locks, returns RV if locking fails. */
#define MLOCK_OR_FAIL(DB,RV) do { int lockRC = whio_udb_mlock((DB),true); if(lockRC) return RV; } while(0)
/** Internal helper macro: unlocks and returns RV. */
#define MUNLOCK_RETURN(DB,RV) do { whio_udb_mlock((DB),false); return RV; } while(0)

int whio_udb_mutex_set( whio_udb * db, whio_mutex const * m )
{
    if( ! db ) return whio_rc.ArgError;
    else if( m &&
             ((!m->lock && m->unlock)
              || (m->lock && !m->unlock)) )
    {
        return whio_rc.RangeError;
    }
    else
    {
        db->mutex = m ? *m : whio_mutex_empty;
        return 0;
    }
}

/** @internal

    Applies the given lock request to db->dev. Returns 0 on success or
    if db->lockMode==0 (the latter case is a no-op - there are no
    side-effects).
*/
static int whio_udb_storage_lock( whio_udb * db, whio_lock_request * req );

/** @internal
   Read-locks (or waits for it) db's magic bytes.

   Always returns 0 unless locking actually fails.  If locking isn't
   supported, 0 is returned.
*/
static int whio_udb_lock_magic( whio_udb * db );

/**
   Checks db->dev for lockability. Returns:

   0 = dev appears to support locking.

   !0 = dev does not support locking.

   db->lockMode is set the first time this function is
   called. Thereafter its result is re-used.
*/
static int whio_udb_probe_lockability( whio_udb * db )
{
    assert(db && db->dev && "Mis-use of function whio_udb_probe_lockability()!");
    if( ! db || !db->dev )
    {
        return whio_rc.ArgError;
    }
    else if( db->lockMode >= 0 )
    { /* Was already probed. Re-use the result. */
        return db->lockMode ? 0 : whio_rc.UnsupportedError;
    }
    else
    {
        int rc = whio_dev_ioctl( db->dev, whio_dev_ioctl_mask_WHIO_LOCKING );
        db->lockMode = ( 0 == rc ) ? 1 : 0;
        if( db->lockMode > 0 )
        {
            /**
               Obtain a read lock on the magic bytes and keep it
               as long as the db is open. This is mainly to give
               whio_udb_dev_format() a way of bailing out if
               another app has the db opened. The magic bytes
               are only written by the formatting process.
            */
            rc = whio_udb_lock_magic(db);
        }
        return rc;
    }
}

int whio_udb_lock_magic( whio_udb * db )
{
    whio_lock_request wli = whio_lock_request_setw_r;
    wli.start = 0;
    wli.length = whio_udb_sizeof_magic;
    return whio_udb_storage_lock( db, &wli );
}


int whio_udb_storage_lock( whio_udb * db, whio_lock_request * req )
{
    if( ! db || !req || !db->dev )
    {
        return whio_rc.ArgError;
    }
    else if( db->lockMode == 0 ) return 0;
    else if( db->lockMode < 0 )
    {
        return whio_rc.UnsupportedError;
    }
    else if( (whio_lock_TYPE_WRITE == req->type)
             && (db->iomode<=0) )
    {
        return whio_rc.AccessError;
    }
    return whio_dev_lock( db->dev, req );
}

/** Locks all of db->dev, waiting for the lock if 'wait' is set. The
    lock's read/write mode depends on db->iomode. If db->lockMode==0
    then this func is a no-op and 0 is returned.
*/
static int whio_udb_storage_lock_all( whio_udb * db, bool wait )
{
    if( ! db || ! db->dev ) return whio_rc.ArgError;
    else if( db->lockMode < 0 ) return whio_rc.UnsupportedError;
    else if( db->lockMode == 0 ) return 0;
    else
    {
        /**
           Maintenance reminder: i split this into two locks:

           a) the magic bytes

           b) everything else

           because i'm not sure what happens if i lock the whole range
           and only want to unlock everything but the magic bytes
           later (to avoid a race condition).

           i _think_ that the unlock would split/resize/join the
           un/locked ranges, but i'm not 100% certain of that. Need to
           read up on it.

           The problem is that after formatting the db, we need to
           unlock the container except for the magic bytes. Doing an
           unlock-all, then locking the bytes leads to a formatting
           race condition which could destroy an in-use db.
        */
        int rc = 0;
        whio_lock_request wli = whio_lock_request_empty;
        wli.start = whio_udb_sizeof_magic;
        wli.type = (db->iomode>0)
            ? whio_lock_TYPE_WRITE
            : whio_lock_TYPE_READ;
        wli.command = wait
            ? whio_lock_CMD_SET_WAIT
            : whio_lock_CMD_SET_NOWAIT;
        rc = whio_udb_storage_lock( db, &wli );
        if( 0 == rc )
        {
            wli.start = 0;
            wli.length = whio_udb_sizeof_magic;
            wli.type = whio_lock_TYPE_READ;
            rc = whio_udb_storage_lock( db, &wli );
        }
        return rc;
    }
}
/** Almost the converse of whio_udb_storage_lock_all().

    If includeMagic is true then the magic bytes are also unlocked,
    else they are not.
 */
static int whio_udb_storage_unlock_some( whio_udb * db, bool includeMagic )
{
    if( ! db || ! db->dev ) return whio_rc.ArgError;
    else if( db->lockMode < 0 ) return whio_rc.UnsupportedError;
    else if( db->lockMode == 0 ) return 0;
    else
    {
        whio_lock_request wli = whio_lock_request_set_u;
        wli.start = includeMagic ? 0 : whio_udb_sizeof_magic;
        return whio_udb_storage_lock( db, &wli );
    }
}


whio_udb_record * whio_udb_record_alloc( whio_udb * db )
{
    if( ! db ) return NULL;
    else
    {
        MLOCK_OR_FAIL(db,NULL);
        const whio_size_t sz = db->osz[whio_udb_osz_sizeof_page_line];
        if( ! db->recordBook )
        {
            /*
              The following is a very arbitrary heuristic for finding
              a "reasonable" default memory usage. We don't want many
              records in RAM if each is if each result is 50kb!  OTOH,
              we don't want to keep de/reallocating such records.
            */
            whio_size_t ObjsPerPage = 5;
            if( sz < 1024 /*arbitrary!*/ )
            {
                ObjsPerPage = 5;
            }
            else if ( sz < (4*1024) /*arbitrary!*/ )
            {
                ObjsPerPage = 4;
            }
            else if ( sz < (10*1024) /*arbitrary!*/ )
            {
                ObjsPerPage = 3;
            }
            else if ( sz < (20*1024) /*arbitrary!*/ )
            {
                ObjsPerPage = 2;
            }
            else
            {
                ObjsPerPage = 1;
            }
            db->recordBook = WHALLOC_API(book_open)( ObjsPerPage, sz, NULL, NULL, NULL, NULL );
            if( db->recordBook )
            {
                WHALLOC_API(book_vacuum_auto)( db->recordBook, false );
            }
        }
        if( ! db->recordBook )
        {
            MUNLOCK_RETURN(db,NULL);
        }
        else
        {

            whio_udb_record * r = NULL;
            unsigned char * m = NULL;
            m = (unsigned char *)WHALLOC_API(book_alloc)( db->recordBook );
            if( ! m )
            {
                MUNLOCK_RETURN(db,NULL);
            }
            UDB_METRIC_INC(db,recordAllocs);
            UDB_METRIC_INC(db,recordAllocsNow);
#if WHIO_UDB_ENABLE_METRICS
            if( db->metrics.recordAllocsNow > db->metrics.recordAllocsMaxConcurrent )
            {
                db->metrics.recordAllocsMaxConcurrent = db->metrics.recordAllocsNow;
            }
#endif
            /**
               Lay out memory like so:

               r = mem

               r->iobuf.buffer = sizeof(whio_udb_record) bytes after r,
               whio_udb_sizeof_record_header bytes long.

               The buffer is for use with
               whio_udb_record_encode/decode().
               
               r->key and r->data point to their appropriate
               places in the r->iobuf.buffer.
            */
            memset( m, 0, sz );
            r = (whio_udb_record *)m;
            *r = whio_udb_record_empty;
            r->iobuf.buffer = m + sizeof(whio_udb_record);
            r->key = r->iobuf.buffer + whio_udb_sizeof_record_header;
            r->data = r->key + db->opt.keyLen + 1/*NULL*/;
            MUNLOCK_RETURN(db,r);
        }
    }
}

/** @internal

   Works as documented for whio_udb_record_free(), but if lockMutex
   is true then db->mutex is locked, otherwise the caller is assumed
   to have locked it.
*/
static int whio_udb_record_free_impl( whio_udb * db, whio_udb_record * r, bool lockMutex )
{
    if( ! db || !r ) return whio_rc.ArgError;
    else
    {
        if( lockMutex )
        {
            MLOCK_OR_FAIL(db,whio_rc.LockingError);
        }
        int rc = (db && db->recordBook)
            ? WHALLOC_API(book_free)( db->recordBook, r )
            : whio_rc.ArgError;
        if( 0 == rc )
        {
            UDB_METRIC_INC(db,recordDeallocs);
            UDB_METRIC_DEC(db,recordAllocsNow);
            (void)0;
        }
        if( lockMutex )
        {
            MUNLOCK_RETURN(db,rc);
        }
        else
        {
            return rc;
        }
    }
}
int whio_udb_record_free( whio_udb * db, whio_udb_record * r )
{
    return whio_udb_record_free_impl( db, r, true );
}

/**
   Assigns all members of src to dest EXCEPT the key/data/iobuf parts. Those
   are kept intact. This is used for "stamping" a record onto another.
   The key/data/iobuf pointers normally point to offsets within dest
   itself, so we don't want them hosed.

   If The iobuf.buffer member of both are set then the contents are copied
   from src to dest.
*/
static whio_udb_record * whio_udb_record_cp_no_ptr( whio_udb_record const * src,
                                                    whio_udb_record * dest )
{
    whio_udb_record tmp = *dest;
    *dest = *src;
    dest->key = tmp.key;
    dest->data = tmp.data;
    dest->iobuf = tmp.iobuf;
    if( src->iobuf.buffer && dest->iobuf.buffer )
    {
        memcpy( dest->iobuf.buffer, src->iobuf.buffer, src->keyLen );
    }
    return dest;
}

/**
   Tries to allocate db's internal buffers (namely db->rbuf). Returns
   true if it can or if they have already been allocated, else false.

   Requires that db->opt be properly populated.

   Reminder to self: This function is only called by the init
   processes, so mutex locking is not needed.
*/
static bool whio_udb_buffers( whio_udb * db )
{
    if( ! db ) return false;
    else if( db->rbuf ) return true;
    else
    {
        if( ! db->rbuf )
        {
            db->rbuf = whio_udb_record_alloc(db);
        }
        return NULL != db->rbuf;
    }
}

/*
Most C compilers handle variable-sized arrays, so we enable
that by default. Some (e.g. tcc) do not, so we provide a way
to disable it: set WHIO_UDB_HAVE_VARARRAY to 0

One approach would be to look at:

  defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)

but some compilers support variable-sized arrays even when not
explicitly running in c99 mode.
*/
#if !defined(WHIO_UDB_HAVE_VARARRAY)
#  if defined(__TINYC__)
#    define WHIO_UDB_HAVE_VARARRAY 0
#  elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#    define WHIO_UDB_HAVE_VARARRAY 1
#  else
#    define WHIO_UDB_HAVE_VARARRAY 0
#  endif
#endif

/**
WHIO_UDB_VLA_DECL() and friends are helpers to allocate variable-sized
arrays.  This exists mainly so this code can compile with the tcc
compiler.
*/
#if WHIO_UDB_HAVE_VARARRAY
#  define WHIO_UDB_VLA_DECL(T,V,N) T V[N+1]; memset(V,0,N+1);
#  define WHIO_UDB_VLA_FREE(V)
#  define WHIO_UDB_VLA_BOOL(V) 1
#else
#  define WHIO_UDB_VLA_DECL(T,V,N) T * V = (T *)malloc(N+1); memset(V,0,N+1);
#  define WHIO_UDB_VLA_FREE(V) free(V)
#  define WHIO_UDB_VLA_BOOL(V) (NULL!=(V))
#endif



whio_udb * whio_udb_alloc()
{
    whio_udb * d = (whio_udb *) malloc( sizeof(whio_udb) );
    if( d )
    {
        *d = whio_udb_empty;
    }
    return d;
}



bool whio_udb_is_rw( whio_udb const * db )
{
    return !db
        ? false
        : (db->dev && (db->iomode>0));
}

/**
   Sets up db->osz based primarily on the
   values of db->opt.

   db need not be fully initialized, but db->opt
   needs to be properly set up.
   
   
   Returns 0 on success or non-zero if !db.
*/
static int whio_udb_init_osz( whio_udb * db )
{
    if( ! db ) return whio_rc.ArgError;
    else
    {
        db->osz[whio_udb_osz_sizeof_magic] = whio_udb_sizeof_magic;
        db->osz[whio_udb_osz_sizeof_hashent] =
            whio_udb_sizeof_hashent
            ;
        db->osz[whio_udb_osz_sizeof_ht] =
            (
             db->opt.hashSize
             * db->osz[whio_udb_osz_sizeof_hashent]
             )
            ;

        db->osz[whio_udb_osz_sizeof_record] =
            + whio_udb_sizeof_record_header
            + db->opt.keyLen
            + 1 /*NULL byte*/
            + db->opt.dataLen
            + 1 /*NULL byte*/
            ;

        db->osz[whio_udb_osz_sizeof_dbopt] =
            whio_udb_sizeof_dbopt
            ;

        db->osz[whio_udb_osz_sizeof_hints] =
            whio_udb_sizeof_hints
            ;

        db->osz[whio_udb_osz_pos_dbopt] = whio_udb_pos_dbopt;

        db->osz[whio_udb_osz_pos_hints] = whio_udb_pos_hints;

        db->osz[whio_udb_osz_pos_ht] = whio_udb_pos_hashtable;

        db->osz[whio_udb_osz_pos_data] =
            db->osz[whio_udb_osz_pos_ht]
            + db->osz[whio_udb_osz_sizeof_ht]
            ;

        {
            /**
               See whio_udb_record_alloc()
            */
            whio_size_t sz = sizeof( whio_udb_record ) + whio_udb_sizeof_record( db );
            sz += (sz%8) /* round up to some sort of alignment boundary,
                            though the pager doesn't yet give us any alignment
                            guarantees. */;
            db->osz[whio_udb_osz_sizeof_page_line] = sz;
        }

#if 0
#define OSZ(X) UDBG(("db->osz[whio_udb_osz_"#X"]=%"WHIO_SIZE_T_PFMT,db->osz[whio_udb_osz_##X]))
        OSZ(pos_dbopt);
        OSZ(sizeof_dbopt);
        OSZ(pos_hints);
        OSZ(sizeof_hints);
        OSZ(pos_data);
        OSZ(pos_ht);
        OSZ(sizeof_ht);
        /*OSZ(pos_idx);*/
#undef OSZ
#endif
        return 0;
    }
}

/**
   As documented for whio_udb_flush(), but if lockMutex is false then
   the caller is assumed to have already locked db->mutex.
*/
int whio_udb_flush_impl( whio_udb * db, bool lockMutex )
{
    if( ! db || !db->dev )
    {
        return whio_rc.ArgError;
    }
    else if( db->iomode < 1 )
    {
        return whio_rc.AccessError;
    }
    else
    {
        int rc = 0;
        if( lockMutex )
        {
            MLOCK_OR_FAIL(db,whio_rc.LockingError);
        }
        UDB_METRIC_INC(db,flushes);
        rc = db->dev->api->flush( db->dev );
        if( lockMutex )
        {
            MUNLOCK_RETURN(db,rc);
        }
        else
        {
            return rc;
        }
    }
}
int whio_udb_flush( whio_udb * db )
{
    return whio_udb_flush_impl( db, true );
}
/** @internal

   Writes whio_udb_sizeof_magic bytes to dev. The first bytes
   are taken from str, and any remaining bytes are null-padded.

   Returns 0 on success.
*/
static int whio_udb_magic_write_dev( whio_dev * dev, char const * str )
{
    if( ! dev || ! str)
    {
        return whio_rc.ArgError;
    }
    else
    {
        enum { Len = whio_udb_sizeof_magic };
        unsigned char buf[Len];
        memset( buf, 0, Len );
        char const * p = str;
        size_t i = 0;
        for( ; (i < Len) && *p; ++i, ++p )
        {
            buf[i] = *p;
        }
        return ( Len == whio_dev_writeat( dev, 0, buf, Len ) )
            ? 0
            : whio_rc.IOError;
    }
}

/** @internal
   Writes whio_udb_sizeof_magic bytes at position 0 of
   both of db's devices.

   Returns 0 on success.

   DOES NOT LOCK MUTEX.
*/
static int whio_udb_magic_write( whio_udb * db )
{
    if( ! db || !db->dev )
    {
        return whio_rc.ArgError;
    }
    else
    {
        return whio_udb_magic_write_dev( db->dev, WHIO_UDB_MAGIC_STRING );
    }
}

/** @internal
   Consistency-checking character for dbopts.
*/
static unsigned char whio_udb_funcs_label_tag_char = 'F';

/** @internal

    Writes up to whio_udb_funcs_name_length bytes from str, which must be a
    NULL-terminated string of at most (whio_udb_sizeof_funcs_label-1)
    bytes (the -1 is for the trailing NULL). The write is done at
    position whio_udb_pos_funcs_label of the storage.

    If str is too long, whio_rc.RangeError is returned. If it is too short,
    it is padded with NULLS.

    Returns 0 on success.
*/
static int whio_udb_funcs_label_write( whio_udb * db, char const * str )
{
    const whio_size_t slen = (str && *str) ? strlen(str) : 0;
    if( !db )
    {
        return whio_rc.ArgError;
    }
    else if( slen >= whio_udb_funcs_name_length )
    {
        return whio_rc.RangeError;
    }
    else
    {
        enum { Len = whio_udb_sizeof_funcs_label };
        char buf[Len];
        buf[0] = whio_udb_funcs_label_tag_char;
        if( slen )
        {
            memcpy( buf+1, str, slen );
        }
        memset( buf+1+slen, 0, Len-slen );
        //UDBG(("Writing function set name: [%s]", buf+1));
        return ( Len == whio_dev_writeat( db->dev, whio_udb_pos_funcs_label, buf, Len ) )
            ? 0
            : whio_rc.IOError;
    }
}
/**
   Reads the function set label from db. On success it writes
   whio_udb_funcs_name_length bytes to dest. It may or may not have
   had space for the null terminator, so dest should provide its own
   to be safe.
*/
static int whio_udb_funcs_label_read( whio_udb * db, char * dest )
{
    if( !db )
    {
        return whio_rc.ArgError;
    }
    else
    {
        enum { Len = whio_udb_sizeof_funcs_label };
        char buf[Len] = {0};
        if( Len != whio_dev_readat( db->dev,whio_udb_pos_funcs_label, buf, Len ) )
        {
            return whio_rc.IOError;
        }
        else if( whio_udb_funcs_label_tag_char != buf[0] )
        {
            return whio_rc.ConsistencyError;
        }
        else
        {
            buf[Len-1] = 0;
            memcpy( dest, buf+1, whio_udb_funcs_name_length );
            //UDBG(("Read function set name: [%s]", buf+1));
            return 0;
        }
    }
}

/** @internal
   Reads whio_udb_sizeof_magic from position 0 of
   dev and compares them to cmp.

   Returns 0 on success. Neither dev nor cmp may be NULL.
*/
static int whio_udb_magic_read_dev( whio_dev * dev, char const * cmp )
{
    if( ! dev || ! cmp)
    {
        return whio_rc.ArgError;
    }
    else
    {
        enum { Len = whio_udb_sizeof_magic };
        unsigned char buf[Len];
        memset( buf, 0, sizeof(Len) );
        if( Len != whio_dev_readat( dev, 0, buf, Len ) )
        {
            return whio_rc.IOError;
        }
        return ( 0 == memcmp( cmp, (char const *)buf, Len ) )
            ? 0
            : whio_rc.ConsistencyError;
    }
}

#if 0
/** @internal
   Reads the magic bytes from both of db's devices and ensures
   they have the expected values.

   Returns 0 on success.
*/
static int whio_udb_magic_read( whio_udb * db )
{
    return ( ! db || !db->dev )
        ? whio_rc.ArgError
        : whio_udb_magic_read_dev( db->dev, WHIO_UDB_MAGIC_STRING )
        ;
}
#endif

/** @internal
   Consistency-checking character for dbopts.
*/
static unsigned char whio_udb_dbopt_tag_char = 'O';

/** @internal
   Writes opt to position whio_udb_pos_dbopt of dev.

   Returns 0 on success.
*/
static int whio_udb_dbopt_write_dev( whio_dev * dev, whio_udb_opt const * opt )
{
    if( ! dev || !opt )
    {
        return whio_rc.ArgError;
    }
    else
    {
        enum { Len = whio_udb_sizeof_dbopt };
        unsigned char buf[Len];
        unsigned char * at = buf+1;
        buf[0] = whio_udb_dbopt_tag_char;
        memset(buf+1, 0, Len-1);
        at += whio_encode_size_t( at, opt->hashSize );
        at += whio_encode_size_t( at, opt->keyLen );
        at += whio_encode_size_t( at, opt->dataLen );
        assert( Len == (at - buf) );
        return
            ( Len == whio_dev_writeat( dev, whio_udb_pos_dbopt, buf, Len ) )
            ? 0
            : whio_rc.IOError;
    }
}

/** @internal

Returns the on-storage position in db->dev of the given hashtable index,
or 0 on error. Hashtable indexes are 0-based.
*/
static whio_size_t whio_udb_hashent_pos( whio_udb const * db, whio_size_t htIndex )
{
    return !db
        ? 0
        : (
           (htIndex >= db->opt.hashSize)
           ? 0
           //: (db->osz[whio_udb_osz_pos_ht] + (htIndex * db->osz[whio_udb_osz_sizeof_hashent]))
           : (whio_udb_pos_hashtable + (htIndex * whio_udb_sizeof_hashent))
           )
        ;
}

/**
   Returns a hashtable offset (in db) for the given key. If !db and
   !key then 0 is returned. If !key and db is not NULL, a number equal
   to or greater than db->opt.hashSize is returned on error. On
   success a number less than db->opt.hashSize (possibly 0) is
   returned.  Thus the only way to know if 0==error is to check if key
   is not NULL.
*/
static whio_size_t whio_udb_hash( whio_udb * db, void const * key )
{
    if( ! db || !key ) return db ? db->opt.hashSize : 0;
    else
    {
        const whio_size_t klen = whio_udb_key_length( db, key );
        const whio_udb_hash_t h = db->funcs.hash( key, klen );
        return h ? (h % db->opt.hashSize) : db->opt.hashSize;
    }
}

/** @internal
    Consistency-checking character for whio_udb data record entries.
*/
static unsigned char whio_udb_datrec_tag_char = 'D';

/** @internal
    Consistency-checking character for whio_udb hashtable entries.
*/
static unsigned char whio_udb_hashent_tag_char = 'H';
/** @internal
    Consistency-checking character for whio_udb hints entries.
*/
static unsigned char whio_udb_hints_tag_char = 'h';

/** @internal
   Encodes datID into dest, which must be at least
   whio_udb_sizeof_hashent bytes long. On success exactly
   whio_udb_sizeof_hashent bytes are written to dest and 0 is
   returned. It should only fail if !dest.
*/
static int whio_udb_hashent_encode( unsigned char * dest, whio_size_t datID )
{
    if( !dest ) return whio_rc.ArgError;
    else
    {
        enum { Len = whio_udb_sizeof_hashent };
        *(dest++) = whio_udb_hashent_tag_char;
        return (whio_sizeof_encoded_size_t == whio_encode_size_t( dest, datID ))
            ? 0
            : whio_rc.InternalError;
    }
}

/** @internal

Tries to decode src, which must be whio_udb_sizeof_hashent bytes long, as
a hashtable entry value. On success returns 0 and sets *id to the
value. On error non-zero is returned and id is not modified.
*/
static int whio_udb_hashent_decode( unsigned char const * src, whio_size_t * id )
{
    if( !src || !id ) return whio_rc.ArgError;
    else
    {
        enum { Len = whio_udb_sizeof_hashent };
        if( *(src++) != whio_udb_hashent_tag_char )
        {
            return whio_rc.ConsistencyError;
        }
        else
        {
            return whio_decode_size_t( src, id );
        }
    }
}

/** @internal
   Writes the given ndxID to the given hashtable index in db->dev.idx.

   Returns 0 on success.
*/
static int whio_udb_hashent_write( whio_udb * db, whio_size_t htIndex, whio_size_t datID )
{
    whio_size_t const pos = whio_udb_hashent_pos( db, htIndex );
    if( ! pos ) return whio_rc.RangeError;
    else
    {
        enum { Len = whio_udb_sizeof_hashent };
        unsigned char buf[Len];
        int rc = whio_udb_hashent_encode( buf, datID );
        UDB_METRIC_INC(db,hashWrites);
        return rc
            ? rc
            : (Len == whio_dev_writeat( db->dev, pos, buf, Len ) )
              ? 0
              : whio_rc.IOError;
    }
}

/** @internal

   Reads the given hashtable index and assigns the value read there to *id.

   Returns 0 on success. On error *id is not modified.
*/
static int whio_udb_hashent_read( whio_udb * db, whio_size_t htIndex, whio_size_t * id )
{
    whio_size_t const pos = whio_udb_hashent_pos( db, htIndex );
    if( ! pos ) return whio_rc.RangeError;
    else
    {
        enum { Len = whio_udb_sizeof_hashent };
        unsigned char buf[Len];
        int rc = 0;
        UDB_METRIC_INC(db,hashReads);
        if(Len != whio_dev_readat( db->dev, pos, buf, Len ) )
        {
            rc = whio_rc.IOError;
        }
        else
        {
            rc = whio_udb_hashent_decode( buf, id );
        }
        return rc;
    }
}

/** @internal

    Wipes all hashtable entries clean.

    Returns 0 on success.
*/
static int whio_udb_hashtable_format( whio_udb * db )
{
    if( ! db ) return whio_rc.ArgError;
    else
    {
        int rc = whio_rc.RangeError;
        whio_size_t n;
        enum { Len = whio_udb_sizeof_hashent };
        unsigned char buf[Len+1];
        buf[Len]=0;
        whio_size_t pos = 0;
        rc = whio_udb_hashent_encode( buf, 0 );
        if( rc ) return rc;
        pos = whio_udb_hashent_pos( db, 0 );
        if( ! pos )
        {
            db->err = rc = whio_rc.InternalError;
        }
        for( n = 0; !rc && (n < db->opt.hashSize); ++n, pos += Len )
        {
            UDB_METRIC_INC(db,hashWrites);
            rc = (Len == whio_dev_writeat( db->dev, pos, buf, Len ) )
                ? 0
                : whio_rc.IOError;
        }
        return rc;
    }
}

/** @interal
   Internal. Encodes h and writes it to position whio_udb_pos_hints
   of dev.

   Returns 0 on success.
*/
static int whio_udb_hints_write_dev( whio_dev * dev, whio_udb_hints const * h )
{
    if( ! dev ) return whio_rc.ArgError;
    else
    {
        enum { Len = whio_udb_sizeof_hints };
        unsigned char buf[Len];
        unsigned char * at = buf+1;
        memset( buf+1, 0, Len-1 );
        buf[0] = whio_udb_hints_tag_char;
        at += whio_encode_size_t( at, h->freeList );
        at += whio_encode_size_t( at, h->usedList );
        at += whio_encode_size_t( at, h->highestID );
        if(0)
        {
            UDBG(("writing %"WHIO_SIZE_T_PFMT" bytes of hint data at pos %"WHIO_SIZE_T_PFMT" of device @%p."
                  " Free-list=%"WHIO_SIZE_T_PFMT".",
                  (whio_size_t)Len, whio_udb_pos_hints,(void const *)dev,h->freeList));
        }
        return ( Len == whio_dev_writeat( dev, whio_udb_pos_hints, buf, Len ) )
            ? 0
            : whio_rc.IOError;
    }
}

/** @internal

    Writes db->hints data to the db->dev devices.*

    Returns 0 on success.
*/
static int whio_udb_hints_write( whio_udb * db )
{
    if( ! db
        //|| ! db->dev.idx
        || !db->dev ) return whio_rc.ArgError;
    else
    {
        UDB_METRIC_INC(db,hintWrites);
        int rc = whio_udb_hints_write_dev( db->dev, &db->hints );
#if 0
        if( 0 == rc ) rc = whio_udb_hints_write_dev( db->dev.idx, &db->hints.idx );
#endif
        return rc;
    }
}

/**
   Returns the on-storage position (for db->dev) of the given
   1-based record ID, or 0 if !db or !id.
*/
static whio_size_t whio_udb_record_pos( whio_udb * db,
                                        whio_size_t id )
{
    return (!db || !id)
        ? 0
        : (db->osz[whio_udb_osz_pos_data] + ((id-1)*db->osz[whio_udb_osz_sizeof_record]))
        ;
}


/** @internal

  Encodes r to dest. If howDeep is true then the key/value parts of r
  are also encoded, otherwise r->key and r->data are ignored. r->key
  must be at least whio_udb_key_length(db,NULL) bytes long and r->data
  must be at least whio_udb_data_length(db,NULL) bytes long.

   The required length of dest depends on the howDeep parameter:

   - (howDeep<0): whio_udb_sizeof_record_header bytes

   - (howDeep==0): as above, plus whio_udb_key_length(db,NULL) + 1 bytes.

   - (howDeep>0): as above, plus whio_udb_data_length(db,NULL) + 1 bytes.

   The encoded data is suitable for dumping as-is to disk.

   Trivia: some of the other documentation/code refers the three
   different read/write sizes "small" (howDeep<0), "medium"
   (howDeep==0), and "large" (howDeep>0) reads/writes.
*/
static int whio_udb_record_encode( whio_udb const * db,
                                   unsigned char * dest,
                                   whio_udb_record const * r,
                                   short howDeep
                                   )
{
    if( !r || !dest ) return whio_rc.ArgError;
    if( howDeep && !db ) return whio_rc.ArgError;
    else if( (r->keyLen > db->opt.keyLen)
             || (r->dataLen > db->opt.dataLen) )
    {
        return whio_rc.RangeError;
    }
    else
    {
        enum { Len = whio_udb_sizeof_record_header };
        unsigned char const * origin; origin = dest;
        *(dest++) = whio_udb_datrec_tag_char;
        /* maintenance reminder: store r->hash before
           the others because of its differing type.
           We can then simplify the decode operation
           via a loop.
        */
        dest += whio_encode_uint32( dest, r->hash );
        dest += whio_encode_size_t( dest, r->id );
        dest += whio_encode_size_t( dest, r->nextCollision );
        dest += whio_encode_size_t( dest, r->prevFree );
        dest += whio_encode_size_t( dest, r->nextFree );
        dest += whio_encode_size_t( dest, r->keyLen );
        dest += whio_encode_size_t( dest, r->dataLen );
        assert( origin == (dest-whio_udb_sizeof_record_header) );
        if( howDeep < 0 ) return 0;
        assert( r->keyLen <= db->opt.keyLen );
        memset( dest, 0, db->opt.keyLen );
        if( r->keyLen )
        {
            assert( r->key );
            //UDBG(("dest@%p, r->key@%p",(void const *)dest, (void const *)r->key));
            memcpy( dest, r->key, r->keyLen );
        }
        dest += db->opt.keyLen;
        *(dest++) = 0;
        if( howDeep > 0 )
        {
            assert( r->dataLen <= db->opt.dataLen );
            memset( dest, 0, db->opt.dataLen );
            if( r->dataLen )
            {
                assert( r->data );
                memcpy( dest, r->data, r->dataLen );
            }
            dest += db->opt.dataLen;
            *(dest++) = 0;
        }
        return 0;
    }
}


/**
   Writes r to db. r must be a fully populated object.
   
   buffer must be valid memory with a length depending on howDeep. See
   whio_udb_record_encode() for the details of that parameter.

   whio_udb_sizeof_record_header bytes
*/
static int whio_udb_record_write( whio_udb * db,
                                  unsigned char * buffer,
                                  whio_udb_record const * r,
                                  short howDeep )
{
    if( ! db || ! r || !r->id ) return whio_rc.ArgError;
    const whio_size_t pos = whio_udb_record_pos( db, r->id );
    if( ! pos ) return whio_rc.RangeError;
    else
    {
        const whio_size_t len = (howDeep>0)
            ? whio_udb_record_buffer_size( db->opt.keyLen, db->opt.dataLen )
            : (howDeep==0)
            ? whio_udb_sizeof_record_header + db->opt.keyLen +1/*NULL*/
            : whio_udb_sizeof_record_header;
        int rc = whio_udb_record_encode( db, buffer, r, howDeep );
        if(!rc)
        {
            if( len == whio_dev_writeat( db->dev, pos, buffer, len ) )
            {
                if( db->hints.highestID < r->id )
                {
                    db->hints.highestID = r->id;
                    rc = whio_udb_hints_write( db );
                }
            }
            else
            {
                rc = whio_rc.IOError;
            }
        }
        if( !rc )
        {
            if( howDeep < 0 ) UDB_METRIC_INC(db,recordWritesShort);
            else if( howDeep == 0 ) UDB_METRIC_INC(db,recordWritesMed);
            else UDB_METRIC_INC(db,recordWritesLong);
        }
        return rc;
    }
}

/**
   Decodes src into r. The length requirement for the src parameter
   depend on the value of howDeep, exactly as documented for
   whio_udb_record_encode().

   Returns 0 on success.

   After returning, if (howDeep==0) then r->key and points directly
   back into src. If (howDeep>0) then both r->key and r->data point
   back into src.
*/
static int whio_udb_record_decode( whio_udb const * db,
                                   unsigned char const * src,
                                   whio_udb_record * r,
                                   short howDeep )
{
    if( !src || !r ) return whio_rc.ArgError;
    else
    {
        unsigned char const * at = src;
        if( *(at++) != whio_udb_datrec_tag_char )
        {
            return whio_rc.ConsistencyError;
        }
        whio_size_t * li[] = {
        &r->id,
        &r->nextCollision,
        &r->prevFree,
        &r->nextFree,
        &r->keyLen,
        &r->dataLen
        };
        int rc = 0;
        unsigned short i = 0;
        rc = whio_decode_uint32( at, &r->hash );
        if( rc ) return rc;
        at += whio_sizeof_encoded_uint32;
        for( ; i < (sizeof(li)/sizeof(li[0])); ++i )
        {
            rc = whio_decode_size_t( at, li[i] );
            if( rc ) return rc;
            at += whio_sizeof_encoded_size_t;
        }
        if( howDeep < 0 ) return 0;
        r->key = at;
        at += db->opt.keyLen + 1/*NULL*/;
        if( howDeep > 0 )
        {
            r->data = at;
        }
        return 0;
    }
}


/** @internal

   Reads given record from the db, storing the raw record in
   the given buffer and decoding its data to r.

   buffer's size requirements depend on the value of howDeep.  See
   whio_udb_record_encode() for details of the howDeep parameter.
   
   Returns 0 on success.
*/
static int whio_udb_record_read( whio_udb * db,
                                 unsigned char * buffer,
                                 whio_size_t id,
                                 whio_udb_record * r,
                                 short howDeep )
{
    const whio_size_t pos = whio_udb_record_pos( db, id );
    if( ! r || !pos ) return whio_rc.ArgError;
    if( ! pos ) return whio_rc.RangeError;
    else
    {
        int rc = 0;
        const whio_size_t len = (howDeep>0)
            ? whio_udb_record_buffer_size( db->opt.keyLen, db->opt.dataLen )
            : (howDeep==0)
              ? (whio_udb_sizeof_record_header + db->opt.keyLen +1/*NULL*/)
              : whio_udb_sizeof_record_header
            ;
        if( len != whio_dev_readat( db->dev, pos, buffer, len ) )
        {
            return whio_rc.IOError;
        }
        rc = whio_udb_record_decode( db, buffer, r, howDeep );
        if( ! rc && (db->hints.highestID < r->id) )
        {
            whio_size_t oldHigh; oldHigh = db->hints.highestID;
            db->hints.highestID = r->id;
            if( db->iomode > 0 )
            {
                rc = whio_udb_hints_write( db );
            }
            else
            {
                UDBG(("WARNING: db->hints.highestID is %"WHIO_SIZE_T_PFMT
                      ", but we just read in a record with a higher ID: "
                      "%"WHIO_SIZE_T_PFMT"!",
                      oldHigh, r->id));
            }
        }
        if( howDeep < 0 ) UDB_METRIC_INC(db,recordReadsShort);
        else if( howDeep == 0 ) UDB_METRIC_INC(db,recordReadsMed);
        else UDB_METRIC_INC(db,recordReadsLong);
        return rc;

    }
}


/** @internal

   Updates db->hints.freeList to insert r
   as the first free item. This modifies r->nextFree
   and r->prevFree.

   If r->iobuf.buffer is set then it is assumed to be at least long
   enough to hold a fully encoded record, and that memory is used for
   the encoded/decoding of the record, otherwise db->rbuf is
   used.
   
   ino and its right-side neighbor are flushed.

   Returns 0 on success. On error the free list may be in an undefined
   state.

   On success the results are saved and modified, with one caveat: The
   on-storage key/value are nulled out, but r->key and r->data are not
   touched.


   If r->iobuf.buffer is NULL then this routine must unfortunately use
   db->rbuf in order to ensure that r's key is wiped. That invalidates
   any prior results which used that buffer, of course.

   On success the r->key/data/iobuf.buffer pointers are not modified,
   but all (or most) other members are.
*/
static int whio_udb_record_to_freelist( whio_udb * db, whio_udb_record * r )
{

    int rc = 0;
    whio_udb_record on = whio_udb_record_empty;
    if( db->hints.freeList == r->id ) return rc;
    else if( db->hints.freeList )
    {
        if( 0 )
        {
            UDBG(("Updatine free-list head=#%"WHIO_SIZE_T_PFMT" to be "
                  "r->id=%"WHIO_SIZE_T_PFMT,
                  db->hints.freeList, r->id));
        }
        REC_SMALL_BUF(smallBuf);
        rc = whio_udb_record_read( db, smallBuf, db->hints.freeList, &on, -1 );
        if( rc ) return rc;
        on.prevFree = r->id;
        rc = whio_udb_record_write( db, smallBuf, &on, -1 );
        if( rc ) return rc;
        r->nextFree = on.id;
    }
    else
    { /* This inode is now the head and tail of the free list. */
        r->nextFree = 0 /* if next/prevFree are not zeroed we can get
                           cycles in the free-list or orphaned
                           records. Been there, debuggered that. */
            ;
    }

    {
        unsigned char * ebuf = r->iobuf.buffer ? r->iobuf.buffer : db->rbuf->iobuf.buffer;
        whio_udb_record wipe;
        r->prevFree
            = r->nextCollision
            = r->keyLen
            = r->dataLen = 0;
        r->hash = 0 /* possibly different data type from above members,
                       so cannot asign on that same line. */;
        wipe = *r /* we need to wipe key/data bytes and don't want to modify
                     r's directly. */;
        wipe.key = wipe.data = ebuf + whio_udb_sizeof_record_header;
        memset( ebuf, 0, whio_udb_sizeof_record(db) );
        rc = whio_udb_record_write( db, ebuf, &wipe, 1 );
        if(0)
        {
            UDBG(("Free-list head=#%"WHIO_SIZE_T_PFMT", "
                  "r->id=%"WHIO_SIZE_T_PFMT", r->nextFree=%"WHIO_SIZE_T_PFMT"",
                  db->hints.freeList, r->id, r->nextFree));
        }
        if( !rc )
        {
            db->hints.freeList = r->id;
            rc = whio_udb_hints_write( db );
        }
        UDB_METRIC_INC(db,freeListAdd);
        return rc;
    }
}

/** @internal
   Updates db->hints.freeList to remove r from
   the free-list.

   r and its neighbors are written to storage.

   Returns 0 on success. On error, r and the free list may be in
   undefined states.

   achtung: on error the contents of r->iobuf.buffer might contain
   data for another record.
*/
static int whio_udb_record_from_freelist( whio_udb * db,
                                          whio_udb_record * r )
{

    whio_udb_record on = whio_udb_record_empty;
    int rc = 0;
    int i = 0;
    REC_SMALL_BUF(smallBuf);
    if(0)
    {
        UDBG(("%"WHIO_SIZE_T_PFMT"<-%"WHIO_SIZE_T_PFMT"->%"WHIO_SIZE_T_PFMT"\n",
              r->prevFree, r->id, r->nextFree ));
    }
    if( !r->nextFree && !r->prevFree )
    {
        if( db->hints.freeList == r->id )
        {
            db->hints.freeList = 0;
            rc = whio_udb_hints_write( db );
        }
        return rc;
    }
    for( ; i < 2; ++i )
    {
        whio_size_t nid = (0==i) ? r->prevFree : r->nextFree;
        if( ! nid ) continue;
        rc = whio_udb_record_read( db, smallBuf, nid, &on, -1 );
        if( rc ) break;
        assert( nid == on.id );
        if( 0 == i )
        { /* we're on the left. */
            on.nextFree = r->nextFree;
            if(0)
            {
                UDBG(("Re-linking free-list record #%"WHIO_SIZE_T_PFMT
                      "->nextFree to %"WHIO_SIZE_T_PFMT,
                      on.id, on.nextFree ));
            }
        }
        else
        { /* we're on the right */
            on.prevFree = r->prevFree;
            if(0)
            {
                UDBG(("Re-linking free-list record #%"WHIO_SIZE_T_PFMT
                      "->prevFree to %"WHIO_SIZE_T_PFMT,
                      on.id, on.prevFree ));
            }
        }
        rc = whio_udb_record_write( db, smallBuf, &on, -1 );
        if( rc ) break;
    }
    if( !rc )
    {
        UDB_METRIC_INC(db,freeListRemove);
        db->hints.freeList = r->nextFree;
        if(0)
        {
            UDBG(("Removed record #%"WHIO_SIZE_T_PFMT" from free-list. "
                  "db->hints.freeList=%"WHIO_SIZE_T_PFMT,
                  r->id, db->hints.freeList ));
        }
        r->nextFree = r->prevFree = 0;
        rc = whio_udb_record_write( db, smallBuf, r, -1 );
        if(!rc) rc = whio_udb_hints_write(db);
    }
    return rc;
}



static int whio_udb_hints_read( whio_udb * db )
{
    if( ! db ) return whio_rc.ArgError;
    else
    {
        enum { Len = whio_udb_sizeof_hints };
        unsigned char buf[Len];
        unsigned char * at = buf;
        int rc = 0;
        if( Len != whio_dev_readat( db->dev, whio_udb_pos_hints, buf, Len ) )
        {
            rc = whio_rc.IOError;
        }
        else if( *(at++) != whio_udb_hints_tag_char )
        {
            return whio_rc.ConsistencyError;
        }
        else do
        {
#define DEC(WHERE) \
            rc = whio_decode_size_t( at, &(db->hints.WHERE) );  \
            if( rc ) break; \
            at += whio_sizeof_encoded_size_t

            DEC(freeList);
            DEC(usedList );
            DEC(highestID);
#undef DEC
            //UDBG(("highestID=#%"WHIO_SIZE_T_PFMT,db->hints.highestID));
        } while(0);
        return rc;
    }
}



whio_size_t whio_udb_key_length( whio_udb const * db, void const * key )
{
    if( !db ) return 0;
    else
    {
        return (!key || (NULL==db->funcs.keylen))
            ? db->opt.keyLen
            : db->funcs.keylen(key)
            ;
    }
}

whio_size_t whio_udb_data_length( whio_udb const * db, void const * data )
{
    if( !db ) return 0;
    else
    {
        return (!data || (NULL==db->funcs.datalen))
            ? db->opt.dataLen
            : db->funcs.datalen(data)
            ;
    }
}

/** @internal

Sets up a record object's client-data fields.

Sets:

r->key = key
r->data = val

r->keyLen = key ? whio_udb_key_length(db,key) : 0

r->dataLen = val ? whio_udb_data_length(db,val) : 0


Returns whio_rc.RangeError if either of r->keyLen or r->dataLen are greater
than db->opt.keyLen and db->opt.dataLen, respectively.
*/
#if 0
static int whio_udb_record_populate( whio_udb * db, whio_udb_record * r, void const * key, void const * val )
{
    if( !db || !r ) return whio_rc.ArgError;
    else
    {
        /**
           TODO: reimpl this to use r->iobuf.buffer, if set, and copy
           key/val to those locations and point r->key/r->data to them.
        */
        
        r->keyLen = key
            ? whio_udb_key_length( db, key )
            : 0
            ;
        r->dataLen = val
            ? whio_udb_data_length( db, val )
            : 0
            ;
        r->key = key;
        r->data = val;
        return (r->keyLen <= db->opt.keyLen)
            && (r->dataLen <= db->opt.dataLen)
            ? 0
            : whio_rc.RangeError
            ;
    }
}
#endif

/** @internal

Creates a new record block in db's storage. If markAsUsed, the block
is not added to the free-list, otherwise it is. If dest is not NULL
then the new record's core info (minus key/data) is copies to dest.
dest->key, dest->value, and dest->iobuf.buffer are never modified,
but the content they point to might be.

If dest->iobuf.buffer is not NULL then that memory is used for the
i/o decoding/encoding buffer, otherwise db->rbuf is used.

Returns 0 on success or any range of other values on error.
*/
static int whio_udb_record_add_new( whio_udb * db, whio_udb_record * dest,
                                    bool markAsUsed )
{
    if( !db ) return whio_rc.ArgError;
    else if( db->iomode < 1 )  return whio_rc.AccessError;
    else
    {
        unsigned char * ebuf = (dest && dest->iobuf.buffer)
            ? dest->iobuf.buffer
            : db->rbuf->iobuf.buffer;
        whio_udb_record r = whio_udb_record_empty;
        whio_size_t id = 1 + db->hints.highestID;
        //UDBG(("id=%"WHIO_SIZE_T_PFMT,id));
        int rc = 0;
        if(id)
        { /* make sure it doesn't already exist. */
            REC_SMALL_BUF(sbuf);
            rc = whio_udb_record_read( db, sbuf, id, &r, -1 );
            if( 0 == rc )
            {
                UDBG(("Shouldn't happen: got an existing record ID (#%"WHIO_SIZE_T_PFMT
                      ") while trying to allocate a new block!", id ));
                return db->err = whio_rc.InternalError;
            }
            rc = 0 /* okay - we'll create it... */;
            r = whio_udb_record_empty;
        }
        r.id = id;
        /**
           Reminder to self: we really do need to write a whole
           record here, instead of just a short record. If
           we do a short write, it will likely work in most cases,
           but if it is the very last record then we need
           to have the key/data bytes in place or the next
           read() on it may fail.
        */
        rc = whio_udb_record_write( db, ebuf, &r, 1);
        if( !rc && !markAsUsed )
        {
            rc = whio_udb_record_to_freelist( db, &r );
        }
        if( (0 == rc) && dest )
        {
            whio_udb_record_cp_no_ptr( &r, dest );
        }
        return rc;
    }
}
whio_size_t whio_udb_add_empty_records( whio_udb * db, whio_size_t n )
{
    if( ! db || !n ) return 0;
    else
    {
        MLOCK_OR_FAIL(db,0);
        int rc = 0;
        whio_size_t i = 0;
        for( ; i < n; )
        {
            rc = whio_udb_record_add_new( db, NULL, false );
            if( rc ) break;
            ++i;
        }
        MUNLOCK_RETURN(db,i);
    }
}
/**
   Searches for the next free record in db. If it is found,
   dest is modified to contain its state.

   If markAsUsed is true then the record is "allocated", removed
   from the free-list. Otherwise it is not allocated and should
   not be used.

   If markAsUsed is false and there are no free entries then
   whio_rc.AccessError is returned (meaning basically it got "no
   access to allocate a new record").
   
   If dest->iobuf.buffer is not NULL then it will be used as
   scratch-space for the encode/decode operations, otherwise
   db->rbuf is used.

   On error, the contents of dest->iobuf.buffer are unspecified,
   and thus dest is in an undefined state.

   This call does not change the values of dest->key, dest->data,
   and dest->iobuf, but certainly does change the contents of
   the memory those members point to.
*/
static int whio_udb_record_next_free( whio_udb * db, whio_udb_record * dest, bool markAsUsed )
{
    if( ! db || ! dest ) return whio_rc.ArgError;
    else if( markAsUsed && (db->iomode<1) )
    {
        return whio_rc.AccessError;
    }
    int rc = 0;
    whio_udb_record r = whio_udb_record_empty;
    whio_size_t id = db->hints.freeList;
    if( ! id )
    {
        if( ! markAsUsed ) return whio_rc.AccessError;
        else
        {
            rc = whio_udb_record_add_new( db, &r, markAsUsed );
            if( rc ) return rc;
            id = r.id;
            whio_udb_record_cp_no_ptr( &r, dest );
            return 0;
        }
    }
    else
    {
        REC_SMALL_BUF(smallBuf);
        while(id)
        {
            rc = whio_udb_record_read( db, smallBuf, id, &r, -1 );
            if( rc ) return rc;
            if( r.keyLen )
            { /* should never happen. */
                UDBG(("WARNING: is-free check of record #%"WHIO_SIZE_T_PFMT" failed! This shouldn't happen!",r.id));
                if( r.nextFree )
                {
                    UDBG(("Trying r.nextFree #%"WHIO_SIZE_T_PFMT".",r.nextFree));
                    if( db->hints.freeList == r.id )
                    {
                        db->hints.freeList = r.nextFree;
                    }
                    id = r.nextFree;
                    continue;
                }
                else
                {
                    /**
                       Hashtable chain appears to be in an undefined state :(.
                    */
                    return db->err = whio_rc.ConsistencyError;
                }
            }
            break;
        }
    }
    if( ! id )
    { /* assume we are full */
        return whio_rc.DeviceFullError;
    }
    if( markAsUsed )
    {
        rc = whio_udb_record_from_freelist( db, &r );
    }
    if( ! rc )
    {
        whio_udb_record_cp_no_ptr( &r, dest );
    }
    //UDBG(("next-free=#%"WHIO_SIZE_T_PFMT".",r.id));
    return rc;
}


/** @internal

   Writes opt at position whio_udb_pos_dbopt of both of db's devices.

   Returns 0 on success.
*/
static int whio_udb_dbopt_write( whio_udb * db )
{
    return ( ! db || !db->dev )
        ? whio_rc.ArgError
        : whio_udb_dbopt_write_dev( db->dev, &db->opt )
        ;
}

/** @internal
   Populates opt from position whio_udb_pos_dbopt of dev.

   Returns 0 on success. On error opt is in an undefined state.
*/
static int whio_udb_dbopt_read_dev( whio_dev * dev, whio_udb_opt * opt )
{
    if( ! dev || !opt )
    {
        return whio_rc.ArgError;
    }
    else
    {
        enum { Len = whio_udb_sizeof_dbopt };
        unsigned char buf[Len];
        unsigned char * at = buf;
        int rc = 0;
        memset(buf, 0, Len);
        if( Len != whio_dev_readat( dev, whio_udb_pos_dbopt, buf, Len ) )
        {
            return whio_rc.IOError;
        }
        if( *at++ != whio_udb_dbopt_tag_char )
        {
            return whio_rc.ConsistencyError;
        }
#define DEC(WHERE) \
        rc = whio_decode_size_t( at, &(WHERE) ); \
        if( rc ) return rc; \
        at += whio_sizeof_encoded_size_t

        DEC(opt->hashSize);
        DEC(opt->keyLen);
        DEC(opt->dataLen);
#undef DEC
        return 0;
    }
}

/** @internal

Reads the db options from positon whio_udb_pos_dbopt of db. on success
it returns 0 and updates db->osz and db->opt.
*/
static int whio_udb_dbopt_read( whio_udb *db )
{
    whio_udb_opt opt = whio_udb_opt_empty;
    int rc = whio_udb_dbopt_read_dev( db->dev, &opt );
    if( 0 == rc )
    {
        db->opt = opt;
#if 0
        db->opt.hashSize = opt.hashSize;
        db->opt.keyLen = opt.keyLen;
        db->opt.dataLen = opt.dataLen;
#endif
        whio_udb_init_osz( db );
    }
    return rc;
}

int whio_udb_dev_format( whio_udb ** db_,
                         whio_dev * dev,
                         whio_udb_opt const * opt,
                         char const * funcSetName )
{
    int rc = 0;
    const char ownsDb = (db_ && *db_) ? 0 : 1;
    whio_udb * db = NULL;
    whio_udb_funcs funcs = whio_udb_funcs_empty;
    if( ! opt || ! dev || (!funcSetName || !*funcSetName) ) rc = whio_rc.ArgError;
    else if( 0 != (rc=whio_udb_funcs_parse(funcSetName,&funcs)) )
    {
        return whio_rc.RangeError;
    }
    else do
    {
        /**
           FIXME: try to get a lock on the dev before proceeding, and
           bail if we cannot get an immediate lock (to avoid hosing
           an opened db).
        */
        db = ownsDb ? whio_udb_alloc() : *db_;
        if( ! db )
        {
            rc = whio_rc.AllocError;
            break;
        }
        db->dev = dev;

        if( 0 == whio_udb_probe_lockability(db) )
        { /* storage seems lockable. Make sure we can lock it
             before hosing a useful device... */
            db->iomode = 1 /* kludge: dev might not know its i/o mode yet. */;
            rc = whio_udb_storage_lock_all( db, false );
            /**
               We expect this lock to pass unless someone has the db
               open, in which case we need to abort the format (as
               opposed to waiting for a lock then hosing the udb once
               we get it).
             */
            if( rc ) break;
        }

        db->funcs = funcs;
        /*
          Reminder: ignore the truncate() return codes because
          subdevices do not support truncate but nonetheless might be
          useful for whio_udb. If the db won't fit in the subdevice
          we'll fail later on. And if there's trailing garbage data
          our algorithms "should" be able to ignore it and eventually
          overwrite it with new blocks.

          Note that we truncate db->dev to 1 instead of 0 as a
          special consolation to in-memory devices, as i just happen
          to know that they explicitly free their memory if truncated
          to 0 bytes.

          TODO: try to truncate to the required size in advance, as a
          consolation to whio_epfs pseudofiles (and other devices
          which can realistically run out of space).
        */
        whio_dev_truncate( db->dev, whio_udb_pos_hashtable );
        db->opt = *opt;
        whio_udb_init_osz(db);
        rc = whio_udb_magic_write( db );
        if( rc ) break;
        /**
           Reminder: certain file-based devices cannot know
           if they are writable until after a successful
           write, so we delay the check for write mode
           until a write has succeeded.

           That said, the devices MUST be writable if we've gotten
           this far, so we could just hard-coding db->iomode to 1 here
           (>0==write mode) might not be strictly insane..
        */
        db->iomode = db->dev->api->iomode( db->dev );
        rc = whio_udb_dbopt_write( db );
        if( rc ) break;
        rc = whio_udb_hints_write( db );
        if( rc ) break;
        rc = whio_udb_funcs_label_write( db, funcSetName );
        if( rc ) break;
        rc = whio_udb_hashtable_format( db );
        if( rc ) break;
    }
    while(0);
    if( db )
    {
        if( !rc && ! whio_udb_buffers( db ) )
        {
            rc = whio_rc.AllocError;
        }
        if( 0 != rc )
        {
            whio_udb_storage_unlock_some( db, true );
            db->dev = NULL /* give db->dev back to caller */;
            if( ownsDb )
            {
                whio_udb_free(db);
            }
            else
            {
                whio_udb_close(db);
            }
        }
        else
        {
            if( ownsDb )
            {
                if( db_ )
                {
                    *db_ = db;
                    whio_udb_storage_unlock_some( db, false );
                }
                else
                {/* discard db */
                    whio_udb_storage_unlock_some( db, true );
                    db->dev = NULL /* give db->dev back to caller */;
                    whio_udb_free(db);
                }
            }
        }
    }
    return rc;
}

int whio_udb_dev_open( whio_udb ** tgt, whio_dev * dev )
{
    if( ! tgt || !dev ) return whio_rc.ArgError;
    else if( ! whio_dev_is_udb(dev) )
    {
        return whio_rc.TypeError;
    }
    else
    {
        whio_udb_funcs f = whio_udb_funcs_empty;
        char ownsDb = *tgt ? 0 : 1;
        whio_udb * db = NULL;
        int rc = 0;
        db = ownsDb ? whio_udb_alloc() : *tgt;
        if( ! db )
        {
            return whio_rc.AllocError;
        }
        db->iomode = dev->api->iomode( dev );
        db->dev = dev;
        if( !rc )
        {
            rc = whio_udb_dbopt_read( db );
        }
        if(!rc)
        { /* try to load the function set... */
            char buf[whio_udb_funcs_name_length+1];
            memset( buf, 0, sizeof(buf) );
            rc = whio_udb_funcs_label_read( db, buf );
            if( 0 == rc )
            {
                rc = whio_udb_funcs_parse( buf, &f );
                if( rc )
                {
                    UDBG(("ERROR: could not find/create function set named \"%s\"!",buf));
                    //rc = whio_rc.TypeError/*???*/;
                }
            }
        }
        if( 0 == rc )
        {
            rc = whio_udb_hints_read( db );
        }
        if( (0==rc) && ! whio_udb_buffers( db ) )
        {
            rc = whio_rc.AllocError;
        }
        if( 0 == rc )
        {
            if( ownsDb ) *tgt = db;
            db->funcs = f;
            whio_udb_probe_lockability( db ) /* ignore error: not critical. */;
        }
        else
        {
            db->dev = NULL /* return device ownership to the caller */;
            if( ownsDb )
            {
                /* We allocated db. Destroy it. */
                whio_udb_free( db );
            }
            else
            {
                /* Clean up db, but the caller must deallocate it. */
                whio_udb_close( db );
            }
        }
        return rc;
    }
}

/**
   As documented for whio_udb_close(), but if lockMutex is false then
   the caller is assumed to have locked db->mutex prior to calling.
*/
static int whio_udb_close_impl( whio_udb * db, bool lockMutex )
{
    if( ! db ) return whio_rc.ArgError;
    else
    {
        whio_mutex const mutex = db->mutex;
        if( lockMutex && mutex.lock && mutex.unlock )
        {
            mutex.lock( mutex.state );
            if( mutex.lock != db->mutex.lock )
            { /* wiped out before we could do it! */
                mutex.unlock( mutex.state );
                return whio_rc.LockingError;
            }
        }
        if( db->dev )
        {
            if( ! db->err )
            {
                if( db->iomode > 0 )
                {
                    whio_udb_flush_impl(db, false );
                }
            }
            whio_udb_storage_unlock_some( db, true );
            db->dev->api->finalize( db->dev );
            db->dev = NULL;
            db->iomode = -1;
        }
        if( db->recordBook )
        {
            if( db->rbuf )
            {
                whio_udb_record_free_impl( db, db->rbuf, false );
                db->rbuf = NULL;
            }
            WHALLOC_API(book_close)( db->recordBook );
            db->recordBook = NULL;
        }
        *db = whio_udb_empty;
        if( lockMutex && mutex.lock && mutex.unlock )
        {
            mutex.unlock( mutex.state );
        }
        return 0;
    }
}

int whio_udb_close( whio_udb * db )
{
    return whio_udb_close_impl( db, true );
}

int whio_udb_free( whio_udb * db )
{
    if( ! db ) return whio_rc.ArgError;
    else
    {
#define TRYLOCK 1
#if TRYLOCK
        whio_mutex const m = db->mutex;
        if( m.lock && m.unlock )
        {
            m.lock(m.state);
            if( m.lock != db->mutex.lock )
            {/* wiped out before we could do it! */
                m.unlock( m.state );
                return whio_rc.LockingError;
            }
        }
#endif
        whio_udb_close_impl(db, false);
        free( db );
#if TRYLOCK
        if( m.lock && m.unlock )
        {
            m.unlock(m.state);
        }
#endif
        return 0;
    }
}

int whio_udb_reap_free_records_mode( whio_udb * db, bool doAutoVacuum )
{
    if( ! db || !db->recordBook ) return whio_rc.ArgError;
    else
    {
        MLOCK_OR_FAIL(db,whio_rc.LockingError);
        WHALLOC_API(book_vacuum_auto)( db->recordBook, doAutoVacuum );
        MUNLOCK_RETURN(db,0);
    }
}

int whio_udb_reap_free_records( whio_udb * db )
{
    if( ! db ) return whio_rc.ArgError;
    else
    {
        MLOCK_OR_FAIL(db,whio_rc.LockingError);
        WHALLOC_API(book_vacuum)( db->recordBook );
        MUNLOCK_RETURN(db,0);
    }
}

bool whio_dev_is_udb( whio_dev * dev )
{
    if( ! dev ) return false;
    else
    {
        return (0 == whio_udb_magic_read_dev( dev, WHIO_UDB_MAGIC_STRING ))
            ? true
            : false;
    }
}


/** @internal

Like whio_udb_search(), but takes an optional final argument which...

If non-NULL, each record read is copied over *prev. On success if
prev->id is not 0 and is not result->id then prev points to the
previous collision item from the hash chain. This value is only needed
when we want to remove the result from the chain, and is only in the
API to support the Remove/Insert implementations. If !prev->id then no
previous record was traversed, meaning there was no hash collision.

On search failure, contents of prev are unspecified because it is
overwritten, possibly only partially, on each collision iteration.

BIG FAT WARNING: DO NOT (EVER!) rely on prev's ptr values after this
returns. They probably point to somewhere in dest, or even in
db->rbuf. Only rely on the persistent numeric fields, not the
pointers.


If dest->iobuf.buffer is non-NULL then it is assumed to be at least
whio_udb_sizeof_record(db) bytes long and it is used for
encoding/decoding the record object(s) while searching. If dest was
allocated via whio_udb_record_alloc() then its bits are fiddled with
to set that up as part of the record allocation.

*/
static whio_udb_record * whio_udb_search_impl( whio_udb * db,
                                               void const * key,
                                               whio_udb_record * dest,
                                               whio_udb_record * prev )
{
    if( ! db || !key ) return NULL;
    else
    {
        const whio_size_t klen = whio_udb_key_length( db, key );
        const whio_udb_hash_t h = whio_udb_hash( db, key );
        whio_size_t id = 0;
        if( h >= db->opt.hashSize ) return NULL;
        else
        {
            UDB_METRIC_INC(db,searches);
            whio_udb_record * rec = NULL;
            int rc = whio_udb_hashent_read( db, h, &id );
            //UDBG(("hashent #%"PRIu32" read rc=%d, id=%"WHIO_SIZE_T_PFMT,h,rc,id));
            if( ! rc ) while( id )
            {
                unsigned char * ebuf = db->rbuf->iobuf.buffer;
                rec = dest ? dest : whio_udb_record_alloc( db );
                if( ! rec )
                {
                    UDBG(("ERROR: record alloc failed!"));
                    return NULL;
                }
                if( rec->iobuf.buffer )
                {
                    ebuf = rec->iobuf.buffer;
                }
                whio_size_t k2len = 0;
                rc = whio_udb_record_read( db, ebuf, id, rec, 1 );
                if( rc ) break;
                k2len = rec->keyLen;// ? whio_udb_key_length( db, rec->key ) : 0;
                //UDBG(("klen=%"WHIO_SIZE_T_PFMT", k2len=%"WHIO_SIZE_T_PFMT,klen,k2len));
                if( k2len == klen )
                {
                    if( 0 == db->funcs.keycmp( key, rec->key, klen ) )
                    { /** found match :) */
                        UDB_METRIC_INC(db,searchHits);
                        return rec;
                    }
                }
                id = rec->nextCollision;
                if( id )
                {
                    if( prev ) *prev = *rec;
                    UDB_METRIC_INC(db,collisionTraversals);
                }
                //UDBG(("trying next colliding entry, #%"WHIO_SIZE_T_PFMT,id));
            }
            if( rec && (rec != dest))
            {
                whio_udb_record_free(db, rec);
            }
            UDB_METRIC_INC(db,searchMisses);
            return NULL;
        }
    }
}
whio_udb_record * whio_udb_search( whio_udb * db,
                                   void const * key,
                                   whio_udb_record * dest )
{
    if( ! db || !key ) return NULL;
    else if( ! db->dev ) return NULL;
    else
    {
        whio_udb_record * r = NULL;
        MLOCK_OR_FAIL(db,NULL);
        r = whio_udb_search_impl( db, key, dest, NULL );
        MUNLOCK_RETURN(db,r);
    }
}



int whio_udb_remove( whio_udb * db, void const * key )
{
    if( ! db || !key ) return whio_rc.ArgError;
    else if( ! db->dev ) return whio_rc.InternalError;
    else
    {
        MLOCK_OR_FAIL(db,whio_rc.LockingError);
        // FIXME: lock db here? Later on? Via search()?
        /**
           FIXME: we REALLY should allocate r here, since it's
           using db->rbuf for its key/data parts. But since
           we've locked the mutex, we might as well
           take advantage of the memory.
        */
        whio_udb_record * r =
            db->rbuf
            //whio_udb_record_alloc(db)
            ;
        whio_udb_record p = whio_udb_record_empty;
        whio_udb_record const * s =
            whio_udb_search_impl( db, key, r, &p );
        if( ! s )
        {
            //whio_udb_record_free(db,r);
            MUNLOCK_RETURN(db,whio_rc.NotFoundError);
        }
        else
        {
            int rc = 0;
            if( p.id && (p.id != r->id) )
            { /* remove r from the collision chain */
                assert( p.nextCollision == r->id );
                REC_SMALL_BUF(pbuf);
                p.nextCollision = r->nextCollision;
                if(0)
                {
                    UDBG(("Removing #%"WHIO_SIZE_T_PFMT"."
                          "Linking %"WHIO_SIZE_T_PFMT"<->%"WHIO_SIZE_T_PFMT".",
                          r->id, p.nextCollision, r->nextCollision));
                }
                rc = whio_udb_record_write( db, pbuf, &p, -1 );
            }
            else
            {
                //UDBG(("Setting %"WHIO_SIZE_T_PFMT" as main hashtable entry for slot #%"PRIu32".",r->nextCollision,r->hash));
                rc = whio_udb_hashent_write( db, r->hash,  r->nextCollision );
            }
            r->nextCollision = 0;
            //UDBG(("p.id=%"WHIO_SIZE_T_PFMT", r->id=%"WHIO_SIZE_T_PFMT,p.id,r->id));
            if( 0 == rc )
            {
                UDB_METRIC_INC(db,removals);
                rc = whio_udb_record_to_freelist( db, r );
            }
            //whio_udb_record_free(db,r);
            MUNLOCK_RETURN(db,rc);
        }
    }
}

int whio_udb_insert_with_length( whio_udb * db, void const * key, whio_size_t klen,
                                 void const * val, whio_size_t dlen, bool replaceIfExists )
{
    if( ! db || !key ) return whio_rc.ArgError;
    else if( ! db->dev ) return whio_rc.InternalError;
    else
    {
        MLOCK_OR_FAIL(db,whio_rc.LockingError);
        // FIXME: lock db here
        const whio_udb_hash_t h = whio_udb_hash( db, key );
        if( ! val )
        {
            if( dlen )
            {
                MUNLOCK_RETURN(db,whio_rc.RangeError);
            }
        }
        /*UDBG(("hash=%"PRIu32,h));*/
        if( h >= db->opt.hashSize ) MUNLOCK_RETURN(db, whio_rc.HashingError);
        else if(
                ( klen > whio_udb_key_length(db,NULL) )
                || ( dlen > whio_udb_data_length(db,NULL) )
                )
        {
            MUNLOCK_RETURN(db, whio_rc.RangeError);
        }

        // TODO: LOCK mutex + storage HERE
        unsigned char * ebuf = NULL;
        whio_udb_record prevRec = whio_udb_record_empty;
        whio_udb_record * SR = db->rbuf; // whio_udb_record_alloc(db);
        if( ! SR ) MUNLOCK_RETURN(db, whio_rc.AllocError);
        ebuf = SR->iobuf.buffer;
        bool found = whio_udb_search_impl( db, key, SR, &prevRec ) ? true : false;
        bool replacing = false;

#define CLEANSR (void)0
        //if(SR) whio_udb_record_free(db,SR)
        
        int rc = 0;
        if( ! found )
        { /* append new record... */
            append_record:
            //UDBG(("found [%s]=%d",(char const *)key,found));
            if( found ) /*came in via goto*/
            {
            }
            else
            {
                rc = whio_udb_record_next_free( db, SR, true );
                if( rc ) { CLEANSR; MUNLOCK_RETURN(db, rc); }
                //UDBG(("insert appending new record hash=%"PRIu32" (next-free rc=%d), SR->id=%"WHIO_SIZE_T_PFMT,h,rc,SR->id));
            }
            //UDBG(("prevRec.id==%"WHIO_SIZE_T_PFMT,prevRec.id));
            if( ! prevRec.id )
            {
                rc = whio_udb_hashent_write( db, h, SR->id );
                if( rc ) {CLEANSR; MUNLOCK_RETURN(db, rc);}
                //UDBG(("Wrote hashent #%"PRIu32" w/ id=%"WHIO_SIZE_T_PFMT,h,SR->id));
            }
            SR->hash = h;
            SR->key = (unsigned char const *)key;
            SR->data = (unsigned char const *)val;
            SR->keyLen = klen;
            SR->dataLen = dlen;
            //UDBG(("[%s]=[%s]",(char const *)SR->key,(char const *)SR->data));
            rc = whio_udb_record_write( db, ebuf, SR, 1 );
            //UDBG(("Writing #%"WHIO_SIZE_T_PFMT" [%s]=[%s]",SR->id,(char const *)SR->key,(char const *)SR->data));
            if( !rc && prevRec.id && (prevRec.id != SR->id) && (prevRec.nextCollision != SR->id))
            {
                REC_SMALL_BUF(prevBuf);
                //UDBG(("Updating collision link %"WHIO_SIZE_T_PFMT"->%"WHIO_SIZE_T_PFMT,prevRec.id,SR->id));
                prevRec.nextCollision = SR->id;
                rc = whio_udb_record_write( db, prevBuf, &prevRec, -1 );
            }
            if( replacing ) { UDB_METRIC_INC(db,replacements); }
            else { UDB_METRIC_INC(db,inserts); }
            CLEANSR;
            MUNLOCK_RETURN(db, rc);
        }
        else
        {
            if( ! replaceIfExists )
            {
                CLEANSR;
                //UDBG(("not overwriting [%s] with [%s]",(char const *)SR->key,(char const *)key));
                MUNLOCK_RETURN(db, whio_rc.AccessError);
            }
            replacing = true;
            found = true;
            goto append_record;
        }
#undef CLEANSR
    }
}

int whio_udb_insert( whio_udb * db, void const * key, void const * val, bool replaceIfExists )
{
    return (db && key)
        ? whio_udb_insert_with_length( db, key, whio_udb_key_length(db,key),
                                       val, val ? whio_udb_data_length(db,val) : 0,
                                       replaceIfExists )
        : whio_rc.ArgError;
}


int whio_udb_insert_keyfv(whio_udb * db, void const * val, bool replaceIfExists,
                          char const * fmt, va_list vargs )
{
    if( ! db || ! fmt ) return whio_rc.ArgError;
    else if( ! db->dev ) return whio_rc.InternalError;
    else
    {
        int rc = 0;
        whio_size_t const kLen = db->opt.keyLen;
        WHIO_UDB_VLA_DECL(char,buf, kLen);
        if( ! WHIO_UDB_VLA_BOOL(buf) )
        {
            return whio_rc.AllocError;
        }
        memset( buf, 0, sizeof(buf)/sizeof(buf[0]) );
        int sn = vsnprintf( buf, kLen +1/*let snprintf() write its NULL */,
                            fmt, vargs );
        if( sn < 0 )
        {
            rc = whio_rc.WTFError;
        }
        else if( !sn || ((whio_size_t)sn >= kLen) )
        {
            rc = whio_rc.RangeError;
        }
        else
        {
            rc = whio_udb_insert( db, buf, val, replaceIfExists );
        }
        WHIO_UDB_VLA_FREE(buf);
        return rc;
    }
}
int whio_udb_insert_keyf(whio_udb * db, void const * val, bool replaceIfExists,
                         char const * fmt, ... )
{
    int rc;
    va_list vargs;
    va_start(vargs,fmt);
    rc = whio_udb_insert_keyfv( db, val, replaceIfExists, fmt, vargs );
    va_end(vargs);
    return rc;
}


int whio_udb_insert_datafv(whio_udb * db, void const * key, bool replaceIfExists,
                          char const * fmt, va_list vargs )
{
    if( ! db || ! fmt ) return whio_rc.ArgError;
    else if( ! db->dev ) return whio_rc.InternalError;
    else
    {
        int rc = 0;
        whio_size_t const dLen = db->opt.dataLen;
        WHIO_UDB_VLA_DECL(char, buf, dLen);
        if( ! WHIO_UDB_VLA_BOOL(buf) )
        {
            return whio_rc.AllocError;
        }

        memset( buf, 0, sizeof(buf)/sizeof(buf[0]) );
        int sn = vsnprintf( buf, dLen +1/*let snprintf() write its NULL */,
                            fmt, vargs );
        if( sn < 0 )
        {
            rc = whio_rc.WTFError;
        }
        else if( !sn || ((whio_size_t)sn >= dLen) )
        {
            rc = whio_rc.RangeError;
        }
        else
        {
            rc = whio_udb_insert( db, key, buf, replaceIfExists );
        }
        WHIO_UDB_VLA_FREE(buf);
        return rc;
    }
}
int whio_udb_insert_dataf(whio_udb * db, void const * key, bool replaceIfExists,
                          char const * fmt, ... )
{
    int rc;
    va_list vargs;
    va_start(vargs,fmt);
    rc = whio_udb_insert_datafv( db, key, replaceIfExists, fmt, vargs );
    va_end(vargs);
    return rc;
}



int whio_udb_foreach_entry( whio_udb * db, short whichOnes, whio_udb_foreach_f cb, void * cbData )
{
    if( ! db || !cb ) return whio_rc.ArgError;
    else if( ! db->dev ) return whio_rc.InternalError;
    else
    {
        whio_udb_record * rec = NULL;
        int rc = 0;
        whio_size_t id = 1;
        MLOCK_OR_FAIL(db,whio_rc.LockingError);
        rec = db->rbuf
            //whio_udb_record_alloc( db ) /* locks mutex */
            /* We've locked the db, so we might as well use the
               internal record buffer rather than allocating one.
            */
            ;
        if( ! rec ) MUNLOCK_RETURN(db,whio_rc.InternalError);
        if( 0 == whichOnes )
        { /** Special handling for free-list to allow us to run it in
              order (and faster, since we automatically skip non-free
              blocks).
          */
            id = db->hints.freeList;
            while(id)
            {
                rc = whio_udb_record_read( db, rec->iobuf.buffer, id, rec, 1 );
                if( !rc ) rc = cb( db, rec, cbData );
                if( rc ) break;
                id = rec->nextFree;
            }
        }
        else
        {
            whio_size_t const to = db->hints.highestID;
            for( ; id <= to; ++id )
            {
                rc = whio_udb_record_read( db, rec->iobuf.buffer,
                                           id, rec, 1 );
                if( rc ) break;
                else if( (whichOnes<0) && !rec->keyLen ) continue;
                rc = cb( db, rec, cbData );
                if( rc ) break;
            }
        }
        //whio_udb_record_free( db, rec );
        MUNLOCK_RETURN(db,rc);
    }
}


void const * whio_udb_record_key( whio_udb_record const * r, whio_size_t * length )
{
    if( ! r ) return NULL;
    else
    {
        if( length )
        {
            *length = r->keyLen;
        }
        return r->key;
    }
}

void const * whio_udb_record_data( whio_udb_record const * r, whio_size_t * length )
{
    if( ! r ) return NULL;
    else
    {
        if( length )
        {
            *length = r->dataLen;
        }
        return r->data;
    }
}

/** @internal

    Helper for whio_udb_foreach_matching_globs() and friends.
*/
typedef struct
{
    /** For-each callback. */
    whio_udb_foreach_f cb;
    /** Client data for the callback. */
    void * cbData;
    /**
       glob/SQL patterns
    */
    char const * const * patterns;
    /** # of patterns. */
    unsigned int count;
    /**
       0=glob, <0=case-insensitive LIKE, >0=case-sensitive LIKE
    */
    short mode; 
}  whio_udb_foreach_glob_info;

/**
   Implementes whio_udb_foreach_f(). cbData MUST be-a (whio_udb_foreach_glob_info*).
*/
static int whio_udb_foreach_glob( whio_udb const * db, whio_udb_record const * r, void * cbData )
{
    whio_udb_foreach_glob_info * gi = (whio_udb_foreach_glob_info*) cbData;
    char const * p = NULL;
    int i = 0;
    int rc = 0;
    int check = 0;
    for( ; !rc && (i<gi->count); ++i )
    {
        p = gi->patterns[i];
        if( ! p ) break;
        else
        {
            check = 0;
            if( gi->mode == 0 /* globs */ )
            {
                check = whglob_matches( p, (char const *)r->key );
            }
            else if( gi->mode < 0 /* case-insensitive LIKE */ )
            {
                check = whglob_matches_like( p, (char const *)r->key, 0 );
            }
            else /* case-sensitive LIKE */
            {
                check = whglob_matches_like( p, (char const *)r->key, 1 );
            }
            if( check )
            {
                rc = gi->cb( db, r, gi->cbData );
                break;
            }
        }
    }
    return rc;
}


int whio_udb_foreach_matching_globs( whio_udb * db, char const * const * globs, unsigned int n, whio_udb_foreach_f callback, void * callbackData )
{
    if( ! db || !n || !globs || ! *globs || ! callback )
    {
        return whio_rc.ArgError;
    }
    else
    {
        whio_udb_foreach_glob_info gi;
        gi.cb = callback;
        gi.cbData = callbackData;
        gi.patterns = globs;
        gi.count = n;
        gi.mode = 0;
        return whio_udb_foreach_entry( db, -1, whio_udb_foreach_glob, &gi );
    }
}

int whio_udb_foreach_matching_glob( whio_udb * db, char const * glob, whio_udb_foreach_f callback, void * callbackData )
{
    return (glob && *glob)
        ? whio_udb_foreach_matching_globs( db, &glob, 1, callback, callbackData )
        : whio_rc.ArgError;
}
int whio_udb_foreach_matching_likes( whio_udb * db, char const * const * globs, unsigned int n, bool caseSensitive, whio_udb_foreach_f callback, void * callbackData )
{
    if( ! db || !n || !globs || ! *globs || ! callback )
    {
        return whio_rc.ArgError;
    }
    else
    {
        whio_udb_foreach_glob_info gi;
        gi.cb = callback;
        gi.cbData = callbackData;
        gi.patterns = globs;
        gi.count = n;
        gi.mode = caseSensitive ? 1 : -1;;
        return whio_udb_foreach_entry( db, -1, whio_udb_foreach_glob, &gi );
    }

}
int whio_udb_foreach_matching_like( whio_udb * db, char const * like, bool caseSens, whio_udb_foreach_f callback, void * callbackData )
{
    return (like && *like)
        ? whio_udb_foreach_matching_likes( db, &like, 1, caseSens, callback, callbackData )
        : whio_rc.ArgError;
        
}

int whio_udb_metrics_get( whio_udb const * db, whio_udb_metrics * dest )
{
    return (!db || !dest)
        ? whio_rc.ArgError
        : ((*dest=db->metrics),0)
        ;
}

whio_udb_metrics const * whio_udb_metrics_get_ptr( whio_udb const * db )
{
    return db ? &db->metrics : NULL;
}

int whio_udb_metrics_reset( whio_udb * db )
{
    if( ! db ) return whio_rc.ArgError;
    else if( ! db->dev ) return whio_rc.InternalError;
    else
    {
        MLOCK_OR_FAIL(db,whio_rc.LockingError);
        db->metrics = whio_udb_metrics_empty;
        MUNLOCK_RETURN(db,0);
    }
}



/** Internal type for mapping whio_udb_funcs to a name. */
struct whio_udb_func_table
{
    char name[whio_udb_funcs_name_length];
    whio_udb_funcs funcs;
};
/** Convenience typedef. */
typedef struct whio_udb_func_table whio_udb_func_table;
/** Empty-initialized whio_udb_func_table object. */
#define whio_udb_func_table_empty_m {{0},whio_udb_funcs_empty_m}
static whio_udb_func_table udb_func_table[] =
    {
    /* The API docs claim that at least 10 slots shall be guaranteed
       free for client use.
    */
    whio_udb_func_table_empty_m,
    whio_udb_func_table_empty_m,

    whio_udb_func_table_empty_m,
    whio_udb_func_table_empty_m,

    whio_udb_func_table_empty_m,
    whio_udb_func_table_empty_m,

    whio_udb_func_table_empty_m,
    whio_udb_func_table_empty_m,

    whio_udb_func_table_empty_m,
    whio_udb_func_table_empty_m
    };

whio_udb_funcs const * whio_udb_funcs_search( char const * n )
{
    if( ! n || !*n ) return NULL;
    else
    {
        enum { to = sizeof(udb_func_table)/sizeof(udb_func_table[0]) };
        whio_udb_func_table const * f = udb_func_table;
        unsigned int i = 0;
        for( ; i < to; ++i, ++f )
        {
            if( ! f->name ) continue;
            else if( 0 == strcmp(n,f->name) ) return &f->funcs;
        }
        return NULL;
    }
}

whio_udb_funcs const * whio_udb_funcs_register( char const * n,
                                                whio_udb_funcs const * f )
{
    if( !n || !*n || !f
        || (NULL==f->keycmp)
        || (NULL==f->hash)
        ) return NULL;
    else
    {
        enum { to = sizeof(udb_func_table)/sizeof(udb_func_table[0]) };
        whio_udb_funcs const * x = whio_udb_funcs_search( n );
        if( ! x )
        {
            whio_udb_func_table * t = udb_func_table;
            unsigned int i = 0;
            const size_t slen = strlen(n);
            if( slen > (size_t)whio_udb_funcs_name_length )
            {
                return NULL;
            }
            for( ; i < to; ++i, ++t )
            {
                if( *t->name ) continue;
                memcpy( t->name, n, slen );
                t->funcs = *f;
                return &t->funcs;
            }
        }
        return NULL;
    }
}

/** Temporary internal macro to define whio_udb_hash_ptr_MyKeyType().
*/
#define HASH_FUNC(MyKeyType)                              \
whio_udb_hash_t whio_udb_hash_ptr_##MyKeyType( void const * obj, whio_size_t len ) \
{                                                                \
    MyKeyType const * x = (MyKeyType const *)obj; \
    return (whio_udb_hash_t) (x ? *x : 0); \
}

/** Temporary internal macro to define whio_udb_keylen_ptr_MyKeyType()
    and whio_udb_keycmp_ptr_MyKeyType().
*/
#define KEY_FUNCS(MyKeyType)                                   \
/*static*/ whio_size_t whio_udb_keylen_ptr_##MyKeyType( void const * ignored ) \
{ return sizeof(MyKeyType); }                                           \
int whio_udb_keycmp_ptr_##MyKeyType( void const * lhs, void const * rhs, size_t len ) \
{                                                                   \
    MyKeyType const * l = (MyKeyType const *)lhs; \
    MyKeyType const * r = (MyKeyType const *)rhs; \
    if( ! l ) return r ? -1 : 0; \
    else if( !r ) return 1; \
    else if( *l < *r ) return -1; \
    else if( *l > *r ) return 1; \
    else return 0; \
}

/** Temporary internal macro. */
#define FUNCS(KeyType) \
    HASH_FUNC(KeyType) \
         KEY_FUNCS(KeyType)
/** Temporary internal macro. */
#define FUNCS_N(N) FUNCS(int##N##_t) FUNCS(uint##N##_t)
#if 1 /* this is here to help emacs get back in proper alignment at
         the end of this block :/. Kludge for the missing trailing
         semicolons.
      */
FUNCS_N(8)
FUNCS_N(16)
FUNCS_N(32)
FUNCS_N(64)
FUNCS(whio_size_t)
#endif
#undef FUNCS_N
#undef FUNCS
#undef HASH_FUNC
#undef KEY_FUNCS
#undef KEYLEN_FUNC
    
int whio_udb_funcs_parse( char const * key, whio_udb_funcs * tgt )
{
    enum { BLen = whio_udb_funcs_name_length+1 };
    if( ! key || !*key || ! tgt ) return whio_rc.ArgError;
    else
    {
        whio_udb_funcs const * f = NULL;
        const size_t slen = strlen(key);
        if( slen > whio_udb_funcs_name_length ) return whio_rc.RangeError;
        else if( (f=whio_udb_funcs_search( key )) )
        {
            *tgt = *f;
            return 0;
        }
        else
        {
            *tgt = whio_udb_funcs_empty;
             /**
               parse key in the form KEY_TYPE=DATA_TYPE and poplate
               tgt with the various whio_udb_keycmp_ptr_XXX() and
               whio_udb_hash_ptr_XXX() functions for that combination.
            */

            char buf[BLen];
            char const * p1 = buf; // pre-=
            char const * p2 = NULL; // post-=
            char * at = buf;
            char i =0 ;
            memcpy( buf, key, slen );
            buf[slen] = 0;
            for( ; *at && (*at != '='); ++at ) {}
            if( '=' == *at )
            {
                *at = 0;
                p2 = ++at;
            }
            else if( ! *at ) return whio_rc.ArgError;
            //UDBG(("p1=[%s], p2=[%s]", p1, p2 ));
            for( ; i < 2; ++i )
            {
                if( 1 == i ) p1 = p2;
                if( 0==strncmp(p1,"string",6) )
                {
                    if(0==i)
                    {
                        if( NULL != strstr( p1, ":nocase" ) )
                        {
                            tgt->keycmp = whio_udb_key_cmp_str_nocase;
                            tgt->hash = whio_udb_hash_str_nocase;
                        }
                        else
                        {
                            tgt->keycmp = whio_udb_key_cmp_str;
                            tgt->hash = whio_udb_hash_str;
                        }
                        tgt->keylen = whio_udb_length_str;
                    }
                    else
                    {
                        tgt->datalen = whio_udb_length_str;
                        return 0;
                    }                        
                }
#define TRY(T) \
                else if( 0 == strcmp(p1,#T"*") ) {     \
                    if(0==i) { \
                        tgt->keycmp = whio_udb_keycmp_ptr_##T;  \
                        tgt->keylen = whio_udb_keylen_ptr_##T;  \
                        tgt->hash = whio_udb_hash_ptr_##T;      \
                    } else { \
                        tgt->datalen = whio_udb_keylen_ptr_##T; \
                        return 0; \
                    } \
                }
#if 1 /* emacs indention-fixer kludge */
                TRY(int8_t) TRY(int16_t) TRY(int32_t) TRY(int64_t)
                TRY(uint8_t) TRY(uint16_t) TRY(uint32_t) TRY(uint64_t)
                TRY(whio_size_t)
#endif
                else
                {
                    return whio_rc.NotFoundError;
                }
#undef TRY
            }
        }
    }
    return whio_rc.NYIError;
}

#undef UDBG
#undef WHIO_UDB_VLA_DECL
#undef WHIO_UDB_VLA_FREE
#undef WHIO_UDB_VLA_BOOL
#undef UDB_METRIC_INC
#undef UDB_METRIC_DEC
#undef REC_SMALL_BUF
#undef MLOCK
#undef MLOCK_OR_FAIL
#undef MUNLOCK
#undef MUNLOCK_RETURN

/* end file udb/whio_udb.c */
