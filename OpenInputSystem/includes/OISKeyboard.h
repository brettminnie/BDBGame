/*
The zlib/libpng License

Copyright (c) 2005 Phillip Castaneda (pjcast -- www.wreckedgames.com)

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that 
		you wrote the original software. If you use this software in a product, 
		an acknowledgment in the product documentation would be appreciated but is 
		not required.

    2. Altered source versions must be plainly marked as such, and must not be 
		misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/
#ifndef OIS_Keyboard_H
#define OIS_Keyboard_H
#include "OISObject.h"

namespace OIS
{
	/**
		Keyboard base class. To be implemented by specific system (ie. DirectX Keyboard)
		This class is useful as you remain OS independent using this common interface.
	*/
	class _OISExport Keyboard : public Object
	{
	public:
		virtual ~Keyboard() {};
		
		/**
		@remarks
			Returns true if key is donwn
		@param key
			A KeyCode to check
		*/
		virtual bool isKeyDown( KeyCode key ) = 0;

		/**
		@remarks
			Register/unregister a Keyboard Listener - Only one allowed for simplicity. If broadcasting
			is neccessary, just broadcast from the callback you registered.
		@param keyListener
			Send a pointer to a class derived from KeyListener or 0 to clear the callback
		*/
		virtual void setEventCallback( KeyListener *keyListener ) {listener=keyListener;}

		/**
		@remarks
			Returns currently set callback.. or 0
		*/
		KeyListener* getEventCallback() {return listener;}

		//! Used for setting a Keycode to Character translation
		enum KeyMapping
		{
			English, //Currently only language supported. Returns ASCII
			Unknown
		};
		
		/**
		@remarks
			Gets the KeyMapping setup for the requested KeyMap. Used
			in translating KeyCodes to Characters (including handling modifiers
			like shift). Default setting is English.
		@param km
			the KeyMapping enum to set/load
		*/
		void setKeyMapping( KeyMapping km );

		/**
		@remarks
			Translates KeyCode to Multibyte Character
		@param kc
			The KeyCode (KeyBoard Button Press) to convert
		@returns
			The character as determined from the current KeyMapping
		*/
		unsigned short lookupCharMapping( KeyCode kc );

		/**
		@remarks
			Translates KeyCode to string representation
		@param kc
			The KeyCode (KeyBoard Button Press) to convert
		@returns
			The string as determined from the current KeyMapping
		*/
		const std::string& getAsString( KeyCode kc );
		
	protected:
		Keyboard();

		//! Enum of bit position of modifer
		enum Modifier
		{
			Shift = 1,
			Ctrl = 2,
			Alt = 4
		};

		//! Bit field that holds status of Alt, Ctrl, Shift
		unsigned int mModifiers;
		//! Currently Loaded Translation Map
		std::vector <unsigned short> mMap;
		std::map <KeyCode, std::string> mStringMap;

		//! The type of Key Mapping loaded
		KeyMapping mMapping;
		//! Used for buffered/actionmapping callback
		KeyListener *listener;
	};
}
#endif
