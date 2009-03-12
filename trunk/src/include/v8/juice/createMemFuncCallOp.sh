#!/bin/bash

count=${1-0}

test "$count" -gt 0 || {
    echo "Usage: $0 NUMBER (>0)"
    exit 1
}

function doIt()
{
    local count=$1
cat <<EOF
/** Member function call forwarder for member functions taking ${count} arguments. */
EOF
    local tmplsig="typename WrappedType, typename RV, "
    local ttlist="WrappedType, RV,"
    local tmplsigV="typename WrappedType, void, "
    local aTDecl=""
    local aTParam=""
    at=0
    local callop=MemFuncCallOp${count}
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
    funcSig="(WrappedType::*Func)(${aTParam} )"
    tmplsig="typename WrappedType, typename RV, ${aTDecl}, RV ${funcSig}";
    tmplsigV="typename WrappedType, ${aTDecl}, void (WrappedType::*Func)(${aTParam})";
    tmplspecV="WrappedType, void, ${aTParam}, void ${funcSig}"
    ####################
    # Primary ${callop} template:
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
cat <<EOF
		) );
	}
};
EOF

    #echo aTDecl=$aTDecl
    #echo aTParam=$aTParam

    ####################
    # void specialization for ${callop} template:
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
cat <<EOF
		);
	    return ::v8::Undefined();
	}
};
EOF
    echo

    return
    # ClassBinder::AddMemFunc:
    tmplsig=$(echo $tmplsig | sed -e 's/typename WrappedType,//')
cat <<EOF
/**
   Overload requiring a WrappedType member function taking ${count} args and returning RV.
*/
template < ${tmplsig} >
ClassBinder & AddMemFunc( char const * name )
{
    typedef Detail::${callop}< ${ttlist} > Caller;
    this->Set(name, Detail::MemFuncCallOp< Caller >::Call );
    return *this;
}
EOF

}

max=${2-${count}}
for ((at = ${count}; at <= ${max}; at = at + 1)); do
    doIt $at
done


