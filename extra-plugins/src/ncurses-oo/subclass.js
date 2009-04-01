include('setup-ncurses-app.js');
var nc = ncurses;
var root = nc.App.panels.root;
root.captureCout();
function MyPanel()
{
    var av = Array.prototype.slice.apply(arguments,[0]);
    //this.__proto__ = new nc.NCPanel(av);
    //this.prototype = this.__proto__;
    this.prototype = this.__proto__ = new nc.NCPanel(av);
    this.toString = function() {
//      //return '[MyPanel:'+this.__proto__.toString()+']';
         return '[MyPanel:'+this.prototype.toString()+']';
    };
    return this;
}
MyPanel.prototype.toString = function() {
   throw("This will never be called b/c MyPanel types change their prototype object!");
    return undefined;
};
//MyPanel.prototype = new ncurses.NCPanel(1,root.width(),0,0);
// print('prototype =',MyPanel.prototype);
// MyPanel.prototype.bkgd( nc.color_pair('black','white') | nc.intVal(' '));
// MyPanel.prototype.hide();

var p = new MyPanel(10,10,4,4);
print('p =',p);
p.bkgd(nc.color_pair('white','red'));
p.addstr("This is a JS-side subclass of NCPanel!");
print('p instanceof NCPanel =?',p instanceof nc.NCPanel);
print('p.prototype instanceof NCPanel =?',p.prototype instanceof ncurses.NCPanel);
print("Tap a key to quit.");
p.refresh();
p.bottom();
root.refresh();
nc.App.getch();
p.close();
nc.App.close();
