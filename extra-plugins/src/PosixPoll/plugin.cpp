#include <v8/juice/plugin.h>
#include "PosixPoll.cpp"

static v8::Handle<v8::Value> plugin_init( v8::Handle<v8::Object> dest )
{
    Poller_init( dest );
    return dest;
}


V8_JUICE_PLUGIN_STATIC_INIT(plugin_init);
