#include <stdexcept>
/**
   See the docs below...
*/
#if ! defined(CLASSWRAP_BOUND_TYPE)
#error "You must define CLASSWRAP_BOUND_TYPE before including this file!"
#endif

#if !defined(DOXYGEN)
namespace v8 { namespace juice { namespace convert {

    /**
       JSToNative<> specialization which uses
       v8::juice::ClassWrap_ToNative::Value() to convert from JS
       handles to CLASSWRAP_BOUND_TYPE objects.
       
       See NativeToJS<> for full details.
    */
    template <>
    struct JSToNative< CLASSWRAP_BOUND_TYPE >
    {
        typedef ::v8::juice::ClassWrap_ToNative< CLASSWRAP_BOUND_TYPE >  Cast;
        typedef Cast::NativeHandle ResultType;
        /**
           If h is empty or !h->IsObject() then 0 is returned, else it
           returns the result of passing that object to Cast::Value().

           Achtung: even though this is really a convenience function,
           all specializations should implement it, as this function
           is 
        */
        ResultType operator()( Handle<Value> const & h ) const
        {
            if( h.IsEmpty() || !h->IsObject() )
            {
                return 0;
            }
            else
            {
                Handle<Object> const jo( Object::Cast( *h ) );
                return Cast::Value(jo);
            }
        }
    };

} } }
#endif // DOXYGEN

#undef CLASSWRAP_BOUND_TYPE
