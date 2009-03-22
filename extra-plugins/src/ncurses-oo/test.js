load_plugin('v8-juice-ncurses-oo');

var nc = ncurses;

var w = new nc.NCWindow();
var name = w.name();
w.addstr("Hi, world!");
//w.refresh();
var rc = w.getch();
w.close();
print("window name =",name,", getch =",rc);
print("Done! :-D");
