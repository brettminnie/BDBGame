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


	if (mInputDevice->isKeyDown(Ogre::KC_SPACE))
	{
		if (timer <= 0.0)
		{
			Ogre::Vector3 dir, vec;
			Ogre::Quaternion camorient = msnCam->getWorldOrientation();
			vec = Ogre::Vector3(0,0,-1);

			dir = camorient * vec;

			Entity* ent;
			SceneNode* node;
			Ogre::String name;
			Ogre::Vector3 pos = msnCam->getWorldPosition();

			name = "Body "+Ogre::StringConverter::toString( count++ );

			ent = mSceneMgr->createEntity( name, "ellipsoid.mesh" );
			node = mSceneMgr->getRootSceneNode()->createChildSceneNode( name );
			node->attachObject( ent );

			ent->setMaterialName( "Simple/dirt01" );

			OgreNewt::Collision* col = new OgreNewt::CollisionPrimitives::Ellipsoid( m_World, Ogre::Vector3(1,1,1) );
			OgreNewt::Body* body = new OgreNewt::Body( m_World, col );

			Ogre::Vector3 inertia = OgreNewt::MomentOfInertia::CalcSphereSolid( 10.0, 1.0 );
			body->setMassMatrix( 10.0, inertia );
			body->attachToNode( node );
			body->setStandardForceCallback();
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
