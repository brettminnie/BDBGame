/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/
#ifndef _INCLUDE_OGRENEWT_MATERIALPAIR
#define _INCLUDE_OGRENEWT_MATERIALPAIR

#include <newton.h>
#include "OgreNewt_World.h"
#include "OgreNewt_ContactCallback.h"
#include "OgreNewt_MaterialID.h"

// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{


/*
	CLASS DEFINITION:

		MaterialPair

	USE:
		this class represents a pair of Newton MaterialGroupIDs, which is 
		used to define interaction bewteen objects.
*/
// OgreNewt::MaterialPair
class MaterialPair
{
 protected:

	MaterialID* id0;
	MaterialID* id1;
	World* m_world;

public:

	MaterialPair( World* world, MaterialID* mat1, MaterialID* mat2 );	// constructor
	~MaterialPair();					// desctructor


	// set the default behavior for this material pair.
	void setDefaultSoftness( Ogre::Real softness ) { NewtonMaterialSetDefaultSoftness( m_world->getNewtonWorld(), id0->getID(), id1->getID(), (float)softness ); }
	void setDefaultElasticity( Ogre::Real elasticity ) { NewtonMaterialSetDefaultElasticity( m_world->getNewtonWorld(), id0->getID(), id1->getID(), (float)elasticity ); }
	void setDefaultCollidable( int state ) { NewtonMaterialSetDefaultCollidable( m_world->getNewtonWorld(), id0->getID(), id1->getID(), state ); }
	void setDefaultFriction( Ogre::Real stat, Ogre::Real kinetic ) { NewtonMaterialSetDefaultFriction( m_world->getNewtonWorld(), id0->getID(), id1->getID(), (float)stat, (float)kinetic ); }

	// collision callback functions, create them through the CollisionCallback class...
	void setContactCallback( OgreNewt::ContactCallback* callback );


};


}	// end NAMESPACE OgreNewt

#endif
// _INCLUDE_OGRENEWT_MATERIALPAIR

