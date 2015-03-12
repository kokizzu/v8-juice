

# ClassWrap #

ClassWrap is a `JSClassCreator` subclass which makes use of
v8-juice's type conversions API to add features to the class binding
process.

**Users of the [V8Convert](V8Convert.md) API** should see [ClassCreator.hpp](http://code.google.com/p/v8-juice/source/browse/convert/include/v8/convert/ClassCreator.hpp) instead, which is based on [ClassWrap](ClassWrap.md) API but isn't _quite_ as configurable.

ClassWrap is an alternate API to [ClassBinder](ClassBinder.md) for binding native
classes to JS. Where [ClassBinder](ClassBinder.md) is "monolithic", ClassWrap is
policy-based. That is, it implements only the most basic binding options, leaving the real work
to various policy classes. Policy classes can be specialized for a given
type to provide additional features, and sensible defaults are installed
whenever possible.

These docs assume that the reader is familiar with the
[v8::juice type conversions API](ConvertingTypes.md), and those docs
may be helpful in understanding these docs.

The source code and API documentation for this class is in:

  * [ClassWrap.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/ClassWrap.h)

Some related materials:

  * [ClassWrap\_JuiceBind.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/ClassWrap_JuiceBind.h)
  * [ClassWrap\_TwoWay.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/ClassWrap_TwoWay.h)


# An Example Class #

For the demonstrations below we will use the following class:

```
struct BoundNative
{
public:
    static bool enableDebug;
    double publicProperty;
    BoundNative();
    explicit BoundNative( int i, double d = 42.24 );
    virtual ~BoundNative();
    bool ptr( BoundNative const * b );
    BoundNative * getPtr();
    static size_t InstanceCount();
    int getInt() const;
    void setInt( int i );
    bool overload() const;
    int overload(int i);
    double overload(int i, double d);
    virtual std::string toString() const;
};

```

All of those members are bindable by this framework ("bind" = to tie this class to JavaScript). Now let's see how to bind various aspects of `BoundNative`
into JS...

# Setting up the Basic Policies #

Part of binding our class using this framework involves setting
up various so-called "policy" classes. We won't go into detail
here about what they are and which ones exist. Instead we will
set up only the policies we need.

All of the policy classes must be specialized within the
`v8::juice::cw` namespace, but for brevity that namespace will
be left out of examples. The example code also assumes that
`using namespace v8::juice` is in effect.

For much more information about ClassWrap policies, see
the ClassWrapPolicies page and the API documentation.

## Factory Policy ##

The Factory policy's responsibilities are creating and destroying
new instances of objects. It is one of the few policies for which
there is no default implementation. There are several base Factory
types which we can use to install construction/destruction support,
and we will use one of those bases here.

`BoundNative` has several constructors. Which one shall we
bind to JS? Don't bother trying to decide - we'll bind all of them.

```
template <>
struct Factory<BoundNative>
    : Factory_CtorForwarder<BoundNative,
        tmp::TypeList<
            convert::CtorForwarder0<BoundNative>,
            convert::CtorForwarder1<BoundNative,int>,
            convert::CtorForwarder2<BoundNative,int,double>
        >
      >
{};
```

Notice that we bound the two-arg ctor twice. Because it has a default
value for the second argument, we can bind it as a one- and/or two-arg
constructor.

If we wanted to customize the construction process we would specialize
`Factory<BoundNative>` and implement the `cw::Factory` policy's
interface (two functions and one constant).

## Class Name Policy ##

The ClassName policy defines a bound class' JS-side name. It looks like:

```
template <>
struct ClassName<BoundNative>
{
    static char const * Value()
    {
        return "BoundNative";
    }
};
```

Alternately, we can call the following macro from the global scope:

```
JUICE_CLASSWRAP_CLASSNAME(BoundNative,"BoundNative");
```

which generates something like the above code.

Some policy sets provide this feature as part of their setup.

# Initializing ClassWrap #

We need to initialize our class bindings somewhere. Conventionally
that looks something like this:

```
v8::Handle<v8::Object> SetupBoundNativeClass( v8::Handle<v8::Object> dest )
{
    typedef ClassWrap<BoundNative> CW;
    CW & b( CW::Instance() );

   // ... binding code goes here (see later examples) ...

   b.Seal(); // ends the binding process
   b.AddClassTo( dest ); // installs BoundNative class in dest
   return b.CtorTemplate()->GetFunction();
}
```

The exact signature and return value of that function need to be identical to the one shown here. The important thing is that the function have an Object argument which represents the logical global object for the bound class. (It need not be the real global object, though it often is.)

# Binding Functions #

We can bind both `BoundNative` member functions and free functions
to the JS class using the power of C++ templates.

From our `SetupBoundNativeClass()` routine:

```
typedef convert::MemFuncInvocationCallbackCreator<BoundNative>
        ICM; // typing-saver
b.Set( "toString",
        ICM::M0::Invocable<std::string,&BoundNative::toString> );
b.Set( "setInt",
        ICM::M1::Invocable<void,int,&BoundNative::setInt> );
b.Set( "getInt",
        ICM::M0::Invocable<int,&BoundNative::getInt> );

```

The `ICM` class is a utility class which saves a few bytes of typing.
`ICM::M0` to `ICM::M9` are
convenience typedefs for assisting in binding member (thus the M)
functions taking the given number of arguments.

We can also bind non-member native functions as JS members using
a similar approach:

```
typedef convert::InvocationCallbackCreator ICC; // typing-saver
b.Set( "sleep",
       ICC::F1::Invocable<unsigned int,unsigned int,::sleep> );
```

These templates _create_ functions which implement the `v8::InvocableCallback`
interface. The generated functions use the [type conversion API](ConvertingTypes.md)
to convert their arguments from their native counterparts and convert
the return value to a native value (or JS `undefined` for `void` return types).

# Binding Member Variables #

We can bind native member variables to JS space, such that all
get/set access is directed through the native variables. Alternately,
we can bind JS properties to native getter/setter functions, such that
all JS access to the property goes through those native accessors.

The binding process looks like:

```
// Bind BoundNative.myInt to BoundNative::getInt() and BoundNative::setInt():
b.BindGetterSetter<int,
                   &BoundNative::getInt,
                   void,int,&BoundNative::setInt>
                   ( "myInt");

// Bind BoundNative.intGetter read-only to BoundNative::getInt()
b.BindGetter<int,&BoundNative::getInt>( "intGetter" );

// Bind BoundNative.publicProperty to BoundNative::publicProperty:
b.BindMemVar<double,
             &BoundNative::publicProperty>
             ( "publicProperty" );

// Bind BoundNative.publicPropertyRO read-only to BoundNative::publicProperty:
b.BindMemVarRO<double,
               &BoundNative::publicProperty>
               ( "publicPropertyRO" );
```


We can also bind JS properties to static variables, using `b.BindStaticVar()`:

```
// Read-write access:
b.BindStaticVar<bool,&BoundNative::enableDebug>("enableDebug");
// Read-only access:
b.BindStaticVarRO<bool,&BoundNative::enableDebug>("enableDebug");
```

Caveat: on the JS side such statics are only available via objects, and not the constructor. e.g.:

```
BoundNative.prototype.enableDebug; // === a boolean value
BoundNative.enableDebug; // === undefined
```

As far as i can determine, the v8 API doesn't allow us to bind these at the class level, only the object level.

# Binding a Destruction Function #

It is often useful to provide a mechanism which allows a JS-side client to destroy (or close) an object. For example, when binding i/o streams or database-related objects, we often need to explicitly close/destroy (sometimes in the proper order) them to ensure proper cleanup. Remember that v8 does not guaranty that destructors will _ever_ be called. Binding our own destruction/cleanup functions gives us the level of control we need for many bound native types.

For any class wrapped by the ClassWrap API, we can simply do the following:

```
b.Set( "destroy", CW::DestroyObject );
```

That will trigger the destruction of the _native_ object using the bound destruction function. After the destruction/close function is called, subsequent JS-side use of the object's members will cause an exception to be thrown with a message similar to, "could not find native 'this' object." Thus, after the destruction routine is called, the object should not be used any more. Though the native object is destroyed immediately, the now-empty JS object must be cleaned up by v8, and is left to the whims of the garbage collector. (v8 provides us with no API to force the immediate destruction of the JS object, as far as i'm aware.)

# Creating New Instances #

Once we've bound a type using ClassWrap, we can create new JS-side instances of it from native code like so:

```
namespace cv = v8::juice::convert;

// Constructor arguments:
v8::Handle<v8::Value> argv[] =
    {
    cv::CastToJS( 17 ),
    cv::CastToJS( 42.24 )
    };

typedef v8::juice::cw::ClassWrap<BoundNative> CW;
// Instantiate new instance via JS:
v8::Handle<v8::Object> jobj =
  CW::Instance().NewInstance( sizeof(argv)/sizeof(argv[0]),argv);
// Get a handle to the native, which is owned by v8:
BoundNative * my = cv::CastFromJS<BoundNative>( jobj );
if( ! my ) {
    ... construction failed and possibly threw a JS-side exception ...
}
```


# Caveats and Gotchas #


## Inheritance doesn't come for free ##

Crossing the JS/C++ lines poses several problems for bound classes.

The more glaring ones are:

  * JS-side subclasses complicate the lookup of the native `this` pointer which is stored internally in the JS object.
  * When one bound type inherits another, inherited bound native functions called from the subclass may not be able to find their own `this` pointer because the subtype is not exactly the same type as the parent.


The good news is that there are ways around these problems, and the default ClassWrap
policies try to make these problems go away to the best of their ability.

TODO: document this when you're less tired.

## Achtung: Beware of Copies! ##

Consider the following members:

```
typedef std::list<std::string> StringList;
StringList MyClass::getList() const;
void MyClass::setList( StringList const & );
```

These can be bound to a member, such that getting the member via JS
would return an Array of String object, and setting it would convert
the value to an Array of Strings. That's all fine and good, but
be very aware that that particular conversion _requires copying list_.
The two main implications are that (1) such access may not be cheap,
depending on the conversion necessary for the getter/setter,
and (2) the following _will not work in an intuitive manner_:

```
var ar = ["hi", "world" ];
myobj.list = ar; // COPIES ar!
ar.push("!"); // does NOT modify myobj.list
// myobj.list.length == 2
// ar.length == 3
```

Thus it is not generically possible to update JS references
via this approach.

That said, the following is kosher _if_ a specialization
of the ClassWrap `ToJS<>` policy is in place to convert
from natives to JS. (Such support requires extra tooling in the
policy classes, and the `ClassWrap_TwoWay.h` policy set
provides this.)

```
MyClass * MyClass::getBuddy();
void MyClass::setBuddy( MyClass * );
...
b.BindGetterSetter<MyClass *, &MyClass::getBuddy, // <-getter
                   void, MyClass *,&MyClass::setBuddy // <-setter
                   >( "buddy" );

// JS:
var o1 = new MyClass();
o1.buddy = new MyClass(); // calls MyClass::setBuddy(new object)
...
print( o1.buddy ) // will act on that same MyClass instance.
```

Getting/setting such a property via these bindings will work
as expected as long as the property refers to another
bound `MyClass` object.

_HOWEVER_, there is another caveat: in the above specific case, we "should"
give JS an explicit handle to the newly-created object or we risk it getting
garbage collected because the native member conversions do not store a
reference to the new object in JS space (only in native space). Thus it is
possible, in the above example, that calling `o1.buddy` in a "get" context
would return null or an empty object if GC is called between the assignment
and the call to `print()`.

The general workaround for this problem is to stuff a reference to the
new object somewhere:

```
o1._refs = [];
o1.buddy = (o1._refs[o1._refs.length] = new MyClass());
...
print( o1.buddy ); // this is now safe
```

That is somewhat lame, but it is a generic problem for many types of
JS-to-Native bindings. (When i first encountered it, it took me an hour
to figure out why my new objects kept disappearing.)


## Return values of native setter members ##

Be aware that the return value of a native setter function is
_ignored_ for binding purposes because that's how the JS API works in
regards to assignment.  This example should clarify:

```
myobj.prop = 12; // MyClass::setProp(12)
print( myobj.prop += 3 ); // is essentially (but not exactly) equivalent to:
// print(
//      ( MyClass::setProp( MyClass::getProp() + 3 ),
//        MyClass::getProp() )
// );
// (Recall that (a,b,c...) evaluates to the last expression in the list)
```

In actuality `getProp()` is not called a second time in the `print()`
expression (though i'm honestly not entirely sure why not), but the
effect is essentially as if it is called (assuming the getter has no
side effects which would affect the second call).

Thus the assignment operation returns the JS-internal value of the
assignment, and _not_ the return value of `MyClass::setProp()`. This
is a property (feature or limitation?) of the v8 property accessor
binding API.

# Examples of using ClassWrap #

Here are links to several class bindings which use ClassWrap:

  * [BoundNative.cpp](http://code.google.com/p/v8-juice/source/browse/trunk/src/client/sample/BoundNative/BoundNative.cpp) contains the generic test/demo code for ClassWrap.
  * [PosixFILE.cpp](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/PosixFILE/PosixFILE.cpp) binds the C-standard `FILE` API.
  * [ExpatParser](http://code.google.com/p/v8-juice/source/browse/extra-plugins/src/expat): the sources for the [ExpatParser plugin](PluginExpatParser.md).
  * [PathFinder-cw.cc](http://code.google.com/p/v8-juice/source/browse/trunk/src/lib/juice/PathFinder-cw.cc) implements the [PathFinderClass](PathFinderClass.md) bindings.