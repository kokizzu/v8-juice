**See also:** [Plugins](Plugins.md)



**cvv8 users:** As of 20120219 [V8Convert](V8Convert.md) also has a port of this plugin, available in [the source repo](http://code.google.com/p/v8-juice/source/browse/#svn%2Fconvert%2Faddons%2Fexpat).

# XML Parser Plugin (a.k.a. ExpatParser) #

Added 20091114.

This plugin provides a JS class called ExpatParser which allows the client to
parse XML using [libexpat](http://libexpat.sf.net) as the back-end. It is fed
its input by the client, and thus it is not tied to reading one specific type
of input stream, nor does it require buffering the entire input before parsing.

This XML parser is event-driven. It reads XML input incrementally and calls
client-specified callbacks when it encounters various parts of the XML tree.
This approach makes it relatively simple to create language-specific object trees
from XML input. However, compared to "pull parsers", it is (IMO) less convenient when it comes to fishing out specific data from XML input. Pull-parsers require several higher-level abstractions which the event-drive approach does not require (e.g. an input source interface and a DOM API).

## Loading from JS ##

Install the plugin by following the instructions on the [Plugins](Plugins.md) page.

Then simply do:

```
loadPlugin('v8-juice-ExpatParser');
```

## Loading from C++ ##

The class can be added to a JS engine by linking in
[the relevant code](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/expat)
into your app and calling `v8::juice::expat::ExpatJS::SetupBindings()`.


# JS API #

| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| `ExpatParser` | `new ExpatParser()` | Creates a new parser object. |
| `bool` | `parse(string input,bool finished)` | See below for details. |
| `void` | `reset()` | Re-initializes the XML parser state. Not normally necessary, as `parse()` does this. |
| `void` | `destroy()` | Destroys the parser object, freeing all of its resources. Calling this is optional, but has the benefit of freeing up resources immediately, rather than waiting on the garbage collector. |
| **Type** | **Properties** (read-write) | **Notes** |
| `Function` | `startNode(userData,string name,Object attr)` | Called when an XML node is opened. |
| `Function` | `endNode(userData,string name)` | Called when an XML node is closed. |
| `Function` | `charHandler(userData,string data,int dataLength)` | Called when CDATA is read. |
| `any` | `userData` | This is passed as the first argument to all of the event callbacks. |

The various `Function` properties should be set by the client (see
below for more details).

## `parse()` ##

The `parse()` function can be used to parse an XML string all at once or incrementally.
If the XML passed to it is complete, the second argument must be true. If the XML
is being read incrementally, the second argument should be false for all iterations
_except the last one_. The last call to `parse()` should pass true as the final
parameter so that expat knows that it can then try to make sense of the accumulated
input.

This function throws an exception if parsing fails, as this is the only way it can report
the location (in the XML) of the error.

On a successful `parse()`, or if `parse()` throws an exception, the parser state is automatically re-set (using `reset()`) so that it can start working on new input.

Note that because `parse()` throws an exception on any parsing error, its return value is largely irrelevant.

## `reset()` ##

`reset()` re-initializes the internal parser state. The event callbacks and whatnot all
remain the same, but any accumulated parsing is discared. The `parse()` function resets
the parser on an error or when it finishes a parse. It is only necessary to use this function if, while incrementally reading/parsing input, you decide to abort the parsing process and discard all input.

## Event Callbacks ##

The parser passes information to the client via callback routines. It does so for the following cases:

  * Opening an XML node. Callback name = `startNode`.
  * Closing an XML node. Callback name = `endNode`.
  * Reading CDATA from within an XML node. Callback name = `charHandler`.

At those points in the parsing process, libexpat will trigger the client-supplied callback
to handle the data however the client wishes.

libexpat itself can do much more than these few things, and any missing
features may be added to this API as the need arises.

If any of these callbacks are not set, or are set to non-Function
values, then they will not be called (that is, those events will be
ignored).

The callbacks are described below.

### `startNode()` ###

`startNode()` is called when an XML node is opened. It is passed:

  * `userData` = the parser's `userData` property.
  * `string name` = the name of the XML node.
  * `Object attr` = key/value pairs of the XML node's attributes.

### `endNode()` ###

`endNode()` is called when an XML node is closed. It is passed:

  * `userData` = the parser's `userData` property.
  * `string name` = the name of the XML node.

### `charHandler()` ###

`charHandler()` is called when XML CDATA is parsed. It is passed:

  * `userData` = the parser's `userData` property.
  * `string text` = the character data.
  * `int length` = the length of the character data, in bytes.

Be aware that this might be called multiple times for one XML node, so
if you are accumulating it somewhere be sure to _append_ the new data,
rather than assign over it. It will be called multiple times for one node
when:

  * Parsing incrementally, and the data spans more than one chunk.
  * The CDATA is interspersed between child XML nodes.

# Examples #

## Everything You Need to Know ##
Here's all there is to know:

```
loadPlugin('v8-juice-ExpatParser');

var ex = new ExpatParser();
ex.userData = "hi, world";
ex.startNode = function startNode(ud,name,attr)
{
    print("startNode:",name);
    for( var k in attr )
    {
        print("attr ["+k+"] =",attr[k]);
    }
};

ex.endNode = function endNode(ud,name)
{
    print("endNode:",name,'userData =',ud);
};

ex.charHandler = function charHandler(ud,text,len)
{
    print("charHandler: ["+text+"]");
};


// Some arbitrary XML input. Here we're simulating
// line-by-line reading...
var xmlar = [
             "<root a='b' c='d'><sub ",
             "name='child'>subnode ",
             "text</sub></root>"
             ];
if(0) // parse all in one go
{
    ex.parse( xmlar.join(""), true );
}
else // parse incrementally
{
    for( var i = 0; i < xmlar.length; ++i )
    {
        rc = ex.parse( xmlar[i], i == (xmlar.length-1));
    }
}
ex.destroy();
```


That produces output similar to:

```
startNode: root
attr [a] = b
attr [c] = d
startNode: sub
attr [name] = child
charHandler: [subnode ]
charHandler: [text]
endNode: sub userData = hi, world
endNode: root userData = hi, world
```

Notice that `charHandler` is called twice for the one CDATA block. This is because the CDATA spans input lines and are are parsing incrementally. In such a case it is important to _append_ the CDATA if accumulating it internally in the callback, rather than over-writing data from previous calls on the same XML node.

## Converting XML to Object Trees ##

Here's an example of how to convert XML input to an object tree:

```
/**
   Converts the given XML to an object tree with this structure:

   {
     tag:'NodeName',
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
        root:null, // root node
        stack:[], // node stack
        cur:null // current node
    };
    p.userData = state;
    p.startNode = function(ud,name,attr)
    {
        var o = {tag:name};
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
    try
    {
        p.parse( xml, true );
    }
    finally
    {
        p.destroy();
    }
    return state.root;
}
```