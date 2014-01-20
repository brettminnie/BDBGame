/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/

#ifndef _INCLUDE_OGRENEWT_RAYCAST
#define _INCLUDE_OGRENEWT_RAYCAST


#include <Ogre.h>
#include <newton.h>
#include "OgreNewt_World.h"
#include "OgreNewt_Body.h"


// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{

/*
	CLASS DEFINITION:

		Raycast

	USE:
		this class represents a raycast. users can inherit this class to make custom raycasts.
*/

class Raycast
{
public:

	// constructor
	Raycast();
	
	// destuctor.
	~Raycast();

	// this function performs the raycast.
	void go( OgreNewt::World* world, Ogre::Vector3& startpt, Ogre::Vector3& endpt );

	// user callback function.  Newton calls this function for each body intersected by the ray.  however it doesn't
	// necessarily go perfect cloest-to-farthest order.
	// return true and the callback will only be called for bodies closer to the start point than the current body.
	// return false and the callback will call for any other bodies, even those farther than the current one.
	virtual bool userCallback( OgreNewt::Body* body, Ogre::Real distance, Ogre::Vector3& normal, int collisionID ) = 0;


private:

	// callback used for running the raycast itself...
	static float _cdecl newtonRaycastFilter(const NewtonBody* body, const float* hitNormal, int collisionID, void* userData, float intersetParam);
};




/*
	CLASS DEFINITION:

		BasicRaycast

	USE:
		This is an example of how you can use the Raycast class.  this implements a basic raycast object that can be used
		for most general cases.  if you have a special case, make your own class inherited from Raycast.
*/
class BasicRaycast : public Raycast
{
public:
	// simple class that represents a single raycast rigid body intersection.
	class BasicRaycastInfo
	{
	public:
		Ogre::Real mDistance;	// dist from point1 of the raycast, in range [0,1].
		OgreNewt::Body* mBody;	// pointer to body intersected with
		Ogre::Vector3 mNormal;	// normal of intersection.

		BasicRaycastInfo();
		~BasicRaycastInfo();
	};

	// constructor - performs a raycast, then the results can be queried from the object after creation.
	BasicRaycast( OgreNewt::World* world, Ogre::Vector3& startpt, Ogre::Vector3& endpt );
	
	// destuctor.
	~BasicRaycast();

	// the all-important custom callback function.
	bool userCallback( Body* body, Ogre::Real distance, Ogre::Vector3& normal, int collisionID );

	// ------------------------------------------------------
	// the following functions can be used to retrieve information about the bodies collided by the ray.
	
	// how many bodies did we hit?
	int getHitCount();

	// retrieve the raycast info for a specific hit.
	BasicRaycastInfo getInfoAt( int hitnum );

	// get the closest body hit by the ray.
	BasicRaycastInfo getFirstHit();


private:


	// container for results.
	typedef std::vector<BasicRaycastInfo> RaycastInfoList;
	typedef std::vector<BasicRaycastInfo>::iterator RaycastInfoListIterator;

	RaycastInfoList mRayList;

};



}	// end NAMESPACE OgreNewt
	




#endif	// _INCLUDE_OGRENEWT_RAYCAST