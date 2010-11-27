
var factor = 1.2;
var mPerSec = 1.5;
var max = 10;
var speed = mPerSec;
print("Scale\tSpeed\tM/Rnd\tM/Min");

for( var i = 0; i <= max; ++i )
{
    var mprnd = (speed * 10);
    var mpmin = mprnd*6;
    //var mprnd = Math.ceil( (speed * 10) );
    //var mpmin = Math.ceil( mprnd*6 );
    //print("Scale",(i),"speed =",speed,"meters/second, ",mprnd," per round,",mpmin," per minute");
    print(sprintf("%d\t%0.2f\t%0.2f\t%0.2f",i,speed,mprnd,mpmin));    
    speed = speed * factor;
}

speed = mPerSec;
for( var i = 0; i >= (-1*max); --i )
{
    var mprnd = (speed * 10);
    var mpmin = mprnd*6;
    //print("Scale",(i),"speed =",speed,"meters/second, ",mprnd," per round,",mpmin," per minute");
    print(sprintf("%d\t%0.2f\t%0.2f\t%0.2f",i,speed,mprnd,mpmin));
    speed = speed / factor;
}
