**Newsflash:** as of [r1920](https://code.google.com/p/v8-juice/source/detail?r=1920) we have v8-convert building on MSVC 2010. i occasionally break the MSVC bits by accident (i don't have Windows/MSVC to test on), so if you find that it doesn't compile please post the error logs to [our mailing list](http://groups.google.com/group/v8-juice-js-dev) and i'll be happy to take a look. (It is completely untested on MSVC versions earlier than 2010, by the way, but Cohen suspects that it "should work" with MSVC 2008.)



# v8-convert (a.k.a. cvv8) #

Source code: http://code.google.com/p/v8-juice/source/browse/#svn/convert (see [this link](http://code.google.com/p/v8-juice/source/checkout) for checkout instructions.)

The v8-convert API (a.k.a. `cvv8`, because it's easier to type than `v8cv`) is a spin-off of the core-most components of the [v8-juice library](HomePage.md). It has a templates-heavy, header-only implementation and has no 3rd-party dependencies beyond the STL and whatever v8 needs. While i have always been well served by [v8-juice's type conversion framework](ConvertingTypes.md), v8-convert has improved upon its father's features considerably, allowing client applications and libraries to export their APIs to the v8 JavaScript engine with unprecedented ease, flexibility, and type-safety. `<hype>`As of June 2011 i will go so far to say that i personally consider v8-convert to be the most comprehensive, most powerful, and easiest to use type-conversion API available for v8.`</hype>`

v8-convert is not a "mutually exclusive solution" to its problem domain, and it can easily be used in conjunction with other v8 client libraries like [vu8](https://github.com/tsa/vu8) and [node.js](http://www.nodejs.org/).

V8Convert's main features/focus are:

  * Converting between v8 Value handles and "native types" using a generic interface. This allows us to write generic algorithms which convert between JS/C++ without having to know the exact types they're dealing with.  The basic POD types and some STL types are supported out of the box and plugging in one's own types is normally quite simple.
  * Converting free functions, functors, and member functions into `v8::InvocationCallback` functions. These generated functions convert the JavaScript-originated function arguments into native counterparts, forward the data to the original native function, and convert the return values back to something JS can use. (The `void` return type is translated to the `undefined` JS value.) It supports [overloading bound functions](V8Convert_Overloading.md).
  * Binding JS object properties to native shared or member properties or accessor/mutator functions/functors/methods.

The first two core features give us all we need in order to be able to bind near-arbitrary C/C++ functions with JavaScript, insofar as cross-language calling conventions and type conversions allow us to do so. The last feature is really just "sugar" (but also quite convenient at times). When combined, they give us everything we need to implement generic C++/JS class-binding mechanisms like [V8Convert\_ClassCreator](V8Convert_ClassCreator.md).

While these features are easily summed up in a short list, their broader implications are quite large. The ability to convert functions from one signature to another (at compile time, by the way) has proven to be deceptively powerful, allowing users to bind new functions to v8 with unprecedented ease. We can combine this with template meta-programming techniques to do weird things like combine multiple native functions into a single `InvocationCallback` function (i.e. overload it) which dispatches to a different native function [based on arbitrary logic](V8Convert_Overloading.md) (using a mixture of compile- and run-time dispatching).

For cases where the "automatic" function-to-InvocationCallback conversions are not possible or not suitable, the type-conversion API can still simplify the implementation of custom `v8::InvocationCallback` functions.

The most important functions and types, from a user's perspective, are described throughout this page. Most of the code in this library is internal template specializations which take care of the dirty work. Typical clients won't normally need more than what's demonstrated in the various wiki pages.

All of the API's type/function conversions are compile-time typesafe where possible and fail gracefully (e.g. using a JS-side exception as opposed to an immediate crash) when such a determination can only be made at run-time. Some exceptionally severe problems can only be reported via native exceptions. e.g. if a type conversion to `(T&)` would require dereferencing a `NULL` `(T*)` to create the reference, a native exception will be thrown. Note that _native_ exceptions which pass through v8 normally (always?) crash the application, so clients must generally be careful when using exception-throwing code with v8. cvv8 clients can use the `InCaCatcher` class to encapsulate the catching and conversion of native exceptions to JS exceptions, and in principal it supports catching/converting any conventional client-side exception type. It also supports chaining multiple "catchers" to handle any number of exception types, whether they are derived from some common base or completely unrelated.

## Credits ##

The contributors are listed on the [Contributing](Contributing.md) page.

## Downloading (Getting the Source Code) ##

[The source code](http://code.google.com/p/v8-juice/source/browse/#svn/convert/) is maintained in the v8-juice project's subversion repository:

```
svn checkout http://v8-juice.googlecode.com/svn/convert cvv8
```

The library is header-only, and the only files clients need is under the `include` directory. The source tree contains several demos and complete examples, as well, but building them requires a Unix-like platform with a GNU toolset (contributions of new build/project files for your favourite build tool will be gladly accepted).

Some of the header files are generated by scripts as part of the build process, but pre-generated copies are kept in subversion so that clients to not have to build them.

## Notes about the Namespace ##

Prior to 20110718, this code lived in the `v8::convert` namespace. Having seen the error of my ways in adding a sub-namespace of v8's namespace, it was
moved into the `cvv8` namespace (`cvv8` is easier to type than `v8cv`, at least when using a U.S. English keyboard layout). Some documentation may still refer to `v8::convert`.

Most of the sample code in the wiki (and lots of the real code, for that matter) uses the following namespace alias:

```
namespace cv = cvv8;
```

## Header Files ##

[The header files](http://code.google.com/p/v8-juice/source/browse/#svn/convert/include/cvv8) live in the `cvv8` dir (was `v8/convert` before 20110718). The `cvv8/detail/` dir contains primarily internal details, but also contains much of the public API. Client code should never include the `cvv8/detail` files directly, as they are subject to change at any time. The headers client code will normally need are:

```
// Everything most clients need:
#include "cvv8/v8-convert.hpp" 

// Primary APIs:
#include "cvv8/convert.hpp" // main type conversion API
#include "cvv8/invocable.hpp" // function conversion API
#include "cvv8/properties.hpp" // property-binding APIs
#include "cvv8/arguments.hpp" // mainly function-binding-overloading code
#include "cvv8/XTo.hpp" // the "X-to-..." alternative conversion API

// "Utility" APIs:
#include "cvv8/ClassCreator.hpp" // class-binding mechanism.
... several others ...
```

## Generating API Documentation ##

The (abundant) API docs are written in [doxygen](http://www.doxygen.org)-friendly form and can be processed by doxygen like this:

```
~> cd doc
~> make doc # or simply: doxyen
```

When using `make doc`, the name of the output directory will be shown on stdout.

The public API is thoroughly documented, and as a rule undocumented functions and classes are not for public use. That said, many of the classes are template specializations which exist only for templates-related reasons, and they are not necessarily documented - see the primary template declaration in such cases.

Achtung: for some reason doxygen (the two versions i've tried) is nesting the `cvv8` namespace and a couple of its sub-namespaces within another layer of `cvv8`. e.g. `cvv8::sl` might become `cvv8::cvv8::sl` in the generated docs. i have no idea why this is happening.

# Converting Types #

The public type conversions API is based on only two functions:

  * `CastToJS<T>()`: converts the given object to a `v8::Handle<v8::Value>`.
  * `CastFromJS<T>()`: converts a v8 value to a `T` (actually `T*` for most non-POD types).

These are very thin wrappers around `NativeToJS<>` and `JSToNative<>`, and work identically to the v8-juice APIs with the same names (see [ConvertingTypes](ConvertingTypes.md)). A few special-case overloads also exist to avoid call ambiguities in some cases or to cover special cases (most notably those taking (`char *`) and `v8::InvocationCallback` arguments).

Using these operations allows us to convert a wide variety of types between JS and C++, and the `NativeToJS<>` and `JSToNative<>` classes give us a way to extend the conversion operations to support custom conversions. This level of abstraction allows us, in turn, to implement all sorts of algorithms which are ignorant of the _types_ they are working on. The function-binding APIs, for example, rely on this for converting function argument and return values to/from JS/C++.

# Implementing Custom Conversions #

  * `JSToNative<T>`: template to convert JS values to `T` (_almost_ always `T*` for non-POD types).
  * `NativeToJS<T>`: template to convert `T` objects to JS counterparts. Adding this support for client-side types requires adding tooling which maps the native object to a JS instance (or that the bound type hold this information itself).

These work identically to the v8-juice APIs with the same names (see [ConvertingTypes](ConvertingTypes.md)), the only client-visible difference being that the classes (and thus client-defined specializations) are defined in a different namespace than in v8-juice. Future (post-2010) development in the type conversions API will happen in this copy instead of the `v8::juice` copy.


# Converting Functions to `v8::InvocationCallback` #

This is covered in detail in [V8Convert\_FunctionBinding](V8Convert_FunctionBinding.md) and  [V8Convert\_Overloading](V8Convert_Overloading.md).

# But wait, there's more! #

The code includes the following additional utilities:

  * [properties.hpp](http://code.google.com/p/v8-juice/source/browse/convert/include/v8/convert/properties.hpp) - templates for binding native global or member variables to JS objects. These basically work like `FunctionToInCa` and friends, but create `v8::AccessorSetter()` and `v8::AccessorGetter()` implementations using templates to bind JS properties directly to native variables or to methods/functions which proxy all get/set access on the JS property.
  * [arguments.hpp](http://code.google.com/p/v8-juice/source/browse/convert/include/v8/convert/arguments.hpp) - templates for "interrogating" v8-provided arguments, for use in making templates/functors which can dispatch to various native bindings based on near-arbitrary argument state (count, type, etc.).
  * [V8Convert\_ClassCreator](V8Convert_ClassCreator.md) - yet another C++-to-JS class binding mechanism. Conceptually based on v8-juice's [ClassWrap](ClassWrap.md), with the same overall design but a slightly different policy set.
  * [V8Shell.hpp](http://code.google.com/p/v8-juice/source/browse/convert/include/v8/convert/V8Shell.hpp) provides a really simple way to get v8 integrated quickly into an application. Just create one object, optionally extend the JS environment with your functionality, and start running JS code through it. See: [V8Convert\_Shell](V8Convert_Shell.md).

## Easily Binding C++ Classes ##

See [V8Convert\_ClassCreator](V8Convert_ClassCreator.md) for one approach to binding C++ classes to JS. Many developers already have their own favoured approach, and those approaches can be used with this framework as long as `CastFromJS<T>()` and (only in some cases) `CastToJS<T>()` can be made to work. As a rule `CastFromJS()` support is easy to do, and fairly generic solutions exist for the problem. `CastToJS()`, on the other hand, requires type-specific knowledge which is not genericizable without requiring that all bound classes subclass a specific base type/interface (or having similar restrictions). The `cvv8` demo code shows a couple different ways of adding plumbing (a.k.a. overhead) which allows the JS-to-native conversion to work for a given type.

## Binding Native Variables/Properties to JS ##

See [V8Convert\_PropertyBinding](V8Convert_PropertyBinding.md).

## Passing v8 Arguments directly to Constructors ##

See [V8Convert\_Overloading](V8Convert_Overloading.md) (though one need not overload in order to use this feature).

# Brief Overview of `cvv8` Architecture #

This API is made up of several distinct components, each one layered on top of the next:

  * Basic template-metaprogramming-related types. Most of these are not really considered to be part of the public interface, but are necessary implementation details.
  * The core type conversions layer, e.g. `CastToJS()`/`CastFromJS()`.
  * Function/Method-signature-related templates. These do not do any JS/C++ conversions, but provide a mechanism with which we can store complete type information for a given function signature. These bits form the basis of the function binding API, but are not specific to that API.
  * The function-to-`v8::InvocationCallback` conversions. This provides tons of template glue which creates `v8::InvocationCallback` functions by converting near-arbitrary function signatures to `v8::InvocationCallback`. These rely on `CastToJS()` and `CastFromJS()` to convert all arguments and return types from/to JS (with the exception of `void` returns, which require some special handling due to some syntactic constraints of the `void` type).

One of the most significant properties of this API is that it can report a wide variety of conversion errors at compile-time. Conversion APIs which are based on macros, as opposed to templates, (depressingly common in real-world v8 extension libraries) cannot come _anywhere close_ to achieving the flexibility and type-safety of this library.

For example, the following errors will show up at compile time, not runtime:

```
int my_function( int, double );

v8::InvocationCallback cb;

// This is OK:
cb = cv::FunctionToInCa< int (int,double), my_function>::Call; 

// Compile error: type mismatch:
cb = cv::FunctionToInCa< int (int,int), my_function>::Call;

// Compile error: parameter count mismatch:
cb = cv::FunctionToInCa< int (double), my_function>::Call;
```

As a side-note: that's not to say macros are absolutely evil - combining macros with this API can often greatly reduce the amount of typing needed to create bindings. For example, i often do the following in client-side code:

```
#define M2I cv::MethodToInCa
#define C2I cv::ConstMethodToInCa
#define F2I cv::FunctionToInCa
#define CATCHER cv::InCaCatcher_std
```

# Code Demos and Sample Bindings #

More sources of information and code demos:

  * The `ConvertDemo.?pp` and `test.js` files in this source tree's root directory give a very thorough demonstration, showing off just about every feature the library offers.
  * The [XTo API](V8Convert_XTo.md) provides a _really easy_ interface into all of the function/property bindings.
  * [V8Convert\_Overloading](V8Convert_Overloading.md) goes into detail about how to overload script-side functions to multiple native functions.
  * [V8Convert\_Shell](V8Convert_Shell.md) offers a simple way to bootstrap v8 support into a client application, providing all the v8 features basic apps needs.
  * [V8Convert\_ClassCreator](V8Convert_ClassCreator.md) offers a simple, customizable class-binding mechanism.

The [addons  directory](http://code.google.com/p/v8-juice/source/browse/#svn/convert/addons) of the [V8Convert](V8Convert.md) source tree includes some sample v8 bindings created using this API. They include:

  * A [ByteArray class](V8Convert_ByteArray.md) for handling binary data in JS space.
  * The [JSPDO](JSPDO.md) database access abstraction API.
  * A basic [socket API](V8Convert_Socket.md).
  * An [i/o library](V8Convert_Whio.md).

Each of the "addons" includes a basic v8 shell app which includes the JS bindings provided by that addon, making it simple to try out the code before including it into one's own project (on many Unix-like platforms, at least).