# PluginPosixPoll #

This plugin provides a function called `posix.poll()` which is similar to `setInterval()`, but it runs synchronously. That is, it keeps running until the operation it is looping on triggers it to stop.

It is arguably mis-named. It uses `poll(3)` for the internal implementation, but does not actually watch for any input/output like `poll()` does. It simply runs code in a timed loop, in such a way that other v8-using threads can run while the loop is waiting.

To be honest, i can think of few uses for it, but it seemed like a nice exercise at the time, so here it is. The same effect can easily be implemented without this plugin by using the `mssleep()` function provided by v8-juice.

It can be downloaded from:

  * http://code.google.com/p/v8-juice/source/browse/#svn/extra-plugins/src/PosixPoll

It can be built (assuming v8-juice is installed) like:

```
~> make
# Optionally:
~> v8-juice-shell test.js
~> make install
```


It can be loaded from JS like this:

```
loadPlugin('v8-juice-PosixPoll');
```

And used like this:

```

var App = {
   loopCount:0;
   maxLoops:5;
};
function myLoop() {
   ... do something here ...
}
function doUntil() {
   if( ++App.loopCount == App.maxLoops ) {
      App.loopCount = 0;
      return false;
   }
   return true;
}

posix.poll( 1000/*milliseconds*/, myLoop, doUntil );
```

It will run the `myLoop()` function every 1000 milliseconds, stopping only when:

  * `myLoop()` or `doUntil()` throw an exception. In these cases, `poll()` propagates the exception to the caller.
  * `doUntil()` returns a false value (e.g. null, undefined, an empty string, or the number 0). In this case `poll()` returns normally (but its return value is unspecified).
  * The C-level `poll()` call returns non-0. In this case a JS-side exception is thrown.

If no do-until function is provided, the loop runs one time, after the specified timeout, and returns (or propagates an exception, if one is thrown).

While `poll()` is waiting on the C-level routine to time out, the v8 engine is unlocked so that other threads can use it while we're waiting on the timeout.