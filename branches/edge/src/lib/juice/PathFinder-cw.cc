/************************************************************************
A JS/Native binding of the v8::juice::PathFinder class, using the
v8::juice::cw binding mechanism.
************************************************************************/
//    Author: stephan beal <stephan@s11n.net>
//    License: Public Domain

#include <iostream>
#include <set>

#include <v8.h>
#include <v8/juice/PathFinder.h>
#include <v8/juice/bind.h>
#include <v8/juice/plugin.h>



#include <v8/juice/ClassWrap.h>
//#include <v8/juice/ClassWrap_JuiceBind.h>
// #include <v8/juice/ClassWrap_TwoWay.h>


#define CLASSWRAP_BOUND_TYPE v8::juice::PathFinder
#if 0
   // Default ClassWrap policy set.
   JUICE_CLASSWRAP_CLASSNAME(v8::juice::PathFinder,"PathFinder");
#  include <v8/juice/ClassWrap-JSToNative.h>
#else
#  define CLASSWRAP_BOUND_TYPE_NAME "PathFinder"
#  if 0
//#    warning "JUICEBIND!"
#    include <v8/juice/ClassWrap_JuiceBind.h>
#  else
//#    warning "TWOWAY BIND!"
#    include <v8/juice/ClassWrap_TwoWay.h>
#  endif
#endif

#if !defined(CERR)
#    include <iostream> /* only for debuggering */
#    define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

namespace {
    enum Internal { MagicExternalArgc = 1 /*must be 1 or else crash*/ };
    }

namespace v8 { namespace juice { namespace cw {
    using namespace ::v8::juice;

#define JSTR(X) String::New(X)
#define TOSS(X) ThrowException(JSTR(X))

    //! ClassWrap Factory<PathFinder> policy implementation.
    template <>
    struct Factory<PathFinder>
    {
        //! Required by Factory interface.
	static size_t const AllocatedMemoryCost = sizeof(PathFinder);
        //! Required by Factory interface.
        typedef PathFinder Type;
        //! Required by Factory interface.
	typedef PathFinder * NativeHandle;
    private:
        //! internal
        typedef std::set<NativeHandle> PFSet;
        /**
           All PathFinders which should be deleted by
           this type (that depends on how the ctor is called)
           are added here. Only dtors called for these objects
           will trigger a delete.
        */
        static PFSet & pfset()
        {
            static PFSet pf;
            return pf;
        }
    public:
	static NativeHandle Instantiate( Arguments const & argv,
                                         std::ostream & exceptionText)
	{
	    const int argc = argv.Length();
	    if( (MagicExternalArgc == argc) && argv[0]->IsExternal() )
	    { // assume arg is an externally-owned/shared PathFinder instance
		Local<External> ex( External::Cast( *argv[0] ) );
                NativeHandle xp = bind::GetBoundNative<PathFinder>( ex->Value() );
		if( xp )
		{
                    bind::UnbindNative( xp ); // we don't need this anymore.
		    return xp;
		}
		else
		{
		    exceptionText << "First argument to "
                                  << ClassName<PathFinder>::Value()
                                  << " ctor failed type check!";
		    return 0;
		}
	    }
	    std::string a0 = (argc>0) ? convert::JSToStdString(argv[0]) : "";
	    std::string a1 = (argc>1) ? convert::JSToStdString(argv[1]) : "";
	    std::string a2 = (argc>2) ? convert::JSToStdString(argv[2]) : ":";
	    //CERR << ClassName<PathFinder>::Value()<< "(["<<a0<<"], ["<<a1<<"], ["<<a2<<"])\n";
	    NativeHandle pf = new PathFinder( a0, a1, a2 );
	    if( pf )
	    {
                pfset().insert( pf );
	    }
	    return pf;
	}

	static void Destruct( v8::Handle<v8::Object>, NativeHandle obj )
	{
	    if( obj )
	    {
                PFSet::iterator it = pfset().find(obj);
                if( it != pfset().end() )
                {
                    //CERR << ClassName<PathFinder>::Value() << " dtor deleting on @"<<obj<<'\n';
                    pfset().erase(it);
                    delete obj;
                }
	    }
	}
    };

    
}}} // namespaces


namespace v8 { namespace juice {

    //! PathFinder.toString() impl.
    static v8::Handle<v8::Value> pf_is_accessible( v8::Arguments const & argv )
    {
        return convert::FwdToFunc( PathFinder::IsAccessible, argv );
    }

    //! PathFinder.toString() impl.
    static v8::Handle<v8::Value> pf_toString( v8::Arguments const & argv )
    {
        typedef PathFinder T;
        T * p = convert::CastFromJS<T>( argv.This() );
        convert::StringBuffer sb;
        sb << "[object "<<cw::ClassName<T>::Value()<<"@"<<p<<"]";
        return sb;
    }
    
    ::v8::Handle< ::v8::Value> SetupPathFinderClass( ::v8::Handle< ::v8::Object> target )
    {
        HandleScope scope;
        typedef PathFinder N;
        typedef cw::ClassWrap<N> CW;
        typedef convert::MemFuncInvocationCallbackCreator<N> MF;
        CW & cw( CW::Instance() );

        cw.Set( "toString", pf_toString );
        cw.Set( "getPathSeparator", MF::M0::Invocable<std::string,&N::PathSeparator> );
        cw.Set( "setPathSeparator", MF::M1::Invocable<void,std::string const &,&N::PathSeparator> );
        cw.BindGetterSetter< std::string, &N::PathSeparator, void, std::string const &,&N::PathSeparator>( "pathSeparator" );

        cw.Set( "getPathString", MF::M0::Invocable<std::string,&N::PathString> );
        cw.Set( "setPathString", MF::M1::Invocable<size_t,std::string const &,&N::Path> );
        cw.BindGetterSetter< std::string, &N::PathString, size_t, std::string const &,&N::Path>( "pathString" );

        typedef N::StringList SL;
        typedef SL const & SLCR;
        cw.Set( "getPathArray", MF::M0::Invocable<SL,&N::Path> );
        cw.Set( "setPathArray", MF::M1::Invocable<size_t,SLCR,&N::Path> );
        cw.BindGetterSetter< SL, &N::Path, size_t, SLCR,&N::Path>( "pathArray" );
        
        cw.Set( "getExtensionsArray", MF::M0::Invocable<SL,&N::Extensions> );
        cw.Set( "setExtensionsArray", MF::M1::Invocable<size_t,SLCR,&N::Extensions> );
        cw.BindGetterSetter< SL, &N::Extensions, size_t, SLCR,&N::Extensions>( "extensionsArray" );

        cw.Set( "getExtensionsString", MF::M0::Invocable<std::string,&N::ExtensionsString> );
        cw.Set( "setExtensionsString", MF::M1::Invocable<size_t,std::string const &,&N::Extensions> );
        cw.BindGetterSetter< std::string, &N::ExtensionsString, size_t, std::string const &,&N::Extensions>( "extensionsString" );


        cw.Set( "addPathString", MF::M1::Invocable<void,std::string const &,&N::AddPath> );
        cw.Set( "addExtensionString", MF::M1::Invocable<void,std::string const &,&N::AddExtension> );
        cw.Set( "find", MF::M1::Invocable<std::string,std::string const &,&N::Find> );
        cw.Set( "clearCache", MF::M0::Invocable<void, &N::ClearCache> );
        cw.Set( "isEmpty", MF::M0::Invocable<bool, &N::IsEmpty> );
        typedef convert::InvocationCallbackCreator ICC;
        //cw.Set( "isAccessible", pf_is_accessible );
        cw.Set( "isAccessible", ICC::F1::Invocable<bool,std::string const &,N::IsAccessible> );
        cw.Set( "dirSeparator", convert::CastToJS( N::DirSeparator() ), v8::ReadOnly );
        //cw.Set( "dirSeparator", ICC::F0::Invocable<std::string,N::DirSeparator> );

        Handle<Function> ctor( cw.Seal() );
        cw.AddClassTo(target);
        ctor->Set( JSTR("dirSeparator"), convert::CastToJS( N::DirSeparator() ), v8::ReadOnly );

        v8::InvocationCallback IC;
#define SET(K) ctor->Set( JSTR(K), v8::FunctionTemplate::New(IC)->GetFunction()  )
        IC = ICC::F1::Invocable<bool,std::string const &,N::IsAccessible>;
        SET("isAccessible");
#undef SET

        if(0)
        { // some basic sanity checks...
            typedef cw::Extract<N> XT;
            v8::HandleScope hscope;
            v8::Handle<v8::Object> jobj =
                cw.NewInstance(0,0)
                //ctor->NewInstance(0,0)
                ;
            N const * bound = CW::ToNative::Value(jobj);
            CERR << "bound (void *) == @"<<(void const *)bound<<'\n';
            CERR << "bound (CastFromJS<N*>(jsObj)) == @"<<convert::CastFromJS<N*>( jobj )<<'\n';
            CERR << "bound (JSToNative<N>(jsObj)) == @"<<convert::JSToNative<N>()( jobj )<<'\n';
            CERR << "bound (JSToNative<N*>(jsObj)) == @"<<convert::JSToNative<N*>()( jobj )<<'\n';
            CERR << "bound (cw::JSToNativeImpl<N>()(jsObj)) == @"<<cw::JSToNativeImpl<N>()( jobj )<<'\n';
            CERR << "bound (cw::JSToNativeImpl<N*>()(jsObj)) == @"<<cw::JSToNativeImpl<N*>()( jobj )<<'\n';
            CERR << "Extract: @"<<(void const *)XT::ExtractVoid(jobj,false)<<"\n";
            N const * np = convert::CastFromJS<N>( jobj );
            CERR << "bound (CastFromJS<N>(jsObj)) == @"<<np<<'\n';
            if( bound )
            {
                void * exh = jobj->GetPointerFromInternalField(CW::InternalFields::NativeIndex);
                CERR << "fetched void* == "<<(const void *)exh<<'\n';
            }
        }

#if 1
        // Set up some shared instances:
	Handle<Object> shared = Object::New();
	ctor->Set(JSTR("shared"),shared);
        ctor->Set(JSTR("isAccessible"), FunctionTemplate::New(pf_is_accessible)->GetFunction() );

	{
	    // Install an instance wrapping the v8::juice::plugin::PluginsPath() shared object:
            PathFinder * pf = &::v8::juice::plugin::PluginsPath();
	    Handle<Value> pfex( External::New( pf ) );
            bind::BindNative( pf, pf );
	    Handle<Object> pfobj = cw.NewInstance( MagicExternalArgc, &pfex );
	    shared->Set(String::New("plugins"), pfobj );
	}

	if(1)
	{
	    // Includes includes path:
            PathFinder * pf = &::v8::juice::ScriptsPath();
	    Handle<Value> pfex( External::New( pf ) );
            bind::BindNative( pf, pf );
	    Handle<Object> pfobj = cw.NewInstance( MagicExternalArgc, &pfex );
	    shared->Set(String::New("include"), pfobj );
	}
#endif
	return target;
    }
#undef JSTR
#undef TOSS
}} // namespaces
