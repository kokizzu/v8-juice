loadPlugin('v8-juice-ncurses');
ncurses.initscr();
ncurses.capture_cout(stdscr, ncurses.color_pair("white","blue"));
ncurses.capture_cerr(stdscr,ncurses.BOLD | ncurses.color_pair("red","black"));
function do1() {
    var NC = ncurses;
    NC.scrollok( stdscr, true );
    NC.wmove(stdscr,1,1);
    //NC.waddstr(stdscr,"Hi, world!");
    print("Hi, world!");
    sub = NC.newwin( 10, 30, 3, 5 );
    if( ! sub ) throw new Error("Subwindow creation failed!");
    NC.scrollok( stdscr, true );
    NC.wbkgd( sub, NC.color_pair("white","red") | ncurses.A_BOLD );
    //NC.wrefresh(stdscr);
    NC.wrefresh(sub);
    NC.capture_cout(sub);
    print("std::cout is now sent to to the subwindow, so print() output goes here.");
    var check = sub ? "yes" : "no";
    var key = NC.getch();
    var mv = ncurses.NCURSES_MOUSE_VERSION;
    var i2a = ncurses.i2a[key];
    var a2i = ncurses.a2i['*'];
    var cp = NC.color_pair("white","blue");
    var cpn = NC.color_pairnum("white","blue");
    var cl = NC.wclear(sub);
    NC.wrefresh(sub);
    NC.wclear(sub);
    NC.werase(sub);
    NC.delwin(sub);
    NC.wclear(stdscr);
    NC.wmove(stdscr,3,0);
    var attr = NC.color_pair("red","white") | ncurses.A_BLINK;
    NC.wattron(stdscr, attr );
    print("Subwindow deleted. Note that the cout redirection falls back to the previous redirection.\nTap a key.");
    NC.wattroff(stdscr, attr );
    NC.waddch(stdscr,ncurses.ACS_DIAMOND);
    NC.wrefresh( stdscr );
    NC.getch();
    NC.waddch(stdscr,ncurses.a2i[' ']);
    print('key =',key,'('+i2a+')','check =',check,'mv =',mv,'i2a =',i2a,'a2i =',a2i);
    print("cp =",cp,'cpn =',cpn,'cl =',cl);
    var sh = NC.wheight( stdscr );
    for( var i = 1; i < sh + 3; ++i )
    {
	print("See if auto-scrolling works... Line #"+i);
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
