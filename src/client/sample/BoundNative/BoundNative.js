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

    var loops = 2;
    for( var i = 0; i < loops; ++i )
    {
        var c = 2;
        print("Creating",c,"objects...");
        b = createThem(c);
        total += c;
        if(1)
        {
            ar.push( b ); // keep a reference to ensure that gc doesn't reap it.
            //b.destroy();
        }
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
    print('b.myInt ==',b.myInt);
    print('b.myInt += 17 ==',b.myInt += 17);
    print('b.myInt ==',b.myInt);
    print('b.intGetter ==',b.intGetter);
    print('b.intGetter=3 ==',b.intGetter=3);
    print('b.intGetter ==',b.intGetter);
    print('b.publicProperty ==',b.publicProperty);
    print('b.publicProperty/=3 ==',b.publicProperty/=3);
    print('b.publicProperty ==',b.publicProperty);
    print('b.publicPropertyRO ==',b.publicPropertyRO);
    print('b.publicPropertyRO=13 ==',b.publicPropertyRO=13);
    print('b.publicPropertyRO ==',b.publicPropertyRO);
    if(1)
    {
        var b2 = new BoundNative();
        print('b.ptr(b) ==',b.ptr(b));
        print('b.getPtr() ==',b.getPtr());
        print('b.ptr(b2) ==',b.ptr(b2));
        print('b2.ptr(0) ==',b2.ptr(0));
        delete b2;
    }
    if(0)
    {
        var stime = 1;
        print("b.sleep("+stime+")...");
        b.sleep(stime);
        print("b.sleep("+stime+")'d!");
    }
    print('b.instanceCount() ==',b.instanceCount());
    print("b.destroy()...");
    b.destroy();
    print('BoundNative.instanceCount() ==',BoundNative.instanceCount());
}
testOne();

function testTwo()
{
    print('BoundNative.instanceCount() before New ==',BoundNative.instanceCount());
    var b = new BoundNative;
    print('BoundNative.instanceCount() after New ==',BoundNative.instanceCount());
    b.destroy();
    print('BoundNative.instanceCount() after b.destroy() ==',BoundNative.instanceCount());
    //gc();
}
testTwo();
//gc();

function testInheritance1()
{
    function MyClass()
    {
        this.prototype = this.__proto__ = new BoundNative();
        this.prototype.constructor = MyClass;
        return this;
    }
    var m = new MyClass();
    print("m =",m);
    print("m instanceof BoundNative ==",m instanceof BoundNative);
    print("m.ptr(m)",m.ptr(m));
    if(1)
    {
        print('BoundNative.instanceCount() before m.destroy() ==',BoundNative.instanceCount());
        print("Calling m.destroy()...");
        m.destroy();
        print('BoundNative.instanceCount() after m.destroy() ==',BoundNative.instanceCount());
        print("m.ptr(m)",m.ptr(m));
        if( m.ptr(m) )
        {
            throw new Error("m.ptr(m) should fail after m.destroy()!");
        }
    }

}
print("BoundNative.supportsInheritance ==",BoundNative.supportsInheritance);
if( BoundNative.supportsInheritance )
{
    testInheritance1();
}
else
{
    print("Skipping inheritance tests.");
}

print('BoundNative.instanceCount() ==',BoundNative.instanceCount());
print("Done! You win!");

