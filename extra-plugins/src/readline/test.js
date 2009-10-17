PathFinder.shared.plugins.pathString = '.';
var fn = "test.history";
var my = {};
loadPlugin('v8-juice-readline',my);
if( my.readline.loadHistory(fn) ) {
    print("Loaded history file",fn);
} else {
    print("Couldn't load history file",fn);
}
var s = 1;
while( undefined != (s = my.readline.read("Enter something (Ctrl-D quits): ")) )
{
    print("Result:",s);
}
print(""); // ensure a newline
print("Done!");
