load_plugin('v8-juice-ncurses-oo');

var nc = ncurses;

var w = new nc.NCWindow();
var rc = w.getch();
w.close();
print("getch =",rc);
print("Done! :-D");
