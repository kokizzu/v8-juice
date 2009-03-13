#!/bin/bash
########################################################################
# a very, very ugly hack to generate code for the ClassBinder API.
# It generates MemFuncCallOp${1} and friends.
count=${1-0}

test "$count" -gt 0 || {
    echo "Usage: $0 NUMBER (>0)"
    exit 1
}

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

function doIt()
{
    local count=$1
    if [[ "x$2" = "x" ]]; then
    ####################
    # Primary ${callop} template:
    {
	echo "/** Member function call forwarder for const member functions taking ${count} arguments. */"
	echo "template < ${tmplsig} >"
	echo -e "struct ${callop}\n{"
	cat <<EOF
	enum { Arity = $count };
	typedef WrappedType Type;
	static Handle<Value> Call( Type CONSTNESS * obj, Arguments const & argv )
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
        echo "template < ${tmplsigV} >"
	echo -e "struct ${callop}< WrappedType,void,${aTParam}, Func >\n{"
	cat <<EOF
	enum { Arity = $count };
	typedef WrappedType Type;
	static Handle<Value> Call( Type CONSTNESS * obj, Arguments const & argv )
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
}
    return
fi # ${callop} gen

    # ClassBinder::BindMemFunc:
    tmplsig=$(echo $tmplsig | sed -e 's/typename WrappedType,//')
cat <<EOF
/**
   Overload requiring a CONSTNESS WrappedType member function taking ${count} args and returning RV.
*/
template < ${tmplsig} >
ClassBinder & BindMemFunc( char const * name )
{
    typedef Detail::${callop}< WrappedType, RV, ${aTParam}, Func > Caller;
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
makeOps1
makeOpsConst
