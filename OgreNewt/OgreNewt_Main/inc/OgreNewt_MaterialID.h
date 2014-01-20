/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/
#ifndef _INCLUDE_OGRENEWT_MATERIALID
#define _INCLUDE_OGRENEWT_MATERIALID

#include <newton.h>
#include "OgreNewt_World.h"

// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{

	class World;

/*
	CLASS DEFINITION:

		MaterialID

	USE:
		this class represents a single MaterialGroupID.
*/
// OgreNewt::MaterialID
class MaterialID
{
 protected:

	int id;
	OgreNewt::World* m_world;

public:

	MaterialID( World* world );	// constructor
	MaterialID( World* world, int ID );	// custom consructor.
	~MaterialID();					// desctructor

	// get Newton-assigned material ID.
	int getID() { return id; }

};



}	// end NAMESPACE OgreNewt

#endif
// _INCLUDE_OGRENEWT_MATERIALID

