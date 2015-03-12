The readline plugin provides a minimal API for reading user input from the keyboard. It can be configured (via compile-time options) to use GNU Readline, BSD Editline, or plain stdin as its input source. The programming interface is the same, however.

# Getting the plugin #

This plugin, due to its licensing (mixed-license with a GPL component), is stored as an add-on in a separate source tree. It's in the `extra-plugins` Subversion tree. Visit this page for instructions:

http://code.google.com/p/v8-juice/source/checkout

and check out the `extra-plugins` directory instead of `trunk`.

# Building/installing #

See [the plugins page for build instructions](Plugins.md).

# Example usage #


```
var my = {fn:"my.history"};
loadPlugin('v8-juice-readline',my);
if( my.readline.loadHistory(my.fn) ) {
    print("Loaded history file",my.fn);
} else {
    print("Couldn't load history file",my.fn);
}
var s;
while( undefined != (s = my.readline.read("Enter something (Ctrl-D quits): ")) )
{
    print("Result:",s);
}
print(""); // kludge: ensure a newline after Ctrl-D
print("Done!");
// history will be auto-saved at shutdown
```

That demonstrates all of the functions except for `clearHistory()`, which clears the history list, and `saveHistory()`, which saves the history. If `saveHistory()` is passed no filename then it uses the name which was passed to `loadHistory()`.

Points of note:

  * An application only has only one input terminal and therefore only one shared instance of readline.
  * If loadHistory() is ever called then the history will be saved automatically at shutdown. If saveHistory() is ever called, the name set there will be used for auto-save.
  * ACHTUNG: auto-save is not reliable because v8 does not guaranty that destructors are ever called. Thus one should use `saveHistory()` to explicitly save the history, either periodically (e.g. after reading each line) or after exiting the read loop. A try/catch block around the read loop is recommended so that history can be saved in the face of an exception.