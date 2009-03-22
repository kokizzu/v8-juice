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
var root = new nc.NCPanel();
var name = root.name();
root.attrset( nc.color_pair('white','red') );
root.addstr("I am the root panel");
root.childs = [];
root.captureCout();
print("std::cout is redirected here.");

function tryPanel()
{
    try
    {
        var ch = new nc.NCPanel(10,30,3,3);
        //new nc.NCPanel();
        root.childs.push(ch);
        print('new child =',ch.name());
        ch.captureCerr( nc.color_pair('white','red') | nc.A_BLINK | nc.A_UNDERLINE );
        ch.scrollok(true);
        ch.bkgd( nc.intVal(' ') | nc.color_pair('white','blue') );
        var cn = ch.name();
        for( var i = 0; i < 50; ++i )
        {
            ch.addstr("#"+i+": This is panel "+cn+"\n");
        }
        ch.refresh();

        var ch2 = new nc.NCPanel(10,35,5,10);
        print('new child =',ch2.name());
        ch2.bkgd( nc.intVal(' ') | nc.color_pair('white','yellow') | ncurses.A_BOLD );
        ch2.scrollok(true);
        for( var i = 0; i < 50; ++i )
        {
            ch2.addstr("\n#"+i+": This is not stdscr, either.");
        }
        ch2.refresh();
        ch2.getch();
        ch.top();
        ch.addstr("\nTap a key.");
        ch.getch();
        ch.addstr("\nGreat job!");
        ch2.top();
        ch2.refresh();
        ch2.addstr("\nTap a key.");
        ch2.getch();
        ch2.addstr("\nGreat job! Again!");
        ch2.mvwin( 0, 5 );
        ch2.refresh();
        ch.captureReset();
    }
    catch(e)
    {
        print("exception:",e);
    }
}

//root.addstr("Hi, world! Tap a key to continue.");
//root.refresh();
tryPanel();
var rc = root.getch();
root.close();
//ncurses.endwin(); // kludge until i get window destruction working properly
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
