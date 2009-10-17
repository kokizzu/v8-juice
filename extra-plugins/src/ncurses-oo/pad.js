loadPlugin('v8-juice-ncurses-oo');
var nc = ncurses;
//nc.ripoffline(true);nc.ripoffline(true);nc.ripoffline(false);nc.ripoffline(false);
var root = new nc.NCPanel();
nc.curs_set(0);
var name = root.name();
root.bkgd( nc.color_pair('yellow','cyan') );
root.mvaddstr(14,0,"I am the root panel\n");
//root.mvaddch(15,3,ncurses.intVal('*'));
root.captureCout( ncurses.color_pair('yellow','cyan') | nc.A_BOLD);
//root.captureCerr();
print("std::cout is redirected here.");

if(0)
{
    var rc = nc.popupDialog("Dialog! Achtung!","You are about to see... a PAD!");
    print("Dialog keypress was:",rc,nc.charVal(rc));
}
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

        var pad = true
            ? new nc.NCFramedPad(pnl,100,100)
            : new nc.NCPad(pnl,100,100);
        print("Wrapper pad added.");
        pad.bkgd( nc.intVal(' ') | nc.color_pair('blue','white') | ncurses.A_DIM );
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

function colorPad()
{
    var culrs = ncurses.colorNames();
    var attrlist = [
                    ncurses.A_NORMAL,
                    ncurses.A_BOLD,
                    //ncurses.A_REVERSE,
                    ncurses.A_UNDERLINE
                    ];
    var ydim = culrs.length;
    var xdim = culrs.length * attrlist.length;
    var pnl = 
        //new ncurses.NCPanel( 10,8,0,5 );
        new ncurses.NCPanel( [10,8,0,5] );
    print("Created wrapper panel.");
    var pad = new ncurses.NCFramedPad(pnl,ydim,xdim,1,1);
        //pad = new ncurses.NCWindow(pnl);
        //pnl;
    pad.scrollok(true);
    pad.mapKeyToReq( ncurses.KEY_CTRL('G'), ncurses.NCPad.PadReqExit );
    pad.bkgd( nc.intVal(' ') | nc.color_pair('blue','white') | ncurses.A_DIM );
    pnl.mvwin( 5,5);
    var theChar = ncurses.intVal('*');
    for( var b = 0; b < culrs.length; ++b )
    {
        var bg = culrs[b];
        var colm = 0;
        for(var f = 0; f < culrs.length; ++f, ++colm )
        {
            var fg = culrs[f];
            for(var attr = 0; attr < attrlist.length; ++attr,++colm)
            {
                var chattr = ncurses.color_pair(fg,bg) | attrlist[attr] ;
                var rc = pad.mvaddch(b,colm,chattr | ncurses.intVal('*'));
            }
            --colm;
        }
    }
    pnl.refresh();
    print("Entering scroll area loop. Tap Ctrl-G to end.");
    pad.inputLoop();
    print("Done. Now use the cursor keys to move the window. Tap a non-movement key to quit.");
    print("Panel pos:",pnl.begy(),pnl.begx());
    pnl.interactivelyMove();
    print("Panel pos:",pnl.begy(),pnl.begx());

    print("Done. Tap a key to destroy the window.");
    pnl.getch();
    pad.close();
    pnl.close();
}

try
{
    colorPad();
    //tryPad();
}
catch(e)
{
    var attr = ncurses.color_pair('white','red') | ncurses.A_BOLD;
    root.attron(attr);
    print("EXCEPTION:",e);
    root.attroff(attr);
}
root.refresh();
print("All done. Tap a key to quit.");
ncurses.beep();
var rc = root.getch();
root.close();
print("window name =",name,", getch =",rc);
print("Done! :-D");
