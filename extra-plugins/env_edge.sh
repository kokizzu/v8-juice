# to be sourced by shell code. Sets up the PATH and libpath
# to use the edge-branch v8-juice library and shell.

edge=$(cd ../branches/edge >/dev/null 2>&1 && pwd)
if [ x = "x$edge" ]; then
    echo "'edge' branch dir not found in ../branches!"
else
    PATH="$edge/src/client/shell:$edge:$PATH"
    LD_LIBRARY_PATH="$edge/src/lib/juice:${LD_LIBRARY_PATH}"
    export PATH LD_LIBRARY_PATH
fi
unset edge
alias m='make -e'

