

**See also:** [V8Convert](V8Convert.md), which provides a standalone, header-only distribution of the API described below (but uses a different namespace).

# An introduction to the `v8::juice::convert` API #

(These docs also apply to the [v8::convert API](V8Convert.md) - just mentally replace the namespace `v8::juice::convert` with `v8::convert`.)

Juice comes with an API for casting between JS types and native types. While v8 of course provides convenience routines like `Value::IntegerValue()` and `Value::ToString()`, that leaves us with two limitations:

  * The types of conversions are defined by v8's implementors and are not extensible.
  * We have to know the exact conversion routine we want to call. Believe it or not, when our code uses typedefs (e.g. numeric types of unspecified sizes, possibly from external libraries) we cannot safely know which conversion to rely upon.

And thus we let C++ templates decide it for us...

```
#include <v8/juice/convert.h>

...
namespace cv = ::v8::juice::convert;
using namespace v8;

int i = cv::CastFromJS<int>(someJSVal);
std::string s = cv::CastFromJS<std::string>(someJSVal);
double d = cv::CastFromJS<double>(someJSVal);
...

Handle<Value> v1 = cv::CastToJS( myInteger );
Handle<Value> v2 = cv::CastToJS( "hi, world" );
Handle<Value> v3 = cv::CastToJS( myType ); // we can add custom conversions!
```


The `CastToJS()` and `CastFromJS()` routines are very small functions which
delegate all work to template a specialization of the class `NativeToJS<>`
resp. `JSToNative<>`. Specializations are already installed for all of the
base POD types (except (`char const *`) - see below), and clients can provide
their own to do any conversion they like. An example of this is shown on [the ClassBinder page](ClassBinder.md).

While such a framework might at first seem like syntactic sugar, it has important implications. Namely, we can write generic algorithms which are ignorant of the conversions they are doing. As an example, the library provides a pair of template functions which can act as property getters/setters for _arbitrary_ convertible member variables. The entire implementation is about 10 lines of code, half of which is error checking. Thus, instead of writing a custom getter/setter pair for each member variable, we can get away with a single, trivial implementation. From there, we can _"let the templates to the typing"_ (if i may now coin a phrase).

For those averse to template arguments, there are a number of convenience instantiations of the `JSToNative` and `NativeToJS` template which can be used like this:

```
  int32_t i = JSToInt32(...);
  std::string s( JSToStdString(...) );
  bool b = JSToBool(...);
  return DoubleToJS(42.42);
```

but then we're again stuck with the limitation of hard-coded conversion operator names, so these aren't very useful in generic algoriths.

## Out-of-the-box Conversions ##

The following native types are supported out of the box by the conversion API:

  * `int16_t`, `int32_t`, `int64_t`
  * `uint16_t`, `uint32_t`, `uint64_t`
  * `double`
  * `bool`
  * `long`, `unsigned long`
  * `std::string`
  * Conversion _from_ (`char const *`) to JS-string, but not the reverse (see below for why).
  * `std::list<T>`, `std::vector<T>`, provided `T` is convertible.
  * `std::map<K,T>`, provided `K` and `T` are convertible.

(`int8_t`/`uint8_t` are missing due to template type collisions with (`char const *`) on platforms where `int8_t` is `char`. The `long` internally requires special handling on some platforms to avoid a collision with one of the fixed-size integer types.)

# Implementing custom conversions #

## Achtung: not for noobs ##

Forewarning: customizing the type conversions requires a fair amount of
knowledge in how to use and abuse C++ templates. DO NOT just blindly
copy/paste some example from this page without understanding what it does and
how it gets triggered. One tiny little typing mistake can lead to literally
hundreds of lines of compiler error messages (or weird link errors), and it
often takes patience and experience to be able to decipher such problems
("dammit, it's a pointer, not a reference").

Good luck!

## Custom native-to-JS conversions ##

To make our own types support work with `CastToJS()` we have to provide
a template specialization for `NativeToJS<>`. The example shown here works
in conjunction with [WeakJSClassCreator](CreatingJSClasses.md) to do the conversion:

```
namespace v8 { namespace juice { namespace convert {
template <>
struct NativeToJS<MyType>
{
    Handle<Value> operator()( MyType const * n ) const
    {
        // This impl works together with WeakJSClassCreator():
        return ::v8::juice::WeakJSClassCreator<MyType>::GetJSObject( n );
    }
};
} } } // namespaces
```


The important thing is that this specialization is available (visible in code) before
any client code calls (however indirectly) `CastToJS<MyType>(...)`. If it is not,
the compiler will try to pick up another instantiation of the template. With luck
it will fail loudly, but (and this depends on the implicit native conversions possible for `MyType`) it may pick up a different, inappropriate converter. A good place for such a routine is directly after the class' declaration, though other circumstances may require a later placement. e.g. when used together with `WeakJSClassCreator`, the casting specializations must come after the `WeakJSClassCreatorOps<>` specialization has been declared (they are all normally defined where the are declared (inline) since they are all templates).

### Converting Enums ###

Converting from an arbitrary enum to JS via these APIs requires this small hack:

```
template <>
struct NativeToJS<MyEnum> : NativeToJS<int32_t> {};
```

After that, `CastToJS(MY_ENUM_VALUE)` will convert the enum entry into a JS integer.

## Custom JS-to-native conversions ##

The converse of `NativeToJS<>` is of course `JSToNative<>`:

```
namespace v8 { namespace juice { namespace convert {
template <>
struct JSToNative<MyType>
{
    typedef MyType * ResultType;
    ResultType operator()( Handle<Value> const & h ) const
    {
       // This impl works together with WeakJSClassCreator():
       return ::v8::juice::WeakJSClassCreator<MyType>::GetNative( h );
    }
};
} } } // namespaces
```

There's not much else to say about it, really.

There is no need to set up `T &` or `T const &` specializations. When
asked to use `JSToNative<T&>`, a generic `X &` specialization will be
triggered which will actually use `JSToNative<T>` to get a pointer to
the object, and will then (if the pointer is not 0) dereference it to
form a reference. If `JSToNative<T&>` cannot get a native pointer,
it will throw a _native_ exception (it has no other choice except
to crash). Thus client code which requires conversions to references
must use `try`/`catch` blocks in order to keep potential exceptions
for propagating through v8 (which can be fatal to the JS engine).

While the conversions for the built-in numeric types (and certain
POD-like types, e.g. `std::string`) return their natives by
value, in practice we need `JSToNative<T>` to return pointers for any
client-side bound types.

# The `(char const *)` problem #

The following will work as expected:

```
Handle<Value> v = CastToJS("this is a (char const *)");
```

This won't:

```
char const * v = CastFromJS<char const *>(myJSVal);
```

The problem here is ownership and lifetime of the string bytes. v8 does not document them (and also can't realistically provide any requirements which we would need for this conversion). We _can_ write a conversion routine for this, and it would probably work 99% of the time, but its results would technically be undefined. As a workaround, client code should use `std::string` instead (at least for `CastFromJS()`) or implement `JSToNative<>` and/or `NativeToJS<>` specializations for their string type of choice. The `CastToJS()` function is overloaded for (`char const *`) and "does the right thing", but `CastFromJS()` cannot handle such strings as transparently as it can most other types.

# Forwarding to Free- or Member Functions #

The `v8::juice::convert` namespace contains functions named `FwdToFuncN()`, where `N` is the number of arguments they expect, which use the casting framework to marshal calls between JS and native functions. Alternately, one can bind native functions to JS objects as described on the BindingFunctions page.

# Silly Examples #

Here are some examples of the things one can do with the type conversions API:

```
std::list<int> li;
... populate list ...
Handle<Value> v( CastToJS( li ) ); // Array of Integers
```

That will work for _any_ list-contained type which is supported by the casting API. Ergo, a list containing lists also works:

```
typedef list<std::string> StringList;
typedef list<StringList> ListList;
ListList li;
... populate li ...
Handle<Value> v( CastToJS( li ) ); // Array of Arrays of strings
```

And that means that we can use `ClassBinder` to bind to member functions which take or return arguments of those types, and or member variables of those types, too.

And how about:

```
StringList sl = CastFromJS<StringList>( someJSVal );
```

That will result in a populated list, but only if `someJSVal` is-an object of Array type. If it is, each entry in the array will be cast using `CastFromJS<StringList::value_type>()`.

See <tt><a href='http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/convert.h'>convert.h</a></tt> for how such conversions are implemented (search that file for `ListT`).