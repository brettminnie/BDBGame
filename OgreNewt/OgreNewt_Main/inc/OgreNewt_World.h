/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/
#ifndef _INCLUDE_OGRENEWT_WORLD
#define _INCLUDE_OGRENEWT_WORLD

#include <newton.h>
#include <Ogre.h>
#include "OgreNewt_MaterialID.h"

// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{


class MaterialID;

/*
	CLASS DEFINITION:

		World

	USE:
		this class represents a NewtonWorld, which is the basic space
		in which physics elements can exist.  It can have various Rigid Bodies,
		connected by joints, and other constraints.
*/
// OgreNewt::World
class World
{

public:

	enum SolverModelMode { SM_EXACT, SM_ADAPTIVE };
	enum FrictionModelMode { FM_EXACT, FM_ADAPTIVE };

 protected:
	
	NewtonWorld* m_world;
	MaterialID* m_defaultMatID;

 public:
	World();			// Standard Constructor, creates the world.
	~World();			// Standard Destructor, destroys the world.

	void update( Ogre::Real t_step );	// update the world by the specified time_step.

	NewtonWorld* getNewtonWorld() { return m_world; }	// get pointer to Newton World.
	MaterialID* getDefaultMaterialID() { return m_defaultMatID; }	// get pointer to default material ID object.

	void destroyAllBodies() { NewtonDestroyAllBodies( m_world ); }
	void setSolverModel( int model ) { NewtonSetSolverModel( m_world, model ); }
	void setFrictionModel( int model) { NewtonSetFrictionModel( m_world, model ); }
	Ogre::Real getTimeStep();
	void setMinimumFrameRate( Ogre::Real frame ) { NewtonSetMinimumFrameRate( m_world, frame ); }
	void setWorldSize( Ogre::Vector3& min, Ogre::Vector3& max );
	void setWorldSize( Ogre::AxisAlignedBox& box );
	int getVersion() { return NewtonWorldGetVersion( m_world ); }


};

}

	
#endif
// _INCLUDE_OGRENEWT_WORLD


