/*
	OgreNewt library - connecting Ogre and Newton!

	Demo02_Joints - basic demo that shows how to connect rigid bodies via joints.
*/

#include ".\OgreNewtonApplication.h"
#include ".\OgreNewtonFrameListener.h"

#include <OgreNewt.h>



OgreNewtonApplication::OgreNewtonApplication(void)
{
	// create OgreNewt world.
	m_World = new OgreNewt::World();

	mEntityCount = 0;

}

OgreNewtonApplication::~OgreNewtonApplication(void)
{
	// destroy world.
	delete m_World;
}



void OgreNewtonApplication::createScene()
{
	// sky box.
	mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
	
	// shadows on!
	mSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_ADDITIVE );


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
	delete col;
	
	//floornode->setScale( siz );
	bod->attachToNode( floornode );
	bod->setPositionOrientation( Ogre::Quaternion::IDENTITY, Ogre::Vector3(0.0,-20.0,0.0) );



	// make a simple rope.
	Ogre::Vector3 size(3,1.5,1.5);
	Ogre::Vector3 pos(0,3,0);
	Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY;

	// loop through, making bodies and connecting them.
	OgreNewt::Body* parent = NULL;
	OgreNewt::Body* child = NULL;

	for (int x=0;x<5;x++)
	{
		// make the next box.
		child = makeSimpleBox(size, pos, orient);

		// now make a new joint connecting this to the last box.
		OgreNewt::Joint* joint;

		// make the joint right between the bodies...
		

		if (parent)
		{
			joint = new OgreNewt::BasicJoints::BallAndSocket( m_World, child, parent, pos-Ogre::Vector3(size.x/2,0,0) );
		}
		else
		{
			// no parent, this is the first joint, so just pass NULL as the parent, to stick it to the "world"
			joint = new OgreNewt::BasicJoints::BallAndSocket( m_World, child, NULL, pos-Ogre::Vector3(size.x/2,0,0) );
		}

		// offset pos a little more.
		pos += Ogre::Vector3(size.x,0,0);

		// save the last body for the next loop!
		parent = child;
	}

	
	

	

	// position camera
	msnCam = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	msnCam->attachObject( mCamera );
	mCamera->setPosition(0.0, 0.0, 0.0);
	msnCam->setPosition( 0.0, -3.0, 20.0);

	//make a light
	Ogre::Light* light;

	light = mSceneMgr->createLight( "Light1" );
	light->setType( Ogre::Light::LightTypes::LT_POINT );
	light->setPosition( Ogre::Vector3(0.0, 100.0, 100.0) );



}


void OgreNewtonApplication::createFrameListener()
{
	mFrameListener = new OgreNewtonFrameListener( mWindow, mCamera, mSceneMgr, m_World, msnCam );
	mRoot->addFrameListener(mFrameListener);

	mNewtonListener = new OgreNewt::BasicFrameListener( mWindow, mCamera, mSceneMgr, m_World, 60 );
	mRoot->addFrameListener(mNewtonListener);
}


OgreNewt::Body* OgreNewtonApplication::makeSimpleBox( Ogre::Vector3& size, Ogre::Vector3& pos, Ogre::Quaternion& orient )
{
	// base mass on the size of the object.
	Ogre::Real mass = size.x * size.y * size.z * 2.5;
		
	// calculate the inertia based on box formula and mass
	Ogre::Vector3 inertia = OgreNewt::MomentOfInertia::CalcBoxSolid( mass, size );


	Entity* box1;
	SceneNode* box1node;

	box1 = mSceneMgr->createEntity( "Entity"+Ogre::StringConverter::toString(mEntityCount++), "box.mesh" );
	box1node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	box1node->attachObject( box1 );
	box1node->setScale( size );

	OgreNewt::Collision* col = new OgreNewt::CollisionPrimitives::Box( m_World, size );
	OgreNewt::Body* bod = new OgreNewt::Body( m_World, col );
	delete col;
				
	bod->attachToNode( box1node );
	bod->setMassMatrix( mass, inertia );
	bod->setStandardForceCallback();

	box1->setMaterialName( "Simple/BumpyMetal" );


	bod->setPositionOrientation( orient, pos );

	return bod;
}
