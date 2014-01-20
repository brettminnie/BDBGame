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
//Check to see which JoyStick support is being compiled in
#include "OISConfig.h"
#ifdef OIS_JOYEVENTS

#include "linux/LinuxJoyStickEvents.h"
#include "linux/LinuxInputManager.h"
#include "linux/LinuxForceFeedback.h"
#include "linux/EventHelpers.h"

#include "OISEvents.h"
#include "OISException.h"

#include <fcntl.h>        //Needed to Open a file descriptor
#include <linux/input.h>

#include <sstream>

using namespace OIS;

//-------------------------------------------------------------------//
LinuxJoyStick::LinuxJoyStick(bool buffered, const JoyStickInfo& js)
{
	mBuffered = buffered;
	mType = OISJoyStick;

	mDevID = js.devId;
	mJoyStick = js.joyFileD;
	mVendor = js.vendor;
	numAxes = js.axes;
	numButtons = js.buttons;
	
	ff_effect = 0;
}

//-------------------------------------------------------------------//
LinuxJoyStick::~LinuxJoyStick()
{
	EventUtils::removeForceFeedback( &ff_effect );
}

//-------------------------------------------------------------------//
void LinuxJoyStick::_initialize()
{
	//Clear old joy state
	mState.clear();

	//This will create and new us a force feedback structure if it exists
	EventUtils::enumerateForceFeedback( mJoyStick, &ff_effect );

	if( mJoyStick == -1 )
		OIS_EXCEPT(E_InputDeviceNonExistant, "LinuxJoyStick::_initialize() >> JoyStick Not Found!");
}
#define MAX_AXES 6
//LinuxReports / Name / ActualAxisNumber (As we refer to axis with XY - Axes really)
//0	 X Axis  0
//1	 Y Axis  0
//2	 Z Axis  2
//3  Rx Axis 1
//4	 Ry Axis 1
//5	 Rz Axis 3
//Need to add support for all the axes.. todo xxx wip
#define aX 0
#define aY 1
#define aZ 2

//Used for determining which Axis Pair to assign to
const char AxisMask[6] = {0,0,2,1,1,3};
//Used for determining if X, Y, or Z component of axis
const char XYZMask[6] = {aX,aY,aZ,aX,aY,aZ};

//-------------------------------------------------------------------//
void LinuxJoyStick::capture()
{
	if( mJoyStick == -1 )
		OIS_EXCEPT(E_General, "LinuxJoyStick::capture() >> device has not been created!");

	//Used to determine if an axis has been changed and needs an event	
	bool axisMoved[MAX_AXES] = {false,false,false,false,false,false};

	input_event js;
	//We are in non blocking mode - we just read once, and try to fill up buffer
	while( read(mJoyStick, &js, sizeof(struct input_event)) == sizeof(struct input_event) )
	{
		switch(js.type)
		{
		case EV_KEY:
			//Normalise the button value
			int button;
			//Temporary - This should be created in a map during startup/enumeration
			//mapping Buttons to button number
			if( js.code < BTN_GAMEPAD )
				js.code = BTN_THUMBR + 1;
			button = js.code - BTN_GAMEPAD;
			
			if(js.value)
			{
				mState.buttons |= (1 << button);
				if( mBuffered && listener ) 
					if(!listener->buttonPressed(JoyStickEvent(this,0,mState), button)) return;
			}
			else
			{
				mState.buttons &= ~(1 << button);
				if( mBuffered && listener ) 
					if(!listener->buttonReleased(JoyStickEvent(this,0,mState), button)) return;
			}
			break;
		case EV_ABS:
			if( js.code < MAX_AXES )
			{
				axisMoved[AxisMask[js.code]] = true;

				if( XYZMask[js.code] == aX )
					mState.mAxes[AxisMask[js.code]].abX = js.value;
				else if( XYZMask[js.code] == aY )
					mState.mAxes[AxisMask[js.code]].abY = js.value;
				else if( XYZMask[js.code] == aZ )
					mState.mAxes[AxisMask[js.code]].abZ = js.value;
			}
			break;
		case EV_REL:
		default: break;
		}
	}

	//All axes are combined into one movement per pair
	if( mBuffered && listener )
		for( int i = 0; i < 6; ++i )
			if( axisMoved[i] )
				listener->axisMoved( JoyStickEvent(this,0,mState),i);
}

//-------------------------------------------------------------------//
void LinuxJoyStick::setBuffered(bool buffered)
{
	if( buffered != mBuffered )
	{
		mBuffered = buffered;
		_initialize();
	}
}

//-------------------------------------------------------------------//
JoyStickInfo LinuxJoyStick::_getJoyInfo()
{
	JoyStickInfo js;

	js.devId = mDevID;
	js.joyFileD = mJoyStick;
	js.vendor = mVendor;
	js.axes = numAxes;
	js.buttons = numButtons;

	return js;
}

//-------------------------------------------------------------------//
JoyStickInfoList LinuxJoyStick::_scanJoys()
{
	JoyStickInfoList joys;

	//Search through all of the event devices.. and identify which ones are joysticks
	//xxx move this to InputManager, as it can also scan all other events
	for(int i = 0; i < OIS_MAX_DEVICES; ++i )
	{
		std::stringstream s;
		s << "/dev/input/event" << i;

		int fd = open( s.str().c_str(), O_RDONLY |O_NONBLOCK );
	
		try
		{
			JoyStickInfo js;
			if( EventUtils::isJoyStick(fd, js) )
				joys.push_back(js);
			else
				close(fd);
		}
		catch(...)
		{
			close(fd);
			break;
		}
	}

	return joys;
}

//-------------------------------------------------------------------//
void LinuxJoyStick::_clearJoys(JoyStickInfoList &joys)
{
	for(JoyStickInfoList::iterator i = joys.begin(); i != joys.end(); ++i)
		close(i->joyFileD);
	joys.clear();
}

//-------------------------------------------------------------------//
Interface* LinuxJoyStick::queryInterface(Interface::IType type)
{
	if( ff_effect && type == Interface::ForceFeedback )
		return ff_effect;

	return 0;
}

#endif //#ifdef OIS_JOYEVENTS
