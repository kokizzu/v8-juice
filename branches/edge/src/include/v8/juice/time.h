#ifndef CODE_GOOGLE_COM_V8_JUICE_TIME_H_INCLUDED
#define CODE_GOOGLE_COM_V8_JUICE_TIME_H_INCLUDED

#include <v8.h>

namespace v8 { namespace juice {

    /**
       A setTimeout() implementation which can be bound to v8.
    
       JS usage: setTimeout( Function|string, when ), where 'when' is
       a number of milliseconds, after which the given function will
       be called or the given string will be eval()'d. Returns a
       unique timer ID which can be passed to clearTimeout() before
       the interval expires in order to cancel the queued function.

       This starts up a separate thread (using an unspecified
       threading technique). That thread will briefly lock v8, then
       will unlock it to sleep for argv[1] milliseconds.  When it
       wakes up, it waits for the v8 lock and then runs the function
       defined by argv[0]. If clearTimeout() has been called while the
       timer was sleeping, the function will not be executed.
       
       During the countdown this routine uses v8::Unlocker to unlock
       the v8 engine for other threads.
       
       FIXME: add clearTimeout().
    */
    v8::Handle<v8::Value> setTimeout(const v8::Arguments& argv );
    /**
       Identical to setTimeout() except that the client-supplied
       callback is executed in a loop, ending only when the
       client passes the return value from this function to
       clearInterval() (or setTimeout(), which is the same in this
       implementation).
    */
    v8::Handle<v8::Value> setInterval(const v8::Arguments& argv );

    /**
       A clearTimeout() implementation which can be bound to v8.

       Requires argv[0] to be a timer ID returned from setTimeout().
       If such a timer is found and can be cancelled (has not yet been
       fired) then true is returned, else false.
    */
    v8::Handle<v8::Value> clearTimeout(const v8::Arguments& argv );

    /**
       Identical to clearTimeout(), but intended for cancelling
       timeouts set via setInterval().
    */
    v8::Handle<v8::Value> clearInterval(const v8::Arguments& argv );

    /**
       A sleep() implementation which can be bound to v8.
    
       JS usage: sleep(seconds)

       Behaves identically to the POSIX-standard sleep(), but returns
       -1 if argv[0] is not a positive integer.

       This routine uses v8::Unlocker to unlock the v8 engine for
       other threads.
    */
    v8::Handle<v8::Value> sleep(const v8::Arguments & argv);
    /**
       Like sleep(), but naps for the given number of milliseconds
       (thousandths of a second).
       
       JS Usage: msleep( milliseconds )
    */
    v8::Handle<v8::Value> mssleep(const v8::Arguments & argv);
    /**
       Like sleep(), but naps for the given number of microseconds
       (millionths of a second).
    
       JS Usage: usleep( nanoseconds )

       but be aware that you won't get really high resolution via JS
       when sleeping for very small intervals, due to the overhead involved
       in passing around JS params and locking/unlocking the engine.
    */
    v8::Handle<v8::Value> usleep(const v8::Arguments & argv);

}} // namespace

#endif // CODE_GOOGLE_COM_V8_JUICE_TIME_H_INCLUDED
