/************************************************************************
Reads a simple grammar from stdin and creates v8::juice::ClassBinder
code for it (sending it to stdout).

Requires the v8-juice-whio plugin for i/o.

File format:

One record per line. Record types/syntax

class WrappedClassName
func returnType funcName( argType1, ... argTypeN )
prop key aJSValue
# comment line - ignored

A 'func' record's funtion name field may be in the form "nativeName=scriptName",
in which case the name after the '=' will be used for the JS function.

Empty lines and extraneous spaces are ignored.

For example:

###################################
class MyType
func std::string voodoo(int)
func int calc_something=calculateSomething(int,int,double)
prop howdy CastToJS("Hi, there! How're ya?")
prop yo Integer::New(42)
###################################


Notable limitations:

- Currently only handles one class per input stream. If it finds
multiples it will mix the entries into one class with the name of the
last 'class' record.

- Arg parameters may not be named.

- If the return type is pointer-qualified there must be no space
between the '*' and the type name.

- Template parameters with commas in the names will break it.

- Only types supported by v8::juice::convert will work.

************************************************************************/
load_plugin('v8-juice-whio');

var App = {
In:new whio.InStream('/dev/stdin'),
Buf:new whio.IODevice(":memory:"),
lineno:0,
Lines:[],
pos:0,
Tok:[],
Wr:{className:'Unnamed',funcs:[],props:[]}
};

function readAll() {
    var rdSize = 1024 * 2;
    while( rdSize == (App.Buf.write(App.In.read(rdSize))) )
    {
    }
    var len = App.Buf.size();
    //print("Buffered "+len+" bytes.");
    App.Buf.rewind();
    App.Lines = App.Buf.read(len).split('\n');
    App.Buf.truncate(0);
}


function doLine() {
    if( App.pos >= App.Lines.length ) return false;
    ++App.lineno;
    var line = App.Lines[App.pos++];
    if( undefined === line ) return true; // blank line?
    if( ! line.length || line.match(/^\s+$/) ) return true;
    if( line.match(/^\s*#/) )
    {
	return true;
    }
    var wr = App.Wr;
    //print('line =',line);
    var check = line.split(/\s+/);
    var checkPat = function(p) {
        var rx = p.exec(line);
        if( null == rx ) throw new Error("Malformed "+check[0]+" line #"+App.lineno+":\n"+line);
	return rx;
    };
    switch( check[0] ) {
      case 'class':wr.className = check[1];
	  //print("CLASS:",line);
	  return true;
      case 'prop': {
	  var pat = /^\s*prop\s+(\S+)\s+(.+)$/;
	  var rx = checkPat(pat);
	  wr.props[rx[1]] = rx[2];
	  if( null == rx ) throw new Error("Malformed func line #"+App.lineno+":",line);
	  //print("PROP:",line);
	  return true;
      }
      case 'func':
	  //print("FUNC:",line);
	  var pat = /^\s*func\s+(\S+)\s+([^(]+)\s*\((.+)?\)\s*$/;
	  var rx = checkPat(pat);
	  var rest = rx[3] ? rx[3] : "";
	  var fn = rx[2];
	  var name = {cpp:fn,js:fn};
	  var fn2 = null;
	  if( (fn2 = /(\w+)=(\w+)/.exec(fn)) )
	  {
	      name.cpp = fn2[1];
	      name.js = fn2[2];
	  }
	  wr.funcs[name.cpp] = {
	      name:name,
	      returnType:rx[1],
	      argString:rest,
	      args:(rest.length ? rest.split(/\s*,\s*/) : [])
	  };
	  //print("FUNC:",rx);
	  return true;
      default:
	  throw new Error("Unknown line type at line #"+App.lineno+": "+line);
    };
    print('Tokens = ['+App.Tok.join(']\t[')+']');
    return true;
}

function printWrapperInfo() {
    var wr = App.Wr;
    print("Wrapped class:",wr.className);
    for( var k in wr.props )
    {
	print("PROPERTY["+k+"] =",wr.props[k]);
    }

    for( var k in wr.funcs )
    {
	var f = wr.funcs[k];
	print("FUNCITON["+k+"] =",f.returnType,f.name.cpp,'(',f.args.join(','),')');
    }
}

function dumpBinder() {
    var wr = App.Wr;

    if( ! wr.className ) {
	throw new Error("No 'class' record was found! Class name unknown!");
    }
    print("// Implementation code:");
    print("typedef ClassBinder<",wr.className,"> WR;");
    print("WR wr;");
    for( var k in wr.props )
    {
	//print("PROPERTY["+k+"] =",wr.props[k]);
	print('wr.Set("'+k+'",',wr.props[k],"):");
    }

    var out = App.Buf;
    for( var k in wr.funcs )
    {
	out.truncate(0);
	out.rewind();
	var f = wr.funcs[k];
	//print("FUNCITON["+k+"] =",f.returnType,f.name,'(',f.args.join(', '),')');
	out.write("wr.BindMemFunc< ");
	out.write(f.returnType);
	if( f.args.length )
	{
	    out.write(", "+f.args.join(", "));
	}
	out.write(", &"+wr.className+"::"+f.name.cpp+" >");
	out.write('("'+f.name.js+'");');
	out.rewind();
	var line = out.read(out.size());
	print(line);
    }

    print("wr.Seal();");
}


function main() {
    var ex = null;
    try
    {
	readAll();
	while( doLine() ) {}
	printWrapperInfo();
	dumpBinder();
    }
    catch(e) {
	ex = e;
    }
    finally
    {
	for( k in App ) {
	    if( ! (App[k] instanceof whio.IOBase) ) continue;
	    //print("Closing ["+App[k].fileName+"]");
	    App[k].close();
	}
    }
    if(ex) throw ex;
    return;
}

main();
