

See also: [V8Convert](V8Convert.md), [V8Convert\_Overloading](V8Convert_Overloading.md), [V8Convert\_FunctionBinding](V8Convert_FunctionBinding.md), [V8Convert\_XTo](V8Convert_XTo.md)

# Introduction #

[V8Convert](V8Convert.md) has a flexible approach to binding native non-function properties to JS. It uses templates to create `v8::AccessorGetter` and `v8::AccessorSetter` functions, which are the property-side counterparts of `v8::InvocationCallback`. It is superficially similar to [the function-binding API](V8Convert_FunctionBinding.md), and accessors can be created a number of ways. For example, we can bind JS-side properties to:

  * Global/namespace-scope variables.
  * Member variables (static and non-static).
  * Member methods which proxy get/set access.

For any given native type we want to bind as a property, the following limitations apply:

  * For 'get' access, `CastToJS<TheType>()` must be legal.
  * For 'set' access, `CastFromJS<TheType>()` must be legal.

Property binding is done at the level of the `v8::ObjectTemplate` class, which is roughly equivalent to the class level in C++ and represents the JS-side `prototype` object for a JS class. The examples on this page often refer to a `proto` object, which is an instance of `v8::ObjectTemplate`:

```
v8::ObjectTemplate proto = ... wherever you get your Prototype object from ...;
```

If you're using [V8Convert\_ClassCreator](V8Convert_ClassCreator.md) that might look like:

```
typedef cv::ClassCreator<T> CC;
CC & cc( CC::Instance() );
...
v8::Handle<v8::ObjectTemplate> const & proto( cc.Prototype() );
```

(The weird `const &` bit is something i learned from reading something by Alexandrescu. Note that even though the Handle is const, its referenced value is not, so we can modify it.)

The examples also assume the following, for brevity/readability:

```
#define JSTR v8::String::New
```

# Achtung: Methods/Member Variables Need a "This" #

Binding _member_ variables or methods _requires_ that `CastFromJS<MyType>()` can convert the JS-side 'this' object to C++. Making that work is normally the function of a class-binding mechanism like [ClassCreator](V8Convert_ClassCreator.md), but there are a number of ways to set it up without using that specific binding mechanism. Strictly speaking, `CastFromJS<MyType>()` must return a `(MyType*)` and the bindings throw JS exceptions if that conversion returns `NULL`.

Whether or not `CastFromJS()` _can_ work is often not determinable at compile-time. i.e. it is possible (quite likely) that the code will compile fine even if the class is not "fully bound" as a JS class, but the inability to perform the conversion will only be detected when the conversion is attempted at runtime.

See [V8Convert\_ClassCreator](V8Convert_ClassCreator.md) for one approach to binding C++ classes to JS in a manner compatible with this API. There are many possible approaches. If you already have a favourite class-binding mechanism, simply create custom `JSToNative` and/or `NativeToJS` specializations which can use your binding mechanism.

# Binding Non-member or static Member Variables #

Here's how to create create accessors for non-member variables or _static_ class members:

```
struct MyType { static int anInt; };
int MyType::anInt = 42;
int myGlobalInt = 7;

...

proto->SetAccessor(JSTR("anInt"),
   VarToGetter<int,&MyType::anInt>::Get,
   VarToSetter<int,&MyType::anInt>::Set
);

proto->SetAccessor(JSTR("globalInt"),
   VarToGetter<int,&myGlobalInt>::Get,
   VarToSetter<int,&myGlobalInt>::Set
);
```


Often we want to disable setting of a JS property. To do that, do one of the following:

  * Use `ThrowingSetter::Set` as the setter, which will cause a JS exception to be thrown if the client assigns the variable.
  * Do not pass a setter to `SetAccessor()`. That might have undesired side-effects, though. IIRC, that leads to JS-set values "overriding" the variable, such that future `get` access reads the JS-assigned variable. (Again, IIRC.)
  * `SetAccessor()` has other default parameters which can be set to make a variable read-only (in which case setting it is simply ignored).

Note that there is no equivalent for throwing if a JS member _function_ value is assigned to from client code. Since v8 doesn't allow us to assign a setter without _also_ assigning a getter (it crashed when i tried), we cannot programmatically do it unless we also implement a custom getter which returns the JS function handle.

# Binding Native Member Variables #

```
struct MyType { int anInt; ... };

...

proto->SetAccessor(JSTR("anInt"),
    MemberToGetter<MyType,int,&MyType::anInt>::Get,
    MemberToSetter<MyType,int,&MyType::anInt>::Set
);
```

After that, all JS-side access to `anInt` will directly proxy the native `MyType::anInt`. Assigning, e.g., an `Object` to it would perform a numeric conversion, leading to a value of `0`.

(TODO?: consider writing a type-strict variation which throws if the type is not strictly convertible. We have this support for `InCa`s, so the proof-of-concept is there.)

# Binding Native Methods, Functions, and Functors as Property Accessors #

Binding methods isn't any different from binding non-function properties:

```
struct MyType {
    int getInt() const;
    void setInt(int);
    ....
};
...

v8::AccessorGetter g;
v8::AccessorSetter s;

g = ConstMethodToGetter<MyType,int (),&MyType::getInt>::Get,
s = MethodToSetter<MyType,void (int),&MyType::setInt>::Set;
// Or its equivalent: (requires XTo.hpp)
g = MethodTo< Getter, const MyType, int (), &MyType::getInt>::Get,
s = MethodTo< Setter, MyType, void (int), &MyType::setInt>::Set;
```

Non-member functions aren't normally quite as useful in the context of accessors (because they don't have instance-private state), but can also be bound.

```
void set_time(int32_t);
int32_t get_time();

g = FunctionToGetter< int32_t (), get_time >::Get;
s = FunctionToSetter< void (int32_t), set_time >::Set;
```

Functors can also be bound as accessors, but note that there are a few notable limitations:

  * We assume the functor is stateless and has a _const_ `operator()` (possibly multiple implementations). We do not keep a single copy for the lifetime of the bindings, but that might be an improvement to consider.
  * Since functors typically don't have access to the state of a bound native, they are not expected to be useful as accessors all that often, except for getters which return some shared state like the current system time or a random number.

In essence, functors and functions have the same limitations for accessor-binding purposes.

An example:

```
struct TimeFunctor {
  // set time
  void operator()( int32_t ) const;
  // get time
  int32_t operator()() const;
};

g = FunctorToGetter< TimeFunctor, int32_t () >::Get;
s = FunctorToSetter< TimeFunctor, void (int32_t) >::Set;

```

After that, all JS-side access to the bound property will go through the `TimeFunctor` operators..

# Mixing Various Approaches #

Since the property binding API simply creates `v8::AccessorGetter()` and `v8::AccessorSetter()` functions, we can mix the various approaches. Here is a rather contrived example:

```
struct MyType {
    int anInt;
    void setInt(int);
};

...

// Bind the getter to anInt directly and the setter to setInt():
proto->SetAccessor(JSTR("anInt"),
    MemberTo< Getter, MyType, int, &MyType::anInt>::Get,
    MethodTo< Setter, MyType, void (int), &MyType::setInt>::Set
);
```


# Tips and Ticks #

## Converting Native Exceptions thrown by Accessors ##

If a proxied getter or setter throws a native exception then it is likely to kill v8. Any accessors which throws should be wrapped in a `SetterCatcher` or `GetterCatcher` wrapper. These are analogous to the `InCaCatcher` used by the [function binding API](V8Convert_FunctionBinding.md), and they have the same interface.

A brief example demonstrating how to handle `std::exception` types:

```
v8::AccessorGetter g = GetterCatcher_std<
  FunctionTo< Getter, int (),get_some_val>
>::Get;
v8::AccessorSetter s = SetterCatcher_std<
  FunctionTo< Setter, void (int),set_some_val>
>::Set;
```

See the API docs for how to use custom exception types and chain exception handlers.

## Throwing JS Exceptions on Set Operations ##

To throw a JS exception if script code assigns a value to a property we can use `ThrowingSetter`:

```
proto->SetAccessor(JSTR("..."),
    ... some getter ...,
    ThowingSetter::Set
);
```