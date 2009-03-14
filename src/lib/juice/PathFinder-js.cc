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

    enum {
    MagicExternalArgc = 1
    };
    /** Required specialization of WeakJSClassCreatorOps<> for use
	with WeakJSClassCreator<PathFinder>. */
    template <>
    struct WeakJSClassCreatorOps<PathFinder>
    {
	enum { ExtraInternalFieldCount = 0 };
	typedef PathFinder WrappedType;
	static char const * ClassName() { return "PathFinder"; }
	static WrappedType * Ctor( Arguments const & argv,
				   std::string & exceptionText)
	{
	    const int argc = argv.Length();
	    if( (MagicExternalArgc == argc) && argv[0]->IsExternal() )
	    {
		External * ex = External::Cast( *argv[0] );
		if( ex )
		{
		    //CERR << "Creating from external instance...\n";
		    WrappedType * xp = static_cast<WrappedType*>(ex->Value());
		    if( ! xp ) return 0;
		    return xp;
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
	    WrappedType * pf = new WrappedType( a0, a1, a2 );
	    if( pf )
	    {
		cleanup::AddToCleanup(pf, cleanup_callback );
		//bind::BindNative( 0, pf, pf );
	    }
	    return pf;
	}

	static void Dtor( WrappedType * obj )
	{
	    //CERR << "Dtor() passing on @"<<obj<<'\n';
	    if( obj )
	    {
		//bind::UnbindNative( 0, obj, obj );
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

#if 0 // old/unused, but might be a useful reference point later on for member var binding:
#define ARGS(FUNC,COND) const int argc = argv.Length(); if( !(COND) ) TOSS(FUNC "(): argument assertion failed: " # COND)
    //#define PH(H) PathFinder * p = pf_cast( H )
#define PH(H) PathFinder * p = WeakJSClassCreator<PathFinder>::GetSelf( H )
#define PHT(H) PH(H); if( ! p ) TOSS("Native PathFinder pointer is null!")
#define PHV(H) PH(H); if( ! p ) return

    static Handle<Value> pf_get_path_string( Local< String > property, const AccessorInfo &info )
    {
	PHT(info.This());
	return CastToJS( p->PathString() );
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
	PathFinder::StringList li( p->path() );
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
	return CastToJS( p->PathSeparator() );
    }

    static void pf_set_path_sep( Local< String > property, Local< Value > value, const AccessorInfo &info )
    {
	PHV(info.This());
	std::string sep( JSToStdString(value) );
	if( ! sep.empty() )
	{
	    p->PathSeparator( sep );
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
	typedef PW::WrappedType PF;
	//WeakBindMemFunc<PW::WrappedType,std::string, &PF::pathString>( pw, "pathString" );
	pw.BindMemFunc< std::string, &PF::PathString >( "pathString" );
	pw.BindMemFunc< size_t, std::string const &, &PF::Path >( "setPathString" );
	pw.BindMemFunc< PathFinder::StringList, &PF::Path >( "pathArray" );
	pw.BindMemFunc< size_t, PathFinder::StringList const &, &PF::Path >( "setPathArray" );
	pw.BindMemFunc< std::string, &PF::PathSeparator >( "pathSeparator" );
	pw.BindMemFunc< void, std::string const &, &PF::PathSeparator >( "setPathSeparator" );
	pw.BindMemFunc< PathFinder::StringList, &PF::Extensions >( "extensionsArray" );
	pw.BindMemFunc< size_t, PathFinder::StringList const &, &PF::Extensions >( "setExtensionsArray" );
	pw.BindMemFunc< std::string, &PF::ExtensionsString >( "extensionsString" );
	pw.BindMemFunc< size_t, const std::string &, &PF::Extensions >( "setExtensionsString" );
	pw.BindMemFunc< void, std::string const &, &PF::AddPath >( "addPathString" );
	pw.BindMemFunc< void, std::string const &, &PF::AddExtension >( "addExtensionString" );
	pw.BindMemFunc< std::string, std::string const &, &PF::Find >( "find" );
	pw.BindMemFunc< void, &PF::ClearCache >( "clearCache" );
	pw.BindMemFunc< bool, &PF::IsEmpty >( "isEmpty" );
	pw.Set( "dirSeparator", CastToJS( PF::DirSeparator() ), v8::ReadOnly );

	//pw.BindMemFunc<void,&PF::callMemFunc>(("callMemFunc");

	target->Set( String::New(PW::ClassOpsType::ClassName()), pw.Seal() );
	Handle<Object> shared = Object::New();
	pw.CtorTemplate()->GetFunction()->Set(JSTR("shared"),shared);
	{
	    // Install an instance wrapping the v8::juice::plugin::PluginsPath() shared object:
	    Handle<Value> pfex( External::New( &::v8::juice::plugin::PluginsPath() ) );
	    Local<Object> pfobj = pw.NewInstance( MagicExternalArgc, &pfex );
	    shared->Set(String::New("plugins"), pfobj );
	}

	if(1)
	{
	    // Includes includes path:
	    Handle<Value> pfex( External::New( &::v8::juice::ScriptsPath() ) );
	    Local<Object> pfobj = pw.NewInstance( MagicExternalArgc, &pfex );
	    shared->Set(String::New("include"), pfobj );
	}
	return target;
    }
#undef JSTR
#undef TOSS
}} // namespaces
