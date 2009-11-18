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
    var c = new Curl();
    print( "Curl obj =",JSON.stringify( c, undefined, 2 ) );
    print('opt =',c.opt);
    c.port = 80;
    c.url = "http://wh";
    c.timeout = 17;
    c.headerFunction = function headerFunction(data,len,ud)
    {
        print(arguments.callee.name+"()",data.length,"of",len,"bytes:","["+data.substring(0,data.length-2)+"]");
        return data.length;
    };
    c.setOpt({
        verbose:true,
        port:80,
        url:"http://s11n",
        writeFunction:function writeFunction(data,len,ud)
        {
            print(arguments.callee.name+"()",data.length,"of",len,"bytes");
            ++ud.count;
            return data.length;
        },
        writeData:{count:0},
        userAgent:"v8-juice-curl",
        placeholder:null
    });
    
    print( "port =",c.port);
    print( "timeout =",c.timeout);
    var rc = c.easyPerform();
    print("perform rc =",rc);
    print("Curl obj =",JSON.stringify(c,undefined,2));
    //print("Curl.opt obj =",JSON.stringify(c.opt,undefined,2));
    c.destroy();
}
function testThree()
{
    var c = new Curl();
    var o = {
        port:80,
        timeout:11,
        connectionTimeout:17,
        headerFunction:function headerFunction(data,len,ud)
        {
            print(arguments.callee.name+"()",data.length,"of",len,"bytes:","["+data.substring(0,data.length-2)+"]");
            return data.length;
        },
        headerData:{count:0}
    };
    var rc = c.setOpt( Curl.OPT_CRLF, 0 )
    print( "setOpt RC =",rc);
    c.url = 'http://wh';
    rc = c.setOpt(o);
    rc = c.setOpt( Curl.OPT_USERAGENT, "v8-juice" );
    print( "setOpt RC =",rc);
    c.url = 'http://s11n';
    print('c.url ==',c.url);

    c.writeFunction =function writeFunction(data,len,ud)
    {
        print(arguments.callee.name+"()",data.length,"of",len,"bytes");
        ++ud.count;
        return data.length;
    };
    c.writeData = {count:0};

    print( "c =",JSON.stringify(c,undefined,2));
    c.easyPerform();
    c.destroy();
}
//testOne();
//testTwo();
testThree();


