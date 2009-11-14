loadPlugin('v8-juice-ExpatParser');

var banner = {
    start:'>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>',
        end:'<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<'
        };
var xmlar = [
             "<root a='b' c='d'><sub ",
             "name='child'>subnode text",
             "</sub></root>"
             ];
var xml = "<root a='b' c='d'><sub name='child'>subnode text</sub></root>";

function startNode(ud,name,attr)
{
    print("startNode:",name);
    for( var k in attr )
    {
        print("attr ["+k+"] =",attr[k]);
    }
}
function endNode(ud,name)
{
    print("endNode:",name,'userData =',ud);
}
function charHandler(ud,text,len)
{
    print("charHandler: ["+text+"]");
}

function testOne()
{
    print(banner.start);
    print( arguments.callee.name+"()" );
    var ex = new ExpatParser();
    ex.startNode = startNode;
    ex.endNode = endNode;
    ex.charHandler = charHandler;
    ex.userData = "hi, world";
    print('ex =',ex);
    var rc;
    if(1)
    {
        rc = ex.parse( xml, true );
    }
    else
    {
        for( var i = 0; i < xmlar.length; ++i )
        {
            ex.parse( xmlar[i], i == (xmlar.length-1));
        }
    }
    print( "parse rc =",rc);
    ex.destroy();
    print(banner.end);
};

testOne();


function xmlToObject(xml)
{
    var p = new ExpatParser();
    var state = {
    root:null,
    stack:[],
    cur:null
    };
    p.userData = state;
    p.startNode = function(ud,name,attr)
    {
        var o = {name:name};
        if( ! ud.root ) ud.root = o;
        if( ud.cur )
        {
            if( ! ud.cur.children ) ud.cur.children = [];
            ud.cur.children.push( o );
        }
        for( var k in attr )
        {
            if( ! o.attr ) o.attr = {};
            o.attr[k] = attr[k];
        }
        ud.stack.push(o);
        ud.cur = o;
    };
    p.endNode = function(ud,name)
    {
        ud.stack.pop();
        ud.cur = ud.stack.length ? ud.stack[ud.stack.length-1] : null;
    };
    p.charHandler = function(ud,text,len)
    {
        ud.cur.cdata = text;
    };
    var rc = false;
    try
    {
        rc = p.parse( xml, true );
    }
    finally
    {
        p.destroy();
    }
    return rc ? state.root : undefined;
}

function testTwo()
{
    var x = // some ancient libs11n test code...
        "<!DOCTYPE s11n::io::expat_serializer>"
        +"<somenode class='NoClass'>"
 	+"<a>()\\b</a>"
 	+"<foo>bar</foo>"
 	+"<empty />"
 	+"<long>this is a long property</long>"
 	+"<dino class='DinoClass' />"
 	+"<fred class='FredClass'>"
        +"<key>value</key>"
 	+"</fred>"
 	+"<wilma class='WilmaClass'>"
        +"<the>lovely wife</the>"
 	+"</wilma>"
 	+"<betty class='BettyClass'>"
        +"<value>the apple of Barney&apos;s eye</value>"
 	+"</betty>"
 	+"<deep class='Foo'>"
        +"<deeper class='Foo'>"
        +"<how_deep>really deep!</how_deep>"
        +"<and_deeper class='Ouch'>"
        +"</and_deeper>"
        +"</deeper>"
 	+"</deep>"
        +"</somenode>"
        ;
    print(banner.start);
    print( arguments.callee.name+"()" );
    var xo = xmlToObject(x);
    print('xmlToObject ==',JSON.stringify(xo,undefined,4));
    print(banner.end);
}
testTwo()
