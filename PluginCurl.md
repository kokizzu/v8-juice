**See also:** [Plugins](Plugins.md)



# Curl Plugin #

**Achtung: this is new code (added 20091118) and the interface may still change significantly.**

This plugin provides a JS class called `Curl` which allows the client to
perform certain operations on URLs using the amazing
[libcurl API](http://curl.haxx.se/libcurl/).

Only the features of the so-called "easy" API (`curl_easy_xxx()`) are
available. The main limitation of this is that the plugin works only
synchronously.

## WARNINGS ##

**Do not try to use this API to handle binary data.** It is not possible
to pass binary data around in v8 without the overhead of transporting the
data from C++ to JS as an Array of integers, which would have a _huge_
memory cost (somewhere around 50 times the size of the input data, i.e.
10kb of input would require roughly 500kb of memory). Another alternative would be to add a `ByteArray` class, which stores the bytes efficiently and allows the client to access them individually.

When a v8 string is created and it contains any "high bits" (anything
non-ASCII), v8 assumes UTF8 format and will try to convert it
accordingly, and this will fail miserably if the data is not exactly
the assumed format. This is unfortunately a limitation of v8. Thus this
plugin cannot be used to fetch images, zip files, PDFs, and the like. Be aware that when reading UTF8 data, the _length in bytes_ of strings may differ from the _length in characters_, and the JS-side `String.length` property returns the number of _characters_.

## Loading from JS ##

Install the plugin by following the instructions on the [Plugins](Plugins.md) page. It requires
that `libcurl` be found in your linker path and that `<curl/curl.h>` is in your default
includes path (or edit the makefile to suit).

Then simply do:

```
loadPlugin('v8-juice-libcurl');
```

## Loading from C++ ##

The class can be added to a JS engine by linking in
[the relevant code](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/curl)
into your app and calling `v8::juice::curl::CurlJS::SetupBindings()`.

Alternately, you can use `v8::juice::plugin::LoadPlugin()` to dynamically load the plugin from native code.

# JS API #

| **Return type** | **Name/Signature** | **Notes** |
|:----------------|:-------------------|:----------|
| `Curl` | `new Curl()` | Creates a new parser object. |
| `int` | `addOpt(Object options)` | Like `setOpt(object)`, but does not clear the options list. |
| `void` | `destroy()` | Destroys the object, freeing all of its resources. Calling this is optional, but has the benefit of freeing up resources immediately, rather than waiting on the garbage collector. |
| `mixed` | `getInfo(int)` | Fetches various information about a request or response. See below. |
| `int` | `easyPerform()` | Processes the current request parameters. Analog to `curl_easy_perform()`. |
| `int` | `pause([int = Curl.PAUSE_ALL])` | Pauses a connection, optionally pausing either reading or writing. |
| `int` | `resume()` | Resumes a paused connection. Equivalent to `pause(Curl.PAUSE_CONT)`. |
| `int` | `setOpt(int CurlOptionID,mixed value)` | See below. Throws on error. |
| `int` | `setOpt(Object options)` | See below. Returns number of properties set or throws on error. |

Each `Curl` object has a number of options which control how libcurl
handles data. These options are set using `setOpt()`. The following
table list the options (in alphabetical order) and their libcurl
counterparts (which are documented
[at the libcurl site](http://curl.haxx.se/libcurl/c/curl_easy_setopt.html)).

| **Property** | **Curl Counterpart** | **Notes** |
|:-------------|:---------------------|:----------|
| `bool crlf` | `CURLOPT_CRLF` |  |
| `int bufferSize` | `CURLOPT_BUFFERSIZE` |  |
| `int connectionTimeout` | `CURLOPT_CONNECTIONTIMEOUT` |  |
| `int dnsCacheTimeout` | `CURLOPT_DNS_CACHE_TIMEOUT` |  |
| `bool failOnError` | `CURLOPT_FAILONERROR` |  |
| `bool followLocation` | `CURLOPT_FOLLOWLOCATION` |  |
| `bool header` | `CURLOPT_HEADER` |  |
| `any headerData` | `CURLOPT_HEADERDATA` | Passed as last argument to `headerFunction()` |
| `function headerFunction(data,readLength,userData)` | `CURLOPT_HEADERFUNCTION` | See below. |
| `StringArray httpHeader` | `CURLOPT_HTTPHEADER` |  |
| `StringArray http200Aliases` | `CURLOPT_HTTP200ALIASES` |  |
| `string interface` | `CURLOPT_INTERFACE` |  |
| `int lowSpeedLimit` | `CURLOPT_LOW_SPEED_LIMIT` |  |
| `int lowSpeedTime` | `CURLOPT_LOW_SPEED_TIME` |  |
| `bool noBody` | `CURLOPT_NOBODY` |  |
| `bool noSignal` | `CURLOPT_NOSIGNAL` |  |
| `string noProxy` | `CURLOPT_NOPROXY` |  |
| `int port` | `CURLOPT_PORT` | default port number to use. |
| `StringArray postQuote` | `CURLOPT_POSTQUOTE` |  |
| `StringArray preQuote` | `CURLOPT_PREQUOTE` |  |
| `string proxy` | `CURLOPT_PROXY` |  |
| `int proxyPort` | `CURLOPT_PROXYPORT` |  |
| `string range` | `CURLOPT_RANGE` |  |
| `any readData` | `CURLOPT_READDATA` | Passed as last argument to `readFunction()` |
| `function readFunction(readLength,userData)` | `CURLOPT_READFUNCTION` | See below. |
| `int resumeFrom` | `CURLOPT_RESUME_FROM` |  |
| `int maxRedirs` | `CURLOPT_MAXREDIRS` |  |
| `bool post` | `CURLOPT_POST` |  |
| `StringArray quote` | `CURLOPT_QUOTE` |  |
| `bool tcpNoDelay` | `CURLOPT_TCP_NODELAY` |  |
| `int timeout` | `CURLOPT_TIMEOUT` |  |
| `int timeoutMS` | `CURLOPT_TIMEOUT_MS` |  |
| `string url` | `CURLOPT_URL` | The URL to read from/write to. |
| `string userAgent` | `CURLOPT_USERAGENT` |  |
| `string userName` | `CURLOPT_USERNAME` |  |
| `string userPwd` | `CURLOPT_USERPWD` |  |
| `bool verbose` | `CURLOPT_VERBOSE` | If true, libcurl sends debugging info to stderr.  |
| `any writeData` | `CURLOPT_WRITEDATA` | Passed as last argument to `writeFunction()` |
| `function writeFunction(data,readLength,userData)` | `CURLOPT_WRITEFUNCTION` | See below. |

All of the supported `CURLOPT_xxx` constants are available in JS space via `Curl.OPT_xxx`.

## libcurl constants ##

The curl API contains many constant values which used as:

  * Error codes (named `CURLE_xxx`)
  * Option codes (`CURLOPT_xxx`)
  * Connection information codes (`CURLINFO_xxx`)
  * A few other miscellaneous things (e.g. the special-purpose return value `CURL_READFUNC_PAUSE`).

All such constants which are supported by the JS API are available by replacing the `CURL` prefix with `Curl.`. e.g. `CURLE_OK` becomes `Curl.E_OK` and `CURLOPT_VERBOSE` becomes `Curl.OPT_VERBOSE`. The majority (about 200!) of the `CURLOPT_xxx`, `CURLINFO_xxx` and `CURLE_xxx` constants are available in JS.

## Setting Curl Options ##

The various option properties can be set like:

```
// Using libcurl notation:
mycurl.setOpt( Curl.OPT_VERBOSE, true );

// Using JS-style option names:
mycurl.setOpt( 'userName', 'stephan' );

// As an object containing key/value pairs:
mycurl.setOpt( { verbose:true, url:'http://code.google.com' } );
```

In the first form, the first argument must be `Curl.OPT_XXX`,
which corresponds to one of the `CURLOPT_XXX` constants listed above.
In the second form, it uses the "JS-like" propert name as the first
argument. All such properties use the "camelCase" spelling of the
equivalent `CURLOPT_xxx` property. e.g. the "friendly" form of
`CURLOPT_TIMEOUT_MS` is `timeoutMs`.

Changes made with `setOpt()` and friends are applied immediately, but
not all have any effect if changed after a connection is established
(by calling `easyPerform()`).

To fetch the options values use `myCurlObj.opt.optionName`. **However**,
if you _assign_ to that variable directly then the settings will not
be applied to the underlying curl routines. _Never_ re-assign the
`opt` property itself - that will cause the internals to use a
different options object! (Those last two behaviours are bugs, but
v8's get/set interceptor interface won't let me do this they way i
would like to.)

Also beware that passing an object to `setOpt()` will replace the
internal list of options but will not "undo" any options previously
set on the object. e.g. if you call `myCurl.setOpt(Curl.OPT_URL,...)`
and then call `myCurl.setOpt({})`, the url property will be erased
from the `myCurl` object (because it is not in the new options) but
the underlying libcurl connection will still use it until another
value is set.

## `getInfo()` and `CURLINFO_xxx` mappings. ##

`getInfo()` is equivalent to [curl\_easy\_getinfo()](http://curl.haxx.se/libcurl/c/curl_easy_getinfo.html),
and allows one to fetch various connection-related information.

It accepts a single integer parameter corresponding to one of the
`CURLINFO_xxx` arguments listed below (but named `Curl.INFO_xxx` in
this API). If `getInfo()` fails (passed an unknown value, or because
the underlying call to `curl_easy_getinfo()` fails) then it throws an
exception, otherwise it returns an different type depending on the
`CURLINFO` argument (string, int, double, or array-of-strings, as
documented in the libcurl API).  Unlike `setOpt()` and friends, the
first argument must be an integer - there are no "JS-friendly" names
for these properties.

The following list shows which `CURLINFO_xxx` constants are available
via `Curl.INFO_xxx`. Most, but not all, of these can be fetched using
`getInfo()`. Those which cannot be fetched via `getInfo()` are marked
with an asterisk.

  * `CURLINFO_APPCONNECT_TIME`
  * `CURLINFO_CERTINFO`
  * `CURLINFO_CONDITION_UNMET`
  * `CURLINFO_CONNECT_TIME`
  * `CURLINFO_CONTENT_LENGTH_DOWNLOAD`
  * `CURLINFO_CONTENT_LENGTH_UPLOAD`
  * `CURLINFO_CONTENT_TYPE`
  * `CURLINFO_COOKIELIST`
  * `CURLINFO_DATA_IN` `*`
  * `CURLINFO_DATA_OUT` `*`
  * `CURLINFO_DOUBLE`
  * `CURLINFO_EFFECTIVE_URL`
  * `CURLINFO_END` `*`
  * `CURLINFO_FILETIME`
  * `CURLINFO_FTP_ENTRY_PATH`
  * `CURLINFO_HEADER_IN` `*`
  * `CURLINFO_HEADER_OUT` `*`
  * `CURLINFO_HEADER_SIZE`
  * `CURLINFO_HTTPAUTH_AVAIL`
  * `CURLINFO_HTTP_CONNECTCODE`
  * `CURLINFO_LASTONE` `*`
  * `CURLINFO_LASTSOCKET`
  * `CURLINFO_LONG`
  * `CURLINFO_MASK` `*`
  * `CURLINFO_NAMELOOKUP_TIME`
  * `CURLINFO_NONE`
  * `CURLINFO_NUM_CONNECTS`
  * `CURLINFO_OS_ERRNO`
  * `CURLINFO_PRETRANSFER_TIME`
  * `CURLINFO_PRIMARY_IP`
  * `CURLINFO_PRIVATE`
  * `CURLINFO_PROXYAUTH_AVAIL`
  * `CURLINFO_REDIRECT_COUNT`
  * `CURLINFO_REDIRECT_TIME`
  * `CURLINFO_REDIRECT_URL`
  * `CURLINFO_REQUEST_SIZE`
  * `CURLINFO_RESPONSE_CODE`
  * `CURLINFO_SIZE_DOWNLOAD`
  * `CURLINFO_SLIST`
  * `CURLINFO_SPEED_DOWNLOAD`
  * `CURLINFO_SPEED_UPLOAD`
  * `CURLINFO_SSL_DATA_IN` `*`
  * `CURLINFO_SSL_DATA_OUT` `*`
  * `CURLINFO_SSL_ENGINES`
  * `CURLINFO_SSL_VERIFYRESULT`
  * `CURLINFO_STARTTRANSFER_TIME`
  * `CURLINFO_STRING`
  * `CURLINFO_TEXT` `*`
  * `CURLINFO_TOTAL_TIME`
  * `CURLINFO_TYPEMASK` `*`

Some of these cannot be fetched because they are markers
(e.g. `CURLINFO_LASTONE`), masks/flags (e.g. `CURLINFO_TYPEMASK` and
`CURLINFO_LONG`), or are otherwise not documented in the
`curl_easy_getinfo()` documentation (e.g. `CURLINFO_HEADER_IN`).

## `pause()` and `resume()` ##

The `pause()` function works like
[curl\_easy\_pause()](http://curl.haxx.se/libcurl/c/curl_easy_pause.html).
Its only argument is an integer, which corresponds to a bitmask of
the `CURLPAUSE_XXX` constants. The following values are supported,
but are called `Curl.PAUSE_XXX` in this API:

  * `CURLPAUSE_RECV`
  * `CURLPAUSE_SEND`
  * `CURLPAUSE_ALL`
  * `CURLPAUSE_CONT`

If called without arguments, `pause()` behaves as if `Curl.PAUSE_ALL`
had been passed in.

To resume a paused connection call `pause(Curl.PAUSE_CONT)` or
`resume()` (which is equivalent).

These functions return an integer value, with 0 being success. They
do not throw JS exceptions like some of the other functions.


## Callback routines ##

### Reading `GET` Data from Curl ###

The `headerFunction()` and `writeFunction()` properties are functions
which are called when a response header or body chunk have been
read. They take three arguments:

  * The data read in, as a string object.
  * The real length of the read-in data.
  * The `Curl` object's `opt.writeData` value (because this is how the native API does it).

They must return the number of bytes they consume, meaning they must
return the value of the second parameter on success. Any other value
will cause the process (initialized by `easyPerform()`) to stop and
will cause `easyPerform()` to return an error code.

**Note that it is not safe to pass binary data into v8**. If the
function is passed binary data then the `.length` property of the
read-in data (the first argument to the callback) will _probably_
differ from the second paremeter's value. In such a case, there are no
guarantees that the data is valid - it might have been corrupted via
v8's string conversion routines (or it _might_ actually be a legal v8
string).

When the `headerFunction()` is called, it is passed a single header line (a complete header entry), but that line has a trailing `\r\n` on it, which should be stripped by the caller. It is not stripped by the wrapper because doing so would cause confusion with the `length` parameter (and we do not adjust to account for the two extra stripped characters that way because it would falsify any byte count calculations done by the client).

### Writing `POST` Data via Curl ###

When curl is told to send data to a server, e.g. by using `setOpt(Curl.OPT_POST,true)`, curl uses a callback
function to fetch the data from the user. The JS callback has, due to differences in C++ and JS calling conventions,
a different signature than the native callback function:

```
function readerCallback( int bytes, any userData );
```

The first argument is the number of bytes which curl would like to have. The second is the value of
the `Curl.OPT_READDATA` option (e.g. set via `setOpt(Curl.OPT_READDATA,...)`). The function must
return one of:

  * An empty string to signal the end of the input.
  * Throw an exception to abort the sending process prematurely.
  * On success, a string value with the same length as the first argument passed to the callback (count in _bytes_, not _characters_).

It is important to stop producing data once curl has requested
`Content-Length` (e.g., set via `setOpt(Curl.OPT_HTTPHEADER,...)`)
bytes.  This is because curl will read until the callback either
throws or returns a value smaller than the first parameter passed to
the callback.

_Encoding of the data is left to the user._ i have absolutely no experience with posting data this way, and
can say nothing more on the topic!

Here's a trivial, largely nonsensical example:

```
var c = new Curl();
var content = "";
for( var i = 0; i < 20000; ++i )
{
    content += '*';
}
c.setOpt({
        url:'http://localhost',
        verbose:true,
        post:true,
        httpHeader:['Content-Length: '+content.length],
        readFunction:function(len,ud)
        {
            if( ud.pos >= ud.max ) return "";
            var clen = ud.length - ud.pos;
            var xlen = (len > clen) ? clen : len;
            var s = ud.content.substr( ud.pos, xlen );
            print("readFunction(",len,") sending",s.length,'bytes.');
            ud.pos += xlen;
            return s;
        },
        readData:{
            pos:0,
            content:content
        }
    });
var rc = c.easyPerform();
c.destroy();
```

That produces output similar to:

```
* About to connect() to localhost port 80 (#0)
*   Trying 127.0.0.1... * connected
* Connected to localhost (127.0.0.1) port 80 (#0)
> POST / HTTP/1.1
Host: localhost
Accept: */*
Accept-Encoding: deflate, gzip
Content-Length: 20000
Content-Type: application/x-www-form-urlencoded

readFunction( 16384 ) sending 16384 bytes.
readFunction( 16384 ) sending 3616 bytes.
readFunction( 16384 ) sending 0 bytes.
< HTTP/1.1 200 OK
< Date: Fri, 20 Nov 2009 20:15:07 GMT
< Server: Apache/2.2.12 (Ubuntu)
< X-Powered-By: PHP/5.2.10-2ubuntu6.1
< Set-Cookie: PHPSESSID=1ce8cb3dacb9d6619991832e9f3c2179; path=/
< Expires: Thu, 19 Nov 1981 08:52:00 GMT
< Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0
< Pragma: no-cache
< Vary: Accept-Encoding
< Content-Encoding: gzip
< Content-Length: 1912
< Content-Type: text/html
< 
* Connection #0 to host localhost left intact
* Closing connection #0
```

(The lines starting with `*` or `<` appear because the `verbose` option is set to true.)

# Example #

```
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
```

That produces output similar to:

```
headerFunction() 17 of 17 bytes: [HTTP/1.1 200 OK]
headerFunction() 37 of 37 bytes: [Date: Wed, 18 Nov 2009 01:11:46 GMT]
headerFunction() 18 of 18 bytes: [Pragma: no-cache]
headerFunction() 40 of 40 bytes: [Expires: Fri, 01 Jan 1990 00:00:00 GMT]
headerFunction() 42 of 42 bytes: [Cache-Control: no-cache, must-revalidate]
headerFunction() 40 of 40 bytes: [Content-Type: text/html; charset=UTF-8]
headerFunction() 33 of 33 bytes: [X-Content-Type-Options: nosniff]
headerFunction() 152 of 152 bytes: [Set-Cookie: PREF=ID=4f2a9007345<...snip...>;]
headerFunction() 18 of 18 bytes: [Server: codesite]
headerFunction() 21 of 21 bytes: [X-XSS-Protection: 0]
headerFunction() 28 of 28 bytes: [Transfer-Encoding: chunked]
headerFunction() 2 of 2 bytes: []
writeFunction() 965 of 965 bytes
writeFunction() 2836 of 2836 bytes
writeFunction() 278 of 278 bytes
writeFunction() 1133 of 1133 bytes
writeFunction() 1418 of 1418 bytes
writeFunction() 1418 of 1418 bytes
writeFunction() 110 of 110 bytes
writeFunction() 1413 of 1413 bytes
writeFunction() 1418 of 1418 bytes
writeFunction() 1248 of 1248 bytes
writeFunction() 1413 of 1413 bytes
writeFunction() 1418 of 1418 bytes
writeFunction() 1063 of 1063 bytes
c.easyPerform() rc = 0
[object Curl@0x8b763f0] = {
  "opt": {
    "url": "http://code.google.com/p/v8-juice/wiki/PluginCurl",
    "userAgent": "Google Chrome, kind of.",
    "writeData": {
      "count": 13
    },
    "headerData": {
      "count": 12
    }
  }
}
```

# Example #2: Fetch Wiki Pages from any Google Code Project #

The following script pulls a Wiki page from an arbitrary Google Code project. It is intended to use with the v8-juice-shell and the [curl plugin](PluginCurl.md), to be called like:

```
  ~> v8-juice-shell this_script.js -- GoogleCodeProjectName WikiPageName
```

The script:

```
/**
    Test/demo app for GoogleCodeWikiParser. Requires
    the v8-juice (http://code.google.com/p/v8-juice) JS shell
    and the libcurl plugin for that shell.

    Usage:

    v8-juice-shell thisScript.js -- GoogleCodeProjectName WikiPageName

    Output:

    The wiki page is pulled from the GoogleCode SVN repo and dumped
    to stdout using print().
*/
loadPlugin('v8-juice-libcurl');

var proj = arguments[0];
var page = arguments[1];

if( ! proj || !page )
{
    throw new Error("Requires two arguments: Google Code Project and Wiki Page name.");
}

var url = 'http://'+proj+'.googlecode.com/svn/wiki/'+page+'.wiki';

var curl = new Curl();
var wikiOutput = [];
curl.setOpt({
    url:url,
    userAgent:"Google Chrome, kind of.",
    writeFunction:function writeFunction(data,len,ud)
    {
        wikiOutput.push(data);
        return data.length;
    },
    writeData:null
});
var rc = curl.easyPerform();
curl.destroy();
if( rc )
{
    throw new Error("curl.easyPerform() returned error code "+rc+"!");
}
print( wikiOutput.join('') );
```

# TODOs #

  * Give some thought to how we might bind the curl "multi" interface, which apparently allows asynchronous communication.