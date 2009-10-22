// EXPERIMENTAL/INCOMPLETE!
#include <sstream>

#include "ClassWrap.h"


#include <unistd.h> // sleep(3)

#ifndef CERR
#include <iostream> /* only for debuggering */
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

namespace v8 { namespace juice {
    /** A class for testing ClasWrap. */
    struct BoundNative
    {
    private:
        static size_t instcount;
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
        ~BoundNative()
        {
            --instcount;
        }
        bool ptr( BoundNative const * b )
        {
            CERR << "BoundNative[@"<<(void const *)this<<"]->ptr("<<(void const *)b<<")\n";
            return 0 != b;
        }
        static size_t InstanceCount()
        {
            return instcount;
        }
        int getInt() const { return this->propi; }
        void setInt( int i ) { this->propi = i; }
        std::string toString() const
        {
            std::ostringstream os;
            os << "[Object BoundNative@"<<(void const *)this<<']';
            return os.str();
        }
        void tryRoundaboutApproach()
        {
            CERR << "BoundNative[@"<<(void const *)this<<"]::tryRoundaboutApproach()\n";
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
    };
    size_t BoundNative::instcount = 0;

    template <>
    struct ClassWrap_ToNative<BoundNative> :
        ClassWrap_ToNative_StaticCast<BoundNative,
                                      true
                                      //false
                                      > {};

    
    template <>
    struct ClassWrap_ClassName<BoundNative>
    {
        static char const * Value()
        {
            return "BoundNative";
        }
    };

#if 1
    // Only used for testing some compile-time assertions:
    template <>
    struct ClassWrap_InternalFields<BoundNative> : ClassWrap_Opt_Int<1>
    {
        static const int NativeIndex = 0;
    };
#endif

#if 1
    template <>
    struct ClassWrap_Inheritance<BoundNative>
        : ClassWrap_Inheritance_Base<
        BoundNative
        //,std::string // should fail to compile
        //,BoundNative // should work
        >
    {};
#endif
    
    template <>
    struct ClassWrap_Factory<BoundNative>
    {
        typedef convert::TypeInfo<BoundNative>::Type Type;
        typedef convert::TypeInfo<BoundNative>::NativeHandle NativeHandle;
	static NativeHandle Instantiate( Arguments const &  /*argv*/,
                                         std::ostream & /* exceptionText */ )
	{
            NativeHandle x = new BoundNative;
            CERR << "BoundNative->Instantiate() == @"<<(void const *)x<<'\n';
            v8::V8::AdjustAmountOfExternalAllocatedMemory( sizeof(BoundNative) );
            return x;
	}
	static void Destruct( NativeHandle obj )
	{
            CERR << "BoundNative->Destruct() == @"<<(void const *)obj<<'\n';
            v8::V8::AdjustAmountOfExternalAllocatedMemory( -sizeof(BoundNative) );
            delete static_cast<NativeHandle>(obj);
	}
    };

}} // namespaces

#define CLASSWRAP_BOUND_TYPE v8::juice::BoundNative
#include "ClassWrap-CastOps.h"

namespace v8 { namespace juice {
    std::string BoundNative_version()
    {
        return "alphalpha 0.0.1";
    }
    void BoundNative_doSomething(std::string const &x)
    {
        CERR << "doSomething(\""<<x<<"\")\n";
    }
    size_t BoundNative_doSomething2(std::string const &x)
    {
        CERR << "doSomething2(\""<<x<<"\")\n";
        return 42;
    }

    v8::Handle<v8::Value> BoundNative_destroy( v8::Arguments const & argv )
    {
        CERR << "BoundNative_Destroy()\n";
        return convert::CastToJS( ClassWrap<BoundNative>::DestroyObject(argv.This()) );
    }

    void bind_my_native( v8::Handle<v8::Object> dest )
    {
        typedef ClassWrap_Inheritance<BoundNative> Inherit;
        {
            Inherit x;
        }
        using namespace v8;
        using namespace v8::juice;
        HandleScope scope;
        typedef BoundNative N;
        typedef ClassWrap<N> CW;
        CW cw(dest);
        CERR <<"Binding class "<<CW::ClassName::Value()<<"...\n";
        cw.Set("foo",String::New("this is foo"));
        cw.Set("toString2", convert::InvocationCallbackMember<N,&N::toString2>::Call );
        //cw.BindMemberFunc<&N::toString2>("toString");
        typedef
            convert::InvocationCallbackCreator
            //convert::FunctionForwarder<0>
            ICC;
        cw.Set( "toString",
                ICC::M0::Invocable<N,std::string,&N::toString>
                );
        cw.Set( "getInt",
                ICC::M0::Invocable<N,int,&N::getInt>
                );
        cw.Set( "setInt",
                ICC::M1::Invocable<N,void,int,&N::setInt>
                );
        cw.Set( "ptr",
                ICC::M1::Invocable<N,bool,N const * ,&N::ptr>
                //ICC::M1::InvocableVoid<N,bool,N const * ,&N::ptr>
                );
        //typedef convert::PropertyBinder<N> PB;
        typedef CW::PB PB;
        PB::BindGetterSetter<int,&N::getInt,void,int,&N::setInt>( "myInt", cw.Prototype() );
        PB::BindGetter<int,&N::getInt>( "intGetter", cw.Prototype() );
        PB::BindMemVar<double,&N::publicProperty>( "publicProperty", cw.Prototype() );
        PB::BindMemVarRO<double,&N::publicProperty>( "publicPropertyRO", cw.Prototype() );
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

        FH = ICC::F0::Invocable<size_t,BoundNative::InstanceCount>;
        cw.Set( "instanceCount", JFH );
        cw.CtorTemplate()->Set( "instanceCount", JFH );

        FH = ICC::F1::Invocable<unsigned int,unsigned int,::sleep>;
        cw.Set( "sleep", JFH );

        cw.Seal();
#undef JFH

        //v8::HandleScope hscope;
        Handle<Object> jobj = cw.NewInstance(0,0);
        N * bound = CW::ToNative::Value(jobj);
        CERR << "bound (void *) == @"<<(void const *)bound<<'\n';
        CERR << "bound (CastFromJS<T>(jsObj)) == @"<<convert::CastFromJS<N>( jobj )<<'\n';
        if( bound )
        {
            typedef convert::MemFuncForwarder<0> MFF;
            //Handle<Value> = MFF::Call( obj, BoundNative::toString
        }
        if(1)
        {
            v8::HandleScope hs;
            int level = 1;
            v8::Local<v8::Value> proto = jobj->GetPrototype();
            for( ; ! proto.IsEmpty() && proto->IsObject(); ++level )
            {
                CERR << "Prototype level "<<level<<'\n';
                proto = Local<Object>( v8::Object::Cast(*proto) )->GetPrototype();
            }
        }


        CERR << "BoundNative::InstanceCount() == "<<BoundNative::InstanceCount()<<'\n';
        CW::DestroyObject(jobj);
        CERR << "BoundNative::InstanceCount() == "<<BoundNative::InstanceCount()<<'\n';
#if 0
        if(0 && bound)
        {
            Handle<Object> j2 = CW::CastToJS::Value( bound );
            CERR << "JW::CastToJS::Value(jobj) == "<<convert::CastFromJS<std::string>(j2)<<'\n';
        }
#endif
        CERR <<"Binding done.\n";
    }

} } // namespaces

