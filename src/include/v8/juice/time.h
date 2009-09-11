#ifndef CODE_GOOGLE_COM_V8_JUICE_TIME_H_INCLUDED
#define CODE_GOOGLE_COM_V8_JUICE_TIME_H_INCLUDED

#include <v8.h>

namespace v8 { namespace juice {

    /**
       A setTimeout() implementation which can be bound to v8.
    
       JS usage: setTimeout( Function, when ), where 'when' is a number of
       miliseconds. Returns a unique timer ID.

       FIXME: a string should be allowed as the first parameter.

       FIXME: add cancelTimeout().
    */
    v8::Handle<v8::Value> setTimeout(const v8::Arguments& argv );

    /**
       A sleep() implementation which can be bound to v8.
    
       JS usage: sleep(seconds)

       Behaves identically to the POSIX-standard sleep(), but returns -1 if
       argv[0] is not a positive integer.

       This routine calls v8::Unlocker to unlock the v8 engine for other
       threads.
    */
    v8::Handle<v8::Value> sleep(const v8::Arguments & argv);

}} // namespace

#endif // CODE_GOOGLE_COM_V8_JUICE_TIME_H_INCLUDED
