include('echo-config.inc.js');

function echoServer()
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
        s  = new echo.Socket( echo.socketFamily, echo.socketType );
        print('s =',s);
        print('s.hostname =',s.hostname);
        var rc;
        rc = s.bind( echo.host, echo.port );
        print( "s.bind() rc =",rc);
        rc = s.setTimeout( 5 );
        print("s.setTimeout() rc =",rc);
        //var c = s.accept();
        //print("s.accept() =",c);
        if( echo.Socket.SOCK_STREAM == s.type )
        {
            print("Running in stream mode...");
            rc = s.listen();
            print("s.listen() rc =",rc);
            for( ; ; )
            {
                var c = s.accept();
                print('accept =',c);
                if( undefined === c )
                {
                    print("Presuming a timeout and continuing...");
                    continue;
                }
                break;
            }
        }
        else if( echo.Socket.SOCK_DGRAM == s.type )
        {
            var len = 1;
            for( ;; )
            {
                print("Waiting on read...");
                var rc = s.read(len);
                print("read rc =",rc);
            }
        }
        else
        {
            throw new Error("Unknown socket type!");
        }
    }
    finally
    {
        print("Closing server socket...");
        if( s ) s.close();
        print("Closed server socket.");
    }
        
}

try
{
    echoServer();
    print("Done!");
}
catch(e)
{
    print("EXCEPTION:",e);
    throw e;
}
