/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/
#ifndef _INCLUDE_OGRENEWT_COLLISION
#define _INCLUDE_OGRENEWT_COLLISION

#include <newton.h>
#include "OgreNewt_World.h"

// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{


/*
	CLASS DEFINITION:

		Collision

	USE:
		this class represents a NewtonCollision, which is the newton structure
		for collision geometry.
*/
// OgreNewt::Collision
class Collision
{
 protected:

	NewtonCollision* m_col;
	World* m_world;

public:

	Collision( World* world );	// constructor
	~Collision();					// desctructor

	NewtonCollision* getNewtonCollision() { return m_col; }

	// set a user ID for collision callback identification
	void setUserID( unsigned id ) { NewtonConvexCollisionSetUserID( m_col, id); }

	// get user ID, for collision callback identification
	unsigned getUserID() { return NewtonConvexCollisionGetUserID( m_col ); }

	// get the Axis-Aligned Bounding Box for this collision shape.
	Ogre::AxisAlignedBox getAABB( Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY, Ogre::Vector3 pos = Ogre::Vector3::ZERO );

};


}	// end NAMESPACE OgreNewt

#endif
// _INCLUDE_OGRENEWT_COLLISION

