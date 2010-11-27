function Actor()
{
    
};

Actor.prototype = {

};

function testDF()
{
    var fudge = include('FudgeSystem.inc');
    var ar = [];
    var stats = [];
    var n = 10000;
    var dN = 4;
    for( var i = -dN; i<=dN; ++i ) stats[i] = 0;
    var total = 0;
    print('Rolling',n,'sets of',dN+'dF...');
    for( var i = 0; i < n; ++i )
    {
        var d = fudge.rollDF(dN);
        ar.push( d );
        stats[d] = 1 + (+stats[d]);
        total += d;
    }
    if( n < 101 ) print('Results:',ar.join(', '));
    print('Total for',n,'rolls of',dN+'dF =',total,'Average =',(total / n));
    for( var k in stats )
    {
        print(fudge.getAdjective(k),'('+k+')\tx',stats[k],
              "("+(stats[k]/n*100)+"%)" );
    }

    var max = 5;
    var min = -max;
    print("Mass:\nScale\tKilograms");
    for( var i = min; i <= max; ++i )
    {
        var x = fudge.scales.mass.medianValue( i );
        print( sprintf("%d\t%0.2f",i,x) );
    }

    print("Speed:\nScale\tMeters/Second");
    for( var i = min; i <= max; ++i )
    {
        var x = fudge.scales.speed.medianValue( i );
        print( sprintf("%d\t%0.2f",i,x) );
    }

    print("Strength:\nScale\tRelative Strength");
    for( var i = min; i <= max; ++i )
    {
        var x = fudge.scales.strength.medianValue( i );
        print( sprintf("%d\t%0.2f",i,x) );
    }

}
testDF();


