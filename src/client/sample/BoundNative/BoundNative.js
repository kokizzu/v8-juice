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

function testOne()
{
    var ar = [];
    var b = null;
    var total = 0;

    for( var i = 0; i < 5; ++i )
    {
        var c = 2;
        print("Creating",c,"objects...");
        b = createThem(c);
        total += c;
        ar.push( b ); // keep a reference to ensure that gc doesn't reap it.
        print("b ==",b);//"b.foo ==",b.foo);
        //gc();
        print("Created",c,"objects...");
    }
    print("Done. Created",total,"objects in total.");
    print('b.toString() ==',b.toString());
    print('b.toString2() ==',b.toString2());
    print('b.version() ==',b.version());
    print('b.doSomething() ==',b.doSomething("hi, world"));
    print('b.getInt() ==',b.getInt());
    print('b.setInt(17) ==',b.setInt(17));
    print('b.getInt() ==',b.getInt());
    if(1)
    {
        var b2 = new BoundNative();
        print('b.ptr(b) ==',b.ptr(b));
        print('b.ptr(b2) ==',b.ptr(b2));
        print('b2.ptr(0) ==',b2.ptr(0));
        delete b2;
    }
    if(1)
    {
        var stime = 1;
        print("b.sleep("+stime+")...");
        b.sleep(stime);
        print("b.sleep("+stime+")'d!");
        print('b.instanceCount() ==',b.instanceCount());
        print('BoundNative.instanceCount() ==',BoundNative.instanceCount());
    }
}
testOne();

function testTwo()
{
    print('BoundNative.instanceCount() ==',BoundNative.instanceCount());
    var b = new BoundNative;
    print('BoundNative.instanceCount() ==',BoundNative.instanceCount());
    b.destroy();
    print('BoundNative.instanceCount() ==',BoundNative.instanceCount());
}
testTwo();
//gc();
//print('BoundNative.instanceCount() ==',BoundNative.instanceCount());
print("Done! You win!");

