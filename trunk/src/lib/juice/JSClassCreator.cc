
#include <ostream>
#include <sstream>
#include <v8/juice/JSClassCreator.h>
namespace v8 { namespace juice {

    using namespace v8;

    JSClassCreator::JSClassCreator( char const * className,
				    Handle<Object> target,
				    v8::InvocationCallback ctor,
				    int internalFieldCount )
	: className(className),
	  target(target),
	  ctorTmpl( FunctionTemplate::New(ctor) ),
	  proto( ctorTmpl->PrototypeTemplate() )
    {
	if( internalFieldCount > 0 )
	{
	    ctorTmpl->InstanceTemplate()->SetInternalFieldCount(internalFieldCount);
	}
    }

    JSClassCreator::~JSClassCreator()
    {
    }

    JSClassCreator & JSClassCreator::SetInternalFieldCount( int n )
    {
	this->ctorTmpl->InstanceTemplate()->SetInternalFieldCount( n );
	return *this;
    }

    Handle<FunctionTemplate> JSClassCreator::CtorTemplate() const
    {
	return this->ctorTmpl;
    }
    
    char const * JSClassCreator::ClassName() const
    {
	return this->className;
    }

    Local<ObjectTemplate> JSClassCreator::Prototype() const
    {
	return this->proto;
    }

    Handle<Object> JSClassCreator::Target() const
    {
	return this->target;
    }

    void JSClassCreator::Seal()
    {
	// In my experience, if GetFunction() is not AFTER setting up
	// the Prototype object, v8 gets very unhappy.
	this->target->Set( ::v8::String::New(this->className), ctorTmpl->GetFunction() );
	//return target;
    }

    JSClassCreator & JSClassCreator::Set( char const * name, Handle< Data > const & value , PropertyAttribute attributes )
    {
	proto->Set( String::New(name), value, attributes );
	return *this;
    }

    JSClassCreator & JSClassCreator::Set( char const * name, InvocationCallback func, PropertyAttribute attributes )
    {
	proto->Set( String::New(name), FunctionTemplate::New(func), attributes );
	return *this;
    }

    JSClassCreator & JSClassCreator::Inherit( Handle< FunctionTemplate > parent )
    {
	ctorTmpl->Inherit( parent );
	return *this;
    }

    JSClassCreator & JSClassCreator::Inherit( JSClassCreator & parent )
    {
	return this->Inherit( parent.CtorTemplate() );
    }


}}
