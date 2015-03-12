

One of the minorly problematic things in integrating v8 into an application is the setup needed to get v8 initialized. We've got to create a context, a global object template, and a global object. We have to set up those objects (in the right order) and possibly make sure that we have the proper locking in place (this is app-specific and the documentation on the topic is sparse).

If the setup is not done in the exact right order then v8 will produce weird failures, like bound functions not being visible in JS, or might even outright crash (when using debug builds of v8, it will tell us why it crashed, but non-debug builds crash without warning). But since the v8 developers are apparently prohibited from documenting their code, the only way one finds out things like this is through trial and error. (@v8 developers: yes, i will continue to mercilessly bitch about the lack of documentation until there is actually some usable documentation. Give me a contract and i'll write it for you!)

The `V8Shell` class tries to fill that gap and simplify the "bootstrapping" process, making it a snap (instead of "Aw, snap!" ;) to get a v8 environment initialized.

Note that [the V8Shell source code](http://code.google.com/p/v8-juice/source/browse/convert/include/v8/convert/V8Shell.hpp) has no dependencies on any other [V8Convert](V8Convert.md) code (it just happens to be part of that source tree). This means it can be copied and used as-is in arbitrary v8-using clients.

To demonstrate, the following simple program accepts a JS script filename on the command line and runs it:

```
#include "cvv8/V8Shell.hpp"
#include <cassert>

int main(int argc, char const * const * argv) {
    assert( argc >= 2 ); // obviously, user-friendly programs don't do this!
    namespace cv = cvv8;
    cv::Shell shell(NULL, argc, argv);
    //  ^^^^^ Shell is a convenience typedef for V8Shell<>, by the way.
    // All argv args after "--" are passed on to the script in the global
    // 'arguments' array. e.g. ./shell script.js -- these go to JS
    shell.SetupDefaultBindings();
    // If we want to customize the environment (e.g. add more functions)
    // we should do it here.
    try {
        v8::TryCatch tryCatch;
        char const * fname = argv[1];
        v8::Handle<v8::Value> rc( shell.ExecuteFile( fname, &tryCatch ) );
        if( rc.IsEmpty() ) { // exception was reported by shell already
            return 2;
        }
    }
    catch(std::exception const & ex) {
        std::cerr << "Caught a std::exception: " << ex.what() << '\n';
        return 3;
    }
    catch(...) {
        std::cerr << "A non-std::exception was thrown! Srsly.\n";
        return 4;
    }
    return 0;
}
```

For a more complete demonstration
see [shell.cpp](http://code.google.com/p/v8-juice/source/browse/convert/addons/shell-skel/shell.cpp).

`V8Shell` is intended to be used in single-JS-context applications. It is untested in multi-context environments (e.g. multiple `V8Shell` instances) and will possibly not work properly on such setups due (at least partly) to is locking requirements.