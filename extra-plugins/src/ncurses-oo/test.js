load_plugin('v8-juice-ncurses-oo');
var nc = ncurses;
nc.riplines = 5;
if(1) for( var i = 0; i < nc.riplines; ++i )
{
    nc.ripoffline( (i<3) );
}
else
{
    nc.ripoffline(true);
    nc.ripoffline(false);
}

var w = new nc.NCPanel();
var name = w.name();
w.attrset( nc.color_pair('white','red') );
w.childs = [];

function tryRip()
{
    var cl =
        ['white','red','white','black','cyan'];
        //nc.colorNames();
    for( var i = 0; (i < 5); ++i )
    {
        var rw = nc.getRippedLine(i);
        if( ! rw ) break;
        //rw.bkgd( nc.color_pair( 'white', 'red' ) );
        rw.bkgd( nc.color_pair( cl[i], cl[i+1] ) );
        rw.addstr("ripped line #"+i+" ="+rw);
        rw.refresh();
    }
}

function tryChild()
{
    try
    {
        var ch = //new nc.NCWindow(10,30,3,3);
            new nc.NCWindow(w);
        ch.scrollok(true);
        ch.bkgd( nc.intVal(' ') | nc.color_pair('white','blue') );
        for( var i = 0; i < 50; ++i )
        {
            ch.addstr("\n#"+i+": This is not stdscr.");
        }
        ch.refresh();
        w.childs.push(ch);

        var sub = new nc.NCWindow(ch,5,20,3,3);
        w.childs.push(sub);
        sub.scrollok(true);
        sub.bkgd( nc.intVal(' ') | nc.color_pair('black','white'));
        sub.addstr("A sub-window of "+ch.name());
        sub.refresh();
    }
    catch(e)
    {
        print("exception:",e);
    }
}

//w.addstr("Hi, world! Tap a key to continue.");
//w.refresh();
tryRip();
tryChild();
var rc = w.getch();
w.clear();
w.refresh();
//tryRip();

rc = w.getch();
w.close();
//ncurses.endwin(); // kludge until i get window destruction working properly
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
