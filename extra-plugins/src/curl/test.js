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
    //print( 'opt type =',typeof c.opt);
    c.opt = {
        url:'http://code.google.com/p/v8-juice/wiki/HomePage',
        //url:'http://wh',
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
            print(arguments.callee.name+"()",data.length,"of",len,"bytes:","["+data.substring(0,data.length-2)+"]");
            ++ud.count;
            return data.length;
        },
        headerData:{count:0},
        placeholder:undefined
    };
    //c.url = "http://wh";
    print(c,'=',JSON.stringify(c,undefined,2));
    var rc = c.easyPerform();
    print( "c.easyPerform() rc =",rc);
    print( "count =",c.opt.writeData.count);
    print("Curl object properties:");
    for( var k in c )
    {
        print('\t',typeof c[k], k );
    }

    print( c,'=', JSON.stringify(c) );
    var c2 =
        //new Curl();
        //new Curl({ url:"http://yahoo.com" })
        new Curl("http://yahoo.com")
        ;
    print( c2,'=', JSON.stringify(c2) );
    c2.destroy();
    c.destroy();
    print(banner.end);
};


testOne();
