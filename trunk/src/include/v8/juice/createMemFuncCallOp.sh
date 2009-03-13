#!/bin/bash
########################################################################
# a very, very ugly hack to generate code for the ClassBinder API.
# It generates MemFuncCallOp${1} and friends.
count=${1-0}

test "$count" -gt 0 || {
    echo "Usage: $0 NUMBER (>0) [COMMAND=BindMemFunc]"
    exit 1
}

command=${2-BindMemFunc}

tmplsig="typename WrappedType, typename RV, "
ttlist="WrappedType, RV,"
tmplsigV="typename WrappedType, void, "
aTDecl=""
aTParam=""
at=0
callop=MemFuncCallOp${count}

function makeLists()
{
    for (( at = 0; at < count; at = at + 1)); do
    #echo "at=$at"
	local AT="A${at}"
	tmplsig="${tmplsig} typename ${AT},"
	ttlist="${ttlist} ${AT},"
	tmplsigV="${tmplsigV} ${AT},"
	aTDecl="${aTDecl} typename ${AT}"
	aTParam="${aTParam} ${AT}"
	test $at -ne $((count-1)) && {
	    aTDecl="${aTDecl}, "
	    aTParam="${aTParam},"
	}
    done
    #tmplsig="${tmplsig} RV (WrappedType::*Func)(${aTParam})";
    funcSig="(WrappedType::*Func)(${aTParam}) "
    tmplsig="typename WrappedType, typename RV, ${aTDecl}, RV ${funcSig}";
    tmplsigV="typename WrappedType, ${aTDecl}, void ${funcSig}";
    tmplspecV="WrappedType, void, ${aTParam}, void ${funcSig}"
}

function makeOps1()
{
    ####################
    # Primary ${callop} template:
    echo "/** Member function call forwarder for member functions taking ${count} arguments. */"
    echo "template < ${tmplsig} >"
    echo -e "struct ${callop}\n{"
    cat <<EOF
enum { Arity = $count };
typedef WrappedType Type;
static Handle<Value> Call( Type * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("${callop}::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("${callop}::Call(): wrong argument count!"));
	return convert::CastToJS( (RV) (obj->*Func)(
EOF
for (( at = 0; at < count; at = at + 1)); do
    local T="A${at}"
    echo -en "\t\tconvert::CastFromJS< ${T} >(argv[${at}])"
    test $at -ne $((count-1)) && echo "," || echo
done
echo -e ") );\n}\n};"
        #echo aTDecl=$aTDecl
        #echo aTParam=$aTParam
        ####################
        # void specialization for ${callop} template:
echo "/** Specialization for member functions taking ${count} args and returning void. */"
echo "template < ${tmplsigV} >"
echo -e "struct ${callop}< WrappedType,void,${aTParam}, Func >\n{"
cat <<EOF
enum { Arity = $count };
typedef WrappedType Type;
static Handle<Value> Call( Type * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("${callop}::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("${callop}::Call(): wrong argument count!"));
	(obj->*Func)(
EOF
for (( at = 0; at < count; at = at + 1)); do
    local T="A${at}"
    echo -en "\t\tconvert::CastFromJS< ${T} >(argv[${at}])"
    test $at -ne $((count-1)) && echo "," || echo
done
echo -e "\t\t);\n\t\treturn ::v8::Undefined();\n\t}\n};"
echo
return
}

function makeOpsConst()
{
    ####################
    # Primary ${callop} template:
    echo "/** Member function call forwarder for const member functions taking ${count} arguments. */"
    echo "template < typename WrappedType, typename RV, ${aTDecl}, RV ${funcSig} const >"
    echo -e "struct ${callop}<const WrappedType, RV,${aTParam}, Func>\n{"
    cat <<EOF
enum { Arity = $count };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("${callop}::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("${callop}::Call(): wrong argument count!"));
	return convert::CastToJS( (RV) (obj->*Func)(
EOF
for (( at = 0; at < count; at = at + 1)); do
    local T="A${at}"
    echo -en "\t\tconvert::CastFromJS< ${T} >(argv[${at}])"
    test $at -ne $((count-1)) && echo "," || echo
done
echo -e ") );\n}\n};"
        #echo aTDecl=$aTDecl
        #echo aTParam=$aTParam
        ####################
        # void specialization for ${callop} template:
echo "/** Specialization for const member functions taking ${count} args and returning void. */"
    echo "template < typename WrappedType, ${aTDecl}, void ${funcSig} const >"
    echo -e "struct ${callop}< const WrappedType,void,${aTParam}, Func >\n{"
cat <<EOF
enum { Arity = $count };
typedef const WrappedType Type;
static Handle<Value> Call( Type const * obj, Arguments const & argv )
{
    if( ! obj ) return ThrowException(String::New("${callop}::Call(): Native object is null!"));
    else if( argv.Length() < Arity ) return ThrowException(String::New("${callop}::Call(): wrong argument count!"));
	(obj->*Func)(
EOF
for (( at = 0; at < count; at = at + 1)); do
    local T="A${at}"
    echo -en "\t\tconvert::CastFromJS< ${T} >(argv[${at}])"
    test $at -ne $((count-1)) && echo "," || echo
done
echo -e "\t\t);\n\t\treturn ::v8::Undefined();\n\t}\n};"
echo
return
}

function makeBindFunc()
{
cat <<EOF
/**
   Overload requiring a WrappedType CONSTNESS member function
   taking ${count} args and returning RV.
*/
template < typename RV, ${aTDecl}, RV ${funcSig} CONSTNESS >
ClassBinder & BindMemFunc( char const * name )
{
    typedef Detail::${callop}< CONSTNESS WrappedType, RV, ${aTParam}, Func > Caller;
    this->Set(name, Detail::MemFuncCallOp< Caller, RV >::Call );
    return *this;
}
EOF

}


makeLists
false && {
    echo funcSig=${funcSig}
    echo tmplsig=${tmplsig}
    echo tmplsigV=${tmplsigV}
    echo tmplspecV=${tmplspecV}
}


case $command in
    BindMemFunc)
        makeBindFunc | sed -e 's/CONSTNESS //g'
	makeBindFunc | sed -e 's/CONSTNESS /const /g'
	;;
    MemFuncCallOps)
	    makeOps1
	    makeOpsConst
	    ;;
    *)
	echo "Unhandled command: ${command}"
	exit 2
    ;;
esac
