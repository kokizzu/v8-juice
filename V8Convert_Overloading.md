

# Introduction #

[V8Convert](V8Convert.md) has a fairly powerful function overloading mechanism. When binding JS functions to C++ (as described on the [V8Convert](V8Convert.md) page) we can, using templates, create overloads which dispatch to a particular native function depending on any criteria we care to apply to the passed-in arguments.

When we only care about overloading based on the arity (number of arguments), `ArityDispatch` and friends make it easy to do so. However, using more advanced techniques we can dispatch to a different native callback using logic similar to the following:

  * If the call has 3 arguments then...
    * If the first one is a string, call `f1()`.
    * Else if the first arg is a number call `f2()`.
    * Else call `f3()`...
  * Else if the call has 2 arguments and both are functions, call `f4()`.
  * Else if the call has 4 arguments and the 3rd is an object, call `f5()`.
  * ...

_ad nauseum_. You get the idea.

As it turns out, overloading is just a specific case of the more general problem of argument validation. The overloading-related templates can also be used to validate argument types for non-overloaded functions, such that they will fail with an error if JS code passes in the wrong type(s). e.g. If we bind the function `void myfunc(v8::Handle<v8::Object>)`, we can tell cvv8 (when we create the binding) to perform the validation, such that calling `myfunc()` from JS will never cause a non-Object handle to be passed to the native function.

There is still some work to be done in simplifying the generation of certain rules, especially for the simpler cases (which require more code than they should).

# Overloading by Arity #


The API has support for overloading multiple `InvocationCallback`s based on their arity (the number of arguments they require). We have two approaches to doing this, but only one is shown here (the one which is far easier to use for most cases). See the `ArityDispatch` class for a simplified form which only works with one or two overloads (a single-function "overload" is an easy way to ensure that the caller provides only the required number of arguments!).

The `ArityDispatchList` template takes a type-list of function binding templates (or something interface-compatible) and combines them into a single `v8::InvocationCallback` function which can dispatch script-side calls to a different native function depending on the number (not the types) of arguments.

It looks like this:

```
// Overload 4 variants of a member function:
typedef cv::ArityDispatchList< CVV8_TYPELIST((
        cv::MethodToInCa<T, void (), &T::overload0>,
        cv::MethodToInCa<T, void (int), &T::overload1>,
        cv::MethodToInCa<T, void (int,int), &T::overload2>,
        cv::MethodToInCa<T, void (v8::Arguments const &), &T::overloadN> // fallback for N args
))> MyOverloads;
// Note that the order of the list entries is irrelevant for _most_ purposes,
// the exception being that one taking v8::Arguments will always match any
// arity, so it must come at the end of the list (since it trumps any
// coming after it).

// Get the InvocationCallback proxy which does the arity-based dispatching:
v8::InvocationCallback cb = MyOverloads::Call;
```

Note that only one line of that code is evaluated at runtime - the rest is all done at compile-time. The above generates a whole family of `v8::InvocationCallback` functions (at least (overload count + 1) of them, though there are a couple others hidden behind the scenes) and combines them into a single `v8::InvocationCallback` dispatcher function, `MyOverloads::Call()`.

There is actually a limit to how many overloads you can provide in a single `ArityDispatchList`, but the lower limit is _at least_ 10 (and the library can be built to support many more than that). (If you're overloading 10+ variants of one function then the design probably needs to be reconsidered!)


# Overloading Based on Arbitrary Logic #

By creating predicate/callback pairs using templates, we can create `v8::InvocationCallback` functions which dispatch to different native functions depending on near-arbitrary logic. For example, the API provides enough type information so that we can build templates which can dispatch based on the types of the values passed in at runtime. That means, amongst other things, that we can bind overloads of functions which have different-sized numeric types:

```
void f( int16_t );
void f( int32_t );
void f( double );
```

We can then dispatch to one of those based on the numeric value passed in from JS. If the value fits in `int16_t` then we call that overload, else if it fits in `int32_t` we call that overload, else we fall back to the `double` overload. (For the curious, `std::numeric_limits<T>` can be used to find out the minimum/maximum value of a native numeric type, and we can use that to figure out if a given JS numeric value will "fit" in a given native numeric type.)

This approach to overloading appears to be superior to `v8::Signature` in every way, in particular that it is type-safe, can be extended to include client-provided types, extends to essentially any number of combinations, _and_ has no runtime memory costs (the dispatcher is built at compile-time). Though it does have an average of O(N/2) runtime overhead (N=number of rule functors compounded across all overloads), because it walks through the rules (in their defined order) until it finds a match or reaches the end of the list (in which case it throws a JS-side exception), i don't believe this cost is any higher (or not significantly so) than the v8-internal cost of using `v8::Signature` (plus `v8::Signature` adds a runtime memory cost). If the overloading rules are packed into groups/hierarchies (as in the demonstration below), the average O(N/2) cost can be reduced, theoretically (when structured like a binary tree) to as low as O(log N).

This approach does have a couple limitations. Notably:

  * It is not a full-fledged parser. e.g. it cannot backtrack and try different branches in the rules set if it reaches the end of an inner branch without finding a match. It walks them linearly, branching into a subtree if a rule is actually a list of sub-rules.
  * Some potentially interesting rule-ordering optimizations are impossible because of the lack of back-tracking.

We could probably add back-tracking support by using an internally-handled native exception type to tell the higher-level scope that a rule subset failed to match. But using try/catch blocks for that sounds like a good deal of unneeded overhead. That might, incidentally, simplify the current end-of-list handling.
## The Big, Fat, Hairy Example ##

Here's a rather detailed example...

We're going to overload _10_ functions into a single callback, such that any calls with 5 or fewer arguments are unambiguously handled by one of these overloads, and all calls with 6 or more arguments will cause a JS-side exception to the thrown.

(Note that no sane application would have 10 overloads for the same function, but the following comes from library test code where we are trying to stress the API a bit.)

The callbacks:

```
// Takes any number of arguments:
v8::Handle<v8::Value> bogo_callback_arityN( v8::Arguments const & argv );

// Take only 1 argument:
int16_t bogo_callback_int16( int16_t v );
int32_t bogo_callback_int32( int32_t v );
double bogo_callback_double( double v );
int bogo_callback_array( v8::Handle<v8::Array> const & ar );
bool bogo_callback_object( v8::Handle<v8::Object> const & obj );
v8::Handle<v8::Value> bogo_callback_function( v8::Handle<v8::Function> const & f );

// This one is only dispatched when argv.Length()==2
int bogo_callback2( v8::Arguments const & argv );

// 3 arguments:
std::string bogo_callback_fsf( v8::Handle<v8::Function> const & f1,
                                char const * str,
                                v8::Handle<v8::Function> const & f2 )

v8::Handle<v8::Value> bogo_callback_fvf( v8::Handle<v8::Function> const & f1,
                                         v8::Handle<v8::Value> const &v,
                                         v8::Handle<v8::Function> const & f2 );
```

And here's how we do it:

```
using namespace v8::convert;

// Create some rules for the 1-arity overloads:

typedef PredicatedInCa< ArgAt_IsA<0,int16_t>,
                        FunctionToInCa<int16_t (int16_t), bogo_callback_int16>
> PredIsaInt16;

typedef PredicatedInCa< ArgAt_IsA<0,int32_t>,
                       FunctionToInCa<int32_t (int32_t), bogo_callback_int32>
> PredIsaInt32;

typedef PredicatedInCa< ArgAt_IsA<0,double>,
                        FunctionToInCa<double (double), bogo_callback_double>
> PredIsaDouble;

typedef PredicatedInCa< ArgAt_IsArray<0>,
                        FunctionToInCa<int (v8::Handle<v8::Array> const &), bogo_callback_array>
> PredIsaArray;

typedef PredicatedInCa< ArgAt_IsObject<0>,
                        FunctionToInCa<bool (v8::Handle<v8::Object> const &),
                                       bogo_callback_object>
> PredIsaObject;

typedef PredicatedInCa< ArgAt_IsFunction<0>,
                        FunctionToInCa<v8::Handle<v8::Value> (v8::Handle<v8::Function> const &),
                        bogo_callback_function>
> PredIsaFunction;

// Group the 1-arity rules into a PredicatedInCaDispatcher "container".
typedef PredicatedInCaDispatcher< CVV8_TYPELIST((
    // The order IS significant for overloads which can evaluate ambiguously,
    // e.g. Int16/Int32/Double.
    PredIsaFunction, PredIsaArray, PredIsaObject, PredIsaInt16, PredIsaInt32, PredIsaDouble
))> ByTypeOverloads;

// Create a parent rule which only checks ByTypeOverloads if called
// with 1 argument:
typedef PredicatedInCa< Argv_Length<1>, ByTypeOverloads > Group1;

// Set up some other logic paths...

// For 2 arguments:
typedef PredicatedInCa< Argv_Length<2>, InCaLikeFunc<int,bogo_callback2> > Group2;

// For 0 or 3-5 args
typedef PredicatedInCa<
    Argv_Or< Argv_Length<0>, Argv_Length<3,5> >,
    InCa<bogo_callback_arityN>
> GroupN;

// Special case for the weird (Function, cstring, Function) overload...
typedef Argv_AndN< CVV8_TYPELIST((
        Argv_Length<3>,
        ArgAt_IsFunction<0>,
        ArgAt_IsString<1>,
        ArgAt_IsFunction<2>
    ))> Is_Func_String_Func;
typedef PredicatedInCa< Is_Func_String_Func,
        FunctionToInCa< std::string (
                            v8::Handle<v8::Function> const &,
                            char const *,
                            v8::Handle<v8::Function> const &),
                        bogo_callback_fsf
                    >
> PredFSF;


// Special case for the weird (Function, Value, Function) overload...
typedef Argv_AndN< CVV8_TYPELIST((
        Argv_Length<3>,
        ArgAt_IsFunction<0>,
        ArgAt_IsFunction<2>
    ))> MatchesFunc_Value_Func;
typedef PredicatedInCa< MatchesFunc_Value_Func,
        FunctionToInCa< v8::Handle<v8::Value> (
                            v8::Handle<v8::Function> const &,
                            v8::Handle<v8::Value> const &,
                            v8::Handle<v8::Function> const & ),
                        bogo_callback_fvf
                    >
> PredFVF;

// Now create the "top-most" callback, which performs the above-defined
// dispatching at runtime:
typedef PredicatedInCaDispatcher< CVV8_TYPELIST((
        PredFSF, PredFVF, Group1, Group2, GroupN
))> AllOverloads;

// Everything up to here is done at compile-time, by the way.
// Now we can do what we've been working towards all along:
// getting the dispatcher callback:

v8::InvocationCallback cb = AllOverloads::Call;
```

You might be asking, "and that actually works?" Here's some JS code used with that configuration:

```
function testPredicateOverloads()
{
    print("Testing out the experimental predicate-based overloading...");
    var b = new BoundSubNative();
    try {
        var rc;
        rc = b.bogo();
        asserteq( 0, rc );
        b.bogo(1 << 8);
        b.bogo(1 << 17);
        b.bogo((1<<31) * (1 << 10));
        b.bogo([1,2,3]);
        b.bogo({});
        b.bogo(1,"hi");
        b.bogo({},1,"hi");
        b.bogo(function(){
            // reminder: if we call b.bogo(arguments.callee)
            // from here we will crash with endless recursion.
            print("JS-side callback function.");
        });
        var msg;
        msg = {a:1};
        rc = b.bogo( function(){print("FVF: argv[0]()");},
                     msg,
                     function(){print("FVF: argv[2]()");});
        asserteq( 'object', typeof rc );
        asserteq(msg,rc);

        msg = "(char const *)";
        rc = b.bogo( function(){print("FSF: argv[0]()");},
                     msg,
                     function(){print("FSF: argv[2]()");});
        asserteq( 'string', typeof rc );
        asserteq(msg, rc );

        assertThrows( function() { b.bogo(1,2,3,4,5,6); } );
    }
    finally { b.destroy(); }
}
```

That generates something like the following on my machine:

```
ConvertDemo.cpp:151:bogo_callback(): Arg count=0. Dispatching based on predicate rules...
ConvertDemo.cpp:73:bogo_callback_arityN(): Arg count=0
Assertion OK: 0 == 0
ConvertDemo.cpp:151:bogo_callback(): Arg count=1. Dispatching based on predicate rules...
ConvertDemo.cpp:80:bogo_callback_int16(): int_16 overload: 256.
ConvertDemo.cpp:151:bogo_callback(): Arg count=1. Dispatching based on predicate rules...
ConvertDemo.cpp:85:bogo_callback_int32(): int_32 overload: 131072.
ConvertDemo.cpp:151:bogo_callback(): Arg count=1. Dispatching based on predicate rules...
ConvertDemo.cpp:90:bogo_callback_double(): double overload: -2.19902e+12.
ConvertDemo.cpp:151:bogo_callback(): Arg count=1. Dispatching based on predicate rules...
ConvertDemo.cpp:97:bogo_callback_array(): array overload. length=3
ConvertDemo.cpp:151:bogo_callback(): Arg count=1. Dispatching based on predicate rules...
ConvertDemo.cpp:102:bogo_callback_object(): object overload.
ConvertDemo.cpp:151:bogo_callback(): Arg count=2. Dispatching based on predicate rules...
ConvertDemo.cpp:116:bogo_callback2(): native this=@0x82a56f8, arg count=2
ConvertDemo.cpp:151:bogo_callback(): Arg count=3. Dispatching based on predicate rules...
ConvertDemo.cpp:73:bogo_callback_arityN(): Arg count=3
ConvertDemo.cpp:151:bogo_callback(): Arg count=1. Dispatching based on predicate rules...
ConvertDemo.cpp:109:bogo_callback_function(): function overload.
JS-side callback function.
ConvertDemo.cpp:151:bogo_callback(): Arg count=3. Dispatching based on predicate rules...
ConvertDemo.cpp:139:bogo_callback_fvf(): (Function, Value, Function) overload.
FVF: argv[0]()
ConvertDemo.cpp:144:bogo_callback_fvf(): [object Object]
FVF: argv[2]()
Assertion OK: object == object
Assertion OK: [object Object] == [object Object]
ConvertDemo.cpp:151:bogo_callback(): Arg count=3. Dispatching based on predicate rules...
ConvertDemo.cpp:126:bogo_callback_fsf(): (Function, cstring, Function) overload.
FSF: argv[0]()
ConvertDemo.cpp:130:bogo_callback_fsf(): (char const *)
FSF: argv[2]()
Assertion OK: string == string
Assertion OK: (char const *) == (char const *)
ConvertDemo.cpp:151:bogo_callback(): Arg count=6. Dispatching based on predicate rules...
Assertion OK: Got expected exception: Error: No predicates in the argument dispatcher matched the given arguments (arg count=6).
ConvertDemo.hpp:233:~BoundSubNative(): @0x82a56f8 is destructing
ConvertDemo.hpp:53:~BoundNative(): @0x82a56f8 is destructing.
```


## Argument Type/Range Validation ##

One (admittedly unforeseen) use of the overloading API is the ability to use it for performing argument validation for non-overloaded functions. When used this way, cvv8 makes sure the argument predicate(s) match before calling the bound function. Thus the function's body can be assured that the precondition logic passed before the function was called.

### Examples of Type/Range Validation ###

Here's an example taken from [V8Convert\_Whio](V8Convert_Whio.md), where we have an `IODev.seek()` function which natively looks like:

```
whio_size_t IODev::seek( whio_off_t, int );
```

Because the 2nd argument (the `int`) must have one of only a small subset of values (`SEEK_SET`, `SEEK_CUR`, or `SEEK_END`), we can use/abuse "overloading" to ensure that it is only called if JS passes us one of those values.

First we need a custom predicate to determine if the 2nd argument matches one of the special `SEEK_xxx` values:

```
template <int I>
struct ArgAt_IsWhence : ArgumentsPredicate
{
    inline bool operator()( v8::Arguments const & args ) const
    {
        if( args.Length() <= I ) return false;
        else
        {
            v8::Handle<v8::Value> const & v(args[I]);
            if( ! ValIs<int32_t>()(v) ) return false;
            else switch( v->Int32Value() )
            {
              case SEEK_SET:
              case SEEK_CUR:
              case SEEK_END:
                  return true;
              default:
                  return false;
            }
        }
    }
};
```

And now we can bind our `seek()` function using strong type/range checking, such that the native function will never be called unless the JS calls pass values which are both type- and range-legal:

```

// Create the argument validation dispacher:
typedef PredicatedInCaDispatcher< CVV8_TYPELIST((
  PredicatedInCa<
      // The validation rules:
      // Call-time types/values must match:
      // (whio_off_t, one of: SEEK_(SET,END,CUR))
      Argv_AndN< CVV8_TYPELIST((
          Argv_Length<2>, ArgAt_IsA<0,whio_off_t>, ArgAt_IsWhence<1>
      )) >,
      // The function/method to bind to:
      MethodTo<InCa, IODev, whio_size_t (whio_off_t,int), &IODev::seek>
  >
)) > Seek;

// Get the generated v8::InvocationCallback:
v8::InvocationCallback cb = InCaCatcher_std< Seek >::Call;
// PS: we use InCaCatcher_std b/c IODev::seek() can throw a std::exception.
```

_Yes_, it's ugly, and i _am_ looking for a new naming scheme for the `PredicatedXyz` family of templates. But when this approach is used, we can apply near-arbitrary logic to the callback dispatching process which can prevent (or allow) the JS-initiated calls to pass on (or not) into their bound native equivalents.

What does all that really mean? It means that with the above binding in place, all of the following JS-side calls will fail with an argument validation exception:

```
dev.seek("1",whio.SEEK_SET); // 1st type is wrong
dev.seek(0,"hi"); // 2nd type is incorrect
dev.seek(1,17); // 2nd type is correct but it is not a SEEK_xxx value
```

We can use this sort of validation to check for `NULL` pointers in some cases as well, by applying a hypothetical `ArgAt_Is[Not]NULL<int,T>` predicate template (which does not exist but would be easy to do). In fact, the default `ArgAt_IsA<int,T>` specialization will behave that way for client-bound `T` types which cannot be converted from JS to C++, but the behaviour for (`char [const] *`) is different (`char *` is habitually problematic when it comes to conversions). Nonetheless, we could write a `NULL`-safe C-style string check like this:

```
template <int I>
struct ArgAt_IsNonNullCString : ArgumentsPredicate
{
    inline bool operator()( v8::Arguments const & args ) const {
        if( args.Length() <= I ) return false;
        else {
           v8::Handle<v8::Value> const & v(argv[I]);
           return !v->IsNull() && !v->IsUndefined();
        }
    }
};
```

The binding framework's support for conversions from JS-to-(`char const *`) has some notable limitations due to the lifetime/ownership of the C-string bytes (see [V8Convert\_FunctionBinding](V8Convert_FunctionBinding.md) for details), but the conversion specifies that JS `null` and `undefined` convert to `NULL` C-strings and that all other JS values will be converted using `v8::Value::ToString()`. Since the `Value::ToString()` operation presumably cannot return a `NULL` string (there is no such thing in JS!), we can safely assume that non-`null`/`undefined` JS values can be converted to non-`NULL` C strings.

# Forwarding and Overloading Constructors #

The `CtorForwarder` class assists in forwarding `v8::Arguments` objects to constructors. Usage examples:

```
// Typedefs for 3 different ctors:
typedef cv::CtorForwarder<MyType * ()> Ctor0;
typedef cv::CtorForwarder<MyType * (int)> Ctor1;
typedef cv::CtorForwarder<MyType * (argType1,argType2)> Ctor2;

// We can call these individually:
MyType * my = Ctor0::Call( argsObject );

// Or we can group them together and dispatch based on the number of arguments pass in:
typedef cv::CtorArityDispatcher<
        cv::Signature<MyType * (Ctor0, Ctor1, Ctor2)>
    > Dispatch;
MyType * my = Dispatch::Call( argsObject );
```

We can use `PredicatedCtorForwarder` (subject to eventual renaming!) to structure constructor overloads in the same way as we can when using `PredicatedInCa` for function overloads (as demonstrated in a previous section of this page).

Unlike most other bindings, the argument types do not need to match _exactly_. The types declared in the `CtorForwarder` instantiation will be used for purposes of converting the arguments from JS to native values. As long as those types can be implicitly converted to the types accepted by the constructor(s) then it should work okay. e.g. you can declare the forwarder as having an `int` argument even though the ctor really takes a `double`. (However, that would cause any doubles passed in from JS space to first be converted to an integer, which would lose precision.)

An additional property of this particular binding is that we can bind to constructors which have default values for some arguments. e.g. if a constructor has 2 required arguments and 2 more optional ones, we can bind it as a 2, 3, and/or 4-arg constructor.

Another unique property of these bindings is that it is possible for specific ctor overloads/delegates to return a subtype of the declared type. This can be used to wrap factory functions in the guise of constructor call.