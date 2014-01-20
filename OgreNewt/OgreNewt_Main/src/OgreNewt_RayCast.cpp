#include "OgreNewt_RayCast.h"


namespace OgreNewt
{


	Raycast::Raycast()	{}
	Raycast::~Raycast()	{}


	void Raycast::go(OgreNewt::World* world, Ogre::Vector3& startpt, Ogre::Vector3& endpt )
	{
		// perform the raycast!
		NewtonWorldRayCast( world->getNewtonWorld(), (float*)&startpt, (float*)&endpt, OgreNewt::Raycast::newtonRaycastFilter, this );
	}

	float _cdecl Raycast::newtonRaycastFilter(const NewtonBody* body, const float* hitNormal, int collisionID, void* userData, float intersectParam)
	{
		// get our object!
		Raycast* me = (Raycast*)userData;

		Body* bod = (Body*)NewtonBodyGetUserData( body );
		Ogre::Vector3 normal = Ogre::Vector3( hitNormal[0], hitNormal[1], hitNormal[2] );

		if (me->userCallback( bod, intersectParam, normal, collisionID ))
			return intersectParam;
		else
			return 1.1;

	}



	//--------------------------------
	BasicRaycast::BasicRaycastInfo::BasicRaycastInfo()
	{
		mBody = NULL;
		mDistance = -1.0;
		mNormal = Ogre::Vector3::ZERO;
	}

	BasicRaycast::BasicRaycastInfo::~BasicRaycastInfo() {}


	BasicRaycast::BasicRaycast(OgreNewt::World* world, Ogre::Vector3& startpt, Ogre::Vector3& endpt ) : Raycast() 
	{
		go( world, startpt, endpt );
	}

	BasicRaycast::~BasicRaycast()	{}


	int BasicRaycast::getHitCount() { return (int)mRayList.size(); }


	BasicRaycast::BasicRaycastInfo BasicRaycast::getFirstHit()
	{
		//return the closest hit...
		BasicRaycast::BasicRaycastInfo ret;

		Ogre::Real dist = 10000.0;

		RaycastInfoListIterator it;
		for (it = mRayList.begin(); it != mRayList.end(); it++)
		{
			if (it->mDistance < dist)
			{
				dist = it->mDistance;
				ret = (*it);
			}
		}


		return ret;
	}


	BasicRaycast::BasicRaycastInfo BasicRaycast::getInfoAt( int hitnum )
	{
		BasicRaycast::BasicRaycastInfo ret;

		if ((hitnum < 0) || (hitnum > mRayList.size()))
			return ret;

		ret = mRayList.at(hitnum);

		return ret;
	}

	bool BasicRaycast::userCallback( OgreNewt::Body* body, Ogre::Real distance, Ogre::Vector3& normal, int collisionID )
	{
		// create a new infor object.
		BasicRaycast::BasicRaycastInfo newinfo;

		newinfo.mBody = body;
		newinfo.mDistance = distance;
		newinfo.mNormal = normal;

		mRayList.push_back( newinfo );

		return false;
	}



}	// end NAMESPACE OgreNewt