See also: [BindingFunctions](BindingFunctions.md)



# Optional JS Functions #

The native API has several utility functions for use in JS space. They
can be added to a client instance of v8 by binding them using normal
v8 conventions, e.g.:

```
myJSObject->Set(v8::String::New("sleep"),
              v8::FunctionTemplate::New( v8::juice::sleep )->GetFunction() );
```

The supported "extra" functions are listed below, grouped by the header file they are declared in. They are documented in their header files. Most of them are enabled by default in the [v8-juice-shell app](http://code.google.com/p/v8-juice/source/browse/branches/edge/src/client/shell/).

## juice.h ##

[juice.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/juice.h)
[juice.cc](http://code.google.com/p/v8-juice/source/browse/trunk/src/lib/juice/juice.cc)

| **Native Function** (of type `v8::InvocationCallback`) | **JS Signature** | **Description** |
|:-------------------------------------------------------|:-----------------|:----------------|
| `v8::juice::IncludeScript` | `include(string filename)` | Includes external JS files. See [PathFinderClass](PathFinderClass.md). |

## PathFinder.h ##

See [PathFinderClass](PathFinderClass.md).

## plugin.h ##
[plugin.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/plugin.h)
[plugin.cc](http://code.google.com/p/v8-juice/source/browse/trunk/src/lib/juice/plugin.cc)

| **Native Function** (of type `v8::InvocationCallback`) | **JS Signature** | **Description** |
|:-------------------------------------------------------|:-----------------|:----------------|
| `v8::juice::plugin::LoadPlugin` | `loadPlugin( stringpluginName)` | Loads an external plugin from a DLL. See [Plugins](Plugins.md). |


## sprintf.h ##
[sprintf.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/sprintf.h)
[jssprintf.cc](http://code.google.com/p/v8-juice/source/browse/trunk/src/lib/juice/jssprintf.cc)

| **Native Function** (of type `v8::InvocationCallback`) | **JS Signature** | **Description** |
|:-------------------------------------------------------|:-----------------|:----------------|
| `v8::juice::sprintf` | `sprintf(string fmt,args...)` | Similar to conventional `sprintf()` implementations, plus some extensions. Any Objects passed this way will be `toString()`'d. |

## time.h ##

[time.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/time.h)
[time.cc](http://code.google.com/p/v8-juice/source/browse/trunk/src/lib/juice/time.cc)

Added to the API on 20090912.

**ACHTUNG:** For any of these functions to work, a `v8::Locker` instance must have been instantiated before the JS code which uses them starts executing. If you're using `v8-juice-shell` then this is done for you, but if you're writing your own app, you'll need to add a `v8::Locker` sentry somewhere in your main app. If no locker is in place, these functions _will_ cause v8 to crash (with an error message if v8 is built in debug mode, otherwise silently). There is an example of how to set up the locker in the [v8-juice-shell source code](http://code.google.com/p/v8-juice/source/browse/branches/edge/src/client/shell/shell.cc).


| **Native Function** (of type `v8::InvocationCallback`) | **JS Signature** | **Description** |
|:-------------------------------------------------------|:-----------------|:----------------|
| `v8::juice::setTimeout()` | `setTimeout(Function | string, milliseconds)` | Like the browser-standard function, but runs in a separate thread. |
| `v8::juice::clearTimeout()` | `clearTimeout(timerID)` | Removes a scheduled `setTimeout()` callback. |
| `v8::juice::setInterval()` | `setInterval(Function | string, milliseconds)` | Like `setTimeout()`, but runs the given callback at the given interval. |
| `v8::juice::clearInterval()` | `clearInterval(timerID)` | Removes a scheduled `setInterval()` callback. |
| `v8::juice::spawnTimeoutThread()` | `spawnTimeoutThread(Function | string, milliseconds)` | Like setTimeout() but explicitly uses a separate thread of execution. |
| `v8::juice::clearTimeoutThread()` | `clearTimeoutThread(timerID)` | Removes a scheduled `setTimeoutThreadt()` callback. |
| `v8::juice::spawnIntervalThread()` | `spawnIntervalThread(Function | string, milliseconds)` | Like `spawnTimeoutThread()`, but runs the given callback at the given interval. |
| `v8::juice::clearIntervalThread()` | `clearIntervalThread(timerID)` | Removes a scheduled `setIntervalThread()` callback. |
| `v8::juice::sleep()` | `sleep(seconds)` | Sleeps for a given number of seconds, unlocking the v8 VM for use by other threads during that time. |
| `v8::juice::mssleep()` | `mssleep(milliseconds)` | Sleeps for a given number of milliseconds, unlocking the v8 VM for use by other threads during that time. |
| `v8::juice::usleep()` | `usleep(microseconds)` | Sleeps for a given number of microseconds, unlocking the v8 VM for use by other threads during that time. |

Passing 0 to any of the `sleep` variants will briefly unlock the v8 engine for other threads, and this can be used to implement a `yield` operation.

Due to internal details, `clearTimeout()` and `clearInterval()` are interchangeable, and either can be used to clear IDs returned from `setTimeout()` or `setInterval()`. Likewise for `spawnXXXThead()` and `clearXXXThread()`.

Speaking of `setTimeout()`:

**ACHTUNG:** (Yes, another one!) These functions do not work _identically_ to their browser-side counterparts. These actually run in a different thread, whereas browsers use a single thread and once the timeout function is called it will run until finished before returning control to the main code (or another timeout function, or whatever). In practice, however, i don't believe that this will make a big difference in how these functions are used, compared to the same-named browser-side functions. The current `setTimeout()` and `setInterval()` implementations will be replaced at some point to behave more like their browser-side counterparts (assuming i can figure out how to do it).

It has come to my attention that there is a JS-only implementation of `setTimeout()` and friends available here:

  * http://github.com/visionmedia/js-mock-timers/

and that one seems to be more browser-compliant. i've forked a copy of that code, cleaned it up, expanded it a bit, and documented it:

  * http://code.google.com/p/v8-juice/source/browse/branches/edge/src/client/shell/js/MockTimer.js

That code requires that the client provide his own timing loop, but that is simple to add. The forked copy demonstrates at least three ways of implementing such a loop from JS code, using various combinations of the above functions. For browser environments it uses the standard `setInterval()` to control the pseudo-timer loop, while providing its own timers on top of that.