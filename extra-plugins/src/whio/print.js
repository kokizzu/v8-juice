loadPlugin("v8-juice-whio");

function print()
{
   print.stream.write( Array.prototype.slice.apply( arguments, [0] ).join(arguments.callee.separator) + "\n" );
   arguments.callee.stream.flush();
}
print.stream = new whio.OutStream('/dev/stdout'); // or whatever
print.separator = " ";
print("Hi,","world!");
