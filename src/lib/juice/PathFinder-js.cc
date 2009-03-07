//    Author: stephan beal <stephan@s11n.net>
//    License: Public Domain

#include <iostream>

#include <v8/juice/PathFinder.h>
#include <v8.h>
#include <v8/juice/convert.h>
#include <v8/juice/plugin.h>
#include <v8/juice/cleanup.h>

#include <iostream> /* only for debuggering */
#ifndef CERR
#define CERR std::cerr << __FILE__ << ":" << std::dec << __LINE__ << " : "
#endif

namespace v8 { namespace juice {
    using namespace ::v8::juice::bind;
    using namespace ::v8::juice::convert;
#define JSTR(X) String::New(X)
#define TOSS(X) ThrowException(JSTR(X))
    static const void * pf_bind_cx() { static const int x=7;return &x;}

    //! Internal util to reduce code duplication elsewhere: unbinds and deletes pf.
    static void pf_cleanup( void * obj, PathFinder * pf )
    {
	//CERR << "pf_cleanup( void@"<<obj<<",  PathFinder@"<<pf<<")\n";
	if( pf )
	{
	    UnbindNative( pf_bind_cx(), obj, pf );
	    if( pf != &::v8::juice::plugin::PluginPath() )
	    {
		delete pf;
	    }
	}
    }

    /**
       Destructor for use with v8::juice::cleanup::AddToCleanup().
       obj MUST be-a (PathFinder*).
    */
    static void pf_cleanup( void * obj )
    {
	return pf_cleanup( obj, static_cast<PathFinder*>( obj ) );
    }

    static void pf_dtor(Persistent< Value > object, void *parameter)
    {
	::v8::juice::cleanup::RemoveFromCleanup(parameter);
	pf_cleanup(parameter);
	object.Dispose();
	object.Clear();
    }

    static Persistent<Object> pf_wrap( Handle<Object> _self, PathFinder * value )
    {
	if( value != &::v8::juice::plugin::PluginPath() )
	{
	    ::v8::juice::cleanup::AddToCleanup(value, pf_cleanup);
	}
	BindNative( pf_bind_cx(), value, value );
	Persistent<Object> self( Persistent<Object>::New(_self) );
	self.MakeWeak( value, pf_dtor );
	self->SetInternalField(0,External::New(value));
	//CERR << "Wrapped PathFinder @"<<value<<" in a Persistent<Object>.\n";
	return self;
    }

    static Handle<Value> pf_ctor(const Arguments& argv)
    {
	if (!argv.IsConstructCall()) 
	    return ThrowException(String::New("Cannot call PathFinder constructor as function"));

	const int argc = argv.Length();
	if( (1 == argc) && argv[0]->IsExternal() )
	{
	    External * ex = External::Cast( *argv[0] );
	    if( ex )
	    {
		return pf_wrap( argv.This(), static_cast<PathFinder*>(ex->Value()) );
	    }
	    else
	    {
		return ThrowException(String::New("First argument to ctor failed External::Cast()"));
	    }
	}
	std::string a0 = (argc>0) ? JSToStdString(argv[0]) : "";
	std::string a1 = (argc>1) ? JSToStdString(argv[1]) : "";
	std::string a2 = (argc>2) ? JSToStdString(argv[2]) : ":";
	//CERR << "PathFinder(["<<a0<<"], ["<<a1<<"], ["<<a2<<"])\n";
	PathFinder * pf = new PathFinder( a0, a1, a2 );
	//CERR << "PathFinder(["<<a0<<"], ["<<a1<<"], ["<<a2<<"])\n";
	return pf_wrap( argv.This(), pf );
    }
    static PathFinder * pf_cast( Local< Value > v )
    {
	if( v.IsEmpty() || ! v->IsObject() ) return 0;
	Local<Object> o( Object::Cast( *v ) );
	return GetBoundNative<PathFinder>( pf_bind_cx(), o->GetInternalField(0) );
    }

#define ARGS(FUNC,COND) const int argc = argv.Length(); if( !(COND) ) TOSS(FUNC "(): argument assertion failed: " # COND)
#define PH(H) PathFinder * p = pf_cast( H )
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

#undef ARGS
#undef PHT
#undef PHV
#undef PH
    Handle<Value> SetupPathFinderClass(const Handle<Object> target )
    {
	// Set up prototype:
	Handle<FunctionTemplate> ctor = FunctionTemplate::New(pf_ctor);
	Local<ObjectTemplate> objt = ctor->InstanceTemplate();
	objt->SetInternalFieldCount(1);
	objt->SetAccessor(JSTR("pathString"),
			  pf_get_path_string,
			  pf_set_path_string
			  );
	objt->SetAccessor(JSTR("pathArray"),
			  pf_get_path_array,
			  pf_set_path_array
			  );
	objt->SetAccessor(JSTR("pathSeparator"),
			  pf_get_path_sep,
			  pf_set_path_sep
			  );

	objt->Set(JSTR("find"), FunctionTemplate::New( pf_find ) );
	// Install ctor:
	Handle<Function> pffunc( ctor->GetFunction() );
	target->Set(String::New("PathFinder"), pffunc );

	// Install an instance wrapping the v8::juice::plugin::PluginPath() shared object:
	Handle<Value> pfex( External::New( &::v8::juice::plugin::PluginPath() ) );

	Handle<Object> shared = Object::New();
	pffunc->Set(String::New("shared"), shared );

	{
	    // Plugins path:
	    Handle<Object> pfobj = pffunc->NewInstance( 1, &pfex );
	    shared->Set(String::New("plugins"), pfobj );
	}

	{
	    // Includes path:
	    Handle<Value> tmp = JSTR(".:/usr/lib/v8/plugins");
	    Handle<Object> pfInc = pffunc->NewInstance( 1, &tmp );
	    shared->Set(String::New("include"), pfInc );
	}

	return pffunc;
    }
#undef JSTR
#undef TOSS
}} // namespaces

