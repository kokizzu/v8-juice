#ifndef ncutil_NCKEYCONSUMER_HPP_INCLUDED
#define ncutil_NCKEYCONSUMER_HPP_INCLUDED 1

#include <map>
#include <string>

#include <ncurses.h>

namespace ncutil {



	/**
	   Should return a human-readable form for many curses KEY_xxx
	   macros. The focus is short names which take up no more than
	   6 characters, so that spacing them with '\t' will provide
	   "visually usable" results. The strings are intended to be
	   used, e.g., as menu renderings of keys.

	   This function does not work for metad keys, as they come in
	   as two keystrokes and require special handling in client's
	   input loops.

	   It returns an empty string for keys which it has not
	   mapping for (please fix them and send me a patch!).

	   Currently handled:

	   - Ascii [32 .. 126] as string literals.

	   - Function keys [F0..F64] as string literals.

	   - Ctrl-modified KEY as "C-KEY".

	   - Arrows using logical names "Up", "Down", "Left", "Right".

	   - KEY_PPAGE, KEY_NPAGE as "PgUp" and "PgDn".

	   - '\t' as "Tab".

	   - '\n' or KEY_ENTER as "Enter".

	   - Curses' ERR as "ERR".

	*/
	std::string name_for_key( int );


	/**
	   Key handlers take a char encorded as an int and should return
	   the same number. They may do arbitrary processing, presumably
	   based on the passed-in key (though some handlers may be
	   designed for only one key, and ignore the input).

	   Handlers may return a different int, but must be aware that
	   they the int they return might signify a different key and
	   cause confusion in down-stream event handling. On exception
	   to this is the conventional use of a key handler which
	   return 0 to signify an Exit/Quit/Stop gesture. This allows users to
	   map arbitrary keys to a handler which always returns 0,
	   such that those keys would then tell the app to exit/cancel
	   the current operation.
	*/
	typedef int (*key_handler_func)(int);

	/**
	   For use with read_and_handle_key(), this maps ints to key
	   handlers.
	*/
	typedef std::map<int,key_handler_func> key_handler_map;


	/**
	   A no-op key_handler_func which simply returns key. We use
	   it as a roundabout way of getting at a keypress' value.
	*/
	int keyhnd_noop( int key );

	/**
	   A no-op key_handler_func which always returns zero. Used to
	   assign "cancel" semantics to arbitrary keys in a
	   key_handler_map.
	*/
	int keyhnd_zero( int key );


	/**
	   Waits for a keystroke via ::getch(). Once a key is read, a
	   handler is sought out in hmap.  If a handler is found, it
	   is called and it's return value returned.  If no handler is
	   found then:

	   If retry is true then it waits for another key. If retry is
	   false then -1 returned.

	   This function does not properly handle metad keys
	   (Alt/Esc-KEY combinations).

	   An example of using this function:

	   <pre>
	   key_handler_map m;
	   for( char i = 'a'; i <= 'z'; i++ )
	   {
	       m[i] = keyhnd_noop;
	   }
	   m[KEY_ENTER] = keyhnd_zero;
	   int key = read_and_handle_key( m, true );
	   </pre>

	   That will wait until they user enters 'a'..'z'
	   (case-sensitive) or taps Enter (KEY_ENTER is defined
	   by curses).

	*/
	int read_and_handle_key( const key_handler_map & hmap, bool retry );


	/**
	   NCKeyConsumer provides a common event consumption mechanism
	   for NCWindows and friends. The aim is to consolidate event
	   handling for disparate types like NCWindow, NCPad and
	   NCWorkspace, such that we can do "window management" more
	   generically by sending events to an "in-focus" key handler
	   (and potentially trying others if that one cannot handle
	   it).

	*/
	class NCKeyConsumer
	{
	public:
		NCKeyConsumer() {}
		virtual ~NCKeyConsumer() {}

		/**
		   This function is intended to provide the interface
		   in which "window managers" and whatnot can send
		   keys to a window for handling without having to
		   know the difference between, e.g., NCPad and NCPanel.

		   The key parameter should be a curses KEY_xxx value,
		   or compatible.

		   If metad is true then this object may assume that
		   key is in a meta'd state. It is not required to
		   honor the metad flag, and may freely ignore it, as
		   not all client code does the extra handling
		   necessary to determine if the key is meta-modified.
		   
		   Implementations must return true if they
		   "recognize" the key and false if they do not. If
		   they recognize the command but take no action they
		   should still return true.

		   A return value of false is a guaranty to caller
		   that this function generated no side effects. It is
		   up to implementors to comply with this!

		   Event loops based off of this type should:

		   a) Assume that if this function returns false,
		   another NCKeyConsumer may be tried in an attempt to
		   find a handler.

		   b) Assume that if this function returns true,
		   event handling should not continue for this event:
		   it was consumed and handled here.

		   c) Accomodate for an arbitrary exception to be
		   thrown from here: that is the only way for a
		   handler to report a real error.

		   Subclasses are expected to have documented
		   exception throwing behaviour if they throw.
		   Side-note: if C++ allowed it, i would declare
		   this whole namespace as:

		   namespace ncutil throw(std::exception) {...}

		   :/ becauses curses is very intolerant of things
		   like trying to allocate windows which are larger
		   than the size curses thinks your terminal is.
		   
		*/
		virtual bool consumeKey( int key, bool metad ) = 0;


		/**
		   Same as this->consumeKey( key, false ), provided
		   because the majority of cases will be non-meta
		   handlers and i don't want to have a default value
		   in an arg for a pure virtual function :/ (it's a
		   stylistic and maintenance thing).

		   i'm not at all sure why, but subclasses need (might
		   need?) this line in their class decl, or calling
		   this func via a derived pointer type might result
		   in an "undeclared function" error:

		   using NCKeyConsumer::consumeKey;

		   ???? my compiler == (gcc 3.3.4 20040809)
		   
		   Why is this only necessary for SOME
		   classes/functions? The whole NCWindow interface is
		   used in my client code via derived pointer types
		   and don't exhibit this!
		*/
		bool consumeKey( int key ) { return this->consumeKey(key,false); }
	};

	/**


	   NCKeyMapping is a bastardized mixture of two concepts:

	   - Using a translation function to translate one key value
	   to another, which is sometimes useful in event handling.

	   - Abusing the translation function as an action handler.
	   The "abuse" of it is that such a handler has no common
	   gesture for returning whether it succeeded or not, and thus
	   does not fit in well with conventional action handling.
	   It is convenient for menus/views which need only a handful
	   of simple action handlers, though.

	   The label and description fields are HINTS to the UI, and
	   may be ignored. They are intended mainly for use in
	   labeling menus/lists.

	   The func member should be called to translate this object's
	   key into it's "logical" key value, which might be mapped to
	   some other key. e.g., internally to ncutil it is common to
	   map keys to keyhnd_zero() to signify Quit/Exit semantics,
	   which often signifies that widgets should end their event
	   loop and return control to their caller.

	   Some clients may wish for mapper_type to perform arbitrary
	   logic. This class may be used that way, but if it is it should
	   be *consistently* used that way, so that we don't end up with
	   spaghetti code where some mappers perform all actions and
	   some don't, and we have to figure out in client code which
	   do and don't do what.
	*/
	struct NCKeyMapping
	{
		/**
		   The exact conventions of the mapper functions are
		   usage-specific, but in most places in ncutil they return
		   the equivalent of the int they are passed. They may
		   "remap" a key by returning another value for it.
		   Returning zero conventionally means "quit what you're
		   doing". The most common cases may use the predefined
		   keyhnd_noop() and keyhnd_zero().
		*/
		typedef key_handler_func mapper_type;

		/**
		   The key this object represents.
		*/
		int key;
		/**
		   A description hint.
		*/
		std::string description;

		/**
		   This object's function handler. The default is
		   keyhand_noop(), which simply returns the int passed
		   to it.
		*/
		mapper_type func;

		NCKeyMapping( int _key,
			      const std::string & _description,
			      mapper_type _func )
			:
			key(_key),
			description(_description),
			func(_func)
		{
		}

		NCKeyMapping( int _key )
			:
			key(_key),
			description(""),
			func(keyhnd_noop)
		{
		}

		NCKeyMapping( int _key,
			      const std::string & _description )
			:
			key(_key),
			description(_description),
			func(keyhnd_noop)
		{
		}
		NCKeyMapping( int _key,
			      mapper_type _func )
			:
			key(_key),
			description(""),
			func(_func)

		{
		}

		NCKeyMapping()
			:
			key(-1),
			description(""),
			func(keyhnd_noop)
		{
		}


	};


	/**
	   Don't use this class: it will be redesigned to operate
	   compatibly with NCKeyConsumer's conventions.

	   NCMappedKeyConsumer works along with NCKeyMapping to perform input
	   loops without the need for an associated widget. These can
	   theoretically be used as the main input loop of an
	   application.

	   See NCKeyMapping for the requirements of mapper_type.

	   TODO: the usage of this class is awkward. It needs to be
	   re-thought-out.
	*/
	class NCMappedKeyConsumer : public NCKeyConsumer
	{
	public:
		typedef NCKeyMapping key_entry;
		typedef NCKeyMapping::mapper_type mapper_type;


		/**
		   Reserved IDs which pad-related mapper_type
		   instances should honor if they expect to work with
		   this class.

		   Note that this type is designed specifically to
		   expect curses KEY_xxx definitions, and thus
		   the entries in this enum are guaranteed to play
		   fairly within the bounds of that sandbox.
		*/
		enum PredefinedKeyIDs {
		/**
		   Tells doLoop() that processing may stop.
		   Note that curses' spec actually implies
		   that 0 may be the same as curses' ERR, but
		   in practical technical terms ERR is (-1)
		   and will never change.
		   
		   curses' OK is defined only as "an integer other
		   than ERR", which means it could be potentially ANY
		   number. OK is coincidentally defined as 0 in
		   curses.h and will never change (i hope).
		*/
		KeyEndInput = OK,
		/**
		   Will always be the same as curses' ERR.
		*/
		KeyError = ERR
		};

		NCMappedKeyConsumer();
		virtual ~NCMappedKeyConsumer();

		typedef std::map<int,key_entry> key_map;

 		/** 
 		   Returns the key_entry associated with the given
 		   key, creating an empty one if necessary.
 		*/
 		key_entry & operator[](int key);

		key_map & keyMap();

		const key_map & keyMap() const;

		/**
		   Adds a new entry to our map.
		*/
		void mapKey( const key_entry & k );

		/**
		   Convenience form of mapKey().
		*/
		void mapKey( int key, mapper_type h );


		/**
		   Looks for a handler mapped tin keyMap(). If none is
		   found, false is returned. If one is found, it is
		   called and false is returned.

		   See NCKeyConsumer's docs for full details of the
		   conventions, including the exception policy. This
		   implementation only throws if a mapped handler
		   throws.

		   The implementation currently disregards the second
		   argument.
		*/
		virtual bool consumeKey( int, bool );

		// WTF do i need this for downstream code to see it this function???
 		using NCKeyConsumer::consumeKey;

	private:
		key_map m_map;
	};


} // namespace ncutil


#endif // ncutil_NCKEYCONSUMER_HPP_INCLUDED
