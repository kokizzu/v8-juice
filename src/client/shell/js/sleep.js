var sleeptime = 10; // time to sleep while waiting for loops
var dms = 1900; // delay between setTimeout() loops
var count = 0; // loops count
var reps = 6; // max number of loops to run.
function foofunc()
{
    if( ++count <= reps )
    {
        print("Scheduling foofunc() in",dms," miliseconds.");
        setTimeout( foofunc, dms );
    }
    else
    {
        print("Loop limit reached. foofunc() not looping.");
    }
}

foofunc();
print("sleeping",sleeptime,"seconds:",new Date());
for( var x = sleeptime; x > 0; --x )
{
    sleep(1);
    print("Zzzzzz....");
}
print("slept",sleeptime,"seconds:",new Date());
print("Calling foofunc() manually once:");
foofunc();

