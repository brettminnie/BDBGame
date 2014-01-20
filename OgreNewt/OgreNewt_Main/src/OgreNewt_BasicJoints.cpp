#include <newton.h>
#include <OgreNewt_BasicJoints.h>
#include <OgreNewt_World.h>
#include <OgreNewt_Body.h>

namespace OgreNewt
{

namespace BasicJoints
{
	
BallAndSocket::BallAndSocket( World* world, OgreNewt::Body* child, OgreNewt::Body* parent, Ogre::Vector3& pos ) : Joint()
{
	m_world = world;

	if (parent)
		m_joint = NewtonConstraintCreateBall( world->getNewtonWorld(), &pos.x, child->getNewtonBody(), parent->getNewtonBody() );
	else
		m_joint = NewtonConstraintCreateBall( world->getNewtonWorld(), &pos.x, child->getNewtonBody(), NULL );


	// all constructors inherited from Joint MUST call these 2 functions to make the joint function properly.
	NewtonJointSetUserData( m_joint, this );
	NewtonJointSetDestructor( m_joint, destructor );
										
}

BallAndSocket::~BallAndSocket()
{
	// nothing, the ~Joint() function will take care of us.
}

Ogre::Vector3 BallAndSocket::getJointAngle()
{
	Ogre::Vector3 ret;

	NewtonBallGetJointAngle( m_joint, &ret.x );

	return ret;
}


Ogre::Vector3 BallAndSocket::getJointOmega()
{
	Ogre::Vector3 ret;

	NewtonBallGetJointOmega( m_joint, &ret.x );

	return ret;
}


Ogre::Vector3 BallAndSocket::getJointForce()
{
	Ogre::Vector3 ret;

	NewtonBallGetJointForce( m_joint, &ret.x );

	return ret;
}



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


Hinge::Hinge( World* world, OgreNewt::Body* child, OgreNewt::Body* parent, Ogre::Vector3& pos, Ogre::Vector3& pin ) : Joint()
{
	m_world = world;

	if (parent)
	{
		m_joint = NewtonConstraintCreateHinge( world->getNewtonWorld(), &pos.x, &pin.x,
												child->getNewtonBody(), parent->getNewtonBody() );
	}
	else
	{
		m_joint = NewtonConstraintCreateHinge( world->getNewtonWorld(), &pos.x, &pin.x,
												child->getNewtonBody(), NULL );
	}

	NewtonJointSetUserData( m_joint, this );
	NewtonJointSetDestructor( m_joint, destructor );
	NewtonHingeSetUserCallback( m_joint, newtonCallback );

	m_callback = NULL;
}

Hinge::~Hinge()
{
}


Ogre::Vector3 Hinge::getJointForce()
{
	Ogre::Vector3 ret;

	NewtonHingeGetJointForce( m_joint, &ret.x );

	return ret;
}

unsigned _cdecl Hinge::newtonCallback( const NewtonJoint* hinge, NewtonHingeSliderUpdateDesc* desc )
{
	Hinge* me = (Hinge*)NewtonJointGetUserData( hinge );

	me->m_desc = desc;
	me->m_retval = 0;

	if (me->m_callback)
		(*me->m_callback)( me );

	me->m_desc = NULL;

	return me->m_retval;
}

/////// CALLBACK FUNCTIONS ///////
void Hinge::setCallbackAccel( Ogre::Real accel )
{
	if (m_desc)
	{
		m_retval = 1;
		m_desc->m_accel = (float)accel;
	}
}

void Hinge::setCallbackFrictionMin( Ogre::Real min )
{
	if (m_desc)
	{
		m_retval = 1;
		m_desc->m_minFriction = (float)min;
	}
}

void Hinge::setCallbackFrictionMax( Ogre::Real max )
{
	if (m_desc)
	{
		m_retval = 1;
		m_desc->m_maxFriction = (float)max;
	}
}

Ogre::Real Hinge::getCallbackTimestep()
{
	if (m_desc)
		return (Ogre::Real)m_desc->m_timestep;
	else
		return 0.0;
}

Ogre::Real Hinge::calculateStopAlpha( Ogre::Real angle )
{
	if (m_desc)
		return (Ogre::Real)NewtonHingeCalculateStopAlpha( m_joint, m_desc, (float)angle );
	else
		return 0.0;
}



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



Slider::Slider( World* world, OgreNewt::Body* child, OgreNewt::Body* parent, Ogre::Vector3& pos, Ogre::Vector3& pin ) : Joint()
{
	m_world = world;

	if (parent)
	{
		m_joint = NewtonConstraintCreateSlider( world->getNewtonWorld(), &pos.x, &pin.x,
												child->getNewtonBody(), parent->getNewtonBody() );
	}
	else
	{
		m_joint = NewtonConstraintCreateSlider( world->getNewtonWorld(), &pos.x, &pin.x,
												child->getNewtonBody(), NULL );
	}

	NewtonJointSetUserData( m_joint, this );
	NewtonJointSetDestructor( m_joint, destructor );
	NewtonSliderSetUserCallback( m_joint, newtonCallback );
}

Slider::~Slider()
{
}

Ogre::Vector3 Slider::getJointForce()
{
	Ogre::Vector3 ret;

	NewtonSliderGetJointForce( m_joint, &ret.x );

	return ret;
}

unsigned _cdecl Slider::newtonCallback( const NewtonJoint* slider, NewtonHingeSliderUpdateDesc* desc )
{
	Slider* me = (Slider*)NewtonJointGetUserData( slider );

	me->m_desc = desc;
	me->m_retval = 0;

	if (me->m_callback)
		(*me->m_callback)( me );

	me->m_desc = NULL;

	return me->m_retval;
}


/////// CALLBACK FUNCTIONS ///////
void Slider::setCallbackAccel( Ogre::Real accel )
{
	if (m_desc)
	{
		m_retval = 1;
		m_desc->m_accel = (float)accel;
	}
}

void Slider::setCallbackFrictionMin( Ogre::Real min )
{
	if (m_desc)
	{
		m_retval = 1;
		m_desc->m_minFriction = (float)min;
	}
}

void Slider::setCallbackFrictionMax( Ogre::Real max )
{
	if (m_desc)
	{
		m_retval = 1;
		m_desc->m_maxFriction = (float)max;
	}
}

Ogre::Real Slider::getCallbackTimestep()
{
	if (m_desc)
		return (Ogre::Real)m_desc->m_timestep;
	else
		return 0.0;
}

Ogre::Real Slider::calculateStopAccel( Ogre::Real dist )
{
	if (m_desc)
		return (Ogre::Real)NewtonSliderCalculateStopAccel( m_joint, m_desc, (float)dist );
	else
		return 0.0;
}



///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



Universal::Universal( World* world, OgreNewt::Body* child, OgreNewt::Body* parent, Ogre::Vector3& pos, Ogre::Vector3& pin0, Ogre::Vector3& pin1 ) : Joint()
{
	m_world = world;

	if (parent)
	{
		m_joint = NewtonConstraintCreateUniversal( world->getNewtonWorld(), &pos.x, &pin0.x, &pin1.x,
												child->getNewtonBody(), parent->getNewtonBody() );
	}
	else
	{
		m_joint = NewtonConstraintCreateUniversal( world->getNewtonWorld(), &pos.x, &pin0.x, &pin1.x,
												child->getNewtonBody(), NULL );
	}

	NewtonJointSetUserData( m_joint, this );
	NewtonJointSetDestructor( m_joint, destructor );
	NewtonUniversalSetUserCallback( m_joint, newtonCallback );
}

Universal::~Universal()
{
}

Ogre::Vector3 Universal::getJointForce()
{
	Ogre::Vector3 ret;

	NewtonUniversalGetJointForce( m_joint, &ret.x );

	return ret;
}

unsigned _cdecl Universal::newtonCallback( const NewtonJoint* universal, NewtonHingeSliderUpdateDesc* desc )
{
	Universal* me = (Universal*)NewtonJointGetUserData( universal );

	me->m_desc = desc;
	me->m_retval = 0;

	if (me->m_callback)
		(*me->m_callback)( me );

	me->m_desc = NULL;

	return me->m_retval;
}


/////// CALLBACK FUNCTIONS ///////
void Universal::setCallbackAccel( Ogre::Real accel, unsigned int axis )
{
	if (axis > 1) { return; }

	if (m_desc)
	{
		m_retval |= axis;
		m_desc[axis].m_accel = (float)accel;
	}
}

void Universal::setCallbackFrictionMax( Ogre::Real max, unsigned int axis )
{
	if (axis > 1) { return; }

	if (m_desc)
	{
		m_retval |= axis;
		m_desc[axis].m_maxFriction = (float)max;
	}
}

void Universal::setCallbackFrictionMin( Ogre::Real min, unsigned int axis )
{
	if (axis > 1) { return; }

	if (m_desc)
	{
		m_retval |= axis;
		m_desc[axis].m_minFriction = (float)min;
	}
}

Ogre::Real Universal::getCallbackTimestep()
{
	if (m_desc)
		return (Ogre::Real)m_desc->m_timestep;
	else
		return 0.0;
}

Ogre::Real Universal::calculateStopAlpha0( Ogre::Real angle )
{
	if (m_desc)
		return (Ogre::Real)NewtonUniversalCalculateStopAlpha0( m_joint, m_desc, (float)angle );
	else
		return 0.0;
}

Ogre::Real Universal::calculateStopAlpha1( Ogre::Real angle )
{
	if (m_desc)
		return (Ogre::Real)NewtonUniversalCalculateStopAlpha1( m_joint, m_desc, (float)angle );
	else
		return 0.0;
}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



UpVector::UpVector( World* world, Body* body, Ogre::Vector3& pin )
{
	m_world = world;

	m_joint = NewtonConstraintCreateUpVector( world->getNewtonWorld(), &pin.x, body->getNewtonBody() );

	NewtonJointSetUserData( m_joint, this );
	NewtonJointSetDestructor( m_joint, destructor );

}

UpVector::~UpVector()
{
}

Ogre::Vector3 UpVector::getPin()
{
	Ogre::Vector3 ret;

	NewtonUpVectorGetPin( m_joint, &ret.x );

	return ret;
}





}	// end NAMESPACE BasicJoints

}	// end NAMESPACE OgreNewt