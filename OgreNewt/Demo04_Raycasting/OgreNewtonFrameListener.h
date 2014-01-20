#pragma once
#include "ExampleFrameListener.h"

#include <OgreNewt.h>


class OgreNewtonFrameListener :
	public ExampleFrameListener
{
protected:
	OgreNewt::World* m_World;
	SceneNode* msnCam;
	SceneManager* mSceneMgr;
	int count;
	float timer;

	bool dragging;
	Ogre::Vector3 dragPoint;
	Ogre::Real dragDist;
	OgreNewt::Body* dragBody;

	// for drawing 3D lines...
	Ogre::SceneNode* mDragLineNode;
	Line3D* mDragLine;

	void remove3DLine();

public:
	OgreNewtonFrameListener(RenderWindow* win, Camera* cam, SceneManager* mgr, OgreNewt::World* W, SceneNode* ncam);
	~OgreNewtonFrameListener(void);

	bool frameStarted(const FrameEvent &evt);

	// special callback for applying drag force to bodies.
	static void _cdecl dragCallback( OgreNewt::Body* me );

};
