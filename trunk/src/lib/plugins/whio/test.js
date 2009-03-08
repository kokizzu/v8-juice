load_plugin('v8-juice-whio');

var fname = "iodev.out";
var dev = //whio.IODevice.forFile("/etc/passwd", false );
    new whio.IODevice(fname, true );

for( var i = 0; i < 10; ++i )
{
    dev.write( "Line #"+(i+1)+"\n" );
}
//dev.truncate( 16 );
dev.close();

dev = new whio.IODevice(fname,false);
dev.seek( 5, whio.IODevice.SEEK_CUR );
dev.seek( 10, whio.IODevice.SEEK_CUR );
var rdSize = 13;
var rdRC = 0;
while( 0 != (rdRC = dev.read(rdSize)) ) {
    print('rdRC.length =',rdRC.length,'data=['+rdRC+']');
    if( rdRC < rdSize ) break;
}
print("File size:",dev.size());
dev.close();

if( 1 ) {
    var ostr = new whio.OutStream(fname);
    for( var i = 0; i < 10; ++i )
    {
	ostr.write( "This is line #"+i+'\n');
    }
    print("ostr =",ostr);
    ostr.close();
    //ostr.write("ouch"); // should throw
    var istr = new whio.InStream(fname);
    rdSize = 200;
    while( 0 != (rdRC = istr.read(rdSize)) ) {
	print('rdRC.length =',rdRC.length,'data=['+rdRC+']');
	if( rdRC < rdSize ) break;
    }
    istr.close();
}

if(1) {
    var os = new whio.OutStream("/dev/stdout");
    os.write("Hi, world!\n");
    os.close();
}

print("Output is in file",fname);

function tryMemory()
{
    var dev = new whio.IODevice(":memory:");
    print("Buffer size:",dev.size());
    dev.write("Hi, world!");
    dev.seek(4,whio.SEEK_SET);
    dev.write("Guido");
    dev.rewind();
    print("In-memory device says ["+dev.read(10)+']');
    print("Buffer size:",dev.size());
    dev.truncate(1024 * 1024);
    print("Grown buffer size:",dev.size());
    dev.truncate(10);
    print("Shrunk buffer size:",dev.size());

    dev.rewind();
    var ostr = new whio.OutStream( dev );
    var istr = new whio.InStream( dev );
    ostr.write("Hi from the stream!");
    dev.seek( 10 );
    print("Read back:",istr.read(10));
    dev.seek( 10 );
    ostr.write("xxx");
    dev.seek(10);
    print("Read back:",istr.read(10));
    ostr.close();
    istr.close();
    print("Buffer size:",dev.size());
    dev.close();
    //new whio.InStream( dev ); // should throw.
}

tryMemory();

function trySubdev()
{
    var fn = "subdev.out";
    var dev = new whio.IODevice(fn,true);
    for( var i = 0; i < 10; ++i )
    {
	dev.write(".........+");
    }
    var sub = new whio.IODevice(dev,10,20);
    for( var i = 0; i < 3; ++i )
    {
	if( 10 != sub.write("0123456789") ) break;
    }
    sub.close();
    dev.close();
    print("Check file",fn,"for subdevice test.");

    var dev = new whio.IODevice(":memory:");
    dev.write("012345679");

    var dump = function(){
        dev.rewind();
	print('dev contents: ',dev.read(10));
	sub.rewind();
	print('subdev contents: ',sub.read(10));
    };

    var sub = new whio.IODevice(dev, 4, 7 );
    sub.write("abcdefghij");
    dump();

    print('rebound:',sub.rebound);
    var rc = sub.rebound(3,8);
    print('rebound rc =',rc);
    sub.seek(1);
    print('tell =',sub.tell());
    sub.write("ZYXWVUTSRQ");
    print('sub.ioDevice =',sub.originDevice);
    print('sub =',sub);
    dump();    

    sub.close();
    dev.close();

}
trySubdev();

function tryBadStream()
{
    var a = null;
    var fn = '/etc/passwd';
    try {
	a = new whio.IODevice(fn,false);
	print(fn,'=',a,a.fileName);
	print(fn+'.canWrite =',a.canWrite);
	var o = new whio.OutStream(a); // should throw
	var i = new whio.InStream(a);
	print(i,'=',i);
	i.close();
	a.close();
    }
    catch(e) {
	print("As hoped, it threw:",e);
    } finally {
	print("finally{} closing",fn);
	if(a) a.close();
	return;
    }
    throw new Error("You shouldn't have gotten this far!");
}

tryBadStream();

function tryStreamFilter()
{
    // callback for use with filter()
    var cb = function(out,data,isEnd,udata) {
        if( 0 == data.length ) return 0;
	//print('data.length =',data.length,'isEnd =',isEnd);
	udata.hits += 1;
	udata.lastIn = data.length;
        var xl = data.replace(/([1357])/g,'$1*');
	var rc = out.write(xl,xl.length);
	if( rc != xl.length )
	{
	    throw new Error("Requested write of "+xl.length+" but got only "+rc+" :(!");
	}
	if( isEnd ) out.write("\n",1);
	return data.length;
    };
    var dev = new whio.IODevice(":memory:");
    for( var i = 0; i < 10; ++i )
    {
	dev.write("0123456789");
    }
    var os = new whio.OutStream("/dev/stdout");
    dev.rewind();
    var is = new whio.InStream(dev);
    print('dev =',dev);
    print('os =',os);
    print('is =',is);
    os.write("filter() test:\n");
    var ud = {hits:0,lastIn:0};
    var ex = null;
    try
    {
	dev.filter(os, cb, ud);
	dev.rewind();
	is.filter(os, cb, ud);
	//is.filter(is, cb, ud); // should throw
	//dev.filter(is, cb, ud); // should throw
    }
    catch(e) { ex = e; }
    finally
    { // Because of the 'is'--uses-->'dev' relationship, 'is' must close first:
	is.close();
	dev.close();
	os.write("\n");
	os.close();
	if( ex ) throw ex;
    }
    print('userData.lastIn='+ud.lastIn, 'userData.hits='+ud.hits);
}
tryStreamFilter();

function tryTypeInfo()
{
    print("Typeinfo:");
    var dev = new whio.IODevice('/dev/stdout',true);
    var os = new whio.OutStream('/dev/stdout');
    var is = new whio.InStream('/dev/stdin');
    var f = function(o,F) {
        var ack;
// 	if( F instanceof String ) F = eval(F);
// 	print(o,'instanceof',F,'==',o instanceof F);
	print(o,'instanceof',F,'==',o instanceof (( 'string' == typeof F) ? eval(F) : F));
    };
    f(dev,'whio.IODevice');
    f(is,'whio.InStream');
    f(is,'whio.IODevice');
    f(os,'whio.OutStream');
    f(os,'whio.InStream');
    f(os,'whio.IODevice');
    is.close();
    os.close();
    dev.close();
    print
}
tryTypeInfo();

print(":-D");
