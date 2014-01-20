#include <assert.h>
#include <OgreNewt_Joint.h>


namespace OgreNewt
{

	
Joint::Joint()
{
	// nothing here
}

Joint::~Joint()
{

	if(m_joint)
	{
		if (NewtonJointGetUserData(m_joint))
		{
			NewtonJointSetDestructor( m_joint, NULL );
			NewtonDestroyJoint( m_world->getNewtonWorld(), m_joint );
		}
	}

}


void _cdecl Joint::destructor( const NewtonJoint* me )
{
	Joint* jnt;

	jnt = (Joint*)NewtonJointGetUserData( me );

	NewtonJointSetDestructor( me, NULL );
	NewtonJointSetUserData( me, NULL );

	delete jnt;
}




}