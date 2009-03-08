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
