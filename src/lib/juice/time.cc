/**
   Time/delay-related functions for v8-juice.

   Requires pthreads, sleep(3), and usleep(3). The mutex-locking code
   required for a proper implementation of clearTimeout() requires
   some accompanying mutex code, but those bits can easily be replaced
   with a mutex of your choice.

   This code is intentionally kept free of v8-juice-specific APIs
   so that it can easily be ported to other v8-based projects.

   Author: Stephan Beal (http://wanderinghorse.net/home/stephan/)

   License: Public Domain

   TODOs: remove the hard-coded pthread dep and instead use Marc
   Duerner's thread class, or similar.
*/
#include <v8/juice/time.h>
#include <pthread.h>

#if ! defined(_XOPEN_SOURCE)
   /** on Linux, required for usleep(). */
#  define _XOPEN_SOURCE 500
#endif

#include <unistd.h> // sleep(), usleep()
#include <map>
#include <set>

/** Set USE_WHNET_MUTEX to a true value to enable the mutex
    package from the v8-juice tree. If set to false it uses
    a dummy/no-op mutex, which the client can replace with
    his own if he likes.
*/
#define USE_WHNET_MUTEX 1
#if USE_WHNET_MUTEX
#  include "mutex.hpp"
#endif

// #include <iostream> // only for debuggering
// #define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "

namespace v8 { namespace juice {

    namespace Detail
    {
        struct js_thread_info;
        /**
           MutexType can be any type compatible with the MutexSentry
           interface (see below).
           
           Conventionally has lock() and unlock() methods which lock a
           specific mutex.

           Must be allocable as a static variable.

           Note that this mutex has NOTHING to do with v8 mutexes.  It
           is for fencing off internal access to our internal
           cross-thread data.
        */
#if USE_WHNET_MUTEX
        typedef v8::juice::mutex MutexType;
#else
        struct MutexType
        {
            /** Must lock the mutex, not returning until it can lock. */
            void lock(){}
            /** Must unlock the mutex, not returning until it can unlock. */
            void unlock(){}
        };
#endif
        /**
           MutexSentry can be any type with the interface:

           MutextSentry::MutextSentry( MutextType & );

           Must lock the given mutex as construction and unlock it at
           destruction. Or it may be a no-op which does nothing,
           which opens up the code to very minor race conditions
           involving setTimeout()/clearTimeout(), but "should" be
           harmless other than a very rare failed clearTimeout().
        */
#if USE_WHNET_MUTEX
        typedef v8::juice::mutex_sentry MutexSentry;
#else
        /**
           A dummy/no-op MutexSentry impl.
        */
        struct MutexSentry
        {
        private:
            MutexType & mutex;
        public:
            MutexSentry( MutexType & m ) : mutex(m)
            {
                this->mutex.lock();
            }
            ~MutexSentry()
            {
                this->mutex.unlock();
            }
        };
#endif
        /**
           Internal utility type to lock access to internal
           set of setTimeout()/clearTimeout() timer IDs.

           Only one may exist at a time per thread. Results are
           undefined (but probably a deadlock) if this is violated.

           All data published by this class use cross-thread
           shared data. Instantiating this class locks a mutex
           to allow locked access to the data. When this object
           is destroyed, the lock is freed.
        */
        class TimerLock : public MutexSentry
        {
        public:
            typedef uint32_t TimerIDType;
            typedef std::set<TimerIDType> SetType;
        private:
            /** This class' shared mutex. */
            static MutexType & timeMutex()
            {
                static MutexType bob;
                return bob;
            }
            static SetType & _set()
            {
                static SetType bob;
                return bob;
            }
        public:
            /**
               Locks a mutex shared by all instances of this class.
               Does not return until the lock is acquired.
            */
            TimerLock() : MutexSentry(timeMutex())
            {
            }
            /**
               Unlocks this class's mutex.
             */
            ~TimerLock()
            {
            }
            /** Returns the shared timer set. */
            SetType & set()
            {
                return _set();
            }
            /** Returns the shared timer set. */
            const SetType & set() const
            {
                return _set();
            }
            /**
               Removes the given timer id from the internal
               list. Returns 0 if no timer was found, else some other
               value.
             */
            size_t take(TimerIDType id)
            {
                return set().erase(id);

            }
            /** Returns true if id is a registered timer ID. */
            bool has(TimerIDType id) const
            {
                const SetType & s( set() );
                return s.find(id) != s.end();
            }
            /**
               Generates a unique timer ID and inserts it into
               the internal list.
            */
            TimerIDType next_id()
            {
                static TimerIDType x = 0;
                // Generate unique timer ID, avoiding collisions after overflow:
                while( ! this->set().insert( ++x ).second )
                { (void)1; }
                return x;
            }
        };

        /**
           Internal holder for setTimeout() callback data.
        */
        struct js_thread_info
        {
        public:
            /** Timeout delay, in miliseconds. */
            uint32_t delay;
            /**
               If true, the object is running via setInterval(),
               else setTimeout().
            */
            bool isInterval;
            /** Set only by the ctor or copy ops. */
            TimerLock::TimerIDType id;
            /** The Function/String handle passed as argv[0] to setTimeout(). */
            v8::Persistent<v8::Value> jv;
            /** The 'this' object (the global object) for making the
                callback. If we don't store this here we get segfaults when we
                Call() the callback. */
            v8::Persistent<v8::Object> jself;
            /**
               This is the eval() Function of the global object. We
               have to store it in setTimeout() because in the pthread
               callback we often get a null context when using
               Context::GetCurrent() and friends.
            */
            v8::Persistent<v8::Function> evalfunc;
            /**
               Creates an empty (except for this->id) holder object
               for setTimeout() metadata. After calling this,
               this->id is guaranteed to be unique within the current
               set of timeout timers.
            */
            js_thread_info()
                : delay(0),
                  isInterval(false),
                  id(TimerLock().next_id()),
                  jv(),
                  jself(),
                  evalfunc()
            {
            }
        };        
    } // namespace Detail

    /**
       pthread_create() callback for implementing the delayed start of
       JS code via setTimeout().

       arg must be-a pointer to a fully-populated Detail::js_thread_info
       object. This function will free that object.

       Uses v8::Unlocker to unlock v8 for the duration of the timeout.
    */
    static void * thread_setTimeout( void * arg )
    {
#define THREAD_RETURN return NULL; //::pthread_exit( (void *)0 )
        if( ! v8::V8::IsDead() )
        {
            v8::Locker locker;
            v8::HandleScope hsc;
            Detail::js_thread_info * jio = reinterpret_cast<Detail::js_thread_info*>( arg );
            /** reminder: copy jio to the stack so we can free it up immediately
                and not worry about a leak via an exception propagating below... */
            const Detail::js_thread_info ji(*jio);
            const uint32_t udelay = ji.delay * 1000;
            delete jio;
            bool isFunc = ji.jv->IsFunction();
            typedef v8::Local<v8::Function> LoF;
            LoF fh( ( isFunc ) ? LoF( v8::Function::Cast( *(ji.jv) ) ) : LoF() );
            typedef v8::Local<v8::Value> LoS;
            LoS jscode( (isFunc) ? LoS() : *(ji.jv) );
            do
            {
                int src = 0;
                {
                    v8::Unlocker ul;
                    /**
                   FIXME: for long waits, wake up periodically and see if
                   we should still be waiting, otherwise this might keep
                   the app from exiting from an arbitrarily long time
                   after the main thread has gone. But how to know that?
                    */
                    /**
                       FIXME: posix has obsoleted usleep() and recommends
                       nanonosleep(), with it's much more complicated
                       interface. OTOH, according to APUE, nanosleep() is only
                       required to be defined on platforms which implement "the
                       real-time extensions". What to do?
                    */
                    src = ::usleep( udelay );
                    // Check for cancellation resp. unregister the timer ID:
                    Detail::TimerLock lock;
                    if( ! (ji.isInterval ? lock.has(ji.id) : lock.take(ji.id)) ) break;
                }
                if( v8::V8::IsDead() ) break;
                if( isFunc )
                {
                    fh->Call( ji.jself, 0, 0 );
                }
                else
                {
                    ji.evalfunc->Call( ji.jself, 1, &jscode );
                }
            } while( ji.isInterval );
        }
        THREAD_RETURN;
#undef THREAD_RETURN
    }

    v8::Handle<v8::Value> clearTimeout(const v8::Arguments& argv )
    {
        if( argv.Length() != 1 )
        {
            return v8::False();
        }
        v8::Handle<v8::Value> arg = argv[0];
        if( arg.IsEmpty() || ! arg->IsNumber() ) return v8::False();
        // do we need this: v8::Locker locker;
        Detail::TimerLock::TimerIDType id = static_cast<Detail::TimerLock::TimerIDType>( arg->ToInteger()->Value() );
        return Detail::TimerLock().take(id) ? v8::True() : v8::False();
    }
    v8::Handle<v8::Value> clearInterval(const v8::Arguments& argv )
    {
        return clearTimeout(argv);
    }

    /**
       If isInterval, this behaves like setInterval(), otherwise as
       setTimeout().
    */
    template <bool isInterval>
    static v8::Handle<v8::Value> setTimeoutImpl(const v8::Arguments& argv )
    {
        if( argv.Length() < 2 )
        {
            return v8::ThrowException( v8::String::New("setTimeout() requires two arguments!") );
        }
        v8::Local<v8::Value> fh( argv[0] );
        if( fh.IsEmpty() || (! fh->IsFunction() && ! fh->IsString()) )
        {
            return v8::ThrowException( v8::String::New("setTimeout() requires a Function or evalable string as its first argument!") );
        }
        v8::Locker locker;
        v8::HandleScope hsc; // this must exist to avoid a weird error
        Detail::js_thread_info * ji = new Detail::js_thread_info;
        ji->delay = static_cast<uint32_t>( argv[1]->ToNumber()->Value() );
        if( fh->IsString() )
        {
            Local<Object> evalObj = v8::Context::GetCurrent()->Global();
            Local<Function> eval = v8::Function::Cast( *(evalObj->Get(v8::String::New("eval"))) );
            ji->evalfunc = v8::Persistent<v8::Function>::New( eval );
        }
        ji->jv = v8::Persistent<v8::Value>::New( fh );
        ji->jself = v8::Persistent<v8::Object>::New( argv.This() );
        ji->isInterval = isInterval;
        v8::Handle<v8::Value> id = v8::Integer::New( ji->id );
        pthread_t tid;
        pthread_attr_t tattr;
        pthread_attr_init( &tattr );
        pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
        // Reminder: transfer ownership of ji to thread_setTimeout,
        // and ji might be invalidated by the time pthread_create()
        // returns.
        ::pthread_create( &tid, &tattr, thread_setTimeout, ji );
        pthread_attr_destroy(&tattr);
        return hsc.Close( id );
    }

    v8::Handle<v8::Value> setTimeout(const v8::Arguments& argv )
    {
        return setTimeoutImpl<false>( argv );
    }
    
    v8::Handle<v8::Value> setInterval(const v8::Arguments& argv )
    {
        return setTimeoutImpl<true>( argv );
    }

    /**
       Internal impl of sleep()/mssleep()/usleep(). DelayMult
       must be:

       sleep(): 1000*1000
       mssleep(): 1000
       usleep(): 1
    */
    template <uint32_t DelayMult>
    static v8::Handle<v8::Value> sleepImpl(const v8::Arguments& argv)
    {
        int32_t st = argv.Length() ? static_cast<int32_t>( argv[0]->ToInteger()->Value() ) : -1;
        int rc = -1;
        if(0 <= st)
        {
            v8::Unlocker ul;
            rc = ::usleep( static_cast<uint32_t>(st) * DelayMult );
        }
        return v8::Integer::New(rc);
    }

    v8::Handle<v8::Value> sleep(const v8::Arguments& argv)
    {
        return sleepImpl<1000*1000>( argv );
    }

    v8::Handle<v8::Value> mssleep(const v8::Arguments& argv)
    {
        return sleepImpl<1000>( argv );
    }

    v8::Handle<v8::Value> usleep(const v8::Arguments& argv)
    {
        return sleepImpl<1>( argv );
    }
    
}}
