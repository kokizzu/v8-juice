

See also: [V8Convert](V8Convert.md), [V8Convert\_Overloading](V8Convert_Overloading.md), [V8Convert\_PropertyBinding](V8Convert_PropertyBinding.md), [V8Convert\_XTo](V8Convert_XTo.md)

# Introduction to Function Binding #

Quite possibly the simplest way in the world to convert free functions and member functions to `v8::InvocationCallback` functions is using the `XXXToInCa<>` templates ("InCa" is short for "InvocationCallback"):

```
v8::InvocationCallback cb;

// A non-const method: int MyType::nonConstMethod(char const *)
cb = cv::MethodToInCa<MyType, int (char const *), &MyType::nonConstMethod>::Call;
// Equivalent to:
cb = cv::ToInCa<MyType, int (char const *), &MyType::nonConstMethod>::Call;
// Equivalent to:
cb = cv::MethodTo< InCa, MyType, int (char const *), &MyType::nonConstMethod>::Call;

// A const method: int MyType::constMethod(char const *) const
cb = cv::ConstMethodToInCa<MyType, int (char const *), &MyType::constMethod>::Call;
// Or the equivalent as of r2019:
cb = cv::MethodToInCa<MyType const, int (char const *), &MyType::constMethod>::Call;
// Equivalent to:
cb = cv::ToInCa<MyType const, int (char const *), &MyType::constMethod>::Call;
// Equivalent to:
cb = cv::MethodTo< InCa, MyType const, int (char const *), &MyType::constMethod>::Call;


// A free function: int puts(char const *)
cb = cv::FunctionToInCa< int (char const *), ::puts >::Call;
// Equivalent to:
cb = cv::FunctionTo< InCa, int (char const *), ::puts >::Call;
// Equivalent to:
cb = cv::ToInCa< void, int (char const *), ::puts >::Call;
// The leading void arg is a bit of a hack to accommodate non-member functions.

// Binding a Functor, i.e. MyFunctorType::operator():
cb = cv::FunctorToInCa< MyFunctorType, bool (double) >::Call
// Equivalent to:
cb = cv::FunctorTo< InCa, MyFunctorType, bool (double) >::Call
```

(See [V8Convert\_XTo](V8Convert_XTo.md) for the current preferred conversion API.)

Because these templates _create_ `v8::InvocationCallback` functions at compile-time, they require no dynamically-allocated proxy objects or other hidden memory usage like some binding mechanisms do. They do add some runtime overhead, but not notably more than hand-written/optimized solutions do. Their overhead comes in the form of the intermediary/generated `InvocationCallback`s and the time/space it takes to convert arguments and return values to/from JS.

The library does as much work as possible at compile time to help detect type-conversion errors early and help avoid extra runtime costs.

The core function-binding features are encapsulated in three templates:

  * `FunctionToInCa<>`
  * `MethodToInCa<>`
  * `ConstMethodToInCa<>`

By using only those templates clients can bind a wide variety of native functions to arbitrary JS objects (where language calling conventions and cross-language type conversions allow). `FunctorToInCa` is actually a thin proxy over `ConstMethodToInCa` which proxies `FunctorType::operator()`.

Support for const- and non-const methods are split into two implementations to help support MSVC compilers (but as of 20110720 we _might_ have a way to consolidate them into once class even for MSVC). As of [r2019](https://code.google.com/p/v8-juice/source/detail?r=2019) `ConstMethodToInCa<T,...>` and `MethodToInCa<const T,...>` are equivalent, so clients may choose to ignore `ConstMethodToInCa` altogether (just remember to const-qualify the `T` parameter to `MethodToInCa` and friends).

Note that binding to non-static member methods requires (at runtime), that `CastFromJS<T>()` can convert the current JS 'this' object to its native counterpart. See [V8Convert\_ClassCreator](V8Convert_ClassCreator.md) for an example of one way to set this up (there are many ways).

# Argument Type Information #

When using `FunctionToInCa` and friends we can use template techniques to get type information about the arguments if we want to use that for conversion purposes:

```
using namespace cvv8;
typedef FunctionToInCa<int (char const *,double), my_function> MyFunc;

typedef char AssertArityIs2[(2 == cv::sl::Arity<MyFunc>::Value) ? 1 : -1];
typedef sl::At< 0, MyFunc >::Type A0; // == (char const *)
typedef sl::At< 1, MyFunc >::Type A1; // == double
typedef sl::At< 2, MyFunc >::Type A2; // compile-time error (arg out of bounds)

// Note that "InCa-like" functions have an Arity value of -1:
typedef char AssertArityIsNegative[
   (sl::Arity< Signature< void (v8::Arguments const &) > >::Value == -1)
   ? 1
   : -1];
```

We can use further templating techniques to do things like choose different code paths based on that type information, as in this rather contrived example which changes `int64_t` to `double` (v8 does not internally support 64-bit integers):

```
namespace tmp = cv::tmp;
typedef tmp::IfElse< tmp::SameType<int64_t,SomeNumericType>::Value,
                     double,
                     SomeNumericType
                   >::Type NumericType;
```

Template gurus will certainly come up with new and original ways to use and abuse this feature.

# Examples #

Some examples of binding functions and methods:

```
#include <cstdio> // puts()
...
namespace cv = cvv8;

v8::InvocationCallback cb; // v8's generic callback function interface

// Here's how we can "convert" various native functions to
// v8::InvocationCallback functions:

// Function: int puts(char const *)
cb = cv::FunctionToInCa<
       int (char const *), ::puts
     >::Call;

// Class method: double MyType::multiply(double, double)
cb = cv::MethodToInCa<
       MyType,
       double (double,double),
       &MyType::multiply
     >::Call;

// Const class method:
//    char const * MyType::errorCodeToString(int) const
cb = cv::ConstMethodToInCa<
       MyType,
       char const * (int),
       &MyType::errorCodeToString
     >::Call;

// A functor type (it must have a const operator()):
cb = cv::FunctorToInCa< MyFunctorType, bool (double) >::Call

// Here's how we can bind a JS-side garbage-collection-triggering
// function:
cb = cv::FunctionToInCa<bool (),v8::V8::IdleNotification>::Call;
```

Those examples demonstrate all of the public API one needs for converting functions to `v8::InvocationCallback` equivalents! There are some additional templates which add features on top of those, such as dispatching overloaded functions, but those are all one really needs for most basic function bindings.

It might be interesting to note that the above functions are _created_, but _not actually called_, by the above code (note that we pass no non-template parameters to them). All of the real work is done at compile-time.

# Caveats and Unsupported Functions #

Some examples of function signatures which _cannot_ (or must not) be converted using this framework:

  * `int func( someType ** )`: pointer-to-pointer cannot be translated generically to JS
  * `int func( void * )`: Giving access to `void *` in JS makes it really easy to crash your apps from script code.
  * `int func(...)`: elipsis lists, `va_list`, and C++0x var-args are not directly supported.
  * JavaScript has no inherent support for binary data, and v8's string class does not support it. Thus this API must not be used with functions returning binary data (e.g. via `char [const]*`).

Supported, but _possibly_ ill-advised:

  * `T * func(...anything...)`: Because...

If the function returns dynamically-allocated memory, or transfers ownership of the returned memory to the caller, it _will_ cause a leak unless that memory has been somehow given over to v8 from within that function. That said... types for which `NativeToJS<T>` is feasible (this depends on several factors), we can in fact make this conversion legal for many client-defined types. The demo code in the source tree shows an example of how to do this.

## The (`char [const] *`) Problem ##

Because of issues regarding the ownership and lifetimes of raw C-style string, functions taking `(char [const] *)` have a few relatively minor limitations:

  * The C-strings _must_ be `NULL` or nul-terminated. It cannot be used with binary data because they are converted to JS strings.
  * Non-const (`char *`) ~~is not supported~~ support was added 20120408, with the caveat that changes made to the string contents by the called function cannot be passed back to JS space because the JS calling conventions do not allow us to pass changes to strings back this way.
  * The bound function _must not_ hold the pointer it is passed after it returns. It must either consume or ignore the input, and keep no copy of the pointer itself. It is also illegal (it was determined only after a debugging session) to _return_ the pointer passed to the function, as that indirectly extends its lifetime past its legal point. That particular case actually works on some platforms but that only exemplifies the evil nature of Undefined Behaviour.
  * The internal conversion process uses `v8::String::Utf8Value`, and "should" be safe for use with functions accepting ASCII or UTF8 input. Behaviour with any other encodings is undefined.
  * If passed a JS `null` or `undefined`, it converts to a literal `NULL`, otherwise it will convert to a string (though possibly empty). This is significant for functions which treat `NULL` different than an empty string (like `strlen()`, which may crash your app if passed `NULL`).

When native functions return (`char const *`), they are converted to v8 Strings, and therefore have the same encoding limitations as for C-string function arguments. A return value of `NULL` is converted to a JS `null`, as opposed to an empty string.

This support allows us to convert functions like the following to `v8::InvocableCallback`s:

```
std::string cstring_test( char const * c )
{
    std::cerr << "cstring_test( @"<<(void const *)c
              <<") ["<<(c ? c : "<NULL>")<<"]\n";
    return std::string(c ? c : "");
    /*
       We don't return (char const *) because it is
       illegal to return the c argument because the
       lifetime of its pointer cannot be guaranteed
       to extend beyond the return of this call.
    */
}

v8::InvocationCallback cb =
   cv::FunctionToInCa<
     std::string (char const *),
     cstring_test
    >::Call;
```

If we bind that to JS with the name `cstr`:

```
var jstr = "Hi, world!";
jstr = cstr(jstr); print(jstr);
jstr = cstr(undefined); print(jstr);
jstr = cstr(null); print(jstr);
jstr = cstr("Bye, world!"); print(jstr);
```

The output looks something like:

```
cstring_test( @0x984acbc) [Hi, world!]
Hi, world!
cstring_test( @0) [<NULL>]
null
cstring_test( @0) [<NULL>]
null
cstring_test( @0x984acbc) [Bye, world!]
Bye, world!
```

(The fact that the pointer addresses are the same in the first and last calls is a mere fluke of memory re-allocation, and must not be understood to mean that the address is somehow stable across calls.)

**DO NOT DO THIS:** (even though i documented it for a long time as being valid - it isn't!)

```
char const * cstring_test( char const * c )
{
    std::cerr << "cstring_test( @"<<(void const *)c
              <<") ["<<(c ? c : "<NULL>")<<"]\n";
    return c;
}

v8::InvocationCallback cb =
   cv::FunctionToInCa<
     char const * (char const *),
     cstring_test
    >::Call;
```

The problem is that it will work sometimes and not others - it's undefined. The function bindings specifically disallow a passed-in (`char const *`) from being "kept", and returning it is a form of "keeping" it and that violates the lifetime constraints. The function bindings cannot handle this particular corner case (returning the (`char const *`) passed in to it) because the lifetime of the  raw bytes cannot be properly managed at the binding level. (Allowing (`char const *`) parameters, as opposed to return values, requires an extra level of indirection to keep the bytes alive for any useful length of time.)

# Special Cases #

This section describes "special cases" in the API, meaning cases for which the library has special support.

## `v8::InvocationCallback`-like Functions ##

The library provides special support for binding functions which "look like" a `v8::InvocationCallback`. The definition of "looks like" is: the function takes only a single `(v8::Arguments const &)` argument and returns any type.

Such functions are treated like `v8::InvocationCallback` instances, in that they can accept any number of arguments of any type. The return value is converted to a `v8::Handle<v8::Value>` (or to JS `undefined` for `void` returns).

Examples of what we consider to be `v8::InvocationCallback`-like:

```
int my_callback( v8::Arguments const & );
void MyType::callback( v8::Arguments const & );
double MyType::constCallback( v8::Arguments const & ) const;
v8::Handle<v8::Value> RealInvocationCallback( v8::Arguments const & );
```

And we can create `v8::InvocationCallback` functions from them like:

```
InvocationCallback cb;

// Non-member function:
cb = FunctionToInCa< int (Arguments const &),
                     my_callback >::Call;
// Equivalent to:
cb = InCaLikeFunction<int, my_callback>::Call;


// Non-const member method:
cb = MethodToInCa< MyType,
                   void (Arguments const &),
                   &MyType::callback >::Call;
// Equivalent to:
cb = InCaLikeMethod<MyType, void, &MyType::callback>::Call;


// Const member method:
cb = ConstMethodToInCa< MyType,
                        double (Arguments const &),
                        &MyType::constCallback >::Call;
// Equivalent to:
cb = InCaLikeConstMethod<MyType, double, &MyType::constCallback>::Call;


// The simplest case is:
cb = InCaToInCa< RealInvocationCallback >::Call;
// We can of course simply use:
cb = RealInvocationCallback;
// But the former approach is useful when we need to pass an InCa type
// as a template parameter and we only have an InvocationCallback.
// We can also use FunctionToInCa and friends to bind InCa-like functions,
// but InCaToInCa is a simpler way to do it for this special case.
```

(Pretty simple, huh?)

InCa-like non-member functions bound have a particularly interesting property: if such a function is bound as a member of a `T` native object, we can access the native `this` pointer from inside that function like this:

```
int my_callback( v8::Arguments const & argv )
{
   T * self = cv::CastFromJS<T>(argv.This());
   if( !self ) {
       // no native T found. This might or might not
       // be an error, depending on app-specific conditions.
   }
   ...
}
```

## Overloading Functions ##

See [V8Convert\_Overloading](V8Convert_Overloading.md).

## Non-convertible (or ignored) Return Types ##

Sometimes we want to bind a function which has a return type which we cannot convert to JS. That means we can't bind it (compile errors if we try). There is, however, a half-workaround. If we don't mind simply losing the return value on the way back to JS, we can use `ToInCaVoid<>` to generate an `v8::InvocationCallback` proxy which explicitly does not call `CastToJS()` to convert the return type. The implication of that is that we can then bind it (despite the non-convertible return value), but JS will get the `undefined` value as the result of each call:

```
v8::InvocationCallback cb = cv::FunctionToInCaVoid<SomeNonConvertibleType (args...), myFunction >::Call;
// Equivalent to:
cb = cv::FunctionTo< InCaVoid, SomeNonConvertibleType (args...), myFunction >::Call;
```

We can also use that to bind functions whos return types we _could_ convert but don't want to for some reason (e.g. we don't want native error codes going back to JS).

**Achtung:** Be careful not to leak resources this way. If dynamically-allocated resources are returned from a function bound this way, they will be irrevocably leaked on every call to the function!


## Unlocking v8 for the Duration of a Call ##

Consider this function binding:

```
#include <unistd.h> // sleep(), or Sleep(N*1000) on Windows
v8::InvocationCallback cb =
    cv::FunctionToInCa<
       unsigned int (unsigned int), sleep
     >::Call;
```

When that callback is called, the calling code is locking v8, meaning that other v8-using threads cannot preempt it (that's not actually true anymore, but assume it is for now - we'll clarify this lie a couple paragraphs down). When calling C-level APIs which themselves do not call back into v8, we may instead want to tell v8 that other threads may run. That is, in the generic case, done like this:

```
... somewhere inside a v8 callback function ...
{
  v8::Unlocker unlock;
  nativeFunction(...);
}
... v8 is locked again here, so we can use v8 again ...
```

If the native function will only run very briefly then unlocking will probably be slower than leaving v8 locked, but if the function may run an arbitrarily long amount of time, unlocking it is wise because v8 can then allow other threads to continue instead of blocking while this call holds the v8 lock.

As of the early morning of 20110625, the library includes generic support for optionally telling function bindings that they should unlock v8 for the duration of the native call.

The default value for this option (for any given instantiation of the templates, not globally) depends on several factors but it will normally be enabled (meaning that v8 will be unlocked for the duration of the native function call). If the function signature involved contains any v8 types then unlocking is disabled and a compile-time assertion is triggered if the client explicitly enables it (see examples below).

To force a given function/method binding to run in "unlocked" mode, simply pass `true` as the final (optional) argument to `XXXToInCa`:

```
v8::InvocationCallback cb;

cb = cv::FunctionToInCa<int (char const *), ::puts, true >::Call;

// Class methods are handled the same way:
cb = cv::MethodToInCa<T, int (double), &T::foo, true>::Call;
cb = cv::ConstMethodToInCa<T, int (double), &T::fooConst, true>::Call;
```

**Achtung:** this support has a couple notable gotchas, caveats, and Things One Needs to Know. The ones which immediately come to mind are:

  * A compile-time assertion will be triggered if you pass `true` to enable it and the function cannot, due to one of the types in its signature, be run in unlocked mode. See the `IsUnlockable<>` and `SignatureIsUnlockable<>` API docs for all the gory details.
  * Unlocking tells v8 to perform some threading/mutex-related voodoo about which i know remarkably little. There is a performance hit for this, but it is marginal.
  * _Reacquiring_ the v8 lock after the native function returns can theoretically take arbitrarily long. This could happen if the code being run in another thread never releases the v8 lock and runs in a loop which v8 cannot interrupt (e.g. the loop never calls back into v8).

It is illegal for the unlock option to be enabled if ANY of the following applies:

  * The callback itself will "use" v8. If it uses a `v8::Locker` to fence its access then it _is_ legal to use v8 in that function even if unlocking is enabled at this API's level.
  * Any of the return- or argument types, or the containing type itself for bound class methods, are  "blacklisted", meaning that `IsUnlockable<T>::Value` is false (that includes the common v8 types, e.g. `v8::Handle<Anything>` or `v8::Arguments`). These types cause unlocking to be disabled by default and a compile-time assertion is triggered if one explicitly tries to enable unlocking for such a function. By extension, but more concretely...
  * `InvocationCallback`-like functions will, if unlocking is enabled, trigger a compile-time assertion. We _cannot_ unlock for these because that would make using the `v8::Arguments` object illegal (which would mean that we could not legally convert and pass on the arguments to the native function!).

There might be other corner cases where unlocking v8 is semantically illegal at this level of the API but are not caught as compile-time errors. The "out of the box" behaviour uses template magic to disable unlocking by default for all known-illegal signatures. See the `IsUnlockable<>` API docs for how to add additional types to "the blacklist" (it requires only a new template specialization with one enum/boolean value).

i have in fact seen binding cases where the default determination of "can we safely unlock v8?" is incorrect, leading to v8 assertions (app crashes) at runtime. In such cases v8's assertion dump showed me where the problem was (which function calls) and i could use that info to disable unlocking for those few functions. They caused this error because they outwardly (in their signatures) use only non-v8 types, but internally they use v8 without also using a `v8::Locker` (they assume it's being called from JS and therefore already locked). So they crashed. In such cases, unlocking should be explicitly disabled for that bind, as shown above, _or_ the functions in question need a `v8::Locker` added to them.

If you are binding a class and want to disable unlocking for _all_ bound member functions of that class, do:

```
template <>
cv::IsUnlockable<MyType> : cv::tmp::BoolVal<false> {};
```

That effectively adds `MyType` to the "do not unlock" blacklist, and any bound JS functions which are members of that type, return that type, or have it in their arguments list will have unlocking support disabled by default (this is a compile-time decision). In such cases, explicitly enabling locking for any such function will cause a compile-time assertion to be triggered, just as it does for functions having v8 data types in their signatures. For example, assuming the above "blacklist specialization" is in place:

```
v8::InvocationCallback cb;

cb = cv::FunctionToInCa< int (MyType *, double), some_function >::Call;
// That's legal b/c unlocking is disabled due to IsUnlockable<MyType>.
// But this will fail to compile:
cb = cv::FunctionToInCa< int (MyType *, double), some_function, true >::Call;
// because unlocking _cannot_ be legally enabled for that function (because of
// the IsUnlockable<MyType> specialization).
```

Note, however, that _non-member functions_ bound as JS-side member functions to such a native do not "know" that they have been "upgraded" to be a member method, and do not consider the bound type for unlocking purpose (but parameter/return types are considered). In such a case, explicitly disabling locking for that function might be required (see above) or the functions might need a `v8::Locker` added to them.

## Converting Native Exceptions to JS ##

v8 does not like for client-thrown exceptions to propagate through it, and therefore bindings like the following can be dangerous to an app's lifetime:

```
v8::InvocationCallback cb = cv::FunctionToInCa< int (),someFuncWhichThrows>::Call;
```

If that function throws in the context of a JS-initiated call, it might crash v8. And when i say "might", i mean "it almost certainly will."

The `InCaCatcher` class template creates an `InvocationCallback` function which calls a client-specified `InCa` and catches any exceptions of a given base type. It has a template option which specifies whether other exceptions should be propagated or converted into "unknown error" for JS, and this property allows us to chain the catchers to handle multiple concrete exception types.

It can, in principal, catch and report any exception type which meets the following conditions: it must have a const member method taking no arguments and returning an error message value in a type which is convertible to JS using `CastToJS()`. e.g. (`char const * std::exception::what()`) qualifies nicely. A hypothetical (`int MyException::GetErrorCode()`) would also qualify, provided it is const, though the messages it returns to v8 would simply be stringified numbers.

`InCaCatcher` has a relatively detailed interface, which is documented (in corresponding detail) here:

> [invocable\_core.hpp](http://code.google.com/p/v8-juice/source/browse/convert/include/v8/convert/detail/invocable_core.hpp)

Search that file for `struct InCaCatcher` and the API docs are just above that.

The `InCaCatcher_std` template is a `InCaCatcher` convenience wrapper which handles `std::exception` errors (and subtypes).

Example usage:

```
typedef InCaCatcher<
       std::exception, // type to catch (by (const &)!)
       char const *(), // signature of message-getter
       &std::exception::what, // message-fetching method
       InCaToInCa<callbackWhichThrowsStdException>, // the callback to try/catch
       false // whether to propagate other exceptions or not (default=false)
   > Catcher;

// Note: InCaToInCa<> turns an InvocationCallback into a "type-rich" InCa.

// Or, more simply (for std::exception):
typedef InCaCatcher_std< InCaToInCa<callbackWhichThrowsStdException> > Catcher;

// Get the callback function with:
v8::InvocationCallback cb = Catcher::Call;
```

Here's an example which demonstrates how we can use `std::string` as an exception type (_not_ that i recommend doing so, but this same approach can be used for custom exception types):

```
void throwStdString()
{
    throw std::string("std::string thrown as an exception.");
}

v8::InvocationCallback cb =
  cvv8::InCaCatcher<std::string,
    char const * () const,
    &std::string::c_str,
    FunctionToInCa< void (), throwStdString >
  >::Call;
```

When called from JS, the script will get an exception containing the error string (but converted to a JS `Error` object).

For examples of chaining these to support multiple exception types, including handling exception hierarchies, see the demo code and/or the API documentation.

# Forwarding v8-supplied Arguments to Non-bound Functions #

  * `forwardFunction()`
  * `forwardMethod()`
  * `forwardConstMethod()`

Those functions allow the client to pass on `v8::Arguments` to non-bound functions "manually".

**Achtung:** Because these functions use `FunctionToInCa` (and friends), they inherit the "v8 unlocking" behaviour from that class. That is described in much more detail in another section of this page, but in short: if the arguments list or return value of the function does not have any v8 types (`v8::Handle`, `v8::Arguments`, etc.) then then the v8 engine is _unlocked_ for the duration of the native call. _If_ the function, despite having no v8-specific arguments/return type, internally uses v8 (or calls another function which does), then it _must_ also use a `v8::Locker` _or_ clients must use `FunctionToInCa` (or compatible) instead of these functions and explicitly disable locking (see the documentation and examples on the [V8Convert\_TipsAndTricks](V8Convert_TipsAndTricks.md) page). If one of these two conditions are _not_ met, v8 will trigger an assertion at runtime (when the function is called from JS), killing the application (and frustrating the user). Debug builds of libv8 will dump an explanation of the error to `stderr`, but non-debug builds will silently crash.


# Passing Native Arguments to JS Functions #

We occasionally (or more often, or less often) want to call JS functions, in the form of `v8::Handle<v8::Function>` objects, from native code and pass them native-level parameters.

One simple way to do this is via the `CallForwarder`, which is parameterized on the number of native arguments it accepts. Its interface looks like:

```
static v8::Handle<v8::Value> Call( v8::Handle<v8::Object> const & self,
                                   v8::Handle<v8::Function> const & func,
                                   ... N arguments ... );

// Equivalent to Call( func, func, ... N args ... ).
static v8::Handle<v8::Value> Call( v8::Handle<v8::Function> const & func,
                                   ... N arguments ... );
```

The `self` object is the `func` object's logical `this` pointer. If you don't need a specific `this` object then use the second form of `Call()` (which is equivalent to passing `func` as the first _and_ second parameters to the first overload).

```
v8::Handle<v8::Value> rc;

rc = cv::CallForwarder<0>::Call( someJsObject, jsFunction );
// That's equivalent to jsFunction->Call( someJsObject, 0, NULL )

rc = cv::CallForwarder<1>::Call( jsFunction, "argument 1" );
rc = cv::CallForwarder<2>::Call( jsFunction, "argument 1", arg2 );
...

// We can fetch results from output parameters like this:
v8::Handle<v8::Object> jobj( v8::Object::New() );
rc =
   cv::CallForwarder<1>::Call( jsFunction, jobj );
   // If jsFunction modifies jobj, we can now see those changes.
```

# Converting Functors to Callbacks #

Interestingly, binding C++ functors to v8 turns out to be a special case of binding a member method, and the implementation for it is just a thin wrapper around the member method binding. However, the extra level of abstraction adds a couple limitations on such bindings. Functors can be bound to v8 provided they can meet the following requirements:

  * They are assumed to be stateless. The library makes no guarantees about the lifetime of any given functor instance.
  * The must have one or more _const_ `operator()`s. The `const` requirement is an unfortunate side-effect of the level of abstraction. Maybe we can template our way around it, but so far there has been no pressing need to do so.

Functors can be converted to `v8::InvocationCallback`s like:

```
struct MyFunctor {
    bool operator()() const;
    int operator()(int) const;
    void operator()(int, double) const;
};

...

v8::InvocationCallback cb;

cb = FunctorToInCa< MyFunctor, bool() >::Call;
cb = FunctorToInCa< MyFunctor, int(int) >::Call;
cb = FunctorToInCa< MyFunctor, void(int,double) >::Call;
// Equivalent:
cb = FunctorTo< InCa, MyFunctor, bool() >::Call;
cb = FunctorTo< InCa, MyFunctor, int(int) >::Call;
cb = FunctorTo< InCa, MyFunctor, void(int,double) >::Call;
```

See [V8Convert\_XTo](V8Convert_XTo.md) for more information regarding the `FunctorTo` approach.