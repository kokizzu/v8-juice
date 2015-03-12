**ACHTUNG: this page documents a new API and is far from complete.** The `ClassWrap` API was introduced on 20091025 - it is quite new and may still change significantly.

Please read the [ClassWrap](ClassWrap.md) page for an overview of the binding API before reading this page.



# ClassWrap Policies #

ClassWrap (as documented on its own page) is policy-driven. The core binding
API provides only a small amount of the real code needed in binding
classes, and anything which can/should be genericized is defined by
various policy classes. This page provides an overview of the policies
and their uses.

Polices are template classes (or "class templates", if you prefer)
which are specialized or partially specialized to provide features
required for binding a specific type to JS. To implement a policy,
the client specializes the given policy template for his type
and implements the behaviour described in the policy class' API
documentation.

Policies fall into two general categories: option policies and
functional policies.  Option policies typically define only a single
constant value (a boolean or integer), and that constant is used in
enabling or disabling certain aspects of functional policies.
Functional policies each have a very specific role in the class
binding process, and typically provide one function (sometimes two)
for performing their role.

Most policies can use defaults, but three policies in particular cannot
be defaulted (explained below).

All examples in this page, unless noted otherwise, assume that the code
is in the `v8::juice::cw` namespace (where ClassWrap lives).

## API Documentation ##

ClassWrap has fairly extensive API documentation
[in ClassWrap.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/ClassWrap.h).


# Options Policies #

Options policies hold constant values which are used as toggles or ranges
for various functional policies.


## `InternalFields` Option ##

The `InternalFields` policy specifies two values: the number of "internal fields"
(the v8 js-to-native mechanism) and the index of the internal field which holds
the native value.

The policy's interface looks like this:
```
template <typename T>
struct InternalFields {
    static const int Count = 1;
    static const int NativeIndex = 0;
};
```

The default implementation (Count=1, NativeIndex=0) is suitable for all
but the wierdest class-binding cases. When specializing it, `NativeIndex`
must be smaller than `Count`, and neither may be negative. Violating this
constraint will trigger a compile-time assertion.

**Important:** when one bound class inherits another, it _must_ use the same
values as the parent type's policy. This can be accomplished by subclassing
the child specialization from the parent specialization. e.g.:

```
template <>
  struct InternalFields<Subclass> : InternalFields<BaseClass> {};
```

## `AllowCtorWithoutNew` Option ##

This policy is a toggle for the internal (to ClassWrap) native object
construction process.

This policy looks like:

```
template <typename T>
struct AllowCtorWithoutNew
{
    static const bool Value = false;
};
```

And it specifies whether the following JS code is equivalent:

```
var a = new MyType();
var b = MyType(); // ctor called without "new"
```

By default the second form will trigger an error from v8. If this toggle is
true then ClassWrap internally treats the second form as the first form
by making the call to "new" itself.

The library does not support providing two different behaviours
depending on whether "new" is used or not.

## `ToNative_SearchPrototypesForNative` Option ##

This boolean policy is used by some of the `ToNative` policies (described
somewhere below) to specify whether or not they should traverse the
JS prototype chain when looking for the native object.

This policy looks like:

```
template <typename T>
struct ToNative_SearchPrototypesForNative
{
        static const bool Value = true;
};
```

What this means is:

If a JS-defined class inherits from a bound native class, the bindings
code can no longer find the native "this" object in that JS object
because that JS object's internal structure was not set up by the binding
layer.

If this toggle is off, the `ToNative` implementations which use this
toggle will only look in the first object of the prototype chain
(itself, in this case an instance of a class defined in script
code). If this toggle is on then some `ToNative` implementations will
start recursively walking up the prototype chain to look for the
native "this" object.

If a bound class will never need to be subclassed (either from JS or
another bound class) then this policy can be set to false. If
inheritance will ever play a role then it should be set to true. Not
doing so will eventually trigger JS-side exceptions when bound
functions are called and the associated native "this" object cannot be
found.


# Functional Policies #

Functional policies are those which add or change behaviours of the
binding processes. The list of functional policies, and very brief
descriptions of each:

  * `ClassName` defines the JS-side name of the classs.
  * `Factory` is responsible for creating and destroying natives.
  * `Extract` is responsible for extracting (`void*`) data from bound JS objects.
  * `ToNative` converts (JS Object)-to-(T). Basically a thin wrapper around the `Extract` policy.
  * `ToJS` converts (`T*`)-to-(JS Object). This is not possible for the generic case, and there is no default implementation! (But it is not always needed!)
  * `WeakWrap` may be used to customize the binding/unbinding process, e.g. doing/undoing custom native-to-JS mappings.

All policies except `ToJS`, `ClassName`, and `Factory` have reasonable defaults
which should work for the many common cases. The `Factory` and `ClassName` policies are
necessarily type-specific, but we have class templates which can
generate a Factory-compliant policy implementation from a list of
native constructors. The `ToJS` policy does not have a default
implementation because this cannot be done in the generic case (but
it is only needed if we need to pass (`T*`) _to_ JS space).

Each factory is described in more detail below.

## ClassName Policy ##

The ClassName policy defines a bound class's JS-side name. It looks like:

```
template <>
struct ClassName<BoundNative>
{
    static char const * Value() { return "BoundNative"; }
};
```

Alternately, we can call the following macro from the global scope:

```
JUICE_CLASSWRAP_CLASSNAME(BoundNative,"BoundNative");
```

which generates something equivalent to the above code.

There is no sensible default implementation, but some policy sets
provide this feature as part of their setup process.

## `Factory` Policy ##

The Factory policy's responsibilities are creating and destroying
new instances of objects. It is one of the few policies for which
there is no default implementation. There are several base Factory
types which we can use to install construction/destruction support.

This policy looks like:

```
template <typename T>
struct Factory
{
  typedef typename convert::TypeInfo<T>::Type Type;
  typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
  static NativeHandle Instantiate( Arguments const &  argv, std::ostream & exceptionText );
  static void Destruct( v8::Handle<v8::Object> jself, NativeHandle nself );
  static const size_t AllocatedMemoryCost = 0;
};
```


To simplify the class creation process, concrete implementations of
this interface are provided which use either the default constructor
or a list of overloaded constructors. See `Factory_NewDelete` and the
`Factory_CtorForwarder` family of classes for details.

To install one of the concrete policies, simply subclass the
`Factory<T>` specialization from the appropriate base
implementation. For example, the following creates a factory
implementation which creates native objects using `new T` and
destroys them using `delete t`:

```
template <>
    struct Factory<MyType> : Factory_NewDelete<MyType> {};
```

Note, however, that that requires a default constructor, and passes
no arguments to the ctor. For arguments which take arguments,
either implement a custom `Factory` or subclass `Factory_CtorForwarder`.

## `WeakWrap` Policy ##

After a Factory policy creates a native object, the class binder
passes the object via this interface to allow the policy to do
any custom JS/Native binding it needs to perform on each object.
As an example, policies which implement more intricate JS/Native
conversions may need to map the object as being of a certain native
type (or types, when inheritance is used).

The policy looks like this:

```
template <typename T>
struct WeakWrap
{
    typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;

    // Called soon after Factory<T>::Instantiate().
    static void Wrap( v8::Persistent<v8::Object> const & jsSelf, NativeHandle nativeSelf );

    // Called right before Factory<T>::Destruct().
    static void Unwrap( v8::Handle<v8::Object> const & jsSelf, NativeHandle nativeSelf );
};
```

The default implementation does nothing, which is suitable for most cases.

## `Extract` Policy ##

This policy serves two purposes:

  * Fishing (`void*`) handles out of JS objects.
  * Converting (`void*`) to (`T*`).

```
template <typename T>
struct Extract
{
    typedef typename convert::TypeInfo<T>::Type Type;
    typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
    static void * ExtractVoid( v8::Handle<v8::Object> const & jo, bool searchPrototypes );
    static NativeHandle VoidToNative( void * x );
};
```

The `ExtractVoid()` member is responsible for fishing out the (`void*`)s which v8
holds for us. This is the easy part, philosophically speaking, and this functionality
is provided by the class `Extract_Base<T>`. The base implementation is suitable for
most, if not all, cases.

The `VoidToNative()` routine is responsible for converting a (`void*`)
to a native object. How it does that is up to the policy. The default
implementation simply uses `static_cast<T*>(void*)`, which is (despite
appearances) remarkably safe for the majority of use cases (and also
incidentally avoids several pitfalls related to inheriting one bound
class from another).

Design note: in fact, `VoidToNative()` should arguably be part of the
`ToNative` interface. The the evolution of this API favoured it being
in this class, however, for reasons i don't quite recall.

## `ToNative` Policy ##

This policy is responsible for converting JS objects native objects.

In practice, this is simply a very thin wrapper around the `Extract`
policy, but concrete implementations exists which do other stuff, like
assist in certain lookups which are problematic when a JS class inherits
a bound native class.

This policy looks like:

```
template <typename T>
struct ToNative
{
    typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
    static NativeHandle Value( v8::Handle<v8::Object> const jo );
};
```

The default implementation should be suitable for most cases, but
clients should also look at the available concrete implementation for
more information.


## `ToJS` Policy ##

This policy is responsible for converting bound native objects to
bound JS objects.  This is not possible for the generic case, and
there is no default implementation! However, this operation is only
needed if we bind to functions (or members) which return a native object, e.g.
`Foo * somefunc()`. It is not needed if (`Foo*`) will be passed
as function arguments (that is handled by the `ToNative` policy).

This policy looks like:

```
template <typename T>
struct ToJS
{
public:
    typedef typename convert::TypeInfo<T>::Type Type;
    typedef typename convert::TypeInfo<T>::NativeHandle NativeHandle;
    static v8::Handle<v8::Object> Value( NativeHandle );
};
```

Currently only the
[TwoWay policy set](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/ClassWrap_TwoWay.h)
implements the internals needed to support this operation.

# Conversion of Bound Types to/from JS/C++ #

The `ToNative` and `ToJS` policies describe, _for the ClassWrap API_,
how to convert values to/from JS and C++ space. To plug these conversions
in to the [core v8-juice type conversions framework](ConvertingTypes.md) we must
implement two template specializations required by that framework:

```
namespace v8 { namespace juice { namespace convert {

template <> struct JSToNative<MyBoundType>
   : v8::juice::cw::JSToNativeImpl<MyBoundType>
{};

template <> struct NativeToJS<MyBoundType>
   : v8::juice::cw::NativeToJSImpl<MyBoundType>
{};

} } } // namespaces
```

That will route all `CastToJS<MyBoundType>()` and `CastFromJS<MyBoundType>()`
calls through the appropriate ClassWrap policy class to perform the conversion.

However: most policy sets will define `JSToNative<>`, and installing a custom
copy in addition to one installed by a policy set will produce duplicate definition
errors at compile time.

Some (currently only one) policy sets _might_ implement `NativeToJS<>'
as well, but the `ToJS` policy (the basis of the above specialization)
necessarily requires policy-specific infrastructure, and there appears to be no
way(?) to create a generic default implementation.

It is simple to create a `JSToNative` specialization for a given type. Simply
call the following from global-scope code:

```
// Call a macro:
JUICE_CLASSWRAP_JSTONATIVE(MyBoundType)

// Or the equivalent supermacro:
#define CLASSWRAP_BOUND_TYPE MyBoundType
#include <v8/juice/ClassWrap-JSToNative.h>
// ^^^^ undefines CLASSWRAP_BOUND_TYPE!
```


# Concrete Policy Implementations #

The library comes with enough parts to easily build your own policy
sets.  For our purposes, a "policy set" is a collection of
policies which work together to provide certain features above and
beyond the basics necessary for binding.

For the barest of features, the only two policies which are _required_
to be specialized by client code `ClassName` and `Factory`. The
library also comes with base classes and complete policy sets to make
it easy to plug in a bound class with very little client-written code.
Which policy set is used depends on the needs of the class.

The following sections briefly describe some of the options.

## Defaults ##

Most of the policies have fairly reasonable defaults, which makes it fairly
easy to get started with binding a class.

Not all policies have default implementations. Notably:

  * `ClassName` must be implemented. (See the `JUICE_CLASSWRAP_CLASSNAME()` macro.)
  * `Factory` must be implemented, but there are several useful base classes which do all the work (specializations must simply subclass them).
  * `ToJS` has no default but is never invoked unless some bound function/member/whatever must return (`T`) to JS space.

When starting work on a binding, it is simplest to use the defaults (except
for those listed above). One can switch policies later if it is determined
that certain features are necessary. For example, two-way conversions from/to
JS/Native are currently provided only by the TwoWay policy set (described below).
That said, JS-to-native conversions are only needed if we have to pass (`T`) to JS
space (e.g. a bound function returns (`T*`)).

Some properties of the default policy implementations:

  * They internally use `static_cast<T*>(void*)` for conversions. This is fast, cheap, and surprisingly safe (but not infallibly so).
  * As a happy accident, they avoid some of the inheritance-related problems which "deeper" (and type-safer) policy sets must internally account for.

## JuiceBind ##

This policy set uses the v8::juice::bind API to provide type-safe JS-to-Native conversions
(i.e., no casts are used), but requires a small amount of memory per bound
object to maintain the void-to-T mappings.

This policy set is very easy to install for a given bound class, as
documented in the header file:

  * [JuiceBind](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/ClassWrap_JuiceBind.h)

## TwoWay ##

This policy set provides 100% type-safe (with _no_ casts) JS-to-Native
_and_ Native-to-JS conversions, but requires more memory per binding
than the other policies. However, it is also the only one which
provides a working `ToJS` policy, so it will be the policy of choice
when we must convert from (`T*`) to JS (e.g. a bound function returns
(`T*`)).

This policy set is very easy to install for a given bound class, as
documented in the header file:

  * [TwoWay](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/ClassWrap_TwoWay.h)


# Utility Functions and Types #

TODO: describe the more useful ones here.

# Example of using ClassWrap #

The complete test/demo code for ClassWrap can be found here:

  * [BoundNative.cpp](http://code.google.com/p/v8-juice/source/browse/trunk/src/client/sample/BoundNative/BoundNative.cpp)

That test code is set up to be conditionally compiled against one of
three policy sets, to demonstrate how one might go about customizing the
wrapping process.