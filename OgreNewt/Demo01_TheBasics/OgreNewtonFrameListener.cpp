#include ".\ogrenewtonframelistener.h"


OgreNewtonFrameListener::OgreNewtonFrameListener(RenderWindow* win, Camera* cam, SceneManager* mgr, OgreNewt::World* W, SceneNode* ncam) :
		ExampleFrameListener(win,cam)
{
	m_World = W;
	msnCam = ncam;
	mSceneMgr = mgr;
}

OgreNewtonFrameListener::~OgreNewtonFrameListener(void)
{
}

bool OgreNewtonFrameListener::frameStarted(const FrameEvent &evt)
{
	// in this frame listener we control the camera movement, and allow the user to "shoot" cylinders
	// by pressing the space bar.  first the camera movement...
	Vector3 trans, strafe, vec;
	Quaternion quat;

	quat = msnCam->getOrientation();

	vec = Vector3(0.0,0.0,-0.5);
	trans = quat * vec;

	vec = Vector3(0.5,0.0,0.0);
	strafe = quat * vec;

	mInputDevice->capture();

	msnCam->pitch( Degree(mInputDevice->getMouseRelativeY() * -0.5) );
	msnCam->yaw( Degree(mInputDevice->getMouseRelativeX() * -0.5) );

	if (mInputDevice->isKeyDown(Ogre::KC_UP))
		msnCam->translate(trans);

	if (mInputDevice->isKeyDown(Ogre::KC_DOWN))
		msnCam->translate(trans * -1.0);

	if (mInputDevice->isKeyDown(Ogre::KC_LEFT))
		msnCam->translate(strafe * -1.0);

	if (mInputDevice->isKeyDown(Ogre::KC_RIGHT))
		msnCam->translate(strafe);

	// now "shoot" an object!
	if (mInputDevice->isKeyDown(Ogre::KC_SPACE))
	{
		if (timer <= 0.0)
		{
			// we get the position and direction from the camera...
			Ogre::Vector3 dir, vec;
			Ogre::Quaternion camorient = msnCam->getWorldOrientation();
			vec = Ogre::Vector3(0,0,-1);

			dir = camorient * vec;

			// then make the visual object (again a cylinder)
			Entity* ent;
			SceneNode* node;
			Ogre::String name;
			Ogre::Vector3 pos = msnCam->getWorldPosition();

			name = "Body "+Ogre::StringConverter::toString( count++ );

			ent = mSceneMgr->createEntity( name, "cylinder.mesh" );
			node = mSceneMgr->getRootSceneNode()->createChildSceneNode( name );
			node->attachObject( ent );

			ent->setMaterialName( "Simple/dirt01" );

			// again, make the collision shape.
			OgreNewt::Collision* col = new OgreNewt::CollisionPrimitives::Cylinder(m_World, 1, 1);
			
			// then make the rigid body.
			OgreNewt::Body* body = new OgreNewt::Body( m_World, col );

			//no longer need the collision shape object
			delete col;

			// something new: moment of inertia for the body.  this describes how much the body "resists"
			// rotation on each axis.  realistic values here make for MUCH more realistic results.  luckily
			// OgreNewt has some helper functions for calculating these values for many primitive shapes!
			Ogre::Vector3 inertia = OgreNewt::MomentOfInertia::CalcSphereSolid( 10.0, 1.0 );
			body->setMassMatrix( 10.0, inertia );

			// attach to the scene node.
			body->attachToNode( node );

			// this is a standard callback that simply add a gravitational force (-9.8*mass) to the body.
			body->setStandardForceCallback();

			// set the initial orientation and velocity!
			body->setPositionOrientation( camorient, pos );
			body->setVelocity( (dir * 50.0) );

			timer = 0.2;
		}
	}

	timer -= evt.timeSinceLastFrame;


	if (mInputDevice->isKeyDown(Ogre::KC_ESCAPE))
		return false;

	return true;
}
