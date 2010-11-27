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
    app.client = {};
    app.panels = {};
    app.itemsToClose = [];
    /** Calls close() on all added panels. */
    app.close = function()
    {
        for( var k in app.itemsToClose )
        {
            var p = this.itemsToClose[k];
            if( ! (p instanceof ncurses.NCWindow) ) continue;
            try{
                if( 'onClose' in p )
                {
                    p.onClose();
                }
                p.close();
            }
            catch(e){}
        }
        this.itemsToClose = [];
    };
    /** Adds win to the close() routine, so that close() will also
        close() it. if onCloseFunc is-a Function then it is assigned
        to win.onClose and called when app.close() is called.
    */
    app.addToClose = function(win,onCloseFunc)
    {
        if( ! (win instanceof ncurses.NCWindow ) )
        {
            throw new Error("Argument must be-a NCWindow!");
        }
        if( onCloseFunc instanceof Function ) win.onClose = onCloseFunc;
        app.itemsToClose.push(win);
    };

    /** Adds a panel to the close() list. */
    app.addPanel = function( name, p )
    {
        if( 'string' != (typeof name) )
        {
            throw new Error("addPanel() requires a string as the first argument!");
        }
        if( ! (p instanceof ncurses.NCPanel ) )
        {
            throw new Error("Second argument must be-a NCPanel!");
        }
        app.panels[name] = p;
        app.addToClose( p );
        return p;
    }
    /** Refreshes the root panel. */
    app.refresh = function() { return this.panels.root.refresh(); }
    app.addPanel( 'root', new ncurses.NCPanel() );
    app.getch = function() { return this.panels.root.getch(); }
    ncurses.curs_set(0);
}
catch(e)
{
    if(ncurses) ncurses.endwin();
    print("EXCEPTION:",e);
    throw e;
}
ncurses.App; // return value for include()
