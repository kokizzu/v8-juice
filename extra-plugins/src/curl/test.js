loadPlugin('v8-juice-libcurl');

var banner = {
    start:'>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>',
      end:'<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<'
        };

function testOne()
{
    print(banner.start);
    print( arguments.callee.name+"()" );
    var c = new Curl();
    c.setOpt({
        url:'http://code.google.com/p/v8-juice/wiki/PluginCurl',
        userAgent:"Google Chrome, kind of.",
        //verbose:true,
        //noBody:true,
        writeFunction:function writeFunction(data,len,ud)
        {
            print(arguments.callee.name+"()",data.length,"of",len,"bytes");
            ++ud.count;
            return data.length;
        },
        writeData:{count:0},
        //header:true,
        headerFunction:function headerFunction(data,len,ud)
        {
            print(arguments.callee.name+"()",data.length,"of",len,
                  "bytes:","["+data.substring(0,data.length-2)+"]");
            ++ud.count;
            return data.length;
        },
        headerData:{count:0},
        placeholder:undefined
    });
    var rc = c.easyPerform();
    print( "c.easyPerform() rc =",rc);
    print(c,'=',JSON.stringify(c,undefined,2));
    c.destroy();
    print(banner.end);
};
function testTwo()
{
    print(banner.start);
    print( arguments.callee.name+"()" );
    var c = new Curl();
    print( "Curl obj =",JSON.stringify( c, undefined, 2 ) );
    c.setOpt({
        //verbose:true,
        port:80,
        url:"http://s11n",
        writeFunction:function writeFunction(data,len,ud)
        {
            print(arguments.callee.name+"()",data.length,"of",len,"bytes");
            ++ud.count;
            return data.length;
        },
        writeData:{count:0},
        headerFunction:function headerFunction(data,len,ud)
        {
            print(arguments.callee.name+"()",data.length,"of",len,"bytes:","["+data.substring(0,data.length-2)+"]");
            return data.length;
        },
        headerData:{count:0},
        userAgent:"v8-juice-curl",
        placeholder:null
    });
    print( "port =",c.opt.port);
    print( "timeout =",c.opt.timeout);
    var rc = c.easyPerform();
    print("perform rc =",rc);
    print("Curl obj =",JSON.stringify(c,undefined,2));
    //print("Curl.opt obj =",JSON.stringify(c.opt,undefined,2));
    c.destroy();
    print(banner.end);
}
function testThree()
{
    print(banner.start);
    print( arguments.callee.name+"()" );
    var c = new Curl();
    var o = {
        url:'http://wh',
        //verbose:true,
        port:80,
        timeout:11,
        connectionTimeout:17,
        headerFunction:function headerFunction(data,len,ud)
        {
            print(arguments.callee.name+"()",data.length,"of",len,"bytes:","["+data.substring(0,data.length-2)+"]");
            return data.length;
        },
        headerData:{count:0},
        httpHeader:["Host: s11n","Accept-Encoding: gzip","Accept: text/html"],
        http200Aliases:['wh','s11n']
    };
    rc = c.setOpt(o);
    print( "setOpt RC =",rc);
    rc = c.setOpt( Curl.OPT_HTTPHEADER,
                   ["Host: wh","Accept-Encoding: gzip","Accept: text/html"]
                   );
    print( "setOpt RC =",rc);
    var rc = c.setOpt( Curl.OPT_CRLF, 0 )
    print( "setOpt RC =",rc);
    rc = c.setOpt( 'userAgent', "v8-juice" );
    print( "setOpt RC =",rc);
    var rc = c.setOpt( Curl.OPT_URL, 'http://localhost' )
    print( "setOpt RC =",rc);
    print('c.opt.url ==',c.opt.url);
    c.setOpt( Curl.OPT_WRITEFUNCTION, function writeFunction(data,len,ud)
    {
        print(arguments.callee.name+"()",data.length,"of",len,"bytes");
        ++ud.count;
        return data.length;
    } );
    c.setOpt( Curl.OPT_WRITEDATA, {count:0} );

    print( "c =",JSON.stringify(c,undefined,2));
    c.easyPerform();
    c.destroy();
    print(banner.end);
}

function testCrash()
{
    /**
       v8 weirdness/bug:

       If we set an slist property, e.g. httpHeader to an invalid
       value (e.g. an integer) as part of setOpt(object), v8 crashes
       when the function throws. If we do it as part of
       setOpt(key,val), it does not.
    */
    print(banner.start);
    print( arguments.callee.name+"()" );
    var c = new Curl();
    var o = {
        port:80,
        httpHeader:["Host: s11n","Accept-Encoding: gzip","Accept: text/html"],
        //httpHeader:3, // v8 crash or "secondary" throw, depending on the workarounds in the native code
        url:'http://localhost'
    };
    rc = c.addOpt(o);
    print( "setOpt RC =",rc);
    rc = c.setOpt( Curl.OPT_HTTPHEADER,
                   "" // causes error code or throws
                   //["Host: wh","Accept-Encoding: gzip","Accept: text/html"]
                   );
    print( "setOpt RC =",rc);
    print( "c =",JSON.stringify(c,undefined,2));
    c.destroy();
    print(banner.end);
}

function testCurlInfo()
{
    print(banner.start);
    print( arguments.callee.name+"()" );
//     for( var i = Curl.INFO_NONE; i < Curl.INFO_LASTONE; ++i )
//     {
//         print( i );
//     }
    var c = new Curl();
    var irc = c.getInfo( Curl.INFO_OS_ERRNO );
    print( 'irc =',irc);
    irc = c.getInfo( Curl.INFO_PRIMARY_IP );
    print( 'irc =',typeof irc,irc);
    c.destroy();
    print(banner.end);
}

//testOne();
//testTwo();
// testThree();
 testCurlInfo();
// testCrash();
