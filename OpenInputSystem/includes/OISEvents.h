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
#ifndef _OIS_EVENTHEADERS_
#define _OIS_EVENTHEADERS_
#include "OISPrereqs.h"

namespace OIS
{
	/**
		Base class of all events
	*/
	class _OISExport EventArg
	{
	public:
		EventArg( Object* obj, unsigned int ts ) : device(obj), timeStamp(ts) {}
		virtual ~EventArg() {}

		//! Pointer to the Input Device
		const Object* device;
		//! A time Stamp
		const unsigned int timeStamp;
	};

	/**
		Specialised for key events
	*/
	class _OISExport KeyEvent : public EventArg
	{
	public:
		KeyEvent( Object* obj, unsigned int ts, KeyCode kc ) : 
			EventArg(obj,ts), key(kc) {}
		virtual ~KeyEvent() {}

		//! KeyCode of event
		const KeyCode key;
	};

	/**
		Specialised for mouse events
	*/
	class _OISExport MouseEvent : public EventArg
	{
	public:
		MouseEvent( Object *obj, unsigned int ts, const MouseState &ms )
			: EventArg(obj,ts), state(ms) {}
		virtual ~MouseEvent() {}

		//! The state of the mouse - including buttons and axes
		const MouseState &state;
	};

	/**
		Specialised for joystick events
	*/
	class _OISExport JoyStickEvent : public EventArg
	{
	public:
		JoyStickEvent( Object* obj, unsigned int ts, const JoyStickState &st ) : 
		  EventArg(obj,ts), state(st) {}
		virtual ~JoyStickEvent() {}

		const JoyStickState &state;
	};

	/**
		To recieve buffered keyboard input, derive a class from this, and implement the
		methods here. Then set the call back to your Keyboard instance with Keyboard::setEventCallback
	*/
	class _OISExport KeyListener
	{
	public:
		virtual ~KeyListener() {}
		virtual bool keyPressed( const KeyEvent &arg ) = 0;
		virtual bool keyReleased( const KeyEvent &arg ) = 0;		
	};

	/**
		To recieve buffered mouse input, derive a class from this, and implement the
		methods here. Then set the call back to your Mouse instance with Mouse::setEventCallback
	*/
	class _OISExport MouseListener
	{
	public:
		virtual ~MouseListener() {}
		virtual bool mouseMoved( const MouseEvent &arg ) = 0;
		virtual bool mousePressed( const MouseEvent &arg, MouseButtonID id ) = 0;
		virtual bool mouseReleased( const MouseEvent &arg, MouseButtonID id ) = 0;
	};

	/**
		To recieve buffered joystick input, derive a class from this, and implement the
		methods here. Then set the call back to your JoyStick instance with JoyStick::setEventCallback
		Each JoyStick instance can use the same callback class, as a devID number will be provided
		to differentiate between connected joysticks. Of course, each can have a seperate 
		callback instead.		
	*/
	class _OISExport JoyStickListener
	{
	public:
		virtual ~JoyStickListener() {}

		virtual bool buttonPressed( const JoyStickEvent &arg, int button ) = 0;
		virtual bool buttonReleased( const JoyStickEvent &arg, int button ) = 0;

		virtual bool axisMoved( const JoyStickEvent &arg, int axis ) = 0;
		virtual bool povMoved( const JoyStickEvent &arg, int povID ) = 0;
	};
}
#endif //_OIS_EVENTHEADERS_
