

**Achtung:** This page is part two of the [ClassBinder documentation](ClassBinder.md). If you are not familiar with that material, this page will probably make little sense.


# Object Construction with or without `new` #

Consider this code:

```
var x = new Foo();
var y = Foo();
```

Notice how the `Foo()` constructor is called two different ways. JavaScript differentiates
between the two, and native implementations must normally "do some special stuff" if they
want the second form to work as one might expect it to.

Versions of v8-juice newer than 20090331 have a toggle to allows class binders to specify
whether the second syntax works like the first or not. By default this option is turned off,
but it can be enabled with:

```
WeakJSClassCreator<T>::AllowCtorWithoutNew(true);
```

`WeakJSClassCreator<T>` will, if that toggle is activated, force the second syntax
to behave exactly like the first one. `WeakJSClassCreator` does not currently support
different behaviours for both syntaxes, such that `Foo()` and `new Foo()` would both
be legal but have different symantics.


# Dual JS/C++ Inheritance #

As of 20090324 WeakJSClassCreator supports dual JS/C++
inheritance. What does that mean?  It means that if we have native
type called `ParentType` and a subclass called `SubType`, and we bind
both of them with WeakJSClassCreator, then the JS-side `SubType`
objects can access their _inherited_ bound member functions as if they
were a native object of type `ParentType`.  That might sound a bit
confusing, but an example might clear it up:

```
class ParentType
{
  ...
  virtual int vfunc();
};
class SubType : public ParentType
{
...
};

... add WeakJSClassCreatorOps<ParentType>
    and WeakJSClassCreatorOps<SubType> ...

// And we can then do:
// Bind parent type:
typedef ClassBinder<ParentType> WP;
WP wp( targetObject );
wp.BindMemFunc< int, &ParentType::vfunc >("vfunc")
  .Seal();

// Bind subclass:
typedef ClassBinder<SubType> WS;
WS ws( targetObject );
ws.Inherit( wp )
  .Seal();
```

This support is actually a feature of `WeakJSClassCreator<>`, the
parent class of `ClassBinder`, but since `WeakJSClassCreator<>`
has no direct support for binding member functions (because it is
intentionally independent of the v8-juice core and therefore has no access to the
type conversions framework), this feature is only really accessible
via extensions or subclasses like `ClassBinder`.

In C++ and JS, we take it for granted that calling
`aSubTypeObject.vfunc()` would call the inherited virtual
function. However, when crossing JS/C++ boundaries, that feature does
not come for free. Since we use C++ templates to create the bindings
for classes, any conversions must know the _exact_ templatized type of
the native class. Because of this, `WeakJSClassCreator<ParentType>`
and `WeakJSClassCreator<SubType>` are, for purposes of typing,
unrelated, and thus have no inherent implicit conversion ability.

When `WeakJSClassCreator<SubType>::Inherit()` is called, an internal
lookup proxy is added to `WeakJSClassCreator<ParentType>`, such
that calls to `WeakJSClassCreator<ParentType>::GetSelf()` (and
friends) will act "virtually". That is, if they cannot determine that
the JS object being converted is-exactly-a `ParentType` object, they
will use the internal lookup proxy to see if the object is an
instance of some other subclass of `ParentType`. Using this approach,
the member functions bound to `ParentType` can get a pointer to their
`this` object, even though the objects weren't created by
`WeakJSClassCreator<ParentType>`.

It is a _compile-time_ error to try to inherit either a base type for
which `WeakJSClassCreator<>` is not implemented or which is not a C++
base type of the wrapped type.

Any number of types may `Inherit()` a given base this way, and when
looking for a conversion they will be searched in an unspecified order
(but in theory only one can possibly match).

This approach has some limitations, the most glaring being that when we have more than
one level of inheritance, we must call `Inherit()` for each level of the hierarchy,
which is a real drag. i'm looking into other approaches which won't require this.

To inherit multiple levels deeply, we currently need to do the following:

```
struct TypeA
{...};
struct TypeB : TypeA
{...};
struct TypeC : TypeB
{...};

// When we bind TypeC:
typedef ClassBinder<TypeC> CB;
CB b( targetObject );
b.Inherit( binderForTypeB ); // sets up inheritance of TypeB
b.InheritNative<TypeA>(); // sets up inhertiance of TypeA
...
b.Seal();

// And we need to include the WeakJSClassCreator-CastOps.h
// supermacro, as shown above.
```

With that in place, JS-side `TypeC` objects will be able to convert their
native object to a `TypeB` or `TypeA` object, as needed, via the standard
type conversions mechanism.

## JS/C++ cast operations in the face of inheritance ##

If we register the "standard" casting operators for a wrapped type using:

```
// must be in global-scope code
#define WEAK_CLASS_TYPE ParentType
#include <v8/juice/WeakJSClassCreator-CastOps.h>
#define WEAK_CLASS_TYPE SubType
#include <v8/juice/WeakJSClassCreator-CastOps.h>
```

(or equivalent)

then `CastToJS<ParentType>()` and `CastFromJS<ParentType>()` will use
`WeakJSClassCreator<ParentType>` for the conversions, and thus provide
the same "virtual" conversions to `SubType` objects. `SubType` objects
may then participate in type conversions which would normally apply
to `ParentType` objects (for this to work, each base has to be
`Inherit()`ed separately by its subclasses). This applies, for example, to the type conversions applied automatically to function arguments (as well as for the
`this` object) for bound member function and bound non-member functions.

# Inheriting Native Classes from JS #

As of 20090331, we've got a way for JS classes to subclass a bound native class. This requires a couple
of steps.

First, when we bind the native class we must call:

```
WeakJSClassCreator<MyNative>::SearchPrototypesForNative(true);
```

This will tell the binding engine that if it cannot find the native
`this` object for a given JS `this` object, then it should search the
JS prototype object chain (recursively). This option is off by default because
it is a performance hit for each call to `GetSelf()` and friends, and is only
necessary if a class needs to be inherited by script-side classes.

Secondly, in JS code we need to follow this pattern:

```
function MyClass()
{
    this.prototype = this.__proto__ = new MyNative(...);
    ...
    return this;
}
```

Setting the "magic" `__proto__` property is v8's way of forcing `this`
to be of a specific class.

The weird `this.prototype = this.__proto__ = ...` is the only way i've found
to ensure that calling base implementations of overridden functions can be
done in a conventional manner (see below).

Lastly, use `MyClass` as if it is-a `MyNative`:

```
var my = new MyClass();
print( my instanceof MyNative ); // "true"
```

However, there are some weird limitations to this approach. The most
notable is that the `prototype` object is not 100% of the correct
type. When reimplementing member functions, we _have_ to do add
them to _each_ new instance, as opposed to adding them to `MyClass.prototype`
as we would normally do. The reason is because we force-set the prototype
in the constructor, which causes the prototype to be different in different
contexts. e.g `this.prototype` will _not_ be the same type as `MyClass.prototype`!

So, to implement new member, we have to add them in the constructor
function (or after calling the ctor) _and_ we have to use an ugly hack
to call the inherited members. It looks like this:

```
    // inside of the MyClass ctor:
    this.toString = function() {
        return '[MyPanel:'+this.__proto__.toString()+']';
    /**
    Actually, this.prototype will work here, but only
    because of the explicit assignment demonstrated above.
    this.prototype will not work, for this purpose, if
    this member is defined outside of the ctor, even if
    we use this.__proto__ instead, because the ctor actually
    replaces the declared prototype with another one, which
    means it never sees and functions set in MyClass.prototype.
    */
    };
```

This is an unfortunate limitation, and has implications other than
just adding/calling member functions. i know of no workaround to 100%
transparently work around this :(.


## A slightly different, but even less correct, approach ##

The following approach will _almost_ do the same thing as above, but
has the tragic flaw that all JS instances of the class share the exact
same native instance:

```
function MyClass()
{
    ...
    return this;
}
MyClass.prototype = new MyNative(...);

// And now...
var my = new MyClass();
my.nativeString = "foo";
var my2 = new MyClass();
my2.nativeString = "bar"
print( my.nativeString, my2.nativeString ); // "bar bar" :(
```


# Optimizations #

## Shallow Binding ##

As of 20091018, `WeakJSClassCreator` supports a feature called "shallow binding". When this is enabled, it uses a much faster mechanism for converting from JS-to-Native `(T*)`, _but it also loses several features_ provided by the normal bindings:

  * It uses `static_cast<T*>(void*)` and is not strictly as type-safe as the normal mechanism. That said, in theory the pointers we cast _must_ be of the correct type (and _will_ be unless client code muddles with the binding data).
  * If the bound class inherits other bound classes, or will be inherited by bound classes or JS-side classes, not all features of inheritance will work. Notably (but not the only problem), inherited bound functions may not be visible in subclasses.
  * `NativeToJS()` cannot work for that type, as the native has no notion of a JS counterpart. This will prohibit conversion of native (`T*`) to their JS counterparts, e.g. for bound function arguments or return values.
  * Certain binding routines will fail with a compile-time error because they cannot function in shallow mode.
  * Certain binding routines may trigger a JS or native exception at runtime because they cannot function in shallow mode (but cannot make that determination at compile-time, for unfortunate internal reasons).

Despite the daunting list of limitations, if the a bound class meets the following guidelines then this can be a useful (and "almost certainly safe") optimization to turn on:

  * It does not participate in any JS-related inheritance (inheritance of non-bound natives is okay).
  * It binds no native member functions which return `(T [const] *)` (as those conversions cannot work).
  * It binds no JS member properties to native `(T [const] *)`, as the conversion will get lost in some contexts.

To enable this optimization add the following code somewhere _before `WeakJSClassCreator<T>` is instantiated_:

```
namespace v8 { namespace juice {

template <>
struct WeakJSClassCreator_Opt_ShallowBind< T >
  : WeakJSClassCreator_Opt_Bool<true> {};

} } /* namespaces */
```

To disable it, remove that code or change the `true` value to `false`.