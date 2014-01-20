//////////////////////////////// OS Nuetral Headers ////////////////
#include "OISInputManager.h"
#include "OISException.h"
#include "OISKeyboard.h"
#include "OISMouse.h"
#include "OISJoyStick.h"
#include "OISEvents.h"

//Advanced Usage
#include "OISForceFeedback.h"

#include <iostream>
#include <vector>
#include <sstream>

////////////////////////////////////Needed Windows Headers////////////
#if defined OIS_WIN32_PLATFORM
#  define WIN32_LEAN_AND_MEAN
#  include "windows.h"
#  include "resource.h"
   LRESULT DlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//////////////////////////////////////////////////////////////////////
////////////////////////////////////Needed Linux Headers//////////////
#elif defined OIS_LINUX_PLATFORM
#  include <X11/Xlib.h>
   void checkX11Events();
#endif
//////////////////////////////////////////////////////////////////////
using namespace OIS;

//-- Some local prototypes --//
void doStartup();
void handleNonBufferedKeys();
void handleNonBufferedMouse();
void handleNonBufferedJoy( JoyStick* js );

//-- Some hacky globals --//
bool appRunning = true;	//Global Exit Flag

Keyboard *g_kb  = 0;		//Keyboard Device
Mouse	 *g_m   = 0;		//Mouse Device
JoyStick* g_joys[4] = {0,0,0,0};   //This demo supports up to 4 controllers
ForceFeedback* g_ff[4] = {0,0,0,0};//Array to hold ff interface for each joy

///// OS Specific Globals //////
#if defined OIS_WIN32_PLATFORM
  HWND hWnd = 0;
#elif defined OIS_LINUX_PLATFORM
  Display *xDisp = 0;
  Window xWin = 0;
#endif

//////////// Common Event handler class ////////
class EventHandler : public KeyListener, public MouseListener, public JoyStickListener
{
public:
	EventHandler() {}
	~EventHandler() {}
	bool keyPressed( const KeyEvent &arg ) {
		char keyChar = (char)((Keyboard*)(arg.device))->lookupCharMapping(arg.key);

		std::cout << "\nKeyPressed {" << arg.key
			<< ", " << ((Keyboard*)(arg.device))->getAsString(arg.key)
			<< "} || Character (" << keyChar << ")" << std::endl;
		return true;
	}
	bool keyReleased( const KeyEvent &arg ) {
		if( arg.key == KC_ESCAPE || arg.key == KC_Q ) 
			appRunning = false;
		//std::cout << "\nKeyReleased: " << arg.key << " time[" << arg.timeStamp << "]\n";
		return true;
	}
	bool mouseMoved( const MouseEvent &arg ) {
		return true;
	}
	bool mousePressed( const MouseEvent &arg, MouseButtonID id ) {
		std::cout << "\nMousePressed: " << id << " time[" << arg.timeStamp << "]";
		return true;
	}
	bool mouseReleased( const MouseEvent &arg, MouseButtonID id ) {
		//std::cout << "\nMouseReleased: " << id << " time[" << arg.timeStamp << "]";
		return true;
	}
	bool buttonPressed( const JoyStickEvent &arg, int button ) {
		std::cout << "\nJoy ButtonPressed: " << button << " time[" << arg.timeStamp << "]";
		return true;
	}
	bool buttonReleased( const JoyStickEvent &arg, int button ) {
		std::cout << "\nJoy ButtonReleased: " << button << " time[" << arg.timeStamp << "]";
		return true;
	}
	bool axisMoved( const JoyStickEvent &arg, int axis )
	{
		//std::cout << "\nJoy Axis: " << axis 
		//	  << " absolute X: " << arg.state.mAxes[axis].abX 
		//	  << " absolute Y: " << arg.state.mAxes[axis].abY
		//	  << " absolute Z: " << arg.state.mAxes[axis].abZ << std::endl;
		return true;
	}
	bool povMoved( const JoyStickEvent &arg, int pov )
	{
		std::cout << "\nJoy POV (" << pov + 1 
			<< ") Moved. Value = " << arg.state.mPOV[pov] << std::endl;
		return true;
	}
};

//Create a global instance
EventHandler handler;

int main()
{
	std::cout << "\n\n*** OIS Console Demo App is starting up... *** \n";
	try
	{
		doStartup();
		std::cout << "\nStartup done... Hit 'q' or ESC to exit (or joy button 1)\n\n";

		while(appRunning)
		{
			if( g_kb )
			{
				g_kb->capture();
				if( !g_kb->buffered() ) 
					handleNonBufferedKeys();
			}

			if( g_m )
			{
				g_m->capture();
				if( !g_m->buffered() ) 
					handleNonBufferedMouse();
			}
			
			for( int i = 0; i < 4 ; ++i )
			{
				if( g_joys[i] )
				{
					g_joys[i]->capture();
					if( !g_joys[i]->buffered() ) 
						handleNonBufferedJoy( g_joys[i] );
				}
			}

			//Throttle down CPU usage
			#if defined OIS_WIN32_PLATFORM
			  Sleep( 30 );
			  MSG  msg;
			  while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			  {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			  }
			#elif defined OIS_LINUX_PLATFORM
			  checkX11Events(); //To prove that we do not steal events
			  usleep( 300 );
			#endif
		}
	}
	catch( const Exception &ex )
	{
		#if defined OIS_WIN32_PLATFORM
		  MessageBox( NULL, ex.eText, "An exception has occured!", MB_OK | 
				MB_ICONERROR | MB_TASKMODAL);
		#else
		  std::cout << "\nOIS Exception Caught!\n" << "\t" << ex.eText << "[Line " 
			<< ex.eLine << " in " << ex.eFile << "]\nExiting App";
		#endif
	}

	if( InputManager::getSingletonPtr() )
	{
		InputManager::getSingletonPtr()->destroyInputObject( g_kb );
		InputManager::getSingletonPtr()->destroyInputObject( g_m );

		for(int i = 0; i < 4; ++i)
			InputManager::getSingletonPtr()->destroyInputObject( g_joys[i] );

		InputManager::destroyInputSystem();
	}

#if defined OIS_LINUX_PLATFORM
	// Be nice to X and clean up the x window
	XDestroyWindow(xDisp, xWin);
	XCloseDisplay(xDisp);
#endif

	return 0;
}

void doStartup()
{
	ParamList pl;

#if defined OIS_WIN32_PLATFORM
	//Create a capture window for Input Grabbing
	hWnd = CreateDialog( 0, MAKEINTRESOURCE(IDD_DIALOG1), 0,(DLGPROC)DlgProc);
	ShowWindow(hWnd, SW_SHOW);

	std::stringstream wnd;
	wnd << (size_t)hWnd; 

	pl.insert(std::make_pair( std::string("WINDOW"), wnd.str() ));
	
	//Default mode is foreground exclusive..but, we want to show mouse - so nonexclusive
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
	//Connects to default X window
	if( !(xDisp = XOpenDisplay(0)) )
		OIS_EXCEPT(E_General, "Error opening X!");
	//Create a window
	xWin = XCreateSimpleWindow(xDisp,DefaultRootWindow(xDisp), 0,0, 100,100, 0, 0, 0);
	//bind our connection to that window
	XMapWindow(xDisp, xWin);
	//Select what events we want to listen to locally
	XSelectInput(xDisp, xWin, StructureNotifyMask);
	XEvent evtent;
	do 
	{
		XNextEvent(xDisp, &evtent);
	} while(evtent.type != MapNotify);

	std::stringstream wnd;
	wnd << xWin;

	pl.insert(std::make_pair(std::string("WINDOW"), wnd.str()));

	//For this demo, show mouse and do not grab (confine to window)
//	pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
//	pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
#endif

	//This never returns null.. it will raise an exception on errors
	InputManager &im = *InputManager::createInputSystem(pl);

	std::cout << "Input Manager [" << im.inputSystemName() << "]" 
		<< "\n Numer of Mice: " << im.numMice()
		<< "\n Number of Keyboards: " << im.numKeyBoards()
		<< "\n Number of Joys/Pads = " << im.numJoysticks();

	if( im.numKeyBoards() > 0 )
	{
		g_kb = (Keyboard*)im.createInputObject( OISKeyboard, true );
		std::cout << "\n\nCreated buffered keyboard";
		g_kb->setEventCallback( &handler );
	}

	if( im.numMice() > 0 )
	{
		g_m = (Mouse*)im.createInputObject( OISMouse, true );
		std::cout << "\nCreated buffered mouse";
		g_m->setEventCallback( &handler );

		const MouseState &ms = g_m->getMouseState();
		ms.width = 100;
		ms.height = 100;
	}

	//This demo only uses at max 4 joys
	int numSticks = im.numJoysticks();
	if( numSticks > 4 )	numSticks = 4;

	for( int i = 0; i < numSticks; ++i ) 
	{
		g_joys[i] = (JoyStick*)im.createInputObject( OISJoyStick, true );
		g_joys[i]->setEventCallback( &handler );
		
		g_ff[i] = (ForceFeedback*)g_joys[i]->queryInterface( Interface::ForceFeedback );
		if( g_ff[i] )
		{
			std::cout << "\nCreated buffered joystick with ForceFeedback support.\n";
			//Dump out all the supported effects:
			const ForceFeedback::SupportedEffectList &list = g_ff[i]->getSupportedEffects();
			ForceFeedback::SupportedEffectList::const_iterator i = list.begin(),
				e = list.end();
			for( ; i != e; ++i)
				std::cout << "Force =  " << i->first << " Type = " << i->second << std::endl;
		}
		else
			std::cout << "\nCreated buffered joystick. **without** FF support";
	}
}

void handleNonBufferedKeys()
{
	if( g_kb->isKeyDown( KC_ESCAPE ) || g_kb->isKeyDown( KC_Q ) )
		appRunning = false;
}

void handleNonBufferedMouse()
{
	//Just dump the current mouse state
	const MouseState &ms = g_m->getMouseState();
	std::cout << "\nMouse: Abs(" << ms.abX << " " << ms.abY << " " << ms.abZ
		<< ") B: " << ms.buttons << " Rel(" << ms.relX << " " << ms.relY << " " << ms.relZ << ")";
}

void handleNonBufferedJoy( JoyStick* js )
{
	//Just dump the current joy state
	const JoyStickState &joy = js->getJoyStickState();
	std::cout << "\nJoyStick: button bits: " << joy.buttons << " ";
	for( int i = 0; i < 8; ++i )
		std::cout << " Axis " << i << " X: " << joy.mAxes[i].abX
				  << " Y: " << joy.mAxes[i].abY
				  << " Z: " << joy.mAxes[i].abZ << std::endl;
}

#if defined OIS_WIN32_PLATFORM
LRESULT DlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return FALSE;
}
#endif

#if defined OIS_LINUX_PLATFORM
//This is just here to show that you still recieve x11 events, as the lib only needs mouse/key events
void checkX11Events()
{
	XEvent event;

	//Poll x11 for events (keyboard and mouse events are caught here)
	while( XPending(xDisp) > 0 )
	{
		XNextEvent(xDisp, &event);
		//Handle Resize events
		if( event.type == ConfigureNotify )
		{
			if( g_m )
			{
				const MouseState &ms = g_m->getMouseState();
				ms.width = event.xconfigure.width;
				ms.height = event.xconfigure.height;
			}
		}
	}
}
#endif
