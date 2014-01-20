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
#include "Win32/Win32InputManager.h"
#include "Win32/Win32KeyBoard.h"
#include "OISException.h"
#include "OISEvents.h"

using namespace OIS;

//-------------------------------------------------------------------//
Win32Keyboard::Win32Keyboard( IDirectInput8* pDI, bool buffered, DWORD coopSettings )
{
	mBuffered = buffered;
	mKeyboard = 0;
	mDirectInput = pDI;
	coopSetting = coopSettings;
	mType = OISKeyboard;
	listener = 0;

	//Clear our keyboard state buffer
	memset( &KeyBuffer, 0, 256 );
}

//-------------------------------------------------------------------//
void Win32Keyboard::_initialize()
{
	mModifiers = 0;

	if(FAILED(mDirectInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, NULL)))
		OIS_EXCEPT( E_General, "Win32Keyboard::Win32Keyboard >> Could not init device!");

	if(FAILED(mKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		OIS_EXCEPT( E_General, "Win32Keyboard::Win32Keyboard >> format error!");
	
	HWND hwin = ((Win32InputManager*)(InputManager::getSingletonPtr()))->getWindowHandle();
	
	if(FAILED(mKeyboard->SetCooperativeLevel( hwin, coopSetting)))
		OIS_EXCEPT( E_General, "Win32Keyboard::Win32Keyboard >> coop error!");

	if( mBuffered )
	{
		DIPROPDWORD dipdw;
		dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj        = 0;
		dipdw.diph.dwHow        = DIPH_DEVICE;
		dipdw.dwData            = DX_BUFFERSIZE;

		if (FAILED(mKeyboard->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph )))
			OIS_EXCEPT( E_General, "Win32Keyboard::Win32Keyboard >> buffer error!");
	}

	HRESULT hr = mKeyboard->Acquire();
	if(FAILED(hr) && hr != DIERR_OTHERAPPHASPRIO)
		OIS_EXCEPT( E_General, "Win32Keyboard::Win32Keyboard >> aquire error!");
}

//-------------------------------------------------------------------//
Win32Keyboard::~Win32Keyboard()
{
	if(mKeyboard)
	{
		mKeyboard->Unacquire();
		mKeyboard->Release();
		mKeyboard = 0;
	}
}

//-------------------------------------------------------------------//
bool Win32Keyboard::isKeyDown( KeyCode key )
{
	return (KeyBuffer[key] & 0x80) != 0;
}

//-------------------------------------------------------------------//
void Win32Keyboard::capture()
{
	if( mBuffered )
		_readBuffered();
	else
		_read();
}

//-------------------------------------------------------------------//
void Win32Keyboard::_readBuffered()
{
	DIDEVICEOBJECTDATA diBuff[DX_BUFFERSIZE];
	DWORD entries = DX_BUFFERSIZE;
	HRESULT hr;

	hr = mKeyboard->GetDeviceData( sizeof(DIDEVICEOBJECTDATA), diBuff, &entries, 0 );
	if( hr != DI_OK )
	{
		hr = mKeyboard->Acquire();
		while( hr == DIERR_INPUTLOST )
			hr = mKeyboard->Acquire();
		return;
	}

	if( FAILED(hr) )
		OIS_EXCEPT( E_General, "Win32Keyboard::_readBuffered() >> Problem with Device!" );

	//Update keyboard and modifier states.. And, if listener, fire events
	for(unsigned int i = 0; i < entries; ++i )
	{
		//If the listener returns false, that means that we are probably deleted...
		//send no more events and just leave as the this pointer is invalid now...
		bool ret = true;
		KeyCode kc = (KeyCode)diBuff[ i ].dwOfs;
			
		//Store result in our keyBuffer too
		KeyBuffer[kc] = diBuff[ i ].dwData;
			
		if( diBuff[ i ].dwData & 0x80 )
		{
			//Turn on modifier
			if( kc == KC_LCONTROL || kc == KC_RCONTROL )
				mModifiers |= Ctrl;
			else if( kc == KC_LSHIFT || kc == KC_RSHIFT )
				mModifiers |= Shift;
			else if( kc == KC_LMENU || kc == KC_RMENU )
				mModifiers |= Alt;

			//Fire off event
			if( listener )
				ret = listener->keyPressed( KeyEvent( this, diBuff[i].dwTimeStamp, kc ) );
		}
		else
		{
			//Turn off modifier
			if( kc == KC_LCONTROL || kc == KC_RCONTROL )
				mModifiers &= ~Ctrl;
			else if( kc == KC_LSHIFT || kc == KC_RSHIFT )
				mModifiers &= ~Shift;
			else if( kc == KC_LMENU || kc == KC_RMENU )
				mModifiers &= ~Alt;

			//Fire off event
			if( listener )
				ret = listener->keyReleased( KeyEvent( this, diBuff[i].dwTimeStamp, kc ) );
		}

		if(ret == false)
			break;
	}
}

//-------------------------------------------------------------------//
void Win32Keyboard::_read()
{
    HRESULT  hr = mKeyboard->GetDeviceState( sizeof(KeyBuffer), &KeyBuffer );

	if( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED )
	{
		hr = mKeyboard->Acquire();
		if (hr != DIERR_OTHERAPPHASPRIO)
			mKeyboard->GetDeviceState(sizeof(KeyBuffer), &KeyBuffer);
	}

	//Set Shift, Ctrl, Alt
	mModifiers = 0;
	if( isKeyDown(KC_LCONTROL) || isKeyDown(KC_RCONTROL) )
		mModifiers |= Ctrl;
	if( isKeyDown(KC_LSHIFT) || isKeyDown(KC_RSHIFT) )
		mModifiers |= Shift;
	if( isKeyDown(KC_LMENU) || isKeyDown(KC_RMENU) )
		mModifiers |= Alt;
}

//-------------------------------------------------------------------//
void Win32Keyboard::setBuffered(bool buffered)
{
	if( buffered != mBuffered )
	{
		if(mKeyboard)
		{
			mKeyboard->Unacquire();
			mKeyboard->Release();
			mKeyboard = 0;
		}
		
		mBuffered = buffered;
		_initialize();
	}
}
