load_plugin('v8-juice-ncurses-oo');
load_plugin('v8-juice-whio');
var nc = ncurses;
var logger = new whio.OutStream('/dev/stderr',false);
function log()
{
    for( var i = 0; i < arguments.length; ++i )
    {
        logger.write(arguments[i]+' ');
    }
    logger.write('\n',1);
}
var root = new nc.NCWindow();
var name = root.name();
root.attrset( nc.color_pair('white','red') );
root.childs = [];

function tryPanel()
{
    try
    {
        var ch = new nc.NCPanel(10,30,3,3);
        //new nc.NCPanel();
        root.childs.push(ch);
        log('new child =',ch.name());
        ch.scrollok(true);
        ch.bkgd( nc.intVal(' ') | nc.color_pair('white','blue') );
        for( var i = 0; i < 50; ++i )
        {
            ch.addstr("\n#"+i+": This is not stdscr.");
        }
        ch.refresh();
    }
    catch(e)
    {
        log("exception:",e);
    }
}

//root.addstr("Hi, world! Tap a key to continue.");
//root.refresh();
tryPanel();
var rc = root.getch();
root.close();
ncurses.endwin(); // kludge until i get window destruction working properly
logger.close();
print("window name =",name,", getch =",rc);
if(0) {
    var check = ['*','!','a'];
    for( var k in check )
    {
        var iv = nc.intVal(check[k]);
        var cv = nc.charVal(iv);
        print('intVal('+check[k]+') =',iv);
        print('charVal('+iv+') =',cv);
    }
}
print("Done! :-D");
