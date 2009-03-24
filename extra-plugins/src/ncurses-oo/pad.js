load_plugin('v8-juice-ncurses-oo');
var nc = ncurses;
nc.curs_set(0);
var root = new nc.NCPanel();
var name = root.name();
root.bkgd( nc.color_pair('yellow','cyan') );
root.mvaddstr(4,0,"I am the root panel");
root.captureCout( ncurses.color_pair('yellow','cyan') | nc.A_BOLD);
root.captureCerr();
print("std::cout is redirected here.");
function tryPad()
{
    try
    {
        var pnl = new nc.NCPanel(10,30,3,3);
        //new nc.NCPanel();
        print('new child =',pnl.name());
        pnl.captureCerr( nc.color_pair('white','red') | nc.A_BLINK | nc.A_UNDERLINE );
        pnl.scrollok(true);
        pnl.bkgd( nc.intVal(' ') | nc.color_pair('white','blue') );

        var pad = new nc.NCPad(pnl,100,100);
        print("Wrapper pad added.");
        pad.bkgd( nc.intVal(' ') | nc.color_pair('blue','white') | ncurses.A_BIM );
        pad.mapKeyToReq( ncurses.KEY_CTRL('G'), ncurses.NCPad.PadReqExit );
        pad.scrollok(true);
        //pad.captureCout();
        cn = pad.name();
        for( var i = 0; i < 5; ++i )
        {
            //print("This is window",cn);
            pad.addstr("#"+i+": This is window "+cn+"\n");
        }
        pad.addstr("Going into input loop.\nTry scrolling.\nTap Ctrl-G to stop.\n");
        pnl.refresh();
        pad.inputLoop();
        pad.addstr("Input loop finished.");
        pad.refresh();
        print("Now tap a key to destroy these objects.");
        ncurses.beep();
        pnl.getch();
        pad.close();
        pnl.erase();
        pnl.addstr("Pad destroyed. Now type in something and tap ENTER: ");
        pnl.refresh();
        nc.curs_set(1);
        nc.echo(true);
        var str = pnl.getstr(10);
        nc.echo(false);
        nc.curs_set(0);
        print("Entered text: ["+str+']');
        pnl.close();
    }
    catch(e)
    {
        for( var i = 0; i < 5; ++i ) ncurses.beep();
        print("exception:",e);
    }
}

//root.addstr("Hi, world! Tap a key to continue.");
//root.refresh();
tryPad();
root.refresh();
print("All done. Tap a key to quit.");
ncurses.beep();
var rc = root.getch();
root.close();
print("window name =",name,", getch =",rc);
print("Done! :-D");
