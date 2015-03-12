

# Non-critique Commentary #

## Why use v8? ##

For several years i've maintained a project called [SpiderApe](http://SpiderApe.sf.net), which is basically a library like v8-juice but built for the SpiderMonkey JavaScript engine (the only used by Mozilla and friends). SpiderMonkey isn't actually all that bad except that it suffers from a serious lack of documentation, especially for threading-related issues. i haven't written any code for Monkey in some time, and when i found out about v8 (late February, 2009) i was very excited to finally have access to a C++-based JS engine. So i started hacking.

My initial experiments took the form of porting sqlite3 bindings from
SpiderApe to v8-juice. Ninety percent of the code was a 1-to-1 copy, mostly
changing the names of the JS/C++ conversion operators. So, on the surface v8
and SpiderMonkey would seem to have very similar interfaces and semantics (they
don't, actually, but the usages are nonetheless very similar). So then came
the question, "why use v8 if the client code looks just like SpiderMonkey code?" One
good reason is that v8 allows us to register object destructors for use by the
garbage collector. (However, i only found out later that the v8 GC is never
guaranteed to call those destructors - see below for a rant on that topic.)

All of that said, i'm probably one of the world's biggest fans of Google's technology offerings, and i couldn't resist the chance to experiment with their JavaScript engine. Long term, i am convinced that v8 will become the dominant C++ ECMAScript implementation.

## Excellent Licensing Terms ##

One strong advantage v8 has over older, more mature JS engines, like SpiderMonkey and QtScript, is its license. With its BSD license it can be included into commercial projects without license contamination. It also allows me, the extension author, to license my v8-using code however i want to. SpiderMonkey, which is MPL (basically GPL), and QtScript, which is QPL/GPL/Commercial, cannot provide this level of freedom. (Ironic, isn't it, seeing as the GPL's primary purpose is to protect the freedom of its users?)

## It's Frigging Fast ##

v8 is damned fast compared to SpiderMonkey (the engine used by the Mozilla family of web browsers). Function calls, for example, are about _200 times_ faster. i cannot say that _all_ operations are faster (i haven't measured them). Interestingly, early versions of v8 were really slow when calling functions which were members of an object (about 10x slower than free functions, IIRC), but changes in early 2009 fixed that, and the speed difference between calling member- vs. non-member functions is now much smaller (about 10%-20% slower than calling a free function).

# Criticisms #

In my experience, these are the significant problems and caveats associated
with binding classes in JS, as demonstrated on the [CreatingJSClasses](CreatingJSClasses.md) page.

## No GC guaranty ##

> "Let me explain. No, no, there is too much. Let me sum up." - Inigo Montoya

v8 allows us to register "weak pointer callbacks" which get called when a
given JS object is to be destructed by the GC system. However, even if we
register destructors, v8 may decide to _never_ call the GC. Doh. So we still
never get a guaranty that our destructors are all. This is, in my opinion, a
half-ass non-solution. For bound types which require a destructor call for
proper cleanup, it is useless as a cleanup mechanism, requiring the client to
build add-on GC support. That, in turn, requires adding some form of API which
we can call to say "clean up any objects which v8 didn't get around to
cleaning up." That intrudes not only on the library author, but also on any
clients of that library (i.e. it is intrusive). Another wrench in this
approach is that we, at the library level, cannot know if v8 is done working
yet (i.e. if the app using our library may still need to execute JS code).
This can only be known by the top-level app which creates the v8 context.

Anyway... there's lots more bitching and moaning to do here, but i've already
said it all elsewhere and i'm just beating a dead horse.

Oh, except to say: reportedly, the reason which v8 never guarantees that GC
will be called (even when it is shutting down and therefore cannot be
executing client-side JS code), is for "performance reasons." So, it trades
performance for resource leaks. Great. Just what we need in a script-language library,
where objects are indiscriminately created by the thousands by script users
who have no idea what the underlying call to `malloc()` just robbed them of.

To the v8 team's credit, though, their intentions behind shutting down without running GC beforehand was to speed up their Chromium browser, for which v8 was originally authored. In the context of their browser, where each web page is its own system process and JS does not use native types which require a dtor call for proper semantics, this limitation is not so serious. However, in uses outside of Chromium, where we'd like to bind database handles, output streams, and such, it is extremely annoying, however.

## The Stack and static functions ##

v8's stack-centric API makes it difficult to keep shared copies of a JS objects for passing around amongst native code. e.g. if we want to make a JS class from the main library or app available to plugins, we can't just have a static copy of JS handle (`v8::Handle<T>`) in the main library which we can pass around because the static object is not allowed to be created until after v8 is initialized. In my experience, instantiating a static JS handle will crash the app somewhere in v8 unless we're careful about where that static is instantiated.

## Post-`main()` Issues ##

Post-`main()` issues are a real-world problem all v8 clients face, especially anyone who wraps classes using the weak pointer mechanism. In short, the problem is this: v8's API, instead of having the user pass around his context info (as in SpiderMonkey, and as v8 should do), keeps it all in static/shared functions. Sometime post-`main()`, that data will be destroyed. If we have a wrapped class which is not destroyed by v8 (because its GC didn't kick in) but is destroyed by a supplemental garbage collector (long story), the object being destroyed must not interact with v8 in any way (type conversions, creating new handles, etc.), as v8 may have been destroyed and the class would then crash (again, post-`main()`). Any time you see crashes post-`main()` in v8 client code, the first suspect is delayed destruction of shared objects which live outside v8 but may interact with it. The only known workaround for some of these problems is to not clean up the long-living objects and let them be destroyed without calling their destructors. Indeed, that is the only approach supported by v8 (which never guarantees that it will destruct an object, even if we go through the trouble of registering a destructor).

## Narrowed Applicability due to Design Constraints ##

In my opinion, most of these problems are a direct side-effect of v8 having
been primarily designed for use in a single application, namely the Google
Chrome browser. (One of the v8 developers admitted so much when he said the
reason v8 doesn't guaranty that GC will _ever_ be called was made on the basis
of improving Chrome's performance.) This led them to make some design
decisions which severely limit the more widespread integration of v8 in other
applications. My biggest gripe here is that the JS state is all hidden away in
shared data, accessible via static functions, instead of being associated with
a context handle which is passed around to all client-side functions which use
the API (as in SpiderMonkey, and that approach goes a long way towards eliminating
some of the lifetime issues mentioned above).

Having said that: v8's approach will apparently make it possible to write SWIG wrappers for v8. In SpiderMonkey this is not feasible, largely because SWIG apparently cannot account for the JSContext argument which is required to be passed to all client functions. (At least that's my relatively uninformed analysis, after looking into implementing SWIG bindings for SpiderMonkey a couple years ago.)

## WTFM ##

**_Write The Fucking Manual!_**

Another hindrance is the relative lack of v8 documentation. Most functions and classes aren't documented or have only a single line with no real information (e.g. what _exactly_ does "Enters the context" mean?). This means a great deal of experimentation in order to find out if something will or won't work in v8 (and another amount of guesswork as to whether it's actually _legal_ to do it that way in v8).

Any library which is to enjoy widespread use _must_ be well documented. It's a simple fact. History is full of libraries which thrive despite lacking docs (libexpat comes to mind), but i find that shameful.

The members of the mailing lists have been extraordinarily helpful in helping me understand v8 usage, and it is only because of them that i've been able to accomplish anything beyond "hello, world!" in v8.

As i enjoy writing, and i tend to write "insane" API docs (to quote one user), i did contact the v8 team about contributing API documentation. However, the process of getting involved in the project is just too much effort for my tastes. Why can't i just upload a patch, without having to install (and learn) a custom set of tools? Maybe one day Google will pay me to document v8 (dream on!), but until then using v8 will require a lot more experimentation and guesswork than it really should.