
try
{
    undefined === FudgeSystem;
}
catch(e){
FudgeSystem = (function() {
    var fudge = {};
    fudge.rollDF = function (n) {
        if( 0 == arguments.length ) n = 4;
        var rc = 0;
        var df = function() { return Math.floor( (Math.random() * 1000 % (3))) - 1; };
        for( var i = 0; i < n; ++i )
        {
            rc += df();
        }
        return rc;
    };
    return fudge;
})();
}

function rollDF(n) {
    if( 0 == arguments.length ) n = 4;
    var rc = 0;
    var df = function() { return Math.floor( (Math.random() * 1000 % (3))) - 1; };
    for( var i = 0; i < n; ++i )
    {
            rc += df();
    }
    return rc;
}

function testDF()
{
    var n = 10000;
    var dN = 4;
    var func =
        //FudgeSystem.rollDF; // HOLY COW! This is about 3.5 slower!
    rollDF;
    print( new Date() );
    print('Rolling',n,'sets of',dN+'dF...');
    for( var i = 0; i < n; ++i )
    {
        func(dN);
    }
    print( new Date() );
}
testDF();
