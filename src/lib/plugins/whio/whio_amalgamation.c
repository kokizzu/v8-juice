/* auto-generated on Sat Mar  7 19:32:28 CET 2009. Do not edit! */
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200112L /* needed for ftello() and friends */
#endif
#if !defined(_ISOC99_SOURCE)
#define _ISOC99_SOURCE 1 /* needed for snprintf() */
#endif
#include "whio_amalgamation.h"
/* begin file whio.c */
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
/* begin file whio_dev.c */
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

const whio_dev_client whio_dev_client_init = whio_dev_client_init_m;

#define WHIO_DEV_EMPTY_INIT {0/*api*/, {/*impl*/ 0/*data*/, 0/*typeID*/}, whio_dev_client_init_m }

static const whio_dev whio_dev_empty_init = WHIO_DEV_EMPTY_INIT;

#if WHIO_USE_STATIC_MALLOC
enum {
/**
   The number of elements to statically allocate
   in the whio_dev_alloc_slots object.
*/
whio_dev_alloc_count = 5
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
#if WHIO_USE_STATIC_MALLOC
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
#endif /* WHIO_USE_STATIC_MALLOC */
    if( ! dev ) dev = (whio_dev *) malloc( sizeof(whio_dev) );
    if( dev ) *dev = whio_dev_empty_init;
    return dev;
}

void whio_dev_free( whio_dev * dev )
{
    if( dev ) *dev = whio_dev_empty_init;
    else return;	
#if WHIO_USE_STATIC_MALLOC
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
		       &whio_dev_alloc_slots.devs[0],
		       &whio_dev_alloc_slots.devs[whio_dev_alloc_count-1],
		       ndx
		       );
	    WHIO_DEBUG("Freeing object @0x%p from static pool index %u (@0x%p)\n",
		       dev,
		       ndx,
		       &whio_dev_alloc_slots.devs[ndx] );
	}
	whio_dev_alloc_slots.used[ndx] = 0;
	if( ndx < whio_dev_alloc_slots.next ) whio_dev_alloc_slots.next = ndx;
	return;
    }
#else
    free(dev);
#endif /* WHIO_USE_STATIC_MALLOC */
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


static const unsigned char whio_dev_uint64_tag_char = 0x80 | 64;
size_t whio_dev_uint64_encode( whio_dev * fs, uint64_t i )
{
    unsigned char buf[whio_dev_sizeof_uint64]; /* Flawfinder: ignore This is intentional and safe as long as whio_dev_sizeof_uint64 is the correct size. */
    const uint64_t mask = UINT64_C(0x00ff);
    size_t x = 0;
    buf[x++] = whio_dev_uint64_tag_char;
    buf[x++] = (unsigned char)((i>>56) & mask);
    buf[x++] = (unsigned char)((i>>48) & mask);
    buf[x++] = (unsigned char)((i>>40) & mask);
    buf[x++] = (unsigned char)((i>>32) & mask);
    buf[x++] = (unsigned char)((i>>24) & mask);
    buf[x++] = (unsigned char)((i>>16) & mask);
    buf[x++] = (unsigned char)((i>>8) & mask);
    buf[x++] = (unsigned char)(i & mask);
    return whio_dev_write( fs, buf, whio_dev_sizeof_uint64 );
}

int whio_dev_uint64_decode( whio_dev * dev, uint64_t * tgt )
{
    if( ! dev || ! tgt ) return whio_rc.ArgError;
    unsigned char buf[whio_dev_sizeof_uint64]; /* Flawfinder: ignore This is intentional and safe as long as whio_dev_sizeof_uint64 is the correct size. */
    memset( buf, 0, whio_dev_sizeof_uint64 );
    size_t rc = dev->api->read( dev, buf  /*Flawfinder: ignore  This is safe in conjunction with whio_dev_sizeof_uint64*/, whio_dev_sizeof_uint64 );
    if( whio_dev_sizeof_uint64 != rc )
    {
	return whio_rc.IOError;
    }
    if( whio_dev_uint64_tag_char != buf[0] )
    {
	return whio_rc.ConsistencyError;
    }
#define SHIFT(X) ((uint64_t)buf[X])
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


static const unsigned char whio_dev_uint32_tag_char = 0x80 | 32;
size_t whio_dev_uint32_encode( whio_dev * fs, uint32_t i )
{
    unsigned char buf[whio_dev_sizeof_uint32];  /* Flawfinder: ignore This is intentional and safe as long as whio_dev_sizeof_uint32 is the correct size. */
    const unsigned short mask = 0x00ff;
    size_t x = 0;
    /** We tag all entries with a prefix mainly to simplify debugging
	of the vfs files (it's easy to spot them in a file viewer),
	but it incidentally also gives us a sanity-checker at
	read-time (we simply confirm that the first byte is this
	prefix).
    */
    buf[x++] = whio_dev_uint32_tag_char;
    buf[x++] = (unsigned char)(i>>24) & mask;
    buf[x++] = (unsigned char)(i>>16) & mask;
    buf[x++] = (unsigned char)(i>>8) & mask;
    buf[x++] = (unsigned char)(i & mask);
    //WHIO_DEBUG("wrote int=%u to [%02x %02x %02x %02x]\n",i,buf[1],buf[2],buf[3],buf[4]);
    return whio_dev_write( fs, buf, whio_dev_sizeof_uint32 );
}

int whio_dev_uint32_decode( whio_dev * dev, uint32_t * tgt )
{
    if( ! dev || ! tgt ) return whio_rc.ArgError;
    unsigned char buf[whio_dev_sizeof_uint32];  /* Flawfinder: ignore This is intentional and safe as long as whio_dev_sizeof_uint32 is the correct size. */
    memset( buf, 0, whio_dev_sizeof_uint32 );
    size_t rc = dev->api->read( dev, buf, whio_dev_sizeof_uint32 ); /*Flawfinder: ignore  This is safe in conjunction with whio_dev_sizeof_uint32*/
    if( whio_dev_sizeof_uint32 != rc )
    {
	//WHIO_DEBUG("read of integer failed! rc=%u\n",rc);
	return whio_rc.IOError;
    }
    //WHIO_DEBUG("read int(?) from [%c %02x %02x %02x %02x]\n",buf[0],buf[1],buf[2],buf[3],buf[4]);
    if( whio_dev_uint32_tag_char != buf[0] )
    {
	//WHIO_DEBUG("read bytes are not an encoded integer value!\n");
	return whio_rc.ConsistencyError;
    }
    uint32_t val = (buf[1] << 24)
	+ (buf[2] << 16)
	+ (buf[3] << 8)
	+ (buf[4]);
    *tgt = val;
    //WHIO_DEBUG("read int=%u from [%c %02x %02x %02x %02x]\n",val,buf[0],buf[1],buf[2],buf[3],buf[4]);
    return whio_rc.OK;
}


static const unsigned char whio_dev_uint16_tag_char = 0x80 | 16;
size_t whio_dev_uint16_encode( whio_dev * fs, uint16_t i )
{
    unsigned char buf[whio_dev_sizeof_uint16]; /* Flawfinder: ignore This is intentional and safe as long as whio_dev_sizeof_uint16 is the correct size. */
    const size_t mask = 0x00ff;
    size_t x = 0;
    buf[x++] = whio_dev_uint16_tag_char;
    buf[x++] = (unsigned char)(i>>8) & mask;
    buf[x++] = (unsigned char)(i & mask);
    return whio_dev_write( fs, buf, whio_dev_sizeof_uint16 );
}

int whio_dev_uint16_decode( whio_dev * dev, uint16_t * tgt )
{
    if( ! dev || ! tgt ) return whio_rc.ArgError;
    unsigned char buf[whio_dev_sizeof_uint16]; /* Flawfinder: ignore This is intentional and safe as long as whio_dev_sizeof_uint16 is the correct size. */
    memset( buf, 0, whio_dev_sizeof_uint16 );
    size_t rc = dev->api->read( dev, buf, whio_dev_sizeof_uint16 ); /*Flawfinder: ignore  This is safe in conjunction with whio_dev_sizeof_uint16*/
    if( whio_dev_sizeof_uint16 != rc )
    {
	//WHIO_DEBUG("read of uint16 failed! rc=%u\n",rc);
	return whio_rc.IOError;
    }
    if( whio_dev_uint16_tag_char != buf[0] )
    {
	//WHIO_DEBUG("read bytes are not an encoded uint16 value!\n");
	return whio_rc.ConsistencyError;
    }
    uint16_t val = + (buf[1] << 8)
	+ (buf[2]);
    *tgt = val;
    return whio_rc.OK;
}

size_t whio_dev_uint32_array_encode( whio_dev * dev, size_t n, uint32_t const * list )
{
    size_t i = (dev && n && list) ? 0 : n;
    size_t rc = 0;
    for( ; i < n; ++i, ++rc )
    {
	if( whio_dev_sizeof_uint32 != whio_dev_uint32_encode( dev, *(list++) ) )
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
size_t whio_dev_cstring_encode( whio_dev * dev, char const * s, uint32_t n )
{
    if( ! dev || !s ) return 0;
    if( ! n )
    {
	char const * x = s;
	loop: if( x && *x ) { ++x; ++n; goto loop; }
	//for( ; x && *x ; ++x, ++n ){}
    }
    size_t rc = dev->api->write( dev, &whio_dev_cstring_tag_char, 1 );
    if( 1 != rc ) return rc;
    rc += whio_dev_uint32_encode( dev, n );
    if( (1 + whio_dev_sizeof_uint32) != rc ) return rc;
    return rc + dev->api->write( dev, s, n );
}

int whio_dev_cstring_decode( whio_dev * dev, char ** tgt, size_t * length )
{
    if( !dev || ! tgt ) return whio_rc.ArgError;

    { /* check tag byte */
	unsigned char tagbuf[1] = {0}; /* Flawfinder: ignore  This is intended and safe. */
	size_t sz = dev->api->read( dev, tagbuf, 1 );/*Flawfinder: ignore  This is safe used safely.*/
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
#if WHIO_USE_STATIC_MALLOC
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
#if WHIO_USE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whio_blockdev_alloc_count; ++i )
    {
	if( whio_blockdev_alloc_slots.used[i] ) continue;
	whio_blockdev_alloc_slots.used[i] = 1;
	obj = &whio_blockdev_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_USE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_blockdev *) malloc( sizeof(whio_blockdev) );
    if( obj ) *obj = whio_blockdev_init;
    return obj;
}

void whio_blockdev_free( whio_blockdev * obj )
{
    whio_blockdev_cleanup( obj );
#if WHIO_USE_STATIC_MALLOC
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
    free(obj);
#endif /* WHIO_USE_STATIC_MALLOC */
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
			   whio_size_t block_size, whio_blockdev_id count, void const * prototype )
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

int whio_blockdev_wipe( whio_blockdev * self, whio_blockdev_id id )
{
    return whio_blockdev_write( self, id, self->blocks.prototype );
}

bool whio_blockdev_in_range( whio_blockdev * self, whio_blockdev_id id )
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
static whio_size_t whio_block_offset_for_id( whio_blockdev * self, whio_blockdev_id id )
{
    return ( ! self || !whio_blockdev_in_range( self, id ) )
	? whio_rc.SizeTError
	: (id * self->blocks.size);
}

int whio_blockdev_write( whio_blockdev * self, whio_blockdev_id id, void const * src )
{
    if( ! src ) return whio_rc.ArgError;
    whio_size_t pos = whio_block_offset_for_id( self, id );
    if( whio_rc.SizeTError == pos )
    {
	WHIO_DEBUG("id #%"PRIu32" is not valid for this whio_blockdev. block count=%"PRIu32"\n",id,self->blocks.count);
	return whio_rc.RangeError;
    }
    if( ! src ) return false;
    if( pos != self->impl.fence->api->seek( self->impl.fence, pos, SEEK_SET ) ) return whio_rc.IOError;
    return (self->blocks.size == self->impl.fence->api->write( self->impl.fence, src, self->blocks.size ))
	? whio_rc.OK
	: whio_rc.IOError;
}

int whio_blockdev_read( whio_blockdev * self, whio_blockdev_id id, void * dest )
{
    whio_size_t pos = whio_block_offset_for_id( self, id );
    if( whio_rc.SizeTError == pos ) return whio_rc.RangeError;
    if( pos != self->impl.fence->api->seek( self->impl.fence, pos, SEEK_SET ) ) return whio_rc.IOError;
    return (self->blocks.size == self->impl.fence->api->read( self->impl.fence, dest, self->blocks.size ))  /*Flawfinder: ignore  Bounds check is mostly done in self->impl.fence->api->read(). Bounds of dest must be ensured by the caller. */
	? whio_rc.OK
	: whio_rc.IOError;
}
/* begin file whio_dev_FILE.c */
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
#include <unistd.h> /* ftruncate() */



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
} whio_dev_FILE;


/**
   Initialization object for whio_dev_FILE objects. Also used as
   whio_dev::typeID for such objects.
*/
#define WHIO_DEV_FILE_INIT { \
    0, /* fp */ \
    0, /* fileno */ \
    0, /* needsFlush */ \
    0 /* ownsFile */ \
    }
static const whio_dev_FILE whio_dev_FILE_meta_init = WHIO_DEV_FILE_INIT;

#if WHIO_USE_STATIC_MALLOC
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
#if WHIO_USE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whio_dev_FILE_alloc_count; ++i )
    {
	if( whio_dev_FILE_alloc_slots.used[i] ) continue;
	whio_dev_FILE_alloc_slots.used[i] = 1;
	obj = &whio_dev_FILE_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_USE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_FILE *) malloc( sizeof(whio_dev_FILE) );
    if( obj ) *obj = whio_dev_FILE_meta_init;
    return obj;
}

static void whio_dev_FILE_free( whio_dev_FILE * obj )
{
#if WHIO_USE_STATIC_MALLOC
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
#endif /* WHIO_USE_STATIC_MALLOC */
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

static bool whio_dev_FILE_close( whio_dev * dev )
{
    if( dev )
    {
	dev->api->flush(dev);
	if( dev->client.dtor ) dev->client.dtor( dev->client.data );
	dev->client = whio_dev_client_init;
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
    whio_dev_FILE_ioctl
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
	f = 0;
    }
    return d;
}
#endif

/* begin file whio_dev_fileno.c */
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
#include <string.h>
#include <fcntl.h>
#include <errno.h>



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
    0 /* errstate */ \
    }
static const whio_dev_fileno whio_dev_fileno_meta_init = WHIO_DEV_fileno_INIT;

#if WHIO_USE_STATIC_MALLOC
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
#if WHIO_USE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whio_dev_fileno_alloc_count; ++i )
    {
	if( whio_dev_fileno_alloc_slots.used[i] ) continue;
	whio_dev_fileno_alloc_slots.used[i] = 1;
	whio_dev_fileno_alloc_slots.objs[i] = whio_dev_fileno_meta_init;
	obj = &whio_dev_fileno_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_USE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_fileno *) malloc( sizeof(whio_dev_fileno) );
    return obj;
}

static void whio_dev_fileno_free( whio_dev_fileno * obj )
{
#if WHIO_USE_STATIC_MALLOC
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
#endif /* WHIO_USE_STATIC_MALLOC */
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
	dev->client = whio_dev_client_init;
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
    whio_dev_fileno_ioctl
    };

static const whio_dev whio_dev_fileno_init =
    {
    &whio_dev_fileno_api,
    { /* impl */
    0, /* data. Must be-a (whio_dev_fileno*) */
    (void const *)&whio_dev_fileno_meta_init /* typeID */
    }
    };

#if 1 /* there is a separate implementation in whio_dev_FILE.c, but the API
	 docs describe this one. */
whio_dev * whio_dev_for_filename( char const * fname, char const * mode )
{
    if( ! fname || !mode ) return 0;
    whio_dev * dev = whio_dev_alloc();
    if( ! dev ) return 0;
    whio_dev_fileno * meta = whio_dev_fileno_alloc();
    if( ! meta )
    {
	whio_dev_free(dev);
	return 0;
    }
    FILE * f = fopen( fname, mode );
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
    return dev;
}
#endif
/* begin file whio_dev_mem.c */
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
   Also used as whio_dev::typeID for membuf.
*/
static const whio_dev_membuf_meta whio_dev_membuf_meta_init = WHIO_DEV_MEMBUF_META_INIT;

#if WHIO_USE_STATIC_MALLOC
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
#if WHIO_USE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whio_dev_membuf_meta_alloc_count; ++i )
    {
	if( whio_dev_membuf_meta_alloc_slots.used[i] ) continue;
	whio_dev_membuf_meta_alloc_slots.used[i] = 1;
	whio_dev_membuf_meta_alloc_slots.objs[i] = whio_dev_membuf_meta_init;
	obj = &whio_dev_membuf_meta_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_USE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_membuf_meta *) malloc( sizeof(whio_dev_membuf_meta) );
    return obj;
}

static void whio_dev_membuf_meta_free( whio_dev_membuf_meta * obj )
{
#if WHIO_USE_STATIC_MALLOC
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
#endif /* WHIO_USE_STATIC_MALLOC */
}


/**
   A helper for the whio_dev_membuf API. Requires that the 'dev'
   parameter be-a whio_dev and that that device is-a whio_dev_membuf.
 */
#define WHIO_MEMBUF_DECL(RV) whio_dev_membuf_meta * mb = (dev ? (whio_dev_membuf_meta*)dev->impl.data : 0); \
    if( !mb  || ((void const *)&whio_dev_membuf_meta_init != dev->impl.typeID) ) return RV

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
	    memset( b + mb->alloced, 0, alen - mb->alloced );
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

static int whio_dev_membuf_ioctl( whio_dev * dev, int arg, va_list vargs )
{
    int rc = whio_rc.UnsupportedError;
    WHIO_MEMBUF_DECL(rc);
    switch( arg )
    {
      case whio_dev_ioctl_BUFFER_size:
      case whio_dev_ioctl_GENERAL_size:
	  rc = whio_rc.OK;
	  *(va_arg(vargs,whio_size_t*)) = mb->alloced;
	  break;
      default: break;
    };
    return rc;
}

static bool whio_dev_membuf_close( whio_dev * dev )
{
    if( dev )
    {
	if( dev->client.dtor ) dev->client.dtor( dev->client.data );
	dev->client = whio_dev_client_init;
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

static const whio_dev_api whio_dev_api_membuf =
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
    whio_dev_membuf_ioctl
    };

#define WHIO_DEV_MEMBUF_INIT { \
    &whio_dev_api_membuf, \
    { /* impl */ \
    0, /* data. Must be-a (whio_dev_membuf*) */ \
    (void const *)&whio_dev_membuf_meta_init /* typeID */ \
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
    mb->expandable = (expFactor > 1.0);
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
   Also used as whio_dev::typeID for memmap.
*/
#define WHIO_DEV_MEMMAP_INIT { \
    0, /* size */ \
    0, /* maxsize */ \
    0, /* pos */ \
    0, /* rw */ \
    0 /* ro */ \
    }

static const whio_dev_memmap whio_dev_memmap_init = WHIO_DEV_MEMMAP_INIT;


#if WHIO_USE_STATIC_MALLOC
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
#if WHIO_USE_STATIC_MALLOC
    size_t i = 0;
    for( ; i < whio_dev_memmap_alloc_count; ++i )
    {
	if( whio_dev_memmap_alloc_slots.used[i] ) continue;
	whio_dev_memmap_alloc_slots.used[i] = 1;
	whio_dev_memmap_alloc_slots.objs[i] = whio_dev_memmap_init;
	obj = &whio_dev_memmap_alloc_slots.objs[i];
	break;
    }
#endif /* WHIO_USE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_memmap *) malloc( sizeof(whio_dev_memmap) );
    return obj;
}

static void whio_dev_memmap_free( whio_dev_memmap * obj )
{
#if WHIO_USE_STATIC_MALLOC
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
#endif /* WHIO_USE_STATIC_MALLOC */
}


/**
   A helper for the whio_dev_memmap API. Requires that the 'dev'
   parameter be-a whio_dev and that that device is-a
   whio_dev_memmap. Declares the local variable (whio_dev_memmap * mb)
   - the internal device data. On error (null argument or type
   mismatch) it calls (return RV).
 */
#define WHIO_MEMMAP_DECL(RV) whio_dev_memmap * mb = (dev ? (whio_dev_memmap*)dev->impl.data : 0); \
    if( !mb  || ((void const *)&whio_dev_memmap_init != dev->impl.typeID) ) return RV

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
	memcpy( dest, mb->ro + mb->pos, rlen );
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
	memcpy( mb->rw + mb->pos, src, wlen );
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

static int whio_dev_memmap_ioctl( whio_dev * dev, int arg, va_list vargs )
{
    int rc = whio_rc.UnsupportedError;
    WHIO_MEMMAP_DECL(rc);
    switch( arg )
    {
      case whio_dev_ioctl_GENERAL_size:
      case whio_dev_ioctl_BUFFER_size:
	  rc = whio_rc.OK;
	  *(va_arg(vargs,whio_size_t*)) = mb->maxsize;
	  break;
      default: break;
    };
    return rc;
}

static bool whio_dev_memmap_close( whio_dev * dev )
{
    if( dev )
    {
	if( dev->client.dtor ) dev->client.dtor( dev->client.data );
	dev->client = whio_dev_client_init;
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

static const whio_dev_api whio_dev_api_memmap =
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
    whio_dev_memmap_ioctl
    };

static const whio_dev whio_dev_memmap_dev_init =
    {
    &whio_dev_api_memmap,
    { /* impl */
    0, /* data. Must be-a (whio_dev_memmap*) */
    (void const *)&whio_dev_memmap_init /* typeID */
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
/* begin file whio_dev_subdev.c */
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

#if WHIO_USE_STATIC_MALLOC
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
#if WHIO_USE_STATIC_MALLOC
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
#endif /* WHIO_USE_STATIC_MALLOC */
    if( ! obj ) obj = (whio_dev_subdev_meta *) malloc( sizeof(whio_dev_subdev_meta) );
    return obj;
}

void whio_dev_subdev_meta_free( whio_dev_subdev_meta * obj )
{
#if WHIO_USE_STATIC_MALLOC
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
		       &whio_dev_subdev_meta_alloc_slots.objs[0],
		       &whio_dev_subdev_meta_alloc_slots.objs[whio_dev_subdev_meta_alloc_count-1],
		       ndx
		       );
	    WHIO_DEBUG("Freeing object @0x%p from static pool index %u (@0x%p)\n",
		       obj,
		       ndx,
		       &whio_dev_subdev_meta_alloc_slots.objs[ndx] );
	}

	whio_dev_subdev_meta_alloc_slots.objs[ndx] = whio_dev_subdev_meta_init;
	whio_dev_subdev_meta_alloc_slots.used[ndx] = 0;
	return;
    }
#else
    free(obj);
#endif /* WHIO_USE_STATIC_MALLOC */
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
    whio_dev_subdev_ioctl
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
/* begin file whio_stream.c */
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
    { /* impl */
    0, /* data */
    0 /* typeID */
    }
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


#ifdef __cplusplus
} /* extern "C" */
#endif
/* begin file whio_stream_dev.c */
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
/** whio_dev::impl::typeID value for whio_stream_dev objects. */

const whio_stream_api whio_stream_api_dev = 
    {
    whio_stream_dev_read,
    whio_stream_dev_write,
    whio_stream_dev_close,
    whio_stream_dev_finalize,
    whio_stream_dev_flush,
    whio_stream_dev_isgood
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
    if( meta )
    {
	self->impl.data = 0;
	if( meta->dev )
	{
	    meta->dev->api->flush( meta->dev );
	    if( meta->ownsDev )
	    {
		meta->dev->api->finalize( meta->dev );
	    }
	}
	free( meta );
	return true;
    }
    return false;
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
/* begin file whio_stream_FILE.c */
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

const whio_stream_api whio_stream_api_FILE = 
    {
    whio_stream_FILE_read,
    whio_stream_FILE_write,
    whio_stream_FILE_close,
    whio_stream_FILE_finalize,
    whio_stream_FILE_flush,
    whio_stream_FILE_isgood
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
} whio_stream_FILEINFO;
static const whio_stream_FILEINFO whio_stream_FILEINFO_init =
    {
    0, /* fp */
    0, /* fileno */
    false /* ownsFile */
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
    if( meta )
    {
	self->api->flush( self );
	self->impl.data = 0;
	if( meta->fp && meta->ownsFile )
	{
	    fclose( meta->fp );
	}
	*meta = whio_stream_FILEINFO_init;
	free(meta);
	return true;
    }
    return false;
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

/* begin file whprintf.c */
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
  etByte xtype;              /* Conversion paradigm */
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
/* begin file whio_zlib.c */
/*
  Implementations for whio gzip support.

  Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

  License: Public Domain
*/

#if WHIO_ENABLE_ZLIB
#include <zlib.h>
#endif /* WHIO_ENABLE_ZLIB */

int whio_stream_gzip( whio_stream * src, whio_stream * dest, int level )
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


int whio_stream_gunzip( whio_stream * src, whio_stream * dest )
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
	if(0)
	{
	    size_t x = 0;
	    for( ; x < 100; ++x ) putchar(in[x]);
	    putchar('\n');
	}
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
