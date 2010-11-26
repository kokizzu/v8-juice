include('echo-config.inc.js');

function echoClient()
{
    var k,v;
    print("echo.Socket members:");
    for( k in echo.Socket )
    {
        print('\t'+k,'=',echo.Socket[k]);
    }
    var s, c, n;
    try
    {
        var rc;
        s  = new echo.Socket( echo.socketFamily, echo.socketType );
        print('s =',s);
        print('s.hostname =',s.hostname);
        rc = s.setTimeout( 5 );
        print("s.setTimeout() rc =",rc);

        var msg = ["GET / HTTP/1.1",
                   "Host: "+echo.host
                   ].join(echo.crnl)
            +echo.crnl
            +echo.crnl;
        if( echo.Socket.SOCK_STREAM == s.type )
        {
            print("Running in stream mode...");
            rc = s.connect( echo.host, echo.port );
            print('s.connect() rc =',rc);
            rc = s.write( msg );
            print( "s.write() rc =",rc);
            var buf = "";
            while( undefined != (rc = s.read(20) ) ) {
                buf += rc;
            }
            print("Read in "+buf.length+" bytes:\n"+buf);
        }
        else if( echo.Socket.SOCK_DGRAM == s.type )
        {
            rc = s.sendTo( echo.host, echo.port, msg );
            print('s.sendTo() rc =',rc);
        }
        else
        {
            throw new Error("Unknown socket type!");
        }
    }
    finally
    {
        print("Closing client socket...");
        if( s ) s.close();
        print("Closed client socket.");
    }
        
}

try
{
    echoClient();
    print("Done!");
}
catch(e)
{
    print("EXCEPTION:",e);
    throw e;
}
