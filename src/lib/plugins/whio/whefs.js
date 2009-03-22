load_plugin('v8-juice-whio');

function showErrCodes()
{
    print("whefs.rc result codes:");
    for( var k in whefs.rc )
    {
        print("\twhefs.rc."+k,'==',whefs.rc[whefs.rc[k]],'==',whefs.rc[k]);
    }
}

function tryOne() {
    var fname = "test.whefs";
    print("Using whefs file",fname);
    var store = new whio.IODevice(fname,true);

    var fs =
    //new whefs.WhEfs(fname,true);
        new whefs.WhEfs(store);

    var fi = fs.openPseudofile("first.file",true);
    fi.write("Hi, world!\n");
    fi.close();
    fs.close();
    store.close();

    fname = 'test.2.whefs';
    var fsopts = {
    blockSize:4096,
    inodeCount:100,
    blockCount:100,
    filenameLength:32
    }

    print("Creating whefs filesystem in file",fname,'...');
    fs  = whefs.mkfs(fname,fsopts);
    print('fs =',fs);
    var pfname = 'second.file';
    fi = fs.openPseudofile(pfname,true);
    fi.write("Hi, world!\n");
    fi.close();
    print("File list:",fs.ls('*'));
    var rc = fs.unlink(pfname);
    print("unlink rc =",rc);
    print("File list:",fs.ls('*'));
    fi = fs.openPseudofile(pfname,true);
    fi.write("Hi again, world!\n");
    fi.close();
    print("File list:",fs.ls(''));
    fs.close();
}

function tryMemory()
{
    var fsopts = {
    blockSize:(1024*32),
    inodeCount:1024,
    blockCount:(1024*2),
    filenameLength:128
    };
    print("Creating in-memory vfs...");
    var fs = whefs.mkfs(":memory:");
    print("Created in-memory vfs. Size =",fs.size());

    var istr = new whio.InStream("/etc/hosts");
    var fz = fs.openPseudofile("/etc/hosts.gz",true);
    fz.truncate(0); // make sure we don't mix old data with it
    print('fz =',fz.fileName);
    var ostr = new whio.OutStream(fz);
    var rc = istr.gzipTo(ostr);
    print('gzip rc =',rc);
    istr.close();
    ostr.close();
    fz.close();

    fz = fs.openPseudofile("/etc/hosts.gz",false);
    istr = new whio.InStream(fz);
    ostr = new whio.OutStream('/dev/stdout',false);
    print(">>-----------------------------------------<<");
    istr.gunzipTo(ostr);
    istr.close();
    ostr.close();
    print(">>-----------------------------------------<<");
    fz.close();

    fs.dumpToFile("memory.whefs");
    print(fs);
    fs.close();
}

showErrCodes();
tryOne();
tryMemory();

print(":-D");

