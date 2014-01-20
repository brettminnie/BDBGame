/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/
#ifndef _INCLUDE_OGRENEWT_BASICJOINTS
#define _INCLUDE_OGRENEWT_BASICJOINTS

#include <newton.h>
#include "OgreNewt_World.h"
#include "OgreNewt_Body.h"
#include "OgreNewt_Joint.h"

// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{



namespace BasicJoints
{

//////////////////////////////////////////////////////////////////////////////////
/*
	CLASS DEFINITION:

		BallandSocket 

	USE:
		this class represents a Ball and Socket joint.
*/
// OgreNewt::BallAndSocket - simple ball and socket joint, with limits.
class BallAndSocket : public Joint
{
 
public:

	BallAndSocket( World* world, OgreNewt::Body* child, OgreNewt::Body* parent, Ogre::Vector3& pos );	// constructor
	~BallAndSocket();					// desctructor

	Ogre::Vector3 getJointAngle();
	Ogre::Vector3 getJointOmega();
	Ogre::Vector3 getJointForce();

	void setLimits( Ogre::Vector3& pin, Ogre::Real maxCone, Ogre::Real maxTwist ) { NewtonBallSetConeLimits( m_joint, &pin.x, (float)maxCone, (float)maxTwist ); }


};


//////////////////////////////////////////////////////////////////////////////////
/*
	CLASS DEFINITION:

		Hinge 

	USE:
		this class represents a Hinge joint.
*/
// OgreNewt::Hinge - simple hinge joint.  implement motors/limits through a callback.
class Hinge : public Joint
{
 
public:

	// type of function used for a hinge callback.  use the setCallback() function to assign your custom function.
	typedef void(_cdecl *HingeCallback)( Hinge* me );

	Hinge( World* world, OgreNewt::Body* child, OgreNewt::Body* parent, Ogre::Vector3& pos, Ogre::Vector3& pin );	// constructor
	~Hinge();					// desctructor

	Ogre::Real getJointAngle() { return (Ogre::Real)NewtonHingeGetJointAngle( m_joint ); }
	Ogre::Real getJointOmega() { return (Ogre::Real)NewtonHingeGetJointOmega( m_joint ); }
	Ogre::Vector3 getJointForce();

	void setCallback( HingeCallback callback ) { m_callback = callback; }

	////////// CALLBACK COMMANDS ///////////
	// the following commands are only valid from inside a hinge callback function
	void setCallbackAccel( Ogre::Real accel );
	void setCallbackFrictionMin( Ogre::Real min );
	void setCallbackFrictionMax( Ogre::Real max );
	Ogre::Real getCallbackTimestep();
	Ogre::Real calculateStopAlpha( Ogre::Real angle );

protected:

	// newton callback, used internally.
	static unsigned _cdecl newtonCallback( const NewtonJoint* hinge, NewtonHingeSliderUpdateDesc* desc );

	HingeCallback m_callback;
	NewtonHingeSliderUpdateDesc* m_desc;

	unsigned m_retval;


};


//////////////////////////////////////////////////////////////////////////////////
/*
	CLASS DEFINITION:

		Slider 

	USE:
		this class represents a Slider joint.
*/
// OgreNewt::Slider - simple slider joint.  implement motors/limits through a callback.
class Slider : public Joint
{
 
public:

	// type of function used for a slider callback.  use the setCallback() function to assign your custom function.
	typedef void(_cdecl *SliderCallback)( Slider* me );

	Slider( World* world, OgreNewt::Body* child, OgreNewt::Body* parent, Ogre::Vector3& pos, Ogre::Vector3& pin );	// constructor
	~Slider();					// desctructor

	Ogre::Real getJointPosit() { return (Ogre::Real)NewtonSliderGetJointPosit( m_joint ); }
	Ogre::Real getJointVeloc() { return (Ogre::Real)NewtonSliderGetJointVeloc( m_joint ); }
	Ogre::Vector3 getJointForce();

	void setCallback( SliderCallback callback ) { m_callback = callback; }

	////////// CALLBACK COMMANDS ///////////
	// the following commands are only valid from inside a hinge callback function
	void setCallbackAccel( Ogre::Real accel );
	void setCallbackFrictionMin( Ogre::Real min );
	void setCallbackFrictionMax( Ogre::Real max );
	Ogre::Real getCallbackTimestep();
	Ogre::Real calculateStopAccel( Ogre::Real dist );

protected:

	static unsigned _cdecl newtonCallback( const NewtonJoint* slider, NewtonHingeSliderUpdateDesc* desc );

	SliderCallback m_callback;
	NewtonHingeSliderUpdateDesc* m_desc;

	unsigned m_retval;


};



//////////////////////////////////////////////////////////////////////////////////
/*
	CLASS DEFINITION:

		Universal 

	USE:
		this class represents a Universal joint.
*/
// OgreNewt::Universal - simple universal joint.  implement motors/limits through a callback.
class Universal : public Joint
{
 
public:
	
	// type of function used for a universal callback.  use the setCallback() function to assign your custom function.
	typedef void(_cdecl *UniversalCallback)( Universal* me );

	Universal( World* world, OgreNewt::Body* child, OgreNewt::Body* parent, Ogre::Vector3& pos, Ogre::Vector3& pin0, Ogre::Vector3& pin1 );	// constructor
	~Universal();					// desctructor

	Ogre::Real getJointAngle0() { return (Ogre::Real)NewtonUniversalGetJointAngle0( m_joint ); }
	Ogre::Real getJointAngle1() { return (Ogre::Real)NewtonUniversalGetJointAngle1( m_joint ); }
	Ogre::Real getJointOmega0() { return (Ogre::Real)NewtonUniversalGetJointOmega0( m_joint ); }
	Ogre::Real getJointOmega1() { return (Ogre::Real)NewtonUniversalGetJointOmega1( m_joint ); }
	Ogre::Vector3 getJointForce();

	void setCallback( UniversalCallback callback ) { m_callback = callback; }

	////////// CALLBACK COMMANDS ///////////
	// the following commands are only valid from inside a hinge callback function
	void setCallbackAccel( Ogre::Real accel, unsigned axis );
	void setCallbackFrictionMin( Ogre::Real min, unsigned axis );
	void setCallbackFrictionMax( Ogre::Real max, unsigned axis );
	Ogre::Real getCallbackTimestep();
	Ogre::Real calculateStopAlpha0( Ogre::Real angle );
	Ogre::Real calculateStopAlpha1( Ogre::Real angle );

protected:
	
	static unsigned _cdecl newtonCallback( const NewtonJoint* universal, NewtonHingeSliderUpdateDesc* desc );

	UniversalCallback m_callback;
	NewtonHingeSliderUpdateDesc* m_desc;

	unsigned m_retval;



};




//////////////////////////////////////////////////////////////////////////////////
/*
	CLASS DEFINITION:

		UpVector 

	USE:
		this class represents a UpVector joint.
*/
// OgreNewt::UpVector - simple upvector joint.
class UpVector : public Joint
{
 
public:

	UpVector( World* world, Body* body, Ogre::Vector3& pin );
	~UpVector();

	void setPin( Ogre::Vector3& pin ) { NewtonUpVectorSetPin( m_joint, &pin.x ); }
	Ogre::Vector3 getPin();


};







//////////////////////////////////////////////////////////////////////////////////
/*
	CLASS DEFINITION:

		CustomJoint 

	USE:
		this class represents a basic class for creating user-defined joints.  this class must be inherited.
*/
// OgreNewt::UpVector - simple upvector joint.
class CustomJoint : public Joint
{
 
public:

	CustomJoint( unsigned int maxDOF, Body* body0, Body* body1 );

	// must be over-written for a functioning joint.
	virtual void submitConstraint() = 0;

protected:
	virtual ~CustomJoint();

	unsigned int m_maxDOF;
	Body* m_body0;
	Body* m_body1;

	static void newtonSubmitConstraint( const NewtonJoint* me );

};




}	// end NAMESPACE BasicJoints


}	// end NAMESPACE OgreNewt

#endif
// _INCLUDE_OGRENEWT_BASICJOINTS

