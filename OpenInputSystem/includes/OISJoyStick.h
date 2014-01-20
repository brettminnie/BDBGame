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
#ifndef OIS_Joystick_H
#define OIS_Joystick_H
#include "OISObject.h"

namespace OIS
{
	/**
		Joystick base class. To be implemented by specific system (ie. DirectX joystick)
		This class is useful as you remain OS independent using this common interface.
	*/
	class _OISExport JoyStick : public Object
	{
	public:
		virtual ~JoyStick() {}

		/**
		@remarks
			Returns the number of buttons
		*/
		short buttons() {return numButtons;}

		/**
		@remarks
			Returns the number of axes
		*/
		short axes() {return numAxes;}

		/**
		@remarks
			Register/unregister a JoyStick Listener - Only one allowed for simplicity. If broadcasting
			is neccessary, just broadcast from the callback you registered.
		@param joyListener
			Send a pointer to a class derived from JoyStickListener or 0 to clear the callback
		*/
		virtual void setEventCallback( JoyStickListener *joyListener ) {listener = joyListener;}

		/**
		@remarks
			Returns currently set callback.. or 0
		*/
		JoyStickListener* getEventCallback() {return listener;}

		/**
		@remarks
			Returns the state of the joystick - is valid for both buffered and non buffered mode
		*/
		const JoyStickState& getJoyStickState() const { return mState; }

	protected:
		JoyStick() : numButtons(0), numAxes(0), listener(0) {}

		short numButtons;
		short numAxes;

		JoyStickState mState;

		JoyStickListener *listener;
	};
}
#endif
