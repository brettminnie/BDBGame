#include <Ogre.h>
#include <OgreNewt_Collision.h>
#include <OgreNewt_World.h>
#include <OgreNewt_Tools.h>


namespace OgreNewt
{

	
Collision::Collision( World* world )
{
	m_world = world;
}

Collision::~Collision()
{
	if (m_world->getNewtonWorld())
		NewtonReleaseCollision( m_world->getNewtonWorld(), m_col );
}


Ogre::AxisAlignedBox Collision::getAABB( Ogre::Quaternion orient, Ogre::Vector3 pos )
{
	Ogre::AxisAlignedBox box;
	Ogre::Vector3 min, max;
	float matrix[16];
	OgreNewt::Converters::QuatPosToMatrix( orient, pos, matrix );

	NewtonCollisionCalculateAABB( m_col, matrix, &min.x, &max.x );

	box = Ogre::AxisAlignedBox(min, max);
	return box;
}

}	// end NAMESPACE OgreNewt