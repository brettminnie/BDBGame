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
#include "linux/LinuxMouse.h"
#include "linux/LinuxInputManager.h"
#include "OISException.h"
#include "OISEvents.h"

using namespace OIS;

//-------------------------------------------------------------------//
LinuxMouse::LinuxMouse(bool buffered, bool grab, bool hide)
{
	mBuffered = buffered;
	mType = OISMouse;
	listener = 0;
	display = 0;
	window = 0;
	cursor = 0;

	grabMouse = grab;
	hideMouse = hide;
}

//-------------------------------------------------------------------//
void LinuxMouse::_initialize()
{
	//Clear old state
	mState.clear();
	moved = false;
	timeStamp = 0;
	oldX = oldY = oldZ = 0;
	xAbsMouseX = xAbsMouseY = 0;

	if( display ) XCloseDisplay(display);
	display = 0;
	window = static_cast<LinuxInputManager*>(InputManager::getSingletonPtr())->_getWindow();

	//Create our local X listener connection
	if( !(display = XOpenDisplay(0)) )
		OIS_EXCEPT(E_General, "LinuxMouse::_initialize >> Error opening X!");

	//Set it to recieve Mouse Input events
	if( XSelectInput(display, window, ButtonPressMask | ButtonReleaseMask | PointerMotionMask) == BadWindow )
		OIS_EXCEPT(E_General, "LinuxMouse::_initialize >> X error!");

	//Warp mouse inside window
	XWarpPointer(display, None, window, 0, 0, 0, 0, 0, 0);

	//Create a blank cursor:
	Pixmap bm_no;
	XColor black, dummy;
	Colormap colormap;
	static char no_data[] = { 0,0,0,0,0,0,0,0 };

	colormap = DefaultColormap( display, DefaultScreen(display) );
	XAllocNamedColor( display, colormap, "black", &black, &dummy );
	bm_no = XCreateBitmapFromData( display, window, no_data, 8, 8 );
	cursor = XCreatePixmapCursor( display, bm_no, bm_no, &black, &black, 0, 0 );

	grab( grabMouse );
	hide( hideMouse );

	mouseFocusLost = false;
}

//-------------------------------------------------------------------//
LinuxMouse::~LinuxMouse()
{
	if( display )
	{
		grab(false);
		hide(false);
		XFreeCursor(display, cursor);
		XCloseDisplay(display);
	}
}

//-------------------------------------------------------------------//
void LinuxMouse::setBuffered(bool buffered)
{
	mBuffered = buffered;
}

//-------------------------------------------------------------------//
void LinuxMouse::capture()
{
	XEvent event;

	//Clear old relative state values
	mState.relX = mState.relY = mState.relZ = 0;

	//Poll x11 for events mouse events
	while( XPending(display) > 0 ) 
	{
		XNextEvent(display, &event);
		if( _injectXEvent( event ) == false )
			return;
	}

	if( moved == true )
	{
		//Compute this frames Relative X & Y motion
		mState.relX = xAbsMouseX - oldX;
		mState.relY = xAbsMouseY - oldY;
		//Z is just accumulitive
		mState.abZ += mState.relZ;

		//Store old values for next time to compute relative motion
		oldX = xAbsMouseX;
		oldY = xAbsMouseY;

		if( mBuffered && listener )
			listener->mouseMoved( MouseEvent( this, timeStamp, mState ) );

		//Figure out Absoulte values
		mState.abX += mState.relX;
		mState.abY += mState.relY;
		mState.abZ += mState.relZ;

		if( grabMouse )
		{
			//We only need to clip when we are warping mouse around
			if( mState.abX < 0 ) //Clip X
				mState.abX = 0;
			else if( mState.abX > mState.width )
				mState.abX = mState.width;
			if( mState.abY < 0 ) //Clip Y
				mState.abY = 0;
			else if( mState.abY > mState.height )
				mState.abY = mState.height;

			if( mouseFocusLost == false )
			{
				//Keep mouse in window
				if( xAbsMouseX < 5 || xAbsMouseX > mState.width - 5 ||
				    xAbsMouseY < 5 || xAbsMouseY > mState.height - 5 )
				{
					oldX = xAbsMouseX = mState.width >> 1;
					oldY = xAbsMouseY = mState.height >> 1;
					XWarpPointer(display, None, window, 0, 0, 0, 0,
					     xAbsMouseX, xAbsMouseY);
				}
			}
		}
	}

	moved = false;

	//Check for losing/gaining mouse grab focus (alt-tab, etc)
	if( grabMouse )
	{
		if( static_cast<LinuxInputManager*>(InputManager::getSingletonPtr())->_getGrabState() )
		{
			if( mouseFocusLost ) //We just regained mouse grab focus
			{
				grab( true );
				hide( hideMouse );
				mouseFocusLost = false;
			}
		}
		else
		{
			if( mouseFocusLost == false ) //We just lost mouse grab focus
			{
				grab( false );
				hide( false );
				mouseFocusLost = true;
			}
		}
	}
}

//-------------------------------------------------------------------//
bool LinuxMouse::_injectXEvent(XEvent &event)
{
	//X11 Button Events: 1=left 2=middle 3=right; Our Bit Postion: 1=Left 2=Right 3=Middle
	char mask[4] = {0,1,4,2};

	if( event.type == MotionNotify )
	{
		xAbsMouseX = event.xmotion.x;
		xAbsMouseY = event.xmotion.y;
		timeStamp = event.xmotion.time;
		moved = true;
	}
	else if( event.type == ButtonPress )
	{
		static_cast<LinuxInputManager*>(InputManager::getSingletonPtr())->_setGrabState(true);

		if( event.xbutton.button < 4 )
		{
			mState.buttons |= mask[event.xbutton.button];

			if( mBuffered && listener )
				return listener->mousePressed( MouseEvent( this, event.xbutton.time, mState ), (MouseButtonID)(mask[event.xbutton.button] >> 1));
		}
	}
	else if( event.type == ButtonRelease )
	{
		if( event.xbutton.button < 4 )
		{
			mState.buttons &= ~mask[event.xbutton.button];
			if( mBuffered && listener )
				return listener->mouseReleased( MouseEvent( this, event.xbutton.time, mState ), (MouseButtonID)(mask[event.xbutton.button] >> 1));
		}
		//The Z axis gets pushed/released pair message (this is up)
		else if( event.xbutton.button == 4 )
		{
			mState.relZ += 120;
			moved = true;
			timeStamp = event.xbutton.time;
		}
		//The Z axis gets pushed/released pair message (this is down)
		else if( event.xbutton.button == 5 )
		{
			mState.relZ -= 120;
			moved = true;
			timeStamp = event.xbutton.time;
		}
	}

	return true;
}

//-------------------------------------------------------------------//
void LinuxMouse::grab(bool grab)
{
	if( grab )
		XGrabPointer(display, window, True, 0, GrabModeAsync, GrabModeAsync, window, None, CurrentTime);
	else
		XUngrabPointer(display, CurrentTime);
}

//-------------------------------------------------------------------//
void LinuxMouse::hide(bool hide)
{
	if( hide )
		XDefineCursor(display, window, cursor);
	else
		XUndefineCursor(display, window);
}
