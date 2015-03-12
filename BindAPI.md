2011-01-21: this API has mostly fallen out of use, surpassed by new binding mechanisms. It is a remnant of the initial draft of v8-juice, which was based off of older code for the SpiderMonkey JS engine.



# An introduction to the `v8::juice::bind` API #

The `bind` API, defined in [bind.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/bind.h) is not often used by client code, but is
used by the v8-juice [type conversions API](ConvertingTypes.md) and, by extension, the ClassBinder API.

This API allows us to bind arbitrary `(void const *)` to arbitrary native pointers. To bind JS objects to natives, the JS objects need to have some sort of unique ID which we can associate with the native object. In v8, only full-fledged Objects can have any sort of unique ID, and the only way to get one is to store it in an "internal field" (a v8 feature) of the JS object. The only option v8 provides for is to store a (`void *`) in the object by adding an `v8::External` value to the object. That lookup key (the `void *`) can then be used by client applications.

The bind API maps those keys to bound object, keeping the full type of the bound object, which avoids all casting and allows us to make conversions from JS-to-Native 100% typesafe.

## The functions ##

The `bind` API has only a small handlful of public functions, all of which are templatized on the native type:

  * `bool BindNative<T>(JSObject,T*)` (in several convenience overloads) maps a JS object to a native object.
  * `bool UnbindNative<T>(JSObject)` (with several convenience overloads) unmaps the binding.
  * `T * GetBoundNative<T>(JSObject)` performs a type-safe conversion of search for the `T` object mapped to the given JS object.

They are fully documented in [bind.h](http://code.google.com/p/v8-juice/source/browse/trunk/src/include/v8/juice/bind.h).

# Questions and Answers #

## Why store the JS-to-native binding? ##

From Rob Tsuk:

> "Why do we need to keep a map of js object to bound native object?"

Good question. For two reasons:

1) We could leave out the mapping, but this would require us to do all conversions via casting the External::Value() (`void *`) values into their native objects, and i'm uniformly against casting unless absolutely necessary. The v8-juice bind/conversion framework doesn't use (void-to-T) casts anywhere except for one unforuntane place in cleanup.h. Instead it uses the mappings to provide a type-safe conversion from JS (via (`void const *`) key) to native (`T*`). Yes, it's slower, but it's also 100% typesafe.

2) As a basis for the JSToNative() and NativeToJS() operations, so that they can convert arbitrary client-side types between JS/Native-space (a necessary component for binding native funcs to JS callables). In the SpiderMonkey engine each JS value (object or not) has a unique ID which we can use to map _any_ JS value to any native. In v8, the only types for which we can get a unique ID refering to that JS object are Objects and Externals, and (unfortunately) the only way to get such an ID is to store the ID as a (`void *`) via an External handle. If v8 had a stable, per-JS-val unique ID then we would be mapping those IDs to the natives instead. It would be convenient to use their GC-pool addresses as their uids, but the v8 GC allows objects to be moved, so their pointer values are not stable (and they're not available in the public API, anyway, AFAIK).

Okay, maybe 3 reasons:

3) If i'm not mistaken, if we used void-to-T casting then JS-side inheritance of native bound classes could not be implemented because the native cast has no way of knowing if inter-mixed JS/Native inheritance is involved. The mapping (incidentally, it turns out) indirectly gives us a way to walk up a JS class heirarchy to find a bound native virtual base class mapped to the same object pointer. That makes it possible for a JS class to inherit a native T, for another JS class to inherit that first JS class, and for the second JS class to still be-a T object for purposes of virtual method lookup for methods bound via the base native class. i found that without this extra heirarchy-climbing, member functions inherited from bound natives (but not re-implemented in the subclass) could not be found when called via objects which had multiple levels of inheritance between themselves and their native parent. i don't remember all of the details - they're hidden away somewhere in the private parts of WeakJSClassCreator (i hate that class name, by the way).

#3 was not an original intention of the design, but it turns out the design allows us to implement "proper" cross-JS/Native virtual member lookup with "relatively" little effort.