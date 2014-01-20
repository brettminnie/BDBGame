/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber



		"BasicFrameListener"

		this is a simple Ogre FrameListener that will update the Newton world you supply it
		with, at a desired framerate, using a simple time-slicer.  this means the simulation
		should run at the same speed regardless of the actual framerate.  set the update_framerate
		to a higher value for a more accurate simluation.

		NOTE: update_framerate must be a value between [60,600]
*/

#pragma once
#include "ExampleFrameListener.h"

#include <OgreNewt_World.h>

namespace OgreNewt
{


// BasicFrameListener - updates the Newto World at the specified rate, with time-slicing, and
// also implements a simple debug view, press F3 to render Newto world with lines in 3D view.
class BasicFrameListener :
	public ExampleFrameListener
{
protected:
	OgreNewt::World* m_World;

	int desired_framerate;
	Ogre::Real m_update, m_elapsed;


public:
	BasicFrameListener(RenderWindow* win, Camera* cam, SceneManager* mgr, OgreNewt::World* W, int update_framerate = 60);
	~BasicFrameListener(void);

	bool frameStarted(const FrameEvent &evt);

};



}	// end NAMESPACE OgreNewt
