/**
   Time/delay-related functions for v8-juice.

   Requires pthreads, sleep(3), and usleep(3)

   This code is intentionally kept free of v8-juice-specific APIs
   so that it can easily be ported to other v8-based projects.

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: Public Domain

*/
#include <v8/juice/time.h>
#include <pthread.h>
#include <unistd.h> // sleep(), usleep()

// #ifndef CERR
// #define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
// #endif
// #ifndef COUT
// #define COUT std::cout << __FILE__ << ":" << std::dec << __LINE__ << " : "
// #endif

namespace v8 { namespace juice {

    using namespace v8;

    /**
       Internal holder for setTimeout() callback data.
    */
    struct js_thread_info
    {
        /** Timeout delay, in miliseconds. */
        uint32_t delay;
        /** The Function/String handle passed as argv[0] to setTimeout(). */
        v8::Persistent<v8::Value> jv;
        /** The 'this' object (the global object) for making the
            callback. If we don't store this here we get segfaults when we
            Call() the callback. */
        v8::Persistent<v8::Object> jself;

        /** Set by the ctor. */
        uint32_t id;

        js_thread_info()
            : delay(0),
              id( next_id() ),
              jv(),
              jself()
        {}
        static uint32_t next_id()
        {
            static uint32_t x = 0;
            // FIXME: check for dupes after overflow. First we need the timer id map.
            return ++x;
        }
    };

    /**
       pthread_create() callback for implementing JS setTimeout().

       arg must be-a pointer to a fully-populated js_thread_info
       object. This function will free that object.

       Uses v8::Unlocker to unlock v8 for the duration of the timeout.
    */
    static void * thread_setTimeout( void * arg )
    {
        if( v8::V8::IsDead() ) return NULL;
        v8::Locker locker;
        v8::HandleScope hsc;
        js_thread_info * jio = reinterpret_cast<js_thread_info*>( arg );
        /** reminder: copy jio to the stack so we can free it up immediately
            and not worry about a leake via an exception propagating below... */
        js_thread_info ji(*jio);
        delete jio;
        unsigned int d = ji.delay * 1000;
        int src = 0;
        {
            v8::Unlocker ul;
            /**
               FIXME: posix has obsoleted usleep() and recommends
               nanonosleep(), with it's much more complicated
               interface. OTOH, according to APUE, nanosleep() is only
               required to be defined on platforms which implement "the
               real-time extensions". What to do?
            */
            src = ::usleep( d );
        }
        if( ji.jv->IsFunction() )
        {
            v8::Handle<v8::Function> fh( v8::Function::Cast( *(ji.jv) ) );
            fh->Call( ji.jself, 0, 0 );
        }
        return NULL;
    }


    v8::Handle<v8::Value> setTimeout(const v8::Arguments& argv )
    {
        if( argv.Length() < 2 )
        {
            return v8::ThrowException( v8::String::New("setTimeout() requires two arguments!") );
        }
        v8::Locker locker;
        v8::HandleScope hsc; // this must exist to avoid a weird error
        v8::Handle<v8::Value> fh( argv[0] );
        if( ! fh->IsFunction() )
        {
            return v8::ThrowException( v8::String::New("setTimeout() requires a function [FIXME: or string] as its first argument!") );
        }
        uint32_t tm = static_cast<uint32_t>( argv[1]->ToNumber()->Value() );
        pthread_t tid;
        js_thread_info * ji = new js_thread_info;
        ji->delay = tm;
        ji->jv = v8::Persistent<v8::Value>::New( fh );
        ji->jself = v8::Persistent<v8::Object>::New( argv.This() );
        v8::Handle<v8::Value> id = v8::Integer::New( ji->id );
        ::pthread_create( &tid, NULL, thread_setTimeout, ji );
        return hsc.Close( id ); // fixme: return a timer ID
    }

    v8::Handle<v8::Value> sleep(const v8::Arguments& argv)
    {
        int st = argv.Length() ? static_cast<uint32_t>( argv[0]->ToInteger()->Value() ) : 0;
        int rc = -1;
        if(st)
        {
            v8::Unlocker ul;
            rc = ::sleep( st );
        }
        return v8::Integer::New(rc);
    }

}}
