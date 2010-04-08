/**

Mock Timers - Copyright TJ Holowaychuk <tj@vision-media.ca> (MIT Licensed)

Originally from:

http://github.com/visionmedia/js-mock-timers/blob/master/lib/timers.js

(Version 1.0.2, git ID e5d1a122c50151296282e771beb55cdb23c02d11)

This copy has been hacked significanty by Stephan Beal
(http://wanderinghorse.net/home/stephan/). Changes include:

- Encapsulated the core features in a class.

- Added semicolons to the end of all statements. JS doesn't require
this but not having them breaks code minifiers and
accidentally-reformatted code can have its logic inadvertently
changed.
*/


function MockTimer()
{
    this.resetTimers();
    return this;
}
/**
 * Destroy timer with _id_.
 *
 * @param  {int} id
 * @return {bool}
 * @api public
 */
MockTimer.prototype.clearIntervalImpl = function(id)
{
    return delete this.timers[--id]
};

/**
 * Localized timer stack.
 */
MockTimer.prototype.timers = [];
  
    /**
     * Set mock timeout with _callback_ and timeout of _ms_.
     *
     * @param  {function} callback
     * @param  {int} ms
     * @return {int}
     * @api public
     */
MockTimer.prototype.setTimeout = function(callback, ms)
{
    var id;
    return id = setInterval(function(){
                                callback();
                                this.clearInterval(id);
                            }, ms);
};
/**
 * Set mock interval with _callback_ and interval of _ms_.
 *
 * @param  {function} callback
 * @param  {int} ms
 * @return {int}
 * @api public
 */
MockTimer.prototype.setInterval = function(callback, ms)
{
    callback.mock = {step:ms, current:0,last:0};
    //callback.step = ms, callback.current = callback.last = 0;
    return this.timers[this.timers.length] = callback, this.timers.length;
};
/**
   Expects arguments[0] to be a timer ID which was returned from
   setTimeout() or setInterval(). If it is, it is removed from the
   timer queue, such that it will not be fired the next time its timer
   would normally have triggered.

   A setTimeout() handler is only called once, but clearing it will
   keep it from firing (if it is cleared in time, of course).
*/
MockTimer.prototype.clearTimeout = MockTimer.prototype.clearIntervalImpl;
/** Identical to clearTimeout(). */
MockTimer.prototype.clearInterval = MockTimer.prototype.clearIntervalImpl;
/**
 * Reset timers. All existing timers added via setTimeout() and setInterval()
 * are removed from the timer queue.
 *
 * @return empty {array}
 * @api public
 */
MockTimer.prototype.resetTimers = function()
{
    return this.timers = [];
};
/**
  Increment each timer's internal clock by _ms_. If a given timer's
  "time has come", its callback is called. A callback may be called
  multiple times in one tick() if its interval is smaller than the
  tick increment and it needs to "catch up". Thus events are not
  "lost" by a long tick, but may be run in very rapid order
  when catching up.

  @param  {int} ms
  @api public
*/
MockTimer.prototype.tick = function(ms)
{
    var tm = this.timers;
    for (var i = 0, len = tm.length; i < len; ++i)
    {
        var it = tm[i];
        //var mo = it.mock; // weird: i hold a reference to tm[i].mock i can get segfaults at app exit
        if (it && (it.mock.current += ms))
        {
            if (it.mock.current - it.mock.last >= it.mock.step) {
                var times = Math.floor((it.mock.current - it.mock.last) / it.mock.step);
                var remainder = (it.mock.current - it.mock.last) % it.mock.step;
                it.mock.last = it.mock.current - remainder;
                while (times-- && it) it();
            }
        }
    }
};

if( 'spawnIntervalThread' in this/* smells like v8-juice-shell or v8::juice::JuiceShell! */ )
{
    /**
       Starts an interval timer which calls this.tick(intervalMS) ever
       intervalMS milliseconds. If beforeTick is-a function then beforeTick(intervalMS)
       is called just before every call to this.tick().

       Any current and previous timers created via this.setTimeout() or this.setInterval()
       will be triggered using their own timing intervals, but only insofar as the
       granularity of intervalMS allows them to do so. If intervalMS is larger than
       any of the timers' intervals, then such timers may be called multiple times
       in immediate succession to "catch up". See tick() for details.

       IMO, a good default value for intervalMS is 100-250ms, or much
       higher if all of your timers are "slow" (only run at long
       intervals).  The library-wide default value is defined as
       MockTimer.prototype.startTickLoop.defaultIntervalMS, and that
       default is used if no arguments are passed in or if
       !intervalMS.

       If intervalMS is very small, this loop will require
       appropriately more runtime.

       Returns this object on success.

       It is illegal to call startTickLoop() more than once without a
       subsequent call to this.stopTickLoop() between the calls. Doing so will
       cause this function to throw an exception.
    */
    MockTimer.prototype.startTickLoop = function( intervalMS, beforeTick )
    {
        if( this.loopInfo )
        {
            throw new Error( "MockTimer.startTickLoop() must only be called once!" );
            return false;
        }
        if( ! arguments.length || !arguments[0] )
        {
            intervalMS = MockTimer.prototype.startTickLoop.defaultIntervalMS;
        }
        var loopInfo = { ms:0+intervalMS };
        this.loopInfo = loopInfo;
        var self = this;
        loopInfo.timerID = spawnIntervalThread( function()
                                                {
                                                    if( beforeTick instanceof Function )
                                                    {
                                                        beforeTick(loopInfo.ms);
                                                    }
                                                    self.tick(loopInfo.ms);
                                                },
                                                loopInfo.ms );
        return this;
    };
    MockTimer.prototype.startTickLoop.defaultIntervalMS = 200;
    /**
       Sticks the startTickLoop() loop. By default it also clears
       all timers, but if passed the explicit boolean value
       false (i.e. false===arguments[0]) then it will not clear
       the timeouts. They will not be run, but they can be
       started again by calling startTickLoop() again.
    */
    MockTimer.prototype.stopTickLoop = function()
    {
        if( ! this.loopInfo || ! this.loopInfo.timerID )
        {
            throw new Error( "MockTimer.startTickLoop() is not running!" );
            return false;
        }
        clearIntervalThread( this.loopInfo.timerID );
        if( !arguments.length || (false!==arguments[0]) )
        {
            this.resetTimers();
        }
        this.loopInfo = undefined;
        delete this.loopInfo;
        return this;
    };
}

/* Test/debug code. */
if(1) (function(){
           function timestamp()
           {
               var t =  (new Date()).getTime();
               var rc = t + ' (+'
                   +( t - arguments.callee.prev )
                   +'): '
                   ;
               arguments.callee.prev = t;
               return rc;
           }
           timestamp.prev = (new Date()).getTime();
           
           MockTimer.test1 = function()
           {
               var pt = new MockTimer();
               function one()
               {
                   print("one()");
               }
               function labelFunc(s)
               {
                   return function(){ print(s); };
               }

               var t = pt.setInterval( labelFunc("Hi!"), 3 );
               var i;
               var tick = 1;
               for( i = 0; i < 10; ++i )
               {
                   print( i );
                   pt.tick( tick );
               }
           };

           MockTimer.test2 = function()
           {
               var pt = new MockTimer();
               pt.runCount = 0;
               pt.countdown = 10;
               function labelFunc(s)
               {
                   return function(){
                       if( 1 == pt.countdown ) pt.clearInterval( pt.myID );
                       --pt.countdown;
                       print(pt.countdown, timestamp(),++pt.runCount,':',s);
                   };
               }
               pt.startTickLoop( 300, function() {
                                     print('tick...');
                                 } );
               pt.myID = pt.setInterval( labelFunc("tock!"), 250 );
               print("setInterval() =",pt.myID);
               for( ; pt.countdown > 0; ) {
                   print('... main thread still waiting ...',pt.countdown);
                   sleep(1);
               }
               print('Main thread done.');
               pt.stopTickLoop();
           };

           if( 1 && ('v8JuiceVersion' in this /* == is v8-juice-shell or v8::juice::JuiceShell client*/) )
           {
               MockTimer.test1();
               MockTimer.test2();
               //sleep(3);
           }
})();
