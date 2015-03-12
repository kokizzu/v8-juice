See also: [V8Convert](V8Convert.md), [V8Convert\_Overloading](V8Convert_Overloading.md), [V8Convert\_PropertyBinding](V8Convert_PropertyBinding.md), [V8Convert\_FunctionBinding](V8Convert_FunctionBinding.md)



# Introduction #

If the [overwhelming](V8Convert_ClassCreator.md) [number](V8Convert_Overloading.md) of [options](V8Convert_TipsAndTricks.md) for [binding functions, methods](V8Convert_FunctionBinding.md), [variables, and member properties](V8Convert_PropertyBinding.md) in cvv8 has got you down, here's something which might brighten your day: you only need to learn a few simple templates.

On the rainy day of 20110724, during a testing/debugging chat session, Coen Champman suggested a "kinda weird" syntax for doing "x-to-v8" conversions, particularly in the context of function and variable binding. His suggestion was to wrap all function-to-X conversions in a single template named `Function`, saying that we could pass a tag type as the first argument to the template to specify exactly which conversion we wanted to perform.

Kinda strange, i know, but his pasted-in hypothetical usage example intrigued me, and 50 lines of code later we had a working version. (All of the code is compile-time only - no additional runtime overhead is added.)

In client code it abstractly looks something like this:

```
#include "cvv8/XTo.hpp"

...
int aBoundInt = 3;
void test_xto_bindings()
{
    v8::InvocationCallback c;
    v8::AccessorGetter g;
    v8::AccessorSetter s;

    using namespace cvv8;

    // Function-to-X conversions:
    c = FunctionTo< InCa, int(char const *), ::puts>::Call;
    c = FunctionTo< InCaVoid, int(char const *), ::puts>::Call;
    g = FunctionTo< Getter, int(void), ::getchar>::Get;
    s = FunctionTo< Setter, int(int), ::putchar>::Set;

    // Var-to-X conversions:
    g = VarTo< Getter, int, &aBoundInt >::Get;
    s = VarTo< Setter, int, &aBoundInt >::Set;
    typedef VarTo< Accessors, int, &aBoundInt > VarGetSet;
    g = VarGetSet::Get;
    s = VarGetSet::Set;

    typedef BoundNative T;

    // Member Var-to-X conversions:
    g = MemberTo< Getter, T, int, &T::publicInt >::Get;
    s = MemberTo< Setter, T, int, &T::publicInt >::Set;
    typedef MemberTo< Accessors, T, int, &T::publicInt > MemAcc;
    g = MemAcc::Get;
    s = MemAcc::Set;
    

    // Method-to-X conversions:
    c = MethodTo< InCa, T, void (), &T::doFoo >::Call;
    c = MethodTo< InCaVoid, T, void (), &T::doFoo >::Call;
    g = MethodTo< Getter, const T, int (), &T::getInt >::Get;
    s = MethodTo< Setter, T, void (int), &T::setInt >::Set;
    // Const methods:
    c = MethodTo< InCa, const T, int (), &T::getInt >::Call;
    c = MethodTo< InCaVoid, const T, int (), &T::getInt >::Call;
    

    // Functor-to-X conversions:
    typedef MyFunctor F;
    c = FunctorTo< InCaVoid, F, bool () >::Call;
    c = FunctorTo< InCa, F, bool (int) >::Call;
    g = FunctorTo< Getter, F, bool () >::Get;
    s = FunctorTo< Setter, F, void (bool) >::Set;
}
```

While the syntax is a bit unconventional, i find it to be quite readable, writable, and easy to remember how to use. And we can extend it as new conversions come up, e.g. `MethodTo<Monkey,... >`.

One of the nicest aspects of this API (as with the other conversion-related APIs) is that if it compiles than it will work (barring any bugs or undiscovered corner-cases in cvv8's code). The majority of the hand-written code in cvv8 are simple dispatching proxies around generated code which blindly converts and passes on (or back) values. Since those features are known to work well, simply being able to compile is a sign that everything is okay from the library's point of view.

The above example incidentally shows, however indirectly, essentially every feature [V8Convert](V8Convert.md) offers except for [class binding](V8Convert_ClassCreator.md):

  * Converting functions, methods, and functors to `v8::InvocationCallback`, `v8::AccessorGetter`, and `v8::AccessorSetter`. (That's 9 different conversion combinations, or 11 if we consider that `const` methods are internally handled separately and setters are "essentially never" const.)
  * Proxying JS property access directly to a native non-member variable or static non-function member (`VarTo<>`), or to non-static non-function members (`MemberTo<>`).
  * Type conversions happen implicitly all throughout the function/property-binding process, so those are also (indirectly) demonstrated here.

Though that might not look like much, the templates shown above provide the majority of the JS/C++ binding features we need for typical applications. If these templates are "too generic" to handle a specific use case, virtual _mountains_ (well, _hills_) of lower-level cvv8 features can assist clients in writing their own conversions and bindings. There _are_ many binding cases which the library cannot handle directly, but (A) it handles many millions, if not billions, of combinations out of the box and (B) it provides many tools which can simplify the creation of hand-written bindings.

# Conventions and Options #

The demonstration in the first section shows everything one really needs to know about using this API. Here we'll go into a bit more detail...

Header file: "[cvv8/XTo.hpp](http://code.google.com/p/v8-juice/source/browse/convert/include/cvv8/XTo.hpp)"

The common conventions used by this API are:

The conversion templates are all named like `XyzTo`. This documentation often refers to the `Xyz` part of the name as the "target" or "type" of the conversion, e.g. the "target" for a conversion is a `Function` (`FunctionTo<>`), `Method` (`MethodTo<>`), etc.

The first argument to the template is a "tag" type. The tag types each correspond to a distinctly different type of conversion (see below for the whole list).

The template arguments after the first (the tag type) depend on the main target of the conversion (e.g. a function, method, or functor), and for a given target, the template arguments after the tag type are the same regardless of the tag's value. i.e. the interface is identical for `FunctionTo<InCa...>` and `FunctionTo<Getter,...>`, though specific conversions might place limitations on the range of values/types for a given conversion. e.g. a "getter" function (`FunctionTo<Getter,..>`) must take no arguments and return a convertible-to-JS, non-void type.

Note that not all targets support all tags. e.g. there no `FunctionTo<Accessors,...>` because a function signature can only ever match either a getter _or_ a setter. Likewise, `VarTo` can only bind property accessors (Tags=`Getter`, `Setter`, or `Accessors`), not `v8::InvocationCallback`s (Tags=`InCa`, `InCaVoid`). Trying to use invalid conversions will lead to compile-time errors.

The set of conversions can be summarized in the following set of template signatures:

  * **Functions**: <br><code>FunctionTo&lt; Tag, signature, function_ptr_matching_signature [, UnlockV8=a_bool_value]&gt;</code>
<ul><li><b>Class methods</b>: (use const-qualified <code>T</code> for const methods!)<br><code>MethodTo&lt; Tag, T, signature, method_ptr_matching_signature [, UnlockV8=a_bool_value]&gt;</code>
</li><li><b>Functors</b>: <br><code>FunctorTo&lt; Tag, signature_of_const_operator() [, UnlockV8=a_bool_value]&gt;</code>
</li><li><b>Non-member variables</b> or <b>static member variables</b>:<br><code>VarTo&lt; Tag, PropertyType, ptr_to_property&gt;</code>
</li><li><b>Non-static member variables</b>:<br><code>MemberTo&lt; Tag, T, PropertyType, ptr_to_member_property&gt;</code></li></ul>

You can read more about the meaning of the <code>UnlockV8</code> option on the <a href='V8Convert_FunctionBinding.md'>V8Convert_FunctionBinding</a> page. (It has a sane default for <i>most</i> cases, and can <i>usually</i> be ignored.)<br>
<br>
The expanded <code>XyzTo</code> templates evaluate to types which contain a specific function, depending on the conversion tag type:<br>
<br>
<ul><li><code>InCa</code> and <code>InCaVoid</code>: <code>Call()</code>, implementing the <code>v8::InvocationCallback</code> interface. <code>InCaVoid</code> is like <code>InCa</code> but explicitly does <i>not</i> invoke a conversion of the native return type. This means it can be used to bind functions/methods with void or non-convertible types, or to ignore return values which we don't <i>want</i> to convert for some reason.<br>
</li><li><code>Getter</code>: <code>Get()</code>, implementing the <code>v8::AccessorGetter</code> interface.<br>
</li><li><code>Setter</code>: <code>Set()</code>, implementing the <code>v8::AccessorSetter</code> interface.<br>
</li><li><code>Accessors</code>: as for <code>Getter</code> and <code>Setter</code> combined.</li></ul>

The complete array of legal target/tag combinations is demonstrated in the example at the top of this page.