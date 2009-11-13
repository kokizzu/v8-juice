if( ! ('posix' in this) || (!posix.FILE) )
{
    loadPlugin("posix.FILE");
}

var fi = { name:"my.file", mode:"w+" };
var ns = posix;
var f =
    new ns.FILE(fi.name,fi.mode)
    //ns.fopen("my.file","w+")
    ;
print("f =",f);
f.write("Hello, world!\n");
ns.fwrite("Bye, world!\n",f);
print( "tell =",f.tell());
var pos = f.seek( 3, ns.SEEK_SET );
print( "tell =",f.tell());
print("feof() =",ns.feof(f));
pos = f.seek( 0, ns.FILE.SEEK_END );
print("feof() =",ns.feof(f));
ns.rewind(f);
print( "ftell =",ns.ftell(f));
pos = ns.fseek( f, -1, ns.SEEK_SET );
print( "ftell =",ns.ftell(f));
print("errno =",f.errno);
pos = ns.fseek( f, 7, ns.SEEK_SET );
print("errno =",f.errno);
var c = ns.fread(4,f);
print("read() =",c);
ns.fsync(f.fileno());
ns.fflush(f);
ns.fclose(f);
//f.close();
print("Done!");
