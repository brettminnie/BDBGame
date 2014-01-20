#include "OgreNewt_Body.h"
#include "OgreNewt_Tools.h"


namespace OgreNewt
{

	
Body::Body( World* W, OgreNewt::Collision* col, int bodytype ) 
{
	m_world = W;
	m_collision = col;
	m_type = bodytype;
	m_node = NULL;
	m_matid = NULL;
	
	m_userdata = NULL;

	m_forcecallback = NULL;
	m_transformcallback = NULL;

	m_body = NewtonCreateBody( m_world->getNewtonWorld(), col->getNewtonCollision() ); 

	NewtonBodySetUserData( m_body, this );
	NewtonBodySetDestructorCallback( m_body, newtonDestructor );
}

Body::~Body()
{
	if (m_body)
	{
		if (NewtonBodyGetUserData(m_body))
		{
			NewtonBodySetDestructorCallback( m_body, NULL );
			NewtonDestroyBody( m_world->getNewtonWorld(), m_body );
		}
	}
}

// destructor callback
void _cdecl Body::newtonDestructor( const NewtonBody* body )
{
	//newton wants to destroy the body.. so first find it.
	OgreNewt::Body* me;

	me = (OgreNewt::Body*)NewtonBodyGetUserData( body );

	// remove destructor callback
	NewtonBodySetDestructorCallback( body, NULL );
	// remove the user data
	NewtonBodySetUserData( body, NULL );

	//now delete the object.
	delete me;
}


// transform callback
void _cdecl Body::newtonTransformCallback( const NewtonBody* body, const float* matrix )
{
	Ogre::Quaternion orient;
	Ogre::Vector3 pos;
	OgreNewt::Body* me;

	me = (OgreNewt::Body*) NewtonBodyGetUserData( body );

	OgreNewt::Converters::MatrixToQuatPos( matrix, orient, pos );

	me->m_node->setOrientation( orient );
	me->m_node->setPosition( pos );

	if (me->m_transformcallback)
		(*me->m_transformcallback)( me );
}

	
void _cdecl Body::newtonForceTorqueCallback( const NewtonBody* body )
{
	OgreNewt::Body* me = (OgreNewt::Body*)NewtonBodyGetUserData( body );

	if (me->m_forcecallback)
		(*me->m_forcecallback)( me );
}

void _cdecl Body::standardForceCallback( OgreNewt::Body* me )
{
	//apply a simple gravity force.
	Ogre::Real mass;
	Ogre::Vector3 inertia;

	me->getMassMatrix(mass, inertia);
	Ogre::Vector3 force(0,-9.8,0);
	force *= mass;

	me->addForce( force );

}



// attachToNode
void Body::attachToNode( Ogre::SceneNode* node )
{
	m_node = node;
	if (m_body)
	{
		NewtonBodySetTransformCallback( m_body, newtonTransformCallback );
	}
}

void Body::setPositionOrientation( Ogre::Quaternion orient, Ogre::Vector3 pos )
{
	if (m_body)
	{
		float matrix[16];

		OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );
		NewtonBodySetMatrix( m_body, &matrix[0] );

		if (m_node)
		{
			m_node->setOrientation( orient );
			m_node->setPosition( pos );
		}
	}
}

// set mass matrix
void Body::setMassMatrix( Ogre::Real mass, Ogre::Vector3 inertia )
{
	if (m_body)
		NewtonBodySetMassMatrix( m_body, (float)mass, (float)inertia.x, (float)inertia.y, (float)inertia.z );
}

// basic gravity callback
void Body::setStandardForceCallback()
{
	setCustomForceAndTorqueCallback( standardForceCallback );
}

// custom user force callback
void Body::setCustomForceAndTorqueCallback( ForceCallback callback )
{
	if (!m_forcecallback)
	{
		m_forcecallback = callback;
		NewtonBodySetForceAndTorqueCallback( m_body, newtonForceTorqueCallback );
	}
	else
	{
		if (m_forcecallback != callback)
			m_forcecallback = callback;
	}

}

// custom user force callback
void Body::setCustomTransformCallback( TransformCallback callback )
{
	if (!m_transformcallback)
	{
		m_transformcallback = callback;
	}
	else
	{
		if (m_transformcallback != callback)
			m_transformcallback = callback;
	}

}

//set collision
void Body::setCollision( OgreNewt::Collision* col )
{
	NewtonBodySetCollision( m_body, col->getNewtonCollision() );

	m_collision = col;
}

//get collision
OgreNewt::Collision* Body::getCollision()
{
	return m_collision;
}

//get material group ID
OgreNewt::MaterialID* Body::getMaterialGroupID()
{
	if (m_matid)
		return m_matid;
	else
		return NULL;
}


// get position and orientation
void Body::getPositionOrientation( Ogre::Quaternion& orient, Ogre::Vector3& pos )
{
	float matrix[16];

	NewtonBodyGetMatrix( m_body, matrix );
	OgreNewt::Converters::MatrixToQuatPos( matrix, orient, pos );
}

void Body::getMassMatrix( Ogre::Real& mass, Ogre::Vector3& inertia )
{
	NewtonBodyGetMassMatrix( m_body, &mass, &inertia.x, &inertia.y, &inertia.z );
}

void Body::getInvMass( Ogre::Real& mass, Ogre::Vector3& inertia )
{
	NewtonBodyGetInvMass( m_body, &mass, &inertia.x, &inertia.y, &inertia.z );
}

Ogre::Vector3 Body::getOmega()
{
	Ogre::Vector3 ret;
	NewtonBodyGetOmega( m_body, &ret.x );
	return ret;
}

Ogre::Vector3 Body::getVelocity()
{
	Ogre::Vector3 ret;
	NewtonBodyGetVelocity( m_body, &ret.x );
	return ret;
}

Ogre::Vector3 Body::getAngularDamping()
{
	Ogre::Vector3 ret;
	NewtonBodyGetAngularDamping( m_body, &ret.x );
	return ret;
}

void Body::addGlobalForce( Ogre::Vector3& force, Ogre::Vector3& pos )
{
	Ogre::Vector3 bodypos;
	Ogre::Quaternion bodyorient;
	getPositionOrientation( bodyorient, bodypos );

	Ogre::Vector3 topoint = pos - bodypos;
	Ogre::Vector3 torque = topoint.crossProduct( force );

	addForce( force );
	addTorque( torque );
}

void Body::addLocalForce( Ogre::Vector3& force, Ogre::Vector3& pos )
{
	Ogre::Vector3 bodypos;
	Ogre::Quaternion bodyorient;

	getPositionOrientation( bodyorient, bodypos );

	Ogre::Vector3 globalforce = bodyorient * force;
	Ogre::Vector3 globalpoint = (bodyorient * pos) + bodypos;

	addGlobalForce( globalforce, globalpoint );
}


// --------------------------------------------------------------------------------------


}