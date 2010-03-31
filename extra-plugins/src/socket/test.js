var ns = {};
loadPlugin("v8-juice-Socket",ns);

function test1()
{
    var k,v;
    print("ns.Socket members:");
    for( k in ns.Socket )
    {
        print('\t'+k,'=',ns.Socket[k]);
    }
    var s, c, n;
    try
    {
        s  = new ns.Socket( ns.Socket.AF_INET, ns.Socket.SOCK_STREAM );
        c = new ns.Socket( ns.Socket.AF_INET,
                           ns.Socket.SOCK_STREAM
                           //ns.Socket.SOCK_DGRAM
                           );
        print('s =',s,'c =',c);
        print('s.hostname =',s.hostname);
        var host =
            //'localhost'
            'wh'
            //'127.0.0.1'
            //'www.google.de'
            ;
        var port = 80;
        var rc;
        if(false && (port > 1024))
        {
            rc = s.bind( host, port );
            print( "s.bind() rc =",rc);
        }
        
        var plist = ['ntp', 'tcp','udp'];
        for( k in plist )
        {
            v = plist[k];
            print('getProtoByName('+v+') =',
                  ns.Socket.getProtoByName(v) );
        }
        var hlist = ['localhost'];//,'www.google.com'];
        rc = s.setTimeout( 0, 1000 );
        print("s.setTimeout() rc =",rc);
        if(1)
        {
            for( k in hlist )
            {
                var ao = ns.Socket.nameToAddress( hlist[k] );
                print('nameToAddress(',hlist[k],') == ',ao);
                print('addressToName(',ao,') == ',ns.Socket.addressToName(ao));
            }
        }

        //host = 'nosuchfuckinghost'
        var chost = ns.Socket.nameToAddress( host );
        print("Connecting to",chost+":"+port,'...');
        rc = c.connect( chost, port );
        print("c.peerInfo =",c.peerInfo);
        rc = c.setTimeout( 3, 0 );
        print("c.setTimeout() rc =",rc);
        var crnl = '\r\n';
        var header = ["GET / HTTP/1.1",
                      "Host: "+host
                      ].join(crnl)+crnl+crnl;
        var ba = new ns.ByteArray( header );
        rc = c.write( ba );
        print('header =['+header+']');
        print("write() rc =",rc);
        function nukeRC(BA)
        {
            if( BA instanceof ns.ByteArray ) BA.destroy();
            else BA.length = 0;
        }
        function rcVal(BA)
        {
            return ( BA instanceof ns.ByteArray ) ? BA.stringValue : BA.toString();
        }
        if( c.type === ns.Socket.SOCK_STREAM )
        {
            n = 64;
            print("Reading...");
            while( undefined !== (rc = c.read(n,false)) )
            {
                if( null === rc )
                {
                    print("Apparently interrupted by timeout before data arrived.");
                    nukeRC(rc);
                    continue;
                }
                print("read("+n+") ==",(rc.length),"["+rcVal(rc)+"]");
                if( rc.length < n )
                {
                    /* corner case: we might have been interrupted by
                       a timeout here and got a short read. We need to
                       add a flag value to Socket to be able to
                       distinguish that here.

                       ...

                       it turns out we cannot recognize it in the native
                       code, either!                      
                    */
                    nukeRC(rc);
                    break;
                }
                nukeRC(rc);
            }
        }
        print("c.peerInfo =",c.peerInfo);
    }
    finally
    {
        print("Closing sockets...");
        if( c ) c.close();
        if( s ) s.close();
        print("Closed sockets.");
    }
        
}

function testBA()
{
    var n = 10;
    var ba = new ns.ByteArray(n);
    var i;
    print(ba,'length =',ba.length);
    ba.length = ba.length/2;
    print(ba,'length =',ba.length);
    for( i = 0; i < ba.length; ++i )
    {
        ba[i] = i * 17;
        print( 'ba[',i,'] =',ba[i] );
    }
    print('length =',ba.length);
    for( var k in ba )
    {
        print( '\t',k,'=',ba[k] );
    }
}

try
{
    test1();
    testBA();
    print("Done!");
}
catch(e)
{
    print("EXCEPTION:",e);
    throw e;
}
