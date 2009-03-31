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
// print("Setting other to null.");
//m.other = null; // .other is marked as R/O: shouldn't do anything
//print("Set other to null.");
m.other.str = "i am the other!";
//print("Re-assigned other.str.");
print('m.other.str =',m.other.str,m.other.me());
print('forwarder() =',m.other.forwarder(12,-13,8));
print('someref() =',m.someref(m.other));
var ex = null;
try
{
    print('someref(non-MyNative) =',m.someref(m.other.str));
}catch(e)
{
    ex = e;
    print("Caught expected exception:",e);
}
if( null === ex ) throw("The last test should have thrown but didn't!");
print("Done!");

if(1)
{
    function SubType()
    {
        this.__proto__ =
            //new MyNative();//.call(this);
            MyNative.call( this, Array.prototype.slice( arguments, [0] ) );//.call(this);
        print('proto =',this.prototype,'str=',this.str);
        return this;
    }
    
    var sub = new SubType();
    sub.str = "sub.str";
    var sub2 = new SubType();
    sub2.str = "sub2.str";
    //sub.prototype = m;
    print(sub.str,sub2.str,sub.me(),sub2.me());
    print(sub.hi());
    print( sub2 instanceof MyNative );
    print("Done again!");
}

