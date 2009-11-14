loadPlugin('v8-juice-ExpatParser');

var banner = {
    start:'>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>',
        end:'<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<'
        };
var xmlar = [
             "<root a='b' c='d'><sub ",
             "name='child'>subnode ",
             "text</sub></root>"
             ];
//var xml = xmlar.join("");

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
    if(0)
    { // parse all in one go
        rc = ex.parse( xmlar.join(""), true );
    }
    else
    { // parse incrementally
        for( var i = 0; i < xmlar.length; ++i )
        {
            rc = ex.parse( xmlar[i], i == (xmlar.length-1));
        }
    }
    print( "parse rc =",rc);
    ex.destroy();
    print(banner.end);
};

testOne();

/**
   Converts the given XML to an object tree with this structure:

   {
     name:'NodeName',
     attr:{key/val pairs of XML element attributes},
     children:[ ... nested objects ... ],
     cdata:string_of_char_data_from_xml_node
   }

   The cdata, attr, and children fields will be missing if the node has
   no cdata, attributes, or children.

   On error it throws. On success the root object in the tree
   is returned.
*/
function XMLToObject(xml)
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
        if( ! ud.cur.cdata ) ud.cur.cdata = "";
        ud.cur.cdata += text;
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
    var xo = XMLToObject(x);
    print('XMLToObject ==',JSON.stringify(xo,undefined,4));
    print(banner.end);
}
testTwo()
