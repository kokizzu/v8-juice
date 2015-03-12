


# Introduction #

[V8Convert](V8Convert.md)'s `ClassCreator` API is a template-based mechanism for binding native classes to JS. It was originally derived from [ClassWrap](ClassWrap.md), making it the 3rd generation of class-binding mechanism since this project was started, and it is very similar to that binding mechanism. ClassCreator's source code can be found [here](http://code.google.com/p/v8-juice/source/browse/convert/include/v8/convert/ClassCreator.hpp).

> Note: this API represents _one_ possible solution (of many, many possible solutions) to the problem of binding classes to JS, and it is primarily targeted at binding C++ classes which the v8 user cannot modify to make them more binding-friendly. For many client types, custom v8-aware classes, or a base v8-aware class which is subclassed by bound types, would probably be more suitable/easier to use.

To allow customization of the binding process, `ClassCreator` uses so-called policy classes. These are templates which clients may specialize to customize various aspects of the bindings without having to touch the `ClassCreator` code. In effect they are "hooks" into various specific points of the class-binding mechanism, and every part of the class-binding machinery which can conceivably be customizable has an associated policy class to which that customization is delegated. e.g. one policy determines how we actually associate the native and JS objects with one another ("quick-and-easy" vs. "slower but typesafe"). Most, but not all, policies have reasonable default implementations, but a small number of them require clients to provide their own specializations (this is demonstrated below).

Let's just jump straight to an example...

# Demo: Binding A Class in just Moments #

```
//-----------------------------------
// My favourite debugging mechanism...
#define CERR std::cerr << __FILE__ << ":" << \
        std::dec << __LINE__ << ":" <<__FUNCTION__ << "(): "

//-----------------------------------
// Required header(s):
#include "cvv8/ClassCreator.hpp"

// Convenience namespace alias:
namespace cv = cvv8;

//-----------------------------------
// The type we want to bind to v8.
struct MyType
{
    MyType() {
        CERR << "MyType::MyType() @ "<<this<<'\n';
    }
    MyType( int i, double d ) {
        CERR << "MyType::MyType("<<i<<", "<<d<<") @ "<<this<<'\n';
    }
    MyType( char const * str ) {
        CERR << "MyType::MyType("<<str<<") @ "<<this<<'\n';
    }
    MyType( v8::Arguments const & argv ) {
        CERR << "MyType::MyType("<<argv.Length()<<" arg(s)) @ "<<this<<'\n';
    }
    ~MyType() {
        CERR << "MyType::~MyType() @ "<<this<<'\n';
    }
    
    // MyType constructors we want to bind to v8 (there are several other ways
    // to do this): This does NOT need to be a member of this class: it can be
    // defined anywhere which is convenient for the client.
    typedef cv::Signature<MyType (
        cv::CtorForwarder<MyType *()>,
        cv::CtorForwarder<MyType *(char const *)>,
        cv::CtorForwarder<MyType *( int, double )>,
        cv::CtorForwarder<MyType *( v8::Arguments const &)>
    )> Ctors;

    // A function we want to make visible to JS.
    int func( double ) { return 42; }
};

// A non-member function which we want to bind as a member of MyType.
// See below for how we get the native 'this' object at runtime.
void non_member_func( v8::Arguments const & argv );

//-----------------------------------
// Policies used by cv::ClassCreator (it also has others)...
namespace cvv8 {

    // Optional: used mostly for error reporting purposes but can
    // also be used to hold the class' JS-side name (which often differs
    // from its native name).
    CVV8_TypeName_DECL((MyType));
    // Out-of-line definition of TypeName<MyType>::Value. The declaration
    // should be visible to any client code which will use the bindings,
    // but the definition may only be done once (put it in the binding's
    // implementation file).
    CVV8_TypeName_IMPL((MyType),"MyType");

    // The policy which tells ClassCreator how to instantiate and
    // destroy native objects. This one forwards to one of the
    // ctors specified in MyType::Ctors.
    template <>
    class ClassCreator_Factory<MyType>
     : public ClassCreator_Factory_Dispatcher< MyType,
                           CtorArityDispatcher<MyType::Ctors> >
    {};

    // A JSToNative specialization which makes use of the plumbing
    // installed by ClassCreator. This is required so that
    // CastFromJS<MyType>() will work, as the JS/native binding process
    // requires that we be able to convert (via CastFromJS()) a JS-side
    // MyType object to a C++-side MyType object (so that we know what
    // native object to apply a given method/property to).
    template <>
    struct JSToNative< MyType > : JSToNative_ClassCreator< MyType >
    {};

    // There are several other policies not shown here.
    // e.g. ClassCreator_InternalFields defines the v8-side internal
    // field layout (it is rarely necessary to customized that, but
    // it has some obscure uses). We can also hook into the post-construction
    // and pre-destruction phases to perform custom un/binding work if needed.
}

//-------------------------
// How to fetch the native 'this' pointer from bound InCa-like functions:
//
// Because non_member_func() uses CastFromJS<MyType>(), it must be defined
// AFTER JSToNative<MyType> is declared, or the wrong specialization
// of JSToNative will likely be used! If we're lucky it will fail to
// compile. If we're unlucky it will cause a link-time error.
// If we do this in the right order then it will compile and link
// just fine.
v8::Handle<v8::Value> non_member_func( v8::Arguments const & argv )
{
    MyType * self = cv::CastFromJS<MyType>( argv.This() );
    if( ! self ) {
        return cv::TossMissingThis<MyType>();
        // ^^^ that throws a JS-side exception.
    }
    ....
}


//-----------------------------------
// Ultra-brief ClassCreator demo. See ConvertDemo.?pp for MUCH more detail.
void bind_MyType( v8::Handle<v8::Object> dest )
{
    typedef cv::ClassCreator<MyType> CC;
    CC & cc(CC::Instance());
    if( cc.IsSealed() ) { // the binding was already initialized.
        cc.AddClassTo( cv::TypeName<MyType>::Value, dest );
        return;
    }
    // Else initialize the bindings...
    cc
        ("destroy", CC::DestroyObjectCallback)
        ("func", cv::MethodToInCa<MyType, int (double),
                                  &MyType::func>::Call)
        ("nonMember", cv::FunctionToInCa<
                          void (v8::Arguments const &),
                          non_member_func >::Call )
        .AddClassTo( cv::TypeName<MyType>::Value, dest );
}
```

While there are lots of possibilities to customize the class-binding process, what is shown above is all that's needed for the basic cases. We generally do not have to modify the to-be-bound classes, but we often (due to differences in how C++ and JS see the world) have to write additional bits of shim/glue code to make specific bindings possible or more flexible.

If we simply construct/destruct a few of them from JS code:

```
(new MyType()).destroy();
(new MyType("hi")).destroy();
(new MyType(1,2.3)).destroy();
(new MyType(1,2,3,4,5)).destroy();
```

We will see something like:

```
demo.cpp:184:MyType(): MyType::MyType() @ 0x1308ee0
demo.cpp:200:~MyType(): MyType::~MyType() @ 0x1308ee0
demo.cpp:192:MyType(): MyType::MyType(hi) @ 0x1308ee0
demo.cpp:200:~MyType(): MyType::~MyType() @ 0x1308ee0
demo.cpp:188:MyType(): MyType::MyType(1, 2.3) @ 0x1308ee0
demo.cpp:200:~MyType(): MyType::~MyType() @ 0x1308ee0
demo.cpp:196:MyType(): MyType::MyType(5 arg(s)) @ 0x1308ee0
demo.cpp:200:~MyType(): MyType::~MyType() @ 0x1308ee0
```

Notice that the system's memory allocator re-allocated each new instance at the same address as the previously destroyed one. That is a mere fluke of the memory manager, and not a well-defined behaviour.

> Side note: that re-allocation behaviour is also incidentally a good demonstration of why class-binding mechanisms "should" (philosophically speaking) make an effort to perform _type-safe_ conversions from JS to the native type. Most binding mechanisms, and the approach demonstrated in the v8 docs, are not runtime-type-safe, and are subject to crashes or mis-use of re-allocated memory addresses (possibly now used by a _different_ type) if script-side code goes out of its way to confuse the binding mechanism. `ClassCreator` policies exist which perform "quick-and-easy" or "slower-but-typesafe" conversions. (Though philosophically highly arguable, the quick-and-easy binding (as demonstrated in v8's docs) is perfectly reasonable until someone writes some convoluted JS code which criss-crosses the bindings between two native objects.)

Real-world examples: the [addons directory](http://code.google.com/p/v8-juice/source/browse/#svn/convert/addons) of the source tree contains several complete, non-trivial examples of custom bindings, e.g. [JSPDO](JSPDO.md), [V8Convert\_Socket](V8Convert_Socket.md), and [ByteArray](V8Convert_ByteArray.md).

# Policies #

As mentioned above `ClassCreator` (CC) is policy-based. To customize certain behaviours we specialize specific policies (often by simply subclassing a convenience base class). This section gives an overview of the policies and their roles.

The ordering of declarations for CC policies is not always intuitive or immediately obvious. As a general rule:

  * `TypeName`, if used, should go first.
  * `ClassCreator_TypeID`, if used, should go next.
  * `ClassCreator_Factory` is normally required (the default implementation only supports a single default ctor taking no arguments).
  * `JSToNative_ClassCreator` (a convenience base class) invokes several other policies, so it normally needs to come late in the declarations. See the docs for that policy later on down this page.

The remaining orderings depend largely on how each policy is implemented and what other conversion operations they use. If they are declared separately from their implementations then there is rarely any ordering problems except in some cases where a subclassed policy invokes e.g. a not-yet-legal `JSToNative` conversion. However, separating the declarations and implementations can be tedious. It is, however, the safest best in terms of getting the ordering correct so that none of the specialized policies are invoked before they are declared. Instantiating (invoking) them too early will pick up an invalid (either type-wise or semantically) default specialization. Compilation will normally fail in such cases, and linking will fail in most others (with undefined symbol errors, normally meaning the template was never specialized for the given bound type but it was invoked somewhere and that invocation picked up an "invalid" default template). i've never seen a case where a "wrong" specialization both compiled and linked, though i can imagine that there are corner cases where that might theoretically happen.

## `TypeName` ##

`TypeName` is not strictly a CC policy, but it is often useful in conjunction with CC.

It is used mainly by some error reporting code but can also be used as a convenient place to store the type's JS name (which often differs from its C++ name).

It is used like this:

```
// In the header/declaration area:
namespace cvv8 {
   CVV8_TypeName_DECL((MyType));
}

// In the impl file:
namespace cvv8 {
    CVV8_TypeName_IMPL((MyType),"MyType");
}
```

After that `cvv8::TypeName<MyType>::Value` will hold the string `MyType`. While not required, having this specialization in place enables some error-reporting routines to return a useful type name (as opposed to "T" or some other generic value).

This policy "should" have a default value but we cannot give it one unless we sacrifice cross-DLL compatibility on Windows. (Long story!) We are seeking a solution to that problem.

## `JSToNative` ##

This is not a CC policy, but CC uses it and provides a way to create `JSToNative` specializations for CC-bound types:

```
template <>
struct JSToNative< MyType > : JSToNative_ClassCreator< MyType > {};
```

With that in place, `CastFromJS<MyType>()` will work (until the handle is somehow invalidated, at least).

`JSToNative_ClassCreator` instantiates the `ClassCreator_InternalFields` and `ClassCreator_SearchPrototypeForThis` policies, so any customizations of those policies must come _before_ `JSToNative_ClassCreator` is subclassed.

## `NativeToJS` ##

This is not a CC policy, but CC clients can plug in their natives so that a `NativeToJS` conversion is legal for their type.

Note that a `NativeToJS` conversion is generally only required when binding any of:

  * Functions which return the bound type, e.g. `T * T::self()`
  * Member properties of the bound type, e.g. `T * T::*selfPtr`

For most binding cases such conversions are not necessary, and thus no specialization of `NativeToJS` is required.

Implementing this conversion requires significantly more type-specific/type-dependent plumbing than `JSToNative` does, and there is no 99% (or even 50%) generic solution.

TODO: show an example. Until then, see the demo sources in the source tree.


## `ClassCreator_TypeID` ##

This policy is basically a shim for `TypeName` and `JSToNative_ObjectWithInternalFieldsTypeSafe`, to allow us to treat a `(char const *)` as `(void const *)` for templating purposes. The only time clients need to be aware of it is when subclassing one bound native from another _and_  (`ClassCreator_InternalFields<T>::TypeIDIndex >= 0`)). In such cases the derived types require a specialization which subclasses the parent specialization (they require the same type ID as the parent class, but this is arguably a bug):

```
template <>
    struct cvv8::ClassCreator_TypeID<BoundSubNative> : cvv8::ClassCreator_TypeID<BoundNative>
{};
```

If (`ClassCreator_InternalFields<T>::TypeIDIndex < 0`) then the CC-internal JS-to-native type-safety-check is disabled, in which case clients do _not_ need to (but may, if they like) specialize this policy.

The default implementation uses `TypeName` (see above), so if that type will be customized by the client then it must be done before this template is instantiated.


## `ClassCreator_InternalFields` ##

`ClassCreator_InternalFields` tells CC how the `v8::Object` "internal fields" are laid out, so that we know where to implant/extract native objects into/from their JS counterparts. For the vast majority of cases the default implementation is fine, but clients needing extra internal fields can add them (and optionally relocate the one CC uses) here.

```
    template <>
    struct cvv8::ClassCreator_InternalFields<BoundNative>
        // Use default settings for additional type-safety at runtime:
        //: ClassCreator_InternalFields_Base<BoundNative>
        // Add a number of interal fields for your own use, optionally
        // moving the TypeID and native object fields:
        : ClassCreator_InternalFields_Base<BoundNative,
                                           8/*# of fields*/,
                                           4/*TypeID field*/,
                                           7/*native object field*/>
        // Use a negative TypeID value to disable the extra type-safety checking:
        //: ClassCreator_InternalFields_Base<BoundNative,1,-1,0>
    {
    };
```

If the `TypeIDIndex` (the 3rd template parameter to the base type) is not negative then `ClassCreator_TypeID<BoundNative>::Value` will be stored in the `TypeIDIndex`th internal field of each bound object.

When subclassing one bound type from another, the subclasses _must all_ have the same internal field layout as the base type or the bindings won't be able to treat a subclass object as an instance of its base type. To do this, simply subclass the parent type's specialization. If there is a mismatch between parent/subclass internal fields, `JSToNative` conversions (`CastFrom()`) will fail in strange ways (depending on the exact context of the conversion) or fail completely.

## `ClassCreator_Factory` ##

`ClassCreator_Factory` tells CC how to instantiate and destroy native objects. It can often be specialized by subclassing a convenience base class, but sometimes it is useful to implement it by hand for particular use cases.

Here's an example which forwards JS calls to one of several different constructors, depending on the argument count (see [V8Convert\_Overloading](V8Convert_Overloading.md) for more options):

```
template <> class ClassCreator_Factory<MyType>
 : public ClassCreator_Factory_Dispatcher< MyType,
       CtorArityDispatcher< Signature< MyType (
           CtorForwarder<MyType *()>,
           CtorForwarder<MyType *(char const *)>,
           CtorForwarder<MyType *( int, double )>,
           CtorForwarder<MyType *( v8::Arguments const &)>
       )> >;
   >
{};
```

It might be interesting to note that individual `CtorForwarder` constructs in type-lists like the one shown above can in fact return derived types. However, the bindings will treat them as the base type for all purposes (including lookup of the native pointer). This has some interesting uses when binding C structs (which cannot subclass in the conventional sense but can emulate subclassing to a large degree by swapping out implementations of the interface functions defined in the struct).

## `ClassCreator_WeakWrap` ##

`ClassCreator_WeakWrap` is closely related to `ClassCreator_Factory` and gives us a place to hook in to the pre-construction, post-construction, and pre-destruction phases. The majority of cases do not need to specialize this policy. It is normally used only to add plumbing needed by `NativeToJS` conversions, but could also be used for logging purposes or similar things.

## `ClassCreator_SearchPrototypeForThis` ##

This policy tells CC (and the `JSToNative_ClassCreator` convenience class) whether or not a `CastFromJS()` conversions should recursively check the prototype chain if the JS `this` object does not contain the native. Such a lookup is only required when JS-side classes extend the bound native type. By default prototype lookups are enabled because the library-level code cannot predict whether JS classes will need to subclass the type or not.

If clients know that JS-side subclassing will never be used, or they want to intentionally break such uses (causing certain runtime conversions on the objects to fail), they can disable it like this:

```
template <>
struct ClassCreator_SearchPrototypeForThis<MyType> : Opt_Bool<false>
{};
```



## `ClassCreator_AllowCtorWithoutNew` ##

This policy tells CC whether or not (`new MyType()`) and (`MyType()`) are equivalent. By default `MyType()` without the `new` operator will cause a JS exception, but clients can cause both call styles to be equivalent with:

```
template <>
struct ClassCreator_AllowCtorWithoutNew<MyType> : Opt_Bool<true>
{};
```

There is no support for providing different behaviours depending on whether or not `new` was specified, e.g. calling different constructor functions for each case.

## `ClassCreator_SetupBindings` ##

`ClassCreator_SetupBindings` is _intended_ to be the generic interface for initializing a given CC binding. It is not used by any of the public API, and is intended to genericize CC-based initialization in downstream code.

Intended usage:

```
ClassCreator<T>::SetupBindings( myGlobalObject );
```

That will (if this policy has been specialized) initialize the bindings for the given native type. If it has not been specialized, that will throw a native exception! It is up to the implementor to ensure that the bindings are not actually processed multiple times in the life of the application. The various demo/example programs (like the one at the top of this page) demonstrate one way to do that.

See `ClassCreator_SetupBindings_ClientFunc` for a convenience base type which proxies a client-provided function with this signature:

```
typedef void (*SetupBindings)( v8::Handle<v8::Object> const &);
```

The reason for that particular signature is largely historical, but that class can be used as a model for implementing similar classes which set up client-defined functions/functors to initialize the CC bindings. It is sometimes slightly less work, or less problematic vis-a-vis visability of specific templates, to implement such a function and subclass `ClassCreator_SetupBindings<T>` from `ClassCreator_SetupBindings_ClientFunc` to wrap it.

# Tips and Tricks #

## Binding a Destructor Function ##

It is trivial to bind a JS function which will force a call to the native destructor:

```
typedef ClassCreator<MyT> CC;
CC & cc( CC::Instance() );
cc( "destroy", CC::DestroyObjectCallback );
```

As long as the native type cleans everything up in its dtor, as opposed to requiring that a specific method be called before destruction, this is arguably the simplest way to _ensure_ clean-up of native resources from script code.

In my experience, native C++ types like i/o streams and database drivers will, when destructed, call their `close/finalize/whatever()` method, and the above binding indirectly ties in to such resource-freeing functions.

## Creating new Instances of Bound Classes ##

We can of course create new "v8-aware" instances of bound classes. However, they are _owned by v8_ and will be destroyed (or not) whenever v8 wants to (or not) destroy them. Clients can use `ClassCreator::DestroyObject()` to force (via v8) destruction of the native object.

```
typedef ClassCreator<MyType> CC;
MyType * m = NULL;
v8::Handle<v8::Object> obj = CC::Instance().NewInstance( 0, NULL, m );
if( ! m ) {
  ... something went wrong. A JS exception _might_ be propagating...
}

// Or with arguments:

v8::Handle<v8::Value> args[] = {
  CastToJS(7),
  CastToJS(24.42),
   ...
};

v8::Handle<v8::Object> obj =
  CC::Instance().NewInstance( sizeof(args)/sizeof(args[0]), args, m );
```

## _Properly_ Destroying v8-created Bound Natives ##

Given a v8 Handle to a ClassCreator-created object, we can destroy the associated native object and disconnect v8 from the native:

```
CC::DestroyObject( theJSObject );
```

Remember that we can bind the destruction function to JS:

```
cc("destroy", CC::DestroyObjectCallback);
```


Both approachs will, if the JS object is bound to a native, end up calling the native destructor and disconnecting the JS object from that native. A side-effect of that disconnection is that future calls (from script code) to native methods will end up throwing a JS exception because the underlying native is no longer there. This is correct behaviour - the alternative would be to step on the now-deallocated pointer (i.e. Undefined Behaviour).

If we simply destroy the native object ourselves (i.e. `delete myNative`) then v8 will _still_ have a handle to the native pointer, which is now invalid and might be re-allocated to a different object later on. That is of course Bad News.

**The moral of the story is:** the JS and native halves of the object (A) are owned by v8, so any destruction must happen via v8 (e.g. via `CC::DestroyObject()`), and (B) have an intimate relationship, so don't go deleting the native pointer.