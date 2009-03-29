load_plugin('v8-juice-ncurses-oo');
var nc = ncurses;
//nc.ripoffline(true);nc.ripoffline(true);nc.ripoffline(true);
nc.curs_set(0);
var root = new nc.NCPanel();
var name = root.name();
root.attrset( nc.color_pair('white','red') );
root.addstr("I am the root panel");
root.childs = [];
root.captureCout();
print("std::cout is redirected here.");
print("NCPad.PadReqPageDown =",ncurses.NCPad.PadReqPageDown);
function tryPanel()
{
    try
    {
        var ch = new nc.NCPanel(10,30,3,3);
        //new nc.NCPanel();
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
        ch.getch();

        var ch2 = new nc.NCPanel(10,35,5,10);
        print('new child =',ch2.name());
        ch2.bkgd( nc.intVal(' ') | nc.color_pair('white','yellow') | ncurses.A_BOLD );
        ch2.scrollok(true);
        for( var i = 0; i < 50; ++i )
        {
            ch2.addstr("\n#"+i+": This is not stdscr, either.");
        }
        ch.top();
        ch2.refresh();
        ch.addstr("\nTap a key.");
        ch.getch();
        ch.addstr("\nGreat job!");
        ch2.top();
        ch2.refresh();
        ch2.addstr("\nTap a key.");
        ch.getch();
        ch2.addstr("\nGreat job! Again!");
        ch2.mvwin( 0, 5 );
        ch2.refresh();

        var p3 = new nc.NCPanel( 15, 30, 2, 15 );
        p3.top();
        p3.scrollok(true);
        p3.bkgd( nc.intVal(' ') | nc.color_pair('yellow','cyan') | ncurses.A_BOLD );

        p3.refresh();
        print("About to add a window around the second panel. Tap a key...");
        ch2.getch();
        var ch3 = //new nc.NCWindow( p3 );
            new nc.NCPad(p3,100,100,3,3);
        print("Wrapper added.");
        ch3.bkgd( nc.intVal(' ') | nc.color_pair('blue','white') | ncurses.A_BIM );
        ch3.scrollok(true);
        //ch3.captureCout();
        cn = ch3.name();
        for( var i = 0; i < 5; ++i )
        {
            //print("This is window",cn);
            ch3.addstr("#"+i+": This is window "+cn+"\n");
        }
        ch3.addstr("Going into input loop.\nTry scrolling.\nTap Ctrl-W or F10 to stop.\n");
        ch3.refresh();
        ch3.inputLoop();
        ch3.addstr("Input loop finished.");
        ch3.refresh();
        //p3.refresh();
        print("Now tap a key.");
        ncurses.beep();
        p3.getch();
        //ch3.captureReset();
        ch.captureReset();
        ch.close();
        ch2.close();
        ch3.close(); // ?
        p3.clear();
        p3.mvaddstr(0,0,"Tap it...");
        p3.refresh();
        p3.getch();
        p3.close();
    }
    catch(e)
    {
        for( var i = 0; i < 5; ++i ) ncurses.beep();
        print("exception:",e);
    }
}

//root.addstr("Hi, world! Tap a key to continue.");
//root.refresh();
tryPanel();
root.refresh();
print("All done. Tap a key to quit.");
ncurses.beep();
//ncurses.flash();
var rc = root.getch();
root.close();
nc.endwin(); // aarrgghh!
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
