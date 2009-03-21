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

    var fs = new whefs.WHEFS(store);

    var fi = fs.openDevice("first.file",true);
    fi.write("Hi, world!\n");
    fi.close();
    fs.close();
    store.close();

    fname = 'test.2.whefs';
    var fsopts = {
    blockSize:4095,
    inodeCount:100,
    blockCount:100,
    filenameLength:32
    }

    print("Creating whefs filesystem in file",fname,'...');
    fs  = whefs.mkfs(fname,fsopts);
    print('fs =',fs);
    fi = fs.openDevice("second.file",true);
    fi.write("Hi, world!\n");
    fi.close();
    fs.close();
}


showErrCodes();
tryOne();

print(":-D");

