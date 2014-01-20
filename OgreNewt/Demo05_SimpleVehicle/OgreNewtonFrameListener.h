#pragma once
#include "ExampleFrameListener.h"

#include <OgreNewt.h>
#include "SimpleVehicle.h"


class OgreNewtonFrameListener :
	public ExampleFrameListener
{
protected:
	OgreNewt::World* m_World;
	SceneNode* msnCam;
	SceneManager* mSceneMgr;
	int count;
	float timer;

	SimpleVehicle* mCar;

public:
	OgreNewtonFrameListener(RenderWindow* win, Camera* cam, SceneManager* mgr, OgreNewt::World* W, SceneNode* ncam, SimpleVehicle* car);
	~OgreNewtonFrameListener(void);

	bool frameStarted(const FrameEvent &evt);

};
