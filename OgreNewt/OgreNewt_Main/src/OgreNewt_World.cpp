#include <OgreNewt_World.h>
#include <OgreNewt_Body.h>
#include <OgreNewt_MaterialID.h>
#include <OgreNewt_Tools.h>

namespace OgreNewt
{


// Constructor
World::World()
{
	m_world = NewtonCreate( OgreNewt::_allocateMemory, OgreNewt::_freeMemory );

	if (!m_world)
	{
		// world not created!
	}

	NewtonWorldSetUserData( m_world, this );

	// create the default ID.
	m_defaultMatID = new OgreNewt::MaterialID( this, NewtonMaterialGetDefaultGroupID( m_world ) );

	//m_iterator = NULL;
}

// Destructor
World::~World()
{
	if (m_defaultMatID)
		delete m_defaultMatID;

	if (m_world)
		NewtonDestroy( m_world );
}

// update
void World::update( Ogre::Real t_step )
{
	NewtonUpdate( m_world, (float)t_step );
}

// get timestep
Ogre::Real World::getTimeStep()
{
	float ret = 0.0f;

	ret = NewtonGetTimeStep( m_world );

	return (Ogre::Real)ret;
}

void World::setWorldSize( Ogre::Vector3& min, Ogre::Vector3& max )
{
	NewtonSetWorldSize( m_world, (float*)&min.x, (float*)&max.x );
}

void World::setWorldSize( Ogre::AxisAlignedBox& box )
{
	NewtonSetWorldSize( m_world, (float*)&box.getMinimum(), (float*)&box.getMaximum() );
}

// -------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------


}