function createThem(count)
{
    print("Constructing",count,"BoundNative objects.");
    function interruptionPoint()
    {
        mssleep(1);
    }
    var b;
    for( var i = 0; i < count;++i )
    {
        b = new BoundNative();
        //delete b;
        if( 0 == (i%333) )
        {
            print("Interruption point.");
            //gc();
            interruptionPoint();
        }
        
    }
    print('b ==',b,'typeof b ==',typeof b);
    print("Constructed",count,"BoundNative objects.");
    return b;
}
var ar = [];
var b = null;
var total = 0;

for( var i = 0; i < 5; ++i )
{
    var c = 2;
    print("Creating",c,"objects...");
    b = createThem(c);
    total += c;
    //ar.push( b ); // keep a reference to ensure that gc doesn't reap it.
    print("b ==",b);//"b.foo ==",b.foo);
    //gc();
    print("Created",c,"objects...");
}
print("Done. Created",total,"objects in total.");
print('b.toString() ==',b.toString());
print('b.toString2() ==',b.toString2());
print('b.version() ==',b.version());
print('b.doSomething() ==',b.doSomething("hi, world"));
print("b.sleep(2)...");
b.sleep(2);
print("b.sleep'd(2)...");
