/*
The zlib/lyibpng License

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
#include "OISKeyboard.h"
#include "OISException.h"

using namespace OIS;
//----------------------------------------------------------------------//
Keyboard::Keyboard() : mModifiers(0), listener(0)
{
	setKeyMapping( English );
}

//----------------------------------------------------------------------//
void Keyboard::setKeyMapping( KeyMapping km )
{ 
	mMapping = km;
	mMap.clear();
	//Load this in as a file? - makes adding newer maps easier
	if( mMapping == English )
	{
		//Load up/init character conversion for Printable text
		mMap.reserve(170);
		mMap.push_back(0);
		mMap.push_back(0);
		mMap.push_back('1'); mMap.push_back('2'); mMap.push_back('3'); 
		mMap.push_back('4'); mMap.push_back('5'); mMap.push_back('6');	
		mMap.push_back('7'); mMap.push_back('8'); mMap.push_back('9');
		mMap.push_back('0');
		mMap.push_back('-'); mMap.push_back('=');
		mMap.push_back(0); mMap.push_back(0);
		mMap.push_back('q'); mMap.push_back('w'); mMap.push_back('e');
		mMap.push_back('r'); mMap.push_back('t'); mMap.push_back('y');
		mMap.push_back('u'); mMap.push_back('i'); mMap.push_back('o');
		mMap.push_back('p');
		mMap.push_back('['); mMap.push_back(']'); mMap.push_back(0);
		mMap.push_back(0);
		mMap.push_back('a'); mMap.push_back('s'); mMap.push_back('d');
		mMap.push_back('f'); mMap.push_back('g'); mMap.push_back('h');
		mMap.push_back('j'); mMap.push_back('k'); mMap.push_back('l');
		mMap.push_back(';'); mMap.push_back('\''); mMap.push_back('`');
		mMap.push_back(0); mMap.push_back('\\'); mMap.push_back('z');
		mMap.push_back('x'); mMap.push_back('c'); mMap.push_back('v');
		mMap.push_back('b'); mMap.push_back('n'); mMap.push_back('m');
		mMap.push_back(','); mMap.push_back('.'); mMap.push_back('/');
		mMap.push_back(0); mMap.push_back('*'); mMap.push_back(0);
		mMap.push_back(' '); mMap.push_back(0); mMap.push_back(0);
		mMap.push_back(0); mMap.push_back(0); mMap.push_back(0);
		mMap.push_back(0); mMap.push_back(0); mMap.push_back(0);
		mMap.push_back(0); mMap.push_back(0); mMap.push_back(0);
		mMap.push_back(0); mMap.push_back(0); mMap.push_back('7');
		mMap.push_back('8'); mMap.push_back('9'); mMap.push_back('-');
		mMap.push_back('4'); mMap.push_back('5'); mMap.push_back('6');
		mMap.push_back('+'); mMap.push_back('1'); mMap.push_back('2');
		mMap.push_back('3'); mMap.push_back('0'); mMap.push_back('.');
		//Shift state values
		mMap.push_back(0);
		mMap.push_back(0);
		mMap.push_back('!'); mMap.push_back('@'); mMap.push_back('#');
		mMap.push_back('$'); mMap.push_back('%'); mMap.push_back('^');
		mMap.push_back('&'); mMap.push_back('*'); mMap.push_back('(');
		mMap.push_back(')');
		mMap.push_back('_'); mMap.push_back('+');
		mMap.push_back(0); mMap.push_back(0);
		mMap.push_back('Q'); mMap.push_back('W'); mMap.push_back('E');
		mMap.push_back('R'); mMap.push_back('T'); mMap.push_back('Y');
		mMap.push_back('U'); mMap.push_back('I'); mMap.push_back('O');
		mMap.push_back('P');
		mMap.push_back('{'); mMap.push_back('}'); mMap.push_back(0);
		mMap.push_back(0);
		mMap.push_back('A'); mMap.push_back('S'); mMap.push_back('D');
		mMap.push_back('F'); mMap.push_back('G'); mMap.push_back('H');
		mMap.push_back('J'); mMap.push_back('K'); mMap.push_back('L');
		mMap.push_back(':'); mMap.push_back('"'); mMap.push_back('~');
		mMap.push_back(0); mMap.push_back('|'); mMap.push_back('Z');
		mMap.push_back('X'); mMap.push_back('C'); mMap.push_back('V');
		mMap.push_back('B'); mMap.push_back('N'); mMap.push_back('M');
		mMap.push_back('<'); mMap.push_back('>'); mMap.push_back('?');
		mMap.push_back(0); mMap.push_back('*'); mMap.push_back(0);
		mMap.push_back(' '); mMap.push_back(0); mMap.push_back(0);
		mMap.push_back(0); mMap.push_back(0); mMap.push_back(0);
		mMap.push_back(0); mMap.push_back(0); mMap.push_back(0);
		mMap.push_back(0); mMap.push_back(0); mMap.push_back(0);
		mMap.push_back(0); mMap.push_back(0); mMap.push_back('7');
		mMap.push_back('8'); mMap.push_back('9'); mMap.push_back('-');
		mMap.push_back('4'); mMap.push_back('5'); mMap.push_back('6');
		mMap.push_back('+'); mMap.push_back('1'); mMap.push_back('2');
		mMap.push_back('3'); mMap.push_back('0'); mMap.push_back('.');

		//Load up/init character conversion for keycodes to string
		mStringMap[KC_ESCAPE] = "ESC";
		mStringMap[KC_1] = "1";
		mStringMap[KC_2] = "2";
		mStringMap[KC_3] = "3"; 
		mStringMap[KC_4] = "4";
		mStringMap[KC_5] = "5";
		mStringMap[KC_6] = "6";
		mStringMap[KC_7] = "7";
		mStringMap[KC_8] = "8";
		mStringMap[KC_9] = "9";
		mStringMap[KC_0] = "0";
		mStringMap[KC_MINUS] = "-";
		mStringMap[KC_EQUALS] = "=";
		mStringMap[KC_BACK] = "Back";
		mStringMap[KC_TAB] = "Tab";
		mStringMap[KC_Q] = "Q";
		mStringMap[KC_W] = "W";
		mStringMap[KC_E] = "E";
		mStringMap[KC_R] = "R";
		mStringMap[KC_T] = "T";
		mStringMap[KC_Y] = "Y";
		mStringMap[KC_U] = "U";
		mStringMap[KC_I] = "I";
		mStringMap[KC_O] = "O";
		mStringMap[KC_P] = "P";
		mStringMap[KC_LBRACKET] = "[";
		mStringMap[KC_RBRACKET] = "]";
		mStringMap[KC_RETURN] = "Return";
		mStringMap[KC_LCONTROL] = "LCtrl";
		mStringMap[KC_A] = "A";
		mStringMap[KC_S] = "S";
		mStringMap[KC_D] = "D";
		mStringMap[KC_F] = "F";
		mStringMap[KC_G] = "G";
		mStringMap[KC_H] = "H";
		mStringMap[KC_J] = "J";
		mStringMap[KC_K] = "K";
		mStringMap[KC_L] = "L";
		mStringMap[KC_SEMICOLON] = ";";
		mStringMap[KC_APOSTROPHE] = "'";
		mStringMap[KC_GRAVE] = "Grave";
		mStringMap[KC_LSHIFT] = "LShift";
		mStringMap[KC_BACKSLASH] = "\\";
		mStringMap[KC_Z] = "Z";
		mStringMap[KC_X] = "X";
		mStringMap[KC_C] = "C";
		mStringMap[KC_V] = "V";
		mStringMap[KC_B] = "B";
		mStringMap[KC_N] = "N";
		mStringMap[KC_M] = "M";
		mStringMap[KC_COMMA] = ",";
		mStringMap[KC_PERIOD] = ".";
		mStringMap[KC_SLASH] = "/";
		mStringMap[KC_RSHIFT] = "RShift";
		mStringMap[KC_MULTIPLY] = "*";
		mStringMap[KC_LMENU] = "LAlt";
		mStringMap[KC_SPACE] = "Space";
		mStringMap[KC_CAPITAL] = "CapsLock";
		mStringMap[KC_F1] = "F1";
		mStringMap[KC_F2] = "F2";
		mStringMap[KC_F3] = "F3";
		mStringMap[KC_F4] = "F4";
		mStringMap[KC_F5] = "F5";
		mStringMap[KC_F6] = "F6";
		mStringMap[KC_F7] = "F7";
		mStringMap[KC_F8] = "F8";
		mStringMap[KC_F9] = "F9";
		mStringMap[KC_F10] = "F10";
		mStringMap[KC_NUMLOCK] = "Numlock";
		mStringMap[KC_SCROLL] = "Scr Lock";
		mStringMap[KC_NUMPAD7] = "NP 7";
		mStringMap[KC_NUMPAD8] = "NP 8";
		mStringMap[KC_NUMPAD9] = "NP 9";
		mStringMap[KC_SUBTRACT] = "NP -";
		mStringMap[KC_NUMPAD4] = "NP 4";
		mStringMap[KC_NUMPAD5] = "NP 5";
		mStringMap[KC_NUMPAD6] = "NP 6";
		mStringMap[KC_ADD] = "+";
		mStringMap[KC_NUMPAD1] = "NP 1";
		mStringMap[KC_NUMPAD2] = "NP 2";
		mStringMap[KC_NUMPAD3] = "NP 3";
		mStringMap[KC_NUMPAD0] = "NP 0";
		mStringMap[KC_DECIMAL] = "NP Dec";
		mStringMap[KC_OEM_102] = "OEM 102";
		mStringMap[KC_F11] = "F11";
		mStringMap[KC_F12] = "F12";
		mStringMap[KC_F13] = "F13";
		mStringMap[KC_F14] = "F14";
		mStringMap[KC_F15] = "F15";
		mStringMap[KC_NUMPADEQUALS] = "NP =";
		mStringMap[KC_PREVTRACK] = "Prev";
		mStringMap[KC_AT] = "AT";
		mStringMap[KC_COLON] = ":";
		mStringMap[KC_UNDERLINE] = "_";
		mStringMap[KC_STOP] = "Stop";
		mStringMap[KC_AX] = "Ax";
		mStringMap[KC_NEXTTRACK] = "Next";
		mStringMap[KC_NUMPADENTER] = "NP Enter";
		mStringMap[KC_RCONTROL] = "RControl";
		mStringMap[KC_MUTE] = "Mute";
		mStringMap[KC_CALCULATOR] = "Calc";
		mStringMap[KC_PLAYPAUSE] = "Play/Pause";
		mStringMap[KC_MEDIASTOP] = "Stop";
		mStringMap[KC_VOLUMEDOWN] = "VDown";
		mStringMap[KC_VOLUMEUP] = "VUp";
		mStringMap[KC_WEBHOME] = "WHome";
		mStringMap[KC_NUMPADCOMMA] = "NP ,";
		mStringMap[KC_DIVIDE] = "/";
		mStringMap[KC_SYSRQ] = "SysRq";
		mStringMap[KC_RMENU] = "RAlt";
		mStringMap[KC_PAUSE] = "Pause";
		mStringMap[KC_HOME] = "Home";
		mStringMap[KC_UP] = "Up";
		mStringMap[KC_PGUP] = "PgUp";
		mStringMap[KC_LEFT] = "Left";
		mStringMap[KC_RIGHT] = "Right";
		mStringMap[KC_END] = "End";
		mStringMap[KC_DOWN] = "Down";
		mStringMap[KC_PGDOWN] = "PgDown";
		mStringMap[KC_INSERT] = "Ins";
		mStringMap[KC_DELETE] = "Del";
		mStringMap[KC_LWIN] = "LWin";
		mStringMap[KC_RWIN] = "RWin";
		mStringMap[KC_APPS] = "Apps";
		mStringMap[KC_POWER] = "Power";
		mStringMap[KC_SLEEP] = "Sleep";
		mStringMap[KC_WAKE] = "Wake";
		mStringMap[KC_WEBSEARCH] = "WSearch";
		mStringMap[KC_WEBFAVORITES] = "WFav";
		mStringMap[KC_WEBREFRESH] = "WRef";
		mStringMap[KC_WEBSTOP] = "WStop";
		mStringMap[KC_WEBFORWARD] = "WFor";
		mStringMap[KC_WEBBACK] = "WBack";
		mStringMap[KC_MYCOMPUTER] = "MyComp.";
		mStringMap[KC_MAIL] = "Mail";
		mStringMap[KC_MEDIASELECT] = "MSelect";
	}
}

//----------------------------------------------------------------------//
unsigned short Keyboard::lookupCharMapping( KeyCode kc )
{
	unsigned short key;

	if( mMapping == English )
	{
		unsigned int middle = mMap.size() >> 1;
		if( kc >= middle ) 
			return 0;

		key = mMap[ kc + (middle * (mModifiers & Shift)) ];
	}
	else
		OIS_EXCEPT(E_General, "Keyboard::lookupCharMapping unsupported KeyMap!");

	return key;
}

const std::string ois_unknown_string = "Unknown";
//----------------------------------------------------------------------//
const std::string& Keyboard::getAsString( KeyCode kc )
{
	std::map<KeyCode, std::string>::iterator i = mStringMap.find( kc );

	if( i != mStringMap.end() )
		return i->second;
	else
		return ois_unknown_string;
}
