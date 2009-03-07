#include <v8/juice/plugin.h>
#include <v8/juice/PathFinder.h>


namespace v8 {
namespace juice {
namespace plugin {

    std::string open( const std::string & basename, std::string & errmsg ); // defined in one of the platform-specific files

    using namespace ::v8;
    using ::v8::juice::PathFinder;

    PathFinder & PluginPath()
    {
	static PathFinder bob(v8_juice_plugin_CONFIG_PLUGINS_PATH,
			      v8_juice_plugin_CONFIG_DLL_EXTENSIONS,
			      v8_juice_plugin_CONFIG_PATH_SEPARATOR);
	return bob;
    }

    LoadPluginScope * LoadPluginScope::current = 0;
    LoadPluginScope::LoadPluginScope( Handle<Object> target )
	: target(target),
	  returnVal(target),
	  prev(LoadPluginScope::current)
    {
	LoadPluginScope::current = this;
    }

    LoadPluginScope::~LoadPluginScope()
    {
	LoadPluginScope::current = this->prev;
    }

    LoadPluginScope * LoadPluginScope::Current()
    {
	return LoadPluginScope::current;
    }

    Handle<Object> LoadPluginScope::Target() const
    {
	return this->target;
    }

    void LoadPluginScope::SetReturnValue( Handle<Value> v )
    {
	this->returnVal = v;
    }

    Handle<Value> LoadPluginScope::ReturnValue() const
    {
	return this->returnVal;
    }

    Handle< Value > LoadPlugin( std::string const & modname,
				Handle< Object > target )
    {
	LoadPluginScope sentry(target);
	std::string errmsg;
	std::string fn( open( modname, errmsg ) );
	if( ! errmsg.empty() )
	{
	    errmsg = "LoadPlugin(\""+modname+"\") DLL error message: dll=["+fn+"]: "+errmsg;
	    return ThrowException( String::New(errmsg.c_str(),
					       static_cast<int>(errmsg.size())) );
	}
	else if( fn.empty() )
	{
	    errmsg = "LoadPlugin(\""+modname+"\"): No DLL found!";
	    return ThrowException( String::New(errmsg.c_str(),
					       static_cast<int>(errmsg.size())) );
	}
	return sentry.ReturnValue();
    }

    Handle<Value> LoadPlugin(const Arguments& argv)
    {
	const int argc = argv.Length();
	if(  !argc || (argc>2) )
	{
	    return ThrowException(String::New("Invalid usage: LoadPlugin(String[,Object])"));
	}
	Handle<Object> tgt = argv.Holder();
	if( argc == 2 )
	{
	    if( ! argv[1]->IsObject() )
	    {
		return ThrowException(String::New("LoadPlugin() Second argument is-not-a Object!"));
	    }
	    tgt = Handle<Object>( Object::Cast( *argv[1] ) );
	}
	String::AsciiValue av( argv[0] );
	std::string dllname = *av ? *av : "";
#if 0
	TryCatch tryer;
	Handle<Value> rv = LoadPlugin( dllname, tgt );
	if( rv.IsEmpty() )
	{
	    ThrowException( tryer.Exception() );
	}
	return rv;
#else
	return LoadPlugin( dllname, tgt );
#endif
    }


    /** Used by the platform-specific open() implementations. */
    static std::string FindPlugin( const std::string & basename )
    {
	// CERR << "find("<<basename<<")... path="<<path().path_string()<<"\nextensions="<<path().extensions_string()<<"\n";
	return PluginPath().find( basename );
    }

#if PLUGIN_USE_NOOP
    std::string open( const std::string & basename, std::string & errmsg )
    {
	errmsg = std::string("s11n::plugin::open(")
	    + basename
	    + std::string( "): not implemented on this platform." )
	    ;
	return std::string();
    }
#endif // PLUGIN_USE_NOOP


}}} // namespaces

#if ! PLUGIN_USE_NOOP
#    if v8_juice_plugin_CONFIG_HAVE_LIBLTDL || v8_juice_plugin_CONFIG_HAVE_LIBDL
#      include "plugin.dl.cpp"
#    elif defined(WIN32)
#      include "plugin.win32.cpp"
#    endif
#endif // WIN32?
