#include <stdexcept>
/**
   See the docs below...
*/
#if ! defined(CLASSWRAP_BOUND_TYPE)
#error "You must define CLASSWRAP_BOUND_TYPE before including this file!"
#endif

namespace v8 { namespace juice { namespace convert {

    /**
       See NativeToJS< CLASSWRAP_BOUND_TYPE > for full details.
    */
    template <>
    struct JSToNative< CLASSWRAP_BOUND_TYPE >
    {
        typedef ::v8::juice::ClassWrap_ToNative< CLASSWRAP_BOUND_TYPE >  Cast;
        typedef Cast::NativeHandle ResultType;
        ResultType operator()( Handle<Value> const & h ) const
        {
            return Cast::Value(h);
        }
    };

} } }

#undef CLASSWRAP_BOUND_TYPE
