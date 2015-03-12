

# Contributing to v8-juice #

Anyone is welcomed to contribute to this project - just
[get in touch](http://wanderinghorse.net/home/stephan). Coders, documenters, commentors, code reviewers, bug submitters, etc.... they're all welcomed. Even marketing people and armchair code analysts.

Particular areas where help is currently sought are:

  * Porting the build tree to scons, for platform portability
  * Adding plugins, especially bindings for popular 3rd-party toolkits.

## Contributing code ##

Potential contributors must be willing to follow a very simple set of guidelines:

New code _must be documented_. The documentation belongs in the header files (e.g. doxygen style). Commenting of implementation code (the `.cc` or `.cpp` files) is left to personal judgement and taste - there are no rules. Any function, class, typedef, or macro which is not documented cannot be part of a public API. Internal/private functions/types/macros must also be documented if they are used in more than one implementation file.

(And because of that rule, this library, at one week old, already had more API docs than v8 does.)

Nobody's asking for doctorate-level documentation, but some description of what the code does is a must. The existing API docs are good guideline (though sometimes there's admittedly a bit more documentation than is strictly necessary).

Other than that, i'm pretty much open to anything.

# Contributors #

So far they include:
  * [Stephan Beal](http://wanderinghorse.net/home/stephan) is the primary architect and author.
  * Anton Yemelyan joined the project in March 2011 to try to get the code working on Windows, and is using it in projects developed at [LSI SACO Technologies Inc](http://www.smartvision.com/).
  * Rob Tsuk has contributed several fixes and made a number of suggestions which led to new features or fundamental improvements.
  * James Pike, author of [vu8](https://github.com/tsa/vu8), helped me through the learning curve of getting function-pointer-style template arguments working for the [V8Convert](V8Convert.md) API. These allow for more readable templates when compared to the v8-juice API.
  * Coen "The Pipe" Campman got [V8Convert](V8Convert.md) running on Windows for us.
  * Bionic Badger provided several Windows portability patches to [V8Convert\_Whio](V8Convert_Whio.md).
  * The folks at http://groups.google.com/group/v8-users have been very helpful in all things related to v8.