This is an extension of the [page about creating JS classes](CreatingJSClasses.md). Please read that page before reading this one. The page for the [v8::juice type conversions API](ConvertingTypes.md) might also be helpful.

The ClassBinder API is now (as of late 2009) considered obsolete, and newer code should prefer the ClassWrap API. This API will stay around because i have a significant amount of plugins code which uses it, but no further features or significant changes are planned for this API.



# ClassBinder #

ClassBinder is a [WeakJSClassCreator](CreatingJSClasses.md) subclass which makes use of
v8-juice's type conversions API to add features to the class binding process.
Unlike JSClassCreator and WeakJSClassCreator, ClassBinder is not a
standalone implementation which can be used as-is in arbitrary v8 client
code - it requires [the v8-juice type conversions framework](ConvertingTypes.md) in order to convert between native and JS types.

The source code and API documentation for this class is in
[ClassBinder.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/ClassBinder.h).

An interesting property of this class binding framework is that it's compile-time typesafe. Though v8 uses `void *` as the parameter type for destruction callbacks, the implementation uses a mix of class templates, private static functions, and an internal void-to-native mapping to ensure that only the correct types can, e.g., be passed to the correct destructor, and that `CastFromJS<TypeB>()` cannot cast (using the default conversions) to `TypeA`. The implementation is believed to be 100% typesafe.


# Binding member functions #

The primary feature added by ClassBinder is the ability to bind native
member functions and variables of the wrapped type. The syntax for doing so is a tiny
bit odd, but there's a reason for it which we'll explain after we demonstrate:

```
int MyClass::func( double ); // function we want to wrap


ClassBinder<MyClass> binder;
binder.AddMemFunc< int /* return value type */,
		double /* first arg type*/,
		&MyClass::func /* member func */
	>( "myFunc" );
```

_WTF?_ Every function-wrapping framework i've seen so far passes
function pointers as function arguments to the function templates (got
that?). In this one, we pass the function pointer as a _template
parameter_ instead of as a function argument! Why? It's ugly, but
there are reasons for it.

First, it is compile-time typesafe and this allows us to set up the
callbacks such that it is impossible to mis-cast a (`void*`) pointer
to a (`T*`) (in fact, no casts are used at all for this, even though
v8 internally gives us only (`void*`) handles).

Secondly, it costs us nothing at runtime. If we passed
pointers to functions as the arguments, we would have to create a
small wrapper object for _each_ binding (that is, one per class/member
combination), and the wrapper's only purpose would be to forward the
call to a specific native object (which is found at runtime of
course).  We would have no logical place to store the call-wrappers
for later cleanup (long story) and we would effectively introduce a
memory leak for every bound function.

By using function pointers as template parameters, we bind the
function pointers at compile time and therefor need no intermediary
call-proxy object instance. We _still_ have plenty of internal proxy
functions, but they require no dynamic memory and are bound (and
possibly inlined) at compile-time.

The primary down-side to this approach, as opposed to passing function
pointers as _function_ arguments (not _template_ arguments), is that the
function pointer signature's contained types must exactly match the
types specified in the other template parameters. e.g. the following
will not work:

```
int MyClass::func( double );

...

binder.AddMemFunc< int, // return value type
		int /* first arg type. Must be of type double because: */,
		&MyClass::func /* int (MyClass::*)(double) */
		>
		("func");
```

That won't work because the template type for the first member function
argument is wrong - it must the type used by the signature of the given
member function (so in this case it must be `double` instead of `int`).

If i'm not mistaken, if we bound at runtime to function pointers (via an
internal functor used as a call wrapper), we could rely on implicity
type conversions to do this sort of type conversions for us. That would,
however, require the above-mentioned leak, which i'm not at all keen on.

The conversion of member function parameters to/from JS is achieved
using the `v8::juice::convert` API, and this code inherits most of the type
conversion limitations (and features) of that API. As an exception, it supports
member functions returning `void` (which the `v8::juice::convert` API cannot
directly handle).

# Binding member variables #

Again, the syntax is ugly, but not as ugly as that for binding functions. If we have a type with a `std::string` member named `str`, we can bind that member with:

```
binder.Set( "str",
     MemberVarGetter<MY,std::string, &MY::str>,
     MemberVarSetter<MY,std::string, &MY::str> );
```

Or, more succinctly:

```
binder.BindMemVar<std::string,&MY::str>( "str" );
```

which is basically a shorthand form of the above (but also uses different default access options for the generated function!).

The `MemberVarSetter<>()` and `MemberVarGetter<>()` templates create functions which set/get the native member using [CastFromJS() and CastToJS()](ConvertingTypes.md). This means that we can also bind members which are pointers to other wrapped types, and access those transparently via JS. e.g. if the MY type has a member `MY * other`, we can bind that:

```
binder.BindMemVar<MY *, &MY::other>( "other" );
```

Though whether or not such an automatic conversion makes sense for your case is another question entirely (e.g. it might be possible to leak objects or cause other problems if client code doesn't manage such pointers in a manner which is kosher in your API).

We can of course choose to apply only the getter or the setter if we like, to allow conversions in only one direction.

# Binding JS member properties to native get/set methods #

(Feature added 20091015.)

It is also possible to bind JS member properties to native accessor/mutator functions (i.e. "get" and "set" functions),
such that JS-side access to the properties will be marshalled via the native members. e.g.:

```
// Accessor:
int MyClass::getProp() const;
// Mutator:
void MyClass::setProp(int);

// Binding code:
binder.BindPropToAccessors< int, &MyClass::getProp, void, int, &MyClass::setProp>( "prop" );

// JS-side:
myobj.prop = 17; // calls MyClass::setProp(17)
print( myobj.prop ); // calls MyClass::getProp()
```

However, one must be careful to pass the propert type to the getter, or else the automatic
converion from JS to C++ may contain an incorrect or invalid value. e.g.:

```
myobj.prop = "17";
// calls MyClass::setProp(0) because of the String-to-Integer conversion
```

It is also possible to bind only a getter:

```
binder.BindPropToGetter< int, &MyClass::getProp >( "prop" );
```

However, it appears to be impossible to properly bind only a setter,
as v8 will crash on read-access to the bound member if we do that.
(This could theoretically be corrected by splitting `v8::ObjectTemplate::SetAccessor()`
into two functions, one each for getter and setter.) In this case,
the setter would use the default setter behaviour but access to the getter
would use the native method, which is likely to return a different value
than is set via JS. For example, assuming that the native value
proxied by `MyClass::getProp()` is 19:

```
myobj.prop = 27; // uses JS-internal property storage
print( myobj.prop ); // calls MyClass::getProp(), returning 19!
```

In effect, this makes the property read-only from the native side, as
setting it will have no effect unless the native accessor is specifically
coded to look up the JS-assigned value of the property (which kind of
defeats the purpose of the to-native binding).

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
and (2) the following _will not work in an intuive manner_:

```
var ar = ["hi", "world" ];
myobj.list = ar; // COPIES ar!
ar.push("!"); // does NOT modify myobj.list
// myobj.list.length == 2
// ar.length == 3
```

Thus it is not generically possible to update JS references
via this approach.

That said, the following is kosher:

```
MyClass * MyClass::getBuddy();
void MyClass::setBuddy( MyClass * );
...
binder.BindPropToAccessors< MyClass *, &MyClass::getBuddy, void, MyClass *, &MyClass::setBuddy >( "buddy" );

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
garbage collected because the native member conversions to not store a
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
_ignored_ for binding purposes because that's how the v8 API works in
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


# Example of using ClassBinder #

Here's an example of how `ClassBinder` can be used.

## A native type to bind ##

First we need a native type. Oh, look, here's one:

```
struct my_native
{
    int func1() { return 42; }
    int func2(int x) { return x*2; }
    double func3(int x,int y) { return x*y; }
    std::string hi() { return "hi!"; }
    my_native * me() { return this; }
    bool him(my_native * him) { return true; }
    void avoid() {}
    void avoid1(int x ) {}
    void avoid2(int x,double d ) {}
    double takes3(int x, int y, int z) { return x * y * z; }
    std::string str;
    my_native * other;
private:
    int proxied;
public:
    my_native() : str(), other(0), proxied(19)
    {}
    int propGetter() const
    {
        return this->proxied;
    }
    void propSetter(int v)
    {
        this->proxied = v;
        return;
    }
};
```

Great, now that's out of the way...

## WeakJSClassCreatorOps specialization ##

We need a `WeakJSClassCreatorOps<my_native>` specialization which defines the
common binding operations:

```
namespace v8 { namespace juice {
    template <>
    struct WeakJSClassCreatorOps<my_native>
    {
    public:
	enum { ExtraInternalFieldCount = 0 };
	typedef my_native WrappedType;
	static char const * ClassName() { return "MyNative"; }
	static WrappedType * Ctor( Arguments const & argv,
				   std::string & exceptionText)
	{
            return new my_native;
            // If we couldn't create from the given arguments
            // we would populate exceptionText with a description.
            // e.g. "Constructor requires two integer arguments!"
	}

	static void Dtor( WrappedType * obj )
	{
	    if( obj ) delete obj;
	}
    };
}} // namespaces
```

That example is feature-complete and ready for copy/paste into your source files.

## Set up automatic JS/C++ conversions ##

The following "supermacro" hooks `my_native` in with the `v8::juice::convert`
API, so that we can use `CastToJS()` and `CastFromJS()` with
`my_native`. We want that in this because some of the `my_native`
members take or return pointers to `my_native` objects. Since the
function forwarding mechanism uses `CastToJS()` and `CastFromJS()` for
all type conversions, we want to be able to hook in to that so that `my_native`
objects can also be converted generically:

```
#define WEAK_CLASS_TYPE my_native
#include <v8/juice/WeakJSClassCreator-CastOps.h>
```

(That must be called from global-scope code!)

That will generate a pair of template specializations for the cast operators, and can be included multiple times in the same file (once per wrapped type).
See [WeakJSClassCreator-CastOps.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/WeakJSClassCreator-CastOps.h) for the complete docs.

If the native type will never be passed as a parameter or returned
from a function (i.e. it will only be the `this` object) then these
conversions are not strictly required. They're easy to add and allow you pass the custom
types to/from other (non-member) functions with ease, so they're recommended.

For the extremely curious, that macro generates something like the following:

```
namespace v8 { namespace juice { namespace convert {
template <>
struct NativeToJS<my_native>
{
    Handle<Value> operator()( my_native * p ) const
    {
	Handle<Object> jo = ::v8::juice::WeakJSClassCreator<my_native>::GetJSObject(p);
	if( jo.IsEmpty() ) return Null();
	else return jo;
    }
};
template <>
struct JSToNative<my_native>
{
    typedef my_native * result_type;
    result_type operator()( Handle<Value> const & h ) const
    {
	return ::v8::juice::WeakJSClassCreator<my_native>::GetNative(h);
    }
};
} } } // namespaces
```

(See the [type conversions page](ConvertingTypes.md) for the details.)

## Bringing it all together ##

And now what we originally set out to to: bind `my_native` with JS:

```
    typedef ClassBinder<my_native> WT;
    WT w( v8::Context()->GetCurrent()->Global() );
    typedef my_native MY;
    w.Set("prop1", String::New("this is my first property"))
      .Set("answer", Integer::New('*'));
    w.BindMemFunc< int, &MY::func1>( "func1" )
     .BindMemFunc< int, int, &MY::func2 >( "func2" )
     .BindMemFunc< std::string,&MY::hi >( "hi" )
     .BindMemFunc< MY *,&MY::me >( "me" )
     .BindMemFunc< bool,MY *,&MY::him >( "him" )
     .BindMemFunc< double,int,int,&MY::func3 >( "func3" )
     .BindMemFunc< void,&MY::avoid >( "avoid" )
     .BindMemFunc< void,int,&MY::avoid1 >( "avoid1" )
     .BindMemFunc< void,int,double,&MY::avoid2 >( "avoid2" )
     .BindMemFunc< double,int,int,int,&MY::takes3 >( "takes3" )
     .BindMemVar< std::string, &MY::str >("str")
     .BindMemVar< MY *, &MY::other >("other")
     // Bind 'proxiedProp' property to native getter/setter methods: (requires version r726+)
     .BindPropToAccessors< int, &MY::propGetter, void, int, &MY::propSetter >( "proxiedProp" )
    w.Seal();
```

Again, the templates syntax is ugly, but it also has no extra runtime memory costs other than that needed by the v8 internals and the conversion from JS to native.

While it isn't demonstrated here, `const` member functions can also be bound (the syntax is the same). For more information on binding member variables, see the API docs for `v8::juice::MemberVarGetter<>()` and `v8::juice::MemberVarSetter<>()` (in [ClassBinder.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/ClassBinder.h)).

## And finally... script code ##

Once we've done all that, here's what it looks like in JS:

```
var m = new MyNative();
print('void returns:',m.avoid(),m.avoid1(32),m.avoid2(17,11));
print(m.hi());
print(m.func3(m.func1(),m.func2(m.func1())))
print(m.him(m));
print(m.me());
print(m.takes3(2,2,3));
print('m.str =',(m.str='bye, world!'));
print('m.other =',m.other);
m.other = new MyNative();
m.other.str = "i am the other!";
print('m.other.str =',m.other.str);
print('proxiedProp =', m.proxiedProp );
m.proxiedProp += 23;
print('proxiedProp =', m.proxiedProp );
```

The output:

```
stephan@jareth:~/cvs/v8-juice/trunk/src/client/shell$ v8-juice-shell js/fwd.js
void returns: undefined undefined undefined
hi!
3528
true
[object Object]
12
m.str = bye, world!
m.other = null
m.other.str = i am the other!
proxiedProp = 19
proxiedProp = 42
```

# Advanced Topics #

The discussion continues in [ClassBinderPartTwo](ClassBinderPartTwo.md)...