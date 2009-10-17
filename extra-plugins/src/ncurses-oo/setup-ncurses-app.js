/**
   Loads the ncurses-oo plugin and sets up an object named
   ncurses.App, which encapsulates some commonly-used
   functionality for curses app.
*/
try
{
    loadPlugin('v8-juice-ncurses-oo');
    ncurses.App = {};
    var app = ncurses.App;
    app.panels = {};
    /** Calls close() on all added panels. */
    app.close = function()
    {
        for( var k in app.panels )
        {
            var p = this.panels[k];
            if( ! (p instanceof ncurses.NCWindow) ) continue;
            try{p.close();}
            catch(e){}
        }
        this.panels = {};
    };
    /** Adds a panel to the close() list. */
    app.addPanel = function( name, p )
    {
        if( ! (p instanceof ncurses.NCPanel ) )
        {
            throw new Error("Second argument must be-a NCWindow!");
        }
        app.panels[name] = p;
        return p;
    }
    var  p = new ncurses.NCPanel();
    //p.bkgd( ncurses.intVal(' ') );
    app.addPanel( 'root', p );
    /** Refreshes the root panel. */
    app.refresh = function() { return this.panels.root.refresh(); };
    app.getch = function() { return this.panels.root.getch(); };
}
catch(e)
{
    if(ncurses) ncurses.endwin();
    print("EXCEPTION:",e);
    throw e;
}
ncurses.App; // return value for include()
