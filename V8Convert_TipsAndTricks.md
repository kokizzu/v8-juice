Here we'll try to cover various tips and tricks for using [V8Convert](V8Convert.md)...



# Some Terminology... #

Terms used throughout the docs which might not have an obvious meaning:

  * `cvv8` is the namespace for the [V8Convert](V8Convert.md) API, and is also a nickname for the library.
  * `cv` is a namespace alias for `cvv8` used by much of the wiki/example/demo code. Most example code does not show the alias declaration.
  * `InCa` is short for `v8::InvocationCallback`. It also refers to _classes_ in the `cvv8` API which implement a static `Call()` function which itself implements the `InvocationCallback` interface.

# Type Conversion Tips and Tricks #

## Converting enum Values to JS ##

When converting JS values to C/C++ enum values, e.g. when binding functions which take enum parameters, we normally need to add the following custom conversion:

```
template <>
cvv8::JSToNative<MyEnumType> : cvv8::JSToNative<int32_t> {};
```

When converting enum values to JS, e.g. when binding functions which _return_ enum values, we need the converse:

```
template <>
cvv8::NativeToJS<MyEnumType> : cvv8::NativeToJS<int32_t> {};
```

But to be honest i've never tried that one. "It should work."

In theory we could use `int64_t` as the base for enums with 64-bit values, but v8 does not directly support 64-bit integers, so we would likely have problems with that at some point.

# Function-binding #

See [V8Convert\_FunctionBinding](V8Convert_FunctionBinding.md).

# Binding Native Variables/Properties to JS #

See [V8Convert\_PropertyBinding](V8Convert_PropertyBinding.md).