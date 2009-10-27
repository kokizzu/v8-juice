// EXPERIMENTAL/INCOMPLETE!
#include <sstream>

#include <v8/juice/ClassWrap.h>
#include <v8/juice/forwarding.h>
#include <v8/juice/overloading.h>

#include <unistd.h> // sleep(3)

#ifndef CERR
#include <iostream> /* only for debuggering */
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

namespace v8 { namespace juice { namespace convert {
} } } // namespace v8::juice::convert

/** A class for testing ClasWrap. */
struct BoundNative
{
public:
    static bool enableDebug;
private:
    static size_t instcount;
#define DBGOUT if(BoundNative::enableDebug) CERR
    int propi;
public:
    double publicProperty;
    //int publicProperty;
    BoundNative()
        : propi(13),
          publicProperty(42.24)
    {
        ++instcount;
    }
    explicit BoundNative( int i, double d = 42.24 )
        : propi(i),
          publicProperty(d)
    {
        ++instcount;
    }
    virtual ~BoundNative()
    {
        --instcount;
    }
    bool ptr( BoundNative const * b )
    {
        DBGOUT << "BoundNative[@"<<(void const *)this<<"]->ptr("<<(void const *)b<<")\n";
        return 0 != b;
    }
    BoundNative * getPtr();
#if 0
    {
        return this;
    }
#endif
    static size_t InstanceCount()
    {
        return instcount;
    }
    int getInt() const { return this->propi; }
    void setInt( int i ) { this->propi = i; }
    bool overload() const { return true; }
    int overload(int i) { return i; }
    double overload(int i, double d) { return d; }
    virtual std::string toString() const
    {
        std::ostringstream os;
        os << "[Object BoundNative@"<<(void const *)this<<']';
        return os.str();
    }
    void tryRoundaboutApproach()
    {
        DBGOUT << "BoundNative[@"<<(void const *)this<<"]::tryRoundaboutApproach()\n";
    }
    Handle<Value> toString2( Arguments const & argv )
    {
#if 0
        typedef convert::MemFuncForwarder<0> MF;
        return MF::Call( this, &BoundNative::tryRoundaboutApproach, argv );
#else
        return convert::CastToJS( this->toString() );
#endif
    }

    static void SetupClass( v8::Handle<v8::Object> dest );

};

namespace v8 { namespace juice {
    namespace cw
    {
        template <> struct DebugLevel<BoundNative> : Opt_Int<2> {};

        template <>
        struct ToNative_SearchPrototypesForNative<BoundNative>
            : Opt_Bool<true>
        {};

        template <>
        struct AllowCtorWithoutNew<BoundNative>
            : Opt_Bool<false>
        {};

        template <>
        struct InternalFields<BoundNative>
            : InternalFields_Base<BoundNative>//,4,2>
        {};

#if 1
        template <>
        struct Inheritance<BoundNative>
            : Inheritance_Base<
            BoundNative
            //,std::string // should fail to compile
            //,BoundNative // should work
            >
        {};
#endif
    } // namespace cw
} }
size_t BoundNative::instcount = 0;
bool BoundNative::enableDebug = v8::juice::cw::DebugLevel<BoundNative>::Value > 2;
    
class BoundSub : public BoundNative
{
public:
    BoundSub()
    {
        DBGOUT << "BoundSub() this=@"<<(void const *)this << '\n';
    }
    BoundSub( int a, double d = 191.919 )
        : BoundNative(a, d)
    {
        DBGOUT << "BoundSub("<<a<<", "<<d<<") this=@"<<(void const *)this << '\n';
    }
    virtual ~BoundSub()
    {
        DBGOUT << "~BoundSub() this=@"<<(void const *)this << '\n';
    }
    virtual std::string toString() const
    {
        std::ostringstream os;
        os << "[Object BoundSub@"<<(void const *)this<<']';
        return os.str();
    }
};

////////////////////////////////////////////////////////////////////////
// Set up our policy classes...
namespace v8 { namespace juice { namespace cw
{

    template <>
    struct RuntimeOps<BoundNative> :
        RuntimeOpsBasic<BoundNative, &BoundNative::SetupClass >
    {
    };

    template <>
    struct Factory<BoundNative>
        : Factory_CtorForwarder<BoundNative,
                                convert::TypeList<
            convert::CtorForwarder0<BoundNative>,
            convert::CtorForwarder2<BoundNative,int,double>
            >
        >
    {
    };

    template <> struct DebugLevel<BoundSub> : Opt_Int<2> {};

    using namespace v8::juice;
    template <>
    struct Factory<BoundSub> :
    Factory_CtorForwarder<BoundSub,
                          convert::TypeList<
        convert::CtorForwarder0<BoundSub>,
        convert::CtorForwarder1<BoundSub,int>,
        convert::CtorForwarder2<BoundSub,int,double>
        >
    >
    {};

    /**
       The rest of the policies will be set up via supermacros below.
    */
} } } // v8::juice::cw


/************************************************************************
    Decide which policy set we want to use. For these particular
    tests, all of these policies should work (if they don't, it's a
    bug). But some use cases will call for a specific policy set.

    It is important that all bound subtypes of a bound parent class
    use the same policy set! Yes, this is tedious, but the approach
    shown below seems to work fairly well.
************************************************************************/

////////////////////////////////////////////////////////////////////////
// Set ONE of the following to a true value to select that ClassWrap
// policy set:
#if 0
// #  warning "Using JuiceBind policies!"
#  define USING_JUICEBIND_POLICIES
#  define CLASSWRAP_POLICY_HEADER <v8/juice/ClassWrap_JuiceBind.h>
#include CLASSWRAP_POLICY_HEADER
#elif 1
// #  warning "Using TwoWay policies!"
#  define USING_TWOWAY_POLICIES
#  define CLASSWRAP_POLICY_HEADER <v8/juice/ClassWrap_TwoWay.h>
#include CLASSWRAP_POLICY_HEADER
#else
// #  warning "Using default policies!"
#  define USING_DEFAULT_POLICIES
#  define CLASSWRAP_POLICY_HEADER <v8/juice/ClassWrap-JSToNative.h>
#endif



////////////////////////////////////////////////////////////////////////
// Import policies...
// Import the ClassWrap policy set for BoundNative...
#define CLASSWRAP_BOUND_TYPE BoundNative
#if !defined(USING_DEFAULT_POLICIES)
#  define CLASSWRAP_BOUND_TYPE_NAME "BoundNative"
#endif
#include CLASSWRAP_POLICY_HEADER
#if defined(USING_DEFAULT_POLICIES)
  JUICE_CLASSWRAP_CLASSNAME(BoundNative,"BoundNative")
#endif

// Import the ClassWrap policy set for BoundSub...
#define CLASSWRAP_BOUND_TYPE BoundSub
#if !defined(USING_DEFAULT_POLICIES)
#  define CLASSWRAP_BOUND_TYPE_NAME "BoundSub"
#endif
#if defined(USING_TWOWAY_POLICIES) || defined(USING_JUICEBIND_POLICIES)
#  define CLASSWRAP_BOUND_TYPE_INHERITS BoundNative
#endif
#include CLASSWRAP_POLICY_HEADER

// Inherit some of the options policies for BoundSub
#define CLASSWRAP_BOUND_TYPE BoundSub
#define CLASSWRAP_BOUND_TYPE_INHERITS BoundNative
#include <v8/juice/ClassWrap-InheritOptions.h>


// And define BoundSub's JS-side name:      
#if defined(USING_DEFAULT_POLICIES)
   JUICE_CLASSWRAP_CLASSNAME(BoundSub,"BoundSub")
#endif


/************************************************************************
   Down here is where the runtime setup parts of the bindings take place...
************************************************************************/

BoundNative * BoundNative::getPtr()
{
#if 0
        typedef v8::juice::cw::Extract<BoundNative> XT;
        BoundNative const * x = XT::VoidToNative(this);
        CERR << "BoundNative::getPtr() this@"<<(void const *)this
             << ", x@"<<(void const *)x<<'\n';
#endif
        return this;
}

std::string BoundNative_version()
{
    return "alphalpha 0.0.1";
}
void BoundNative_doSomething(std::string const &x)
{
    DBGOUT << "doSomething(\""<<x<<"\")\n";
}

size_t BoundNative_doSomething2(std::string const &x)
{
    DBGOUT << "doSomething2(\""<<x<<"\")\n";
    return 42;
}

v8::Handle<v8::Value> BoundNative_destroy( v8::Arguments const & argv )
{
    DBGOUT << "BoundNative_Destroy()\n";
    return convert::CastToJS( cw::ClassWrap<BoundNative>::DestroyObject(argv.This()) );
}
void BoundNative_overload()
{
    DBGOUT << "BoundNative_overload()\n";
    return;
}
int BoundNative_overload( int i )
{
    DBGOUT << "BoundNative_overload("<<i<<")\n";
    return i;
}
double BoundNative_overload( int i, double d )
{
    DBGOUT << "BoundNative_overload("<<i<<", "<<d<<")\n";
    return d;
}
v8::Handle<v8::Value> BoundNative_overload( v8::Arguments const & argv )
{
    DBGOUT << "BoundNative_overload(v8::Arguments)\n";
    v8::juice::convert::StringBuffer msg;
    msg << "BoundNative_overload("<<argv.Length()<<" Arguments)";
    return msg;
}

void BoundNative::SetupClass( v8::Handle<v8::Object> dest )
{

    //         typedef Inheritance<BoundNative> Inherit;
    //         {
    //             Inherit x;
    //         }
    using namespace v8;
    using namespace v8::juice;
    HandleScope scope;
    typedef BoundNative N;
    typedef cw::ClassWrap<N> CW;
    CW & cw( CW::Instance() );
    DBGOUT <<"Binding class "<<CW::ClassName::Value()<<"...\n";
    cw.Set("foo",String::New("this is foo"));
    cw.Set("toString2", convert::InvocationCallbackMember<N,&N::toString2>::Call );
    typedef convert::InvocationCallbackCreator ICC;
    typedef convert::MemFuncInvocationCallbackCreator<N> ICM;
    cw.Set( "toString",
            //ICC::M0::Invocable<N,std::string,&N::toString>
            ICM::M0::Invocable<std::string,&N::toString>
            );
    cw.Set( "getInt",
            //ICC::M0::Invocable<N,int,&N::getInt>
            ICM::M0::Invocable<int,&N::getInt>
            );
    cw.Set( "setInt",
            ICM::M1::Invocable<void,int,&N::setInt>
            );
    cw.Set( "ptr",
            //ICC::M1::Invocable<N,bool,N const * ,&N::ptr>
            ICM::M1::Invocable<bool,N const * ,&N::ptr>
            //convert::TMemFuncForwarder<N,1>::Invocable<bool,N const * ,&N::ptr>
            //ICC::M1::InvocableVoid<N,bool,N const * ,&N::ptr>
            );
#if defined(USING_TWOWAY_POLICIES)
    // If JSToNative isn't specialized, we should get a compile-time error here:
    cw.Set( "getPtr",
            ICM::M0::Invocable<N*,&N::getPtr >
            //ICC::M1::InvocableVoid<N,bool,N const * ,&N::ptr>
            );
#endif
    cw.Set( "overload",
            convert::OverloadInvocables<
            convert::TypeList<
            convert::InvocableConstMemFunc0<N,bool,&N::overload>,
            //convert::InvocableFunction0<void,BoundNative_overload>,
            //convert::DiscardInvocableReturnVal<
              convert::InvocableMemFunc1<N,int,int,&N::overload>
            //>
            ,            
            //convert::InvocableFunction1<int,int,BoundNative_overload>,
            convert::InvocableMemFunc2<N,double,int,double,&N::overload>,
            //convert::InvocableFunction2<double,int,double,BoundNative_overload>,
            convert::InvocableCallback<-1, BoundNative_overload>
            //convert::
            > >::Invocable );
    //typedef convert::PropertyBinder<N> PB;
    typedef CW::PB PB;
    v8::Handle<v8::ObjectTemplate> cwproto = cw.Prototype();
    //PB::BindGetterSetter<int,&N::getInt,void,int,&N::setInt>( "myInt", cwproto );
    cw.BindGetterSetter<int,&N::getInt,void,int,&N::setInt>( "myInt" );
    //PB::BindGetter<int,&N::getInt>( "intGetter", cwproto );
    cw.BindGetter<int,&N::getInt>( "intGetter" );
    //PB::BindMemVar<double,&N::publicProperty>( "publicProperty", cwproto );
    cw.BindMemVar< double, &N::publicProperty >( "publicProperty" );
    //PB::BindMemVarRO<double,&N::publicProperty>( "publicPropertyRO", cwproto );
    cw.BindMemVarRO<double,&N::publicProperty>( "publicPropertyRO" );

    PB::BindStaticVar<bool,&N::enableDebug>( "debug", cwproto );
    PB::BindStaticVarRO<bool,&N::enableDebug>( "debugRO", cwproto );
        
    v8::InvocationCallback FH;
    FH =
        ICC::F0::Invocable<std::string,BoundNative_version>
        //ICC::Invocable<std::string,BoundNative_version>
        ;
#define JFH v8::FunctionTemplate::New(FH)->GetFunction()
    cw.Set( "version", JFH );

    FH = ICC::F1::Invocable<void,std::string const &,BoundNative_doSomething>;
    FH = ICC::F1::InvocableVoid<size_t,std::string const &,BoundNative_doSomething2>;
    FH = ICC::F1::Invocable<size_t,std::string const &,BoundNative_doSomething2>;
    cw.Set( "doSomething", JFH );

    cw.Set( "destroy", CW::DestroyObject );
    //cw.Set( "destroy", BoundNative_destroy );

    FH = ICC::F0::Invocable<size_t,BoundNative::InstanceCount>;
    cw.Set( "instanceCount", JFH );
    cw.CtorTemplate()->Set( "instanceCount", JFH );
    cw.CtorTemplate()->Set( "supportsInheritance",
                            convert::CastToJS(cw::ToNative_SearchPrototypesForNative<BoundNative>::Value) );
    FH = ICC::F1::Invocable<unsigned int,unsigned int,::sleep>;
    cw.Set( "sleep", JFH );
#if 0
    v8::Handle<v8::Function> ctor = cw.Seal();
    cw.AddClassTo( dest );
#endif
    // not yet possible:  PB::BindStaticVar<bool,&N::enableDebug>( "debug", ctor );

#undef JFH

#if 0
    {
        typedef cw::RuntimeOps<N>:: RO;
        RO::InstallClass( dest );
    }
#endif
    
#if 1 // try to create a few objects for testing/sanity checking...
    {
        //v8::HandleScope hscope;
        Handle<Object> jobj =
            cw.NewInstance(0,0)
            //ctor->NewInstance(0,0)
            ;
        N * bound = CW::ToNative::Value(jobj);
        DBGOUT << "bound (void *) == @"<<(void const *)bound<<'\n';
        DBGOUT << "bound (CastFromJS<T>(jsObj)) == @"<<convert::CastFromJS<N>( jobj )<<'\n';
        if( bound )
        {
            //Handle<Value> = MFF::Call( obj, BoundNative::toString
            void * exh = jobj->GetPointerFromInternalField(CW::InternalFields::NativeIndex);
            DBGOUT << "fetched void* == "<<(const void *)exh<<'\n';
        }
        if(0)
        {
            v8::HandleScope hs;
            int level = 1;
            v8::Local<v8::Value> proto = jobj->GetPrototype();
            for( ; ! proto.IsEmpty() && proto->IsObject(); ++level )
            {
                DBGOUT << "Prototype level "<<level<<'\n';
                proto = Local<Object>( v8::Object::Cast(*proto) )->GetPrototype();
            }
        }

        if(1)
        {
            v8::Handle<v8::Object> so = cw::FindHolder<N>( jobj, bound );
            DBGOUT << "FindHolder() == [" << convert::CastFromJS<std::string>( so ) << "]\n";
        }

        DBGOUT << "BoundNative::InstanceCount() == "<<BoundNative::InstanceCount()<<'\n';
        CW::DestroyObject(jobj);
        DBGOUT << "BoundNative::InstanceCount() == "<<BoundNative::InstanceCount()<<'\n';
#if 0
        if(0 && bound)
        {
            Handle<Object> j2 = CW::CastToJS::Value( bound );
            DBGOUT << "JW::CastToJS::Value(jobj) == "<<convert::CastFromJS<std::string>(j2)<<'\n';
        }
#endif

        if(1)
        { ////////////////////////////////////////////////////////////////////////
            // subclassing tests...
            typedef BoundSub BS;
            typedef cw::ClassWrap<BS> WBS;
            WBS &b( WBS::Instance() );
            //b.Inherit( cw );
            b.InheritNative( cw );
            //NativeSubtypeLookup<BoundNative>::RegisterSubclass<BS>();
            //typedef cw::NativeInheritance<BoundNative> NIT;
            //NIT::RegisterSubclass<BS>();

            b.Seal();
            b.AddClassTo(dest);

            v8::Handle<Value> sj = b.NewInstance(0,0);
            BS * s = convert::CastFromJS<BS>( sj );
            DBGOUT << "BoundSub == "<<(void const *) s<<'\n';
            if( s )
            {
                WBS::DestroyObject(sj);
            }
        }
    }
#endif
    if(0)
    {
        typedef convert::TypeList<> LT0;
        typedef convert::TypeList<int> LT1;
        typedef convert::TypeList<LT0,LT1> LT2;
        typedef convert::TypeList<LT0,LT1,LT2> LT3;
#define X(TL) CERR << "convert::TypeListSize<"<<# TL<<">::Value == "<<convert::TypeListSize<TL>::Value<<'\n'
        X(LT0);
        X(LT1);
        X(LT2);
        X(LT3);
#undef X
    }
    DBGOUT <<"Binding done.\n";
    return;
}
#undef DBGOUT

