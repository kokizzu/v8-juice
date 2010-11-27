include('StoryLine.inc.js');
include('setup-ncurses-app.inc.js');

var nc = ncurses;

function setupUI()
{
    var app = nc.App;
    app.ui = {};
    var p = app.panels.root;
    p.captureCout();
    var x = new nc.NCPanel( p.height()/3*2, p.width(),1,0 );
    x.bkgd( nc.color_pair('white','blue') );
    app.addPanel('storyElement',x);
    var c = new nc.NCFramedPad(x,x.height()*3,x.width()-2);
    app.addToClose(c);
    app.ui.storyElement = c;
    c.scrollok(true);
    c.mapKeyToReq( nc.KEY_CTRL('G'), nc.NCPad.PadReqExit );
    //print('KEY_ENTER =',nc.KEY_ENTER,nc.NCPad.PadReqExit);
    // no workie? c.mapKeyToReq( nc.KEY_ENTER, nc.NCPad.PadReqExit );

    app.ui.status = new nc.NCPanel( 1, p.width(), 0, 0 );
    app.ui.status.bkgd( nc.color_pair('yellow','cyan') | nc.A_BOLD );
    app.ui.status.addstr("Status bar (someday).");
}

function ncRenderStoryElement(elem,win)
{
    if( (arguments.length<2) || !(win instanceof nc.NCWindow))
    {
        win = app.ui.storyElement;
    }
    if( ! (elem instanceof StoryLine.Element) )
    {
        throw new Error("First argument must be-a StoryLine.Element! Instead got: "+(typeof elem));
    }
    win.erase();
    if( win instanceof nc.NCPad )
    {
        win.requestOp( nc.NCPad.PadReqScrollToTop );
    }
    win.refresh();
    win.attron(nc.A_BOLD);
    win.addstr("Scene #"+elem.number+" ["+elem.id+']:\n\n');
    win.attroff(nc.A_BOLD);

    if( elem.beforeVisit instanceof Function )
    {
        elem.beforeVisit();
    }

    win.addstr(elem.getText()+'\n');
    if( elem.choices.length )
    {
        win.attron(nc.A_BOLD);
        win.addstr('\nMake a choice:\n');
        win.attroff(nc.A_BOLD);
        var i = 1;
        var chattr = nc.A_STANDOUT | nc.A_UNDERLINE;
        for( var k in elem.choices )
        {
            var c = elem.choices[k];
            var a = elem.story.getElement(c.areaID);
            win.addstr('\t');
            win.attron(chattr);
            win.addstr((i++));
            win.attroff(chattr);
            win.addstr(': '+c.getText());
            //win.addstr('\n\t  --> Go to scene #'+a.number);
            win.addstr('\n');
        }
    }
    else
    {
        //win.addstr("No choices available :(\n");
    }
    win.refresh();
}


function setupAdventure()
{
    var adv = new StoryLine();
    var a = adv.getElement('START');
    a.text = "This is the beginning. This is where it all begins. Someday this place will be where it all will have begun.";

    var chBarkeep = new StoryLine.Element.Choice('TALK_TO_BARKEEP',"Talk to the barkeep.");
    var chWaitress = new StoryLine.Element.Choice('TALK_TO_WAITRESS',"Talk to the waitress.");
    var chBarFight = new StoryLine.Element.Choice('START_BARFIGHT',"Try to start a barfight.");
    var chLeave = new StoryLine.Element.Choice('THE_END_SUCCESS',"Leave the bar.");
    a.addChoice(chBarkeep);
    a.addChoice(chWaitress);
    a.addChoice(chBarFight);
    a.addChoice(chLeave);

    a = adv.getElement('TALK_TO_BARKEEP');
    a.text = "You try to butter up to the barkeep. He blesses you with a gruff look and frown, then continues washing the glass he seems so intent on polishing.";
    a.addChoice(chWaitress);
    a.addChoice(chBarFight);
    a.addChoice(chLeave);

    a = adv.getElement('START_BARFIGHT');
    a.text = "You look for someone to start a fight with, but most of them are much smaller than you, and you can feel only sorry for them.";
    a.addChoice(chWaitress);
    a.addChoice(chBarkeep);
    a.addChoice('THE_END_FAILURE',"Fight to the death!");
    a.addChoice(chLeave);

    a = adv.getElement('TALK_TO_WAITRESS');
    a.text = "Once you get closer you realize the waitress isn't a waitress at all, but a rather hairy waiter.";
    a.addChoice(chBarkeep);
    a.addChoice(chBarFight);
    a.addChoice(chLeave);

    a = adv.getElement('THE_END_FAILURE');
    a.text = "This is where it all ends. This is where they will say it was to have ended.";
    a.afterVisit = function() { this.story.endStory("Those little guys creamed you!"); };
    a.beforeVisit = function() { print("Dammit! The bastards are tougher than they look!"); };

    a = adv.getElement('THE_END_SUCCESS');
    a.text = "You did it!";
    a.afterVisit = function() { this.story.endStory("You won!"); };

    return adv;
}

//StoryLine.testIt();
nc.App.exception = null;
try
{
    setupUI();

    /** Process the given StoryLine.Element, wait for a choice, and return
        the chosen element. Returns undefined when the exit key is
        tapped.
    */
    function doElement(elem)
    {
        if( elem.beforeVisit instanceof Function )
        {
            print("Running pre-visit function.");
            elem.beforeVisit();
            print("Ran pre-visit function.");
        }
        ncRenderStoryElement( elem );
        var key = 0;
        var quitKey = nc.KEY_F(10);
        print("Entering input loop. Tap F10 to quit.");
        var next = undefined;
        while( elem.choices.length && (quitKey != (key = app.getch())) )
        {
            if( app.ui.storyElement.consumeKey(key) ) continue;
            if( (key >= 48 /*ascii 0*/) && (key <= 57) )
            { // numbers 0..9
                var v = key-48;
                var ndx = v-1;
                if( elem.choices[ndx] )
                {
                    next = elem.story.getElement( elem.choices[ndx].areaID );
                    break;
                }
            }
            print("Unmapped key:",key);
        }
        if( elem.afterVisit instanceof Function )
        {
            print("Running post-visit function.");
            elem.afterVisit(next);
        }
        if( next )
        {
            print("Choice selected: story element #"+next.number);
            return next;
        }
        return undefined;
    }
    nc.App.story = setupAdventure();
    var adv = nc.App.story;
    var elem = adv.getElement('START');
    var next = null;
    while( (next = doElement(elem)) )
    {
        print("Advancing to elem #"+next.number);
        elem = next;
    }
}
catch(e)
{
    if( e instanceof StoryLine.TheEnd )
    {
        print("The game has ended.");
        print(e);
    }
    else
    {
        if(ncurses) ncurses.endwin();
        print("EXCEPTION:",e);
    }
}

print("Done adventuring. Press a key to quit.");
nc.App.getch();
nc.App.close();
print("Done!");
if( nc.App.exception )
{
    print("Something threw:",nc.App.exception);
    throw nc.App.exception;
}
