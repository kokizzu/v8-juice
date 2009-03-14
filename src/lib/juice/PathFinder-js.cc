//    Author: stephan beal <stephan@s11n.net>
//    License: Public Domain

#include <iostream>
#include <vector>

#include <v8/juice/PathFinder.h>
#include <v8.h>
#include <v8/juice/convert.h>
#include <v8/juice/plugin.h>
#include <v8/juice/cleanup.h>
#include <v8/juice/ClassBinder.h>


#ifndef CERR
#include <iostream> /* only for debuggering */
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

namespace v8 { namespace juice {
    using namespace ::v8::juice::bind;
    using namespace ::v8::juice::convert;
#define JSTR(X) String::New(X)
#define TOSS(X) ThrowException(JSTR(X))

    // TODO: must write a thin wrapper over PathFinder
    // due to constness and reference passing issues.

    template <>
    struct WeakJSClassCreatorOps<PathFinder>;

    class PFJS
    {
    private:
	PathFinder * pf;
	bool ownsPF;
    public:
	PFJS( const std::string & path = std::string(), const std::string & ext = std::string(), const std::string & pathsep = ":" )
	    : pf( new PathFinder(path,ext,pathsep)),
	      ownsPF(true)
	{
	}
	explicit PFJS( PathFinder * shared )
	    : pf( shared ),
	    ownsPF(false)
	{
	}

	~PFJS()
	{
	    if( pf && ownsPF ) delete pf;
	}

	std::string pathString() const
	{
	    return pf->path_string();
	}
	std::string pathSeparator() const
	{
	    return pf->path_separator();
	}

	void setPathSeparator(std::string const &v)
	{
	    pf->path_separator(v);
	}

	void setPathString( std::string const & v)
	{
	    pf->path(v);
	}

	void setPathArray( PathFinder::string_list v )
	{
	    pf->path(v);
	}
	PathFinder::string_list pathArray()
	{
	    return pf->path();
	}

	std::string find( std::string const & f ) const
	{
	    return pf->find(f);
	}

	int tryConst() const
	{
	    return 42;
	}

	void tryConstV() const
	{
	}

#if 1
	int nonBoundFunction() { CERR << "nonBoundFunction()\n"; return 17; }

	static Handle<Value> callMemFunc( Arguments const & argv )
	{
	    return ::v8::juice::CallWeakMemFunc<PFJS,int>( argv, &PFJS::nonBoundFunction );
	}
#endif
    };

    /** Required specialization of WeakJSClassCreatorOps<> for use
	with WeakJSClassCreator<PathFinder>. */
    template <>
    struct WeakJSClassCreatorOps<PathFinder>
    {
	enum { ExtraInternalFieldCount = 0 };
	typedef PFJS WrappedType;
	static char const * ClassName() { return "PathFinder"; }
	static WrappedType * Ctor( Arguments const & argv,
				   std::string & exceptionText)
	{
	    const int argc = argv.Length();
	    if( (1 == argc) && argv[0]->IsExternal() )
	    {
		External * ex = External::Cast( *argv[0] );
		if( ex )
		{
		    PathFinder * xp = static_cast<PathFinder*>(ex->Value());
		    if( ! xp ) return 0;
		    return new PFJS(xp);
		}
		else
		{
		    exceptionText = "First argument to ctor failed External::Cast()";
		    return 0;
		}
	    }
	    std::string a0 = (argc>0) ? JSToStdString(argv[0]) : "";
	    std::string a1 = (argc>1) ? JSToStdString(argv[1]) : "";
	    std::string a2 = (argc>2) ? JSToStdString(argv[2]) : ":";
	    //CERR << "PathFinder(["<<a0<<"], ["<<a1<<"], ["<<a2<<"])\n";
	    PFJS * pf = new PFJS( a0, a1, a2 );
	    if( pf )
	    {
		cleanup::AddToCleanup(pf, cleanup_callback );
		bind::BindNative( 0, pf, pf );
	    }
	    return pf;
	}

	static void Dtor( WrappedType * obj )
	{
	    //CERR << "Dtor() passing on @"<<obj<<'\n';
	    if( obj )
	    {
		bind::UnbindNative( 0, obj, obj );
		cleanup::RemoveFromCleanup(obj);
		delete obj;
	    }
	}
    private:
	/** Callback for use with juice::cleanup::AddToCleanup(). */
	static void cleanup_callback( void * obj )
	{
	    Dtor( static_cast<WrappedType*>(obj) );
	}
    };

//     template <>
//     struct WeakJSClassCreatorOps<PFJS> : WeakJSClassCreatorOps<PathFinder> {};

#if 0 // old/unused, but might be a useful reference point later on for member var binding:
#define ARGS(FUNC,COND) const int argc = argv.Length(); if( !(COND) ) TOSS(FUNC "(): argument assertion failed: " # COND)
    //#define PH(H) PathFinder * p = pf_cast( H )
#define PH(H) PathFinder * p = WeakJSClassCreator<PathFinder>::GetSelf( H )
#define PHT(H) PH(H); if( ! p ) TOSS("Native PathFinder pointer is null!")
#define PHV(H) PH(H); if( ! p ) return

    static Handle<Value> pf_get_path_string( Local< String > property, const AccessorInfo &info )
    {
	PHT(info.This());
	return CastToJS( p->path_string() );
    }

    static void pf_set_path_string( Local< String > property, Local< Value > value, const AccessorInfo &info )
    {
	PHV(info.This());
	p->path( JSToStdString(value) );
    }

    static Handle<Value> pf_get_path_array( Local< String > property, const AccessorInfo &info )
    {
	PHT(info.This());
	return CastToJS( p->path() );
    }

    static void pf_set_path_array( Local< String > property, Local< Value > value, const AccessorInfo &info )
    {
	PHV(info.This());
	if( ! value->IsArray() ) TOSS("Path value is-not-an Array!");
	PathFinder::string_list li( p->path() );
	Handle<Array> ar( Array::Cast( *value ) );
	li.clear();
	for( uint32_t i = 0; ; ++i )
	{
	    if( ! ar->Has(i) ) break;
	    li.push_back( JSToStdString( ar->Get(Integer::New(static_cast<int>(i))) ) );
	}
    }

    static Handle<Value> pf_get_path_sep( Local< String > property, const AccessorInfo &info )
    {
	PHT(info.This());
	return CastToJS( p->path_separator() );
    }

    static void pf_set_path_sep( Local< String > property, Local< Value > value, const AccessorInfo &info )
    {
	PHV(info.This());
	std::string sep( JSToStdString(value) );
	if( ! sep.empty() )
	{
	    p->path_separator( sep );
	}
    }

    static Handle<Value> pf_find(const Arguments& argv)
    {
	ARGS("find",(argc==1 || argc==2));
	PHT(argv.This());
	std::string fn = JSToStdString(argv[0]);
	bool useCache = argc>1 ? JSToBool(argv[1]) : true;
	return StdStringToJS( p->find( fn, useCache ) );
    }
#endif

#undef ARGS
#undef PHT
#undef PHV
#undef PH
    Handle<Value> SetupPathFinderClass(Handle<Object> target )
    {
	HandleScope scope;
	typedef ClassBinder<PathFinder> PW;
	PW pw;

	pw.BindMemFunc< std::string, &PFJS::pathString >( "pathString" );
	pw.BindMemFunc< std::string, &PFJS::pathSeparator >( "pathSeparator" );
	pw.BindMemFunc< int, &PFJS::tryConst >( "tryConst" );
	pw.BindMemFunc< void, &PFJS::tryConstV >( "tryConstV" );
	pw.BindMemFunc< PathFinder::string_list, &PFJS::pathArray >( "pathArray" );
#if 1
	pw.BindMemFunc< void, std::string const &, &PFJS::setPathString >( "setPathString" );
	pw.BindMemFunc< void, std::string const &, &PFJS::setPathSeparator >( "setPathSeparator" );
	pw.BindMemFunc< void, PathFinder::string_list, &PFJS::setPathArray >( "setPathArray" );
	pw.BindMemFunc< std::string, std::string const &, &PFJS::find >( "find" );
#endif
	//pw.BindMemFunc<void,&PFJS::callMemFunc>(("callMemFunc");
	pw.Set("callMemFunc", PFJS::callMemFunc );
	target->Set( String::New(PW::ClassOpsType::ClassName()), pw.Seal() );
	Handle<Object> shared = Object::New();
	pw.CtorTemplate()->GetFunction()->Set(JSTR("shared"),shared);
	{
	    // Install an instance wrapping the v8::juice::plugin::PluginPath() shared object:
	    Handle<Value> pfex( External::New( &::v8::juice::plugin::PluginPath() ) );
	    Handle<Object> pfobj = pw.NewInstance( 1, &pfex );
	    shared->Set(String::New("plugins"), pfobj );
	}

	if(0) // we don't yet have an include() func for JS...
	{
	    // Includes includes path:
	    std::vector<Handle<Value> > vec(3,Null());
	    vec[0] = JSTR(".");
	    vec[1] = JSTR(".js:.v8:.juice:.v8-juice");
	    vec[2] = JSTR(":");
	    Handle<Object> pfInc = pw.NewInstance( 3, &vec[0] );
	    shared->Set(String::New("include"), pfInc );
	}
	return target;
    }
#undef JSTR
#undef TOSS
}} // namespaces
