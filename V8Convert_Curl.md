**See also:** [V8Convert](V8Convert.md)



# cvv8 Curl #

This code provides a JS class called `Curl` which allows the client to
perform certain operations on URLs using the amazing
[libcurl API](http://curl.haxx.se/libcurl/).

Only the features of the so-called "easy" API (`curl_easy_xxx()`) are
available. The main limitation of this is that the plugin works only
synchronously.

## Source Code ##

http://code.google.com/p/v8-juice/source/browse/#svn%2Fconvert%2Faddons%2Fcurl


## Loading from C++ ##

The class can be added to a JS engine by linking in
[the relevant code](http://code.google.com/p/v8-juice/source/browse/convert/addons/curl)
into your app and calling `v8::cvv8::CurlJS::SetupBindings()`. This API also requires the [V8Convert\_ByteArray](V8Convert_ByteArray.md) code (only 2 files).

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
| `function headerFunction(byteArray,userData)` | `CURLOPT_HEADERFUNCTION` | See below. |
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
| `function writeFunction(byteArray,userData)` | `CURLOPT_WRITEFUNCTION` | See below. |

All of the supported `CURLOPT_xxx` constants are available in JS space via `Curl.OPT_xxx`.

## ByteArray Class ##

The [V8Convert\_ByteArray](V8Convert_ByteArray.md) class is bound as `Curl.ByteArray` in this API.

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

### Reading Data from Curl ###

The `headerFunction()` and `writeFunction()` properties are functions
which are called when a response header or body chunk have been
read. They take two arguments:

  * The data read in, as a `Curl.ByteArray` object (see [V8Convert\_ByteArray](V8Convert_ByteArray.md)).
  * The `Curl` object's `opt.writeData` value (because this is how the native API does it).

They must return the number of bytes they consume, meaning they must
return the length of the first parameter on success. Any other value
will cause the process (initialized by `easyPerform()`) to stop and
will cause `easyPerform()` to return an error code.

When the `headerFunction()` is called, it is passed a single header line (a complete header entry), but that line has a trailing `\r\n` on it, which should be stripped by the caller. It is not stripped by the wrapper because doing so would cause confusion with the `length` parameter (and we do not adjust to account for the two extra stripped characters that way because it would falsify any byte count calculations done by the client).

Note that for the `writeFunction()` it is important to remember that the data can come in chunked on arbitrary boundaries. That means that we cannot assume that any given chunk is legal UTF, e.g. the chunk might end in the middle of a UTF8 character. Thus it is not safe to use JS strings to buffer the data. Use a `ByteArray` instead.

### Writing `POST` Data via Curl ###

When curl is told to send data to a server, e.g. by using `setOpt(Curl.OPT_POST,true)`, curl uses a callback
function to fetch the data from the user. The JS callback has, due to differences in C++ and JS calling conventions,
a different signature than the native callback function:

```
function readerCallback( int bytes, any userData );
```

The first argument is the number of bytes which curl would like to have. The second is the value of the `Curl.OPT_READDATA` option (e.g. set via `setOpt(Curl.OPT_READDATA,...)`). The function must return one of:

  * Throw an exception to abort the sending process prematurely.
  * On success, return a `ByteArray` no longer than the first argument (counted in _bytes_, not _characters_).
  * A non-`ByteArray` value (e.g. `null`) to signal the end of the input.

It is important to stop producing data once curl has requested
`Content-Length` bytes (e.g., set via `setOpt(Curl.OPT_HTTPHEADER,...)`). This is because curl will read until the callback either
throws or returns a value smaller than the first parameter passed to
the callback.

_Encoding of the data is left to the user._

Here's a trivial, largely nonsensical example:

```
var c = new Curl();
var content = "";
for( var i = 0; i < 20000; ++i )
{
    content += '*';
}
var baBody = new Curl.ByteArray(content);
var contentLength = baBody.length;
c.setOpt({
        url:'http://localhost',
        verbose:true,
        post:true,
        httpHeader:['Content-Length: '+contentLength],
        readFunction:function(len,ud)
        {
            if( ud.pos >= contentLength ) return null;
            var clen = contentLength - ud.pos;
            var xlen = (len > clen) ? clen : len;
            var s = baBody.slice( ud.pos, xlen );
            ud.pos += xlen;
            return s;
        },
        readData:{
            pos:0
        }
    });
var rc = c.easyPerform();
c.destroy();
```

# Examples #

See the script files in [this code's repository](http://code.google.com/p/v8-juice/source/browse/#svn%2Fconvert%2Faddons%2Fcurl) for examples.

Here's a brief example which reads data from a Fossil source repository using the Fossil JSON API:

```
Curl.fossilJson = function(opt){
    var c = new Curl();
    var content;
    var inBody;
    try {
        var contentLength;
        if(opt.envelope){
            if('object' === typeof opt.envelope) {
                content = JSON.stringify(opt.envelope);
            }
            else if('string' === typeof opt.envelope){
                content = opt.envelope;
            }
        }
        if(content) {
            content = new Curl.ByteArray(content);
            contentLength = content.length;
        }

        var copt = {
            url: opt.url,
            //verbose: true,
            httpHeader:['Content-Type: application/json'],
            writeFunction: function(data, ud){
                if(!inBody) inBody = new Curl.ByteArray();
                inBody.append(data);
                var len = data.length;
                data.destroy();
                return len;
            }
        };
        if( content ){
            copt.post = true;
            copt.httpHeader.push('Content-Length: '+contentLength);
            copt.readFunction = function(len,ud)
            {
                //throw new Error("Testing error handling.");
                if( ud.pos >= contentLength ) return null;
                var clen = contentLength - ud.pos;
                var xlen = (len > clen) ? clen : len;
                var s = content.slice( ud.pos, xlen );
                //print("readFunction(",len,") sending",s.length,'bytes');
                ud.pos += xlen;
                return s;
            };
            copt.readData = {
                pos:0
            };
        }
        c.setOpt(copt);
        var rc = c.easyPerform();
        if(rc){
            throw new Error("Curl returned error code #"+rc+".");
        }
        if('function' === typeof opt.onResponse){
            var body = inBody ? inBody.stringValue() : "";
            //print("CONTENT: "+body);
            opt.onResponse(JSON.parse(body));
        }
    }
    finally{
        if(inBody) inBody.destroy();
        if(content) content.destroy();
        c.destroy();
    }
};

function test1(){
    var opt = {
        onResponse:function(obj){
            print("onResponse: "+JSON.stringify(obj));
        },
        url: 'http://fossil.wanderinghorse.net/repos/whio/index.cgi/json',
        envelope: {
            "command": "wiki/list",
            "requestId": "yoohooÄöüß"
        }
    };
    Curl.fossilJson(opt);

}

test1();
```