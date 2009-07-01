/* auto-generated on Wed Jul  1 16:21:03 CEST 2009. Do not edit! */
#line 1 "whefs_amalgamation.c"
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200112L /* needed for ftello() and friends */
#endif
#if !defined(_ISOC99_SOURCE)
#define _ISOC99_SOURCE 1 /* needed for snprintf() */
#endif
#include "whefs_amalgamation.h"
/* begin file src/whio.c */
#line 8 "src/whio.c"
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/
#ifdef NDEBUG
#  undef NDEBUG
#endif


#include <stdlib.h>
#include <assert.h>
#include <memory.h>


const whio_rc_t whio_rc =
    {
    0, /* OK */
    /* -1 is reserved for SizeTError */
    -2, /* ArgError */
    -3, /* IOError */
    -4, /* AllocError */
    -5, /* InternalError */
    -6, /* RangeError */
    // -7 can be reused
    -8, /* AccessError */
    -9, /* ConsistencyError */
    -10, /* NYIError */
    -11, /* UnsupportedError */
    -12, /* TypeError */
    (whio_size_t)-1 /* SizeTError */
    };


void whio_noop_printf(char const * fmt, ...)
{
}
/* end file src/whio.c */
/* begin file src/whio_common.c */
#line 8 "src/whio_common.c"
#include <string.h> /* strchr() */

const whio_client_data whio_client_data_init = whio_client_data_init_m;
const whio_impl_data whio_impl_data_init = whio_impl_data_init_m;

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

#define WHIO_DEV_EMPTY_INIT {0/*api*/, whio_impl_data_init_m, whio_client_data_init_m }

static const whio_dev whio_dev_empty_init = WHIO_DEV_EMPTY_INIT;

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
} whio_dev_alloc_slots = { {WHIO_DEV_EMPTY_INIT}, {0}, 0 };
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
    if( ! dev ) dev = (whio_dev *) malloc( sizeof(whio_dev) );
    if( dev ) *dev = whio_dev_empty_init;
    return dev;
}

void whio_dev_free( whio_dev * dev )
{
    if( dev ) *dev = whio_dev_empty_init;
    else return;	
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (dev < &whio_dev_alloc_slots.devs[0]) ||
	(dev > &whio_dev_alloc_slots.devs[whio_dev_alloc_count-1]) )
    { /* doesn't belong to us. */
	free(dev);
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
    free(dev);
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

whio_size_t whio_dev_seek( whio_dev * dev, off_t pos, int whence )
{
    return dev ? dev->api->seek( dev, pos, whence ) : whio_rc.SizeTError;
}

int whio_dev_flush( whio_dev * dev )
{
    return dev ? dev->api->flush( dev ) : whio_rc.ArgError;
}

int whio_dev_truncate( whio_dev * dev, off_t size )
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

whio_fetch_result * whio_dev_fetch( whio_dev * dev, whio_size_t n )
{
    if( ! dev ) return 0;
    whio_fetch_result * rc = (whio_fetch_result*)malloc(sizeof(whio_fetch_result));
    if( ! rc ) return 0;
    rc->alloced = 0;
    rc->requested = n;
    rc->read = 0;
    if( ! n ) return rc;
    const whio_size_t sza = n+1; /* the +1 is necessary so we can ensure nulls for script-side strings. */
    rc->data = (char *)malloc(sza);
    if( ! rc->data )
    {
	free(rc);
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
	free(r->data);
	free(r);
	return whio_rc.OK;
    }
    return whio_rc.ArgError;
}
int whio_dev_fetch_free_data( whio_fetch_result * r )
{
    if( r )
    {
	free(r->data);
	r->alloced = 0;
	r->data = 0;
	return whio_rc.OK;
    }
    return whio_rc.ArgError;
}


const whio_blockdev whio_blockdev_init = whio_blockdev_init_m;
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
} whio_blockdev_alloc_slots = { { whio_blockdev_init_m }, {0} };
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
    if( ! obj ) obj = (whio_blockdev *) malloc( sizeof(whio_blockdev) );
    if( obj ) *obj = whio_blockdev_init;
    return obj;
}

void whio_blockdev_free( whio_blockdev * obj )
{
    whio_blockdev_cleanup( obj );
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_blockdev_alloc_slots.objs[0]) ||
	(obj > &whio_blockdev_alloc_slots.objs[whio_blockdev_alloc_count-1]) )
    { /* it does not belong to us */
	free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whio_blockdev_alloc_slots.objs[0]);
	whio_blockdev_alloc_slots.objs[ndx] = whio_blockdev_init;
	whio_blockdev_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    if( obj ) *obj = whio_blockdev_init;
    free(obj);
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
}

bool whio_blockdev_cleanup( whio_blockdev * self )
{
    if( ! self ) return false;
    if( self->impl.fence )
    {
	self->impl.fence->api->finalize( self->impl.fence );
	self->impl.fence = 0;
    }
    *self = whio_blockdev_init;
    return true;
}


int whio_blockdev_setup( whio_blockdev * self, whio_dev * parent, whio_size_t parent_offset,
			   whio_size_t block_size, whio_size_t count, void const * prototype )
{
    if( ! self || ! parent || ! count || ! block_size ) return whio_rc.ArgError;
    *self = whio_blockdev_init;
    self->impl.fence = whio_dev_subdev_create( parent, parent_offset, parent_offset + (count * block_size) );
    if( ! self->impl.fence ) return whio_rc.AllocError;
    self->blocks.prototype = prototype;
    self->blocks.size = block_size;
    self->blocks.count = count;
    return whio_rc.OK;
}

int whio_blockdev_wipe( whio_blockdev * self, whio_size_t id )
{
    return whio_blockdev_write( self, id, self->blocks.prototype );
}

bool whio_blockdev_in_range( whio_blockdev const * self, whio_size_t id )
{
    return !self
	? false
	: (id < self->blocks.count);
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
#include <stdlib.h>
#include <stdio.h>
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
   -1 /*iomode*/ \
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
    if( ! obj ) obj = (whio_dev_FILE *) malloc( sizeof(whio_dev_FILE) );
    if( obj ) *obj = whio_dev_FILE_meta_init;
    return obj;
}

static void whio_dev_FILE_free( whio_dev_FILE * obj )
{
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_dev_FILE_alloc_slots.objs[0]) ||
	(obj > &whio_dev_FILE_alloc_slots.objs[whio_dev_FILE_alloc_count-1]) )
    { /* it does not belong to us */
	free(obj);
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
    free(obj);
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
    return (dev && src && n)
	? fwrite( src, sizeof(char), n, f->fp )
	: 0;
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

static whio_size_t whio_dev_FILE_seek( whio_dev * dev, off_t pos, int whence )
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
    f->needsFlush = 0;
    return (0 == fflush( f->fp ))
	? whio_rc.OK
	: whio_rc.IOError;
}

static int whio_dev_FILE_trunc( whio_dev * dev, off_t len )
{
    WHIO_FILE_DECL(whio_rc.ArgError);
    return ftruncate( f->fileno, len );
    /** ^^^ is there a way to truncate a FILE handle without using fileno()? */
}

static int whio_dev_FILE_ioctl( whio_dev * dev, int arg, va_list vargs )
{
    WHIO_FILE_DECL(whio_rc.ArgError);
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
    if( dev )
    {
	dev->api->flush(dev);
	if( dev->client.dtor ) dev->client.dtor( dev->client.data );
	dev->client = whio_client_data_init;
	whio_dev_FILE * f = (whio_dev_FILE*)dev->impl.data;
	if( f )
	{
	    dev->impl.data = 0;
	    if( f->fp && f->ownsFile ) fclose( f->fp );
	    f->fileno = 0;
	    //free( f );
	    whio_dev_FILE_free( f );
	    return true;
	}
    }
    return false;
}

static void whio_dev_FILE_finalize( whio_dev * dev )
{
    if( dev )
    {
	dev->api->close( dev );
	whio_dev_free(dev);
    }
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
    whio_dev * dev = whio_dev_alloc();
    if( ! dev ) return 0;
    whio_dev_FILE * meta = whio_dev_FILE_alloc();
    if( ! meta )
    {
	whio_dev_free(dev);
	return 0;
    }
    *dev = whio_dev_FILE_init;
    *meta = whio_dev_FILE_meta_init;
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> /* ftruncate(), fdatasync() */
#include <fcntl.h>
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



/**
   Internal implementation details for the whio_dev FILE wrapper.
*/
typedef struct whio_dev_fileno
{
    /**
       Underlying FILE handle. Owned by this
       object.
    */
    FILE * fp;
    int fileno;
    char const * filename;
    bool atEOF;
    int errstate;
    short iomode;
} whio_dev_fileno;


/**
   Initialization object for whio_dev_fileno objects. Also used as
   whio_dev::typeID for such objects.
*/
#define WHIO_DEV_fileno_INIT { \
    0, /* fp */ \
    0, /* fileno */ \
    0, /* filename */ \
    false, /* atEOF */ \
    0, /* errstate */                       \
   -1 /*iomode*/ \
    }
static const whio_dev_fileno whio_dev_fileno_meta_init = WHIO_DEV_fileno_INIT;

#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whio_dev_fileno_alloc_slots object.
*/
whio_dev_fileno_alloc_count = 5
};
struct
{
    whio_dev_fileno objs[whio_dev_fileno_alloc_count];
    char used[whio_dev_fileno_alloc_count];
} whio_dev_fileno_alloc_slots = { {WHIO_DEV_fileno_INIT}, {0} };
#endif

static whio_dev_fileno * whio_dev_fileno_alloc()
{
    whio_dev_fileno * obj = 0;
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whio_dev_fileno_alloc_count; ++i )
    {
	if( whio_dev_fileno_alloc_slots.used[i] ) continue;
	whio_dev_fileno_alloc_slots.used[i] = 1;
	whio_dev_fileno_alloc_slots.objs[i] = whio_dev_fileno_meta_init;
	obj = &whio_dev_fileno_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_fileno *) malloc( sizeof(whio_dev_fileno) );
    return obj;
}

static void whio_dev_fileno_free( whio_dev_fileno * obj )
{
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_dev_fileno_alloc_slots.objs[0]) ||
	(obj > &whio_dev_fileno_alloc_slots.objs[whio_dev_fileno_alloc_count-1]) )
    { /* it does not belong to us */
	free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whio_dev_fileno_alloc_slots.objs[0]);
	whio_dev_fileno_alloc_slots.objs[ndx] = whio_dev_fileno_meta_init;
	whio_dev_fileno_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    free(obj);
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
}


/**
   A helper for the whio_dev_fileno API. Requires that the 'dev'
   parameter be-a whio_dev and that that device is-a whio_dev_fileno.
 */
#define WHIO_fileno_DECL(RV) whio_dev_fileno * f = (dev ? (whio_dev_fileno*)dev->impl.data : 0); \
    if( !f || !f->fp || ((void const *)&whio_dev_fileno_meta_init != dev->impl.typeID) ) return RV

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
    return (whio_size_t) lseek( f->fileno, 0L, SEEK_CUR );
}

static whio_size_t whio_dev_fileno_seek( whio_dev * dev, off_t pos, int whence )
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
    return (whio_size_t) rc;
}

static int whio_dev_fileno_flush( whio_dev * dev )
{
    WHIO_fileno_DECL(whio_rc.ArgError);
    return fsync( f->fileno );
    /* reminder: i realy want fdatasync(), but some platforms
    *cough* Solaris *cough* don't appear to have it. */
}

static int whio_dev_fileno_trunc( whio_dev * dev, off_t len )
{
    WHIO_fileno_DECL(whio_rc.ArgError);
    int rc = ftruncate( f->fileno, len );
    if( 0 == rc )
    {
	whio_dev_fileno_flush( dev );
    }
    return rc;
}

short whio_dev_fileno_iomode( whio_dev * dev )
{
    WHIO_fileno_DECL(-1);
    return f->iomode;
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
      case whio_dev_ioctl_FCNTL_lock_nowait:
      case whio_dev_ioctl_FCNTL_lock_wait:
      case whio_dev_ioctl_FCNTL_lock_get:
	  do
	  {
	      struct flock * fl = (va_arg(vargs,struct flock *));
	      if( fl )
	      {
		  int lockCmd = (whio_dev_ioctl_FCNTL_lock_nowait == arg)
		      ? F_SETLK
		      : ((whio_dev_ioctl_FCNTL_lock_get == arg) ? F_GETLK :  F_SETLKW);
		  rc = fcntl( f->fileno, lockCmd, fl );
	      }
	      else
	      {
		  rc = whio_rc.ArgError;
	      }
	  } while(0);
	  break;
      default: break;
    };
    return rc;
}

static bool whio_dev_fileno_close( whio_dev * dev )
{
    if( dev )
    {
	dev->api->flush(dev);
	if( dev->client.dtor ) dev->client.dtor( dev->client.data );
	dev->client = whio_client_data_init;
	whio_dev_fileno * f = (whio_dev_fileno*)dev->impl.data;
	if( f )
	{
	    dev->impl.data = 0;
	    if( f->fp ) fclose( f->fp );
	    *f = whio_dev_fileno_meta_init;
	    whio_dev_fileno_free( f );
	    return true;
	}
    }
    return false;
}

static void whio_dev_fileno_finalize( whio_dev * dev )
{
    if( dev )
    {
	dev->api->close( dev );
	whio_dev_free(dev);
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

static const whio_dev whio_dev_fileno_init =
    {
    &whio_dev_fileno_api,
    { /* impl */
    0, /* data. Must be-a (whio_dev_fileno*) */
    (void const *)&whio_dev_fileno_meta_init /* typeID */
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
        return 0;
    }
    /** Maintenance reminder:

        i would like to move these two allocs to below the fopen(),
        but if we open the file first then we have to check whether we
        created the file, and delete it if we did not.
    */
    whio_dev * dev = whio_dev_alloc();
    if( ! dev )
    {
        return 0;
    }
    whio_dev_fileno * meta = whio_dev_fileno_alloc();
    if( ! meta )
    {
	whio_dev_free(dev);
	return 0;
    }
    FILE * f = (fname && *fname) ? fopen(fname,mode) : fdopen( filenum, mode );
    if( ! f )
    {
        whio_dev_free(dev);
        whio_dev_fileno_free(meta);
	return 0;
    }
    *dev = whio_dev_fileno_init;
    *meta = whio_dev_fileno_meta_init;
    dev->impl.data = meta;
    meta->fp = f;
    meta->fileno = fileno(f);
    meta->filename = fname;
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
static const whio_dev_membuf_meta whio_dev_membuf_meta_init = WHIO_DEV_MEMBUF_META_INIT;

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
	whio_dev_membuf_meta_alloc_slots.objs[i] = whio_dev_membuf_meta_init;
	obj = &whio_dev_membuf_meta_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_membuf_meta *) malloc( sizeof(whio_dev_membuf_meta) );
    return obj;
}

static void whio_dev_membuf_meta_free( whio_dev_membuf_meta * obj )
{
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_dev_membuf_meta_alloc_slots.objs[0]) ||
	(obj > &whio_dev_membuf_meta_alloc_slots.objs[whio_dev_membuf_meta_alloc_count-1]) )
    { /* it does not belong to us */
	free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whio_dev_membuf_meta_alloc_slots.objs[0]);
	whio_dev_membuf_meta_alloc_slots.objs[ndx] = whio_dev_membuf_meta_init;
	whio_dev_membuf_meta_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    free(obj);
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

static whio_size_t whio_dev_membuf_seek( whio_dev * dev, off_t pos, int whence )
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

static int whio_dev_membuf_trunc( whio_dev * dev, off_t _len )
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

static bool whio_dev_membuf_close( whio_dev * dev )
{
    if( dev )
    {
	if( dev->client.dtor ) dev->client.dtor( dev->client.data );
	dev->client = whio_client_data_init;
	whio_dev_membuf_meta * f = (whio_dev_membuf_meta*)dev->impl.data;
	if( f )
	{
	    dev->impl.data = 0;
	    free(f->buffer);
	    //*f = whio_dev_membuf_meta_init;
	    //free(f);
	    whio_dev_membuf_meta_free( f );
	    return true;
	}
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

static const whio_dev whio_dev_membuf_init = WHIO_DEV_MEMBUF_INIT;

whio_dev * whio_dev_for_membuf( whio_size_t size, float expFactor )
{
    whio_dev * dev = whio_dev_alloc();
    if( ! dev ) return 0;
    *dev = whio_dev_membuf_init;
    whio_dev_membuf_meta * mb = whio_dev_membuf_meta_alloc();
    if( !mb )
    {
	whio_dev_free(dev);
	return 0;
    }
    *mb = whio_dev_membuf_meta_init;
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
} whio_dev_memmap;

/**
   Initialization object for new whio_dev_memmap objects.
*/
#define WHIO_DEV_MEMMAP_INIT { \
    0, /* size */ \
    0, /* maxsize */ \
    0, /* pos */ \
    0, /* rw */ \
    0 /* ro */ \
    }

static const whio_dev_memmap whio_dev_memmap_init = WHIO_DEV_MEMMAP_INIT;


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
	whio_dev_memmap_alloc_slots.objs[i] = whio_dev_memmap_init;
	obj = &whio_dev_memmap_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_memmap *) malloc( sizeof(whio_dev_memmap) );
    return obj;
}

static void whio_dev_memmap_free( whio_dev_memmap * obj )
{
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_dev_memmap_alloc_slots.objs[0]) ||
	(obj > &whio_dev_memmap_alloc_slots.objs[whio_dev_memmap_alloc_count-1]) )
    { /* it does not belong to us */
	free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whio_dev_memmap_alloc_slots.objs[0]);
	whio_dev_memmap_alloc_slots.objs[ndx] = whio_dev_memmap_init;
	whio_dev_memmap_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    free(obj);
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

static whio_size_t whio_dev_memmap_seek( whio_dev * dev, off_t pos, int whence )
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

static int whio_dev_memmap_trunc( whio_dev * dev, off_t _len )
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
	dev->client = whio_client_data_init;
	whio_dev_memmap * f = (whio_dev_memmap*)dev->impl.data;
	if( f )
	{
	    dev->impl.data = 0;
	    *f = whio_dev_memmap_init;
	    whio_dev_memmap_free(f);
	    return true;
	}
    }
    return false;
}

static void whio_dev_memmap_finalize( whio_dev * dev )
{
    if( dev )
    {
	dev->api->close( dev );
	whio_dev_free(dev);
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

static const whio_dev whio_dev_memmap_dev_init =
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
    if( (!rw && !ro) || ! size ) return 0;
    if( rw && ro )
    {
	if( ro != rw ) return 0;
    }
    whio_dev * dev = whio_dev_alloc();
    if( ! dev ) return 0;
    *dev = whio_dev_memmap_dev_init;
    whio_dev_memmap * mb = whio_dev_memmap_alloc();
    if( !mb )
    {
	whio_dev_free(dev);
	return 0;
    }
    *mb = whio_dev_memmap_init;
    dev->impl.data = mb;
    mb->size = mb->maxsize = size;
    mb->rw = rw;
    mb->ro = ro ? ro : rw;
    //WHIO_DEBUG( "memmap @%p, buffer @%p: size=%u\n", (void const *)dev, (void const *)mb->ro, mb->size );
    return dev;
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
    */
    whio_size_t upper;
    /**
       Current cursor pos, relative to parent BOF.
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
static const whio_dev_subdev_meta whio_dev_subdev_meta_init = WHIO_DEV_SUBDEV_META_INIT;

#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whio_dev_subdev_meta_alloc_slots object.
*/
whio_dev_subdev_meta_alloc_count = 4
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
	whio_dev_subdev_meta_alloc_slots.objs[i] = whio_dev_subdev_meta_init;
	obj = &whio_dev_subdev_meta_alloc_slots.objs[i];
	//WHIO_DEBUG("Allocated device #%u @0x%p\n", i, (void const *)obj );
	break;
    }
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_subdev_meta *) malloc( sizeof(whio_dev_subdev_meta) );
    return obj;
}

void whio_dev_subdev_meta_free( whio_dev_subdev_meta * obj )
{
#if WHIO_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_dev_subdev_meta_alloc_slots.objs[0]) ||
	(obj > &whio_dev_subdev_meta_alloc_slots.objs[whio_dev_subdev_meta_alloc_count-1]) )
    { /* it does not belong to us */
	free(obj);
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

	whio_dev_subdev_meta_alloc_slots.objs[ndx] = whio_dev_subdev_meta_init;
	whio_dev_subdev_meta_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    free(obj);
#endif /* WHIO_CONFIG_ENABLE_STATIC_MALLOC */
}


/**
   A helper for the whio_dev_subdev API. Requires that the 'dev'
   parameter be-a whio_dev and that that device is-a whio_dev_subdev.
 */
#define WHIO_subdev_DECL(RV) whio_dev_subdev_meta * sub = (dev ? (whio_dev_subdev_meta*)dev->impl.data : 0); \
    if( !sub || ((void const *)&whio_dev_subdev_meta_init != dev->impl.typeID) || (!sub->dev)) return RV


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

static whio_size_t whio_dev_subdev_seek( whio_dev * dev, off_t pos, int whence )
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

static int whio_dev_subdev_trunc( whio_dev * dev, off_t len )
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
	  return sub->dev->api->ioctl( sub->dev, arg, vargs );
	  break;
    };
    return rc;
}

static bool whio_dev_subdev_close( whio_dev * dev )
{
    WHIO_subdev_DECL(false);
    dev->api->flush(dev);
    *sub = whio_dev_subdev_meta_init;
    whio_dev_subdev_meta_free( sub );
    dev->impl.data = 0;
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

static const whio_dev whio_dev_subdev_init =
    {
    &whio_dev_api_subdev,
    { /* impl */
    0, /* implData. Must be-a (whio_dev_subdev_meta*) */
    (void const *)&whio_dev_subdev_meta_init /* typeID */
    }
    };

whio_dev * whio_dev_subdev_create( whio_dev * parent, whio_size_t lowerBound, whio_size_t upperBound )
{
    if( ! parent || (upperBound && (upperBound <= lowerBound)) ) return 0;
    whio_dev * dev = whio_dev_alloc();
    if( ! dev ) return 0;
    whio_dev_subdev_meta * meta = whio_dev_subdev_meta_alloc();
    if( ! meta )
    {
	whio_dev_free(dev);
	return 0;
    }
    *dev = whio_dev_subdev_init;
    *meta = whio_dev_subdev_meta_init;
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
    if( (void const *)&whio_dev_subdev_meta_init != dev->impl.typeID ) return whio_rc.TypeError;
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
    self->client = whio_client_data_init;
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

const whio_stream_api whio_stream_api_init = 
    {
    whio_stream_default_read,
    whio_stream_default_write,
    whio_stream_default_close,
    whio_stream_default_finalize,
    whio_stream_default_flush,
    whio_stream_default_isgood
    };

const whio_stream whio_stream_init = 
    {
    &whio_stream_api_init,
    whio_impl_data_init_m,
    whio_client_data_init_m
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

static const whio_stream_dev_meta whio_stream_dev_meta_init =
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
    self->client = whio_client_data_init;
    if( meta->dev && meta->ownsDev )
    {
        meta->dev->api->finalize( meta->dev );
    }
    free( meta );
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
	free( self );
    }
}

whio_stream * whio_stream_for_dev( whio_dev * dev, bool takeOwnership )
{
    if( ! dev ) return 0;
    whio_stream * str = (whio_stream *) malloc( sizeof(whio_stream) );
    if( ! str ) return 0;
    whio_stream_dev_meta * meta = (whio_stream_dev_meta *) malloc( sizeof(whio_stream_dev_meta) );
    if( ! meta )
    {
	free(str);
	return 0;
    }
    *str = whio_stream_dev;
    *meta = whio_stream_dev_meta_init;
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
} whio_stream_FILEINFO;
static const whio_stream_FILEINFO whio_stream_FILEINFO_init =
    {
    0, /* fp */
    0, /* fileno */
    false, /* ownsFile */
    -1 /* iomode */
    };

whio_stream * whio_stream_for_FILE( FILE * fp, bool takeOwnership )
{
    if( ! fp ) return 0;
    whio_stream * st = (whio_stream *) malloc( sizeof(whio_stream) );
    if( ! st ) return 0;
    whio_stream_FILEINFO * meta = (whio_stream_FILEINFO*) malloc( sizeof(whio_stream_FILEINFO) );
    if( ! meta )
    {
	free( st );
	return 0;
    }
    *st = whio_stream_FILE_init;
    st->impl.data = meta;
    *meta = whio_stream_FILEINFO_init;
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

#undef WHIO_STR_FILE_DECL
static bool whio_stream_FILE_close( whio_stream * self )
{
    whio_stream_FILEINFO * meta = (self && self->impl.data)
	? (whio_stream_FILEINFO*)self->impl.data
	: 0;
    if( ! meta ) return false;
    self->api->flush( self );
    if( self->client.dtor ) self->client.dtor( self->client.data );
    self->client = whio_client_data_init;
    self->impl.data = 0;
    if( meta->fp && meta->ownsFile )
    {
        fclose( meta->fp );
    }
    *meta = whio_stream_FILEINFO_init;
    free(meta);
    return true;
}

/**
   whio_stream_api.destroy implementation for whio_stream_FILE.
*/
static void whio_stream_FILE_finalize( whio_stream * self )
{
    if( self )
    {
	self->api->close( self );
	*self = whio_stream_FILE_init;
	free(self);
    }
}

/* end file src/whio_stream_FILE.c */
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
size_t whio_uint64_encode( unsigned char * dest, uint64_t i )
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


int whio_uint64_decode( unsigned char const * src, uint64_t * tgt )
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


static const unsigned char whio_uint32_tag_char = 0x80 | 32;
size_t whio_uint32_encode( unsigned char * dest, uint32_t i )
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

int whio_uint32_decode( unsigned char const * src, uint32_t * tgt )
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

static const unsigned char whio_uint16_tag_char = 0x80 | 16;
size_t whio_uint16_encode( unsigned char * dest, uint16_t i )
{
    if( ! dest ) return 0;
    static const uint16_t mask = 0x00ff;
    uint8_t x = 0;
    dest[x++] = whio_uint16_tag_char;
    dest[x++] = (unsigned char)(i>>8) & mask;
    dest[x++] = (unsigned char)(i & mask);
    return whio_sizeof_encoded_uint16;
}

int whio_uint16_decode( unsigned char const * src, uint16_t * tgt )
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

static const unsigned char whio_uint8_tag_char = 0x80 | 8;
size_t whio_uint8_encode( unsigned char * dest, uint8_t i )
{
    if( ! dest ) return 0;
    dest[0] = whio_uint8_tag_char;
    dest[1] = i;
    return whio_sizeof_encoded_uint8;
}

int whio_uint8_decode( unsigned char const * src, uint8_t * tgt )
{
    if( ! src ) return whio_rc.ArgError;
    if( whio_uint8_tag_char != src[0] )
    {
	return whio_rc.ConsistencyError;
    }
    *tgt = src[1];
    return whio_rc.OK;
}


size_t whio_uint32_array_encode( unsigned char * dest, size_t n, uint32_t const * list )
{
    size_t i = (dest && n && list) ? 0 : n;
    size_t rc = 0;
    for( ; i < n; ++i, ++rc )
    {
	if( whio_sizeof_encoded_uint32 != whio_uint32_encode( dest, *(list++) ) )
	{
	    break;
	}
    }
    return rc;
}

size_t whio_uint32_array_decode( unsigned char const * src, size_t n, uint32_t * list )
{
    size_t i = (src && n && list) ? 0 : n;
    size_t rc = 0;
    for( ; i < n; ++i, ++rc )
    {
	if( whio_rc.OK != whio_uint32_decode( src, &list[i] ) )
	{
	    break;
	}
    }
    return rc;
}

static const unsigned char whio_cstring_tag_char = 0x80 | '"';
size_t whio_cstring_encode( unsigned char * dest, char const * s, uint32_t n )
{
    if( ! dest || !s ) return 0;
    if( ! n )
    {
	char const * x = s;
	loop: if( x && *x ) { ++x; ++n; goto loop; }
	//for( ; x && *x ; ++x, ++n ){}
    }
    *(dest++) = whio_cstring_tag_char;
    size_t rc = whio_uint32_encode( dest, n );
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

int whio_cstring_decode( unsigned char const * src, char ** tgt, size_t * length )
{
    if( !src || ! tgt ) return whio_rc.ArgError;

    if( whio_cstring_tag_char != *src )
    {
	return whio_rc.ConsistencyError;
    }
    ++src;
    uint32_t slen = 0;
    int rc = whio_uint32_decode( src, &slen );
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
whio_size_t whio_size_t_encode( unsigned char * dest, whio_size_t v )
{
#if WHIO_SIZE_T_BITS == 64
    return whio_uint64_encode( dest, v );
#elif WHIO_SIZE_T_BITS == 32
    return whio_uint32_encode( dest, v );
#elif WHIO_SIZE_T_BITS == 16
    return whio_uint16_encode( dest, v );
#elif WHIO_SIZE_T_BITS == 8
    return whio_uint8_encode( dest, v );
#else
#error "whio_size_t size (WHIO_SIZE_T_BITS) is not supported!"
#endif
}

int whio_size_t_decode( unsigned char const * src, whio_size_t * v )
{
#if WHIO_SIZE_T_BITS == 64
    return whio_uint64_decode( src, v );
#elif WHIO_SIZE_T_BITS == 32
    return whio_uint32_decode( src, v );
#elif WHIO_SIZE_T_BITS == 16
    return whio_uint16_decode( src, v );
#elif WHIO_SIZE_T_BITS == 8
    return whio_uint8_decode( src, v );
#else
#error "whio_size_t is not a supported type!"
#endif
}

whio_size_t whio_dev_size_t_encode( whio_dev * dev, whio_size_t v )
{
    unsigned char buf[whio_sizeof_encoded_size_t];
    whio_size_t_encode( buf, v );
    return dev->api->write( dev, &buf, whio_sizeof_encoded_size_t );
}

int whio_dev_size_t_decode( whio_dev * dev, whio_size_t * tgt )
{
    unsigned char buf[whio_sizeof_encoded_size_t]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint16 is the correct size. */
    memset( buf, 0, whio_sizeof_encoded_size_t );
    size_t rc = dev->api->read( dev, buf  /*Flawfinder: ignore  This is safe in conjunction with whio_sizeof_encoded_uint16*/, whio_sizeof_encoded_size_t );
    return ( whio_sizeof_encoded_size_t == rc )
        ? whio_size_t_decode( buf, tgt )
        : whio_rc.IOError;
}


size_t whio_dev_uint64_encode( whio_dev * dev, uint64_t i )
{
    unsigned char buf[whio_sizeof_encoded_uint64]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint64 is the correct size. */
    size_t const x = whio_uint64_encode( buf, i );
    return ( whio_sizeof_encoded_uint64 == x )
        ? whio_dev_write( dev, buf, whio_sizeof_encoded_uint64 )
        : 0;
}

int whio_dev_uint64_decode( whio_dev * dev, uint64_t * tgt )
{
    if( ! dev || ! tgt ) return whio_rc.ArgError;
    unsigned char buf[whio_sizeof_encoded_uint64]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint64 is the correct size. */
    memset( buf, 0, whio_sizeof_encoded_uint64 );
    size_t rc = dev->api->read( dev, buf  /*Flawfinder: ignore  This is safe in conjunction with whio_sizeof_encoded_uint64*/, whio_sizeof_encoded_uint64 );
    return ( whio_sizeof_encoded_uint64 == rc )
        ? whio_uint64_decode( buf, tgt )
        : whio_rc.IOError;
}


size_t whio_dev_uint32_encode( whio_dev * dev, uint32_t i )
{
    unsigned char buf[whio_sizeof_encoded_uint32]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint32 is the correct size. */
    size_t const x = whio_uint32_encode( buf, i );
    return ( whio_sizeof_encoded_uint32 == x )
        ? whio_dev_write( dev, buf, whio_sizeof_encoded_uint32 )
        : 0;
}

int whio_dev_uint32_decode( whio_dev * dev, uint32_t * tgt )
{
    if( ! dev || ! tgt ) return whio_rc.ArgError;
    unsigned char buf[whio_sizeof_encoded_uint32]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint32 is the correct size. */
    memset( buf, 0, whio_sizeof_encoded_uint32 );
    size_t rc = dev->api->read( dev, buf  /*Flawfinder: ignore  This is safe in conjunction with whio_sizeof_encoded_uint32*/, whio_sizeof_encoded_uint32 );
    return ( whio_sizeof_encoded_uint32 == rc )
        ? whio_uint32_decode( buf, tgt )
        : whio_rc.IOError;
}


static const unsigned char whio_dev_uint16_tag_char = 0x80 | 16;
size_t whio_dev_uint16_encode( whio_dev * dev, uint16_t i )
{
    unsigned char buf[whio_sizeof_encoded_uint16]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint16 is the correct size. */
    size_t const x = whio_uint16_encode( buf, i );
    return ( whio_sizeof_encoded_uint16 == x )
        ? whio_dev_write( dev, buf, whio_sizeof_encoded_uint16 )
        : 0;
}

int whio_dev_uint16_decode( whio_dev * dev, uint16_t * tgt )
{
    if( ! dev || ! tgt ) return whio_rc.ArgError;
    unsigned char buf[whio_sizeof_encoded_uint16]; /* Flawfinder: ignore This is intentional and safe as long as whio_sizeof_encoded_uint16 is the correct size. */
    memset( buf, 0, whio_sizeof_encoded_uint16 );
    size_t rc = dev->api->read( dev, buf  /*Flawfinder: ignore  This is safe in conjunction with whio_sizeof_encoded_uint16*/, whio_sizeof_encoded_uint16 );
    return ( whio_sizeof_encoded_uint16 == rc )
        ? whio_uint16_decode( buf, tgt )
        : whio_rc.IOError;
}

size_t whio_dev_uint32_array_encode( whio_dev * dev, size_t n, uint32_t const * list )
{
    size_t i = (dev && n && list) ? 0 : n;
    size_t rc = 0;
    for( ; i < n; ++i, ++rc )
    {
	if( whio_sizeof_encoded_uint32 != whio_dev_uint32_encode( dev, *(list++) ) )
	{
	    break;
	}
    }
    return rc;
}

size_t whio_dev_uint32_array_decode( whio_dev * dev, size_t n, uint32_t * list )
{
    size_t i = (dev && n && list) ? 0 : n;
    size_t rc = 0;
    for( ; i < n; ++i, ++rc )
    {
	if( whio_rc.OK != whio_dev_uint32_decode( dev, &list[i] ) )
	{
	    break;
	}
    }
    return rc;
}

static const unsigned char whio_dev_cstring_tag_char = 0x80 | '"';
whio_size_t whio_dev_cstring_encode( whio_dev * dev, char const * s, uint32_t n )
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
    rc += whio_dev_uint32_encode( dev, n );
    if( (1 + whio_sizeof_encoded_uint32) != rc ) return rc;
    return rc + dev->api->write( dev, s, (whio_size_t)n );
}

int whio_dev_cstring_decode( whio_dev * dev, char ** tgt, uint32_t * length )
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
    int rc = whio_dev_uint32_decode( dev, &slen );
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
/* end file src/whio_encode.c */
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
/* begin file src/whefs_details.c */
#line 8 "src/whefs_details.c"
#if !defined(WANDERINGHORSE_NET_WHEFS_DETAILS_C_INCLUDED)
#define WANDERINGHORSE_NET_WHEFS_DETAILS_C_INCLUDED 1
#if ! defined __STDC_FORMAT_MACROS
#  define __STDC_FORMAT_MACROS 1
#endif

/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/

  License: Public Domain
*/

/** @file whefs_details.c

  This file contains private implementation details for whefs which
  are shared amongst implementation files but which are NOT part of
  the public API. It is intended to be included into the C files which
  need it, like a rather, rather than compiled by itself. The various extern
  objects/funcs declared here are likely implemented in whefs.c, but may be
  in other source files.
*/

#include <assert.h>

#if WHEFS_CONFIG_ENABLE_THREADS
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/** @enum whefs_debug_flags
   Debugging flags for use with whdbg() and friends.
*/
enum whefs_debug_flags {
WHEFS_DBG_F_ERROR = WHDBG_ERROR,
WHEFS_DBG_F_WARNING = WHDBG_WARNING,
WHEFS_DBG_F_FIXME = WHDBG_FIXME,
WHEFS_DBG_F_NYI = WHDBG_NYI,
WHEFS_DBG_F_FYI = WHDBG_FYI,
WHEFS_DBG_F_ALWAYS = WHDBG_ALWAYS,
WHEFS_DBG_F_VERBOSE = WHDBG_VERBOSE,
WHEFS_DBG_F_mymask = 0x0f000000,
WHEFS_DBG_F_LOCK = WHEFS_DBG_F_mymask & 0x01000000,
WHEFS_DBG_F_CACHE = WHEFS_DBG_F_mymask & 0x02000000,
WHEFS_DBG_F_DEFAULTS_CLIENT = WHEFS_DBG_F_WARNING | WHEFS_DBG_F_ERROR | WHDBG_NYI,
WHEFS_DBG_F_DEFAULTS_HACKER = WHEFS_DBG_F_DEFAULTS_CLIENT | WHDBG_FIXME | WHEFS_DBG_F_FYI | WHDBG_VERBOSE, // | WHEFS_DBG_F_CACHE, // | WHEFS_DBG_F_LOCK,

#if defined(NDEBUG)
WHEFS_DBG_F_DEFAULT = 0 /* they'll be if(0)'d out in this case, anyway. */
#else
WHEFS_DBG_F_DEFAULT = WHEFS_DBG_F_DEFAULTS_CLIENT
//WHEFS_DBG_F_DEFAULTS_HACKER
#endif
};

#define WHEFS_DBG WHDBG(WHEFS_DBG_F_ALWAYS)
#define WHEFS_DBG_ERR WHDBG(WHEFS_DBG_F_ERROR)
#define WHEFS_DBG_WARN WHDBG(WHEFS_DBG_F_WARNING)
#define WHEFS_FIXME WHDBG(WHEFS_DBG_F_FIXME)
#define WHEFS_DBG_LOCK WHDBG(WHEFS_DBG_F_LOCK)
#define WHEFS_DBG_CACHE WHDBG(WHEFS_DBG_F_CACHE)
#define WHEFS_VERBOSE WHDBG(WHEFS_DBG_F_VERBOSE)
#define WHEFS_NYI WHDBG(WHEFS_DBG_F_NYI)("Not yet implemented!");
#define WHEFS_DBG_FYI WHDBG(WHEFS_DBG_F_FYI)

/** @enum whefs_flags

Flags used for various purposes within the VFS internals.

MAINTENANCE REMINDER: whefs_block::flags is uint8, so the flags
used with that type need to stay small.
*/
typedef enum {
/**
   Mark unused inodes.
 */
WHEFS_FLAG_Unused = 0x00,
/**
   Mark used inodes/blocks.
 */
WHEFS_FLAG_Used = 0x01,
/**
   Mark an fs or inode object as readable.
*/
WHEFS_FLAG_Read = 0x02,
/**
   Mark an fs or inode object as read/write.
 */
WHEFS_FLAG_Write = 0x04,
WHEFS_FLAG_ReadWrite = WHEFS_FLAG_Write | WHEFS_FLAG_Read,
WHEFS_FLAG_Opened = 0x8,
/**
   If a whefs_fs mmap()s the underlying storage then this flag
   is set.
*/
WHEFS_FLAG_FS_IsMMapped = 0x10,
/**
   If set then the inode names hashcode cache
   is enabled.
*/
WHEFS_FLAG_FS_EnableHashCache = 0x20,
//WHEFS_FLAG_FS_AutoExpand = 0x0020,
/**
   Mark error state for whefs_file objects.
*/
WHEFS_FLAG_FileError = 0x0100
} whefs_flags;

/**
   Array index symbols for whefs_fs::offsets.
*/
enum whefs_fs_offsets {
WHEFS_OFF_SIZE = 0,
WHEFS_OFF_CORE_MAGIC,
WHEFS_OFF_CLIENT_MAGIC,
WHEFS_OFF_OPTIONS,
WHEFS_OFF_INODE_NAMES,
WHEFS_OFF_INODES_NO_STR,
WHEFS_OFF_BLOCK_TABLE,
WHEFS_OFF_BLOCKS,
WHEFS_OFF_EOF,
WHEFS_OFF_COUNT /* must be the last entry! */
};
/**
   Array index symbols for whefs_fs::sizes.
*/
enum whefs_fs_sizes {
WHEFS_SZ_INODE_W_STR = 0,
WHEFS_SZ_INODE_NO_STR,
WHEFS_SZ_INODE_NAME,
WHEFS_SZ_BLOCK,
WHEFS_SZ_OPTIONS,
WHEFS_SZ_COUNT /* must be the last entry! */
};

/** @def WHEFS_FS_HASH_CACHE_IS_ENABLED

WHEFS_FS_HASH_CACHE_IS_ENABLED() returns true if whefs_fs object FS
has the WHEFS_FLAG_FS_EnableHashCache flag set, else false.
*/
#define WHEFS_FS_HASH_CACHE_IS_ENABLED(FS) ((FS) && (WHEFS_FLAG_FS_EnableHashCache & (FS)->flags))

/**
   For use with whefs_fs_closer_list::type.
*/
enum whefs_fs_closer_types {
/** Type flag for whefs_file objects. */
WHEFS_CLOSER_TYPE_FILE = 'f',
/** Type flag for whio_dev objects. */
WHEFS_CLOSER_TYPE_DEV = 'd',
/** Type flag for whio_stream objects. */
WHEFS_CLOSER_TYPE_STREAM = 's'
};
/** @struct whefs_fs_closer_list

   A type for holding a list of "opened objects" which refer to
   underlying opened inodes.  We have this list only so that
   whefs_fs_finalize() can clean up opened handles if the client did
   not do so before finalizing the fs.

   How to safely close an object depends on how it was opened,
   i.e. what type it is. The factory function responsible for
   associating i/o-related objects with inodes must add their
   objects to fs->closers when they are opened and remove
   them from fs->closers when they are closed. If the fs is
   shut down before the client closes the objects in that list
   then whefs_fs_finalize() closes them all.
   
*/
struct whefs_fs_closer_list
{
    /** Must be a value from enum whefs_fs_closer_types. */
    char type;
    /**
       The object pointed to by this object. It MUST be properly set
       to the same object type set in the 'type' member. See the
       whefs_fs_closer_types enum.
    */
    union
    {
        whefs_file * file;
        whio_dev * dev;
        whio_stream * stream;
    } item;
    /** Next entry in the list. */
    struct whefs_fs_closer_list * next;
    /** Previous entry in the list. */
    struct whefs_fs_closer_list * prev;
};
typedef struct whefs_fs_closer_list whefs_fs_closer_list;

/** Empty initialize object for whefs_fs_closer_list instances. */
#define whefs_fs_closer_list_init_m { 0/*type*/,{/*item*/NULL},NULL/*next*/}

/** Empty initialize object for whefs_fs_closer_list instances. */
extern const whefs_fs_closer_list whefs_fs_closer_list_init;

/**
   Allocates a new whefs_fs_closer_list instance containing
   zero-initialized values.
*/
whefs_fs_closer_list * whefs_fs_closer_list_alloc();

/**
   Frees x and re-links its neighbors to one-another.
*/
void whefs_fs_closer_list_free( whefs_fs_closer_list * x );

/**
   Closes the item referenced by head. If right is true then
   all items to the right of head are also closed.
*/
int whefs_fs_closer_list_close( whefs_fs_closer_list * head, bool right );

/**
   Adds f to fs's close-at-shutdown list. Since a whefs_file is
   just a small wrapper around a whio_dev specialization,
   this routine requires that an entry already exists for
   f->dev in fs->closers. If that entry is found then it is
   "promoted" to a WHEFS_CLOSER_TYPE_FILE entry, so that
   whefs_fclose() will be used to free the entry (which includes
   the f->dev).

   Returns whefs_rc.OK on success.

   Closing f will automatically remove it from the list.
*/
int whefs_fs_closer_file_add( whefs_fs * fs, whefs_file * f );

/**
   Removes f from fs's close-at-shutdown list.

   Returns whefs_rc.OK on success. Not finding an entry is considered
   success because this is normal behaviour when the close-list is
   cleaned up as part of the shutdown process.
*/
int whefs_fs_closer_file_remove( whefs_fs * fs, whefs_file const * f );

/**
   Adds d to fs's close-at-shutdown list.

   Closing d will automatically remove it from the list.
*/
int whefs_fs_closer_dev_add( whefs_fs * fs, whio_dev * d );
/**
   Removes d from fs's close-at-shutdown list.

   See whefs_fs_closer_file_remove() for more info.
*/
int whefs_fs_closer_dev_remove( whefs_fs * fs, whio_dev const * d );

/**
   Adds s to fs's close-at-shutdown list. s MUST be a proxy for the
   whio_dev d and d MUST have been added to fs via
   whefs_fs_closer_dev_add(), or results are undefined.

   Closing s will automatically remove it from the list.
*/
int whefs_fs_closer_stream_add( whefs_fs * fs, whio_stream * s, whio_dev const * d );

/**
   Removes s from fs's close-at-shutdown list.

   See whefs_fs_closer_file_remove() for more info.
*/
int whefs_fs_closer_stream_remove( whefs_fs * fs, whio_stream const * s );

/**
   Main filesystem structure.
*/
struct whefs_fs
{
    /**
       General flags.
    */
    unsigned int flags;
    /**
       Error code.
     */
    unsigned int err;
    /**
       Stores file position offsets for commonly-used
       reference points within a vfs.

       The array indexes should be from the whefs_fs_offsets enum.
    */
    uint32_t offsets[WHEFS_OFF_COUNT];
    /**
       Stores sizes of commonly used data structures.

       The array indexes should be from the whefs_fs_sizes enum.
    */
    uint32_t sizes[WHEFS_SZ_COUNT];
    /**
       Underlying i/o device for the backing store.

       Maintenance warning:

       If this object get swapped out during the life of this object
       (the mmap() handler does that) then the objects in
       whefs_fs::fences which were initialized to point to the
       previous device must be cleaned before swapping the device and
       re-set-up after swapping out the device.

       That goes for any copied pointers back to whefs_fs::dev.
    */
    whio_dev * dev;

    /**
       If true, this object owns the dev member and will destroy
       it when the time comes.
    */
    bool ownsDev;
    /**
       The expected size of the container file. It must not change
       after initialization of the vfs state (i.e. after opening or
       mkfs).

       This really isn't needed, and is more of a sanity checking tool
       than anything. It has come in quite handy for that purpose.
    */
    uint32_t filesize;
    /**
       All "opened" inodes are store in this linked list.
    */
    whefs_inode_list * opened_nodes;
    whefs_fs_closer_list * closers;
    /**
       IFF the fs thinks that it is using file-based storage it may
       try to enable some locking features. This is the file number of
       the underlying file. This relies on whio_dev::ioctl() for the
       underlying device supporting the whio_dev_ioctl_FILE_fd ioctl
       command.
    */
    int fileno;

    /**
       Bitsets for the minimal internal caching we do.
     */
    struct _bits
    {
	/**
	   bitset for marking in-use inodes.

	   Bits:
	     In-use: offset == inode.id
	*/
	whbits i;
	/**
	   bitset for marking in-use blocks.

	   Bits:
	     In-use: offset == block.id
	*/
	whbits b;
        /** Whether or not the inode bitset has been loaded. */
        bool i_loaded;
        /** Whether or not the block bitset has been loaded. */
        bool b_loaded;
    } bits;
    struct _hints
    {
	/**
	   This is used as a starting hint for whefs_pos_next_free().
	   Profiling showed that that function (due to its horribly
	   linear nature) took the single biggest time slot for some
	   use cases.  By using this hint we reduce the time (in one
	   test app) from 6% of the runtime to 0.5% of the runtime.

	   Each time whefs_block_next_free() finds a block, it sets
	   this variable to the offset of that block, plus 1. The next
	   time it is run, it starts at that offset. If a block is
	   wiped (via whefs_block_wipe()) and its ID is lower than
	   this value then this variable is set to that ID, such that
	   the next call to whefs_block_next_free() will start at that
	   block (and hit it on the first attempt). That transforms
	   whefs_block_next_free() into a linear operation only the
	   first time it is run (when this var is at its default
	   starting point and we may have used blocks we don't know
	   about).
	*/
	whefs_id_type unused_block_start;

	/**
	   This is the inode counterpart of unused_block_start, and is
	   used by whefs_inode_next_free() to speed up searching in
	   the same way that unused_block_start is used for blocks.
	*/
	whefs_id_type unused_inode_start;
    } hints;

    /**
       Client-configurable vfs options. Except in some very controlled
       circumstances, these must not change after initialization of
       the vfs state or the fs will become corrupted.
    */
    whefs_fs_options options;
    /** Holder for threading-related data. */
    struct thread_info
    {
        int placeholder;
#if WHEFS_CONFIG_ENABLE_THREADS
#endif
    } threads;
    struct _caches
    {
        whefs_hashid_list * hashes;
        whefs_hashval_type (*hashfunc)( char const * vstr);
    } cache;
};

/** Empty initialization object. */
extern const whefs_fs whefs_fs_init;

/** inode/block in-use caching... */
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
#define WHEFS_CACHE_ASSERT(NID) (assert(0 && ("bit #" # NID " out of range! Debug to here and look for fs->bits.{i,b}.sz_bits and friends")),0)
#define WHEFS_ICACHE_PRECHECK(FS,NID) (FS && FS->bits.i.bytes && ((FS->bits.i.sz_bits > NID)||WHEFS_CACHE_ASSERT(NID)))
#define WHEFS_ICACHE_SET_USED(FS,NID) (WHEFS_ICACHE_PRECHECK(FS,NID) && (WHBITS_SET(&FS->bits.i,NID)))
#define WHEFS_ICACHE_UNSET_USED(FS,NID) (WHEFS_ICACHE_PRECHECK(FS,NID) && (WHBITS_UNSET(&FS->bits.i,NID)))
#define WHEFS_ICACHE_IS_USED(FS,NID) (WHEFS_ICACHE_PRECHECK(FS,NID) && (WHBITS_GET(&FS->bits.i,NID)))
#else
#define WHEFS_ICACHE_SET_USED(FS,NID)
#define WHEFS_ICACHE_UNSET_USED(FS,NID)
#define WHEFS_ICACHE_IS_USED(FS,NID)
#endif

#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
#define WHEFS_BCACHE_PRECHECK(FS,NID) (FS &&  FS->bits.b.bytes && ((FS->bits.b.sz_bits > NID)||WHEFS_CACHE_ASSERT(NID)))
#define WHEFS_BCACHE_SET_USED(FS,NID) (WHEFS_BCACHE_PRECHECK(FS,NID) && WHBITS_SET(&FS->bits.b,NID))
#define WHEFS_BCACHE_UNSET_USED(FS,NID) (WHEFS_BCACHE_PRECHECK(FS,NID) && WHBITS_UNSET(&FS->bits.b,NID))
#define WHEFS_BCACHE_IS_USED(FS,NID) (WHEFS_BCACHE_PRECHECK(FS,NID) && WHBITS_GET(&FS->bits.b,NID))
#else
#define WHEFS_BCACHE_SET_USED(FS,NID)
#define WHEFS_BCACHE_UNSET_USED(FS,NID)
#define WHEFS_BCACHE_IS_USED(FS,NID) THIS_MUST_BE_IN_A_BLOCK_GUARDED_BY__WHEFS_CONFIG_ENABLE_BITSET_CACHE
#endif


#define WHEFS_FS_ISRO(FS) ((FS) && ((FS)->flags & WHEFS_FLAG_Read))
#define WHEFS_FS_ISRW(FS) ((FS) && ((FS)->flags & WHEFS_FLAG_Write))
//#define WHEFS_FS_ISERR(F) ((F) && ((F)->err != WHEFS_ERR_None))

/**
   Equivalent to calling whio_dev::read() on fs's underlying i/o
   device.
*/
whio_size_t whefs_fs_read( whefs_fs * fs, void * dest, whio_size_t n );
whio_size_t whefs_fs_readat( whefs_fs * fs, whio_size_t pos, void * dest, whio_size_t n );
/**
   Equivalent to calling whio_dev::write() on fs's underlying i/o
   device.
*/
whio_size_t whefs_fs_write( whefs_fs * fs, void const * src, whio_size_t n );
/**
   Equivalent to calling whefs_fs_seek(), then whefs_fs_write(). Returns the
   number of bytes written, or 0 if seek fails.
*/
whio_size_t whefs_fs_writeat( whefs_fs * fs, whio_size_t pos, void const * src, whio_size_t n );
/**
   Equivalent to calling whio_dev::seek() on fs's underlying i/o
   device.
*/
whio_size_t whefs_fs_seek( whefs_fs * fs, off_t offset, int whence );
/**
   Equivalent to calling whio_dev::tell() on fs's underlying i/o
   device.
*/
whio_size_t whefs_fs_tell( whefs_fs * fs );

/**
   Tries to read the metadata associated with id, storing it in bl,
   which must not be 0. On success, bl is populated with the metadata
   and whefs_rc.OK is returned. On error, some other value is returned
   and bl may be in an undefined state.
*/
int whefs_block_read( whefs_fs * fs, whefs_id_type id, whefs_block * bl );

/**
   Returns the on-disk size of a data block, including the header, for
   the given options, or 0 if !opt.
*/
whio_size_t whefs_fs_sizeof_block( whefs_fs_options const * opt );

/**
   Reads the block following bl.  bl must be a valid block object
   (with a valid ID) and nextBlock must be valid memory (which will be
   overwritten). If bl has a next block, nextBlock is set to the
   contents of that block. bl and nextBlock may point to the same
   object.

   On successs, whefs_rc.OK is returned and nextBlock is populated.

   On error nextBlock is left in an undefined state and one of the
   following is returned:

   - !fs or !bl or !nextBlock or bl is not a valid block:
     whefs_rc.ArgError.

   - bl has no next block then whefs_rc.RangeError is returned.

   - If the read of the next block fails, the return value is that
   of whefs_block_read().
*/
int whefs_block_read_next( whefs_fs * fs, whefs_block const * bl, whefs_block * nextBlock );


/**
   fs is repositioned to the start of bl's on-disk position and bl's
   metadata is written to disk at that position.

   bl->id must be valid for fs.

   Returns whefs_rc.OK on success, else some other value.
*/
int whefs_block_flush( whefs_fs * fs, whefs_block const * bl );


/**
   Searches for the next free block and populates target with its
   metadata. On success, whefs_rc.OK is returned. If markUsed is true
   and a free block is found, the block is marked as being used,
   otherwise it is not marked (in which case a future call to this
   routine may return that same block).
*/
int whefs_block_next_free( whefs_fs * restrict fs, whefs_block * restrict tgt, bool markUsed );

/**
   Zeroes out parts of the given data block. Unlike most routines,
   which require only that bl->id is valid, bl must be fully populated
   for this to work.

   If data is true then the block's on-disk data bytes are zeroed out.

   If meta is true then the metadata associated with the block (block
   flags and next block ID) are reset to their default state.

   If deep is true then all linked blocks (starting at bl->next_block)
   are also cleaned with the same parameters.

   WARNING:

   If (meta && !deep), lost blocks will result if bl has child blocks! 
   Lost blocks are are marked used but are no longer reachable via a
   block chain, and thus cannot be directly recovered without manually
   fixing the block relationships.

   Returns whefs_rc.OK on success.
*/
int whefs_block_wipe( whefs_fs * fs, whefs_block * bl, bool data, bool meta, bool deep );

/**
   Fills all data bytes of the given block with 0, starting at the given starting
   position. If startPos is greater or equal to fs's block size, whefs_rc.RangeError
   is returned.

   On success, whefs_rc.OK is returned.
*/
int whefs_block_wipe_data( whefs_fs * fs, whefs_block const * bl, whio_size_t startPos );


/**
   Returns the on-disk position of the block with the given id,. fs
   must be opened and initialized. On error (!fs or !b, or b->id is
   out of range), 0 is returned.
*/
whio_size_t whefs_block_id_pos( whefs_fs const * fs, whefs_id_type id );

/**
   Returns the on-disk position of bl's data block (the point just
   past the block header), or 0 on error (!bl, !fs, or !bl->id).
*/
whio_size_t whefs_block_data_pos( whefs_fs const * fs, whefs_block const * bl );

/**
   Seeks to the given block's on-disk position. Returns whefs_rc.OK
   on success.
*/
int whefs_block_seek( whefs_fs const * fs, whefs_block const * bl );
/**
   Seeks to the on-disk position of the given block ID. Returns
   whefs_rc.OK on success.
*/
int whefs_block_id_seek( whefs_fs const * fs, whefs_id_type id );

/**
   Seeks to the on-disk position of the given block's data
   segment. On success it returns whefs_rc.OK and updates tgt
   (if tgt is not null).
*/
int whefs_block_seek_data( whefs_fs const * fs, whefs_block const * bl, whio_size_t * tgt );

/**
   Identical to whefs_block_seek_data() but takes a block ID instead
   of a whefs_block parameter.
*/
int whefs_block_id_seek_data( whefs_fs const * fs, whefs_id_type id, whio_size_t * tgt );


/** @def whefs_block_id_is_valid_m

   Internal implementation of the more public whefs_block_id_is_valid().

   Evaluates to true if block id BLID is valid for the given fs. That
   is, it has a non-zero id in a range legal for the given fs object.
 */
#define whefs_block_id_is_valid_m(FS,BLID) ((FS) && (BLID) && ((BLID) <= (FS)->options.block_count))
/** @def whefs_block_is_valid_m

   Internal implementation of the more public whefs_block_is_valid().

   Evaluates to true if block BL (a const pointer to a whefs_block) is
   valid for the given fs. That is, it has a non-zero id in a range
   legal for the given fs object.
 */
#define whefs_block_is_valid_m(FS,BL) ((BL) ? (whefs_block_id_is_valid_m((FS),(BL)->id)) : false)

#if WHEFS_MACROIZE_SMALL_CHECKS
#  define whefs_block_id_is_valid(FS,BL) whefs_block_id_is_valid_m(FS,BL)
#  define whefs_block_is_valid(FS,BL) whefs_block_is_valid_m(FS,BL)
#else
/**
   Returns true if blockID is "valid" for the given fs. That is, has a
   non-zero id in a range legal for the given fs object.
*/
bool whefs_block_id_is_valid( whefs_fs const * fs, whefs_id_type blockID );
/**
   Equivalent to whefs_block_id_is_valid(fs,bl->id). Returns false if
   !bl.
*/
bool whefs_block_is_valid( whefs_fs const * fs, whefs_block const * bl );
#endif


#if 0 // unused code
/**
   Appends a block to a chain of blocks.

   If bl is a valid block:

   - Find the last block in bl's chain. Call that B.
   - Append a new block after B. Call that B2.
   - Update B to point to B2.
   - Flush blocks to disk
   - Assign tgt to B2.

   If bl is not null and bl->next_block is not 0 then
   whefs_rc.ArgError is returned to avoid orphaning bl's current next
   block.

   If bl is not a valid block (is null or id is out of range)

   - Find next free block and mark it as used.
   - Flush block to disk.
   - Assign tgt to that block.

   On success returns whefs_rc.OK, else some other value is returned
   and tgt is left in an undefined state.

   It is theoretically OK for bl and tgt to be the same underlying
   object.
*/
int whefs_block_append( whefs_fs * fs, whefs_block const * bl, whefs_block * tgt );
#endif

/**
   This updates the internal used-blocks cache (if enabled) and search
   hints for the block with id bl->id.

   WARNING: as profiling shows that block validation check here is, over time,
   expensive, neither fs nor bl nor bl->id are verified. It is up to the caller
   to ensure that bl has a valid block ID for the given filesystem.
*/
void whefs_block_update_used( whefs_fs * fs, whefs_block const * bl );


/** @struct whefs_file

   Internal details for whefs_fs pseudofiles. They are created using
   whefs_fopen() and closed with whefs_fclose().
*/
struct whefs_file
{
    /**
       Underlying vfs.
    */
    whefs_fs * fs;
    /**
       Internal flags (RW/RO, etc)
    */
    uint32_t flags;
    /**
       i/o device specialized for whefs_inode access.
    */
    whio_dev * dev;
    /**
       inode ID. To be phased out.
    */
    whefs_id_type inode;
    /** This is only so whefs_file_name_get() will work. It
        may go away.
    */
    whefs_string name;
};
/** Empty initialization object. */
extern const whefs_file whefs_file_init;

/**
   Writes an object of type whefs_id_type to the given
   device. Returns the number of bytes written on succes, which
   will be whefs_sizeof_encoded_id_type.
 */
size_t whefs_dev_id_encode( whio_dev * dev, whefs_id_type v );

/**
   Identical to whefs_dev_id_encode(), but writes to a char array,
   which must be at least whefs_sizeof_encoded_id_type bytes long.

   Returns whefs_sizeof_encoded_id_type on success.
*/
size_t whefs_id_encode( unsigned char * dest, whefs_id_type v );

/**
   Tries to read a whefs_id_type object, which must have been encoded
   using whefs_id_encode(), from the given device. On success,
   whefs_rc.OK is returned and v (which may not be null) is updated to
   that value.
 */
int whefs_dev_id_decode( whio_dev * dev, whefs_id_type * v );

/**
   Identical to whefs_dev_id_decode(), but reads from a char array, which
   must be at least whefs_sizeof_encoded_id_type bytes long.
*/
int whefs_id_decode( unsigned char const * src, whefs_id_type * v );

/**
   If the library is not built with fcntl() support or fs's backing
   store does not report a file descriptor (via whio_dev_ioctl()) then
   this function does nothing and returns whefs_rc.UnsupportedError.
   Otherwise...

   It requests (and waits for) an advisory file lock on fs's
   underlying storage. If writeLock is true it requests an exclusive
   write lock, otherwise a read-only lock. The start, whence, and len
   parameters are described in the man page for fcntl() - their
   semantics are identical here.

   On success it returns 0 on success, on error some value defined by
   fcntl() (except as described above).

   @see whefs_fs_unlock()
   @see whefs_fs_lock_range()
   @see whefs_fs_unlock_range()
*/
int whefs_fs_lock( whefs_fs * fs, bool writeLock, off_t start, int whence, off_t len );

/**
   Unlocks a lock set via whefs_fs_lock(). If locking support is not compiled in
   or fs's data store does not report a file handle (see whefs_fs_lock()) then
   whefs_rc.UnsupportedError is returned, otherwise it returns 0 on success and
   some value defined by fcntl() on error.

   @see whefs_fs_lock()
   @see whefs_fs_lock_range()
   @see whefs_fs_unlock_range()
*/
int whefs_fs_unlock( whefs_fs * fs, off_t start, int whence, off_t len );

/** @struct whefs_fs_range_locker

   A type for holding parameters for a disk (un)locking request. See
   whefs_fs_lock_range() for details.

   @see whefs_fs_lock_range()
   @see whefs_fs_unlock_range()
*/
struct whefs_fs_range_locker
{
    /**
       Start position of lock/unlock request, relative
       to the whence member.
    */
    off_t start;
    /**
       Relative starting point of the lock/unlock request. Must be one
       of SEEK_SET, SEEK_CUR, or SEEK_END.
    */
    int whence;
    /**
       Length of the byte range for the lock/unlock request.
    */
    off_t len;
};
typedef struct whefs_fs_range_locker whefs_fs_range_locker;

/**
   Equivalent to whefs_fs_lock() but takes its arguments packages in an
   object (which may not be null). The reason for this overload is to simplify
   the implementation of matching lock/unlock calls by storing the lock
   arguments in an object which can be passed to both operations.

   @see whefs_fs_lock_range()
   @see whefs_fs_unlock()
   @see whefs_fs_unlock_range()
*/
int whefs_fs_lock_range( whefs_fs * fs, bool writeLock, whefs_fs_range_locker const * range );

/**
   Equivalent to whefs_fs_lock() but takes its arguments as an object (which
   may not be null). See whefs_fs_lock_range() for why.

   @see whefs_fs_lock()
   @see whefs_fs_unlock()
   @see whefs_fs_lock_range()
*/
int whefs_fs_unlock_range( whefs_fs * fs, whefs_fs_range_locker const * range );

/**
   Writes the name for the given inode ID in the names table.
   Only the first fs->options.filename_length bytes of name
   are used, or until the first null. The table's entry is
   padded with nulls. Returns whefs_rc.OK on success.

   The library uses uint16_t to store the string length, but doesn't
   allow strings longer thant WHEFS_MAX_FILENAME_LENGTH. If name is
   longer than fs->options.filename_length, whefs_rc.RangeError is
   returned.

   Returns whefs_rc.OK on success. Errors
   include: !name, id is not valid for fs, or an i/o error.
*/
int whefs_fs_name_write( whefs_fs * fs, whefs_id_type id, char const * name );


/**
   Returns the on-disk size of an inode name record for the given options,
   or 0 if !opt.
*/
size_t whefs_fs_sizeof_name( whefs_fs_options const * opt );

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* WANDERINGHORSE_NET_WHEFS_DETAILS_C_INCLUDED */
/* end file src/whefs_details.c */
/* begin file src/whefs_string.c */
#line 8 "src/whefs_string.c"
/**
   whefs_string implementations.
*/
#include <stdlib.h> // malloc()/free()
#include <string.h> // memset()

const whefs_string whefs_string_init = whefs_string_init_m;

whefs_string * whefs_string_alloc()
{
    whefs_string * x = (whefs_string*)malloc(sizeof(whefs_string));
    if( x ) *x = whefs_string_init;
    return x;
}


int whefs_string_copy_cstring( whefs_string * tgt, char const * str )
{
    if( ! str || !tgt ) return whefs_rc.ArgError;
    size_t slen = 0;
    {
	char const * x = str;
	for( ; x && *x; ++x, ++slen ) {}
    }
    if( ! slen )
    {
        tgt->length = 0;
        if( tgt->string /* && tgt->alloced */)
        {
            //tgt->string[0] = 0;
            memset( tgt->string, 0, tgt->alloced );
        }
        return 0;
    }
    if( tgt->alloced > slen )
    {
	memset( tgt->string + slen, 0, tgt->alloced - slen );
	memcpy( tgt->string, str, slen );
	tgt->length = slen;
	return whefs_rc.OK;
    }
    if( (slen+1) >= UINT16_MAX ) return whefs_rc.RangeError;
    const whefs_string_size_t alen =
        //slen + 1
        slen * 1.5 // FIXME: cap at WHEFS_MAX_FILENAME_LENGTH
        ;
    if( alen < slen )
    { /* overflow! */
	return whefs_rc.RangeError;
    }
    char * xp = (char *) realloc( tgt->string, alen );
    if( ! xp ) return whefs_rc.AllocError;
    tgt->string = xp;
    tgt->alloced = alen;
    tgt->length = slen;
    memset( xp + slen, 0, alen - slen );
    memcpy( tgt->string, str, slen );
    return whefs_rc.OK;
}

int whefs_string_clear( whefs_string * tgt, bool clearChain )
{
    if( ! tgt ) return whefs_rc.ArgError;
    while( tgt )
    {
	whefs_string * n = tgt->next;
        if( tgt->alloced && tgt->string ) free( tgt->string );
	*tgt = whefs_string_init;
	if( ! clearChain ) break;
	tgt = n;
    }
    return whefs_rc.OK;
}

int whefs_string_finalize( whefs_string * tgt, bool clearChain )
{
    if( ! tgt ) return whefs_rc.ArgError;
    while( tgt )
    {
	whefs_string * n = tgt->next;
	free( tgt->string );
	*tgt = whefs_string_init;
	free(tgt);
	if( ! clearChain ) break;
	tgt = n;
    }
    return whefs_rc.OK;
}

/* end file src/whefs_string.c */
/* begin file src/whefs_encode.c */
#line 8 "src/whefs_encode.c"
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

uint64_t whefs_bytes_hash( void const * data, uint32_t len )
{
    /**
       One-at-a-time hash code taken from:

       http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
    */
    if( ! data || !len ) return 0;
    unsigned const char *p = data;
    uint64_t h = 0;
    uint32_t i;
    for ( i = 0; i < len; i++ )
    {
	h += p[i];
	h += ( h << 10 );
	h ^= ( h >> 6 );
    }
    h += ( h << 3 );
    h ^= ( h >> 11 );
    h += ( h << 15 );
    return h;
}

/**
   tag byte for encoded whefs_id_type objects.
*/
static const unsigned int whefs_id_type_tag_char = 0x08 | 8;
size_t whefs_dev_id_encode( whio_dev * dev, whefs_id_type v )
{
#if WHEFS_ID_TYPE_BITS == 64
    return whio_dev_uint64_encode( dev, v );
#elif WHEFS_ID_TYPE_BITS == 32
    return whio_dev_uint32_encode( dev, v );
#elif WHEFS_ID_TYPE_BITS == 16
    return whio_dev_uint16_encode( dev, v );
#elif WHEFS_ID_TYPE_BITS == 8
    if( ! dev ) return whefs_rc.ArgError;
    unsigned char buf[2];
    buf[0] = whefs_id_type_tag_char;
    buf[1] = v;
    return dev->api->write( dev, buf, 2 );
#else
#error "whefs_id_type size (WHEFS_ID_TYPE_BITS) is not supported!"
#endif
}

size_t whefs_id_encode( unsigned char * dest, whefs_id_type v )
{
#if WHEFS_ID_TYPE_BITS == 64
    return whio_uint64_encode( dest, v );
#elif WHEFS_ID_TYPE_BITS == 32
    return whio_uint32_encode( dest, v );
#elif WHEFS_ID_TYPE_BITS == 16
    return whio_uint16_encode( dest, v );
#elif WHEFS_ID_TYPE_BITS == 8
    if( ! dest ) return whefs_rc.ArgError;
    dest[0] = whefs_id_type_tag_char;
    dest[1] = v;
    return whefs_sizeof_encoded_id_type;
#else
#error "whefs_id_type size (WHEFS_ID_TYPE_BITS) is not supported!"
#endif
}

int whefs_dev_id_decode( whio_dev * dev, whefs_id_type * v )
{
#if WHEFS_ID_TYPE_BITS == 64
    return whio_dev_uint64_decode( dev, v );
#elif WHEFS_ID_TYPE_BITS == 32
    return whio_dev_uint32_decode( dev, v );
#elif WHEFS_ID_TYPE_BITS == 16
    return whio_dev_uint16_decode( dev, v );
#elif WHEFS_ID_TYPE_BITS == 8
    if( ! v || ! dev ) return whefs_rc.ArgError;
    unsigned char buf[2] = {0,0};
    size_t sz = dev->api->read( dev, buf, 2 );
    if( 2 != sz)
    {
	return whefs_rc.IOError;
    }
    else if( buf[0] != whefs_id_type_tag_char )
    {
	return whefs_rc.ConsistencyError;
    }
    else
    {
	*v = buf[1];
	return whefs_rc.OK;
    }
#else
#error "whefs_id_type is not a supported type!"
#endif
}

int whefs_id_decode( unsigned char const * src, whefs_id_type * v )
{
#if WHEFS_ID_TYPE_BITS == 64
    return whio_uint64_decode( src, v );
#elif WHEFS_ID_TYPE_BITS == 32
    return whio_uint32_decode( src, v );
#elif WHEFS_ID_TYPE_BITS == 16
    return whio_uint16_decode( src, v );
#elif WHEFS_ID_TYPE_BITS == 8
    if( ! src ) return whefs_rc.ArgError;
    else if( src[0] != whefs_id_type_tag_char )
    {
	return whefs_rc.ConsistencyError;
    }
    else
    {
	if( v ) *v = src[1];
    }
    return whefs_rc.OK;
#else
#error "whefs_id_type is not a supported type!"
#endif
}

/* end file src/whefs_encode.c */
/* begin file src/whefs.c */
#line 8 "src/whefs.c"
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/


#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <string.h>


#if defined(__cplusplus)
extern "C" {
#endif


char const * whefs_home_page_url()
{
    return "http://code.google.com/p/whefs";
}

const whefs_rc_t whefs_rc =
    {
    0, /* OK */
    1, /* ArgError */
    2, /* IOError */
    3, /* AllocError */
    4, /* BadMagicError */
    5, /* InternalError */
    6, /* RangeError */
    7, /* FSFull */
    8, /* AccessError */
    9, /* ConsistencyError */
    10, /* NYIError */
    11, /* UnsupportedError */
    (whio_size_t)-1 /* SizeTError */,
    (whefs_id_type)-1 /* IDTypeEnd */
    };

const whefs_magic whefs_magic_default = WHEFS_MAGIC_DEFAULT;
const whefs_fs_options whefs_fs_options_default = WHEFS_FS_OPTIONS_DEFAULT;
const whefs_fs_options whefs_fs_options_nil = WHEFS_FS_OPTIONS_NIL;
 
const uint32_t * whefs_get_core_magic()
{
    return whefs_fs_magic_bytes;
}

char const * whefs_data_format_version_string()
{
    enum { bufLen = 60 };
    static char buf[bufLen] = {0,};
    if( ! *buf )
    {
	snprintf( buf, bufLen, "%4u-%02u-%02u with %02u-bit IDs",
		  whefs_fs_magic_bytes[0],
		  whefs_fs_magic_bytes[1],
		  whefs_fs_magic_bytes[2],
		  whefs_fs_magic_bytes[3] );
    }
    return buf;
}


#if 0
struct whefs_id_links
{
    whefs_id_type id;
    whefs_id_type parent;
    whefs_id_type next;
    whefs_id_type prev;
};
typedef struct whefs_id_links whefs_id_links;
int whefs_id_link( whefs_id_links * tgt, whefs_id_type const * parent, whefs_id_type const * next, whefs_id_type const * prev );
int whefs_id_link( whefs_id_links * tgt, whefs_id_type const * parent, whefs_id_type const * next, whefs_id_type const * prev )
{
    if( ! tgt ) return whefs_rc.ArgError;
    if( parent ) tgt->parent = *parent;
    if( next ) tgt->parent = *next;
    if( prev ) tgt->parent = *prev;
    return whefs_rc.OK;
}
#endif

void whefs_setup_debug( FILE * ostream, unsigned int flags )
{
    whdbg_set_stream( ostream );
    whdbg_set_flags( (-1==flags) ? WHEFS_DBG_F_DEFAULT : flags );
}


typedef struct
{
    char letter;
    unsigned int flag;
    char const * descr;
} whefs_dbg_flag_info;
static const whefs_dbg_flag_info whefs_dbg_flags[] =
    {// keep sorted on the letter field.
    {'a',WHDBG_ALWAYS,"All messages."},
    {'c',WHEFS_DBG_F_CACHE,"Caching messages."},
    {'d',WHEFS_DBG_F_DEFAULT,"Default log level."},
    {'e',WHEFS_DBG_F_ERROR,"Error messages."},
    {'f',WHEFS_DBG_F_FIXME,"FIXME messages."},
    {'h',WHEFS_DBG_F_DEFAULTS_HACKER,"Hacker-level messages."},
    {'l',WHEFS_DBG_F_LOCK,"Locking messages."},
    {'n',WHEFS_DBG_F_NYI,"NYI messages."},
    {'w',WHEFS_DBG_F_WARNING,"Warning messages."},
    {0,0,0}
    };

void whefs_setup_debug_arg( FILE * ostream, char const * arg )
{
    unsigned int flags = 0;
    if( arg ) for( ; *arg; ++arg )
    {
        whefs_dbg_flag_info const * fi = &whefs_dbg_flags[0];
        for( ; fi->letter && (*arg >= fi->letter); ++fi )
        {
            if( *arg == fi->letter )
            {
                flags |= fi->flag;
                break;
            }
        }
    }
    whefs_setup_debug( ostream, flags );
}

#if defined(__cplusplus)
} /* extern "C" */
#endif
/* end file src/whefs.c */
/* begin file src/whefs_fs.c */
#line 8 "src/whefs_fs.c"
/*
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain

  This file contains most of the guts of the whefs_fs object.
*/


#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <string.h>


#if WHEFS_CONFIG_ENABLE_FCNTL
#  include <fcntl.h>
#endif

#if WHEFS_CONFIG_ENABLE_MMAP
#  include <sys/mman.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


#define WHEFS_LOAD_CACHES_ON_OPEN 1 /* make sure and test whefs-cp and friends if setting this to 0. */
#if ! WHEFS_LOAD_CACHES_ON_OPEN
#  warning "(0==WHEFS_LOAD_CACHES_ON_OPEN) is is known to cause bugs. Fix it!"
/*
  Reminder to self: the problem here is one of "how do we distinguish empty names vs. not-cached names,"
and is complicated by the voodoo we use to store the strings inside whefs_fs::cache::strings.

20090623: The last i checked it appeared to work fine when disabled,
but i need to look closer to be sure.
*/
#endif

/**
   The various WHEFS_FS_STRUCT_xxx macros are parts of the whefs_fs structure.
*/

#if WHEFS_CONFIG_ENABLE_THREADS
/**
   WHEFS_FS_STRUCT_THREAD_INFO is the initializer for whefs_fs.threads.
*/
#  define WHEFS_FS_STRUCT_THREAD_INFO {/*threads*/ \
        0 /* placeholder */ \
    }
#else
#  define WHEFS_FS_STRUCT_THREAD_INFO {0/* placeholder */}
#endif


/* whefs_fs::cache struct ... */
#define WHEFS_FS_STRUCT_CACHE                \
    {/*cache*/                                  \
        0/*hashes*/,                        \
        whefs_hash_cstring/*hashfunc*/,     \
    }

/* whefs_fs::bits struct ... */
#define WHEFS_FS_STRUCT_BITS                  \
    { /* bits */ \
	WHBITS_INIT, /* i */ \
        WHBITS_INIT, /* b */             \
        false, /* i_loaded */            \
        false /* b_loaded */                \
    }

/* whefs_fs::hints struct ... */
#define WHEFS_FS_STRUCT_HINTS                  \
    { /* hints */ \
	1, /* unused_block_start */ \
	2 /* unused_inode_start == 2 b/c IDs 0 and 1 are reserved for not-an-inode and the root node */  \
    }

/**
   An empty whefs_fs object for us in initializing new objects.
*/
//const whefs_fs whefs_fs_init =
#define whefs_fs_init_m { \
    (WHEFS_CONFIG_ENABLE_STRINGS_HASH_CACHE ? WHEFS_FLAG_FS_EnableHashCache : 0), /* flags */ \
    0, /* err */ \
    {0}, /* offsets */ \
    {0}, /* sizes */ \
    0, /* dev */ \
    true, /* ownsDev */ \
    0, /* filesize */ \
    0, /* opened_nodes */ \
    0, /* closers */ \
    0, /* fileno */ \
    WHEFS_FS_STRUCT_BITS,    \
    WHEFS_FS_STRUCT_HINTS,   \
    WHEFS_FS_OPTIONS_DEFAULT, \
    WHEFS_FS_STRUCT_THREAD_INFO, \
    WHEFS_FS_STRUCT_CACHE,       \
    } /* end of whefs_fs */
const whefs_fs whefs_fs_init = whefs_fs_init_m;

#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate in the
   whefs_fs_alloc_slots object.
*/
whefs_fs_alloc_count = 2
};
static struct
{
    whefs_fs objs[whefs_fs_alloc_count];
    char used[whefs_fs_alloc_count];
    size_t next;
} whefs_fs_alloc_slots = { {whefs_fs_init_m}, {0}, 0 };
#endif
/** @internal

   Allocates an empty-initializes a new whefs_fs object, which the
   caller owns and must destroy with whefs_fs_free() (if the object is
   never used) or whefs_fs_finalize() (if it contains data which needs
   to be cleaned up).
*/
static whefs_fs * whefs_fs_alloc()
{
    whefs_fs * obj = 0;
#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = whefs_fs_alloc_slots.next;
    for( ; i < whefs_fs_alloc_count; ++i )
    {
	if( whefs_fs_alloc_slots.used[i] ) continue;
	whefs_fs_alloc_slots.next = i+1;
	whefs_fs_alloc_slots.used[i] = 1;
	obj = &whefs_fs_alloc_slots.objs[i];
	break;
    }
#endif /* WHEFS_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whefs_fs *) malloc( sizeof(whefs_fs) );
    if( obj ) *obj = whefs_fs_init;
    return obj;
}
/** @internal

    Deallocates obj.
*/
static void whefs_fs_free( whefs_fs * obj )
{
    if( obj ) *obj = whefs_fs_init;
    else return;
#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whefs_fs_alloc_slots.objs[0]) ||
	(obj > &whefs_fs_alloc_slots.objs[whefs_fs_alloc_count-1]) )
    { /* it does not belong to us */
	free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whefs_fs_alloc_slots.objs[0]);
	if( whefs_fs_alloc_slots.next > ndx ) whefs_fs_alloc_slots.next = ndx;
	whefs_fs_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    free(obj);
#endif /* WHEFS_CONFIG_ENABLE_STATIC_MALLOC */
}


int whefs_fs_lock( whefs_fs * restrict fs, bool writeLock, off_t start, int whence, off_t len )
{
#if WHEFS_CONFIG_ENABLE_FCNTL
    if( ! fs ) return whefs_rc.ArgError;
    //WHEFS_DBG_FYI("whefs_fs_lock(%p [fileno=%d],%d,%ld,%d,%ld)",fs,fs->fileno,writeLock,start,whence,len);
    if( fs->fileno < 1 ) return whefs_rc.UnsupportedError;
    else
    {
	struct flock lock;
	lock.l_type = writeLock ? F_WRLCK : F_RDLCK;
	lock.l_start = start;
	lock.l_whence = whence;
	lock.l_len = len;
	int rc = fcntl( fs->fileno, F_SETLKW, &lock);
        WHEFS_DBG_LOCK("whefs_fs_lock(fs=%p,writeMode=%d,start=%ld,whence=%d,len=%ld) rc=%d",fs,writeLock,start,whence,len,rc);
        return rc;
    }
#else
    return whefs_rc.UnsupportedError;
#endif
}

int whefs_fs_unlock( whefs_fs * restrict fs, off_t start, int whence, off_t len )
{
#if WHEFS_CONFIG_ENABLE_FCNTL
    if( ! fs ) return whefs_rc.ArgError;
    if( fs->fileno < 1 ) return whefs_rc.UnsupportedError;
    else
    {
	struct flock lock;
	lock.l_type = F_UNLCK;
	lock.l_start = start;
	lock.l_whence = whence;
	lock.l_len = len;
	int rc = fcntl( fs->fileno, F_SETLK, &lock);
        WHEFS_DBG_LOCK("whefs_fs_unlock(fs=%p,start=%ld,whence=%d,len=%ld) rc=%d",fs,start,whence,len,rc);
        return rc;
    }
#else
    return whefs_rc.UnsupportedError;
#endif
}

int whefs_fs_lock_range( whefs_fs * restrict fs, bool writeLock, whefs_fs_range_locker const * range )
{
    return (fs && range)
	? whefs_fs_lock( fs, writeLock, range->start, range->whence, range->len )
	: whefs_rc.ArgError;
}

int whefs_fs_unlock_range( whefs_fs * restrict fs, whefs_fs_range_locker const * range )
{
    return (fs && range)
	? whefs_fs_unlock( fs, range->start, range->whence, range->len )
	: whefs_rc.ArgError;
}

#if WHEFS_CONFIG_ENABLE_MMAP
/** Internal data for storing info about mmap()ed storage. */
typedef struct
{
    void * mem;
    whio_dev * fdev;
    int fileno;
    whio_size_t size;
    bool writeMode;
    bool async;
}  WhioDevMMapInfo;
static const WhioDevMMapInfo WhioDevMMapInfo_init = {NULL,NULL,0,0,false,false};

/** Internal whio_dev_api::flush() impl for mmap()'d storage. */
static int whio_dev_mmap_flush( whio_dev * dev )
{
    if(0) whio_dev_mmap_flush(0); /* avoid "static func defined but not used" warning. */
    //WHEFS_DBG("msyncing mmap()...");
    WhioDevMMapInfo * m = (WhioDevMMapInfo *)dev->client.data;
    return m->writeMode
        ? msync( m->mem, m->size, m->async ? MS_ASYNC : MS_SYNC )
        : whio_rc.OK;
}
/** Internal whio_dev_api::close() impl for mmap()'d storage. */
static bool whio_dev_mmap_close( whio_dev * dev )
{
    if(0) whio_dev_mmap_close(0); /* avoid "static func defined but not used" warning. */
    if( ! dev ) return false;
    //WHEFS_DBG("closing mmap()...");
    WhioDevMMapInfo * m = (WhioDevMMapInfo *)dev->client.data;
    if( ! m ) return true;
    if( m->mem )
    {
        dev->api->flush( dev );
        //WHEFS_DBG("munmap()...");
        munmap( m->mem, m->size );
    }
    if( m->fdev ) m->fdev->api->finalize(m->fdev);
    *m = WhioDevMMapInfo_init;
    free(m);
    dev->client.data = 0;
    return whio_dev_api_memmap.close( dev );
}

#endif // WHEFS_CONFIG_ENABLE_MMAP

/**
   If WHEFS_CONFIG_ENABLE_MMAP is true then this tries to mmap() the
   underlying storage (fs->dev) IF fs is read/write (profiling shows
   memmapping read access to cost more than direct file access). If it
   succeeds it replaces fs->dev with a proxy device. Returns
   whefs_rc.OK on success. Failure can be ignored unless you REALLY
   need mmap().

   If WHEFS_CONFIG_ENABLE_MMAP is false then whefs_rc.UnsupportedError
   is returned.

   If fs is already mmap()ed or is read-only then whefs_rc.OK is
   returned but fs is not modified.
*/
static int whefs_fs_mmap_connect( whefs_fs * fs )
{
    if( !fs || !fs->dev ) return whefs_rc.ArgError;
    //WHEFS_DBG("Trying mmap? fileno=%d",fs->fileno);
#if ! WHEFS_CONFIG_ENABLE_MMAP
    return whefs_rc.UnsupportedError;
#else
    if( WHEFS_FLAG_FS_IsMMapped & fs->flags ) return whefs_rc.OK;
    if( !whefs_fs_is_rw(fs) ) return whefs_rc.OK;
    if( fs->fileno < 1 ) return whefs_rc.UnsupportedError;
    /**
       HOLY FARGING SHITE! What a speed difference mmap() makes!!!

       Here we do a bit of trickery: we swap out fs->dev with a proxy
       device which mmap()'s the file.
    */
    static whio_dev_api whio_dev_api_mmap = {0};
    static bool doneIt = false;
    if( !doneIt )
    {
        whio_dev_api_mmap = whio_dev_api_memmap;
        whio_dev_api_mmap.flush = whio_dev_mmap_flush;
        whio_dev_api_mmap.close = whio_dev_mmap_close;
        doneIt = true;
    }
    whio_size_t dsz = whio_dev_size( fs->dev );
    void * m = mmap( 0, dsz, whefs_fs_is_rw(fs) ? PROT_WRITE : PROT_READ, MAP_SHARED, fs->fileno, 0 );
    if( ! m )
    {
        WHEFS_DBG_WARN("mmap() failed for %"WHIO_SIZE_T_PFMT" bytes of fs->fileno (#%d)!",dsz,fs->fileno);
        return whefs_rc.IOError;
    }
    whio_dev * md = whefs_fs_is_rw(fs)
        ? whio_dev_for_memmap_rw( m, dsz )
        : whio_dev_for_memmap_ro( m, dsz );
    if( ! md )
    {
        WHEFS_DBG_WARN("whio_dev_for_memmap_%s() failed for %"WHIO_SIZE_T_PFMT" bytes of fs->fileno (#%d)!",
                       (whefs_fs_is_rw(fs) ? "rw" : "ro"), dsz,fs->fileno);
        return whefs_rc.IOError;
    }
    md->api = &whio_dev_api_mmap;
    WhioDevMMapInfo * minfo = (WhioDevMMapInfo*)malloc(sizeof(WhioDevMMapInfo)); // FIXME: add static allocator for WhioDevMMApInfo
    if( ! minfo )
    {
        WHEFS_DBG_ERR("Allocation of %u bytes for WhioDevMMapInfo failed!",sizeof(WhioDevMMapInfo));
        md->api->finalize(md);
        return whefs_rc.AllocError;
    }
    md->client.data = minfo;
    minfo->size = dsz;
    minfo->fileno = fs->fileno;
    minfo->mem = m;
    minfo->fdev = fs->dev;
    minfo->writeMode = whefs_fs_is_rw(fs);
    minfo->async = WHEFS_CONFIG_ENABLE_MMAP_ASYNC ? true : false;
    fs->dev = md;
    fs->flags |= WHEFS_FLAG_FS_IsMMapped;
    WHEFS_DBG_FYI("Swapped out EFS file-based whio_dev with mmap() wrapper! Flushing in %s mode.",
                  WHEFS_CONFIG_ENABLE_MMAP_ASYNC ? "asynchronous" : "synchronous");
    return whefs_rc.OK;
#endif
}

/**
   If !WHEFS_CONFIG_ENABLE_MMAP this simply returns
   whefs_rc.UnsupportedError, otherwise:

   If fs->dev is a mmap() device proxy then it is removed and fs->dev
   is redirected to the non-proxy device. This is necessary before
   certain operations, namely a truncate() on an mmap()'d file.

   We should be able to add a whio_dev::truncate() impl to our proxy which
   could take care of re-mmap()ing for us.
*/
static int whefs_fs_mmap_disconnect( whefs_fs * fs )
{
#if ! WHEFS_CONFIG_ENABLE_MMAP
    return whefs_rc.UnsupportedError;
#else
    if( ! fs ) return whefs_rc.ArgError;
    if( ! (fs->flags & WHEFS_FLAG_FS_IsMMapped) ) return whefs_rc.OK;
    if( fs->fileno < 1 ) return whefs_rc.InternalError;
    if( fs->dev->impl.typeID != &whio_dev_api_memmap ) return whefs_rc.InternalError;
    // We appear to have an mmap() proxy in place...
    WhioDevMMapInfo * m = (WhioDevMMapInfo *)fs->dev->client.data;
    whio_dev * dx = m->fdev;
    m->fdev = 0;
    fs->dev->api->finalize(fs->dev);
    fs->dev = dx;
    WHEFS_DBG_FYI("Disconnected mmap() proxy. Restored fs->dev to %p.",
                  (void const *)fs->dev );
    return whefs_rc.OK;
#endif
}


whio_size_t whefs_fs_read( whefs_fs * restrict fs, void * dest, whio_size_t n )
{
    return (fs && fs->dev)
	? fs->dev->api->read( fs->dev, dest, n )
	: 0;
}
whio_size_t whefs_fs_write( whefs_fs * restrict fs, void const * src, whio_size_t n )
{
    return (fs && fs->dev)
	? fs->dev->api->write( fs->dev, src, n )
	: 0;
}

whio_size_t whefs_fs_writeat( whefs_fs * fs, whio_size_t pos, void const * src, whio_size_t n )
{
    whio_size_t x = whefs_fs_seek( fs, (off_t)pos, SEEK_SET );
    if( x != pos ) return 0;
    return whefs_fs_write( fs, src, n );
}

whio_size_t whefs_fs_readat( whefs_fs * fs, whio_size_t pos, void * dest, whio_size_t n )
{
    whio_size_t x = whefs_fs_seek( fs, (off_t)pos, SEEK_SET );
    if( x != pos ) return 0;
    return whefs_fs_read( fs, dest, n );
}

whio_size_t whefs_fs_seek( whefs_fs * restrict fs, off_t offset, int whence )
{
    return (fs && fs->dev)
	? fs->dev->api->seek( fs->dev, offset, whence )
	: whio_rc.SizeTError;
}

whio_size_t whefs_fs_tell( whefs_fs * restrict fs )
{
    return (fs && fs->dev)
	? fs->dev->api->tell( fs->dev )
	: whio_rc.SizeTError;
}

int whefs_fs_flush( whefs_fs * restrict fs )
{
    if(fs && fs->dev)
    {
        if( whefs_fs_is_rw(fs) )
        {
            return fs->dev->api->flush( fs->dev );
        }
        return whefs_rc.AccessError;
    }
    else
    {
        return whefs_rc.ArgError;
    }
}


typedef enum
{
WHEFS_ERR_None = 0x0000,
WHEFS_ERR_IO   = 0x10000000,
WHEFS_ERR_IO_READ   = WHEFS_ERR_IO | 0x01,
WHEFS_ERR_IO_WRITE   = WHEFS_ERR_IO | 0x02
} whefs_errors;

void whefs_fs_caches_names_clear( whefs_fs * restrict fs )
{
    if( fs->cache.hashes )
    {
        WHEFS_DBG_CACHE("Emptying names hash cache using %"WHEFS_ID_TYPE_PFMT" of %"WHEFS_ID_TYPE_PFMT" entries and %u bytes.",
                        fs->cache.hashes->count,
                        fs->cache.hashes->alloced,
                        whefs_hashid_list_sizeof(fs->cache.hashes) );
#if 0
        whefs_hashid * h = 0;
	whefs_id_type i;
        for( i = 0; i < fs->cache.hashes->count; ++i )
        {
            h = &fs->cache.hashes->list[i];
            WHEFS_DBG_CACHE("inode #%"WHEFS_ID_TYPE_PFMT" cached entry.", h->id);
        }
#endif
        whefs_hashid_list_alloc( &fs->cache.hashes, 0 );// reminder: we keep fs->cache.hashes itself until finalization.
    }
}
void whefs_fs_caches_clear( whefs_fs * restrict fs )
{
    whbits_free_bits( &fs->bits.i );
    whbits_free_bits( &fs->bits.b );
    whefs_fs_caches_names_clear( fs );
}

void whefs_fs_finalize( whefs_fs * restrict fs )
{
    if( ! fs ) return;
    whefs_fs_flush(fs);
    whefs_fs_mmap_disconnect( fs );
    if( fs->closers )
    {
	WHEFS_DBG_WARN("We're closing with opened objects! Closing them...");
        while( fs->closers->prev )
        {
            fs->closers = fs->closers->prev;
        }
        whefs_fs_closer_list * x = fs->closers;
        fs->closers = 0;
        whefs_fs_closer_list_close( x, true );
    }
    if( fs->opened_nodes )
    {
	WHEFS_DBG_WARN("We're closing with opened inodes! Closing them...");
        while( fs->opened_nodes )
        {
            WHEFS_DBG_WARN("Auto-closing inode #"WHEFS_ID_TYPE_PFMT", but leaking its whefs_file or whio_dev handle (if any).");
            whefs_inode_close( fs, &fs->opened_nodes->inode, fs->opened_nodes->inode.writer );
	}
        // FIXME: this doesn't stop us from leaking unclosed whefs_file or whio_dev handles!
        // We don't currently track those.
    }
    whefs_fs_caches_clear(fs);
    whefs_fs_setopt_hash_cache( fs, false, false );
    if( fs->dev )
    {
        /**
           Philosophical problem: because finalizing fs->dev
           might flush the device, *should* finalize fs->dev
           BEFORE we unlock. However, we cannot unlock once
           fs->dev is destroyed.
        */
        whefs_fs_unlock( fs, 0, SEEK_SET, 0 );
	if( fs->ownsDev ) fs->dev->api->finalize( fs->dev );
	fs->dev = 0;
    }
    whefs_fs_free( fs );
}

whefs_fs_options const * whefs_fs_options_get( whefs_fs const * restrict fs )
{
    return fs ? &fs->options : 0;
}
whefs_fs_options const * whefs_fs_opt( whefs_fs const * restrict fs )
{
    return fs ? &fs->options : 0;
}


/**
   Seeks to the start of fs, writes the magic bytes. Returns
   whefs_rc.OK on success.
*/
static int whefs_mkfs_write_magic( whefs_fs * restrict fs )
{
    if( ! fs || !fs->dev ) return whefs_rc.ArgError;
    /*
      TODO: encode the butes in a buffer and only do one write().

      TODO: double-check the expected positions again:
       
      fs->offsets[WHEFS_OFF_CORE_MAGIC]
      fs->offsets[WHEFS_OFF_CLIENT_MAGIC]
      fs->offsets[WHEFS_OFF_SIZE]
    */
    fs->dev->api->seek( fs->dev, 0L, SEEK_SET );
    whio_dev_uint32_array_encode( fs->dev, whefs_fs_magic_bytes_len, whefs_fs_magic_bytes );
    whio_dev_uint32_encode( fs->dev, fs->filesize /*will be overwritten at end of mkfs*/ );
    whio_dev_uint16_encode( fs->dev, fs->options.magic.length );
    const whio_size_t wrc = whio_dev_write( fs->dev, fs->options.magic.data, fs->options.magic.length );
    return (wrc == fs->options.magic.length)
	? whefs_rc.OK
	: whefs_rc.IOError;
}

/**
   Writes fs->options to the current position of the stream.  Returns
   whefs_rc.OK on success.
*/
static int whefs_mkfs_write_options( whefs_fs * restrict fs )
{
    whefs_fs_seek( fs, fs->offsets[WHEFS_OFF_OPTIONS], SEEK_SET );
    assert( fs->dev->api->tell( fs->dev ) == fs->offsets[WHEFS_OFF_OPTIONS] );
    size_t pos = whio_dev_uint32_encode( fs->dev, fs->options.block_size );
    size_t sz = whefs_dev_id_encode( fs->dev, fs->options.block_count );
    if( whefs_sizeof_encoded_id_type != sz ) return whefs_rc.IOError;
    sz = whefs_dev_id_encode( fs->dev, fs->options.inode_count );
    if( whefs_sizeof_encoded_id_type != sz ) return whefs_rc.IOError;
    pos += whio_dev_uint16_encode( fs->dev, fs->options.filename_length );
    return (pos>0) /* <--- this is not technically correct. */
	? whefs_rc.OK
	: whefs_rc.IOError;
}

/**
   Returns the on-disk size of whefs_fs_options objects.
*/
static size_t whefs_fs_sizeof_options()
{
    const size_t sz = whio_sizeof_encoded_uint32;
    return sz /* block_size */
	+ whefs_sizeof_encoded_id_type /* block_count */
	+ whefs_sizeof_encoded_id_type /* inode_count */
	+ whio_sizeof_encoded_uint16 /* filename_length */
	;
}

/**
   Returns the on-disk size of inode names for the given
   whefs_fs_options object.
*/
size_t whefs_fs_sizeof_name( whefs_fs_options const * opt )
{
    return !opt
	? 0
	: (whefs_sizeof_encoded_inode_name_header +  opt->filename_length);
}


#if 0 // not needed anymore?
/**
   Returns the on-disk position of the given inode's name record,
   or 0 if id is not valid for fs.
*/
static size_t whefs_name_pos( whefs_fs const * restrict fs, whefs_id_type id )
{
    if( ! whefs_inode_id_is_valid( fs, id ) )
    {
	return 0;
    }
    else
    {
	return fs->offsets[WHEFS_OFF_INODE_NAMES]
	    + ( (id-1) * fs->sizes[WHEFS_SZ_INODE_NAME] );
    }
}
#endif

/**
   Tag byte for use in encoding inode name table entries.
*/
static unsigned char const whefs_inode_name_tag_char = 0x80 | '\'';

int whefs_inode_name_get( whefs_fs * restrict fs, whefs_id_type id, whefs_string * tgt )
{ /* Maintenance reminder: this "should" be in whefs_inode.c, but it's not because
     of whefs_inode_name_tag_char.
   */
    if( ! tgt || ! whefs_inode_id_is_valid( fs, id ) ) return whefs_rc.ArgError;
    assert(fs->sizes[WHEFS_SZ_INODE_NAME] && "fs has not been set up properly!");
    int rc = 0;
    enum { bufSize = whefs_sizeof_encoded_inode_name };
    unsigned char buf[bufSize + 1];
    memset( buf, 0, bufSize + 1 );
    whio_size_t const toRead = whefs_fs_sizeof_name(&fs->options);
    assert( toRead <= bufSize );
    whio_size_t spos = fs->offsets[WHEFS_OFF_INODE_NAMES]
        + (fs->sizes[WHEFS_SZ_INODE_NAME] * (id-1));
    whio_size_t rsz = whefs_fs_readat( fs, spos, buf, toRead );
    if( toRead != rsz )
    {
	WHEFS_DBG_ERR("Error #%d reading inode #"WHEFS_ID_TYPE_PFMT"'s name record!",rc,id);
	return whefs_rc.IOError;
    }
    //unsigned char const * buf = fs->buffers.nodeName;
    if( buf[0] != whefs_inode_name_tag_char )
    {
	WHEFS_DBG_ERR("Error reading inode #%"WHEFS_ID_TYPE_PFMT"'s name record! "
		      "Expected byte value 0x%02x but got 0x%02x",
		      id, whefs_inode_name_tag_char, buf[0] );
	return whefs_rc.ConsistencyError;
    }
    unsigned char * bufP = buf;
    ++bufP; // skip tag byte
    bufP += whefs_sizeof_encoded_id_type //skip id field
        //+ whio_sizeof_encoded_uint64 //skip hash field
        ;
    uint16_t sl = 0; // string length
    rc = whio_uint16_decode( bufP, &sl );
    if( whio_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Could not decode string length token from inode #"WHEFS_ID_TYPE_PFMT"'s "
		      "name record! RC=%d",id,rc);
	return rc;
    }
    bufP += whio_sizeof_encoded_uint16; // skip over size field
    //bufP += whio_sizeof_encoded_uint64; // skip hash field
    rc = whefs_string_copy_cstring( tgt, (char const *)bufP );
    if( whio_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Copying of inode #"WHEFS_ID_TYPE_PFMT"'s name record failed! "
		      "RC=%d. String is [%s]",
		      id, rc, bufP );
    }
    if( tgt->length )
    {
        //WHEFS_DBG("Caching inode name [%s]",tgt->string);
        whefs_inode_hash_cache( fs, id, tgt->string );
        // A necessary evil to avoid lots of O(N) searches.
        if( fs->cache.hashes && ! fs->cache.hashes->skipAutoSort )
        {
            whefs_inode_hash_cache_sort(fs);
        }
    }
    return rc;
}

int whefs_fs_name_write( whefs_fs * restrict fs, whefs_id_type id, char const * name )
{
    if( ! whefs_inode_id_is_valid( fs, id ) || !name)
    {
	return whefs_rc.ArgError;
    }
    uint16_t slen = 0;
    {
	char const * c = name;
	uint16_t i = 0;
	for( ; c && *c && (i < fs->options.filename_length); ++i, ++c, ++slen )
	{
	}
	if( (i == fs->options.filename_length) && *c )
	{ /** too long! */
	    return whefs_rc.RangeError;
	}
    }
    int rc = 0;
    /**
       Encode the string to a temp buffer then write it in one go to
       disk. Takes more code than plain i/o, but using this approach
       here means much less overall i/o and requies less error
       handling for the encoding (which can't fail as long as we
       provide the proper parameters and memory buffer sizes).
    */
    const size_t bsz =
        fs->sizes[WHEFS_SZ_INODE_NAME]
        ;
    assert(fs->sizes[WHEFS_SZ_INODE_NAME] && "fs has not been set up properly!");
    assert( bsz == fs->sizes[WHEFS_SZ_INODE_NAME] );
    //    unsigned char * buf = fs->buffers.nodeName;
    enum { bufSize = whefs_sizeof_encoded_inode_name };
    unsigned char buf[bufSize+1];
    memset( buf+1, 0, bufSize );
    buf[0] = whefs_inode_name_tag_char;
    size_t off = 1;
    off += whefs_id_encode( buf + off, id );
    off += whio_uint16_encode( buf + off, slen );
    //uint64_t shash = 0UL; //whefs_bytes_hash( name, slen );
    //off += whefs_uint64_encode( buf + off, shash );
    memcpy( buf + off, name, slen );
    unsigned char const * dbgStr = buf+off;
    off += slen;
    if( off < bsz ) memset( buf + off, 0, bsz - off );
    assert( off <= bsz );
    const off_t spos = fs->offsets[WHEFS_OFF_INODE_NAMES] +
        (bsz * (id-1));
    whio_size_t const sz = whefs_fs_writeat( fs, spos, buf, bsz );
    if( bsz != sz )
    {
	WHEFS_DBG_ERR("Writing inode #%"WHEFS_ID_TYPE_PFMT"[%s] name failed! rc=%d bsz=%u",id,name,rc,bsz);
	return whefs_rc.IOError;
    }
    if( 0 && *name )
    {
	WHEFS_DBG("Writing inode #%"WHEFS_ID_TYPE_PFMT"[%s] name: [%s]",id,name,dbgStr);
    }
    return whefs_rc.OK;

}


/**
   Writes the inode names table to pos fs->offsets[WHEFS_OFF_INODE_NAMES].
   Returns whefs_rc.OK on success.
*/
static int whefs_mkfs_write_names_table( whefs_fs * restrict fs )
{
    whefs_id_type i = 1;
    int rc = whefs_rc.OK;
    whefs_fs_seek( fs, fs->offsets[WHEFS_OFF_INODE_NAMES], SEEK_SET );
    assert( fs->dev->api->tell( fs->dev ) == fs->offsets[WHEFS_OFF_INODE_NAMES] );
    char empty[1] = {'\0'};
    for( ; (i <= fs->options.inode_count) && (whefs_rc.OK == rc); ++i )
    {
	rc = whefs_fs_name_write( fs, i, empty );
    }
    if( whefs_rc.OK == rc )
    {
	// Unfortunate workaround for expectations of mkfs...
	whefs_fs_seek( fs, fs->offsets[WHEFS_OFF_INODES_NO_STR], SEEK_SET );
    }
    return rc;
}



whio_size_t whefs_fs_calculate_size( whefs_fs_options const * opt )
{
    static const whio_size_t sz = (whio_size_t)whio_sizeof_encoded_uint32;
    if( ! opt ) return 0;
    else return (whio_size_t)(
	(whio_sizeof_encoded_uint32 * whefs_fs_magic_bytes_len) // core magic
	+ sz // file size header
	+ whio_sizeof_encoded_uint16 // client magic size
	+ opt->magic.length
	+ whefs_fs_sizeof_options()
	+ (whefs_fs_sizeof_name( opt ) * opt->inode_count)/* inode names table */
	+ (whefs_sizeof_encoded_inode * opt->inode_count) /* inode table */
	+ (whefs_fs_sizeof_block( opt ) * opt->block_count)/* blocks table */
	);
}


/**
   Writes all (empty) blocks of fs to pos
   fs->offsets[WHEFS_OFF_BLOCKS] of the data store.  Returns
   whefs_rc.OK on success.
*/
static int whefs_mkfs_write_blocklist( whefs_fs * restrict fs )
{
    whefs_id_type i = 0;
    int rc = whefs_rc.OK;
    whefs_fs_seek( fs, fs->offsets[WHEFS_OFF_BLOCKS], SEEK_SET );
    assert( fs->dev->api->tell( fs->dev ) == fs->offsets[WHEFS_OFF_BLOCKS] );
    whefs_block bl = whefs_block_init;
    for( i = 1; (i <= fs->options.block_count) && (whefs_rc.OK == rc); ++i )
    {
	bl.id = i;
	if( whefs_rc.OK != (rc = whefs_block_wipe( fs, &bl, true, true, false )) )
	{
	    WHEFS_DBG_ERR("Error %d while writing the block table!", rc);
	    break;
	}
    }
    return rc;
}

/**
   Writes all (empty) inodes to the current position of
   fs->dev. Returns whefs_rc.OK on success.
*/
static int whefs_mkfs_write_inodelist( whefs_fs * restrict fs )
{
    whefs_fs_seek( fs, fs->offsets[WHEFS_OFF_INODES_NO_STR], SEEK_SET );
    assert( fs->dev->api->tell( fs->dev ) == fs->offsets[WHEFS_OFF_INODES_NO_STR] );
    size_t i = 0;
    int rc = whefs_rc.OK;
    whefs_inode node = whefs_inode_init;
    enum { bufSize = whefs_sizeof_encoded_inode };
    unsigned char buf[bufSize];
    memset( buf, 0, bufSize );
    for( i = 1; (i <= fs->options.inode_count) && (whefs_rc.OK == rc); ++i )
    {
	node.id = i;
	rc = whefs_inode_encode( &node, buf );
	if( whefs_rc.OK != rc )
	{
	    WHEFS_DBG_ERR("Error #%d while encoding new-style inode #%"WHEFS_ID_TYPE_PFMT"!",
			  rc,i);
	    return rc;
	}
        whio_size_t check = whefs_fs_writeat( fs, whefs_inode_id_pos( fs, i ), buf, bufSize );
        if( check != bufSize )
        {
            rc = whefs_rc.IOError;
	    WHEFS_DBG_ERR("Error #%d while writing inode #%"WHEFS_ID_TYPE_PFMT"!",
			  rc, i);
            break;
        }
    }
    return rc;
}

/**
   Uses whio_dev_ioctl() to try to get a file descriptor number
   associated with fs->dev. If it succeeds we store the descriptor
   so we can later use it to implement file locking.
*/
static void whefs_fs_check_fileno( whefs_fs * restrict fs )
{
    if( !fs || !fs->dev ) return;
    char const * fname = 0;
    whio_dev_ioctl( fs->dev, whio_dev_ioctl_GENERAL_name, &fname );
    if( whio_rc.OK != whio_dev_ioctl( fs->dev, whio_dev_ioctl_FILE_fd, &fs->fileno ) )
    {
        //WHEFS_DBG("Backing store does not appear to be a FILE." );
        return;
    }
    //WHEFS_DBG_FYI("Backing store appears to be a FILE (named [%s]) with descriptor #%d.", fname, fs->fileno );
    //posix_fadvise( fs->fileno, 0L, 0L, POSIX_FADV_RANDOM );
}

/**
   Opens filename in read-write mode (if writeMode is true) or read-only mode
   (if writeMode is 0) and assigns fs->dev to that device. Returns the new
   whio_dev object, which is owned by fs. Any existing fs->dev object
   is destroyed. On error, 0 is returned.

   If createIt is true and writeMode is true and we cannot open the
   file in "r+" mode then it will be created using "w+" mode. createIt
   is ignored if !writeMode or if the file can be opened using "r+"
   mode.
*/
static whio_dev * whefs_open_FILE( char const * filename, whefs_fs * restrict fs,
                                   bool writeMode, bool createIt )
{
    if( ! filename || !fs ) return 0;
    if( fs->dev )
    {
	fs->dev->api->finalize( fs->dev );
	fs->dev = 0;
    }
    fs->dev = whio_dev_for_filename( filename, writeMode ? "r+b" : "rb" );
    if( writeMode && createIt && !fs->dev )
    { /* didn't exist (we assume), so try to create it */
	WHEFS_DBG_WARN("Opening [%s] with 'r+' failed. Trying 'w+'...", filename );
	fs->dev = whio_dev_for_filename( filename, "w+b" );
    }
    if( ! fs->dev ) return 0;
    whefs_fs_check_fileno( fs );
    int lk = whefs_fs_lock( fs, writeMode, 0, SEEK_SET, 0 );
    if( (whefs_rc.OK == lk) || (whefs_rc.UnsupportedError == lk) )
    {
        // okay
    }
    else
    {
        fs->dev->api->finalize( fs->dev );
        fs->dev = 0;
    }
    return fs->dev;
}

static int whefs_fs_init_bitset_inodes( whefs_fs * restrict fs )
{
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
    if( ! fs ) return whefs_rc.ArgError;
    const size_t nbits = 1; /* flags: used */
    const size_t nbc = fs->options.inode_count * nbits + 1; /* +1 b/c we use the ID (1...N) as bit address */
    //WHEFS_DBG("Initializing bitsets. Node count/bits/bytes=%u/%u/%u", fs->options.inode_count, nbc, nbytes  );
    if( 0 != whbits_init( &fs->bits.i, nbc, 0 ) ) return whefs_rc.AllocError;
#if 0
    WHEFS_DBG("Initialized inode bitset. Node count/bits/bytes=%u/%u/%u",
	      fs->options.inode_count, fs->bits.i.sz_bits, fs->bits.i.sz_bytes );
#endif
    WHEFS_ICACHE_SET_USED(fs,0); /* inode ID 0 is reserved for "not a node". */
    WHEFS_ICACHE_SET_USED(fs,1); /* inode ID 1 is reserved for the root node. */
#endif /* WHEFS_CONFIG_ENABLE_BITSET_CACHE */
    return whefs_rc.OK;
}

static int whefs_fs_init_bitset_blocks( whefs_fs * restrict fs )
{
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
    //WHEFS_DBG("Setting up bitsets for fs@0x%p", (void const *)fs );
    if( ! fs ) return whefs_rc.ArgError;
    const size_t bbits = 1; /* flag: used */
    const size_t bbc = fs->options.block_count * bbits + 1; /* +1 b/c we use the ID (1...N) as bit address */
    //WHEFS_DBG("Initializing bitsets. block count/bits/bytes=%u/%u/%u", fs->options.block_count, bbc, bbytes  );
    if( 0 != whbits_init( &fs->bits.b, bbc, 0 ) ) return whefs_rc.AllocError;
#if 0
    WHEFS_DBG("Initialized bitsets. block count/bits/bytes=%u/%u/%u",
	      fs->options.block_count, fs->bits.b.sz_bits, fs->bits.b.sz_bytes );
#endif
    WHEFS_ICACHE_SET_USED(fs,0); /* inode ID 0 is reserved for "not a block". */
#endif /* WHEFS_CONFIG_ENABLE_BITSET_CACHE */
    return whefs_rc.OK;
}

/**
   Initializes the internal bitset caches. It must not be called
   before fs has a device and a filesystem has been opened. If called
   more than once for the same fs, fs->bits.i and fs->bits.b will get
   reallocated to fit any changes in fs->options.inode_count and
   fs->options.block_count. Does not do any I/O but has to alloc
   memory.

   Returns whefs_rc.OK on success.
*/
static int whefs_fs_init_bitsets( whefs_fs * restrict fs )
{
    int rc = whefs_fs_init_bitset_inodes(fs);
    if( whefs_rc.OK == rc ) rc = whefs_fs_init_bitset_blocks(fs);
    return rc;
}

/**
   Initializes the internal inode cache, reading the state from
   storage.

   Returns whefs_rc.OK on success.
*/
static int whefs_fs_inode_cache_load( whefs_fs * restrict fs )
{
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
    if( ! fs || !fs->dev ) return whefs_rc.ArgError;
    //return 0;
    const whefs_id_type nc = whefs_fs_options_get(fs)->inode_count;
    whefs_id_type id = 2; // root node (id=1) is always considered used
    int rc = 0;
    uint32_t flags;
    //whefs_id_type count = 0;
    whefs_inode ino = whefs_inode_init;
    for( ; id <= nc; ++id )
    {
	//WHEFS_DBG("Trying to cache inode #%"WHEFS_ID_TYPE_PFMT"'s state.", id);
	flags = 0;
	rc = whefs_inode_id_read( fs, id, &ino );
	if( whefs_rc.OK != rc )
	{
	    WHEFS_DBG_ERR("Error #%d while reading inode #%"WHEFS_ID_TYPE_PFMT"!", rc, id);
	    return rc;
	}
	if( ino.flags & WHEFS_FLAG_Used )
	{
	    //++count;
	    WHEFS_ICACHE_SET_USED(fs,id);
	}
	else
	{
	    WHEFS_ICACHE_UNSET_USED(fs,id);
	}
    }
    fs->bits.i_loaded = true;
    //WHEFS_DBG_FYI("Initialized inode cache (entries: %u)", count);
#endif /* WHEFS_CONFIG_ENABLE_BITSET_CACHE */
    return whefs_rc.OK;
}

/**
   Loads the block cache. Returns whefs_rc.OK on success,
*/
static int whefs_fs_block_cache_load( whefs_fs * restrict fs )
{
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
    /* FIXME: instead of using whefs_block_read(), simply extract the
       flags field from each block entry. That'll be much faster. */
    if( ! fs || !fs->dev ) return whefs_rc.ArgError;
    //return 0;
    const size_t bc = whefs_fs_options_get(fs)->block_count;
    size_t id = 1;
    int rc = 0;
    whefs_block bl;
    for( ; id <= bc; ++id )
    {
	bl = whefs_block_init;
	rc = whefs_block_read( fs, id, &bl ); /* this will update the used-blocks cache */
	if( whefs_rc.OK != rc )
	{
	    WHEFS_DBG_ERR("Error #%d while reading block #%u!", rc, id);
	    return rc;
	}
	//WHEFS_DBG("block cache. #%u is %s",bl.id, (WHEFS_BCACHE_IS_USED(fs,id) ? "used" : "unused") );
    }
    //WHEFS_DBG("Initialized block cache.");
    fs->bits.b_loaded = true;
#endif /* WHEFS_CONFIG_ENABLE_BITSET_CACHE */
    return whefs_rc.OK;
}

bool whefs_fs_is_rw( whefs_fs const * restrict fs )
{
    return fs && (fs->flags & WHEFS_FLAG_Write);
}
/**
   Loads the inode and block caches. Returns whefs_rc.OK on success,
*/
#if WHEFS_LOAD_CACHES_ON_OPEN
static
#endif
int whefs_fs_caches_load( whefs_fs * restrict fs )
{
    int rc = whefs_fs_inode_cache_load( fs );
    if( whefs_rc.OK == rc ) rc = whefs_fs_block_cache_load( fs );
    if( whefs_rc.OK == rc )
    {
        rc = whefs_fs_setopt_hash_cache( fs, WHEFS_CONFIG_ENABLE_STRINGS_HASH_CACHE ? true : false, false );
    }
    return rc;
}

/**
   Initializes fs->sizes[] and fs->offsets[]. fs->options must have
   been populated in order for this to work.
*/
static void whefs_fs_init_sizes( whefs_fs * restrict fs )
{
    fs->sizes[WHEFS_SZ_INODE_NO_STR] = whefs_sizeof_encoded_inode;
    fs->sizes[WHEFS_SZ_INODE_NAME] = whefs_fs_sizeof_name( &fs->options );
    fs->sizes[WHEFS_SZ_BLOCK] = whefs_fs_sizeof_block( &fs->options );
    fs->sizes[WHEFS_SZ_OPTIONS] = whefs_fs_sizeof_options();

    fs->offsets[WHEFS_OFF_CORE_MAGIC] = 0;

    size_t sz =	/* core magic len */
	(whio_sizeof_encoded_uint32 * whefs_fs_magic_bytes_len);

    fs->offsets[WHEFS_OFF_SIZE] =
	fs->offsets[WHEFS_OFF_CORE_MAGIC]
	+ sz;
    sz = /* file size */
	whio_sizeof_encoded_uint32;

    fs->offsets[WHEFS_OFF_CLIENT_MAGIC] =
	fs->offsets[WHEFS_OFF_SIZE]
	+ sz;
    sz = /* client magic size */
	whio_sizeof_encoded_uint16 // length
	+ fs->options.magic.length;

    fs->offsets[WHEFS_OFF_OPTIONS] = 
	fs->offsets[WHEFS_OFF_CLIENT_MAGIC]
	+ sz;

    fs->offsets[WHEFS_OFF_INODE_NAMES] =
	fs->offsets[WHEFS_OFF_OPTIONS]
	+ fs->sizes[WHEFS_SZ_OPTIONS];
    sz = /* names table size */
	(fs->options.inode_count * fs->sizes[WHEFS_SZ_INODE_NAME]);

    fs->offsets[WHEFS_OFF_INODES_NO_STR] =
	fs->offsets[WHEFS_OFF_INODE_NAMES]
	+ sz;
    sz = /* new inodes table size */
	(fs->options.inode_count * fs->sizes[WHEFS_SZ_INODE_NO_STR]);

    //fs->offsets[WHEFS_OFF_BLOCK_TABLE] NYI

    fs->offsets[WHEFS_OFF_BLOCKS] =
	fs->offsets[WHEFS_OFF_INODES_NO_STR]
	+ sz;
    sz = /* blocks table size */
	(fs->options.block_count * fs->sizes[WHEFS_SZ_BLOCK]);

    fs->offsets[WHEFS_OFF_EOF] =
	fs->offsets[WHEFS_OFF_BLOCKS]
	+ sz;

#if 0
    fprintf( stdout, "\tOffsets:\n");
#define OFF(X) fprintf(stdout,"\t\tfs->offsets[%s]\t= %u\n",# X, fs->offsets[WHEFS_OFF_ ## X])
    OFF(CORE_MAGIC);
    OFF(SIZE);
    OFF(CLIENT_MAGIC);
    OFF(OPTIONS);
    OFF(INODE_NAMES);
    OFF(INODES);
    OFF(INODES_NO_STR);
    OFF(BLOCKS);
    OFF(EOF);
#undef OFF
#define OFF(X) fprintf(stdout,"\t\tfs->sizes[%s]\t= %u\n",# X, fs->sizes[WHEFS_SZ_ ## X])
    OFF(INODE);
    OFF(INODE_NAME);
    OFF(BLOCK);
    OFF(OPTIONS);
    fflush(stdout);
    //assert(0 && "on purpose");
#undef OFF
#endif
}


/**
   Sets up a new, whefs_fs object an initializes the parts which depend only
   on the options. On success tgt is assigned to the new device and whefs_rc.OK
   is returned. On error, tgt is not modified and some other value is returned.
*/
static int whefs_mkfs_stage1( whefs_fs_options const * opt, whefs_fs ** tgt )
{
    if( !opt || !tgt ) return whefs_rc.ArgError;
    if( (opt->inode_count < 2)
	|| (opt->block_size < 32)
	|| !opt->filename_length
	|| (opt->filename_length > WHEFS_MAX_FILENAME_LENGTH)
	|| !opt->magic.length
	|| !opt->magic.data
	|| (opt->block_count < opt->inode_count)
	)
    {
	return whefs_rc.RangeError;
    }
    whefs_fs * fs = whefs_fs_alloc();
    if( ! fs ) return whefs_rc.AllocError;
    *fs = whefs_fs_init;
    fs->flags |= WHEFS_FLAG_ReadWrite;
    fs->options = *opt;

    whefs_fs_init_sizes( fs );

    int rc = whefs_fs_init_bitsets( fs );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Init of bitsets failed with rc %d!", rc);
	whefs_fs_finalize( fs );
	return rc;
    }
    *tgt = fs;

    return whefs_rc.OK;
}

/**
   Moves the seek position to fs->offsets[WHEFS_OFF_SIZE] and
   writes fs->filesize to that location.
*/
static int whefs_fs_write_filesize( whefs_fs * restrict fs )
{
    fs->dev->api->seek( fs->dev, fs->offsets[WHEFS_OFF_SIZE], SEEK_SET );
    const whio_size_t ck = whio_dev_uint32_encode( fs->dev, fs->filesize );
    return ( whio_sizeof_encoded_uint32 == ck )
        ? whefs_rc.OK
        : whefs_rc.IOError;
}

/**
   Writes out the disk structures for mkfs. fs->dev must be valid.  On
   success whefs_rc.OK is returned. On error, fs is destroyed and some
   other value is returned.
*/
static int whefs_mkfs_stage2( whefs_fs * restrict fs )
{
    if( ! fs || !fs->dev ) return whefs_rc.ArgError;

    size_t szcheck = whefs_fs_calculate_size(&fs->options);
    //WHEFS_DBG("szcheck = %u", szcheck );

    int rc = fs->dev->api->truncate( fs->dev, szcheck );
    if( whio_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Could not truncate EFS container to %u bytes!", szcheck );
	whefs_fs_finalize( fs );
	return rc;
    }

    //size_t bogo = 0;
#define CHECKRC if( rc != whefs_rc.OK ) { whefs_fs_finalize(fs); return rc; } \
    /*bogo= whio_dev_size( fs->dev ); WHEFS_DBG("device size=[%u]", bogo );*/

    // The following orders are important, as fs->offsets[] is used to confirm
    // expected file positions.
    rc = whefs_mkfs_write_magic( fs );
    CHECKRC;
    rc = whefs_mkfs_write_options( fs );
    CHECKRC;
    rc = whefs_mkfs_write_names_table( fs );
    CHECKRC;
    rc = whefs_mkfs_write_inodelist( fs );
    CHECKRC;
    rc = whefs_mkfs_write_blocklist( fs );
    CHECKRC;
#undef CHECKRC
    whefs_fs_flush(fs);
    fs->filesize = whio_dev_size( fs->dev );
    //WHEFS_DBG("File size is(?) %u", fs->filesize );

    szcheck = whefs_fs_calculate_size(&fs->options);
    if( szcheck != fs->filesize )
    {
	WHEFS_DBG_ERR("EFS size error: the calculated size (%u) does not match the real size (%u)!", szcheck, fs->filesize );
	whefs_fs_finalize( fs );
	return whefs_rc.ConsistencyError;
    }
    fs->offsets[WHEFS_OFF_EOF] = fs->dev->api->tell( fs->dev );
    whefs_fs_write_filesize( fs );

    whefs_fs_flush( fs );
    return whefs_rc.OK;
}

int whefs_mkfs( char const * filename, whefs_fs_options const * opt, whefs_fs ** tgt )
{
    if( ! filename || !opt || !tgt ) return whefs_rc.ArgError;
    whefs_fs * fs = 0;
    int rc = whefs_mkfs_stage1( opt, &fs );
    if( whefs_rc.OK != rc ) return rc;

    if( 0 == strcmp(":memory:",filename) )
    {
	if( ! (fs->dev = whio_dev_for_membuf( whefs_fs_calculate_size(opt), 0 )) )
	{
	    whefs_fs_finalize(fs);
	    return whefs_rc.AllocError; /* we're guessing here. */
	}
    }
    else if( ! whefs_open_FILE( filename, fs, true, true ) )
    {
	whefs_fs_finalize(fs);
	return whefs_rc.AccessError;
    }
    if( ! fs->dev )
    {
	whefs_fs_finalize(fs);
	return whefs_rc.InternalError;
    }
    rc = whefs_mkfs_stage2( fs );
    if( whefs_rc.OK != rc )
    {
	/* fs is already destroyed */
	fs = 0;
    }
    else
    {
        whefs_fs_mmap_connect( fs );
	*tgt = fs;
    }
    return rc;
}

int whefs_mkfs_dev( whio_dev * dev, whefs_fs_options const * opt, whefs_fs ** tgt, bool takeDev )
{
    if( ! dev || !opt || !tgt ) return whefs_rc.ArgError;
    whefs_fs * fs = 0;
    int rc = whefs_mkfs_stage1( opt, &fs );
    if( whefs_rc.OK != rc ) return rc;
    fs->ownsDev = false;
    fs->dev = dev;
    rc = whefs_mkfs_stage2( fs );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("mkfs stage 2 failed with rc %d!",rc);
	/* fs was already destroyed */
	fs = 0;
    }
#if 0
    // FIXME: we don't yet have the in-use cache for devices opened using mkfs!
    /* weird. When i do this here i get read() errors in the file-based i/o handler,
       with read() reporting that errno==EBADF (bad file descriptor)!!!

       Maybe(???) it's because of how we're opening the file (with
       "w+"). The workaround may be to close the EFS and
       re-whefs_fsopen_dev() it???
    */
    rc = whefs_fs_caches_load( fs );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Could not initialize EFS caches! Error code=%d.",rc);
	whefs_fs_finalize( fs );
	fs = 0;
    }
#endif
    if( fs )
    {
	fs->ownsDev = takeDev;
        whefs_fs_mmap_connect( fs );
	*tgt = fs;
    }
    return rc;
}

/**
   Performs the final stage of opening a vfs. fs must have been
   initialized and fs->dev must have been set up.

   On error, fs will be destroyed and non-whefs_rc.OK will be
   returned.

   fs will be modified quite significantly here, but this stuff must
   be called before the vfs can be used.
*/
static int whefs_openfs_stage2( whefs_fs * restrict fs )
{
    if( ! fs ) return whefs_rc.ArgError;

    fs->offsets[WHEFS_OFF_CORE_MAGIC] = 0;
    fs->offsets[WHEFS_OFF_SIZE] = (whefs_fs_magic_bytes_len * whio_sizeof_encoded_uint32);
    int rc = whefs_rc.OK;
    uint32_t coreMagic[whefs_fs_magic_bytes_len];

    while(1)
    {
	if( whefs_fs_magic_bytes_len != whio_dev_uint32_array_decode( fs->dev, whefs_fs_magic_bytes_len, coreMagic ) )
	{
	    WHEFS_DBG_ERR("Error reading the core magic bytes.");
	    rc = whefs_rc.BadMagicError;
	    break;
	}
	//WHEFS_DBG("Core magic = %04u %02u %02u %02u", coreMagic[0], coreMagic[1], coreMagic[2], coreMagic[3] );
	size_t i = 0;
	for( ; i < whefs_fs_magic_bytes_len; ++i )
	{
	    if( coreMagic[i] != whefs_fs_magic_bytes[i] )
	    {
		WHEFS_DBG_ERR("Core magic byte #%u does not match the expected value of 0x%04x (%u).",
			      i, whefs_fs_magic_bytes[i], whefs_fs_magic_bytes[i] );
		rc = whefs_rc.BadMagicError;
		break;
	    }
	}
	break;
    }
    if( rc != whefs_rc.OK )
    {
	whefs_fs_finalize(fs);
	return rc;
    }

    uint32_t fsize = 0;
    rc = whio_dev_uint32_decode( fs->dev, &fsize );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Doesn't seem to be a whefs file! error code=%d",rc);
	whefs_fs_finalize( fs );
	return whefs_rc.BadMagicError;
    }
    fs->filesize = fsize;
    fs->offsets[WHEFS_OFF_CLIENT_MAGIC] = fs->dev->api->tell(fs->dev);
    size_t aSize = whio_dev_size( fs->dev );
    if( !fsize || !aSize || (aSize != fsize) )
    { /* reminder: (aSize > fsize) must be allowed for static memory buffers to be usable as i/o devices. */
	WHEFS_DBG_ERR("File sizes don't agree: expected %u but got %u", fsize, aSize );
	whefs_fs_finalize( fs );
	return whefs_rc.ConsistencyError;
    }
    fs->offsets[WHEFS_OFF_EOF] = fsize;

#define CHECK if( whefs_rc.OK != rc ) { whefs_fs_finalize(fs); WHEFS_DBG_WARN("Decode of vfs options failed!"); return rc; }
    whefs_fs_options * opt = &fs->options;
    *opt = whefs_fs_options_nil;
    /* Read FS options... */
    // FIXME: factor this out into whefs_options_read():
    fs->dev->api->seek( fs->dev, fs->offsets[WHEFS_OFF_CLIENT_MAGIC], SEEK_SET );
    rc = whio_dev_uint16_decode( fs->dev, &opt->magic.length );
    CHECK;
    ///fs->offsets[WHEFS_OFF_OPTIONS] = 
    fs->dev->api->seek( fs->dev, opt->magic.length, SEEK_CUR );
    /* FIXME: store the opt->magic.data somewhere! Ownership requires some changes in other code. */
    // FIXME: add whio_dev_size_t_en/decode()
    rc = whio_dev_uint32_decode( fs->dev, &opt->block_size );
    CHECK;
    rc = whefs_dev_id_decode( fs->dev, &opt->block_count );
    CHECK;
    rc = whefs_dev_id_decode( fs->dev, &opt->inode_count );
    CHECK;
    rc = whio_dev_uint16_decode( fs->dev, &opt->filename_length );
    CHECK;
#undef CHECK
    whefs_fs_init_sizes( fs );

    rc = whefs_fs_init_bitsets( fs );
    if( whefs_rc.OK != rc )
    {
	/* we could treat this as non-fatal we we changed how
	   cache-is-on checking is done elsewhere. */
	WHEFS_DBG_ERR("Init of bitsets failed with rc %d!", rc);
	whefs_fs_finalize( fs );
	return rc;
    }
#if WHEFS_LOAD_CACHES_ON_OPEN
    rc = whefs_fs_caches_load( fs );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Error #%d initializing inode/block caches!", rc);
	whefs_fs_finalize( fs );
	return rc;
    }
#endif
    return whefs_rc.OK;
}

int whefs_openfs_dev( whio_dev * restrict dev, whefs_fs ** tgt, bool takeDev )
{
    if( ! dev || !tgt ) return whefs_rc.ArgError;
    whefs_fs * fs = whefs_fs_alloc();
    if( ! fs ) return whefs_rc.AllocError;
    *fs = whefs_fs_init;
    // FIXME: do a 1-byte write test to see if the device is writeable,
    // or add a parameter to the function defining the write mode.
    fs->flags |= WHEFS_FLAG_ReadWrite; /* we're guessing!!! */
    fs->dev = dev;
    fs->ownsDev = takeDev;
    whefs_fs_check_fileno( fs );
    int rc = whefs_openfs_stage2( fs );
    if( whefs_rc.OK == rc )
    {
        whefs_fs_mmap_connect( fs );
	*tgt = fs;
    }
    return rc;
}

int whefs_openfs( char const * filename, whefs_fs ** tgt, bool writeMode )
{
    // FIXME: refactor this to take a whio_dev and split it into
    // two or three parts.
    if( ! filename || !tgt ) return whefs_rc.ArgError;
    whefs_fs * fs = whefs_fs_alloc();
    if( ! fs ) return whefs_rc.AllocError;
    *fs = whefs_fs_init;
    fs->flags |= (writeMode ? WHEFS_FLAG_ReadWrite : WHEFS_FLAG_Read);
    if( ! whefs_open_FILE( filename, fs, writeMode, false ) )
    {
	WHEFS_DBG_WARN("Could not open file [%s] in %s mode.",
		       filename, writeMode ? "read/write" : "read-only" );
	whefs_fs_free(fs);
	return whefs_rc.IOError;
    }
    int rc = whefs_openfs_stage2( fs );
    if( whefs_rc.OK == rc )
    {
        whefs_fs_mmap_connect( fs );
	*tgt = fs;
    }
    return rc;
}

void whefs_fs_dump_info( whefs_fs const * restrict fs, FILE * out )
{
    struct SizeOfs
    {
	char const * label;
	size_t size;
    } sizeofs[] = {
#define X(T) {"sizeof(" # T ")",sizeof(T)}
    X(whio_dev),
    X(whio_dev_api),
    X(FILE),
    X(whefs_fs),
    X(whefs_file),
#if 1
    X(whio_stream),
    X(whio_stream_api),
#endif
    X(whbits),
    X(whefs_string),
    X(whefs_inode),
    X(whefs_block),
    X(whefs_hashid),
    X(whefs_hashid_list),
    X(whefs_fs_closer_list),
    {0,0}
    };
#undef X

    fprintf( out,"%s:%d:%s():\n", __FILE__, __LINE__, __func__);

    struct SizeOfs * so = sizeofs;
    fprintf( out, "sizeof() of various internal types:\n");
    for( ;so && so->label; ++so )
    {
	fprintf( out,"\t%s = %"PRIu64"\n", so->label, (uint64_t)so->size );
    }



    fprintf( out,"Various EFS stats:\n" );
    whefs_fs_options const * o = &fs->options;
    fprintf( out,
	     "\ton-disk sizeof whefs_inode = %u (+%"PRIu64" bytes for the name)\n"
	     "\tbits used for node/block IDs: %d\n"
	     "\tblock size: %"WHIO_SIZE_T_PFMT"\n"
	     "\tblock count: %"WHEFS_ID_TYPE_PFMT"\n"
	     "\tmax inode count: %"WHEFS_ID_TYPE_PFMT" (1 is reserved for the root dir entry!)\n"
	     "\tmax filename length: %u (WHEFS_MAX_FILENAME_LENGTH=%u)\n"
	     "\tmagic cookie length: %"PRIu32"\n"
	     "\tContainer size:\n\t\tcalculated =\t\t%"WHIO_SIZE_T_PFMT"\n\t\tdevice-reported =\t%"WHIO_SIZE_T_PFMT"\n",
	     whefs_sizeof_encoded_inode, (uint64_t)whefs_fs_sizeof_name(&fs->options),
	     (int) WHEFS_ID_TYPE_BITS,
	     o->block_size,
	     o->block_count,
	     o->inode_count,
	     o->filename_length, WHEFS_MAX_FILENAME_LENGTH,
	     (uint32_t)o->magic.length,
	     whefs_fs_calculate_size(&fs->options),
	     whio_dev_size(fs->dev)
	     );
#if 1
    fprintf( out, "\tEFS internal table offsets:\n");
#define OFF(X) fprintf(out,"\t\t%s\t= %u\n",# X, fs->offsets[WHEFS_OFF_ ## X])
    OFF(CORE_MAGIC);
    OFF(SIZE);
    OFF(CLIENT_MAGIC);
    OFF(OPTIONS);
    OFF(INODE_NAMES);
    OFF(INODES_NO_STR);
    OFF(BLOCKS);
    OFF(EOF);
#undef OFF
#endif

}

int whefs_fs_append_blocks( whefs_fs * restrict fs, whefs_id_type count )
{
    if( !count || !fs || !fs->dev ) return whefs_rc.ArgError;
    if( !whefs_fs_is_rw(fs) )
    {
        return whefs_rc.AccessError;
    }
    /**
       Having an active mmap() makes it impossible to truncate an open
       file because our simplified mmap() proxy device doesn't have a
       proper truncate() implementation. So we remove the proxy (if any)
       before truncating, then reconnect it (if the device supports it)
       afterwards.
    */
    whefs_fs_mmap_disconnect(fs);
    whefs_fs_options * opt = &fs->options;
    const whefs_id_type oldCount = opt->block_count;
    const size_t oldEOF = fs->offsets[WHEFS_OFF_EOF];
    const size_t newEOF = oldEOF + (whefs_fs_sizeof_block(opt) * count);
    int rc = fs->dev->api->truncate( fs->dev, newEOF );
    //WHEFS_DBG("Adding %"WHEFS_ID_TYPE_PFMT" blocks to fs (current count=%"WHEFS_ID_TYPE_PFMT").",count,oldCount);
    if( whio_rc.OK != rc )
    {
        WHEFS_DBG_ERR("Could not truncate fs to %u bytes to add %"WHEFS_ID_TYPE_PFMT" blocks!",newEOF, count);
        fs->dev->api->truncate( fs->dev, oldEOF ); // just to be sure
        whefs_fs_mmap_connect(fs);
        return rc;
    }
    fs->offsets[WHEFS_OFF_EOF] = newEOF;
    fs->filesize = newEOF;
    opt->block_count += count;
    // FIXME: error handling!
    // If anything goes wrong here, the EFS *will* be corrupted.
    whefs_fs_write_filesize( fs );
    whefs_mkfs_write_options( fs );
    whefs_fs_init_bitset_blocks( fs ); // will re-alloc the bitset cache.
    whefs_block bl = whefs_block_init;
    whefs_id_type id;
    for( id = (oldCount+1); id <= opt->block_count; ++id )
    {
        //WHEFS_DBG("Adding block #%"WHEFS_ID_TYPE_PFMT,id);
        bl.id = id;
        rc = whefs_block_wipe( fs, &bl, true, true, false );
        if( whefs_rc.OK != rc ) break;
    }
    whefs_fs_flush( fs );
    whefs_fs_mmap_connect( fs ); // We need to re-mmap() to account for the new size!
    return rc;
}

int whefs_fs_setopt_hash_cache( whefs_fs * fs, bool on, bool loadNow )
{
    if( ! fs ) return whefs_rc.ArgError;
    int rc = whefs_rc.OK;
    if( on )
    {
        // Reminder: we don't check if it was already on so we
        // can more easily honor loadNow.
        fs->flags |= WHEFS_FLAG_FS_EnableHashCache;
        if( loadNow )
        {
            rc = whefs_inode_hash_cache_load( fs );
        }
    }
    else
    {
        fs->flags &= ~WHEFS_FLAG_FS_EnableHashCache;
        whefs_hashid_list_free( fs->cache.hashes );
        fs->cache.hashes = 0;
    }
    if( whefs_rc.OK != rc )
    {
        whefs_hashid_list_free( fs->cache.hashes );
        if( on ) fs->flags &= ~WHEFS_FLAG_FS_EnableHashCache;
    }
    return rc;
}


#ifdef __cplusplus
} /* extern "C"*/
#endif
/* end file src/whefs_fs.c */
/* begin file src/whefs_block.c */
#line 8 "src/whefs_block.c"
#include <string.h> /* memset() */
// FIXME: there are lots of size_t's which should be replaced by whio_size_t
const whefs_block whefs_block_init = whefs_block_init_m;
const whefs_block_list whefs_block_list_init = whefs_block_list_init_m;

#if ! WHEFS_MACROIZE_SMALL_CHECKS
bool whefs_block_id_is_valid( whefs_fs const * fs, whefs_id_type blid )
{
    return whefs_block_id_is_valid_m(fs,blid);
}

bool whefs_block_is_valid( whefs_fs const * fs, whefs_block const * bl )
{
    return whefs_block_is_valid_m(fs,bl);
}
#endif //  WHEFS_MACROIZE_SMALL_CHECKS


whio_size_t whefs_block_id_pos( whefs_fs const * fs, whefs_id_type id )
{
    if( ! whefs_block_id_is_valid( fs, id ) )
    {
	return 0;
    }
    else
    {
	return fs->offsets[WHEFS_OFF_BLOCKS]
	    + ( (id-1) * fs->sizes[WHEFS_SZ_BLOCK] );
    }
}

void whefs_block_update_used( whefs_fs * fs, whefs_block const * bl )
{
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
    //if( ! whefs_block_id_is_valid( fs, bl ? bl->id : 0) ) return; // this is relatively costly here
    if( ! fs->bits.b_loaded ) return;
    if( bl->flags & WHEFS_FLAG_Used )
    {
	WHEFS_BCACHE_SET_USED(fs,bl->id);
    }
    else
    {
	WHEFS_BCACHE_UNSET_USED(fs,bl->id);
	if( fs->hints.unused_block_start > bl->id )
	{
	    fs->hints.unused_block_start = bl->id;
	}
    }
#endif /* WHEFS_CONFIG_ENABLE_BITSET_CACHE */
}

whio_size_t whefs_fs_sizeof_block( whefs_fs_options const * opt )
{
    return opt
	? (opt->block_size + whefs_sizeof_encoded_block)
	: 0;
}


static whio_size_t whefs_block_id_data_pos( whefs_fs const * fs, whefs_id_type id )
{
    whio_size_t rc = whefs_block_id_pos( fs, id );
    if( rc )
    {
	rc += whefs_sizeof_encoded_block;
    }
    return rc;
}

whio_size_t whefs_block_data_pos( whefs_fs const * fs, whefs_block const * bl )
{
    return bl ? whefs_block_id_data_pos( fs, bl->id ) : 0;
}

int whefs_block_id_seek( whefs_fs const * fs, whefs_id_type id )
{
    whio_size_t p = whefs_block_id_pos( fs, id );
    if( ! p ) return whefs_rc.ArgError;
    whio_size_t sk = fs->dev->api->seek( fs->dev, p, SEEK_SET );
    return (p == sk) ? whefs_rc.OK : whefs_rc.IOError;
}

int whefs_block_seek( whefs_fs const * fs, whefs_block const * bl )
{
    return bl ? whefs_block_id_seek( fs, bl->id ) : 0;
}

int whefs_block_id_seek_data( whefs_fs const * fs, whefs_id_type id, whio_size_t * tgt )
{
    whio_size_t p = whefs_block_id_data_pos( fs, id );
    if( ! p ) return whefs_rc.ArgError;
    whio_size_t sk = fs->dev->api->seek( fs->dev, p, SEEK_SET );
    return (p == sk) ? ((tgt?(*tgt=sk):0),whefs_rc.OK) : whefs_rc.IOError;
}

int whefs_block_seek_data( whefs_fs const * fs, whefs_block const * bl, whio_size_t * tgt )
{
    return bl ? whefs_block_id_seek_data( fs, bl->id, tgt ) : 0;
}


/**
   On-disk blocks are prefixed with this character.
*/
static const unsigned char whefs_block_tag_char = 0xdf /* sharp S, becase it looks like a B */;

int whefs_block_flush( whefs_fs * fs, whefs_block const * bl )
{
    //if( ! whefs_block_id_is_valid(fs,bl ? bl->id : 0) ) return whefs_rc.ArgError;
    if( ! whefs_fs_is_rw(fs) ) return whefs_rc.AccessError;
    int rc = whefs_rc.OK;

    rc = whefs_block_id_seek( fs, bl->id );
    if( whefs_rc.OK != rc )
    {
        WHEFS_DBG_ERR("FAILED setting correct write position for block #%"WHEFS_ID_TYPE_PFMT"! rc=%d", bl->id, rc );
        return rc;
    }

#if 0
    fs->dev->api->write( fs->dev, &whefs_block_tag_char, 1 );
    whio_size_t check = 0;
    //check = whio_dev_uint32_encode( fs->dev, EXP );
    //if( whio_dev_sizeof_uint32 != check ) return whefs_rc.IOError
    // TODO: encode this data to a buffer and do a single write():
    check = whefs_dev_id_encode( fs->dev, bl->id );
    if( whefs_sizeof_encoded_id_type != check ) return whefs_rc.IOError;
    check = whio_dev_uint32_encode( fs->dev, bl->flags );
    if( whio_dev_sizeof_uint32 != check ) return whefs_rc.IOError;
    check = whefs_dev_id_encode( fs->dev, bl->next_block );
    if( whefs_sizeof_encoded_id_type != check ) return whefs_rc.IOError;
#else
    unsigned char buf[whefs_sizeof_encoded_block];
    // TODO: use whefs_encode_xxx() here
    buf[0] = whefs_block_tag_char;
    whio_size_t off = 1;
    off += whefs_id_encode( buf + off, bl->id );
    off += whio_uint8_encode( buf + off, bl->flags );
    whefs_id_encode( buf + off, bl->next_block );
    const whio_size_t wsz = fs->dev->api->write( fs->dev, buf, whefs_sizeof_encoded_block );
    if( whefs_sizeof_encoded_block != wsz )
    {
        WHEFS_DBG_ERR("FAILED flushing block info to disk for block #%"WHEFS_ID_TYPE_PFMT". wsz=%"WHIO_SIZE_T_PFMT, bl->id, wsz );
        return whefs_rc.IOError;
    }
#endif
    whefs_block_update_used( fs, bl );
    //WHEFS_DBG("block_write for block #%u returning %d", bl->id, rc );
    return rc;
}

/**
   src must contain an encoded block whefs_sizeof_encoded_block bytes long.
   It is decoded and dest is populated with the results. If any code other
   than whefs_rc.OK is returned then dest is left in an undefined state.
*/
//static
int whefs_block_decode( whefs_block * dest, unsigned char const * src )
{

    if( ! dest || !src ) return whefs_rc.ArgError;
    unsigned const char * x = src;
    int rc = 0;
    if( whefs_block_tag_char != *(x++) )
    {
	return whefs_rc.ConsistencyError;
    }
#define RC if( rc != whefs_rc.OK ) return rc
    rc = whefs_id_decode( x, &dest->id );
    RC;
    x += whefs_sizeof_encoded_id_type;
    rc = whio_uint8_decode( x, &dest->flags );
    RC;
    x += whio_sizeof_encoded_uint8;
    rc = whefs_id_decode( x, &dest->next_block );
    RC;
#undef RC
    return rc;
}

int whefs_block_read( whefs_fs * fs, whefs_id_type bid, whefs_block * bl )
{
    if( ! whefs_block_id_is_valid(fs,bid)  )
    {
	if( bl )
	{ // WTF?
	    *bl = whefs_block_init;
	    return whefs_rc.OK;
	}
	return whefs_rc.ArgError;
    }
    whio_size_t to = whefs_block_id_pos( fs, bid );
    if( (whefs_rc.SizeTError == fs->dev->api->seek( fs->dev, to, SEEK_SET )) )
    {
        return whefs_rc.IOError;
    }
#if 0
    // FIXME: error handling!
    // FIXME: read the whole block in one go and decode it from the buffer.
    unsigned char check = 0;
    fs->dev->api->read( fs->dev, &check, 1 );
    if( whefs_block_tag_char != check )
    {
	WHEFS_DBG_ERR("Cursor is not positioned at a data block!");
	return whefs_rc.InternalError;
    }
    whefs_dev_id_decode( fs->dev, &bl->id );
    whio_dev_uint32_decode( fs->dev, &bl->flags );
    whefs_dev_id_decode( fs->dev, &bl->next_block );
#else
    // Read block metadata in one go and decode it from memory.
    unsigned char buf[whefs_sizeof_encoded_block];
    memset( buf, 0, whefs_sizeof_encoded_block );
    whio_size_t iorc = fs->dev->api->read( fs->dev, buf, whefs_sizeof_encoded_block );
    if( iorc != whefs_sizeof_encoded_block )
    {
	WHEFS_DBG_ERR("read() error while reading block #%"WHEFS_ID_TYPE_PFMT". "
                      "Expected %"WHIO_SIZE_T_PFMT" bytes but got %"WHIO_SIZE_T_PFMT"!",
		      bid, whefs_sizeof_encoded_block, iorc );
        return whefs_rc.IOError;
    }
    int rc = whefs_block_decode( bl, buf );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Error #%d while decoding block #%"WHEFS_ID_TYPE_PFMT"!",
		      rc, bid );
	return rc;
    }
#endif
    whefs_block_update_used( fs, bl );
    //WHEFS_DBG("Read block #%"WHEFS_ID_TYPE_PFMT". flags=0x%x",bl->id,bl->flags);
    return whefs_rc.OK;
}

int whefs_block_wipe_data( whefs_fs * fs, whefs_block const * bl, whio_size_t startPos )
{
    whio_size_t fpos = 0;
    const size_t bs = whefs_fs_options_get(fs)->block_size;
    if( startPos >= bs ) return whefs_rc.RangeError;
    int rc = whefs_block_id_seek_data( fs, bl->id, &fpos );
    if( whefs_rc.OK != rc ) return rc;
    if( (fpos + bs) < fpos /* overflow! */ ) return whefs_rc.RangeError;
    const size_t count = bs - startPos;
    {
	enum { bufSize = 1024 * 4 };
	static unsigned char buf[bufSize] = {'*',0};
	if( '*' == buf[0] )
	{
	    memset( buf+1, 0, bufSize-1 );
	    buf[0] = 0;
	}
	size_t wrc = 0;
	size_t total = 0;
	while( total < count )
	{
	    const size_t x = count - total;
            const size_t wsz = (bufSize > x) ? x : bufSize;
	    wrc = whefs_fs_write( fs, buf, wsz);
	    if( ! wrc ) break;
            if( wsz != wrc ) return whefs_rc.IOError;
	    total += wrc;
	}
	//WHEFS_DBG("Wrote %u bytes to zero block #%u. Range=[%u .. %u)", total, bl->id, fpos, fpos+count );
    }
    return whefs_rc.OK;
}

int whefs_block_wipe( whefs_fs * fs, whefs_block * bl,
		      bool wipeData,
		      bool wipeMeta,
		      bool deep )
{
    if( ! whefs_block_id_is_valid( fs, bl ? bl->id : 0 ) ) return whefs_rc.ArgError;
    size_t fpos = 0;
    const size_t bs = whefs_fs_options_get(fs)->block_size;
    if( (fpos + bs) < fpos /* overflow! */ ) return whefs_rc.RangeError;
    int rc = 0;
    if( deep && bl->next_block )
    {
	whefs_block next = *bl;
	whefs_block xb = *bl;
	bl->next_block = 0;
	while( xb.next_block )
	{
	    if( whefs_rc.OK != (rc = whefs_block_read_next( fs, &xb, &next )) )
	    {
		WHEFS_DBG_ERR("block #%"WHEFS_ID_TYPE_PFMT": could not load next block, #%"WHEFS_ID_TYPE_PFMT, xb.id, xb.next_block );
		return rc;
	    }
	    xb = next;
	    next.next_block = 0; /* avoid that the next call recurses deeply while still honoring 'deep'. */
	    if( whefs_rc.OK != (rc = whefs_block_wipe( fs, &next, wipeData, wipeMeta, deep )) )
	    {
		WHEFS_DBG_ERR("Error zeroing block #%"WHEFS_ID_TYPE_PFMT"! deep=%s", xb.id, xb.next_block, deep ? "true" : "false" );
		return rc;
	    }
	}
    }
    if( wipeMeta )
    {
	if( ! deep && bl->next_block )
	{
	    WHEFS_FIXME("Warning: we're cleaning up the metadata without cleaning up children! We're losing blocks!");
	}
	const whefs_id_type oid = bl->id;
	//WHEFS_DBG("Wiping block #%"WHEFS_ID_TYPE_PFMT". flags=0x%x",bl->id,bl->flags);
	*bl = whefs_block_init;
	bl->id = oid;
	rc = whefs_block_flush( fs, bl );
	if( whefs_rc.OK != rc )
	{
	    WHEFS_DBG_ERR("Wiping block #%"WHEFS_ID_TYPE_PFMT" failed: flush failed with error code #%d!\n", bl->id, rc);
	    return rc;
	}
	//WHEFS_DBG("Wiped block #%"WHEFS_ID_TYPE_PFMT". flags=0x%x",bl->id,bl->flags);
	if( oid < fs->hints.unused_block_start )
	{
	    fs->hints.unused_block_start = oid;
	}
    }
    if( wipeData )
    {
	rc = whefs_block_wipe_data( fs, bl, 0 );
	if( whefs_rc.OK != rc )
	{
	    WHEFS_DBG_ERR("Wiping block #%u failed with error code #%d!\n", bl->id, rc);
	    return rc;
	}
    }
    return whefs_rc.OK;
}

int whefs_block_read_next( whefs_fs * fs, whefs_block const * bl, whefs_block * nextBlock )
{
    if( !nextBlock || !whefs_block_id_is_valid(fs,bl?bl->id:0) ) return whefs_rc.ArgError;
    size_t nb = bl->next_block;
    /** don't reference bl after this, for the case that (bl == nextBlock) */
    if( ! nb ) return whefs_rc.RangeError;
    *nextBlock = whefs_block_init;
    return whefs_block_read( fs, nb, nextBlock );
}


int whefs_block_next_free( whefs_fs * restrict fs, whefs_block * restrict tgt, bool markUsed )
{
    if( ! fs || !tgt ) return whefs_rc.ArgError;
    whefs_id_type i = fs->hints.unused_block_start;
    if( ! i )
    {
	i = fs->hints.unused_block_start = 1;
    }
    whefs_block bl = whefs_block_init;
    for( ; i <= fs->options.block_count; ++i )
    {
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
        // TODO(?): i think we could skip 8 entries at a time as long as (0xFF & fs->bits.b.bytes[i*8])
	if( fs->bits.b_loaded && WHEFS_BCACHE_IS_USED(fs,i) )
	{
	    //WHEFS_DBG("Got cached block USED entry for block #%u", i );
	    continue;
	}
	//WHEFS_DBG("Cache says block #%i is unused. markUsed=%d", i, markUsed );
#endif
        // FIXME: try an flock here and skip to the next if we can't get a lock.
        // Use a write lock if markUsed is true???
	int rc = whefs_block_read( fs, i, &bl );
	//WHEFS_DBG("Checking block #%u for freeness. Read rc=%d",i,rc);
	if( whefs_rc.OK != rc )
	{
	    return rc;
	}
	if( bl.id != i )
	{
	    WHEFS_FIXME("Block id mismatch after successful whefs_block_read(). Expected %u but got %u.", i, bl.id );
	    assert( 0 && "block id mismatch after successful whefs_block_read()" );
	    return whefs_rc.InternalError;
	}
	if( WHEFS_FLAG_Used & bl.flags )
	{
	    whefs_block_update_used( fs, &bl );
	    continue;
	}
	if( markUsed )
	{
	    bl.flags = WHEFS_FLAG_Used;
	    whefs_block_flush( fs, &bl );
	    fs->hints.unused_block_start = bl.id + 1;
	    // FIXME: error handling!
	}
	*tgt = bl;
	//WHEFS_DBG( "Returning next free block: #%u",tgt->id );
	return whefs_rc.OK;
    }
    WHEFS_DBG_ERR("VFS appears to be full :(");
    return whefs_rc.FSFull;
}


#if 0 // unused code
int whefs_block_append( whefs_fs * fs, whefs_block const * bl, whefs_block * tgt )
{
    if( ! fs || !tgt ) return whefs_rc.ArgError;
    if( bl && bl->next_block ) return whefs_rc.ArgError;
    int rc = 0;
    const size_t oid = bl ? bl->id : 0;
    if( ! oid )
    { // WTF did i do this for?
	return whefs_block_next_free( fs, tgt, true );
    }
    whefs_block tail = *bl;// = whefs_block_init;
    while( tail.next_block )
    {
	rc = whefs_block_read_next( fs, &tail, &tail );
	if( whefs_rc.OK != rc ) return rc;
    }
    rc = whefs_block_next_free( fs, tgt, true );
    if( whefs_rc.OK != rc ) return rc;
    tail.next_block = tgt->id;
    whefs_block_flush( fs, &tail );
    whefs_block_flush( fs, tgt );
    return whefs_rc.OK;
}
#endif
/* end file src/whefs_block.c */
/* begin file src/whefs_inode.c */
#line 8 "src/whefs_inode.c"
/*
  Implementations for whefs_inode operations.

  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/
#include <string.h>
#include <stdlib.h> /* realloc() and friends. */


#include <time.h> /* gettimeofday() */
#include <sys/time.h>

const whefs_inode whefs_inode_init = whefs_inode_init_m;

const whefs_inode_list whefs_inode_list_init = whefs_inode_list_init_m;


/**
   If WHEFS_CONFIG_ENABLE_STATIC_MALLOC is true then we statically allocate
   whefs_inode_list_alloc_count whefs_inode_list objects to dole out via
   whefs_inode_list_alloc(), falling back to malloc() if the list is full.
*/
#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whefs_inode_list_alloc_slots object.
*/
whefs_inode_list_alloc_count = 10
};
static struct
{
    whefs_inode_list objs[whefs_inode_list_alloc_count];
    char used[whefs_inode_list_alloc_count];
    size_t next;
} whefs_inode_list_alloc_slots = { {whefs_inode_list_init_m}, {0}, 0 };
#endif

static whefs_inode_list * whefs_inode_list_alloc()
{
    whefs_inode_list * obj = 0;
#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = whefs_inode_list_alloc_slots.next;
    for( ; i < whefs_inode_list_alloc_count; ++i )
    {
	if( whefs_inode_list_alloc_slots.used[i] ) continue;
	whefs_inode_list_alloc_slots.next = i+1;
	whefs_inode_list_alloc_slots.used[i] = 1;
	obj = &whefs_inode_list_alloc_slots.objs[i];
	break;
    }
#endif /* WHEFS_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whefs_inode_list *) malloc( sizeof(whefs_inode_list) );
    if( obj ) *obj = whefs_inode_list_init;
    return obj;
}

static void whefs_inode_list_free( whefs_inode_list * obj )
{
    if( obj ) *obj = whefs_inode_list_init;
    else return;
#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whefs_inode_list_alloc_slots.objs[0]) ||
	(obj > &whefs_inode_list_alloc_slots.objs[whefs_inode_list_alloc_count-1]) )
    { /* it does not belong to us */
	free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whefs_inode_list_alloc_slots.objs[0]);
	whefs_inode_list_alloc_slots.used[ndx] = 0;
	if( ndx < whefs_inode_list_alloc_slots.next ) whefs_inode_list_alloc_slots.next = ndx;
	return;
    }
#else
    free(obj);
#endif /* WHEFS_CONFIG_ENABLE_STATIC_MALLOC */
}


/**
   This updates the internal used-inodes cache (if enabled) and inode
   search hints. ino must be a valid inode for fs.
*/
static void whefs_inode_update_used( whefs_fs * fs, whefs_inode const * ino )
{
    if( ! whefs_inode_is_valid( fs, ino) ) return;
    else if( ino->flags & WHEFS_FLAG_Used )
    {
	WHEFS_ICACHE_SET_USED(fs,ino->id);
    }
    else
    {
	WHEFS_ICACHE_UNSET_USED(fs,ino->id);
        if( fs->hints.unused_inode_start > ino->id )
        {
            fs->hints.unused_inode_start = ino->id;
        }
    }
}


int whefs_inode_name_set( whefs_fs * fs, whefs_id_type nid, char const * name )
{
    //if( !n || !n->id ) return whefs_rc.ArgError;
    if( !whefs_inode_id_is_valid(fs,nid) )
    {
        return whefs_rc.ArgError;
    }
    int rc = 0;

    /**
       We have to see if we have an existing entry for the given inode ID, so
       we can replace its hashvalue in the cache. If we don't do this we end
       up with stale/useless entries in the cache.
    */
    whefs_hashid * H = 0;
    char const * nameCheck = name;
    enum { bufSize = WHEFS_MAX_FILENAME_LENGTH + 1 };
    char buf[bufSize] = {0};
    whefs_string ncheck = whefs_string_init;
    ncheck.string = buf;
    ncheck.alloced = bufSize;
    if(1)
    {
        rc = whefs_inode_name_get( fs, nid, &ncheck );
        assert( (ncheck.string == buf) && "illegal (re)alloc!");
        if( whefs_rc.OK != rc ) return rc;
        if( *buf && (0==strcmp(buf,name))) return whefs_rc.OK;
        if( *buf ) nameCheck = ncheck.string;
    }
    WHEFS_DBG_CACHE("inode-name-set check for collision in [old=[%s]][new=[%s]][checkAgainst=[%s]].",ncheck.string,name,nameCheck);
    whefs_id_type ndx = whefs_inode_hash_cache_search_ndx( fs, nameCheck );
    if( ndx != whefs_rc.IDTypeEnd )
    {
        WHEFS_DBG_CACHE("inode-name-set found an existing entry for [%s].",nameCheck);
        H = &fs->cache.hashes->list[ndx];
        if( H->id != nid )
        {
            WHEFS_DBG_ERR("Internal error: cache hash collision for name [%s]!",name);
            return whefs_rc.InternalError;
        }
    }
    /**
       Maintenance reminders:

       We write to disk before updating any opened inode because
       writing is much more likely to fail then updating the opened
       inode is, since the latter operation is either just a string
       copy and possibly a relatively small malloc for the name
       strings cache. So we do the ops in order of likely failure, to
       reduce the possibility of, e.g. on-disk and in-memory inode
       names not matching.
    */
    rc = whefs_fs_name_write( fs, nid, name );
    if( whefs_rc.OK != rc ) return rc;
#if 1
    if( H )
    {
        H->hash = fs->cache.hashfunc(name);
        //fs->cache.hashes->isSorted = false;
        whefs_hashid_list_sort( fs->cache.hashes );
        WHEFS_DBG_CACHE("Replacing hashcode for file [%s].",name);
    }
#endif
    return rc;
}


#if ! WHEFS_MACROIZE_SMALL_CHECKS
bool whefs_inode_id_is_valid( whefs_fs const * restrict fs, whefs_id_type nid )
{
    return whefs_inode_id_is_valid_m(fs,nid);
}
#endif

#if ! WHEFS_MACROIZE_SMALL_CHECKS
bool whefs_inode_is_valid( whefs_fs const * restrict fs, whefs_inode const * n )
{
    return n ? whefs_inode_id_is_valid( fs, n->id ) : false;
}
#endif

whio_size_t whefs_inode_id_pos( whefs_fs const * restrict fs, whefs_id_type nid )
{
    if( ! whefs_inode_id_is_valid( fs, nid ) )
    {
	return 0;
    }
    else
    {
	return fs->offsets[WHEFS_OFF_INODES_NO_STR]
	    + ( (nid - 1) * fs->sizes[WHEFS_SZ_INODE_NO_STR] );
    }
}

int whefs_inode_id_seek( whefs_fs * fs, whefs_id_type id )
{
    whio_size_t p = whefs_inode_id_pos( fs, id );
    if( ! p ) return whefs_rc.ArgError;
    whio_size_t sk = whefs_fs_seek( fs, p, SEEK_SET );
    return (p == sk) ? whefs_rc.OK : whefs_rc.IOError;
}

int whefs_inode_seek( whefs_fs * fs, whefs_inode const * n )
{
    return n ? whefs_inode_id_seek( fs, n->id ) : 0;
}

/**
   On-disk inodes are prefixed with this character as a
   consistency-checking measure.
*/
static const unsigned char whefs_inode_tag_char = 0xef /* small i with diaeresis */;

int whefs_inode_flush( whefs_fs * fs, whefs_inode const * n )
{
    if( ! whefs_inode_is_valid( fs, n ) ) return whefs_rc.ArgError;
    if( ! whefs_fs_is_rw(fs) ) return whefs_rc.AccessError;
    if(0) WHEFS_DBG_FYI("Flushing inode #%"WHEFS_ID_TYPE_PFMT". inode->data_size=%u",
			n->id, n->data_size );
    whefs_inode_update_used( fs, n );
    //WHEFS_DBG("Writing node #%"WHEFS_ID_TYPE_PFMT" at offset %u", n->id, pos );
    enum { bufSize = whefs_sizeof_encoded_inode };
    unsigned char buf[bufSize];
    memset( buf, 0, bufSize );
    whefs_inode_encode( n, buf );
#if 0
    return whio_blockdev_write( &fs->fences.i, n->id - 1, buf );
#else
    int rc = whefs_inode_id_seek( fs, n->id );
    if( whefs_rc.OK != rc ) return rc;
    whio_size_t const wsz = whefs_fs_write( fs, buf, bufSize );
    return (wsz == bufSize) ? whefs_rc.OK : whefs_rc.IOError;
#endif
}

int whefs_inode_id_read( whefs_fs * fs, whefs_id_type nid, whefs_inode * tgt )
{
    if( !tgt || !whefs_inode_id_is_valid( fs, nid ) ) return whefs_rc.ArgError;
    int rc = whefs_rc.OK;
    enum { bufSize = whefs_sizeof_encoded_inode };
    unsigned char buf[bufSize];
    memset( buf, 0, bufSize );
#if 0
    rc = whio_blockdev_read( &fs->fences.i, nid - 1, buf );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Error #%d while reading inode #%"WHEFS_ID_TYPE_PFMT"!",
		      rc, nid );
	return rc;
    }
#else
    rc = whefs_inode_id_seek( fs, nid );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Error #%d while seeking to disk pos for inode #%"WHEFS_ID_TYPE_PFMT"!",
		      rc, nid );
	return rc;
    }
    whio_size_t const rsz = whefs_fs_read( fs, buf, bufSize );
    if( rsz != bufSize )
    {
	WHEFS_DBG_ERR("Error reading %u bytes for inode #%"WHEFS_ID_TYPE_PFMT". Only got %"WHIO_SIZE_T_PFMT" bytes!",
		      bufSize, nid, rsz );
	return rc;
    }
#endif
    rc = whefs_inode_decode( tgt, buf );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Error #%d while decoding inode #%"WHEFS_ID_TYPE_PFMT"!",
		      rc, nid );
	return rc;
    }
    whefs_inode_update_used( fs, tgt );
    return rc;
}

int whefs_inode_read( whefs_fs * fs, whefs_inode * n )
{
    return whefs_inode_id_read( fs, n ? n->id : 0, n );
}

int whefs_inode_read_flags( whefs_fs * fs, whefs_id_type nid, uint32_t * flags )
{
    if( ! whefs_inode_id_is_valid( fs, nid ) || !fs->dev ) return whefs_rc.ArgError;
    whefs_inode ino = whefs_inode_init;
    int rc = whefs_inode_id_read( fs, nid, &ino );
    if( whefs_rc.OK == rc )
    {
        if( flags ) *flags = ino.flags;
    }
    else
    {
	WHEFS_DBG_ERR("Error code #%d while reading inode #%"WHEFS_ID_TYPE_PFMT"!",
		      rc, nid );
        (void)0;
    }
    return rc;
}

int whefs_inode_foreach( whefs_fs * fs, whefs_inode_predicate_f where, void * whereData,
                         whefs_inode_foreach_f func, void * foreachData )
{
    if( ! fs || !func ) return whefs_rc.ArgError;
    whefs_id_type i = 2;// skip root inode
    whefs_inode n = whefs_inode_init;
    int rc = whefs_rc.OK;
    for( ; i <= fs->options.inode_count; ++i )
    {
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
	if( fs->bits.i_loaded && !WHEFS_ICACHE_IS_USED(fs,i) )
	{
	    continue;
	}
#endif
	rc = whefs_inode_id_read( fs, i, &n );
	if( whefs_rc.OK != rc ) return rc;
	if( n.id != i )
	{
	    assert( 0 && "node id mismatch after whefs_inode_id_read()" );
	    WHEFS_FIXME("Node id mismatch after successful whefs_inode_id_read(). Expected %"WHEFS_ID_TYPE_PFMT" but got %"WHEFS_ID_TYPE_PFMT".", i, n.id );
	    return whefs_rc.InternalError;
	}
        if( where && ! where( fs, &n, whereData ) ) continue;
        rc = func( fs, &n, foreachData );
        if( whefs_rc.OK != rc ) break;
    }
    return rc;
}
int whefs_inode_next_free( whefs_fs * restrict fs, whefs_inode * restrict tgt, bool markUsed )
{
    if( ! fs || !tgt ) return whefs_rc.ArgError;
    whefs_id_type i = fs->hints.unused_inode_start;
    if( i < 2 )
    {
	i = fs->hints.unused_inode_start = 2;
	/* we skip the root node, which is reserved at ID 1. */
    }
    whefs_inode n = whefs_inode_init;
    if(0) WHEFS_DBG("i=%"WHEFS_ID_TYPE_PFMT", fs->hints.unused_inode_start=%"WHEFS_ID_TYPE_PFMT
		    ", fs->options.inode_count=%"WHEFS_ID_TYPE_PFMT,
		    i, fs->hints.unused_inode_start, fs->options.inode_count );
    for( ; i <= fs->options.inode_count; ++i )
    {
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
	if( fs->bits.i_loaded && WHEFS_ICACHE_IS_USED(fs,i) )
	{
	    //WHEFS_DBG("Got cached inode USED entry for inode #%"WHEFS_ID_TYPE_PFMT"", i );
	    continue;
	}
	//WHEFS_DBG("Cache says inode #%i is unused.", i );
#endif
	int rc = whefs_inode_id_read( fs, i, &n );
	//WHEFS_DBG("Checking inode #%"WHEFS_ID_TYPE_PFMT" for freeness. Read rc=%d",i,rc);
	if( whefs_rc.OK != rc )
	{
	    return rc;
	}
	if( n.id != i )
	{
	    assert( 0 && "node id mismatch after whefs_inode_id_read()" );
	    WHEFS_FIXME("Node id mismatch after successful whefs_inode_id_read(). Expected %"WHEFS_ID_TYPE_PFMT" but got %"WHEFS_ID_TYPE_PFMT".", i, n.id );
	    return whefs_rc.InternalError;
	}
	if( WHEFS_FLAG_Used & n.flags )
	{
	    whefs_inode_update_used( fs, &n );
	    continue;
	}
	if( markUsed )
	{
	    n.flags |= WHEFS_FLAG_Used;
	    whefs_inode_update_mtime( fs, &n );
	    whefs_inode_flush( fs, &n );
	    fs->hints.unused_inode_start = n.id + 1;
	    // FIXME: error checking!
	}
	*tgt = n;
	//WHEFS_DBG( "Returning next free inode: %"WHEFS_ID_TYPE_PFMT"",tgt->id );
	return whefs_rc.OK;
    }
    WHEFS_DBG_ERR("VFS appears to be full :(");
    if(0) WHEFS_DBG("i=%"WHEFS_ID_TYPE_PFMT", n.id=%"WHEFS_ID_TYPE_PFMT", fs->hints.unused_inode_start=%"WHEFS_ID_TYPE_PFMT", fs->options.inode_count=%"WHEFS_ID_TYPE_PFMT"",
		    i, n.id, fs->hints.unused_inode_start, fs->options.inode_count );
    return whefs_rc.FSFull;
}

int whefs_inode_update_mtime( whefs_fs * fs, whefs_inode * n )
{
    if( ! n ) return whefs_rc.ArgError;
#if 0
    struct timeval tv;
    gettimeofday( &tv, 0 );
    n->mtime = (uint32_t)tv.tv_sec;
#else
    n->mtime = (uint32_t) time(NULL);
#endif
    return whefs_rc.OK;
}

int whefs_inode_search_opened( whefs_fs * fs, whefs_id_type nodeID, whefs_inode ** tgt )
{
    // FIXME: need to lock the fs here, or at least lock fs->opened_nodes.
    if( ! whefs_inode_id_is_valid(fs, nodeID) || !tgt ) return whefs_rc.ArgError;
    whefs_inode_list * li = fs->opened_nodes;
    for( ; li; li = li->next )
    {
	if( li->inode.id < nodeID ) continue;
	else if( li->inode.id > nodeID ) break;
	else
	{
	    //WHEFS_DBG("Found opened node #%"WHEFS_ID_TYPE_PFMT".", nodeID );
	    *tgt = &li->inode;
	    return whefs_rc.OK;
	}
    }
    return whefs_rc.RangeError;
}

int whefs_inode_open( whefs_fs * fs, whefs_id_type nodeID, whefs_inode ** tgt, void const * writer )
{
    /**
       Design note/reminder: the preference would have been to use an
       expanding array of whefs_inode for the opened nodes list, but
       when we realloc() it that could invalidate older pointers to
       those inodes (been there, done that). Thus we suffer a linked
       list and the associated mallocs...
    */
    if( ! whefs_inode_id_is_valid(fs, nodeID) || !tgt ) return whefs_rc.ArgError;
    //WHEFS_DBG_FYI( "Got request to open inode #%"WHEFS_ID_TYPE_PFMT". writer=@0x%p", nodeID, writer );
    whefs_inode * x = 0;
    int rc = whefs_inode_search_opened( fs, nodeID, &x );
    if( whefs_rc.OK == rc )
    { /* got an existing entry... */
	if(0) WHEFS_DBG_FYI( "Found existing entry for inode %"WHEFS_ID_TYPE_PFMT". entry->writer=@0x%p, writer param=@0x%p",
			     x->id, x->writer, writer );
	if( x->writer )
	{
	    if( x->writer == writer )
	    { /* no-op: opened twice by the same caller */
		WHEFS_DBG_WARN("inode #%"WHEFS_ID_TYPE_PFMT" was opened multiple times by the same writer. Ignoring sebsequent request.",
			       x->id );
		*tgt = x;
		return whefs_rc.OK;
	    }
#if 1
	    else if( writer )
	    {
		/* Only allow one writer for now. Maybe we'll fix this some day. */
		WHEFS_DBG_WARN("Only one writer is allowed on a file at a time, and inode #%"WHEFS_ID_TYPE_PFMT" is already opened for writing by the object at 0x%p.",
			       x->id, x->writer );
		return whefs_rc.AccessError;
	    }
#else
	    else
	    {
		WHEFS_DBG_WARN("inode #%"WHEFS_ID_TYPE_PFMT" is already opened in read/write mode by the object at 0x%p.",
			       x->id, x->writer );
		return whefs_rc.AccessError;
	    }
#endif
	}
	else
	{
	    x->writer = writer;
	}
	++x->open_count;
	*tgt = x;
	if(0) WHEFS_DBG_FYI("Re-using opened inode #%"WHEFS_ID_TYPE_PFMT". Open counter=%u, size=%u",
			    x->id, x->open_count, x->data_size );
	return  whefs_rc.OK;
    }
    whefs_inode_list * ent = whefs_inode_list_alloc();
    if( ! ent ) return whefs_rc.AllocError;
    *ent = whefs_inode_list_init;
    
    ent->inode.id = nodeID;
    rc = whefs_inode_id_read( fs, nodeID, &ent->inode );
    if( whefs_rc.OK != rc )
    {
        whefs_inode_list_free(ent);
	WHEFS_DBG_ERR("Opening inode #%"WHEFS_ID_TYPE_PFMT" FAILED - whefs_inode_id_read() returned %d", ent->inode.id, rc );
	return rc;
    }
    //WHEFS_DBG("Opened inode #%"WHEFS_ID_TYPE_PFMT" with name [%s]", ent->inode.id, ent->inode.name.string );
    x = &ent->inode;
    x->writer = writer;
    whefs_inode_list * li = fs->opened_nodes;
    if( ! li )
    { /* we have the distinction of being the first entry. */
	fs->opened_nodes = li = ent;
    }
    else
    { /* let's keep the list sorted here, as that can save us some comparisons later. */
	while( li->next && li && (li->inode.id < ent->inode.id) )
	{
	    li = li->next;
	}
	if( (li->inode.id < ent->inode.id) )
	{ /* insert on the right */
	    ent->prev = li;
	    ent->next = li->next;
	    li->next = ent;
	    if( ent->next ) ent->next->prev = ent;
	}
	else
	{ /* insert on the left */
	    ent->next = li;
	    ent->prev = li->prev;
	    li->prev = ent;
	    if( ent->prev ) ent->prev->next = ent;
	}
    }
    /* make sure we keep the list at the proper position... */
    while( fs->opened_nodes->prev ) fs->opened_nodes = fs->opened_nodes->prev;
    if(0) WHEFS_DBG_FYI("Newly opened inode #%"WHEFS_ID_TYPE_PFMT".", x->id, x->open_count );
    x->open_count = 1;
    *tgt = x;
    return whefs_rc.OK;
}

int whefs_inode_close( whefs_fs * fs, whefs_inode * src, void const * writer )
{
    if( ! whefs_inode_is_valid(fs, src) ) return whefs_rc.ArgError;
    if(0) WHEFS_DBG_FYI("Closing shared inode #%"WHEFS_ID_TYPE_PFMT": Use count=%u, data size=%u",
			src->id, src->open_count, src->data_size );
    whefs_inode * np = 0;
    whefs_inode_list * li = fs->opened_nodes;
    for( ; li; li = li->next )
    {
	if( li->inode.id < src->id ) continue;
	else if( li->inode.id > src->id) break;
	//if( li->inode.id != src->id ) continue;
	else
	{
	    if(0) WHEFS_DBG_FYI("Found opened node #%"WHEFS_ID_TYPE_PFMT". We'll close this one.", src->id );
	    np = &li->inode;
	    break;
	}
    }
    if( ! np )
    {
	WHEFS_DBG_ERR("Cannot close inode #%"WHEFS_ID_TYPE_PFMT" - it was not opened via whefs_inode_open() (or is somehow not in the list)!",
		      src->id );
	return whefs_rc.ArgError;
    }
    if( np != src )
    {
	WHEFS_DBG_ERR("Cannot close shared inode #%"WHEFS_ID_TYPE_PFMT": src inode @0x%p is not the same object as the shared inode @0x%p! "
		      "Read the API docs for whefs_inode_open() and whefs_inode_close() for details!",
		      src->id, (void const *)src, (void const *)np );
	return whefs_rc.InternalError;
    }
    if( np->writer && (np->writer == writer) )
    {
	np->writer = 0;
	whefs_inode_flush( fs, np );
    }
    --np->open_count;
    if( 0 == np->open_count )
    {
	if(0) WHEFS_DBG_FYI("REALLY closing inode #%"WHEFS_ID_TYPE_PFMT": Use count=%u, data size=%u",
			    src->id, src->open_count, src->data_size );
	if( li == fs->opened_nodes ) fs->opened_nodes = (li->next ? li->next : li->prev);
	if( li->prev ) li->prev->next = li->next;
	if( li->next ) li->next->prev = li->prev;
	if( np->blocks.list )
	{
	    free(np->blocks.list);
	}
	np->blocks = whefs_block_list_init;
	whefs_inode_list_free(li);
    }
    if(0) WHEFS_DBG_FYI("%p %p Closed shared inode #%"WHEFS_ID_TYPE_PFMT": Use count=%u, data size=%u",
			np, src, src->id, src->open_count, src->data_size );
    return whefs_rc.OK;
}


int whefs_inode_unlink( whefs_fs * fs, whefs_inode * ino )
{
    if( ! whefs_inode_is_valid(fs,ino) ) return whefs_rc.ArgError;
    while(1)
    {
        whefs_inode * op = 0;
	if( whefs_rc.OK != whefs_inode_search_opened( fs, ino->id, &op ) ) break;
	WHEFS_DBG_WARN("Cannot unlink inode #%"WHEFS_ID_TYPE_PFMT" because it has %u opened handle(s)!",
		       op->id, op->open_count );
	return whefs_rc.AccessError;
    }
    const whefs_id_type nid = ino->id;
    int rc = whefs_rc.OK;
    if( ino->first_block )
    {
	whefs_block bl = whefs_block_init;
	whefs_block_read( fs, ino->first_block, &bl );
 	rc = whefs_block_wipe( fs, &bl, true, true, true );
    }
    *ino = whefs_inode_init;
    ino->id = nid;
    return ( whefs_rc.OK != rc )
	? rc
	: whefs_inode_flush( fs, ino );
}

int whefs_inode_id_unlink( whefs_fs * fs, whefs_id_type nid )
{
    if( ! whefs_inode_id_is_valid(fs,nid) ) return whefs_rc.ArgError;
    whefs_inode ino = whefs_inode_init;
    int rc = whefs_inode_id_read( fs, nid, &ino );
    if( whefs_rc.OK == rc )
    {
	rc = whefs_inode_unlink( fs, &ino );
    }
    return rc;
}


int whefs_inode_by_name( whefs_fs * fs, char const * name, whefs_inode * tgt )
{
    if( ! fs || !name || !*name || !tgt ) return whefs_rc.ArgError;
    if( ! fs->options.inode_count ) return whefs_rc.RangeError;
    const size_t slen = strlen(name);
    if( slen > fs->options.filename_length )
    {
	return whefs_rc.RangeError;
    }
    whefs_string ns = whefs_string_init;
    int rc = whefs_rc.OK;
    bool expectExact = false; // when true, we stop with error if first guess isn't correct
    const whefs_hashval_type nameHash = fs->cache.hashfunc( name );
#if 1
    if( fs->cache.hashes && !fs->cache.hashes->isSorted )
    {
        whefs_inode_hash_cache_sort(fs);
    }
#endif
    char const * cname = 0; // cached name entry
    whefs_id_type i = whefs_hashid_list_index_of( fs->cache.hashes, nameHash );
    if( whefs_rc.IDTypeEnd == i )
    { // no cached record. Start from the beginning.
        i = 2; // 2 = first client-usable inode.
    }
    else
    { // we know directly what inode record to jump to now...
        expectExact = true;
        if(0) WHEFS_DBG_CACHE("Filename matched cached INDEX (%"WHEFS_ID_TYPE_PFMT") for hash code 0x%"WHEFS_HASHVAL_TYPE_PFMT" for name [%s]",i, nameHash,name);
        whefs_hashid * H = &fs->cache.hashes->list[i];
        ++H->hits;
        i = H->id;
    }

    enum { bufSize = WHEFS_MAX_FILENAME_LENGTH+1 };
    unsigned char buf[bufSize] = {0};
    memset(buf,0,bufSize);
    ns.string = (char *)buf;
    ns.alloced = bufSize;
    ns.length = 0;
    rc = whefs_rc.RangeError;
    for( ; i <= fs->options.inode_count; ++i )
    { // brute force... walk the inodes and compare them...
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE // we can't rely on this here.
        if( fs->bits.i_loaded )
        {
            if( ! WHEFS_ICACHE_IS_USED(fs,i) )
            {
                //WHEFS_DBG("Skipping unused inode entry #%"WHEFS_ID_TYPE_PFMT, i );
                if( expectExact )
                {
                    assert(0 && "If we have a cached entry for a specific bit then it must have been flagged as used.");
                    rc = whefs_rc.ConsistencyError;
                    break;
                }
                else continue;
            }
        }
        //WHEFS_DBG("Cache says inode #%i is used.", i );
#endif
        rc = whefs_inode_name_get( fs, i, &ns );
        assert( (ns.string == (char const *)buf) && "Internal consistency error!");
        if( whefs_rc.OK != rc )
        {
            WHEFS_DBG_ERR("whefs_string_read(fs,%"WHEFS_ID_TYPE_PFMT",&ns) returned error code %d. file name=[%s]", i, rc, name );
            break;
        }
        if( 0 && *ns.string ) WHEFS_DBG("Trying inode #%"WHEFS_ID_TYPE_PFMT": name [%s] =? [%s]", i, name, ns.string);
        if( !*ns.string ) continue;
        rc = strcmp( ns.string, name );
        if( 0 == rc )
        {
            cname = ns.string;
            break;
        }
        if( expectExact )
        { /* can't happen */
            assert( 0 && "If expectExact is true then cname must also be non-zero!");
            return whefs_rc.ConsistencyError;
        }
        memset(buf,0,ns.length);
    }
    if( whefs_rc.OK != rc )
    {
        return rc;
    }
    if( ! *buf && ! cname ) return whefs_rc.RangeError;
    assert( cname && "cname must be non-0 or we should have returned by now!" );
    do
    {
        whefs_inode n = whefs_inode_init;
        rc = whefs_inode_id_read( fs, i, &n );
        if( whefs_rc.OK != rc )
        {
            WHEFS_DBG_ERR("whefs_inode_id_read(fs,node[id=%"WHEFS_ID_TYPE_PFMT"]) returned error code %d. file name=[%s]", n.id, rc, name );
            break;
        }
        *tgt = n;
        rc =  whefs_rc.OK;
    } while(false);
    return rc;
}

int whefs_inode_encode( whefs_inode const * src, unsigned char * dest )
{
    if( ! dest || !src ) return whefs_rc.ArgError;
    unsigned char * x = dest;
    *(x++) = whefs_inode_tag_char;
    whefs_id_encode( x, src->id );
    x += whefs_sizeof_encoded_id_type;

    whio_uint8_encode( x, src->flags );
    x += whio_sizeof_encoded_uint8;

    whio_uint32_encode( x, src->mtime );
    x += whio_sizeof_encoded_uint32;

    whio_uint32_encode( x, src->data_size );
    x += whio_sizeof_encoded_uint32;

    whefs_id_encode( x, src->first_block );
    return whefs_rc.OK;
}

int whefs_inode_decode( whefs_inode * dest, unsigned char const * src )
{

    if( ! dest || !src ) return whefs_rc.ArgError;
    unsigned const char * x = src;
    int rc = 0;
    if( whefs_inode_tag_char != *(x++) )
    {
	return whefs_rc.ConsistencyError;
    }
#define RC if( rc != whefs_rc.OK ) return rc
    rc = whefs_id_decode( x, &dest->id );
    RC;
    x += whefs_sizeof_encoded_id_type;
    rc = whio_uint8_decode( x, &dest->flags );
    RC;
    x += whio_sizeof_encoded_uint8;
    rc = whio_uint32_decode( x, &dest->mtime );
    RC;
    x += whio_sizeof_encoded_uint32;
    rc = whio_uint32_decode( x,  &dest->data_size );
    RC;
#undef RC
    x += whio_sizeof_encoded_uint32;
    rc = whefs_id_decode( x, &dest->first_block );
    return rc;
}

/* end file src/whefs_inode.c */
/* begin file src/whefs_hash.c */
#line 8 "src/whefs_hash.c"
#include <stdlib.h> // qsort(), bsearch()
#include <string.h> // memmove()

#include <assert.h>

const whefs_hashid whefs_hashid_init = whefs_hashid_init_m;

whefs_hashval_type whefs_hash_cstring( char const * vstr)
{
#if 1
    /* "djb2" algo code taken from: http://www.cse.yorku.ca/~oz/hash.html */
    if( ! vstr ) return 0U;
    whefs_hashval_type hash = 5381;
    int c = 0;
    while( (c = *vstr++) )
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
#else
 /* "shift-and-xor" algo code taken from: http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx

*/
    whefs_hashval_type hash = 0;
    if(vstr) for( ; *vstr; ++vstr )
    {
        hash ^= ( hash << 5 ) + ( hash >> 2 ) + *vstr;
    }
   return hash;
#endif
}

/**
   A compare routine for bsearch(). Compares the hash fields of lhs
   and rhs based on their hash member. They must be (whefs_hashid
   const *)-compatible.
*/
static int whefs_hashid_cmp( void const * lhs, void const * rhs )
{
    whefs_hashid const * l  = (whefs_hashid const *)lhs;
    whefs_hashid const * r  = (whefs_hashid const *)rhs;
    if( l == r ) return 0;
    else if( !l && r ) return -1;
    else if( l && !r ) return 1;
    else if( l->hash < r->hash ) return -1;
    else if( l->hash > r->hash ) return 1;
    else return 0;    
}

/**
   A compare routine for bsearch(). Compares the hash fields of lhs
   and rhs based on their hits member. They must be (whefs_hashid
   const *)-compatible.
*/
//static
int whefs_hashid_cmp_hits( void const * lhs, void const * rhs )
{
    whefs_hashid const * l  = (whefs_hashid const *)lhs;
    whefs_hashid const * r  = (whefs_hashid const *)rhs;
    if( ! l->id )
    {
        if( r->id ) return -1;
        return 0;
    }
    return ( l->hits == r->hits )
        ? 0
        : ( ( l->hits < r->hits ) ? -1 : 1);
}

const whefs_hashid_list whefs_hashid_list_init = whefs_hashid_list_init_m;

int  whefs_hashid_list_sort( whefs_hashid_list * li )
{
    if( ! li ) return whefs_rc.ArgError;
    li->isSorted = true;
    if( li->count < 2 ) return whefs_rc.OK;
    qsort( li->list, li->count, sizeof(whefs_hashid), whefs_hashid_cmp );
    //qsort( li->_head, li->count, sizeof(whefs_hashid), whefs_hashid_cmp );
#if 1
    // shave off zeroed items...
    whefs_id_type off = 0;
    whefs_hashid * h = li->list;
    while( ! h->id && (off<li->count) ) { ++off; ++h; }
    // FIXME: simply move li->_head, so we can easily re-use those slots in whefs_hashid_list_add().
    if( (h != li->list) && (off < li->count) )
    {
        const whefs_id_type tail = li->alloced; /* li->count? */
        const size_t end = sizeof(whefs_hashid)*(tail - off);
        memmove( li->list, h, end );
        memset( li->list + (tail - off), 0, off );
        li->count -= off;
    }
#endif
    return whefs_rc.OK;
}

int whefs_hashid_list_alloc( whefs_hashid_list ** tgt, whefs_id_type toAlloc )
{
    if( ! tgt ) return whefs_rc.ArgError;
    if( ! *tgt )
    {
        if( ! toAlloc ) return whefs_rc.OK;
        *tgt = (whefs_hashid_list*) malloc(sizeof(whefs_hashid_list));
        if( ! tgt ) return whefs_rc.AllocError;
        **tgt = whefs_hashid_list_init;
    }
    whefs_hashid_list * obj = *tgt;
    if( 0 == toAlloc )
    {
	if( obj->alloced )
        {
            WHEFS_DBG_CACHE("Freeing whefs_hashid_list->list with %"WHEFS_ID_TYPE_PFMT"/%"WHEFS_ID_TYPE_PFMT" used/allocated items.",obj->count,obj->alloced);
            free( obj->list );
        }
        *obj = whefs_hashid_list_init;
        obj->isSorted = true;
	return whefs_rc.OK;
    }
    else if( obj->alloced >= toAlloc )
    {
        return whefs_rc.OK;
    }
    obj->isSorted = false;
    // else realloc...
    if( obj->maxAlloc )
    {
        if( toAlloc > obj->maxAlloc )
        {
            WHEFS_DBG_ERR("Returning AllocError because toAlloc (%"WHEFS_ID_TYPE_PFMT") >= list->maxAlloc (%"WHEFS_ID_TYPE_PFMT").",
                          toAlloc, obj->maxAlloc);
            return whefs_rc.AllocError;
        }
    }

    whefs_hashid * li = (whefs_hashid *) realloc( obj->list, toAlloc * sizeof(whefs_hashid) );
    if( ! li ) return whefs_rc.AllocError;
    obj->list = li;
    obj->alloced = toAlloc;
    whefs_id_type i = obj->count;
    if( toAlloc < obj->count )
    {
        obj->count = toAlloc;
    }
    for( ; i < toAlloc; ++i )
    {
	obj->list[i] = whefs_hashid_init;
    }
    return whefs_rc.OK;
}

void whefs_hashid_list_free( whefs_hashid_list * tgt )
{
    if( tgt )
    {
        whefs_hashid_list_alloc( &tgt, 0 );
        free( tgt );
    }
}

int whefs_hashid_list_chomp_lv( whefs_hashid_list * li )
{
    if( ! li ) return whefs_rc.ArgError;
    if( (li->count<2)
        || (li->count < (li->alloced/2) )
       )
    {
        return whefs_rc.OK;
    }
    qsort( li->list, li->count, sizeof(whefs_hashid), whefs_hashid_cmp_hits );
    //qsort( li->_head, li->count, sizeof(whefs_hashid), whefs_hashid_cmp_hits );
    whefs_id_type i;
    for( i = li->count/2; i < li->count; ++i )
    {
        li->list[i] = whefs_hashid_init;
    }
    whefs_hashid_list_sort(li);
    return whefs_rc.OK;
}

int whefs_hashid_list_add( whefs_hashid_list * tgt, whefs_hashid const * restrict val )
{
    if( ! tgt || !val ) return whefs_rc.ArgError;
    tgt->isSorted = false;
    if( tgt->count >= tgt->alloced )
    {
        whefs_id_type sz = (whefs_id_type)((tgt->count+1) * 2);
        if( tgt->maxAlloc && (sz > tgt->maxAlloc ) )
        {
            sz = tgt->maxAlloc;
        }
        if( sz <= tgt->count )
        { /* overflow or incorrect handling of maxAlloc  */
            return whefs_rc.RangeError;
        }
        int rc = whefs_hashid_list_alloc( &tgt, sz );
        if( whefs_rc.OK != rc ) return rc;
    }
    tgt->list[tgt->count++] = *val;
    return whefs_rc.OK;
}

whefs_id_type whefs_hashid_list_index_of( whefs_hashid_list const * src, whefs_hashval_type val )
{
    if( ! src || !src->count ) return whefs_rc.IDTypeEnd;
    if( ! src->isSorted )
    { // horrible special case to avoid having to re-sort on every inode name-set
#if 1
        WHEFS_DBG_CACHE("Warning: hashid list is unsorted. Running in O(N) here!");
        whefs_hashid *H = src->list;
	whefs_id_type i;
        for( i = 0; H && (i < src->count); ++i, ++H )
        {
            if( H->hash == val )
            {
                ++H->hits;
                return i;
            }
        }
        return whefs_rc.IDTypeEnd;
#endif
    }
    whefs_hashid hv = whefs_hashid_init;
    hv.hash = val;
    void const * f = bsearch( &hv, src->list, src->count, sizeof(whefs_hashid), whefs_hashid_cmp );
    if( ! f ) return whefs_rc.IDTypeEnd;
    whefs_id_type ndx = (((unsigned char const *)f) -((unsigned char const *)src->list)) / sizeof(whefs_hashid);
    while( ndx && (src->list[ndx-1].hash == val) ) --ndx;
    // FIXME: check leftwards for more matches.
    //++(src->list[ndx].hits);
    return ndx;
}

size_t whefs_hashid_list_sizeof( whefs_hashid_list const * li )
{
    if( ! li ) return 0;
    return sizeof(whefs_hashid_list)
        + (sizeof(whefs_hashid) * li->alloced);
}

int whefs_hashid_list_wipe_index( whefs_hashid_list * tgt, whefs_id_type ndx )
{
    if( ! tgt ) return whefs_rc.ArgError;
    if( tgt->count <= ndx ) return whefs_rc.RangeError;
    tgt->list[ndx] = whefs_hashid_init;
    tgt->isSorted = false;
    return whefs_rc.OK;
}

int whefs_hashid_list_add_slots( whefs_hashid_list * li, whefs_id_type pos, whefs_id_type count )
{
    //assert(0 && "Not finished!");
    if( ! li || !count ) return whefs_rc.ArgError;
    if( !li->count || (pos>li->count) ) return whefs_rc.RangeError;
    const whefs_id_type last = pos+count;
    const whefs_id_type asz = last+1;
    if( asz < pos /* overflow */ ) return whefs_rc.RangeError;
    if( li->alloced < asz )
    {
        int rc = whefs_hashid_list_alloc( &li, asz );
        if( whefs_rc.OK != rc ) return rc;
    }
    li->isSorted = false;
    void * from = &li->list[pos];
    const whefs_id_type tondx = pos + (li->count-pos);
    li->count += count;
    const whefs_id_type howmany = tondx - pos;
    void * to = &li->list[last];
    memmove( to, from, sizeof(whefs_hashid) * howmany );
    whefs_id_type i;
    for( i = pos; i < last; ++i )
    {
        li->list[i] = whefs_hashid_init;
    }
    return whefs_rc.OK;
}


#if 0 // unused code

/**
   Swaps the contents of rhs and lhs.
*/
void whefs_hashid_swap( whefs_hashid * lhs, whefs_hashid * rhs );
void whefs_hashid_swap( whefs_hashid * lhs, whefs_hashid * rhs )
{
    if( lhs && rhs )
    {
        whefs_hashid tmp = *lhs;
        *lhs = *rhs;
        *rhs = tmp;
    }
}


int whefs_hashid_list_check_bogo( whefs_hashval_type hash,
                                  whefs_id_type id,
                                  void const * data )
{
    return 0;
}
// e.g. whefs_hashid_list_search(src, hash, whefs_hash_list_inode_name_is, "searched-for-name")

whefs_id_type whefs_hashid_list_search( whefs_hashid_list const * src,
                                        whefs_hashval_type hash,
                                        whefs_hashid_list_search_cmp checkval,
                                        void const * checkvalData
                                        )
{
    whefs_id_type ndx = whefs_hashid_list_index_of( src, hash );
    if( whefs_rc.IDTypeEnd == ndx ) return ndx;
    whefs_hashid const * h = &src->list[ndx];
    while( 1 )
    {
        if( 0 == checkval( hash, h->id, checkvalData ) ) return h->id;
        if( src->count <= ++ndx ) break;
        if( src->list[ndx].hash != h->hash ) break;
        h = &src->list[ndx];
    }
    return whefs_rc.IDTypeEnd;
}
#endif // unused code
/* end file src/whefs_hash.c */
/* begin file src/whefs_nodedev.c */
#line 8 "src/whefs_nodedev.c"
/*
  This file contains a whio_dev implementation which treats
  whefs_inode objects as an i/o device. Reads and writes are directed
  to/from the VFS data blocks associated with the inode. It has an
  embarassingly intimate relationship with the whefs internals.

  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/

  License: Public Domain

  Trivia: by using this whio_dev as a target to whefs_mkfs_dev(), it
  is possible to create a VFS within a VFS.
*/
#include <stdlib.h> /* malloc() and friends */
#include <assert.h>
#include <string.h> /* memset() */

/**
   Ensures that ino->block.list is at least count items long,
   reallocating if need. As a special case, if count is 0 the
   list is freed. It may allocate more than count items, and
   ino->blocks.alloced will reflect the actual number.

   This function does not update ino->blocks.count unless count is 0
   (as described above).
*/
static int whefs_inode_block_list_reserve( whefs_fs * fs,
					whefs_inode * ino,
					whefs_id_type count )
{
    if( ! ino ) return whefs_rc.ArgError;
    if( ino->blocks.alloced >= count ) return whefs_rc.OK;
    else if( 0 == count )
    {
	free( ino->blocks.list );
	ino->blocks = whefs_block_list_init;
	return whefs_rc.OK;
    }
    //WHEFS_DBG("(Re)sizing inode block cache to %u items for inode #%u[%s].", count, ino->id, ino->name );
    whefs_block * li = (whefs_block *)realloc( ino->blocks.list, count * sizeof(whefs_block) );
    if( ! li )
    {
	return whefs_rc.AllocError;
    }
    ino->blocks.alloced = count;
    ino->blocks.list = li;
    whefs_id_type i = ino->blocks.count;
    for( ; i < count; ++i )
    {
	li[i] = whefs_block_init;
    }
    return whefs_rc.OK;
}

/**
   Appends a copy of bl to ino's block list, expanding the list as
   necessary. If ino has blocks already, the last block has bl added
   as its next block and that block is flushed to disk.

   ino is assumed to be an opened inode. If it is not, results
   are undefined.

   Returns whefs_rc.OK on success.
*/
static int whefs_inode_block_list_append( whefs_fs * fs,
					  whefs_inode * ino,
					  whefs_block const * bl )
{
    if( ! fs || !ino || !bl ) return whefs_rc.ArgError;
    if( ino->blocks.alloced <= ino->blocks.count )
    {
	int rc = whefs_inode_block_list_reserve( fs, ino, (ino->blocks.count ? ino->blocks.count : 4 /* arbitrarily chosen*/) * 2 );
	if( whefs_rc.OK != rc ) return rc;
    }
    ino->blocks.list[ino->blocks.count] = *bl;
    if( 0 < ino->blocks.count )
    { // append block to the list
	whefs_block * prev = &ino->blocks.list[ino->blocks.count-1];
	if( ! prev->next_block )
	{
	    prev->next_block = bl->id;
	    whefs_block_flush( fs, prev );
	}
	else if( prev->next_block != bl->id )
	{
	    WHEFS_DBG_ERR("Internal error: previous block (#%"WHEFS_ID_TYPE_PFMT") says its next block is #%"WHEFS_ID_TYPE_PFMT", but request was made to append #%"WHEFS_ID_TYPE_PFMT,
			  prev->id, prev->next_block, bl->id );
	    assert( 0 && "Internal consistency error." );
	    return whefs_rc.InternalError;
	}
    }
    else
    { // set bl as the first block...
	if( ino->first_block )
	{
	    if( ino->first_block != bl->id )
	    {
		WHEFS_DBG_ERR("Internal error: inode #%"WHEFS_ID_TYPE_PFMT" says its first block is #%"WHEFS_ID_TYPE_PFMT", but #%"WHEFS_ID_TYPE_PFMT" was added as the first item of its block list.",
			      ino->id, ino->first_block, bl->id );
		assert( 0 && "Internal consistency error." );
		return whefs_rc.InternalError;
	    }
	}
	else
	{
	    ino->first_block = bl->id;
	    whefs_inode_flush( fs, ino );
	}
    }
    ++ino->blocks.count;
    //WHEFS_DBG("Appended block #%"WHEFS_ID_TYPE_PFMT" to chain (of %"WHEFS_ID_TYPE_PFMT" item(s)) for inode #%"WHEFS_ID_TYPE_PFMT"[%s].", bl->id, ino->blocks.count, ino->id, ino->name );
    return whefs_rc.OK;
}

/**
   Assumes ino is an opened inode and loads a block list cache for it.
   If !ino->first_block then this function does nothing but returns
   whefs_rc.OK, otherwise...

   For each block in the chain starting at ino->first_block,
   the block is loaded and appended to ino->blocks.list.

   On success ino->blocks.list contains ino->blocks.count
   whefs_block items representing ino's block chain. To add
   items to the list use whefs_inode_block_list_append().
   To remove them, change ino->blocks.count, empty out
   the now-unused entries, and update the on-disk blocks.

   ino->blocks.count should be 0 when this function is called. If not,
   it may emit a warning debug message but will return a success
   value.
*/
static int whefs_inode_block_list_load( whefs_fs * fs,
					whefs_inode * ino )
{
    if( ! whefs_inode_is_valid(fs,ino) ) return whefs_rc.ArgError;
    if( ! ino->first_block ) return whefs_rc.OK;
    if( ino->blocks.count )
    {
	WHEFS_DBG_WARN("this function shouldn't be called when ino->blocks.count is !0. inode=#%u",
		       ino->id);
	return whefs_rc.OK;
    }
    whefs_block bl = whefs_block_init;
    int rc = whefs_block_read( fs, ino->first_block, &bl );
    if( whefs_rc.OK != rc ) return rc;
#if 0
    if( ! ino->blocks.list )
    {
	rc = whefs_inode_block_list_reserve( fs, ino, 5 /* arbitrarily chosen */ );
	if( whefs_rc.OK != rc ) return rc;
    }
#endif
    rc = whefs_inode_block_list_append( fs, ino, &bl );
    if( whefs_rc.OK != rc ) return rc;
    while( bl.next_block )
    {
	rc = whefs_block_read_next( fs, &bl, &bl );
	if( whefs_rc.OK != rc ) return rc;
        rc = whefs_inode_block_list_append( fs, ino, &bl );
	if( whefs_rc.OK != rc ) return rc;
    }
    //WHEFS_DBG("Loaded block chain of %u block(s) for inode #%u[%s].", ino->blocks.count, ino->id, ino->name );
    return whefs_rc.OK;
}

/**
   This function is the heart of the pseudofile i/o beast...

   It tries to map a logical file position to a data block
   associated with an inode.

   It starts with ino's first block and increments blocks until the
   block in which pos would land is found. If ino doesn't have enough
   blocks, the behaviour is defined by the expands parameter:

   If expands is true then it will add blocks to the inode's chain in
   order to reach the destination pos, if necessary. If expands is
   false and pos is not within the inode's current data size then the
   function fails.

   On success, tgt is populated with the block associated with the
   given position and inode, and ino *may* be updated (if it had no
   blocks associated with it beforehand). To figure out the proper
   offset of pos to use within the block use
   (pos%whefs_fs_options_get(fs)->block_size).

   This function never actually changes the logical size of the inode,
   but may allocate new blocks to it.

   On success whefs_rc.OK is returned, else some other error
   value. Some possibilities include:

   - whefs_rc.RangeError = pos it past EOF and expands is false.
   - whefs_rc.FSFull = ran out of blocks while trying to expand.
   - whefs_rc.ArgEror = !fs, !tgt, or ino is not valid

   BIG FAT HAIRY WARNING:

   It is intended that the ino argument be an inode which has been
   opened via whefs_inode_open(), but this function does not check
   that because doing so is relatively costly and this routine is
   called from the i/o device implementation for every read and write.

   Because the ino argument *may* be updated, it is imperative that if
   ino refers to an opened inode, that the ino argument actually be a
   pointer to that object, as opposed to being a copy of it. Failure
   to follow this may result in mis-synchronization of the node's
   state or a memory leak. Specifically, if the inode previously had
   no blocks, and this function adds at least one, then ino must be
   updated. If ino has the same ID as an opened inode but is not that
   opened inode object (see whefs_inode_search_opened()), then ino
   will be updated but the opened inode will not, which will probably
   lead to any new blocks allocated by this call to become lost the
   next time the opened inode is flushed.


   BIGGER, FATTER, HAIRIER WARNING:

   Because profiling has shown that this function spends a significant
   amount of time validating fs and ino (by calling
   whefs_inode_is_valid()), that check has been removed. Since this
   function "can only" be called from the whefs_nodedev
   implementation, we're relying on that to do the validation (which
   it does).

*/
static int whefs_block_for_pos( whefs_fs * restrict fs, whefs_inode * restrict ino, whio_size_t pos, whefs_block * restrict tgt, bool expand )
{
    //if(  !tgt || !whefs_inode_is_valid( fs, ino ) ) return whefs_rc.ArgError;
    if( (ino->data_size < pos) && !expand )
    {
	//WHEFS_DBG("return whefs_rc.RangeError");
	return whefs_rc.RangeError;
    }
    const whio_size_t bs = whefs_fs_options_get(fs)->block_size;
    const whefs_id_type bc = /* how many blocks will we need? */
        1 + (pos / bs);
    if(0) WHEFS_DBG("pos=%"WHIO_SIZE_T_PFMT" bs=%"WHIO_SIZE_T_PFMT" bc=%"WHEFS_ID_TYPE_PFMT,pos,bs,bc);
    /** ^^^ does this leave us with one too many blocks when we truncate() to
        an exact multiple of blocksize? */
    if( bc > whefs_fs_options_get(fs)->block_count )
    {
        WHEFS_DBG_WARN("VFS doesn't have enough blocks "
                       "(%"WHEFS_ID_TYPE_PFMT") to satisfy the "
                       "request for position %"WHIO_SIZE_T_PFMT
                       " of inode #%"WHEFS_ID_TYPE_PFMT,
                       whefs_fs_options_get(fs)->block_count, pos, ino->id );
        return whefs_rc.RangeError;
    }
    int rc = whefs_rc.OK;
    if( ! ino->blocks.list )
    {
	rc = whefs_inode_block_list_load( fs, ino );
	if( whefs_rc.OK != rc ) return rc;
    }
    if( !expand && (ino->blocks.count < bc) )
    { /* can't grow list for this request. */
	return whefs_rc.RangeError;
    }
    // TODO: check number of available inodes here, and don't try to expand if we can't reach the end
    whefs_block bl = whefs_block_init;
    //WHEFS_DBG("About to search inode #%u for %u block(s) (size=%u) to find position %u", ino->id, bc, bs, pos );
    rc = whefs_rc.OK;
    whefs_block * blP = 0;
    if( bc <= ino->blocks.count)
    {
	blP = &ino->blocks.list[bc-1]; /* jump right to it */;
    }
    else
    { /* expand the list */
	if( ! expand )
	{
	    if(0) WHEFS_DBG("Cannot expand to %"WHEFS_ID_TYPE_PFMT" blocks for position %"WHIO_SIZE_T_PFMT" because [expand] parameter is false.",
			    bc, pos );
	    return whefs_rc.RangeError;
	}
	//bl = ino->blocks.list[ino->blocks.count-1];
	whefs_id_type i = ino->blocks.count;
	blP = 0;
	for( ; i < bc; ++i )
	{
	    rc = whefs_block_next_free( fs, &bl, true );
	    if( whefs_rc.OK == rc ) rc = whefs_inode_block_list_append( fs, ino, &bl );
	    if( whefs_rc.OK != rc ) return rc;
	    /**
	       We "might" want to truncate the inode back to its
	       previous length, but why add room for yet another error
	       on top of the one we just encountered?
	    */
	}
	blP = &bl;
    }
    if( whefs_rc.OK == rc )
    {
	*tgt = *blP;
    }
    //WHEFS_DBG("Using block id #%u for pos %u of inode #%u", blP->id, pos, ino->id );
    return rc;
}

/**
   Internal implementation details for the whio_dev whefs_inode
   wrapper.
*/
typedef struct whio_dev_inode_meta
{
    /**
       Associated filesystem.
     */
    whefs_fs * fs;
    /* block size, to avoid having to continually deref fs->options.block_size. */
    uint32_t bs;
    /**
       File position cursor.
    */
    whio_size_t posabs;
    /** rw==true if read/write, else false. */
    bool rw;
    /** inode associated with device. */
    whefs_inode * inode;
} whio_dev_inode_meta;

/** Initializer object. */
#define WHIO_DEV_INODE_META_INIT { \
0, /* fs */ \
0, /* bs */ \
0, /* posabs */ \
false, /* read/write */ \
0 /* inode */  \
}

static const whio_dev_inode_meta whio_dev_inode_meta_init = WHIO_DEV_INODE_META_INIT;


#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC /* see whio_common.h for details */
enum {
/**
   The number of elements to statically allocate
   in the whio_dev_inode_meta_alloc_slots object.
*/
whio_dev_inode_meta_alloc_count = 10
};
static struct
{
    whio_dev_inode_meta objs[whio_dev_inode_meta_alloc_count];
    char used[whio_dev_inode_meta_alloc_count];
    size_t next;
} whio_dev_inode_meta_alloc_slots = { {WHIO_DEV_INODE_META_INIT},{0}, 0 };
#endif

static whio_dev_inode_meta * whio_dev_inode_meta_alloc()
{
    whio_dev_inode_meta * obj = 0;
#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = whio_dev_inode_meta_alloc_slots.next;
    for( ; i < whio_dev_inode_meta_alloc_count; ++i )
    {
	if( whio_dev_inode_meta_alloc_slots.used[i] ) continue;
	whio_dev_inode_meta_alloc_slots.next = i+1;
	whio_dev_inode_meta_alloc_slots.used[i] = 1;
	obj = &whio_dev_inode_meta_alloc_slots.objs[i];
	break;
    }
#endif /* WHEFS_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_inode_meta *) malloc( sizeof(whio_dev_inode_meta) );
    if( obj ) *obj = whio_dev_inode_meta_init;
    return obj;
}

static void whio_dev_inode_meta_free( whio_dev_inode_meta * obj )
{
    if( obj ) *obj = whio_dev_inode_meta_init;
    else return;
#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whio_dev_inode_meta_alloc_slots.objs[0]) ||
	(obj > &whio_dev_inode_meta_alloc_slots.objs[whio_dev_inode_meta_alloc_count-1]) )
    { /* it does not belong to us */
	free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whio_dev_inode_meta_alloc_slots.objs[0]);
	whio_dev_inode_meta_alloc_slots.used[ndx] = 0;
	if( ndx < whio_dev_inode_meta_alloc_slots.next ) whio_dev_inode_meta_alloc_slots.next = ndx;
	return;
    }
#else
    free(obj);
#endif /* WHEFS_CONFIG_ENABLE_STATIC_MALLOC */
}


/**
   A helper for the whio_dev_inode API. Requires that the 'dev'
   parameter be-a whio_dev and that that device is-a whio_dev_inode_meta.
 */
#define WHIO_DEV_DECL(RV) whio_dev_inode_meta * meta = (dev ? (whio_dev_inode_meta*)dev->impl.data : 0); \
    if( !meta || ((void const *)&whio_dev_inode_meta_init != dev->impl.typeID) ) return RV

/**
   Internal implementation of whio_dev_inode_read(). All arguments
   are as for whio_dev::read() except keepGoing:

   If this routine must "wrap" over multiple blocks, it will read what it
   can from one block and then set keepGoing to true and return the read
   size. The caller should, if keepGoing is true, call this function
   again, adding the returned size to dest and subtracting it from n.
*/
static whio_size_t whio_dev_inode_read_impl( whio_dev * dev,
					whio_dev_inode_meta * meta,
					void * dest,
					whio_size_t n,
					bool * keepGoing )
{
#if 0
    if( ! dev || !meta || !dest || !n || ! keepGoing )
    {
	if( keepGoing ) *keepGoing = false;
	return 0;
    }
#endif
    *keepGoing = false;
    if( ! n ) return 0U;
    else if( meta->posabs >= meta->inode->data_size ) return 0;
    int rc = 0;
    //whio_size_t eofpos = meta->inode->data_size;
    whefs_block block = whefs_block_init;
    rc = whefs_block_for_pos( meta->fs, meta->inode, meta->posabs, &block, false );
    if( whefs_rc.OK != rc )
    {
#if 0
        if( !(meta->posabs % meta->fs->options.block_size) )
        {
            // FIXME: ensure that meta->inode->blocks[end] is really the end block that should line up here.
            /**
               This is an unusual special case. Directly at the EOF boundary we want to return
               a non-error code. We're at EOF. whefs_block_for_pos() isn't quite smart enough
               to know that, though.

               This fix should go in whefs_block_for_pos()
            */
            WHEFS_FIXME("Special-case block boundary story: inode #%"WHEFS_ID_TYPE_PFMT": error #%d getting block for meta->posabs=%u. n=%"WHIO_SIZE_T_PFMT", bs=%"WHIO_SIZE_T_PFMT", at end of the block chain.",
                        meta->inode->id, rc, meta->posabs, n, meta->fs->options.block_size );
            return 0;
        }
        else
        {
            WHEFS_DBG("Error #%d getting block for meta->posabs=%u. n=%"WHIO_SIZE_T_PFMT", bs=%"WHIO_SIZE_T_PFMT,
                      rc, meta->posabs, n, meta->fs->options.block_size );
            return 0;
        }
#else
        WHEFS_DBG("Error #%d getting block for meta->posabs=%u. n=%"WHIO_SIZE_T_PFMT", bs=%"WHIO_SIZE_T_PFMT,
                  rc, meta->posabs, n, meta->fs->options.block_size );
        return 0;
#endif
    }
    if(0) WHEFS_DBG("inode #%"WHEFS_ID_TYPE_PFMT" will be using block #%"WHEFS_ID_TYPE_PFMT" for a read at pos %"WHIO_SIZE_T_PFMT, meta->inode->id, block.id, meta->posabs );

    if( meta->posabs >= meta->inode->data_size ) return 0;
    const whio_size_t rdpos = (meta->posabs % meta->bs);
    const whio_size_t left = meta->bs - rdpos;
    const whio_size_t bdpos = whefs_block_data_pos( meta->fs, &block );
    whio_size_t rdlen = ( n > left ) ? left : n;
    if( (rdlen + meta->posabs) >= meta->inode->data_size )
    {
	rdlen = meta->inode->data_size - meta->posabs;
    }
    //WHEFS_DBG("rdpos=%u left=%u bdpos=%u rdlen=%u", rdpos, left, bdpos, rdlen );
    whio_dev * fd = meta->fs->dev;
    fd->api->seek( fd, bdpos + rdpos, SEEK_SET );
    const whio_size_t sz = fd->api->read( fd, dest, rdlen );
    if( ! sz ) return 0;
    const whio_size_t szCheck = meta->posabs + sz;
    if( szCheck > meta->posabs )
    {
	meta->posabs = szCheck;
    }
    else
    {
	WHEFS_DBG_ERR("Numeric overflow in read! (pos=%"WHIO_SIZE_T_PFMT" + readLength=%"WHIO_SIZE_T_PFMT") = overflow", meta->posabs, sz );
        return 0;
    }
    //whefs_block_flush( meta->fs, &block );
    if(0) WHEFS_DBG("Read %"WHIO_SIZE_T_PFMT" of %"WHEFS_ID_TYPE_PFMT" (n=%"WHIO_SIZE_T_PFMT") bytes "
		    "from inode #%"WHEFS_ID_TYPE_PFMT"'s block #%"WHEFS_ID_TYPE_PFMT". "
		    "fs pos=%"WHIO_SIZE_T_PFMT", block offset=%"WHIO_SIZE_T_PFMT" file pos=%"WHIO_SIZE_T_PFMT", file eof=%"WHIO_SIZE_T_PFMT,
		    sz, rdlen, n,
		    meta->inode->id, block.id,
		    bdpos, rdpos, meta->posabs, meta->inode->data_size );
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

static whio_size_t whio_dev_inode_read( whio_dev * dev, void * dest, whio_size_t n )
{
    WHIO_DEV_DECL(0);
    bool keepGoing = true;
    whio_size_t total = 0;
    while( keepGoing )
    {
	const whio_size_t sz = whio_dev_inode_read_impl( dev, meta, WHIO_VOID_PTR_ADD(dest,total), n - total, &keepGoing );
	total += sz;
    }
    return total;
}

/**
   This function's logic and handling of the keepGoing parameter are
   identical to that of whio_dev_inode_read_impl(), but apply to
   writes instead of reads.
*/
static whio_size_t whio_dev_inode_write_impl( whio_dev * dev,
					 whio_dev_inode_meta * meta,
					 void const * src, whio_size_t n,
					 bool * keepGoing )
{
    if( ! dev || !meta || !src || !n || !keepGoing )
    {
	if( keepGoing ) *keepGoing = false;
	return 0;
    }
    *keepGoing = false;
    int rc = 0;
    //whio_size_t eofpos = meta->inode->data_size;
    whefs_block block = whefs_block_init;
    rc = whefs_block_for_pos( meta->fs, meta->inode, meta->posabs, &block, true );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG("Error #%d getting block for meta->posabs==%u", rc, meta->posabs );
	return 0;
    }
    const whio_size_t wpos = (meta->posabs % meta->bs);
    const whio_size_t left = meta->bs - wpos;
    const whio_size_t bdpos = whefs_block_data_pos( meta->fs, &block );
    const whio_size_t wlen = ( n > left ) ? left : n;
    //WHEFS_DBG("wpos=%u left=%u bdpos=%u wlen=%u", wpos, left, bdpos, wlen );
    whio_dev * fd = meta->fs->dev;
    fd->api->seek( fd, bdpos + wpos, SEEK_SET );
    whio_size_t sz = fd->api->write( fd, src, wlen );
    if( ! sz ) return 0;
    whefs_inode_update_mtime( meta->fs, meta->inode );
    whio_size_t szCheck = meta->posabs + sz;
    if( szCheck > meta->posabs )
    {
	meta->posabs = szCheck;
    }
    if( meta->inode->data_size < meta->posabs )
    {
	meta->inode->data_size = meta->posabs;
	//whefs_inode_flush( meta->fs, &meta->inode ); // we should do this, really.
    }
    //whefs_block_flush( meta->fs, &block );
    if(0) WHEFS_DBG("Wrote %u of %u (n=%u) bytes "
		    "to inode #%u's block #%u. "
		    "fs pos=%u, block offset=%u file pos=%u, file eof=%u",
		    sz, wlen, n,
		    meta->inode->id, block.id,
		    bdpos, wpos, meta->posabs, meta->inode->data_size );
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

static whio_size_t whio_dev_inode_write( whio_dev * dev, void const * src, whio_size_t n )
{
    WHIO_DEV_DECL(0);
    if( ! meta->rw ) return 0;
    bool keepGoing = true;
    whio_size_t total = 0;
    while( keepGoing )
    {
	const whio_size_t sz = whio_dev_inode_write_impl( dev, meta, WHIO_VOID_CPTR_ADD(src,total), n - total, &keepGoing );
	total += sz;
    }
    return total;
}

static int whio_dev_inode_error( whio_dev * dev )
{
    WHIO_DEV_DECL(whio_rc.ArgError);
    return whio_rc.OK;
}

static int whio_dev_inode_clear_error( whio_dev * dev )
{
    WHIO_DEV_DECL(whio_rc.ArgError);
    return whio_rc.OK;
}

static int whio_dev_inode_eof( whio_dev * dev )
{
    WHIO_DEV_DECL(whio_rc.ArgError);
    return (meta->posabs >= meta->inode->data_size)
	? 1
	: 0;
}

static whio_size_t whio_dev_inode_tell( whio_dev * dev )
{
    WHIO_DEV_DECL(whio_rc.SizeTError);
    return meta->posabs;
}

static whio_size_t whio_dev_inode_seek( whio_dev * dev, off_t pos, int whence )
{
    WHIO_DEV_DECL(whio_rc.SizeTError);
    whio_size_t too = meta->posabs;
    switch( whence )
    {
      case SEEK_SET:
	  if( pos < 0 ) return whio_rc.SizeTError;
	  too = (whio_size_t)pos;
	  break;
      case SEEK_END:
	  too = meta->inode->data_size + pos;
	  //if( too < meta->inode->data_size )  /* overflow! */ return whio_rc.SizeTError;
	  break;
      case SEEK_CUR:
	  too += pos;
	  if( too < meta->posabs )  /* overflow! */ return whio_rc.SizeTError;
	  break;
      default:
	  return whio_rc.SizeTError;
	  break;
    };
    return (meta->posabs = too);
}

static int whio_dev_inode_flush( whio_dev * dev )
{
    WHIO_DEV_DECL(whio_rc.ArgError);
    if(0) WHEFS_DBG_FYI("Flushing i/o %s device for inode #%"WHIO_SIZE_T_PFMT". "
			"inode->data_size=%"WHIO_SIZE_T_PFMT" posabs=%"WHIO_SIZE_T_PFMT,
			meta->rw ? "read/write" : "read-only", meta->inode->id,
			meta->inode->data_size, meta->posabs
			);
    int rc = meta->rw
	? whefs_inode_flush( meta->fs, meta->inode )
	: whefs_rc.OK;
    if( meta->rw )
    {
        whefs_fs_flush( meta->fs );
    }
    if(0) WHEFS_DBG_FYI("Flushed (rc=%d) i/o %s device for inode #%"WHEFS_ID_TYPE_PFMT". "
			"inode->data_size=%"WHIO_SIZE_T_PFMT" posabs=%"WHIO_SIZE_T_PFMT,
			rc, meta->rw ? "read/write" : "read-only", meta->inode->id,
			meta->inode->data_size, meta->posabs
			);
    return rc;
}

static int whio_dev_inode_trunc( whio_dev * dev, off_t len )
{
    /* Man, this was a bitch to do! */
    WHIO_DEV_DECL(whio_rc.ArgError);
    if( len < 0 ) return whio_rc.AccessError;
    if( ! meta->rw ) return whio_rc.AccessError;
    const whio_size_t off = (whio_size_t)len;
    if( off > len ) return whio_rc.RangeError; /* overflow */
    if( off == meta->inode->data_size ) return whefs_rc.OK;

    if( 0 == len )
    { /* special (simpler) case for 0 byte truncate */
	if( ! meta->inode->first_block ) return whefs_rc.OK;
	// (WTF?) FIXME: update ino->blocks.list[0]
        if( meta->inode->first_block ) 
        {
            whefs_block block = whefs_block_init;
            int rc = whefs_block_read( meta->fs, meta->inode->first_block, &block ); // ensure we pick up whole block chain
            if( whefs_rc.OK != rc )
            {
                rc = whefs_block_wipe( meta->fs, &block, true, true, true );
            }
            if( whefs_rc.OK != rc ) return rc;
        }
	meta->inode->blocks.count = 0;
	meta->inode->first_block = 0;
	meta->inode->data_size = 0;
	whefs_inode_flush(meta->fs, meta->inode );
	return whio_rc.OK;
    }

    int rc = whio_rc.OK;
    //const size_t oldSize = off>meta->inode->data_size;
    const short dir = (off < meta->inode->data_size)
	? -1
	: ((off>meta->inode->data_size) ? 1 : 0);
    assert( (0 != off) && "This shouldn't be able to happen!" );

    /* Update inode metadata... */
    //WHEFS_DBG("truncating from %u to %u bytes",meta->inode->data_size, off);
    meta->inode->data_size = off;
    rc = whefs_inode_flush( meta->fs, meta->inode );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Flush failed for inode #%u. Error code=%d.",
		      meta->inode->id, rc );
	return rc;
    }
    /* Update block info... */
    whefs_block bl = whefs_block_init;
    rc = whefs_block_for_pos( meta->fs, meta->inode, off, &bl, true );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("Could not get block for write position %u of inode #%u. Error code=%d.",
		      off, meta->inode->id, rc );
	return rc;
    }
    //const size_t dest = meta->inode->data_size;
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
	const uint32_t bs = whefs_fs_options_get( meta->fs )->block_size;
	rc = whefs_block_wipe_data( meta->fs, &bl, ( off % bs ) );
	if( whefs_rc.OK != rc ) return rc;
#endif
	if( ! bl.next_block )
	{ /* Lucky for us! No more work to do! */
	    meta->inode->blocks.count = 1;
	    return whefs_rc.OK;
	}

	whefs_block * blP = &meta->inode->blocks.list[0];
	whefs_block * nblP = blP + 1;
	uint32_t x = 1;
	for( ; (x < meta->inode->blocks.count)
		 && (nblP->id != bl.next_block)
		 ; ++nblP, ++x )
	{
	    /* Skip to bl.next_block */
	}
	if( (x == meta->inode->blocks.count) || (nblP->id != bl.next_block) )
	{
	    WHEFS_DBG_ERR("nblP->id=%u, bl.next_block=%u", nblP->id, bl.next_block );
	    WHEFS_DBG_ERR("Internal block cache for inode #%u is not as "
			  "long as we expect it to be or is missing entries!",
			  meta->inode->id );
	    return whefs_rc.InternalError;
	}
	blP = nblP - 1;
	meta->inode->blocks.count = x;
	whefs_block_wipe( meta->fs, nblP, true, true, true );
	blP->next_block = 0;
	return whefs_block_flush( meta->fs, blP );
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
	const whio_size_t PosAbs = meta->posabs;
	const whio_size_t orig = meta->inode->data_size;
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
	    ? whefs_rc.OK
	    : whefs_rc.IOError;
    }
    else
    {
	/* cannot happen due to special-case handling of truncate(0), above. */
	assert( 0 && "This is impossible!" );
    }
    WHEFS_DBG("You should never have gotten to this line!");
    return whefs_rc.InternalError;
}

short whio_dev_inode_iomode( whio_dev * dev )
{
    WHIO_DEV_DECL(-1);
    return (meta->inode->writer == dev) ? 1 : 0;
}

static int whio_dev_inode_ioctl( whio_dev * dev, int arg, va_list vargs )
{
    int rc = whio_rc.UnsupportedError;
    WHIO_DEV_DECL(whio_rc.ArgError);
    switch( arg )
    {
      case whio_dev_ioctl_GENERAL_size:
	  rc = whio_rc.OK;
	  *(va_arg(vargs,whio_size_t*)) = meta->inode->data_size;
	  break;
      default: break;
    };
    return rc;
}

static bool whio_dev_inode_close( whio_dev * dev )
{
    if( dev && ((void const *)&whio_dev_inode_meta_init == dev->impl.typeID))
    {
	if( dev->client.dtor ) dev->client.dtor( dev->client.data );
	dev->client = whio_client_data_init;
	whio_dev_inode_meta * meta = (whio_dev_inode_meta*)dev->impl.data;
	if( meta )
	{
            whefs_fs_closer_dev_remove( meta->fs, dev );
	    dev->impl.data = 0;
            if( meta->rw ) dev->api->flush(dev);
	    if(0) WHEFS_DBG_FYI("Closing i/o %s device for inode #%u. "
				"inode->data_size=%u posabs=%u",
				meta->rw ? "read/write" : "read-only",
				meta->inode->id,
				meta->inode->data_size, meta->posabs
				);
	    whefs_inode_close( meta->fs, meta->inode, dev );
	    whio_dev_inode_meta_free( meta );
	    return true;
	}
    }
    return false;
}


static void whio_dev_inode_finalize( whio_dev * dev )
{
    if( dev && ((void const *)&whio_dev_inode_meta_init == dev->impl.typeID))
    {
	if(0)
	{
	    whio_dev_inode_meta * meta = (whio_dev_inode_meta*)dev->impl.data;
	    WHEFS_DBG_FYI("Finalizing %s i/o device for inode #%u. "
			  "inode->data_size=%u posabs=%u",
			  meta->rw ? "read/write" : "read-only",
			  meta->inode->id,
			  meta->inode->data_size, meta->posabs
			  );
	    }
	dev->api->close( dev );
	whio_dev_free( dev );
    }
}

static const whio_dev_api whio_dev_api_inode_init =
    {
    whio_dev_inode_read,
    whio_dev_inode_write,
    whio_dev_inode_close,
    whio_dev_inode_finalize,
    whio_dev_inode_error,
    whio_dev_inode_clear_error,
    whio_dev_inode_eof,
    whio_dev_inode_tell,
    whio_dev_inode_seek,
    whio_dev_inode_flush,
    whio_dev_inode_trunc,
    whio_dev_inode_ioctl,
    whio_dev_inode_iomode
    };

static const whio_dev whio_dev_inode_init =
    {
    &whio_dev_api_inode_init,
    { /* impl */
    0, /* data. Must be-a (whio_dev_inode_meta*) */
    (void const *)&whio_dev_inode_meta_init /* typeID */
    }
    };

#undef WHIO_DEV_DECL



whio_dev * whefs_dev_for_inode( whefs_fs * fs, whefs_id_type nid, bool writeMode )
{
    //WHEFS_DBG("trying to open dev for inode #%u", nid );
    if( ! whefs_inode_id_is_valid( fs, nid ) ) return 0;
    whio_dev * dev = whio_dev_alloc();
    if( ! dev ) return 0;
    whefs_inode * ino = 0;
    void const * writeKey = (writeMode ? dev : 0);
    int rc = whefs_inode_open( fs, nid, &ino, writeKey );
    if( rc != whefs_rc.OK )
    {
	WHEFS_DBG_ERR("whefs_inode_open(fs,[inode #%"WHEFS_ID_TYPE_PFMT"],inode,%d) failed with rc %d!", nid, writeMode, rc );
	whio_dev_free( dev );
	return 0;
    }
    //WHEFS_DBG("Opened inode #%u[%s]", ino->id, ino->name );
    whio_dev_inode_meta * meta = whio_dev_inode_meta_alloc();
    if( ! meta )
    {
	whefs_inode_close( fs, ino, writeKey );
	whio_dev_free(dev);
	return 0;
    }
    *dev = whio_dev_inode_init;
    *meta = whio_dev_inode_meta_init;
    dev->impl.data = meta;
    meta->fs = fs;
    meta->rw = writeMode;
    meta->bs = whefs_fs_options_get(fs)->block_size;
    meta->inode = ino;
#if 0
    if( writeMode )
    {
	whefs_inode_flush( fs, meta->inode ); /* make sure on-disk matches */
    }
#endif
    whefs_fs_closer_dev_add( fs, dev );
    return dev;
}

/* end file src/whefs_nodedev.c */
/* begin file src/whefs_file.c */
#line 8 "src/whefs_file.c"
/*
  Implementations for the whefs_file-related API.

  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/

  License: Public Domain
*/
#include <stdlib.h> /* malloc() and friends */
#include <string.h> /* strchr() */

#define whefs_file_init_m { \
    0, /* fs */ \
    0, /* flags */ \
    0, /* dev */ \
    0 /* inode_id */                       \
    }

const whefs_file whefs_file_init = whefs_file_init_m;
#define WHEFS_FILE_ISOPENED(F) ((F) && ((F)->flags & WHEFS_FLAG_Opened))
#define WHEFS_FILE_ISRO(F) ((F) && ((F)->flags & WHEFS_FLAG_Read))
#define WHEFS_FILE_ISRW(F) ((F) && ((F)->flags & WHEFS_FLAG_Write))
#define WHEFS_FILE_ISERR(F) ((F) && ((F)->flags & WHEFS_FLAG_FileError))
#define WHEFS_FILE_SET_ERR(F,ERR) ((F) && ((F)->flags |= WHEFS_FLAG_FileError))


#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whefs_file_alloc_slots object.
*/
whefs_file_alloc_count = 10
};

static struct
{
    whefs_file objs[whefs_file_alloc_count];
    char used[whefs_file_alloc_count];
} whefs_file_alloc_slots = { {whefs_file_init_m}, {0} };
#endif

static whefs_file * whefs_file_alloc()
{
    whefs_file * obj = 0;
#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whefs_file_alloc_count; ++i )
    {
	if( whefs_file_alloc_slots.used[i] ) continue;
	whefs_file_alloc_slots.used[i] = 1;
	whefs_file_alloc_slots.objs[i] = whefs_file_init;
	obj = &whefs_file_alloc_slots.objs[i];
	break;
    }
#endif /* WHEFS_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj )
    {
        obj = (whefs_file *) malloc( sizeof(whefs_file) );
        if( obj ) *obj = whefs_file_init;
    }
    return obj;
}

static void whefs_file_free( whefs_file * restrict obj )
{
    if(obj) whefs_string_clear( &obj->name, false );
#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whefs_file_alloc_slots.objs[0]) ||
	(obj > &whefs_file_alloc_slots.objs[whefs_file_alloc_count-1]) )
    { /* it does not belong to us */
        *obj = whefs_file_init;
	free(obj);
	return;
    }
    else
    {
	const size_t ndx = (obj - &whefs_file_alloc_slots.objs[0]);
	whefs_file_alloc_slots.objs[ndx] = whefs_file_init;
	whefs_file_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    *obj = whefs_file_init;
    free(obj);
#endif /* WHEFS_CONFIG_ENABLE_STATIC_MALLOC */
}

/**
   Opens a pseudofile in read-only mode. f must be an allocated
   whefs_file object. This routine initializes f->dev and directs it
   to the found inode.  Returns whefs_rc.OK on success.

   This routine has an embarassing intimate relationship with
   whefs_fopen().
*/
static int whefs_fopen_ro( whefs_file * restrict f, char const * name )
{
    whefs_inode n = whefs_inode_init;
    int rc = whefs_inode_by_name( f->fs, name, &n );
    if( whefs_rc.OK == rc )
    {
	if( f->dev ) f->dev->api->finalize(f->dev);
	f->dev = whefs_dev_for_inode( f->fs, n.id, false );
	if( f->dev )
	{
	    f->inode = n.id;
	    f->flags = WHEFS_FLAG_Opened | WHEFS_FLAG_Read;
	}
	else
	{
	    f->flags = WHEFS_FLAG_FileError;
	    rc = whefs_rc.AllocError; /* guessing! */
	}
    }
    return rc;
}

/**
   Like whefs_fopen_ro(), but sets up f in read/write mode.
*/
static int whefs_fopen_rw( whefs_file * restrict f, char const * name )
{
    if( ! f || ! name ) return whefs_rc.ArgError;
    if( ! whefs_fs_is_rw(f->fs) ) return whefs_rc.AccessError;
    whefs_inode n = whefs_inode_init;
    int rc = whefs_inode_by_name( f->fs, name, &n );
    if( whefs_rc.OK != rc ) do
    {
        //WHEFS_DBG_FYI("whefs_inode_by_name(fs,[%s]) found no inode. Trying to create one...",name);
	/**
	   Create new entry...
	*/
	rc = whefs_inode_next_free( f->fs, &n, true );
	if( rc != whefs_rc.OK ) break;
	rc = whefs_inode_name_set( f->fs, n.id, name );
	if( rc != whefs_rc.OK ) break;
	whefs_inode_update_mtime( f->fs, &n );
	rc = whefs_inode_flush( f->fs, &n );
	//if( rc != whefs_rc.OK ) break;
	break;
    } while(1);
    //WHEFS_DBG("f->flags = 0x%04x", f->flags );
    if( rc != whefs_rc.OK )
    {
	WHEFS_DBG_ERR("open-for-write (%s) failed with rc %d\n!",name,rc);
	return rc;
    }
    if( f->dev ) f->dev->api->finalize(f->dev);
    f->dev = whefs_dev_for_inode( f->fs, n.id, true );
    if( f->dev )
    {
	f->inode = n.id;
	f->flags = WHEFS_FLAG_Opened | WHEFS_FLAG_Write;
    }
    else
    {
	f->flags = WHEFS_FLAG_FileError;
        rc = whefs_rc.InternalError;
    }
    return rc;
}

whefs_file * whefs_fopen( whefs_fs * fs, char const * name, char const * mode )
{
    if( ! fs || !name || !*name || !mode || !*mode ) return 0;
    unsigned int flags = 0;
    if( 0 && (0 != strchr( mode, 'w' )) )
    { // FIXME: add support for mode 'w' and 'w+'
	flags = WHEFS_FLAG_ReadWrite;
    }
    else if( 0 != strchr( mode, 'r' ) )
    {
	if( 0 != strchr( mode, '+' ) ) flags = WHEFS_FLAG_ReadWrite;
	else flags = WHEFS_FLAG_Read;
    }
    if( ! flags ) return 0;
    if( (flags & WHEFS_FLAG_Write) && !whefs_fs_is_rw(fs) )
    {
	WHEFS_DBG_WARN("EFS is opened read-only, so we cannot open files in read/write mode.");
	return 0;
    }
    whefs_file * f = whefs_file_alloc();
    if( ! f ) return 0;
    *f = whefs_file_init;
    f->fs = fs;
    f->flags = flags;
    int rc = whefs_rc.IOError;
    //WHEFS_DBG_FYI("fopen(fs,[%s],[%s]) flags=0x%08x ISRW=%d", name, mode, flags, WHEFS_FILE_ISRW(f) );
    rc = WHEFS_FILE_ISRW(f)
	? whefs_fopen_rw( f, name )
	: whefs_fopen_ro( f, name );
    if( (rc != whefs_rc.OK) || !WHEFS_FILE_ISOPENED(f) || WHEFS_FILE_ISERR(f) )
    {
	whefs_fclose( f );
	f = 0;
    }
    else
    {
        
        whefs_fs_closer_file_add( fs, f );
    }
    //WHEFS_DBG("opened whefs_file [%s]. mode=%s, flags=%08x", name, mode, f->flags );
    return f;
}

whio_dev * whefs_dev_open( whefs_fs * fs, char const * name, bool writeMode )
{
    if( ! fs || !name ) return 0;
    if( writeMode && ! whefs_fs_is_rw(fs) )
    {
	return 0;
    }
    whefs_inode ino = whefs_inode_init;
    if( whefs_rc.OK != whefs_inode_by_name( fs, name, &ino ) )
    { // Try to create one...
	if( ! writeMode )
	{
	    WHEFS_DBG_WARN("Open for read failed: did not find pseudofile named [%s].",name);
	    return 0;
	}
	if( whefs_rc.OK != whefs_inode_next_free( fs, &ino, true ) )
	{
	    WHEFS_DBG_WARN("Opening inode for [%s] failed! EFS is likely full.", name );
	    return 0;
	}
	if( whefs_rc.OK != whefs_inode_name_set( fs, ino.id, name ) )
	{
	    WHEFS_DBG_WARN("Setting inode #%"WHEFS_ID_TYPE_PFMT" name to [%s] failed!", ino.id, name );
	    return 0;
	}
	whefs_inode_flush( fs, &ino );
    }
    whio_dev * dev = whefs_dev_for_inode( fs, ino.id, writeMode );
    if( ! dev )
    {
	WHEFS_FIXME("Creation of i/o device for inode #%"WHEFS_ID_TYPE_PFMT" failed - "
		    "be sure we delete the inode entry if it didn't exist before this call!",
		    ino.id );
    }
    return dev;
}


/**
   Internal type to allow us to properly disconnect a whio_stream
   object from close-at-shutdown without needing to add a custom
   whio_stream implementation.
*/
typedef struct
{
    whefs_fs * fs;
    whio_stream const * stream;
} whefs_stream_closer_kludge;

/** v must be-a populated whefs_stream_closer_kludge.
    Removes v->stream from fs' closer list and frees
    v.
*/
static void whefs_stream_closer_kludge_dtor( void * v )
{
    if(0) whefs_stream_closer_kludge_dtor(0); /* avoid "static func defined but not used" warning. */
    if(v)
    {
        //WHEFS_DBG("Removing whio_stream from closer list.");
        whefs_stream_closer_kludge * k = (whefs_stream_closer_kludge*)v;
        whefs_fs_closer_stream_remove( k->fs, k->stream );
        free( k );
    }
}

whio_stream * whefs_stream_open( whefs_fs * fs, char const * name, bool writeMode, bool append )
{
    whio_dev * d = whefs_dev_open( fs, name, writeMode );
    if( ! d ) return 0;
    if( writeMode )
    {
        if( append )
        {
            d->api->seek( d, 0L, SEEK_END );
        }
        else
        {
            d->api->truncate( d, 0 );
        }
    }
    whefs_stream_closer_kludge * k = (whefs_stream_closer_kludge*)malloc(sizeof(whefs_stream_closer_kludge));
    if( ! k )
    {
        d->api->finalize(d);
        return 0;
    }
    whio_stream * s = whio_stream_for_dev( d, true );
    if( ! s )
    {
        free(k);
        d->api->finalize(d);
    }
    else
    {
        k->fs = fs;
        k->stream = s;
        whefs_fs_closer_stream_add( fs, s, d );
        d->client.data = k;
        d->client.dtor = whefs_stream_closer_kludge_dtor;
    }
    return s;
}


whio_dev * whefs_fdev( whefs_file * restrict f )
{
    return f ? f->dev : 0;
}

whio_size_t whefs_fseek( whefs_file * restrict f, size_t pos, int whence )
{
    return (f && f->dev)
	? f->dev->api->seek( f->dev, pos, whence )
	: whefs_rc.SizeTError;
}

int whefs_frewind( whefs_file * restrict f )
{
    return (f)
	? whio_dev_rewind( f->dev )
	: whefs_rc.ArgError;
}

int whefs_ftrunc( whefs_file * restrict f, size_t pos )
{
    return (f && f->dev)
	? f->dev->api->truncate( f->dev, pos )
	: whefs_rc.ArgError;
}


whefs_fs * whefs_file_fs( whefs_file * restrict f )
{
    return f ? f->fs : 0;
}

int whefs_fclose( whefs_file * restrict f )
{
    int rc = f ? whefs_rc.OK : whefs_rc.ArgError;
    if( whefs_rc.OK == rc )
    {
        whefs_fs_closer_file_remove( f->fs, f );
	if( WHEFS_FILE_ISRW(f) && WHEFS_FILE_ISOPENED(f) ) whefs_fs_flush( f->fs );
	if( f->dev ) f->dev->api->finalize(f->dev);
	whefs_file_free(f);
    }
    return rc;
}

int whefs_fflush( whefs_file * restrict f )
{
    if( ! f ) return whefs_rc.ArgError;
    else
    {
	if( WHEFS_FILE_ISOPENED(f) && WHEFS_FILE_ISRW(f) )
        {
            return f->dev->api->flush(f->dev);
        }
        return whefs_rc.OK;
    }
}

int whefs_dev_close( whio_dev * restrict dev )
{
    int rc = dev ? whefs_rc.OK : whefs_rc.ArgError;
    if( whefs_rc.OK == rc )
    {
	dev->api->finalize( dev );
    }
    return rc;
}

size_t whefs_fread( whefs_file * restrict f, size_t size, size_t count, void * dest )
{
    if( ! f || !count || (count && !dest) || !size || !f->dev ) return 0;
    size_t x = 0;
    for( ; x < count; ++x )
    {
	size_t rsz = f->dev->api->read( f->dev, WHIO_VOID_PTR_ADD(dest,(x*size)), size );
	if( size != rsz ) break;
    }
    return x;
}

size_t whefs_fwrite( whefs_file * restrict f, size_t sz, size_t count, void const * src )
{
    if( ! f || !count || (count && !src) || !sz || !f->dev ) return 0;
    size_t x = 0;
    for( ; x < count; ++x )
    {
	size_t wsz = f->dev->api->write( f->dev, WHIO_VOID_CPTR_ADD(src,(x*sz)), sz );
	if( sz != wsz ) break;
    }
    //if( x ) f->dev->api->flush(f->dev);
    return x;
}

size_t whefs_fwritev( whefs_file * restrict f, char const * fmt, va_list vargs )
{
    return f ? whio_dev_writefv( f->dev, fmt, vargs ) : 0;
}

size_t whefs_fwritef( whefs_file * restrict f, char const * fmt, ... )
{
    size_t rc;
    va_list vargs;
    va_start(vargs,fmt);
    rc = whefs_fwritev( f, fmt, vargs );
    va_end(vargs);
    return rc;
}

size_t whefs_file_write( whefs_file * restrict f, void const * src, size_t n  )
{
    return (f && f->dev)
	? f->dev->api->write( f->dev, src, n )
	: 0;
}

size_t whefs_file_read( whefs_file * restrict f, void * dest, size_t n  )
{
    return (f && f->dev)
	? f->dev->api->read( f->dev, dest, n )
	: 0;
}

#if 0
not sure about this one;
int whefs_unlink_file( whefs_file * restrict )
{
    if( ! f ) return whefs_rc.ArgError;
    f->dev->api->finalize(f->dev);
    f->dev = 0;
    whefs_inode ino = whefs_inode_init;
    ino.id = f->inode;
    whefs_inode_read( f->fs, &ino );
    int rc = whefs_inode_unlink( f->fs, &ino );
    whefs_fclose( f );
    return rc;
}
#endif

int whefs_unlink_filename( whefs_fs * fs, char const * fname )
{
    if( ! fs || !fname ) return whefs_rc.ArgError;
    whefs_inode ino = whefs_inode_init;
    int rc = whefs_inode_by_name( fs, (char const *) /* FIXME: signedness*/ fname, &ino );
    if( whefs_rc.OK == rc )
    {
	rc = whefs_inode_unlink( fs, &ino );
    }
    return rc;
}

/**
   An empty whefs_file_stats object which can (and should)
   be used to initialize whefs_file_stats objects to a zeroed
   state. Relying on compiler default values is a bad idea
   (been there, done that).
*/
static const whefs_file_stats whefs_file_stats_init =
    {
    0, /* bytes */
    0, /* inode */
    0 /* blocks */
    };

int whefs_fstat( whefs_file const * f, whefs_file_stats * st )
{
    if( ! f || !st ) return whefs_rc.ArgError;
    *st = whefs_file_stats_init;
    st->inode = f->inode;
    whefs_inode ino = whefs_inode_init;
    ino.id = f->inode;
    int rc = whefs_inode_read( f->fs, &ino );
    if( whefs_rc.OK != rc ) return rc;
    st->bytes = ino.data_size;
    whefs_id_type bid = ino.first_block;
    whefs_block bl = whefs_block_init;
    bl.id = bid;
    while( bl.id )
    {
	++st->blocks;
	rc = whefs_block_read( f->fs, bl.id, &bl );
	if( whefs_rc.OK != rc ) return rc;
	bl.id = bl.next_block;
    }
    return rc;
}

int whefs_file_name_set( whefs_file * restrict f, char const * newName )
{
    if( ! f || (! newName || !*newName) ) return whefs_rc.ArgError;
    if( ! WHEFS_FILE_ISRW(f) ) return whefs_rc.AccessError;
    whefs_inode * ino = 0;
    int rc = whefs_inode_search_opened( f->fs, f->inode, &ino );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("This should never ever happen: f appears to be a valid whefs_file, but we could find no associated opened inode!");
	return rc;
    }
    rc = whefs_inode_name_set( f->fs, ino->id, newName );
    if( whefs_rc.OK != rc ) return rc;
    //whefs_inode_flush( f->fs, ino );
    return whefs_rc.OK;
}

char const * whefs_file_name_get( whefs_file * restrict f )
{
#if 0
    if( ! f ) return 0;
    whefs_inode * ino = 0;
    int rc = whefs_inode_search_opened( f->fs, f->inode, &ino );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("This should never ever happen: f appears to be a "
		      "whefs_file, but we could find no associated opened inode!");
	return 0;
    }
    return ino->name.string;
#else
    int rc =whefs_inode_name_get( f->fs, f->inode, &f->name );
    if( whefs_rc.OK != rc )
    {
	WHEFS_DBG_ERR("This should never ever happen: f appears to be a "
		      "whefs_file, but we could find no associated opened inode!");
	return 0;
    }
    return f->name.string;
#endif
}

whio_size_t whefs_fsize( whefs_file const * restrict f )
{
#if 1
    if( ! f ) return whefs_rc.SizeTError;
    whefs_inode * ino = 0;
    int rc = whefs_inode_search_opened( f->fs, f->inode, &ino );
    return (whefs_rc.OK == rc)
	? ino->data_size
	: whefs_rc.SizeTError;
#else /* faster, but not technically const */
    return (f && f->dev)
	? whio_dev_size( f->dev )
	: whefs_rc.SizeTError;
#endif
}
/* end file src/whefs_file.c */
/* begin file src/whefs_cache.c */
#line 8 "src/whefs_cache.c"
/**
  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/

  License: Public Domain

  This file contiains most of the name-cache-related functionality.
*/

#include <stdlib.h> // free()
#include <string.h> // memset()
#include <assert.h>

int whefs_inode_hash_cache_load( whefs_fs * fs )
{
    if( ! WHEFS_FS_HASH_CACHE_IS_ENABLED(fs) ) return whefs_rc.OK;
    if( ! fs->cache.hashes )
    {
        whefs_id_type toAlloc = 16 /* arbitrary */;
        if( toAlloc > fs->options.inode_count ) toAlloc = fs->options.inode_count;
        whefs_hashid_list_alloc( &fs->cache.hashes, toAlloc );
    }
    if( ! fs->cache.hashes ) return whefs_rc.AllocError;
    else if( !fs->cache.hashes->maxAlloc )
    {
        fs->cache.hashes->maxAlloc = fs->options.inode_count;
    }

    //whefs_hashid h = whefs_hashid_init;
    whefs_string name = whefs_string_init;
    enum { bufSize = WHEFS_MAX_FILENAME_LENGTH+1 };
    unsigned char buf[bufSize];
    memset( buf, 0, bufSize );
    // ensure that whefs_inode_name_get() won't malloc():
    name.string = (char *)buf;
    name.alloced = bufSize;
    name.length = 0;
    int rc = 0;
    whefs_id_type i;
    whefs_id_type count = 0;
    fs->cache.hashes->skipAutoSort = true; // optimization to avoid extra sorting via whefs_inode_name_get()
    for( i = fs->options.inode_count; i >=1 ; --i )
    {
        /**
           Maintenance reminder:

           We do this loop in reverse order as an efficiency hack for
           whefs_string_cache. The trick is: the whefs_string_cache's
           internal buffer grows only as the number of used inodes
           does (it's size is a function of the highest used inode
           ID). If we insert from low to high it may realloc many
           times. If we insert from high to low, we're guaranteed to
           need only one malloc/realloc on it.
        */
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
        if( fs->bits.i_loaded )
	{
            if( ! WHEFS_ICACHE_IS_USED(fs,i) )
            {
                continue;
            }
	}
#endif // WHEFS_CONFIG_ENABLE_BITSET_CACHE
        rc = whefs_inode_name_get( fs, i, &name ); // this caches the name hash
        assert( (name.string == (char *)buf) && "Internal memory management foo-foo." );
        if( whefs_rc.OK != rc ) break;
        ++count;
    }
    fs->cache.hashes->skipAutoSort = false;
    whefs_hashid_list_sort(fs->cache.hashes);
    WHEFS_DBG_CACHE("Loaded all used inodes into cache with %"WHEFS_ID_TYPE_PFMT" name(s).",count);
    return rc;
}
int whefs_inode_hash_cache_chomp_lv( whefs_fs * fs )
{
    if( ! fs ) return whefs_rc.ArgError;
    if( ! fs->cache.hashes || !fs->cache.hashes->count ) return whefs_rc.OK;
    return whefs_hashid_list_chomp_lv( fs->cache.hashes );
}

whefs_id_type whefs_inode_hash_cache_search_ndx(whefs_fs * fs, char const * name )
{
    if( ! WHEFS_FS_HASH_CACHE_IS_ENABLED(fs) ) return whefs_rc.IDTypeEnd;
    if( fs->cache.hashes && !fs->cache.hashes->isSorted )
    {
        WHEFS_DBG_CACHE("Achtung: auto-sorting dirty name cache before search starts.");
        whefs_inode_hash_cache_sort(fs);
    }
    return ( ! fs->cache.hashes )
        ? whefs_rc.IDTypeEnd
        : whefs_hashid_list_index_of( fs->cache.hashes, fs->cache.hashfunc(name) );
}

whefs_id_type whefs_inode_hash_cache_search_id(whefs_fs * fs, char const * name )
{
    if( ! WHEFS_FS_HASH_CACHE_IS_ENABLED(fs) ) return 0;
    if( ! fs->cache.hashes ) return 0;
    whefs_id_type n = whefs_inode_hash_cache_search_ndx( fs, name );
    return ( n == whefs_rc.IDTypeEnd )
        ? 0
        : fs->cache.hashes->list[n].id;
}



void whefs_inode_hash_cache_sort(whefs_fs * fs )
{
    if( fs->cache.hashes && ! fs->cache.hashes->isSorted )
    {
        whefs_hashid_list_sort( fs->cache.hashes );
    }
}


void whefs_inode_name_uncache(whefs_fs * fs, char const * name )
{
    if( !fs->cache.hashes || ! name || !*name  ) return;
    const whefs_id_type ndx = whefs_inode_hash_cache_search_ndx( fs, name );
    if( whefs_rc.IDTypeEnd != ndx )
    {
        fs->cache.hashes->list[ndx] = whefs_hashid_init;
        fs->cache.hashes->isSorted = false;
    }
}

int whefs_inode_hash_cache( whefs_fs * fs, whefs_id_type id, char const * name )
{
    if( ! WHEFS_FS_HASH_CACHE_IS_ENABLED(fs) )
    {
        return whefs_rc.OK;
    }
    if( ! fs || !name || !*name ) return whefs_rc.ArgError;
    int rc = whefs_rc.OK;
    if( ! fs->cache.hashes )
    {
        const whefs_id_type max = fs->options.inode_count;
        whefs_id_type dflt = 100/sizeof(whefs_hashid) /*for lack of a better default value.*/;
        if( dflt > max ) dflt = max;
        rc = whefs_hashid_list_alloc( &fs->cache.hashes, dflt );
        if( fs->cache.hashes )
        {
            fs->cache.hashes->maxAlloc = max;
        }
    }
    if( whefs_rc.OK != rc ) return rc;
    whefs_hashval_type h = fs->cache.hashfunc( name );
#if 1
    const whefs_id_type ndx = whefs_hashid_list_index_of( fs->cache.hashes, h );
    if( whefs_rc.IDTypeEnd != ndx )
    {
        if(0) WHEFS_DBG("CHECKING: name cache count[%"WHEFS_ID_TYPE_PFMT"], alloced=[%"WHEFS_ID_TYPE_PFMT"], hash [%"WHEFS_HASHVAL_TYPE_PFMT"] for name [%s], ndx=[%"WHEFS_ID_TYPE_PFMT"]",
                        fs->cache.hashes->count, fs->cache.hashes->alloced, h, name, ndx );
        if( fs->cache.hashes->list[ndx].id == id ) return whefs_rc.OK;
        WHEFS_DBG_ERR("ERROR: name cache hash collision on hash code "
                      "%"WHEFS_HASHVAL_TYPE_PFMT" "
                      "between inodes #"
                      "%"WHEFS_ID_TYPE_PFMT
                      " and #%"WHEFS_ID_TYPE_PFMT"!",
                      h, id, ndx );
        return whefs_rc.InternalError;
    }
    WHEFS_DBG_CACHE("ADDING: name cache count[%"WHEFS_ID_TYPE_PFMT"], alloced=[%"WHEFS_ID_TYPE_PFMT"], hash [%"WHEFS_HASHVAL_TYPE_PFMT"] for name [%s], ndx=[%"WHEFS_ID_TYPE_PFMT"]",
                          fs->cache.hashes->count, fs->cache.hashes->alloced, h, name, ndx );
#endif
    whefs_hashid H = whefs_hashid_init;
    H.hash = h;
    H.id = id;
    rc = whefs_hashid_list_add( fs->cache.hashes, &H );
    WHEFS_DBG_CACHE("Added to name cache: hash[%"WHEFS_HASHVAL_TYPE_PFMT"]=id[%"WHEFS_ID_TYPE_PFMT"], name=[%s], rc=%d", H.hash, H.id, name, rc );
    return whefs_rc.OK;
}


/* end file src/whefs_cache.c */
/* begin file src/whefs_fs_closer.c */
#line 8 "src/whefs_fs_closer.c"
/**
   Implementations for the whefs_fs_closer_list-related API.
*/
#include <stdlib.h> /* malloc() and co. */

const whefs_fs_closer_list whefs_fs_closer_list_init = whefs_fs_closer_list_init_m;


/**
   Unlinks x from its neighbors.
*/
static void whefs_fs_closer_list_unlink( whefs_fs_closer_list * x )
{
    if( x->prev ) { x->prev->next = x->next; x->prev = 0; }
    if( x->next ) { x->next->prev = x->prev; x->next = 0; }
}


#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whefs_fs_closer_list_alloc_slots object.
*/
whefs_fs_closer_list_alloc_count = 4
};
static struct
{
    whefs_fs_closer_list objs[whefs_fs_closer_list_alloc_count];
    char used[whefs_fs_closer_list_alloc_count];
    size_t next;
    const size_t count;
} whefs_fs_closer_list_alloc_slots = { { whefs_fs_closer_list_init_m }, {0}, 0, whefs_fs_closer_list_alloc_count };
#endif
/**
   Allocates a new zero-initialized object. Ownership is passed
   to the caller.
*/
whefs_fs_closer_list * whefs_fs_closer_list_alloc()
{
    whefs_fs_closer_list * obj = 0;
#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
    size_t i = whefs_fs_closer_list_alloc_slots.next;
    for( ; i < whefs_fs_closer_list_alloc_slots.count; ++i )
    {
	if( whefs_fs_closer_list_alloc_slots.used[i] ) continue;
	whefs_fs_closer_list_alloc_slots.used[i] = 1;
	whefs_fs_closer_list_alloc_slots.next = i+1;
	obj = &whefs_fs_closer_list_alloc_slots.objs[i];
	break;
    }
#endif /* WHEFS_CONFIG_ENABLE_STATIC_MALLOC */
    if( ! obj ) obj = (whefs_fs_closer_list *) malloc( sizeof(whefs_fs_closer_list) );
    if( obj ) *obj = whefs_fs_closer_list_init;
    return obj;
}

/**
   Unlinks obj from its neighbors and deallocates it. Ownership of
   obj is transfered to this function.
*/
void whefs_fs_closer_list_free( whefs_fs_closer_list * obj )
{
    if( ! obj ) return;
#if WHEFS_CONFIG_ENABLE_STATIC_MALLOC
    if( (obj < &whefs_fs_closer_list_alloc_slots.objs[0]) ||
	(obj > &whefs_fs_closer_list_alloc_slots.objs[whefs_fs_closer_list_alloc_slots.count-1]) )
    { /* it does not belong to us */
        whefs_fs_closer_list_unlink( obj );
        *obj = whefs_fs_closer_list_init;
	free(obj);
	return;
    }
    else
    {
        whefs_fs_closer_list_unlink( obj );
        *obj = whefs_fs_closer_list_init;
	const size_t ndx = (obj - &whefs_fs_closer_list_alloc_slots.objs[0]);
	whefs_fs_closer_list_alloc_slots.used[ndx] = 0;
	if( ndx < whefs_fs_closer_list_alloc_slots.next ) whefs_fs_closer_list_alloc_slots.next = ndx;
	return;
    }
#else
    whefs_fs_closer_list_unlink( obj );
    *obj = whefs_fs_closer_list_init;
    free(obj);
#endif /* WHEFS_CONFIG_ENABLE_STATIC_MALLOC */
}


int whefs_fs_closer_list_close( whefs_fs_closer_list * head, bool right )
{
    if( ! head ) return whefs_rc.ArgError;
    int rc = whefs_rc.OK;
    whefs_fs_closer_list * x = head;
    for( ; x; head = x )
    {
        x = head->next;
        switch( head->type )
        {
          case WHEFS_CLOSER_TYPE_FILE:
              whefs_fs_closer_list_unlink(head);
              whefs_fclose( head->item.file );
              break;
          case WHEFS_CLOSER_TYPE_DEV:
              whefs_fs_closer_list_unlink(head);
              head->item.dev->api->finalize( head->item.dev );
              break;
          case WHEFS_CLOSER_TYPE_STREAM:
              whefs_fs_closer_list_unlink(head);
              head->item.stream->api->finalize( head->item.stream );
              break;
          default:
              WHEFS_DBG_ERR("Internal error whefs_fs_closer_list entry does not have a supported type field. Possibly leaking an object here!");
              rc = whefs_rc.InternalError;
              break;
        };
        whefs_fs_closer_list_free(head);
        if( ! right ) break;
    }
    return rc;
}

/**
   Removes the given object from fs->closers. fs->closers must
   contain an entry where entry->type==type and entry->item.XXX==obj,
   where XXX is dependent on entry->type.

   Returns whefs_rc.OK on sucess.
*/
static int whefs_fs_closer_remove( whefs_fs * fs, char type, void const * obj )
{
    if( ! fs || ! obj ) return whefs_rc.ArgError;
    whefs_fs_closer_list * li = fs->closers;
    if( ! li ) return whefs_rc.RangeError;
    while( li->prev ) li = li->prev;
    int rc = whefs_rc.OK;
    bool foundOne = false;
    for( ; li ; li = li->next )
    {
        if( li->type != type ) continue;
        switch( type )
        {
          case WHEFS_CLOSER_TYPE_FILE:
              if( (void const *)li->item.file != obj ) continue;
              foundOne = true;
              break;
          case WHEFS_CLOSER_TYPE_DEV:
              if( (void const *)li->item.dev != obj ) continue;
              foundOne = true;
              break;
          case WHEFS_CLOSER_TYPE_STREAM:
              if( (void const *)li->item.stream != obj ) continue;
              foundOne = true;
              break;
          default:
              WHEFS_DBG_ERR("Internal error whefs_fs_closer_list entry does not have a supported type field.");
              rc = whefs_rc.InternalError;
              break;
        };
        if( foundOne )
        {
            if( fs->closers == li )
            {
                fs->closers = li->prev ? li->prev : li->next;
            }
            whefs_fs_closer_list_free( li );
            li = 0;
            break;
        }
    }
    return rc;
}


/**
   Creates a new, empty whefs_fs_closer_list object and
   appends it to fs->closers.
*/
static whefs_fs_closer_list * whefs_fs_closer_add( whefs_fs * fs )
{
    if( ! fs ) return NULL;
    whefs_fs_closer_list * li = whefs_fs_closer_list_alloc();
    if( ! li ) return NULL;
    if( ! fs->closers )
    {
        fs->closers = li;
    }
    else
    {
        whefs_fs_closer_list * tail = fs->closers;
        while( tail->next ) tail = tail->next;
        tail->next = li;
        li->prev = tail;
    }
    return li;
}

int whefs_fs_closer_file_add( whefs_fs * fs, whefs_file * f )
{
    if( ! fs || ! f ) return whefs_rc.ArgError;
    whefs_fs_closer_list * li = fs->closers;
    if( li )
    { /** If we find f->dev in the list then we
          promote the entry to type WHEFS_CLOSER_TYPE_FILE.
      */
        while( li->prev ) li = li->prev;
        for( ; li ; li = li->next )
        {
            if( li->type != WHEFS_CLOSER_TYPE_DEV ) continue;
            if( li->item.dev != f->dev ) continue;
            li->type = WHEFS_CLOSER_TYPE_FILE;
            li->item.file = f;
            break;
        }
    }
    else
    {
        whefs_fs_closer_list * li = whefs_fs_closer_add(fs);
        if( ! li ) return whefs_rc.AllocError /* that's a guess */;
        li->type = WHEFS_CLOSER_TYPE_FILE;
        li->item.file = f;
    }
    return whefs_rc.OK;
}

int whefs_fs_closer_file_remove( whefs_fs * fs, whefs_file const * f )
{
    return whefs_fs_closer_remove( fs, WHEFS_CLOSER_TYPE_FILE, f );
}

int whefs_fs_closer_dev_add( whefs_fs * fs, whio_dev * d )
{
    if( ! fs || ! d ) return whefs_rc.ArgError;
    whefs_fs_closer_list * li = whefs_fs_closer_add(fs);
    if( ! li ) return whefs_rc.AllocError /* that's a guess */;
    li->type = WHEFS_CLOSER_TYPE_DEV;
    li->item.dev = d;
    return whefs_rc.OK;
}

int whefs_fs_closer_dev_remove( whefs_fs * fs, whio_dev const * d )
{
    return whefs_fs_closer_remove( fs, WHEFS_CLOSER_TYPE_DEV, d );
}

int whefs_fs_closer_stream_add( whefs_fs * fs, whio_stream * s, whio_dev const * d )
{
    if( ! fs || ! s || !d ) return whefs_rc.ArgError;
    whefs_fs_closer_list * li = fs->closers;
    if( li )
    { /** If we find f->dev in the list then we
          promote the entry to type WHEFS_CLOSER_TYPE_STREAM.
      */
        while( li->prev ) li = li->prev;
        for( ; li ; li = li->next )
        {
            if( li->type != WHEFS_CLOSER_TYPE_DEV ) continue;
            if( d != li->item.dev ) continue;
            li->type = WHEFS_CLOSER_TYPE_STREAM;
            li->item.stream = s;
            break;
        }
        if( ! li ) return whefs_rc.RangeError;
    }
    else
    { /* re-evaluate this. Is this sane? */
        whefs_fs_closer_list * li = whefs_fs_closer_add(fs);
        if( ! li ) return whefs_rc.AllocError /* that's a guess */;
        li->type = WHEFS_CLOSER_TYPE_STREAM;
        li->item.stream = s;
    }
    return whefs_rc.OK;
}

int whefs_fs_closer_stream_remove( whefs_fs * fs, whio_stream const * s )
{
    return whefs_fs_closer_remove( fs, WHEFS_CLOSER_TYPE_STREAM, s );
}
/* end file src/whefs_fs_closer.c */
/* begin file src/whbits.c */
#line 8 "src/whbits.c"
#include <stddef.h> /* size_t on my box */
#include <string.h> /* memset() */
#include <stdlib.h> /* malloc()/free() */

const whbits whbits_init_obj = WHBITS_INIT;

int whbits_init( whbits * b, whbits_count_t bitCount, unsigned char initialState )
{
    if( ! bitCount || !b ) return -1;
    const size_t lenB = (bitCount / 8) + ((bitCount%8) ? 1 : 0);
    if( b->sz_alloc >= lenB )
    { /* re-use the memory */
        //memset( b->bytes+b->sz_bytes, initialState, b->sz_bytes - lenB );
        if( b->sz_bytes == lenB )
        {
            b->sz_bits = bitCount;
            return 0;
        }
        else if( b->sz_bytes < lenB )
        {
            memset( b->bytes+b->sz_bytes, initialState, b->sz_alloc - b->sz_bytes );
            b->sz_bytes = lenB;
        }
        else
        {
            memset( b->bytes+lenB, initialState, b->sz_alloc - lenB );
            b->sz_bytes = lenB;
        }
        b->sz_bits = bitCount;
        return 0;
    }
    whbits old = *b;
    unsigned char * x = realloc( b->bytes, lenB );
    if( ! x )
    {
        return -1;
    }
    b->bytes = x;
    b->sz_alloc = lenB;
    b->sz_bytes = lenB;
    b->sz_bits = bitCount;
    memset( b->bytes + old.sz_alloc, initialState, b->sz_alloc - old.sz_alloc );
    return 0;
}

void whbits_free_bits( whbits * b )
{
    if( b )
    {
	if( b->bytes )
	{
	    free(b->bytes);
	}
	*b = whbits_init_obj;
    }
}

size_t whbits_size_bits( whbits const * b )
{
    return b ? b->sz_bits : 0;
}

size_t whbits_size_bytes( whbits const * b )
{
    return b ? b->sz_bytes : 0;
}

char whbits_set( whbits* b, whbits_count_t bitNum )
{
    if( ! b || (bitNum  > b->sz_bits) ) return 0;
    WHBITS_SET( b, bitNum );
    return 1;
}

char whbits_unset( whbits * b, whbits_count_t bitNum )
{
    if( ! b || (bitNum  > b->sz_bits) ) return 0;
    WHBITS_UNSET( b, bitNum);
    return 1;
}

char whbits_toggle( whbits const * b, whbits_count_t bitNum )
{
    if( ! b || (bitNum  > b->sz_bits) ) return -1;
    return (char) WHBITS_TOGGLE(b,bitNum);
}

char whbits_get( whbits const * b, whbits_count_t bitNum )
{
    if( ! b || (bitNum  > b->sz_bits) ) return 0;
    return WHBITS_GET(b,bitNum);
}
/* end file src/whbits.c */
/* begin file src/whdbg.c */
#line 8 "src/whdbg.c"

#include <stdlib.h>


#ifdef __cplusplus
#define ARG_UNUSED(X)
extern "C" {
#else
#define ARG_UNUSED(X) X
#endif

static int whdbg_flags = WHDBG_DEFAULT;
static FILE * whdbg_stream = 0;

void whdbg_set_stream( FILE * f )
{
    whdbg_stream = f;
}

unsigned int whdbg_get_flags()
{
    return whdbg_flags;
}
unsigned int whdbg_set_flags(unsigned int newflags)
{
    unsigned int x = whdbg_flags;
    whdbg_flags = newflags;
    return x;
}

/**
   "Automatic" arguments for whdbgv() and friends. Names of
   elements must match those defined in WHDBG_SOURCE_INFO_PARAMS_DECL.
*/
#define WHDBG_SOURCE_INFO_PARAMS_PASS _file,_line,_func

struct whdbg_PRIVATE_t
{
    char const * filename;
    char const * funcname;
    unsigned int lineno;
    unsigned int condition;
    char const * condString;
};
struct whdbg_PRIVATE_t whdbg_PRIVATE_init = {0,0,0,0,0};
struct whdbg_PRIVATE_t whdbg_PRIVATE = {0,0,0,0,0};


void whdbgv( unsigned int condition,
		WHDBG_SOURCE_INFO_PARAMS_DECL,
		char const * fmt,
		va_list vargs )
{
#if WHDBG_CONFIG_ENABLE
    if( whdbg_stream
	&& ((whdbg_flags & condition) || (WHDBG_ALWAYS == whdbg_flags) || (WHDBG_ALWAYS == condition))
	&& fmt )
    {
	long rc = 0;
#define VAPARGS vappendf_FILE_appender, whdbg_stream
	if( whdbg_PRIVATE.condString )
	{
	    rc = fprintf( whdbg_stream,"whdbg(%s):%s:%d:%s():\n\t",whdbg_PRIVATE.condString,WHDBG_SOURCE_INFO_PARAMS_PASS);
	    whdbg_PRIVATE.condString = 0;
	}
	else
	{
	    rc = fprintf( whdbg_stream,"whdbg(0x%08x):%s:%d:%s():\n\t",condition,WHDBG_SOURCE_INFO_PARAMS_PASS);
	}
	if( rc >= 0 ) rc = vfprintf( whdbg_stream, fmt, vargs );
	fputc('\n',whdbg_stream);
	fflush(whdbg_stream);
#undef VAPARGS
    }
#endif // WHDBG_CONFIG_ENABLE
}

void whdbg(  unsigned int condition,
		WHDBG_SOURCE_INFO_PARAMS_DECL,
		char const * fmt,
		... )
{
#if WHDBG_CONFIG_ENABLE
    if( whdbg_stream
	&& ((WHDBG_ALWAYS == whdbg_flags) || (whdbg_flags & condition))
	&& fmt )
    {
	va_list vargs;
	va_start( vargs, fmt );
	whdbgv( condition, WHDBG_SOURCE_INFO_PARAMS_PASS, fmt, vargs );
	va_end(vargs);
    }
#endif
}

static void whdbg_PRIVATE_phase2( char const * fmt, ... )
{
#if WHDBG_CONFIG_ENABLE
	va_list vargs;
	va_start( vargs, fmt );
	whdbgv( whdbg_PRIVATE.condition,
		   whdbg_PRIVATE.filename,
		   whdbg_PRIVATE.lineno,
		   whdbg_PRIVATE.funcname,
		   fmt, vargs );
	va_end(vargs);
	whdbg_PRIVATE = whdbg_PRIVATE_init;
#endif
}

whdbg_PRIVATE_f whdbg_PRIVATE_phase1( unsigned int condition,
					    char const * condString,
					    char const * file,
					    unsigned int line,
					    char const *  funcName )
{
#if WHDBG_CONFIG_ENABLE
    whdbg_PRIVATE.condition = condition;
    whdbg_PRIVATE.condString = condString;
    whdbg_PRIVATE.filename = file;
    whdbg_PRIVATE.lineno = line;
    whdbg_PRIVATE.funcname = funcName;
#endif
    return whdbg_PRIVATE_phase2;
}


#ifdef __cplusplus
} /* extern "C" */
#endif
/* end file src/whdbg.c */
/* begin file src/whglob.c */
#line 8 "src/whglob.c"
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

int whglob_matches_like( char const * str, char const * pattern, char caseSensitive )
{
    /* The correct SQL-92 behavior is for the LIKE operator to ignore
    ** case.  Thus  'a' LIKE 'A' would be true. */
    static const sqlite3CompareInfo likeInfoNorm = { '%', '_',   0, 1 };
    /* If SQLITE_CASE_SENSITIVE_LIKE is defined, then the LIKE operator
    ** is case sensitive causing 'a' LIKE 'A' to be false */
    static const sqlite3CompareInfo likeInfoAlt = { '%', '_',   0, 0 };
    return patternCompare( (unsigned char *)pattern, (unsigned char *) str,
			   caseSensitive ? &likeInfoNorm : &likeInfoAlt,
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
/* end file src/whglob.c */
/* begin file src/whefs_client_util.c */
#line 8 "src/whefs_client_util.c"
#include <string.h> // memset()

int whefs_test_insert_dummy_files( whefs_fs * fs )
{
    if( ! fs || ! fs->dev ) return whefs_rc.ArgError;
    int rc = whefs_rc.OK;
    whefs_file * F = 0;
    whefs_file_stats st;
    size_t nid = 0;
    char const * fname = "test_file_number_1";
    size_t szrc = 0;

#define INSERT(FN) \
    fname = FN; \
    F = whefs_fopen( fs, FN, "r+" );	   \
    assert( F && "whefs_fopen() failed!"); \
    nid = F->inode; \
    szrc = whefs_fwrite( F, 3, 1, "hi!" );				\
    WHEFS_DBG("fwrite result=%u",szrc); \
    rc = whefs_fstat( F, &st ); \
    WHEFS_DBG("fstat rc=%d\t F->inode = %u\tF->bytes = %u\tF->blocks = %u", rc, st.inode, st.bytes, st.blocks ); \
    rc = whefs_fclose( F ); \
    assert( (whefs_rc.OK == rc) && "whefs_fclose() failed!" ); \
    F = 0;

    INSERT("test_file_number_1");
    INSERT("test_file_number_2");
#undef INSERT

    whefs_inode ino;
    whio_dev * dev = 0;
#if 1
    whefs_inode_id_read( fs, nid, &ino );
    whefs_inode_name_set( fs, ino.id, "will be renamed");
    //whefs_inode_save( fs, &ino );
    dev = whefs_dev_for_inode( fs, ino.id, true );
    WHEFS_DBG("dev=%p",(void const *)dev);
    assert( dev && "opening of device for inode failed!" );
    nid = dev->api->write( dev, "Watson, come here!", 18 );
    WHEFS_DBG("write rc=%u", nid );
    nid = dev->api->write( dev, "\nComing, Alexander!", 19 );
    WHEFS_DBG("write rc=%u", nid );
    dev->api->finalize(dev);
#endif

#if 1
    //whefs_inode_next_free( fs, &ino, true );
    //ino = whefs_inode_init; ino.id = nid;
    whefs_inode_read( fs, &ino );
    WHEFS_DBG("Trampling over inode #%u", ino.id );
    fname = "via whio_dev_inode";
    whefs_inode_name_set( fs, ino.id, fname );
    dev = whefs_dev_for_inode( fs, ino.id, true );
    WHEFS_DBG("dev=%p",(void const *)dev);
    assert( dev && "couldn't re-open device!");
    dev->api->flush(dev);
    assert( dev && "opening of device for inode failed!" );
    size_t i = 0;
    size_t total = 0;
    for( ; i < 10; ++i )
    {
	nid = whio_dev_writef( dev, "Test #%02u", i );
	total += nid;
    }
    WHEFS_DBG("dev size=%u", whio_dev_size(dev) );
    dev->api->truncate( dev, 0 );
    dev->api->seek( dev, 30, SEEK_SET );
    WHEFS_DBG("dev size=%u", whio_dev_size(dev) );
    nid = dev->api->write( dev, "Stop saying that!!", 18 );
    total += nid;
    WHEFS_DBG("dev size=%u", whio_dev_size(dev) );
    dev->api->finalize(dev);

    F = whefs_fopen( fs, fname, "r+" );
    assert( F && "re-open of inode failed!" );
    dev = whefs_fdev( F );
    char const * str = "...And now a final word.";
    size_t slen = strlen(str);
    size_t dsize = whefs_fseek( F, 0, SEEK_END );
    WHEFS_DBG("F size=%u, slen=%u", dsize, slen );
    nid = whefs_fwrite( F, slen, 1, str );
    //WHEFS_DBG("whefs_fwrite() rc=%u", nid );
    assert( (1 == nid) && "write failed!" );
    dev->api->seek( dev, dsize + slen + 10, SEEK_SET );
    dev->api->write( dev, "!", 1 );
    //nid = whefs_fwrite( F, 1, 1, "!" );
    WHEFS_DBG("dev size=%u", whio_dev_size(dev) );
    whefs_fclose( F );
#endif

    return rc;
}

int whefs_fs_stats_get( whefs_fs * fs, whefs_fs_stats * st )
{
    if( ! fs || ! st ) return whefs_rc.ArgError;
    WHEFS_DBG_ERR("NYI!");

    st->size = fs->filesize;
    // FIXME: calculate used nodes.
    st->used_inodes = 1; /* root node is always considered used. */
    // FIXME: calculate used blocks
    st->used_blocks = 0;
    // FIXME: calculate used bytes
    st->used_bytes = 0;

    return whefs_rc.OK;
}



int whefs_fs_dump_to_FILE( whefs_fs * fs, FILE * out )
{
    if( ! fs || !out || !fs->dev ) return whefs_rc.ArgError;
    int rc = whefs_rc.OK;
    enum { bufSize = (1024 * 4) };
    unsigned char buf[bufSize];
    size_t rlen = 0;
    fs->dev->api->seek( fs->dev, 0L, SEEK_SET );
    while( (rlen = fs->dev->api->read( fs->dev, buf, bufSize ) ) )
	//#undef bufSize
    {
	if( 1 != fwrite( buf, rlen, 1, out ) )
	{
	    rc = whefs_rc.IOError;
	    break;
	}
    }
    return rc;
}

int whefs_import_dev( whefs_fs * fs, whio_dev * src, char const * fname, bool overwrite )
{
    if( ! fs || !src || !fname || !*fname ) return whefs_rc.ArgError;
    int rc = whefs_rc.OK;
    whefs_inode ino = whefs_inode_init;
    bool existed = false;
    rc = whefs_inode_by_name( fs, fname, &ino );
    if( rc == whefs_rc.OK )
    {
	if( ! overwrite ) return whefs_rc.AccessError;
	existed = true;
    }
    else
    {
	rc = whefs_inode_next_free( fs, &ino, true );
	if( whefs_rc.OK != rc ) return rc;
	rc = whefs_inode_name_set( fs, ino.id, fname );
	if( whefs_rc.OK != rc ) return rc;
	whefs_inode_flush( fs, &ino );
    }
    whio_dev * imp = whefs_dev_for_inode( fs, ino.id, true );
    if( ! imp ) return whefs_rc.InternalError;

    const whio_size_t oldPos = src->api->tell( src );
    const whio_size_t szrc = src->api->seek( src, 0, SEEK_SET );
    if( whio_rc.SizeTError == szrc )
    {
	imp->api->finalize( imp );
	if( ! existed )
	{
	    whefs_inode_unlink( fs, &ino );
	}
	return whefs_rc.RangeError;
    }

    const size_t oldFSize = ino.data_size;
    const size_t newFSize = whio_dev_size( src );
    rc = imp->api->truncate( imp, newFSize );
    if( rc != whefs_rc.OK )
    {
	WHEFS_DBG_ERR("Could not truncate '%s' to %u bytes!", fname, newFSize );
	imp->api->truncate( imp, oldFSize );
	src->api->seek( src, oldPos, SEEK_SET );
	imp->api->finalize( imp );
	if( ! existed )
	{
	    whefs_inode_unlink( fs, &ino );
	}
	return rc;
    }
    imp->api->flush( imp );
    if( 0 == newFSize )
    {
	//WHEFS_DBG("Importing 0-byte file.");
	src->api->seek( src, oldPos, SEEK_SET );
	imp->api->finalize(imp);
	return whefs_rc.OK;
    }
    enum { bufSize = 1024 * 8 };
    unsigned char buf[bufSize];
    memset( buf, 0, bufSize );
    size_t wlen = newFSize;
    size_t totalR = 0;
    size_t totalW = 0;
    size_t rrc = 0;
    size_t wrc = 0;
    imp->api->seek( imp, 0, SEEK_SET );
    do
    {
	rrc = src->api->read( src, buf, bufSize );
	totalR += rrc;
	if( ! rrc ) break;
	if( (wlen - rrc) > wlen )
	{
	    WHEFS_DBG_ERR("Read an unexpected length (%u)! Continuing would cause an underflow!", rrc);
	    break;
	}
	wlen -= rrc;
	wrc = imp->api->write( imp, buf, rrc );
	//WHEFS_DBG("wrc=%u, rrc=%u, xoimp->tell=%u",wrc, rrc, imp->api->tell(imp));
	totalW += wrc;
	if( wrc != rrc ) break;
    }
    while( rrc && wrc );
    src->api->seek( src, oldPos, SEEK_SET );
    const size_t impSize = whio_dev_size( imp );
    imp->api->finalize(imp);
    imp = 0;

    rc = whefs_rc.OK;
    if( totalR != totalW )
    {
	WHEFS_DBG_ERR("Pseudofile [%s]: Total read bytes (%u) != total written bytes (%u)", fname, totalR, totalW );
	rc = whefs_rc.IOError;
    }
    else if( impSize != newFSize )
    {
	WHEFS_DBG_ERR("Pseudofile [%s]: Imported file size (%u) does not match the source's size (%u)", fname, totalR, totalW );
	rc = whefs_rc.IOError;
    }
    if( whefs_rc.OK != rc )
    {
	if( ! existed )
	{
	    whefs_inode_unlink( fs, &ino );
	}
    }
    return rc;
}

int whefs_fs_dump_to_filename( whefs_fs * fs, char const * filename )
{
    if( ! fs || !filename || !fs->dev ) return whefs_rc.ArgError;
    FILE * f = fopen( filename, "w+" );
    if( ! f ) return whefs_rc.AccessError;
    int rc = whefs_fs_dump_to_FILE( fs, f );
    fclose( f );
    return rc;
}

const whefs_fs_entry whefs_fs_entry_init = whefs_fs_entry_init_m;
int whefs_fs_entry_foreach( whefs_fs * fs, whefs_fs_entry_foreach_f func, void * foreachData )
{
    if( ! fs || !func ) return whefs_rc.ArgError;
    whefs_id_type i = 2;// skip root inode
    whefs_inode n = whefs_inode_init;
    int rc = whefs_rc.OK;
    whefs_fs_entry ent = whefs_fs_entry_init;
    enum { bufSize = whefs_sizeof_max_filename + 1 };
    char buf[bufSize];
    memset(buf,0,bufSize);
    ent.name.string = buf;
    ent.name.alloced = bufSize;
    for( ; i <= fs->options.inode_count; ++i )
    {
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
	if( fs->bits.i_loaded && !WHEFS_ICACHE_IS_USED(fs,i) )
	{
	    continue;
	}
#endif
	rc = whefs_inode_id_read( fs, i, &n );
	if( whefs_rc.OK != rc ) break;
	if( n.id != i )
	{
	    assert( 0 && "node id mismatch after whefs_inode_id_read()" );
	    WHEFS_FIXME("Node id mismatch after successful whefs_inode_id_read(). Expected %"WHEFS_ID_TYPE_PFMT" but got %"WHEFS_ID_TYPE_PFMT".", i, n.id );
	    rc = whefs_rc.InternalError;
            break;
	}
        if( !( n.flags & WHEFS_FLAG_Used ) ) continue;
        ent.inode_id = i;
        ent.mtime = n.mtime;
        ent.block_id = n.first_block;
        ent.size = n.data_size;
        rc = whefs_inode_name_get( fs, i, &ent.name );
        if( whefs_rc.OK != rc ) break;
        rc = func( fs, &ent, foreachData );
        if( whefs_rc.OK != rc ) break;
    }
    assert( (ent.name.string == buf) && "Internal error: illegal (re)alloc on string bytes!");
    return rc;
}


whefs_string * whefs_ls( whefs_fs * fs, char const * pattern, whefs_id_type * count  )
{
    // FIXME: reimplement in terms of whefs_fs_entry_foreach().
    if( ! fs ) return 0;
    const whefs_id_type nc = whefs_fs_options_get(fs)->inode_count;
    whefs_id_type id = 2; /* ID 1 is reserved for root node entry. */
    int rc = whefs_rc.OK;
    whefs_string * head = 0;
    whefs_string * str = 0;
    whefs_string * prev = 0;
    if( count ) *count = 0;
    whefs_string theString = whefs_string_init;
    whefs_inode tmpn = whefs_inode_init;
    for( ; id <= nc; ++id )
    {
#if WHEFS_CONFIG_ENABLE_BITSET_CACHE
	if( fs->bits.i_loaded && !WHEFS_ICACHE_IS_USED(fs,id) )
	{
	    continue;
	}
	//WHEFS_DBG("Cache says inode #%i is used.", i );
#endif
	rc = whefs_inode_name_get( fs, id, &theString );
	if( whefs_rc.OK != rc )
	{
	    WHEFS_DBG_ERR("whefs_inode_name_get() failed! rc=%d",rc);
	    whefs_string_clear( &theString, false );
	    return head;
	}
	if(0) WHEFS_DBG("Here. id=%"WHEFS_ID_TYPE_PFMT" str.len=%u str=[%s]",
			id, theString.length, theString.string);
	if( pattern && *pattern && !whglob_matches( pattern, theString.string ) )
	{
	    continue;
	}
        if(1)
        {// make sure it's marked as used, or skip it
#if 0
            uint32_t flagcheck = 0;
            whefs_inode_read_flags( fs, id, &flagcheck );
            if( ! (flagcheck & WHEFS_FLAG_Used) ) continue;
#else
            rc = whefs_inode_id_read( fs, id, &tmpn );
            if( whefs_rc.OK != rc ) break;
            if( ! (tmpn.flags & WHEFS_FLAG_Used) ) continue;
#endif
        }
	str = whefs_string_alloc();
	if( ! str ) return head;
	if( ! head ) head = str;
	*str = theString;
        theString = whefs_string_init; // take over ownership of theString.string
	if( prev ) prev->next = str;
	prev = str;
	if( count ) ++(*count);
    }
    whefs_string_clear( &theString, false );
    return head;
}

/* end file src/whefs_client_util.c */
