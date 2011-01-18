loadPlugin("v8-juice-PosixPoll");

var App = {
tickNum:0
};

function test1()
{
    print("Tick #"+(++App.tickNum));
}
print("Running Test 1...");
posix.poll(500,test1,function(){return App.tickNum<5;});

function test2()
{
    var count = 0;
    ++App.tickNum;
    posix.poll(100,
               function(){print("test2() tick #"+(++count));},
               function(){return count<3;}
               );
}
App.tickNum = 0;
print("Running Test 2...");
posix.poll(500,test2,function(){return App.tickNum<3;});

try
{
    function test3()
    {
        var count = 0;
        if( ++App.tickNum > 3 ) throw new Error("Testing poll() abort.");
        print("test3() tick #"+(App.tickNum));
    }
    App.tickNum = 0;
    print("Running Test 3...");
    posix.poll(300,test3,function(){return true;});
}
catch(e)
{
    print("Got expected exception: "+e);
}


try
{
    App.tickNum = 0;
    print("Running Test 4...");
    posix.poll(300,test3,function(){throw new Error("until() is throwing");});
}
catch(e)
{
    print("Got expected exception: "+e);
}


print("Done!");
