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
    print( "c.opt ==",c.opt);
    //print( 'opt type =',typeof c.opt);
//     c.opt = {
//     //url:'http://code.google.com/p/v8-juice/wiki/HomePage',
//         url:'http://wh',
//         userAgent:"Google Chrome, kind of.",
//         //verbose:true,
//         //noBody:true,
//         writeFunction:function writeFunction(data,len,ud)
//         {
//             print(arguments.callee.name+"()",data.length,"of",len,"bytes");
//             ++ud.count;
//             return data.length;
//         },
//         writeData:{count:0},
//         //header:true,
//         headerFunction:function headerFunction(data,len,ud)
//         {
//             print(arguments.callee.name+"()",data.length,"of",len,"bytes:","["+data.substring(0,data.length-2)+"]");
//             ++ud.count;
//             return data.length;
//         },
//         headerData:{count:0},
//         placeholder:undefined
//     };
    c.url = "http://wh";
    var rc = c.easyPerform();
    print( "c.easyPerform() rc =",rc);
    print( "count =",c.opt.writeData.count);
    print("Curl object properties:");
    for( var k in c )
    {
        print('\t',typeof c[k], k );
    }

    print(c,'=',JSON.stringify(c,undefined,2));
    var c2 =
        new Curl();
        // v8 is crashing if instantiate our inner holder object in the ctor:
        //new Curl({ url:"http://yahoo.com" })
        //new Curl("http://yahoo.com")
        ;
    print( c2,'=', JSON.stringify(c2) );
    c2.destroy();
    c.destroy();
    print(banner.end);
};
function testTwo()
{
    var c = new Curl();
    print( "Curl obj =",JSON.stringify( c, undefined, 2 ) );
    print('opt =',c.opt);
    c.opt.port = 80;
    c.opt.url = "http://wh";
    c.opt.timeout = 17;
    c.opt.headerFunction = function headerFunction(data,len,ud)
    {
        print(arguments.callee.name+"()",data.length,"of",len,"bytes:","["+data.substring(0,data.length-2)+"]");
        return data.length;
    };
    c.opt = {
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
    };
    
    print( "port =",c.opt.port);
    print( "timeout =",c.opt.timeout);
    var rc = c.easyPerform();
    print("perform rc =",rc);
    print("Curl obj =",JSON.stringify(c,undefined,2));
    print("Curl.opt obj =",JSON.stringify(c.opt,undefined,2));
    c.destroy();
}
function testThree()
{
    var c = new Curl();
    print( "c =",JSON.stringify(c,undefined,2));
    var rc = c.setOpt( Curl.OPT_CRLF, 0 );
    print( "setOpt RC =",rc);
    c.destroy();
}
//testTwo();
//testOne();
testThree();


