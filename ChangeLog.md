# v8-juice ChangeLog #

This is the informal ChangeLog and "news list" for the v8-juice project. Newest items
are at the top. Dates written extra large are release dates. This contains only a high-level list of changes. The exact list of changes can be fished out of our source code repository at:

  * http://code.google.com/p/v8-juice/source/browse
  * http://code.google.com/p/v8-juice/source/list


---


**2012 June 30:**
  * Added initial support for [sqlite4](http://sqlite.org/src4) (which hasn't yet been released) to [JSPDO](JSPDO.md).

**2011 Aug 15:**
  * Added [V8Convert\_Whio](V8Convert_Whio.md).

**2011 July 24:**
  * Added `FunctorToInCa` to bind functors to JS.
  * Introducing the [X-to API](V8Convert_XTo.md), alternate interfaces for binding functions, methods, and variables (arguably _even simpler_ than the ones we have so far).

**2011 July 23**
  * Completely refactored the cvv8 [property binding API](V8Convert_PropertyBinding.md) to mimic the [function binding API](V8Convert_FunctionBinding.md).
  * Moved the MSVC support into the [V8Convert](V8Convert.md) trunk. Coen added project files for VS 2010. He suspects (but we haven't tested) that the code will work on VS 2008 as well.

**2011 July 21**
  * Started a new mailing list: http://groups.google.com/group/v8-juice-js-dev

**2011 July 20**
  * **WOO HOO!** As proof that the internet works: as of [r1920](https://code.google.com/p/v8-juice/source/detail?r=1920) (Central European Time) Coen Campman and i got [V8Convert](V8Convert.md) (or a branch of it, anyway) building on MSVC (over Google Chat, i might add). This requires a few minor incompatible changes, but nothing too painful. Here's a snippet from our chat session (ever so slightly edited for readability):

```
...
Coen:             V8Unlocker<UnlockV8> const unlocker();
again in invocable_core.hpp
 me:  replace like we did before
V8Unlocker<UnlockV8> unlocker;
 Coen:  just did 
seems to do the trick
...
 Coen:  now for some more testing ;P
hehe 
 me:  that means that 99% of the code will work. now i just have to weed out the extra stuff which probably won't work yet.
did you run demo2?
 Coen:  no didn't run it yet
 me:  i'm about to piss my pants with excitement - do it!
 Coen:  haha yeah still have to properly call some of the stuff ^_^
 me:  and when we're done i'll post this part of the chat session in the wiki 
 Coen:  haha 

...

let's see if this stuff works 
it doesn't crash xD
 me:  lol!
 Coen:  but i don't have a console to view output in xD
 me:  that's a great start

... after much fiddling with msvc ...

 Coen:  
var test = new MyType();
print(test);
test.constMethod();
test.nonConstMethod();
print(test.nonMember(42));
 Coen:  d:\path\to\cvv8\demo2.cpp:20:MyType::MyType(): MyType::MyType() @ 00A18358
[object Object]
d:\path\to\cvv8\demo2.cpp:35:MyType::aConstMethod():
d:\path\to\cvv8\demo2.cpp:34:MyType::aNonConstMethod():
d:\path\to\cvv8\demo2.cpp:40:a_non_member():  arg=(42)
42
^_^
 me:  GREAT 
Coen:  yeahh 
 me:  thanks a LOT for helping getting this working
 Coen:  np
 Coen:  didn't require that much changes actually which is awesome ;D
 me:  i still need to clean up some of the code, and some of the docs are no longer valid, but the hard part is behind us.
and it was actually pretty easy 
 Coen:  not sure it works on 2008 for example though  but i think the compiler didn't change that much

...
me:  doh... and i need to go fix all the plugins...
 Coen:  :(
 me:  no big deal - this is a big improvement and i don't mind making changes for those
but i'll probably go blind from all the compiler errors ;)
```

(It's a shame that the emoticons don't copy/paste. (some were added back in for clarity.))

_Many_ thanks to Coen for committing his time and resources to this.

**2011 July 19**
  * Reached [r1900](https://code.google.com/p/v8-juice/source/detail?r=1900), up 400 commits from exactly one calendar month ago. All of that was in the [V8Convert](V8Convert.md) tree.
  * New wiki page: [V8Convert\_TipsAndTricks](V8Convert_TipsAndTricks.md)
  * [V8Convert](V8Convert.md): renamed the `InCa` template to `InCaToInCa` and renamed `Callable` to `InCa`.

**2011 July 18**
  * **Completely incompatible change:** `v8::convert` namespace was renamed to `cvv8`, to preemptively avoid any collisions with future changes in v8. Moved the headers from `v8/convert/` to `cvv8/` for the same reason. (And i learned a bit about symbol lookup resolution in embedded namespaces in the process.)

**2011 July 17**
  * Added "non-trivial function overloading" - see [V8Convert\_Overloading](V8Convert_Overloading.md).
  * Renamed several types for consistency.
  * Backwards-incompatible change: `v8::convert::CastToJS(std::exception)` no longer calls `v8::ThrowException`. The old behaviour was due to a misunderstanding on my part. `v8::juice` still has the old behaviour (and will keep it, since it is heavily used in plugin/client code).

**2011 June 26**
  * Implemented prototype/proof-of-concept templates in [V8Convert](V8Convert.md) which allow us to dispatch JS-called native functions to different native functions depending on any rules we can apply to the arguments. e.g. we can have different overloads based on the number of arguments, their types, and can perform essentially arbitrary logic. Now that the concept is proven, it "shouldn't" be long before we have a useful API for composing dispatch rules in a manner similar to composing template-based parsers (like with the `Boost::Spirit` library).
  * Came up with a simple, lightweight mechanism to figure out if a JS object's native pointer _really_ is of the correct type. Refactored [V8Convert](V8Convert.md)'s class binding mechanism to use this, which closes a couple holes where it would be possible for (badly misbehaving) JS code to force an invalid `static_cast()` at the native level (and subsequent misuse of that native pointer).

**2011 June 25**
  * Added `ToInCa` template to [V8Convert](V8Convert.md), bringing function bindings/conversions to an all-new level of simplicity.
  * The [V8Convert](V8Convert.md) function bindings now optionally (via a template parameter) support unlocking the v8 engine while the bound function is running.

**2011 June 24**
  * The past several days have seen near record activity levels (some 120 commits in 5 days), almost exclusively in the [V8Convert](V8Convert.md) API, its demo/test/add-on code (like [JSPDO](JSPDO.md), [V8Convert\_Socket](V8Convert_Socket.md), and [V8Convert\_ByteArray](V8Convert_ByteArray.md)), and the related documentation. `v8::convert` has gotten a number of subtle additions and internal improvements over the past days. i'm half tempted to move it into its own Google Code project.
  * i renamed several of the core-most templates in `v8::convert`, but i'm not aware of anyone else using that API yet, so i won't lose sleep over code compatibility breakage. If it affected you, it's your own fault for not telling me you're using the code ;).

**2011 June 21**
  * Added some [V8Convert](V8Convert.md) demo code which shows how to add native-to-JS conversions.
  * Added `v8::convert::NativeToJSMap<>` to assist in creating native-to-JS mappings.
  * Got binding to `T [const] & X::func() [const]` and `any X::func( T [const] & ) [const]` working, provided that `T` and `X` are (A) bound to JS and (B) convertible to/from JS using `CastFromJS()` and (for the first conversion) `CastToJS()`.

**2011 June 19**
  * Added an experimental [socket API](V8Convert_Socket.md) based on [V8Convert](V8Convert.md).
  * The project hit its 1500th commit today, which averages out to just over 3 commits per day since the code was imported into Google Code (before that it was maintained elsewhere for a few months).

**2011 June 13**
  * Added [JSPDO](JSPDO.md) a couple days ago.
  * Added [an off-site editor for the main wiki](http://whiki.wanderinghorse.net/wikis/v8-juice/), which i now use as a staging ground for wiki edits.

**2011 Feb 2**
  * Added several const-overload-resolution fixes to templates in the the v8::juice::convert API. Thanks go to Sai Koushik for reporting the problem and providing code which demonstrated it.

**2011 Jan 23**
  * Added the [v8::convert](V8Convert.md) sub-project.

**2011 Jan 22**
  * Inspired by the grace of vu8's API, i've started work on a spin-off of v8-juice called v8-convert. It currently contains only the [core type conversion](ConvertingTypes.md) and a function-binding API similar to [the current function-binding code](BindingFunctions.md), but using function-ptr-style template args to make it more human-readable. Unlike v8-juice, it is a header-only implementation, with the intention that it be easy to drop in to arbitrary v8 clients. i will add a basic class-binding mechanism, but nothing so heavyweight/complex as that provided by v8-juice. The initial code is working well, and once i've added a few missing features and documented it, i will release it as a standalone sub-project of v8-juice.

**2011 Jan 6**
  * Added `v8::juice::GetV8StackTrace()`, which implements the `v8::InvocationCallback` interface and allows us to fetch stacktraces from our JS code. It is available in the `v8-juice-shell` (or via the `JuiceShell` class) with the JS name `getStackTrace()`.

**2010 Dec 29**
  * ~~Joel Reymont has made v8-juice available [via github](https://github.com/wagerlabs/v8-juice)~~. (It has since been removed.)

## 2010 November 26 ##
  * Happy birthday to my brother Gerald!
  * Fixed another int64/(long long) ambiguity in the `v8::juice::convert` API for some 64-bit platforms, which fixes a link-time error in the sqlite3 plugin.
  * Finally made a new release (it's been a year already, if you can believe that).

**2010 June 6**
  * Fixed an int64/(long long) ambiguity for some 64-bit platforms. The sqlite3 plugin now builds on those platforms.

**2010 April 8**
  * Integrated a fix for [issue #19](https://code.google.com/p/v8-juice/issues/detail?id=#19). Thanks go to Feng Fillano for localizing and reporting the problem.

**2010 April 7**
  * Added `spawnTimeoutThread()` and friends, which are multi-threaded variants of the standard `setTimeout()` (and friends) functions). The current `setTimeout()` (and friends) impls are not strictly standards-conformant, and will eventually be replaced by something that is. Until then, `setTimeout()` is simply an alias for `spawnTimeoutThread()`.

**2010 March 30**
  * A templates-related compilation fix for an error which should have been triggered long ago but never was for some reason.

**2010 March 18**
  * Moved one of the class binding demos into its own plugin: [PluginPosixFILE](PluginPosixFILE.md)

**2009 Nov 21**
  * Got function binding working for functions taking (`char const *`) arguments, with a few fairly minor limitations.

**2009 Nov 18**
  * Added [libcurl plugin](PluginCurl.md).

## 2009 Nov 15 ##
  * It's about time for a new release.

**2009 Nov 14**
  * Added an [XML parser plugin](PluginExpatParser.md).

**2009 Nov 9**
  * `JSToNative<std::string>` now treats strings as UTF8 instead of ASCII. (Thanks go to Rob Tsuk.)
  * `NativeToJS<uint32_t>` now behaves properly. (Thanks go to Steven Grafton.)

**2009 Oct 28**
  * Re-implemented the [PathFinder class](PathFinderClass.md) using the [ClassWrap API](ClassWrap.md). The amount of code is about the same, but it's now trivial to swap out some of the back-end parts to make trade-offs for speed and memory consumption vs. type-safety and conversion features.

**2009 Oct 25**
  * Added `CtorForwarder` class, which can pass on `v8::Arguments` objects to native constructors.
  * The new class binder ([ClassWrap](ClassWrap.md)) can now bind a JS ctor to multiple overloaded native ctors.
  * Added bits which allow us to bind (independent of the class binder) JS functions to multiple native overloads, such that the function which gets called depends on the argument count. e.g. `foo.overloaded()` and `foo.overloaded(1,2,3)` may be bound to different native functions (or member functions).
  * Fixed a post-`main()` crash in `v8-juice-shell` when using the latest versions of v8 (where they changed some locking-related assertions).

**2009 Oct 22-24**
  * Significant work done on the new function binding approach (see [BindingFunctions](BindingFunctions.md)).
  * Introduced the `v8::juice::convert::TypeTraits<>` class to help pass around proper type information related to JS/Native conversions.

**2009 Oct 21**
  * (edge branch) We have a much more flexible/generic native-to-JS function binding mechanism which can be used to bind near-arbitrary functions to arbitrary JS objects with a single line of code (with compile-time type safety). If `JSToNative()` and `NativeToJS()` are implemented for a JS-bound native, then native member functions may also be bound to their JS counterpart. This can be used in conjunction with [ClassBinder](ClassBinder.md) and friends or with arbitrary JS objects.

**2009 Oct 17**
  * Started factoring out the functionality of `v8-juice-shell` into the `JuiceShell` class, to make it more reusable.
  * Added an optional (and experimental) optimization for `WeakJSClassCreator`, to speed up conversion to/from JS/Native when cross-JS/Native inheritance is not required.
  * `v8-juice-shell` now gives the global object a name (`v8juice`). The bound function `load_plugin()` has been renamed to `loadPlugin()`, for naming consistency.

**2009 Oct 15**
  * Created a new branch called [edge](http://code.google.com/p/v8-juice/source/browse/#svn/branches/edge) for trying out some up-coming changes/additions.
  * Added to ClassBinder: ability to bind JS member propertes to native accessor/mutator functions. See [PathFinderClass](PathFinderClass.md) for an example which uses this.

**2009 Sept 13**
  * v8-juice-shell now passes all arguments after "--" to the executed script(s), where arguments[0](0.md)==argv[0](0.md) and arguments[1..N] are the arguments passed after "--".

**2009 Sept 12**
  * Added `setTimeout()`, `clearTimeout()`,  `setInterval()`, `clearInterval()` and several variants of `sleep()` (which relinquishes control to other threads while it sleeps). The timeout/interval functions work similarly to their browser-side cousins. See [BindableFunctions](BindableFunctions.md).
  * Other minor touchups i've been collecting over the summer.

**2009 May 27**

  * Patch from robtsuk: `v8::juice::IncludeScript()` (JS `include()`) now returns a live reference instead of a dead one, fixing a crash when the return value of `include()` was dereferenced.

## 2009 April 4 ##
  * New release.

**2009 March 31**
  * Added toggles to [WeakJSClassCreator](CreatingJSClasses.md) to allow constructors to be called without `new` and to activate searches for the native `this` object via JS-side prototypes (necessary if JS types are to subclass native types).
  * Minor additions to the [ncurses plugin](PluginNCurses.md).

**2009 March 28/29**
  * Moved the remaining plugins from the core tree to the [extra-plugins source repo](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src).
  * The [convert.h code](ConvertingTypes.md) is now independent of the rest of the library.
  * The [WeakJSClassCreator cast operators](CreatingJSClasses.md) can now cast from JS to `(T&)` and `(T const &)`. This means functions taking reference types can now be bound.  The conversions must throw a std::exception if the conversion fails, to avoid dereferencing null. The function forwarding framework will catch such exceptions and return the result as a JS exception, rather than letting the native exception propagate through v8 (which can lead to a corrupt stack).
  * Added the `--print-cerr` command line option `v8-juice-shell`. This forces the JS `print()` function to use `std::cerr` for output instead of `std::cout`, which is often helpful when debugging [curses applications](PluginNCurses.md) (we can then redirect `print()` to another console without hosing the curses screen state).


## 2009 March 25 ##
  * New release.

**2009 March 24**
  * Found a way to support dual native- and JS-side inheritance via [WeakJSClassCreator](CreatingJSClasses.md), and refactored [the ncurses plugin](PluginNCurses.md) to use it.
  * The past few days have mostly been spent reimplementing [the ncurses plugin](PluginNCurses.md), which is once again in a usable state (but has a much different API than before).

**2009 March 21**
  * Started adding `ToSource()` to convert native types to JS source code. Has some notable limitations but basically works.
  * Added gzip/gunzip support for [input streams](PluginWhio.md).
  * Added [embedded filesystem plugin](PluginWhefs.md).

## 2009 March 18 ##
  * Released new tarball

**2009 March 17**
  * More work on the sqlite3 plugin. Reimplemented `sqlite3_exec()` using the newer OO API.

**2009 March 16**
  * Added support for binding JS/C++ member variables (as opposed to functions). Supports all types which work with `CastToJS()` and `CastFromJS()`.

**2009 March 15**
  * Started reimplementing the [sqlite3 plugin](PluginSQLite3.md) to use class binding, rather an v8::External, for most argument handling.

**2009 March 13/14**
  * Refactored member function binding code significantly.
  * Many minor cleanups.
  * Completely wrapped [the PathFinder class](PathFinderClass.md) in JS using only `ClassBinder`.


**2009 March 12** (after the release, i think)
  * Got binding to non-const member functions, and those returning void, working.

## **2009 March 12** ##
  * Released new version.

**2009 March 9/10**
  * Added initial JS class creation helpers. See [CreatingJSClasses](CreatingJSClasses.md) wiki page for info.
  * General improvements to the [i/o plugin](PluginWhio.md).

**2009 March 8**
  * Added [extra-plugins](http://code.google.com/p/v8-juice/source/browse/#svn/extra-plugins) directory to svn, for plugins with hard dependencies on external/3rd-party software. Moved sqlite3/ncurses plugins into that tree.
  * Added new plugin: [readline](PluginReadline.md)
  * Several little improvements/additions to the [I/O classes](PluginWhio.md).

## 2009 March 7 ##

  * Released the first tarball.

**2009 March 6**

Started porting in the original source tree to Google Code.