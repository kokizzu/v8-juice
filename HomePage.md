

# Welcome to v8-juice! #
v8-juice (v8 (J)avaScript (U)serland (I)ntegratable (C)omponents (E)mporium) is a C++ library for extending the [v8 JavaScript engine](http://code.google.com/p/v8). It includes components such as [a plugins API](Plugins.md), [a type-conversion API](ConvertingTypes.md), utilities for [creating new JS-side classes from C++](CreatingJSClasses.md), and for [binding near-arbitrary native functions to arbitrary JS objects](BindingFunctions.md). It has extensive API documentation, and supplemental documentation can be found via the Wiki link at the top of this page.

Alternately, the [V8Convert](V8Convert.md) API provides a header-only library which implements extended/improved versions of the v8-juice core type conversions and function/property/class binding features, omitting features which would require non-header implementations. As of mid-2011, the vast majority (over 99%) of the development is happening in the [V8Convert](V8Convert.md) tree, and v8-juice is being touched only for bugfixes and an occasional back-port of v8-convert features.


To any members of the Campbell Soup Company: i would really appreciate it if you guys [don't sue me over this project's name](http://en.wikipedia.org/wiki/V8_(beverage)) :-D.  It's all in good fun! (And it took me 10 minutes to come up with a project name for which 'JUICE' is an appropriate acronym. So i beg you, please don't let that effort be in vain!)

## Code/Project status ##

**ACHTUNG ACHTUNG ACHTUNG: a series of incompatible deprecations and behaviour changes made to v8 in late 2012 and early 2013 have broken the "classic" v8-juice library and _no effort will be made to update it to accommodate v8's changes_. [V8Convert](V8Convert.md), on the other hand, has been/is being updated to work with newer v8 versions.**

v8-juice is primarily an experimentation ground, and not a product which aims to have a "1.0" release (it uses the release date as its version number). That said, i like to keep my own client code working, and will try not to make any major changes which break things unless there is a good reason to do so. Also, the core library parts (primarily the [type conversions](ConvertingTypes.md), which are the most-central component) are based off of [much older implementations i wrote for SpiderMonkey](http://SpiderApe.sourceforge.net), and they've had a stable interface for some years now.

v8-juice is believed to work well, without any undocumented crash conditions and whatnot. It is used in at least one high-profile commercial product (which i unfortunately am not at liberty to name) to simplify the C++/JS binding process. In particular, the newer [V8Convert](V8Convert.md) API makes binding C/C++ functions and classes to JavaScript really simple (making it any simpler would probably require a code generator).

**Having said that:** since mid-2011, v8-juice itself is relatively static. It _is_ actively maintained and reported bugs will be fixed insofar as is possible within the library's current architecture and compatibility constraints. Future work is going into the [V8Convert](V8Convert.md) API, however, and at some point the main `v8::juice` API "might" be re-based on top of `v8::convert`, in effect becoming a 3rd-party user of the `v8::convert` API. [V8Convert](V8Convert.md) focuses primarily on the conversion of types and functions between C++/JS. `v8::juice` does that, plus adds several "heavy-weight" features like [a plugins API](Plugins.md). The main difference between the two APIs is that [V8Convert](V8Convert.md) is header-only, whereas v8-juice is much more heavyweight and cannot realistically be implemented as a header-only library.

## Related work ##

  * [vu8](https://github.com/tsa/vu8) provides many of the same features as v8-juice, and is one of the few bindings frameworks i've seen which uses templates to generate the bindings (as opposed to macros). Its class- and module-binding system is also easier to use that juice's. <em>If i wasn't using v8-juice, i'd be using vu8.</em> The [V8Convert](V8Convert.md) spin-off API was largely inspired by vu8.
  * [cproxyv8](http://code.google.com/p/cproxyv8/) is very similar in purpose to v8-juice, and appears to provide largely the same feature set for binding C++ types to v8.
  * [v8cgi](http://code.google.com/p/v8cgi/) encapsulates features for hosting server-side web applications written in JavaScript.
  * [nodejs](http://nodejs.org/) has a very impressive set of functionality and a well-thought-out module loading system. It is arguably the most popular v8-using project aside from the various Chromium-based browsers.
  * [Flusspferd](http://flusspferd.org/) is a similar library written for use with the [SpiderMonkey JavaScript engine](https://developer.mozilla.org/en/SpiderMonkey). (No longer maintained, as of mid-2011.)
  * [SpiderApe](http://spiderape.sourceforge.net) is another (no longer maintained) C++ wrapper for the SpiderMonkey engine, and v8-juice's [type-conversion API](ConvertingTypes.md) was originally adapted from that library.

One of the interesting things about vu8, cproxyv8, v8cgi, and nodejs is that they're not mutually exclusive with v8-juice. While there is some overlap in functionality, each can provide some parts which the other cannot. e.g., in my humble opinion v8-juice's main strengths lie in its native/JS type-conversion bits, whereas nodejs has a much nicer module-based plugin system than the one v8-juice provides. vu8 has a much nicer class/module-binding system as well.

## License ##

The code base contains code of several licenses:

  * Most of the core library uses the same license as v8: the New BSD license
  * Some of the core code is Public Domain and much of the code adopted from other source trees is Public Domain. Most future code will likely be Public Domain.
  * [Plugins](Plugins.md), which often use external libraries, may have arbitrary licenses. They are stored in their own source tree (outside of the main v8-juice sources) to avoid potential license contamination from viral licenses.

The whole library _would_ be Public Domain, but Google Code neither recognizes nor allows Public Domain as a project license. Since v8-juice is an extension of v8, which has a BSD license, it is assumed that a BSD license will be acceptable to potential v8-juice users who have accepted v8's licensing terms.

Users needing other license conditions may request them from the code author(s). Tip: for any v8-juice code where i'm the attributed author (Stephan Beal), you've got blanket permission to do whatever the hell you want to with your copy of the code (including re-licensing it). The exception to that is any code which necessarily adopts viral licensing conditions from 3rd-party libraries it uses, such as the [GNU Readline plugin](PluginReadline.md).

## Contributors ##

Please see the [Contributing](Contributing.md) page.

## Mailing List ##

http://groups.google.com/group/v8-juice-js-dev

## Project Stats ##
Provided by [Ohloh.net](http://www.ohloh.net):

&lt;wiki:gadget url="http://www.ohloh.net/p/v8-juice/widgets/project\_cocomo.xml" height="240"  border="0" /&gt;

&lt;wiki:gadget url="http://www.ohloh.net/p/v8-juice/widgets/project\_basic\_stats.xml" height="220" border="1" /&gt;

Those stats would appear, however, to be largely skewed. As of now (April 2009), the tree (including plugins) contains code which took less than a year of part-time work, and not the 9 years Ohloh calculates. (Maybe it's because i use the terribly efficient [XEmacs](http://www.xemacs.org) as my editor.)