

# Basic multi-threading in JavaScript #

Updated 20100408 for the `spawnTimeoutThread()` family of functions (see [BindableFunctions](BindableFunctions.md)).


## spawnTimeoutThread() and friends ##

The functions `spawnTimeoutThread()` and `spawnIntervalThread()` are interface-compatible with the standard `setTimeout()` and `setInterval()` functions, but they have a subtle difference: they use their own threads instead of running in the main thread. They may (indirectly) interrupt (and be interrupted by) the main thread at arbitrary point during their execution.

They are used like this:

```
function timeout()
{
   print("Timeout reached!");
}

spawnTimeoutThread(timeout,500 /*milliseconds*/);
sleep(3); // see notes below
```

That will start a second thread and schedule the function `timeout()` to be called one time in 500 milliseconds (0.5 seconds). The call to `sleep()` there is only to give the timeout function time to fire. It also incidentally unlocks the v8 engine for the duration of the sleep, allowing the timeout thread to run. Without the sleep(), the timeout may never run! Using sleep like this is _not_ a reliable way of ensuring that a certain timeout has been run, and is only used here for demonstration purposes.

To call a function at intervals, use `spawnIntervalThread()` instead of `spawnTimeoutThread()`, or have your timeout handler call `spawnTimeoutThread()` when it is finished processing (that is less efficient, because it ends up destroying one thread only to replace it with a functionally identical one).

The `spawnTimeoutThread()` and `spawnIntervalThread()` functions return a unique ID which can be passed to `clearIntervalThread()` or `clearTimeoutThread()` in order to cancel the calling of the callback function (assuming the clear routine is called before the timeout expires).  When `spawnTimeoutThread()` returns without rescheduling itself, the enclosing thread is destroyed automatically. Likewise, when `clearTimeoutThread()` or `clearIntervalThread()` are used, the thread will be destroyed automatically the next time it wakes up to execute.

In this implementation `clearIntervalThread()` is equivalent to `clearTimeoutThread()`, and either can be used for both types of timeout IDs. Two functions are only provided for symmetry with the `setTimeout()` API.

# Major Limitations #

The JavaScript language specification does not define any sort of locking behaviour in the face of multiple threads. It is _impossible_, using JS-only constructs, to write 100% thread-safe code in JavaScript. Keep that in mind when making use of `setTimeout()`.

## There is no Preemtion ##

The v8 engine is internally single-threaded. At certain points during JS execution it checks to see if a thread is waiting to make use of of the engine (via constructing a `v8::Locker` object in native code). If you have a JS thread which is sitting in a tight loop and never triggers one of the v8-internal thread-preemtion checks (e.g. it never calls a v8 function) then other threads might never get to run.

The `sleep()` family of functions will unlock the v8 engine while they are sleeping, and can be used in a thread to give v8 a chance to allow other threads to run. v8 _might_ do that anyway, but will only do so at certain points in its execution (namely function calls). Calling `sleep(0)` is one way to emulate a "yield" operation, as that unlocks the engine, allowing v8 to pass control to other threads, and then locks it again when control is given back to the original thread.

## Timing is Everything (and undefined) ##

The addition of multiple threads in a process always adds some level of uncertainty in the timing of execution of the various threads. High-level languages which use unpredictable GC (and/or lack destructors) add even more uncertainty (destructors are a great way to simplify locking in languages which support them). Threads created via `spawnTimeoutThread()` and friends cannot be reliably used for high-resolution threading (that is, where threads interact in very short intervals).

In the following code it is impossible to know with certainty which of the operations, running or cancelling the timeout, will happen first, due to the imprecision introduced via multi-threading and system-level timing issues:

```
clearTimeoutThread( spawnTimeoutThread(function(){}, 1) );
```

Do not rely on JS code for this type of precision. If you need that level of thread control, do it in native code and (if needed) add a higher-level JS wrapper around that.

# Future possibilities #

"It would be nice" to have a JS-side Thread class, which could be called like:

```
var t = new Thread( callbackfunc );
t.start();
while( t.isRunning() )
{
   print("Waiting on thread...");
   mssleep(500);
}
```

however, i'm not yet up to that task, and currently can see little benefit over using `setTimeout()` or `spawnTimeoutThread()`, given all of the JS-implied limitations of the hypothetical Thread class. It's also not clear whether such a class should run in a detached or joinable mode (`spawnTimeoutThread()` is detached).