var m = new MyNative();
MyNative.prototype.toString = function()
{
    return '{MyNative str:'+this.str+'}';
}
print('void returns:',m.avoid(),m.avoid1(32),m.avoid2(17,11));
print(m.hi());
print(m.func1());
print(m.func2(m.func1()));
print(m.func3(m.func1(),m.func2(m.func1())))
print(m.him(m));
print(m.me());
print(m.takes3(2,2,3));
print('m.str =',(m.str='bye, world!'));
print('m.other =',m.other);
m.other = new MyNative();
m.other.str = "i am the other!";
print('m.other.str =',m.other.str,m.other.me());
print('forwarder() =',m.other.forwarder(12,-13,8));
print('someref1() =',m.someref1(m.other));
print('proxiedProp =', m.proxiedProp = 4 );
m.proxiedProp += 23;
print('proxiedProp =', m.proxiedProp );
print('proxiedProp += 7 ==', (m.proxiedProp += 7) );
print('proxiedProp =', m.proxiedProp );
var ex = null;
try
{
    print('m.someref1(non-MyNative) =',m.someref1(undefined));
}catch(e)
{
    ex = e;
    print("Caught expected exception:",e);
}
if( null === ex ) throw("The last test should have thrown but didn't!");

ex = null;
print("my.other.destroy() ==",m.other.destroy());
try
{
    print('undefined.someref1(non-MyNative) =',m.other.str);
}catch(e)
{
    ex = e;
    print("Caught expected exception:",e);
}
if( null === ex ) throw("The last test should have thrown but didn't!");

print("Done!");

if(0)
{
    function SubType()
    {
        this.__proto__ =
            new MyNative();
            //MyNative.call( this, Array.prototype.slice( arguments, [0] ) );//.call(this);
        print('proto =',this.prototype,'str=',this.str);
        return this;
    }
    
    var sub = new SubType();
    sub.str = "sub.str";
    var sub2 = new SubType();
    sub2.str = "sub2.str";
    print(sub.str,sub2.str,sub.me(),sub2.me());
    print(sub.hi());
    print( sub2 instanceof MyNative );
    print("Done again!");
}

