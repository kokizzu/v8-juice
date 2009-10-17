/**
   Demonstration of implementing a "ticker" widget using the ncurses-oo
   plugin.

   A ticker widget displays a string of text, scrolling it sideways
   across the window.
*/
//loadPlugin('v8-juice-ncurses-oo');
include('setup-ncurses-app.js');
var nc = ncurses;
var root = nc.App.panels.root;
root.captureCout();
print("std::cout is redirected here.");
//nc.ripoffline(true);nc.ripoffline(true);nc.ripoffline(true);
nc.curs_set(0);
var name = root.name();
root.bkgd( nc.color_pair('white','blue') );



/**
   Sets up win, which must an NCWindow, to be the target for a scrolling ticker.
   The optional opt objects defines the options for the ticker.

   win gets a new object member, named ti, which contains several properties
   defining the ticker:

   - string .str = the string to scroll across the window.

   - int .xpos (read-only) = the current x position of the scroller

   - bool .paused = when set to true, doTick() won't do anything.

   - function resetTicker()

   - object offsets{y:Y,x:X} = the y/x offsets to draw the ticker at. See below.

   - function onReset(win) is called when the ticker is resent, which happens
   when doTick() advances the string off of the screen. It defaults to null.


   win gets a new function, doTick(), which advances and renders the
   ticker using the current ticker properties.

   More on win.ti.offsets:

   The ticker restricts itself to a single line. To allow that it can work
   with framed windows, one may provide a y/x offset at which to render
   the ticker. The x offset is used not only for the left side of the window,
   but also as right-side padding (with the assumption that the padding is
   there for a border).
*/
function setupTickerWindow(win,opt)
{
    if( ! (win instanceof ncurses.NCWindow ) )
    {
        throw new Error("The first argument must be an NCWindow!");
    }
    win.ti = {text:"",
              xpos:NaN,
              paused:false,
              offset:{y:0,//(win.height()>2) ? 1 : 0,
                      x:0},
              onReset:null
    };
    if( opt instanceof Object )
    {
        for( var k in opt )
        {
            //print("Importing option",k);
            win.ti[k] = opt[k];
        }
    }
    win.ti.xpos = NaN;
    win.resetTicker = function(){
        this.ti.xpos=NaN;
        // We do manual clear to (A) allow the client to reset
        // the ticker and (B) to try to play nice with any
        // parent decorations.
        var to = this.width() - (2*this.ti.offset.x);
        this.move( this.ti.offset.y, this.ti.offset.x );
        //print("Adding padding to pos",to,this.width());
//         this.addstr(Array(to+1).join(' '));
//         this.refresh();
        var sp = 32; // ascii space
        for( var i = this.ti.offset.x; i <= to; ++i )
        {
            this.addch(sp);
        }
        if(this.ti.onReset instanceof Function) this.ti.onReset(this);
    };
    win.doTick = function() {
        var ti = this.ti;
        if( ti.paused || (ti.text.length<1) ) return;
        if( isNaN( ti.xpos ) )
        {
            //ti.xpos = this.width();
            ti.xpos = this.width()-1 - ti.offset.x;
        }
        var at = ti.xpos;
        var str;
        if( (at < 0) )
        {
            if( (ti.text.length+at) <0 )
            { // reset...
                this.resetTicker();
                //ti.xpos = NaN; //this.maxy() - ti.offset.x;
                //if(ti.offset.y && ti.offset.x) this.box();
                return this.doTick();
            }
            str = ti.text.substr(-1*at);
        }
        else
        {
        str = ti.text;
        }
        if( at < 0 ) at = 0;
        var wlen = this.width() - at - (2 * ti.offset.x);
        if( str.length >= wlen ) str = str.substr(0,wlen);
        else str += ' ';
        this.mvaddstr( ti.offset.y + 0, ti.offset.x + at, str );
        --ti.xpos;
        this.refresh();
    };
    win.resetTicker();
    return win;
}

var ticker = nc.App.addPanel('ticker',
    new nc.NCPanel( 1, root.width(), 0, 0 )
    //new nc.NCPanel( 3, root.width(), 0, 0 )
    //new nc.NCWindow( root, 3, root.width(), 0, 0 )
    //new nc.NCWindow( root, 1, root.width(), 0, 0 )
                             );
//ticker.top();
ticker.bkgd( nc.color_pair('yellow','cyan') | nc.A_BOLD );

//ticker.addstr("This will be the ticker.");
ticker.quitChar = 'q';
ticker.quitKey = nc.intVal(ticker.quitChar);

setupTickerWindow(ticker,
                  {offset:{y:(ticker.height()>2) ? 1 : 0,
                            x:(ticker.height()>2) ? 1 : 0},
                   onReset:function(w){
                          ++w.ti.stringNum;
                          if( w.ti.stringNum >= w.ti.strings.length )
                          {
                              w.ti.stringNum = 0;
                          }
                          w.ti.text = '>> '+w.ti.strings[w.ti.stringNum]+' <<';
                          print("Ticker reset:",w);
                      },
                    strings:["Tap '"+ticker.quitChar+"' to stop.",
                             "Tap UP/DOWN to increase/decrease the ticker speed.",
                             "Tap 'p' to pause.",
                             "Tap 'n' to jump to next text."
                             ],
                    stringNum:-1,
                          msPerTick:200, // msec/tick
                          delayIncrement:50
                  });
if(ticker.ti.offset.y) ticker.box();
//ticker.ti.text = ticker.ti.strings[0];
for( var k in ticker.ti.strings )
{
    print(ticker.ti.strings[k]);
}

var tick2 = nc.App.addPanel('tick2', new ncurses.NCPanel(1,root.width(),root.maxy(),0) );
setupTickerWindow(tick2,{text:"http://code.google.com/p/v8-juice/wiki/PluginNCurses"});

function runTicker()
{
    var chwin = root;
    try
    {
        //nc.halfdelay(ticker.ti.delayTenths);
        chwin.timeout( ticker.ti.msPerTick );
        var ch = chwin.getch();
        for( ; ; ch = chwin.getch() )
        {
            //if( nc.ERR == ch ) continue;
            if( ticker.quitKey == ch ) { break; }
            if( nc.KEY_UP == ch )
            {
                if( ticker.ti.msPerTick > ticker.ti.delayIncrement )
                {
                    ticker.ti.msPerTick -= ticker.ti.delayIncrement;
                }
                else continue;
                print("Setting speed to",ticker.ti.msPerTick,'msec/tick.');
                chwin.timeout( ticker.ti.msPerTick );
                //nc.halfdelay(ticker.ti.delayTenths);
                continue;
            }
            else if( nc.KEY_DOWN == ch )
            {
                ticker.ti.msPerTick += ticker.ti.delayIncrement;
                //nc.halfdelay(ticker.ti.delayTenths);
                chwin.timeout( ticker.ti.msPerTick );
                print("Setting speed to",ticker.ti.msPerTick,'msec/tick.');
                continue;
            }
            else if( nc.intVal('p') == ch )
            {
                ticker.ti.paused = !ticker.ti.paused;
                continue;
            }
            else if( nc.intVal('n') == ch )
            {
                ticker.resetTicker();
                continue;
            }
            ticker.doTick();
            tick2.doTick();
            //print('tick. ch=',nc.charVal(ch),'tap "q" to stop.');
        }
    }
    catch(e)
    {
        //ncurses.nodelay(false)
        //chwin.nodelay(false);
        chwin.timeout(-1);
        for( var i = 0; i < 5; ++i ) ncurses.beep();
        print("EXCEPTION:",e);
        ncurses.napms(5000);
        chwin.getch();
    }
    finally
    {
        //ncurses.nodelay(false)
    }
}

//root.addstr("Hi, world! Tap a key to continue.");
//root.refresh();
runTicker();
//ticker.close();
root.refresh();
print("All done. Tap a key to quit.");
ncurses.beep();
//ncurses.flash();
//var rc = root.getch();
root.captureReset();
nc.App.close();
//root.close();
//nc.endwin(); // aarrgghh!
print("Done! :-D");
0;
