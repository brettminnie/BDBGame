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
#ifndef OIS_Mouse_H
#define OIS_Mouse_H
#include "OISObject.h"

namespace OIS
{
	/**
		Mouse base class. To be implemented by specific system (ie. DirectX Mouse)
		This class is useful as you remain OS independent using this common interface.
	*/
	class _OISExport Mouse : public Object
	{
	public:
		virtual ~Mouse() {}

		/**
		@remarks
			Register/unregister a Mouse Listener - Only one allowed for simplicity. If broadcasting
			is neccessary, just broadcast from the callback you registered.
		@param mouseListener
			Send a pointer to a class derived from MouseListener or 0 to clear the callback
		*/
		virtual void setEventCallback( MouseListener *mouseListener ) {listener = mouseListener;}

		/**
		@remarks
			Returns currently set callback.. or 0
		*/
		MouseListener* getEventCallback() {return listener;}

		/**
		@remarks
			Returns the state of the mouse - is valid for both buffered and non buffered mode
		*/
		const MouseState& getMouseState() const { return mState; }

	protected:
		Mouse() : listener(0) {}
		//! The state of the mouse
		MouseState mState;
		//! Used for buffered/actionmapping callback
		MouseListener *listener;
	};
}
#endif
