var m = new MyNative();
print('void returns:',m.avoid(),m.avoid1(32),m.avoid2(17,11));
print(m.hi());
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


