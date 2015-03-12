The source tree has a relatively elaborate setup, with ease-of-expansion being a primary goal. It's based primarily off of the tree used in the libs11n project, and this structure has proven to be very flexible over the years.

# Directory structure #

It's laid out like this and ordered more or less by their dependencies:

```
src/              All source code is under here...
  +-- include/
    +-- v8/
      +-- juice/  Public API headers go somewhere under here
                  (private headers are normally in the source dir)
  +-- lib/
    +-- juice/    The core library

  +-- client/     Library client apps go under here
    +-- shell/    JS shell with juice functionality added on
    +-- sample/   Sample applications
    ...
doc/              Docs go here
  +-- doxygen/    Doxygen-generated docs
  +-- manual/     Library manual
```

Any directories not listed there are probably part of the build process, or tools used by that process, and are not considered part of the library.

Dependencies between directories always run in one direction only. That is, circular dependencies between two modules or directories are not allowed. Circular deps only cause Grief and Frustration at some point. Implementation files which directly depend on each other are always put in the same module/directory.

# Header files #

When including headers in implementation files, the preferred syntax is:
```
#include <v8/juice/some_header.h> // or .hpp, or .hh, or whatever
```
and the build process should add `-Isrc/include` to the precompiler flags.

The exact file extension for header files is up for debate. When in doubt, stick with v8's convention (`.h` for headers and `.cc` for sources), but "borrowed code" often has other extensions. (Someone can go rename them if this really bothers them!)

# Namespaces #

The main library is in the `v8::juice` namespace. Code written for that library "should" be put in that namespace or a sub-namespace. Plugins and whatnot are free to use their own choice of namespaces, and namespaces are encouraged to avoid collisions.

# Coding standards, naming conventions, etc. #

These are conventions, not rules. And there are not many of them.

Coding style is a personal choice, and anyone who takes the time and effort to contribute is more than welcomed to use whatever style they like. 4-space, 8-space, or N-space indention, with tabs, without tabs, or whatever. If it compiles and links, i'm happy. And if it's ugly, i'll just tell xemacs to reindent it.

Naming conventions:

As a general rule, i try to follow v8's naming convention of CamelCaseWithLeadingCapital. Occasionally (or more often) code will be imported from other open-source sources, and that code does not always get changed just to accommodate naming conventions, especially if it will not be part of the public API.

Core library code will always go in the `v8::juice` namespace or a sub-namespace. Global-scope code is never allowed except as private/static details in an implementation file (and even then a namespace, even an anonymous one, is strongly preferred).

# Licensing #

The code contains parts which are released under several licenses. The majority of it follows the same license as v8: the "new-style" BSD license. Much of it is Public Domain. In no case will GPL, or other virually-licensed (MPL, QPL, etc.), code be allowed into the main tree (but is of course allowed for third-party plugins and such). The _LGPL_ is acceptable in certain contexts (e.g. plugins), but a less strict license is required for any core library code. Permissive, non-viral licenses such as Apache, MIT, zlib, etc., are generally acceptable for core library code.