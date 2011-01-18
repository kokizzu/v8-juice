loadPlugin('v8-juice-ncurses');
function myrip(win,rownum) {
    myrip.lines[myrip.lines.length] = win;
}
myrip.lines = [];

var riprc = [];
if(1) {
    for( var i = 0; i < 5; ++i )
    {
	riprc[riprc.length] = ncurses.ripoffline((i%2) ? -1 : 1,myrip);
    }
}
ncurses.initscr();
ncurses.capture_cout(stdscr, ncurses.color_pair("white","blue"));
ncurses.capture_cerr(stdscr,ncurses.BOLD | ncurses.color_pair("red","black"));
function do1() {
    var NC = ncurses;
    NC.scrollok( stdscr, true );
    NC.wmove(stdscr,0,0);
    print("Hi, world! Rip count: ",myrip.lines.length);
    //NC.wrefresh( stdscr );
    for( var i = 0; i < myrip.lines.length; ++i )
    {
	NC.wprint(myrip.lines[i],"Ripped-off line #"+(i+1));
    }
    NC.getch();
    if(1)
    {
	var sh = NC.wheight(stdscr);
	for( var i = 1; i < sh + 3; ++i )
	{
	    print("See if auto-scrolling works... Line #"+i);
	}
    }
}

var my = {};
try {
    do1();
}
catch(e){my.exc = e;print(e);}


print("Press a key to quit.");
ncurses.getch();
ncurses.endwin();
if(my.exc) throw my.exc;
print("ripoffline rc =[",riprc,'], riprc.length =',riprc.length);
