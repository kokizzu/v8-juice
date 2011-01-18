loadPlugin('v8-juice-ncurses');
nc_initscr();
nc_capture_cout(stdscr);
nc_scrollok(stdscr,true);
nc_noecho();
var mask = ncurses.ALL_MOUSE_EVENTS;
nc_mousemask(mask);
if(1) {
    sprintf("current mousemask() == %08x",nc_mousemask());;
    print("Play with the mouse. Tap 'q' to quit.");
    var key = 0;
    while( ncurses.a2i['q'] != (key = nc_wgetch(stdscr)) ) {
	if( key == ncurses.KEY_MOUSE ) {
	    var evt = nc_getmouse();
	    print( "mouse :",key,evt.toString() );
	}
    }
}
nc_endwin();

