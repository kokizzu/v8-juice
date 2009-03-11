
function dumpPF(p,list)
{
    if( !(p instanceof PathFinder) ) throw new Error("p is-not-a PathFinder! Type="+(typeof p));
    var li = (list instanceof Array) ? list : ['fossil','who','ls','nope'];
    print("PathFinder:",p.pathString);
    for( k in li ) {
	print('\tfind("'+li[k]+'") =',p.find(li[k]));
    }
}
function tryOne()
{
    var p = new PathFinder("/home/stephan/bin:/usr/bin:/bin");
    dumpPF(p);
}


tryOne();
print('PathFinder.shared =',PathFinder.shared);
dumpPF(PathFinder.shared.plugins);
dumpPF(PathFinder.shared.include);
//dumpPF(new Number(3)); // should throw
