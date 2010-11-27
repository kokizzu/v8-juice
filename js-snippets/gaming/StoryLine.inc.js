
/**
   A StoryLine object holds all StoryLine.Elements associated with a story.
*/
function StoryLine()
{
    this.elems = [];
    this.nextIDNum = 0;
}

/**
   Adds the given StoryLine.Element, which must have a unique ID,
   to this storyline. elem.number and elem.story are modified
   by this call, and ownership of the elem is passed to this
   object.
*/
StoryLine.prototype.addElement = function(elem) {
    if( ! (elem instanceof StoryLine.Element ) ) {
        throw new Error("addElement() requires a StoryLine.Element object as its only argument.");
    }
    if( elem.id in this.elems )
    {
        throw new Error("addElement({StoryLine.Element id:"+elem.id+"}): this ID is a duplicate!");
    }
    elem.number = ++this.nextIDNum;
    elem.story = this;
    return (this.elems[elem.id] = elem);
};

/**
   Fetches a StoryLine.Element object by ID. If no such elem exists and autocreate
   is true (the default) then a new StoryLine.Element is created and added to this
   object's elem list.

   Returns a StoryLine.Element object, or null if autocreate is false and
   no such elem is found.
*/
StoryLine.prototype.getElement = function(id,autocreate) {
    if( (arguments.length < 2) ) autocreate = true;
    var a = this.elems[id];
    if( !a && autocreate )
    {
        a = this.addElement(new StoryLine.Element(id));
    }
    return a;
};

/**
   Creates a new StoryLine.Element with the given unique (per StoryLine
   instance) identifier.

   Public properties:

   .id = the ID passed to the ctor.

   .text = all text associated with the elem.

   .choices = the list of Choices for this element.

   function .getText() defaults to returning .text, but a client may
   reassign this to generate a custom text string. StoryLine.Element
   clients should call this instead of getting .text directly.

   Read-only properties:

   .number (read-only) = assigned by StoryLine.addElement().

   Optional properties (to be set by the client):

   function .beforeVisit is called before the StoryLine.Element is "visited," or
   directly before the element is rendered.

   function .afterVisit is called after the StoryLine.Element is "visited" and a choice is made.
   The function is passed the story element which corresponds to the given choice,
   or undefined if the selected choice has no corresponding story element (normally
   this only happens on a Quit request.



   It is simpler to call StoryLine.getElement() to get
   a new element, rather than create one and adding it
   to the storyine oneself.
*/
StoryLine.Element = function(id)
{
    if( (! id) ) throw new Error("StoryLine.Element('"+id+"') ctor requires an ID argument!");
    this.id = id;
    this.text = "Undescribed elem.";
    this.choices = []; // array of Choice objects.
    return this;
};
/**
   Default implementation returns this.text, but users may override
   this to perform some custom text generation.
*/
StoryLine.Element.prototype.getText = function() { return this.text; };

/**
   Adds a new Choice to the list of choices for this elem.  The areaID
   argument may be the ID of a StoryLine.Element or a Choice object. If it is a
   Choice object, the text argument is ignored.

   Returns a Choice object on success, or throws on error.
*/
StoryLine.Element.prototype.addChoice = function(areaID,text) {
   if( areaID instanceof StoryLine.Element.Choice )
   {
       this.choices.push(areaID);
       return areaID;
   }
   else
   {
       var c = new StoryLine.Element.Choice(areaID,text);
       this.choices.push(c);
       return c;
   }
};

/**
   This is the exception type used to trigger the end of a story.  All
   arguments are joined in a string, identically to how print()
   conventionally works but without an automatic trailing newline..
*/
StoryLine.TheEnd = function()
{
    if( arguments.length )
    {
        var av = Array.prototype.slice.apply(arguments,[0]);
        this.what = av.join(' ');
    }
    else
    {
        this.what = 'The End!';
    }
    return this;
};

StoryLine.TheEnd.prototype.toString = function() { return this.what; };

/**
   Throws a new TheEnd exception. All arguments are joined
   into the exception text as if by print() but without an
   automatic trailing newline.
*/
StoryLine.prototype.endStory = function()
{
    var av = Array.prototype.slice.apply(arguments,[0]);
    var ex = new StoryLine.TheEnd();
    ex.what = av.join(' ');
    throw ex;
}

/**
   A single option for selection in a StoryLine.Element. The elem argument must
   be-a StoryLine.Element or an elem ID. The text is optional (can be filled out
   later).
*/
StoryLine.Element.Choice = function(elem,text)
{
    this.text = text ? text : "Undescribed choice.";
    this.areaID = (elem instanceof StoryLine.Element) ? elem.id : ((elem !== undefined) ? elem :0);
}

/**
   Default implementation returns this.text, but users may override
   this to perform some custom text generation.
*/
StoryLine.Element.Choice.prototype.getText = function() { return this.text; };



/**
   A test/debug function which renders a StoryLine.Element object and its
   choices.
*/
function renderStoryLineElementPlain(elem)
{
    if( ! (elem instanceof StoryLine.Element) )
    {
        throw new Error("renderStoryLine.ElementPlain() requires a StoryLine.Element argument!");
    }
    print("************************************************************");
    print( "Scene #"+elem.number,"["+elem.id+"]" );
    print('\n   ',elem.text,'\n');
    if( elem.choices.length )
    {
        print('Choices:');
        var i = 1;
        for( var k in elem.choices )
        {
            var c = elem.choices[k];
            var a = elem.story.getElement(c.areaID);
            //print('\t'+(i++)+':',c.text,' (Go to #'+a.number,'['+c.areaID+'])');
            print('\t'+(i++)+':',c.text,'\n\t  --> Go to elem #'+a.number);
        }
    }
    else
    {
        print("No choices available :(");
    }
    print("************************************************************");
}

/**
   A test/debug function.
*/
StoryLine.testIt = function()
{
    var adv = new StoryLine();
    var a = adv.getElement('START');
    a.text = "This is the beginning. This is where it all begins. Someday this place will be where it all will have begun.";

    var chBarkeep = new StoryLine.Element.Choice('TALK_TO_BARKEEP',"Talk to the barkeep.");
    var chWaitress = new StoryLine.Element.Choice('TALK_TO_WAITRESS',"Talk to the waitress.");
    var chBarFight = new StoryLine.Element.Choice('START_BARFIGHT',"Try to start a barfight.");
    a.addChoice(chBarkeep);
    a.addChoice(chWaitress);
    a.addChoice(chBarFight);

    a = adv.getElement('TALK_TO_BARKEEP');
    a.text = "You try to butter up to the barkeep. He blesses you with a gruff look and a frown, then continues washing the glass he seems so intent on polishing.";
    a.addChoice(chWaitress);
    a.addChoice(chBarFight);

    a = adv.getElement('START_BARFIGHT');
    a.text = "You look for someone to start a fight with, but most of them are much smaller than you, and you can feel only sorry for them.";
    a.addChoice(chWaitress);
    a.addChoice(chBarkeep);

    a = adv.getElement('TALK_TO_WAITRESS');
    a.text = "Once you get closer you realize the waitress isn't a waitress at all, but a waiter.";
    a.addChoice(chBarkeep);
    a.addChoice(chBarFight);


    a = adv.getElement('THE_END_FAILURE');
    a.text = "This is where it all ends. This is where they will say it was to have ended.";
    a = adv.getElement('THE_END_SUCCESS');
    a.text = "You did it!";

    for( var k in adv.elems )
    {
        var ar = adv.elems[k];
        renderStoryLineElementPlain(ar);
        print(JSON.stringify(ar));
    }

    print(JSON.stringify(adv));
}
//StoryLine.testIt();

