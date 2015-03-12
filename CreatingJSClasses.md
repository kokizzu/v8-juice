**Achtung:** the WeakJSClassCreator and ClassBinder APIs described below are considered obsolete, and have been superseded in functionality and flexibility by the [ClassWrap](ClassWrap.md) API. Several of the v8-juice plugins still use the older APIs (and porting them would require a significant effort), so the older APIs will be supported for the foreseeable future. More specifically: there are no plans to remove them, but new C++/JS bindings should use the ClassWrap API instead of the ClassBinder API described below. The JSClassCreator API, described below, is still current and is the basis of both ClassBinder and ClassWrap.

**Achtung #2:** this page documents the _current_ state of the library, which might be five minutes or a thousand years newer than any given version found on the downloads page.



# Introduction #

The JSClassCreator and WeakJSClassCreator classes are helpers which encapsulate
much of the drudge work associated with binding a native class to JS. This page
describes how to use them.

This API is likely to be continually tweaked, so if you use this API be prepared for changes down the road.

## See also... ##

  * ClassBinder was the first class-binding framework built on this API. Most of the v8-juice plugins use this system.
  * ClassWrap is the second generation of class-binding systems built upon this API, and is more flexible than ClassBinder. Newer plugins use this system.

## Getting the source code ##

While this code is part of v8-juice, much of it is independent of any
other v8-juice library components, which means it be extracted from the
v8-juice source tree for use in other v8-based projects. The source
files are:

  * [JSClassCreator.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/JSClassCreator.h)
  * [JSClassCreator.cc](http://code.google.com/p/v8-juice/source/browse/trunk/src/lib/juice/JSClassCreator.cc)
  * [WeakJSClassCreator.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/WeakJSClassCreator.h)

The ClassBinder type, described on its own page, extends `WeakJSClassCreator` to add member function/variable binding, but relies on the [v8::juice type conversions API](ConvertingTypes.md) and is therefore not standalone code.

# JSClassCreator #

The base binding class, JSClassCreator is suitable for creating new JS
classes which do not need to link to a specific native instance, or want
(or need) to do their own handling binding/unbinding of those instances.
The WeakJSClassCreator subclass adds the ability to automatically handle
the binding/unbinding of a native object to/from the garbage collection
system (in v8 this is called a "weak persistent pointer").

```
// JS-side constructor for MyClass objects...
Handle<Value> MyClass_ctor( Arguments const & argv );

// In your JS init code:
JSClassCreator binder( "MyClass", // JS class name
		targetObject, // "global" object (the class we be installed here)
		MyClass_ctor, // constructor callback
		1 ); 	// # of internal fields to reserve
binder.Set( "funcOne", my_callback_func )
      .Set( "valOne", String::New("hi there!") )
      ... more setters ...
      .Seal(); // Seal() MUST come last!
```

With the JSClassCreator, the client is responsible for:

  * In the ctor, put your native object somewhere where member native functions can get to it (e.g. put it in an Internal field in the JS new object).
  * In member functions, extract the native element as needed.

(The WeakJSClassCreator does those things for your, but lets start with the basics before we skip to that.)

Once that's in place we can do the following from JS:

```
var obj = new MyClass(...);
```

For the most basic cases, that's all that necessary, in terms of C++
setup. However, the most significantly missing feature of MyClass is
memory management. v8 provides a callback mechanism with which we can be
notified "when a context no longer needs an object." The callback
mechanism is, due to v8 design decisions, not 100% reliable (there is
never a _guaranty_ that our callback will be called!), which makes it
useless as a basis for resource management when destructors of certain
types _must_ be called to avoid resource leaks, data corruption, etc. To
be clear, when your application exits, the memory used by your bindings
_will_ be freed, but if they are not GC'd before that then their
_destructors_ will not be called. For most types a missing dtor call is
not all that critical, assuming the application is exiting anyway, but
for some types it's essential for proper operation.

# Backing up a bit: the garbage collection problem #

Let's back up a moment and describe the overall problem:

When an object is created in JavaScript, there is normally no client-visible "native" component to that value - it is an opaque type defined by v8, and has no visible association with a native value. That's all fine and good because the JS engine takes care of cleaning up those objects when it no longer needs them. When "binding" native classes to JS, we invariably associate a JS class with an instance of a C++ class (or some other native type - it need not be a class, e.g. it may be an opaque pointer type from a third-party C library). To do such bindings effectively we need at least these components:

  * A way to internally associate the native data with the JS object. In v8 this is done using `v8::Object::SetInternalField()` and the `v8::External` type. In fact, `v8::External` is the _only_ option for us here, because it is the only v8 type for which we can get a stable unique-per-process value (`External::Value()`), and having some sort of lookup key (in this case a void pointer) is an essential component of binding classes.
  * We "should" have a way to be notified when the JS part of an object is destroyed. In v8 this is accomplished by registering a "weak pointer" callback with the JS object. That callback will be called when (_if_) the v8 GC decides that there are no JS-side references to the object. When this callback is triggered, we can take care of properly deleting the native object and unbinding it from JS-land.

In practice there are a few wrinkles in that story, but we will skip over those now for the sake of clarity, and will touch on them again later on.

The documentation for v8's weak pointers support is a bit spotty, and one can easily spend hours experimenting before the first attempts at class wrapping work. _Or_ we can use WeakJSClassCreator, which was authored after many hours of painful experimentation and sorting through the vague documentation regarding weak pointers in v8...

# WeakJSClassCreator #

The WeakJSClassCreator is a JSClassCreator subclass which uses C++
templates to create functions for binding and unbinding JS objects
to/from native objects of a given type. It is based on the v8 "weak
persistent pointers" concept, and thus its odd name. It is used mostly
like JSClassCreator, the notable difference being a template
argument. As a template argument it takes a native type on whos behalf
it should act. In addition, client code must provide a `WeakJSClassCreatorOps<MyType>`
class template specialization for each bound type. That class is described fully in the API docs and briefly shown here:

```
namespace v8 { namespace juice {
    template <>
    struct WeakJSClassCreatorOps<MyClass>
    {
	/** The actual wrapped native type (may differ from the
            template arg in some cases!). */
	typedef MyClass WrappedType;

	/** Number of internal JS Object fields client needs for
	    his own use. */	
	enum { ExtraInternalFieldCount = 0 };

	/** Constructor function. */
	static WrappedType * Ctor( Arguments const &  /*argv*/,
				   std::string & /*exceptionText*/);

	/** Destructor function. */
	static void Dtor( WrappedType * obj );

	/** The JS class name. */
	static char const * ClassName() { return "MyClass"; }
    };
}} // namespaces
```

Again, the semantics of the API are documented in full
[in the source code](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/WeakJSClassCreator.h).

In brief:

  * The constructor is responsible for trying to create a `MyClass` object from the given arguments, but it is _not_ responsible for binding that native to the JS object. If it cannot create a new object, it must return 0 and may optionally put some descriptive error string in the given string reference (note that it is not const!), and the internal handler provided by WeakJSClassCreator will convert the error text into a JS-side exception. On success, the ctor must return a new instance of the native object. The internals of WeakJSClassCreator will then bind that native to JS using the weak pointer callback mechanism.
  * The destructor function must destroy the object using some type-specific logic (e.g. `delete obj` for most C++ types). Note that it may be called from the weak pointer trigger or it may be called via client-side action. e.g. a `OutStream.close()` function for a given type might want or need to destroy the native object, and the API provides `WeakJSClassCreator::DestroyObject()` to support that.

Now that we have a ctor and a dtor, we've got everything we need. From there we can let WeakJSClassCreator implement the more tedious bits for us:

```
typedef WeakJSClassCreator<MyClass> CC;
CC c( objectToAddClassTo );
c.Set("propertyOne", String::New("Hi, world!"))
 .Set("answer", Integer::New(42))
 .Set(...)
 .Set(...)
 .Seal(); // must be the last setter call made on this object.
```

Again, that's all there is to it. Now when we call `new MyClass(...)` from JS, the following will happen:

  * v8 will create a new _JS_ instance of MyClass (not a native instance).
  * v8 will pass that new JS object to an internal WeakJSClassCreator proxy function which will pass on the arguments to the client-specified specified constructor.
  * If the client-side ctor fails, the internal constructor will throw a JS-side exception (there is no other way to notify JS of a failed construction).
  * If the client ctor succeeds, the internal ctor will then set up a weak pointer callback, which will cause the client-specified dtor to be called _if_ the v8 GC ever decides to kick in. The internal ctor also packs the new native pointer into an internal field of the JS object for later reference by bound functions (see `WeakJSClassCreator::GetNative()` and `WeakJSClassCreator::GetJSObject()`).

The native object can be fetched later by passing a bound JS object to
one of the static methods `WeakJSClassCreator::GetSelf()` or
`WeakJSClassCreator::GetNative()`, both of which work similarly but have
very slightly different semantics. The former is expicitly intended to
be pass `argv.This()` from a bound member function, whereas the latter
can be passed a `Handle<Value>` and it will attempt to figure out if
it's of the wrapped native type. The second option is useful when
passing wrapped objects as arguments, as opposed to them being the
`this` object. Both forms use a type-safe conversion and will not crash
if passed a JS reference to an object with a different native type.

It is often useful to destroy a bound object from script code. The
classic example is a `close()` member of a Stream or Database class,
which should free the underlying device handle. This feature is simple
to implement: just call `WeakJSClassCreator::DestroyObject()` from the
appropriate close/destroy routine.

Clients can optionally tie in to their own "supplemental" GC (for when v8's GC doesn't kick in) by adding the logic to their `WeakJSClassCreatorOps<T>::Ctor()` and `Dtor()` routines.

If client code also uses the [v8-juice type conversions framework](ConvertingTypes.md),
wrapped types can easily be tied in to the automatic type conversions process (which _really_ simplifies passing around wrapped types to arbitrary bound functions). Since
that requires additional v8-juice functionality, and WeakJSClassCreator is intended to be "standalone" v8 code, that feature is covered in more detail on [the ClassBinder page](ClassBinder.md).

## Extra internal fields ##

The `WeakJSClassCreatorOps<T>` interface defines a constant integer value named `ExtraInternalFieldCount`, which can normally be left at 0.

If a wrapper needs access to more internal fields for client-side use,
define `ExtraInternalFieldCount` to be some other number. In v8 client
code, clients conventionally store their native data in the internal
fields numbered 0 to (N-1), where N is the number of fields. To avoid
interfering with this convention, `WeakJSClassCreator` stores its
internal binding at field number N (that is, one past the last
client-defined field). When the class is defined the internal field
count is set to (`ExtraInternalFieldCount`+1) and the wrapper
internals use that last position for their own purposes. The client is
then free to use slots 0..(N-1), as is conventional.

That said, i haven't yet seen any v8 client code which would benefit
from having more than one internal data field. Might be handy in some
cases, but it would in general seem easier to bind only one native object (i.e. the `T` type wrapped by `WeakJSClassCreator<T>`) and store any other bound data in that native object.

# Real-world examples #

There are some real-world examples of using these types in the v8-juice source tree:

  * [Pathfinder-js.cc](http://code.google.com/p/v8-juice/source/browse/trunk/src/lib/juice/PathFinder-js.cc) is a fairly straightforward example which wraps the majority of the `v8::juice::PathFinder` class using `ClassBinder`. Search that file for `SetupPathFinderClass` and you'll jump right to the relevant parts. HOWEVER, note that that implementation has been obsoleted in favour of one which uses the ClassWrap API.
  * The much more detailed [v8-whio.h](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/whio/v8-whio.h) and [v8-whio.cc](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/whio/v8-whio.cc) implement the [whio plugin](PluginWhio.md), binding a C API to JS space. This example is way more convoluted than normal, largely because it contains much evolutionary cruft from the various phases of `WeakJSClassCreator`'s development. It also demonstrates `WeakJSClassCreator<T>` where `WeakJSClassCreator<T>::WrappedType` is-not `T`, which might be interesting to someone out there.


# ClassBinder #

The `ClassBinder` class template, [described on its own page](ClassBinder.md), extends `WeakJSClassCreator` type to add the ability to bind member functions and member variables to native types. See that page for more information.

# ClassWrap #

The `ClassWrap` class template, [described on its own page](ClassWrap.md), is the "next generation" of the ClassBinder API, and supersedes it in functionality and flexibility. ClassBinder will stay around because i have plugins which use it extensively, but will not be touched except to fix (fixable) bugs. Newer wrappers are encouraged to use the ClassWrap API instead of ClassBinder.

# Real-world problems and caveats #

For general problems and caveats, see the [v8 criticisms page](CommentaryOnV8.md).

There are other caveats and gotchas which are associated with certain types of bindings, documented
here in the hopes that they might help other developers avoid them.

## Heirarchical Native Types and Garbage Collection ##

When working with heirarchical native types, we must ensure that JS is aware of the parent/child relationships.
If we do not, garbage collection might delete the child or parent before it should.

For example, assume we have a Window class which, like all good GUI toolkits, uses parent/child relationships
to control ownership of window objects:

```
function createWindows()
{
    var parent = new Window();
    var child = new Window(parent); // parent now owns child 
    return parent;
    // at this point, JS probably thinks that child is orphaned, and may clean it up.
}
```

If, in the Window ctor, we set up the native `parent`/`child`
relationship, but do not make JS aware that these is an association,
then JS will not know that there is a connection between the parent
and child objects. When it comes time to run GC, either the parent or
the child need to be deleted (in the above example, it will almost
certainly be the child).  If the native bindings do not take care to
disconnect the native objects when this happens, the next time the
parent (or child) tries to do something it may refer to the
now-deleted other object and cause a crash. Even if one avoids the
dangling pointer problem, not telling JS about the association can lead
to child windows mysteriously disappearing at curious times (when GC
is run).

One workaround for the disappearing child problem is to add a JS Array
object to the parent and simply insert the child into it. The array's
only purpose is to hold a reference to the child, so that GC will not
consider it for destruction (until the parent (and its Array) go
away).

In cases like these it is, in my experience, necessary to include a
few kludges in the native implementation to ensure that objects cannot
try to dereference a JS-destroyed object. The [ncurses plugin](PluginNCurses.md)
has several such kludges to help protect against premature deletion and
prevent dereferencing of a dangling pointer.


One simple and somewhat useful heuristic is: in the destructors for
the bound types, do not actually destroy the objects if they have
native parents, with the assumption that the parent object owns
it. This can lead to other problems, however, such as the JS-side
child being destroyed and the native one being there (and visible to
the client), and the client then wonders why his calls to
`child.func()` all throw with the error `"could not find native 'this' object"`
(that's what the v8-juice binding framework says when the
JS/C++ connection has been severed via the JS destruction process).