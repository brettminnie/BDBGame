/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/
#ifndef _INCLUDE_OGRENEWT_JOINT
#define _INCLUDE_OGRENEWT_JOINT

#include <newton.h>
#include "OgreNewt_World.h"

// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{


/*
	CLASS DEFINITION:

		Joint 

	USE:
		this class represents a basic Joint class which is a base for all other joints...
*/
// OgreNewt::Joint
class Joint
{
 protected:

	NewtonJoint* m_joint;
	OgreNewt::World* m_world;

	static void _cdecl destructor( const NewtonJoint* me );
	virtual ~Joint();	// desctructor

public:

	Joint();	// constructor


	//common functions to all joints
	int getCollisionState() { NewtonJointGetCollisionState( m_joint ); }
	Ogre::Real getStiffness() { return (Ogre::Real)NewtonJointGetStiffness( m_joint ); }

	void setCollisionState( int state ) { NewtonJointSetCollisionState( m_joint, state ); }
	void setStiffness( Ogre::Real stiffness ) { NewtonJointSetStiffness( m_joint, stiffness ); }

};


}	// end NAMESPACE OgreNewt

#endif
// _INCLUDE_OGRENEWT_JOINT

