/*
	OgreNewt library - connecting Ogre and Newton!

	Demo03_CollisionCallbacks - the material system in Newton is extremely powerful.  by assigning
	materials to bodies, you can define all kinds of specific behavior when objects collide.  you can use
	material callbacks to create hot spots, or make sound effects, or spawn particles, etc.  this example
	uses a conveyor belt as an example.  look at the conveyorMatCallback to see how it's implemented.
*/
#include ".\OgreNewtonApplication.h"
#include ".\OgreNewtonFrameListener.h"

#include <OgreNewt.h>



OgreNewtonApplication::OgreNewtonApplication(void)
{
	m_World = new OgreNewt::World();

}

OgreNewtonApplication::~OgreNewtonApplication(void)
{
	std::vector<conveyorBelt*>::iterator it;
	for (it=mBelts.begin(); it!=mBelts.end(); it++)
	{
		conveyorBelt* belt = (*it);
		delete belt;
	}

	delete m_World;
}



void OgreNewtonApplication::createScene()
{

	// sky box.
	mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");

	// shadows on.
	mSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_MODULATIVE );

	createMaterials();

	// lets make a few conveyor belt objects.
	conveyorBelt* belt = new conveyorBelt();
	belt->init( Ogre::String("ConveyorBelt1"), mSceneMgr, m_World, Ogre::Vector3(15,0.3,3), Ogre::Vector3(1,0,0), 2.0, Ogre::Vector3(0,2,0),
		Ogre::Quaternion(Ogre::Quaternion::IDENTITY), mMatConveyor, BT_CONVEYOR );
	mBelts.push_back( belt );

	belt = new conveyorBelt();
	belt->init( Ogre::String("ConveyorBelt2"), mSceneMgr, m_World, Ogre::Vector3(23,0.3,4), Ogre::Vector3(-1,0,0), 6.0, Ogre::Vector3(3,-1,0),
		Ogre::Quaternion(Ogre::Quaternion::IDENTITY), mMatConveyor, BT_CONVEYOR );
	mBelts.push_back( belt );


	// floor object!
	Entity* floor;
	SceneNode* floornode;
	floor = mSceneMgr->createEntity("Floor", "simple_terrain.mesh" );
	floornode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "FloorNode" );
	floornode->attachObject( floor );
	floor->setMaterialName( "Simple/BeachStones" );

	floor->setCastShadows( false );

	//Ogre::Vector3 siz(100.0, 10.0, 100.0);
	OgreNewt::Collision* col = new OgreNewt::CollisionPrimitives::TreeCollision( m_World, floornode, true );
	OgreNewt::Body* bod = new OgreNewt::Body( m_World, col );
	
	//floornode->setScale( siz );
	bod->attachToNode( floornode );
	bod->setPositionOrientation( Ogre::Quaternion::IDENTITY, Ogre::Vector3(0.0,-10.0,0.0) );
	

	// position camera
	msnCam = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	msnCam->attachObject( mCamera );
	mCamera->setPosition(0.0, 0.0, 0.0);
	msnCam->setPosition( 0.0, 2.0, 22.0);

	//make a light
	Ogre::Light* light;

	light = mSceneMgr->createLight( "Light1" );
	light->setType( Ogre::Light::LightTypes::LT_POINT );
	light->setPosition( Ogre::Vector3(0.0, 100.0, 100.0) );



}

void OgreNewtonApplication::createMaterials()
{
	mMatDefault = m_World->getDefaultMaterialID();
	mMatConveyor = new OgreNewt::MaterialID( m_World );

	mMatPairDefaultConveyor = new OgreNewt::MaterialPair( m_World, mMatDefault, mMatConveyor );
	mConveyorCallback = new conveyorMatCallback( BT_CONVEYOR );
	mMatPairDefaultConveyor->setContactCallback( mConveyorCallback );
	mMatPairDefaultConveyor->setDefaultFriction( 1.5, 1.4 );

}


void OgreNewtonApplication::createFrameListener()
{
	mFrameListener = new OgreNewtonFrameListener( mWindow, mCamera, mSceneMgr, m_World, msnCam );
	mRoot->addFrameListener(mFrameListener);

	mNewtonListener = new OgreNewt::BasicFrameListener( mWindow, mCamera, mSceneMgr, m_World, 60 );
	mRoot->addFrameListener(mNewtonListener);
}
