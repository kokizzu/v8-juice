#if defined(NDEBUG)
#undef NDEBUG
#endif
#include <cassert>
// Copyright 2008 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/************************************************************************
This code is based off of the shell.cc sample application in the v8
source tree. It has been mangled to become the default shell for the
v8-juice project.

Some changes from the original shell:

- Binds most of the v8-juice add-on functions by default.

- If the argv list contains "--", all arguments after that are ignored
by the shell but passed on to each script which it runs. Each script
gets a global arguments object, where arguments[0] is the name of the
shell (it SHOULD be the name of the script, but it's not), and arguments
1..N are those after the "--"

 ************************************************************************/


#include <v8.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>


#include "MyNative.h"
#include <v8/juice/PathFinder.h>
//#include <v8/juice/forwarding.h>
//#include <v8/juice/ToSource.h>
#include <v8/juice/JuiceShell.h>
#include <v8/juice/ClassBinder.h>

namespace bind = ::v8::juice::bind;

v8::Handle<v8::Value> Quit(const v8::Arguments& args);
v8::Handle<v8::String> ReadFile(const char* name);

typedef v8::Handle<v8::ObjectTemplate> V8Object;
typedef v8::Local<v8::Object> V8LObject;
typedef v8::Handle<v8::Context> V8CxH;


using namespace v8;
using namespace ::v8::juice;


v8::juice::JuiceShell * ShellInstance = 0; 
int main(int argc, char * argv[])
{
#define JSTR(X) v8::String::New(X)
    if(0)
    { /** fuck - SetFlagsFromCommandLine() changes argv such that the
          "--" arg and those following it are stripped!
       */
        typedef std::vector<char *> AV;
        AV vargv(static_cast<size_t>(argc), 0);
        int i;
        for( i = 0; i < argc; ++i )
        {
            vargv[(unsigned int)i] = argv[i];
        }
        v8::V8::SetFlagsFromCommandLine(&i, &vargv[0], true);
    }
    else
    {
        // or we could use the undocumented 3rd arg:
        //v8::V8::SetFlagsFromCommandLine(&argc, argv, false);
    }
    {
        v8::HandleScope handle_scope;
        v8::juice::cleanup::CleanupSentry cleaner;
        v8::juice::JuiceShell shell("v8juice");
        ShellInstance = &shell;
        shell.ProcessMainArgv(argc,argv,1);
        shell.SetupJuiceEnvironment();
        
        if(1)
        {
            // my_test( context );
            //my_class_test( context );
            MyNative::SetupClass(shell.Context());
            //my_tosource(context);
            //my_bind_test( context );
        }
        bool run_shell = (argc == 1);
        v8::Locker tlocker;
        std::string const endofargs("--");
        for (int i = 1; i < argc; i++) {
            const char* str = argv[i];
            if( endofargs == str )
            {
                break;
            }
            else if (strncmp(str, "--", 2) == 0)
            {
                printf("Error: unknown flag %s.\n", str);
                return 2;
            } else if (strcmp(str, "-e") == 0 && (i + 1 < argc)) {
                // Execute argument given to -e option directly
                v8::HandleScope handle_scope;
                std::string source = (argv[i + 1] ? argv[i + 1] : "");
                if (!shell.ExecuteString(source, "unnamed"))
                {
                    return 1;
                }
                ++i; // skip eval'd string
                continue;
            } else {
                // Use all other arguments as names of files to load and run.
                v8::HandleScope handle_scope;
                v8::Handle<v8::Value> rc = v8::juice::IncludeScript( str );
                if (rc.IsEmpty())
                {
                    std::cerr << "Exiting because of exception via include().\n";
                    return 1;
                }
                continue;
            }
        }
        if (run_shell)
        {
            shell.AddGlobalFunc( "quit", Quit );
            v8::TryCatch jtry;
            shell.InputLoop( v8::juice::JuiceShell::StdinLineFetcher, &std::cout, &jtry );
            std::cout << std::endl;
        }
    }
    return 0;
#undef JSTR
}
// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}


// The callback that is invoked by v8 whenever the JavaScript 'quit'
// function is called.  Quits.
v8::Handle<v8::Value> Quit(const v8::Arguments& args) {
  // If not arguments are given args[0] will yield undefined which
  // converts to the integer value 0.
  int exit_code = args[0]->Int32Value();
  exit(exit_code);
  return v8::Undefined();
}



// Reads a file into a v8 string.
v8::Handle<v8::String> ReadFile(const char* name) {
  FILE* file = fopen(name, "rb");
  if (file == NULL) return v8::Handle<v8::String>();

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);

  char* chars = new char[size + 1];
  chars[size] = '\0';
  for (int i = 0; i < size;) {
    int read = fread(&chars[i], 1, size - i, file);
    i += read;
  }
  fclose(file);
  v8::Handle<v8::String> result = v8::String::New(chars, size);
  delete[] chars;
  return result;
}

