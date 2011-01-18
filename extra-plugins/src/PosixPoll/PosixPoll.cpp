/************************************************************************
Author: Stephan Beal (http://wanderinghorse.net/home/stephan)

License: Public Domain

v8 bindings for a simple loop operation based on poll(2).

************************************************************************/
#if !defined _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#include <v8/juice/forwarding.h>
#include <v8/juice/plugin.h>
#include <v8/juice/juice.h>
#include <v8/juice/ClassWrap.h>

#include <poll.h>

#ifndef CERR
#include <iostream> /* only for debuggering */
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

#define JSTR(X) v8::String::New(X)
namespace cv = v8::juice::convert;

/**
    v8::InvocationCallback impl for poll().

    JS Usage:

    poll( int interval, function onloop[, function until] )

    It runs onloop() every 'interval' milliseconds. After each
    interval until() is called. If until() throws or returns false
    then looping stops. If onloop() throws an exception, looping
    stops.  If until() is not passed or not a function then the loop
    is run only one time, after the specified interval. Exceptions
    thrown by the loop/until functions break the loop and are passed
    back to the caller.

    Unlike setTimeout(), poll() does does return until looping
    completes. 

    The return value is unspecified.
*/
static v8::Handle<v8::Value> do_poll( v8::Arguments const & argv )
{
    char const * usage = "poll() requires (int interval[>0], function f [, function until]) arguments.";
    if( 2 > argv.Length() )
    {
        return v8::ThrowException(JSTR(usage));
    }
    int64_t const interval = argv[0]->IntegerValue();
    if( interval <= 0 )
    {
        return v8::ThrowException(JSTR(usage));
    }

    v8::Handle<v8::Function> func;
    {
        v8::Handle<v8::Value> const & vf = argv[1];
        if( ! vf->IsFunction() )
        {
            return v8::ThrowException(JSTR(usage));
        }
        func = v8::Handle<v8::Function>( v8::Function::Cast(*vf) );
    }

    v8::Handle<v8::Function> until;
    if( argv.Length() > 2 )
    {
        v8::Handle<v8::Value> const & vf = argv[2];
        if( vf->IsFunction() )
        {
            until = v8::Handle<v8::Function>( v8::Function::Cast(*vf) );
        }
    }

    bool keepGoing = true;
    while(keepGoing)
    {
        int prc;
        {
            v8::Unlocker lock;
            prc = ::poll( NULL, 0, static_cast<unsigned int>(interval) );
        }
        if( 0 != prc )
        {
            cv::StringBuffer msg;
            msg << "poll() returned error code "<<prc<<'.';
            return v8::ThrowException(msg);
        }
        v8::HandleScope scope;
        v8::Local<v8::Value> rv = func->Call( argv.This(), 0, NULL );
        if( rv.IsEmpty() ) break;
        else if( until.IsEmpty() ) keepGoing = false;
        else
        {
            rv = until->Call( argv.This(), 0, NULL );
            keepGoing = rv.IsEmpty() ? false : rv->BooleanValue();
        }
    }
    
    return v8::Undefined();
}

/**
   Sets up the Poller-related bindings.

   The dest object gets a new member named posix which contains
   the following properties:

   Classes:
   
   Functions:
   

   Properties (read-only):

*/
static v8::Handle<v8::Value> Poller_plugin_init( v8::Handle<v8::Object> dest )
{
    using namespace v8;
    using namespace v8::juice;
    HandleScope scope;
    typedef convert::InvocationCallbackCreator ICC;
    v8::Handle<v8::Object> posix = v8::juice::GetNamespaceObject( dest, "posix" );
    if( posix.IsEmpty() ) return posix /* assume an exception is passing back. */;
#define JF v8::FunctionTemplate::New(cb)->GetFunction()
#define F(X) posix->Set( JSTR(X), JF )
    v8::InvocationCallback cb;
    cb = do_poll;
    F("poll");
    return dest;
#undef JF
#undef F
}
#undef DBGOUT
#undef JSTR


V8_JUICE_PLUGIN_STATIC_INIT(Poller_plugin_init);
