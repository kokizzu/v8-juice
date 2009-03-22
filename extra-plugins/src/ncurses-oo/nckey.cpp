
#include <iostream> // only for a test.
#include <sstream>

// #include "ncpalette.hpp"
#include "nckey.hpp"

// #include <iostream> // only for debuggering
namespace ncutil {



	std::string name_for_key( int key )
	{
		std::ostringstream lbl;

		if( ERR == key ) return "ERR";

		// ascii chars go back as-is:
		if( key >= 32 /* space */ && key <= 126 /* tilde */ )
		{
			lbl << (char)key;
			return lbl.str();
		}

		// Fn keys
		if( key >= KEY_F(0) && key <= KEY_F(64) )
		{
			lbl << "F" << key - KEY_F(0);
			return lbl.str();
		}

		
		switch( key )
		{ // do these before CTRL keys so ENTER != 'C-J' :/
		  case KEY_HOME: lbl << "Home"; break;
		  case KEY_END: lbl << "End"; break;
		  case KEY_UP: lbl << "Up"; break;
		  case KEY_DOWN: lbl << "Down"; break;
		  case KEY_RIGHT: lbl << "Right"; break;
		  case KEY_LEFT: lbl << "Left"; break;
		  case KEY_PPAGE: lbl << "PgUp"; break;
		  case KEY_NPAGE: lbl << "PgDn"; break;
		  case '\t': lbl << "Tab"; break;
		  case '\n':
		  case KEY_ENTER: lbl << "Enter"; break;
		  default: break;
		};

 		if( ! lbl.str().empty() ) return lbl.str();

		// control key
		if( key < 32 )
		{
			// finally cracked this damned thing...
			key &= 0x00ff;
			key = 'A' + key - 1;
			lbl << "C-" << (char)key;
			return lbl.str();
		}


		return lbl.str();
	}

	NCMappedKeyConsumer::NCMappedKeyConsumer() : NCKeyConsumer()
	{
	}

	NCMappedKeyConsumer::~NCMappedKeyConsumer()
	{
	}

	NCMappedKeyConsumer::key_entry &
	NCMappedKeyConsumer::operator[](int key)
	{
		return this->keyMap()[key];
	}

	NCMappedKeyConsumer::key_map &
	NCMappedKeyConsumer::keyMap()
	{
		return this->m_map;
	}

	const NCMappedKeyConsumer::key_map &
	NCMappedKeyConsumer::keyMap() const 
	{
		return this->m_map;
	}

	void NCMappedKeyConsumer::mapKey( const key_entry & k )
	{
		this->keyMap()[k.key] = k;
	}

	void NCMappedKeyConsumer::mapKey( int key, mapper_type hndlr )
	{
		this->keyMap()[key] = NCKeyMapping(key, hndlr);
	}


	bool NCMappedKeyConsumer::consumeKey( int key, bool )
	{
		key_map & m = this->keyMap();
		if( m.empty() ) return false;
		key_map::iterator it;
		it = m.find( key );
		if( m.end() == it ) return false;
		((*it).second).func( key );
		return true;
	}

// 	bool NCMappedKeyConsumer::doLoop(const int stopOnKey )
// 	{
// 		NCMode sentry; // needed for ::getch()
// 		while( true )
// 		{
// 			key = ::getch();
// 			if( ERR == key ) return false;
// 			if( stopOnKey == key ) return true;
// 			if( ! this->consumeKey(key) ) return false;
// 			it = m.find( key );
// 			if( m.end() == it )
// 			{
// 				if( -1 == stopOnKey ) return false;
// 				continue;
// 			}
// 			((*it).second).func( key );
// 			if( -1 == stopOnKey ) return true;
// 		}
// 		return false;
// 	}


} // namespace ncutil
