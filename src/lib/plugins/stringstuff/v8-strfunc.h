#ifndef CODE_GOOGLE_COM_V8_JUICE_PLUGIN_STRSTUFF_H_INCLUDED
#define CODE_GOOGLE_COM_V8_JUICE_PLUGIN_STRSTUFF_H_INCLUDED 1
#include <stdarg.h>

#include <v8.h>

namespace v8 {
namespace juice {
namespace plugin {
namespace str {

    /**
       This function installs a JS-side sprintf() function into the
       given object. See the sprintf() docs for full details.
    */
    ::v8::Handle< ::v8::Value > SetupPlugin( ::v8::Handle< Object > target );


}}}} // namespaces

#endif /* CODE_GOOGLE_COM_V8_JUICE_PLUGIN_STRSTUFF_H_INCLUDED */
