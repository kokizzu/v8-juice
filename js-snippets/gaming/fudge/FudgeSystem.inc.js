/**
   Returns an object for use with include(), e.g.:

   fudge = include('FudgeSystem.inc.js');
   print( fudge.rollDF() );

   The returned object contains some utilities for the Fudge gaming
   system.

*/
(function() {
    var fudge = {};
    /** Returns a pseudorandom number in the inclusive range [-1,+1]. */
    function df() { return Math.floor( (Math.random() * 9999999 % (3))) - 1; };
    /** Rolls n Fudge dice, returning the accumulated result. */
    fudge.rollDF = function (n) {
        if( 0 == arguments.length ) n = 4;
        var rc = 0;
        for( var i = 0; i < n; ++i )
        {
            rc += df();
        }
        return rc;
    };

    fudge.adjectives = [ 'Abysmal',
                         'Terrible',
                         'Poor',
                         'Mediocre',
                         'Fair',
                         'Good',
                         'Great',
                         'Superb',
                         'Legendary'];

    /**
       For the given integer values, a string is returned
       for the "adjective form" of the value. e.g. -2 == "Poor"
       and +5 == "Legendary+1".
    */
    fudge.getAdjective = function(n) {
            n = +n;// +n = workaround for v8 treating n like a string
            var key = (n + 4);
            if( key in fudge.adjectives ) return fudge.adjectives[key];
            else if( n < 0 ) return fudge.adjectives[0]+''+(n+4);
            return fudge.adjectives[fudge.adjectives.length-1]+'+'+(n-4);
    };

    /**
       A class for calculating values for Fudge Scales.

       Properties:

       .multipler = multipler for this Scaler type. e.g. Mass==1.5, Speed=1.2

       .baseValue = scale-specific "scale 0" value. e.g. kg for mass,
       m/sec for speed.

       .medianValue(scaleLevel) returns the median scale-specific value
       for the given scale level.
    */
    fudge.Scaler = function(mult,baseval)
    {
        this.multiplier = mult;
        this.baseValue = baseval ? baseval : 0;
        this.medianCache = {};
        return this;
    };

    fudge.Scaler.prototype = {
        /**
           For the given scale (integer value) the median value of that
           scale is returned, using this.baseValue. If (0===scale)
           then this.baseValue is returned.

           If my brain was bigger i could use log() to do this instead
           of a loop.
        */
        medianValue:function( scale )
        {
            var val = this.baseValue;
            if( ! scale ) return val;
            var key = scale;
            var check = this.medianCache[key];
            if( undefined !== check )
            {
                return check;
            }
            var inc = (scale > 0) ? -1 : 1;
            var self = this;
            function step( x )
            {
                return (inc<0) ? (x * self.multiplier) : (x / self.multiplier);
            }
            var rc = val;
            for( ; scale != 0; scale += inc )
            {
                rc = step( rc );
            }
            this.medianCache[key] = rc;
            return rc;
        }
    };
    
    fudge.scales = {
        strength:(new fudge.Scaler( 1.5, 1/*relative value*/ )),
        mass:(new fudge.Scaler( 1.5, 75/*kilograms*/ )),
        speed:(new fudge.Scaler( 1.2, 1.5/*meters/second*/ ))
    };

    return fudge;
})();
