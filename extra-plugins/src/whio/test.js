load_plugin('v8-juice-whio');

function showErrCodes()
{
    print("whio.rc result codes:");
    for( var k in whio.rc )
    {
        print("\twhio.rc."+k,'==',whio.rc[whio.rc[k]],'==',whio.rc[k]);
    }
    print('whio.SEEK_CUR =',whio.IOBase.SEEK_CUR);
    print('whio.SEEK_END =',whio.IOBase.SEEK_END);
    print('whio.SEEK_SET =',whio.IOBase.SEEK_SET);

}
function tryOne() {
    var fname = "iodev.out";
    var dev = //whio.IODevice.forFile("/etc/passwd", false );
	new whio.IODevice(fname, true );

    for( var i = 0; i < 10; ++i )
    {
	dev.write( "Line #"+(i+1)+"\n" );
    }
    //dev.truncate( 16 );
    print('dev.isGood() ==',dev.isGood(), "iomode =",dev.ioMode());
    dev.close();

    dev = new whio.IODevice(fname,false);
    dev.seek( 5, whio.IOBase.SEEK_CUR );
    //dev.seek( 10, whio.IODevice.SEEK_CUR ); // doesn't work via whio.IODevice :/
    dev.seek( 10, whio.IOBase.SEEK_CUR );
    if( dev.tell() != 15 ) {
        throw new Error("I/O device is not at its intended position:",dev.tell());
    }
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
}


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
    print('sub.ioDevice =',sub.ioDevice);
    print('sub =',sub);
    dump();    

    sub.close();
    dev.close();

}

function tryBadStream()
{
    var a = null;
    var fn = '/etc/passwd';
    try {
	a = new whio.IODevice(fn,false);
	print(fn,'=',a,a.fileName);
	print(fn+'.canWrite =',a.canWrite,"iomode =",dev.ioMode());
	var o = new whio.OutStream(a); // should throw
	var i = new whio.InStream(a);
	print(i,'=',i);
	i.close();
	a.close();
    }
    catch(e) {
	print("As hoped, it threw:",e);
	print("finally{} closing",fn);
	return;
    }
    finally {
	if(a) a.close();
    }

    throw new Error("You shouldn't have gotten this far!");
}



function tryTypeInfo()
{
    print("Typeinfo:");
    var dev = new whio.IODevice('/dev/stdout',true);
    var stdin = new whio.IODevice('/dev/stdin',false);
    var os = new whio.OutStream('/dev/stdout');
    var is = new whio.InStream('/dev/stdin');
    var typeinfo = function(o,F) {
        var ack;
// 	if( F instanceof String ) F = eval(F);
// 	print(o,'instanceof',F,'==',o instanceof F);
	print(o,'instanceof',F,'==',o instanceof (( 'string' == typeof F) ? eval(F) : F));
    };
    var sar = [os,is,dev,stdin];
    for( var ks in sar  )
    {
	var strdev = sar[ks];
	var types = ['whio.IODevice','whio.IOBase','whio.InStream','whio.OutStream'];
	for( kt in types )
	{
	    typeinfo(strdev,types[kt]);
	}
	print('('+strdev+').isGood() ==',strdev.isGood());
	strdev.close();
    }
    //is.write(7); // should throw
}

function tryOnlyOut()
{
    var os = new whio.OutStream("/dev/null",false);
    os.close();
    //os.close(); //should throw
}

function tryAbstract()
{
    var ex = null;
    try
    {
	//var s = new whio.StreamBase();
	var s = new whio.OutStream('/dev/null');
	s.close();
	s.close();
    }
    catch(E) {
	ex=E;
	print("Caught expected exception:",E);
    }
    finally
    {
	if( ! ex ) throw("We should have gotten an exception!");
    }
}

function tryByteArray()
{
    var ba = new whio.ByteArray(100);
    ba.write("hi, world");
    print(ba);
    ba.close();
}

function tryGzip()
{
    var fname = "test.js";
    var outname = fname + ".gz";
    var ist = new whio.InStream(fname);
    var ost = new whio.OutStream(outname);
    var rc = ist.gzipTo(ost);
    print("gzip rc =",rc,'outfile =', outname);
    ist.close();
    ost.close();
    if( whio.rc.OK != rc ) throw new Error("Gzip failed with code "+rc);
    ist = new whio.InStream(outname);
    outname = outname + '.check';
    ost = new whio.OutStream(outname);
    rc = ist.gunzipTo(ost);
    print("gunzip rc =",rc,'outfile =', outname);
    ist.close();
    ost.close();
    if( whio.rc.OK != rc ) throw new Error("Gunzip failed with code "+rc);
}


showErrCodes();
tryOne();
tryOnlyOut();
tryMemory();
trySubdev();
tryBadStream();
tryTypeInfo();
tryAbstract();
//tryByteArray();
tryGzip();

print(":-D");

