#include ".\conveyormatcallback.h"
#include ".\conveyorBelt.h"

conveyorMatCallback::conveyorMatCallback( int conveyorID ) : OgreNewt::ContactCallback()
{
	mConveyorID = conveyorID;
}

conveyorMatCallback::~conveyorMatCallback(void)
{
}


int conveyorMatCallback::userProcess()
{
	// first, find which body represents the conveyor belt!
	conveyorBelt* belt;
	OgreNewt::Body* object;

	if (m_body0->getType() == mConveyorID)
	{
		belt = (conveyorBelt*)m_body0->getUserData();
		object = m_body1;
	}

	if (m_body1->getType() == mConveyorID)
	{
		belt = (conveyorBelt*)m_body1->getUserData();
		object = m_body0;
	}

	if (!belt) { return 0; }

	// okay, found the belt... let's adjust the collision based on this.
	Ogre::Vector3 thedir = belt->getGlobalDir();

	rotateTangentDirections( thedir );
	Ogre::Vector3 result_accel = (thedir * belt->getSpeed()) - object->getVelocity();
	setContactTangentAcceleration( result_accel.length(), 0 );


	return 1;
}