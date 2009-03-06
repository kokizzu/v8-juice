//    Author: stephan beal <stephan@s11n.net>
//    License: Public Domain

#include <iostream>

#include <v8/PathFinder.h>

#ifndef WIN32
#  define WIN32 0
#endif

#if WIN32
#  include  <io.h>
#  include  <stdio.h>
#else
#  include <unistd.h>
#endif

namespace v8 { namespace p3 {


    PathFinder::~PathFinder()
    {
	// DTOROUT(PathFinder) << this->path_string() << std::endl;
    }

    PathFinder::PathFinder( const std::string & p, const std::string & e, const std::string & pathsep )
    {
	this->path_separator( pathsep );
	this->path( p );
	this->extensions( e );
    }


    bool PathFinder::empty() const
    {
	return this->paths.empty() && this->exts.empty();
    }

    const std::string & PathFinder::path_separator() const
    {
	return ( ( PathFinder * ) this )->pathseparator;
    }

    void PathFinder::path_separator( const std::string & sep )
    {
	this->pathseparator = sep.empty() ? ":" : sep;
    }

    std::string PathFinder::join_list( const string_list & list, const std::string & separator )
    {
	std::string ret;
	unsigned long count = list.size();
	unsigned long at = 0;
	string_list::const_iterator it = list.begin();
	string_list::const_iterator et = list.end();
	for(; it != et; ++it )
	{

	    ret += (*it);
	    if( ++at != count ) ret += separator;
	}
	return ret;
    }
    std::string PathFinder::path_string() const
    {
	return this->join_list( this->paths, this->pathseparator );
    }

    const PathFinder::string_list & PathFinder::path() const
    {
	return this->paths;
    }

    PathFinder::string_list & PathFinder::path()
    {
	return this->paths;
    }

    std::string PathFinder::extensions_string() const
    {
	return this->join_list( this->exts, this->pathseparator );
    }

    const PathFinder::string_list & PathFinder::extensions() const
    {
	return this->exts;
    }

    PathFinder::string_list & PathFinder::extensions()
    {
	return this->exts;
    }


    std::size_t tokenize_to_list( const std::string & str, std::list<std::string> & li, const std::string & sep )
    { // internal helper function
	if( str.empty() ) return 0;

	std::size_t c = 0;

	std::string token;
	std::string::size_type sz = str.size();
	for( std::string::size_type i = 0; i < sz; i++ )
	{
	    if( sz-1 == i ) token += str[i];
	    if( str.find( sep, i ) == i || (sz-1 == i) )
	    {
		//CERR << "token="<<token<<std::endl;
		li.push_back( token );
		token = "";
		i += sep.size() - 1;
		continue;
	    }
	    token += str[i];
	}
	return c;
    }

    std::size_t PathFinder::path( const std::string & p )
    {
	this->paths.erase( this->paths.begin(), this->paths.end() );
	return tokenize_to_list( p, this->paths, this->pathseparator );
    }

    std::size_t PathFinder::path( const PathFinder::string_list & p )
    {
	this->paths = p;
	return this->paths.size();
    }

    void PathFinder::add_path( const std::string & p )
    {
	tokenize_to_list( p, this->paths, this->pathseparator );
    }


    std::size_t PathFinder::extensions( const std::string & p )
    {
	this->exts.erase( this->exts.begin(), this->exts.end() );
	return tokenize_to_list( p, this->exts, this->pathseparator );
    }

    std::size_t PathFinder::extensions( const PathFinder::string_list & e )
    {
	this->exts = e;
	return this->exts.size();
    }

    void PathFinder::add_extension( const std::string & p )
    {
	tokenize_to_list( p, this->exts, this->pathseparator );
    }

    // static
    bool PathFinder::is_accessible( const std::string & path )
    {
#if WIN32
#  define CHECKACCESS _access
#  define CHECKRIGHTS 0
#else
#  define CHECKACCESS access
#  define CHECKRIGHTS F_OK
#endif

	return 0 == CHECKACCESS( path.c_str(), CHECKRIGHTS );
#undef CHECKACCESS
#undef CHECKRIGHTS
    }

    std::string PathFinder::basename( const std::string & name )
    {
	std::string::size_type slashat = name.find_last_of( PathFinder::dir_separator() );
	if ( slashat == std::string::npos )
	    return name;
	return name.substr( slashat + 1 );
    }


    std::string PathFinder::dir_separator()
    {
#if WIN32
	return std::string( "\\" );
#else
	return std::string( "/" );
#endif
    }


    std::string PathFinder::find( const std::string & resource, bool check_cache ) const
    {
	//static const std::string NOT_FOUND = "PathFinder::find() : no findie";
	if( resource.empty() ) return resource;

#define CHECKPATH(CHECKAT)						\
        if( ! CHECKAT.empty() && PathFinder::is_accessible( CHECKAT ) ) \
        { this->hitcache[resource] = CHECKAT; return CHECKAT; }

	//CERR << "find( " << resource << " )" << std::endl;
	if( check_cache )
	{
	    std::map <std::string,std::string>::iterator mapiter;
	    mapiter = this->hitcache.find( resource );
	    if( this->hitcache.end() != mapiter ) return (*mapiter).second;
	}

	CHECKPATH( resource );

	string_list::const_iterator piter = this->paths.begin();
	string_list::const_iterator eiter = this->exts.begin();

	std::string path;
	std::string ext;

	if ( PathFinder::is_accessible( resource ) )
	    return resource;

	piter = this->paths.begin();
	std::string checkhere;
	while ( piter != this->paths.end() )
	{
	    path = ( *piter );
	    if ( !path.empty() )
	    {
		path += PathFinder::dir_separator();
	    }
	    ++piter;
	    checkhere = path + resource;
	    //CERR << "find( " << resource << " ) checking " << checkhere << std::endl;
	    CHECKPATH( checkhere );
	    eiter = this->exts.begin();
	    while ( eiter != this->exts.end() )
	    {
		ext = ( *eiter );
		++eiter;
		checkhere = path + resource + ext;
		//CERR << "find( " << resource << " ) checking " << checkhere << std::endl;
		CHECKPATH( checkhere );
	    }
	}
	//CERR << "find( "<<resource<<" ): not found :(" << std::endl;
	// so arguable:
	// this->hitcache[resource] = "";
	return std::string();
    }
#undef CHECKPATH
    void PathFinder::clear_cache()
    {
	this->hitcache.clear();
    }

}} // namespaces

