# Welcome to v8-juice! #
v8-juice (v8 (J)avaScript (U)serland (I)ntegratable (C)omponents (E)mporium) is a C++ library for extending the [v8 JavaScript engine](http://code.google.com/p/v8). It includes components such as [a flexible type-conversion API](ConvertingTypes.md), utilities for [creating new JS-side classes from C++](CreatingJSClasses.md), [binding near-arbitrary native functions to arbitrary JS objects](BindingFunctions.md), and [a plugins API](Plugins.md) (with several [plugins](PluginExpatParser.md) [binding](PluginCurl.md) [popular](PluginSQLite3.md) [3rd-party](PluginNCurses.md) [libraries](PluginReadline.md)). _And_ its got more docs than the whole of v8 itself (yes, v8 developers, that is a challenge to document your code!). If the API docs aren't enough, this project's wiki has _over 500kb_ of lovingly hand-crafted, cross-referenced text to help you along.

**You can read all about v8-juice on the HomePage.** (Or see juice's younger sister project, [V8Convert](V8Convert.md), which is where most development is happening as of mid-2011.)

**ACHTUNG ACHTUNG ACHTUNG:** as of August 19, 2013 this project is being abandoned due to the degree of incompatible changes in the core v8 made during the first half of of 2013. The v8 API is being changed drastically in ways incompatible with every single v8-using project out there, and to such a degree that it is likely to take 100+ of hours of work to fix cvv8 to work with the changes being made. At this point i _cannot_ commit to such an effort. If you use and appreciate cvv8, and would be willing to assist in the damage control/repair, please contact us on the mailing list (see the sidebar) or contact me directly (contact info is at http://wanderinghorse.net/home/stephan/). Without support in repairing that damage the v8 team has done, cvv8 is dead in the water.

## Project Stats ##
Provided by [Ohloh.net](http://www.ohloh.net):

&lt;wiki:gadget url="http://www.ohloh.net/p/v8-juice/widgets/project\_cocomo.xml" height="240"  border="0" /&gt;

&lt;wiki:gadget url="http://www.ohloh.net/p/v8-juice/widgets/project\_basic\_stats.xml" height="220" border="1" /&gt;

Those stats would appear, however, to be largely skewed. As of now (June 2011), the tree (including plugins) contains code which took something like 2.5-3 years of part-time work, and not the 20+ years Ohloh calculates. (Maybe it's because i use the terribly efficient [XEmacs](http://www.xemacs.org) as my editor.)