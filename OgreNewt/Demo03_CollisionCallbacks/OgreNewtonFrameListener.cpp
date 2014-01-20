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


	if (timer <= 0.0)
	{
		Ogre::Vector3 size( 0.2 + rand() % 2, 0.2 + rand() % 2, 0.2 + rand() % 2 );
		Ogre::Real mass = size.length();
		Entity* ent;
		SceneNode* node;
		Ogre::String name;
		Ogre::Vector3 pos = msnCam->getWorldPosition();

		name = "Body "+Ogre::StringConverter::toString( count++ );

		ent = mSceneMgr->createEntity( name, "box.mesh" );
		node = mSceneMgr->getRootSceneNode()->createChildSceneNode( name );
		node->attachObject( ent );
		node->setScale( size );

		ent->setMaterialName( "Simple/BumpyMetal" );

		OgreNewt::Collision* col = new OgreNewt::CollisionPrimitives::Box( m_World, size );
		OgreNewt::Body* body = new OgreNewt::Body( m_World, col );
		delete col;

		Ogre::Vector3 inertia = OgreNewt::MomentOfInertia::CalcBoxSolid( mass, size );
		body->setMassMatrix( mass, inertia );
		body->attachToNode( node );
		body->setStandardForceCallback();
		body->setPositionOrientation( Ogre::Quaternion(Ogre::Quaternion::IDENTITY), Ogre::Vector3(-5,8,0) );

		timer = 1.5;
	}
	

	timer -= evt.timeSinceLastFrame;


	if (mInputDevice->isKeyDown(Ogre::KC_ESCAPE))
		return false;

	return true;
}
