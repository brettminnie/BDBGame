/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/

#ifndef _INCLUDE_OGRENEWT_BODY
#define _INCLUDE_OGRENEWT_BODY


#include <Ogre.h>
#include <newton.h>
#include "OgreNewt_World.h"
#include "OgreNewt_Collision.h"
#include "OgreNewt_MaterialID.h"


// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{

/*
	CLASS DEFINITION:

		Body

	USE:
		this class represents a NewtonBody rigid body!
*/
// OgreNewt::Body - main class for all Rigid Bodies in the system.
class Body
{
public:
	// force callback pointer
	typedef void(_cdecl *ForceCallback)( Body* me );
	typedef void(_cdecl *TransformCallback)( Body* me );
	typedef void(_cdecl *IteratorCallback)( Body* me );
	


 protected:

	NewtonBody* m_body;
	Collision* m_collision;
	MaterialID* m_matid;
	World* m_world;
	

	void* m_userdata;
	
	int m_type;
	Ogre::SceneNode* m_node;

	ForceCallback m_forcecallback;
	TransformCallback m_transformcallback;

private:

	static void _cdecl newtonDestructor( const NewtonBody* body );

	static void _cdecl newtonTransformCallback( const NewtonBody* body, const float* matrix );
	static void _cdecl newtonForceTorqueCallback( const NewtonBody* body );

	// standard gravity force callback.
	static void _cdecl standardForceCallback( Body* me );

 public:
	 Body( World* W, OgreNewt::Collision* col, int bodytype = 0 );	// constructor
	~Body();				// destructor

	// set user data to connect this class to another.
	void setUserData( void* data ) { m_userdata = data; }

	// retrieve pointer to previously set user data.
	void* getUserData() { return m_userdata; }

	// get a pointer to the NewtonBody object
	NewtonBody* getNewtonBody() { return m_body; }

	// get a pointer to the attached SceneNode.
	Ogre::SceneNode* getOgreNode() { return m_node; }

	// get a pointer to the OgreNewt::World this body belongs to.
	OgreNewt::World* getWorld() { return m_world; }

	// set the type for this body, used for differentation in material callbacks.
	void setType( int type ) { m_type = type; }

	// get the type set for this body.
	int getType() { return m_type; }

	// attach this body to an Ogre::SceneNode*, and position and rotate the node automatically
	void attachToNode( Ogre::SceneNode* node );

	// set a standard gravity callback for this body to use.
	void setStandardForceCallback();

	// set a custom force callback for this body to use.
	void setCustomForceAndTorqueCallback( ForceCallback callback );

	// remove any force callbacks.
	void removeForceAndTorqueCallback() { NewtonBodySetForceAndTorqueCallback( m_body, NULL );  m_forcecallback = NULL; }

	void setCustomTransformCallback( TransformCallback callback );

	// remove any transform callbacks.
	void removeTransformCallback() { m_transformcallback = NULL; }


	// position and orient the body arbitrarily.
	void setPositionOrientation( Ogre::Quaternion orient, Ogre::Vector3 pos );

	//set the mass and inertia for the body.
	void setMassMatrix( Ogre::Real mass, Ogre::Vector3 inertia );

	// freeze the rigid body.
	void freeze() { NewtonWorldFreezeBody( m_world->getNewtonWorld(), m_body ); }

	// unfreeze the rigid body.
	void unFreeze() { NewtonWorldUnfreezeBody( m_world->getNewtonWorld(), m_body ); }

	// set the material for the body, used in adjusting collision behavior
	void setMaterialGroupID( MaterialID* ID ) { m_matid = ID; NewtonBodySetMaterialGroupID( m_body, m_matid->getID() ); }
	
	// prevents fast moving bodies from "tunneling" through other bodies.  *NOT YET IMPLEMENTED
	void setContinuousCollisionMode( unsigned state ) { NewtonBodySetContinuousCollisionMode( m_body, state ); }

	// set whether all parent/children pairs connected to this body should be allowed to collide.
	void setJointRecursiveCollision( unsigned state ) { NewtonBodySetJointRecursiveCollision( m_body, state ); }

	// set an arbitrar omega for the body.
	void setOmega( Ogre::Vector3& omega ) { NewtonBodySetOmega( m_body, &omega.x ); }

	// set an arbitrar velocity for the body.
	void setVelocity( Ogre::Vector3& vel ) { NewtonBodySetVelocity( m_body, &vel.x ); }

	// set the linear damping for the body.
	void setLinearDamping( Ogre::Real damp ) { NewtonBodySetLinearDamping( m_body, (float)damp ); }

	// set the angular damping for the body.
	void setAngularDamping( Ogre::Vector3& damp ) { NewtonBodySetAngularDamping( m_body, &damp.x ); }


	void setCoriolisForcesMode( int mode ) { NewtonBodyCoriolisForcesMode( m_body, mode ); }

	// set the collision that represents the shape of the body.  pass an OgreNewt::Collision object.
	void setCollision( OgreNewt::Collision* col );

	// set whether the body should "freeze" when equilibruim is reached.  user-controlled bodies should disable this behavior.
	void setAutoFreeze( int flag ) { NewtonBodySetAutoFreeze ( m_body, flag); }

	// set the factors that cause a body to "freeze" when equilibrium reached.
	void setFreezeThreshold( Ogre::Real speed, Ogre::Real omega, int framecount ) { NewtonBodySetFreezeTreshold( m_body, (float)speed, (float)omega, framecount ); }

	// get a pointer to the OgreNewt::Collision for this body
	OgreNewt::Collision* Body::getCollision();

	// get a pointer to the Material assigned to this body.
	OgreNewt::MaterialID* getMaterialGroupID();

	// returns current setting for this body.
	int getContinuousCollisionMode() { return NewtonBodyGetContinuousCollisionMode( m_body ); }

	// returns current setting for this body.
	int getJointRecursiveCollision() { return NewtonBodyGetJointRecursiveCollision( m_body ); }

	// get position and orientation in form of an Ogre::Vector(position) and Ogre::Quaternion(orientation)
	void getPositionOrientation( Ogre::Quaternion& orient, Ogre::Vector3& pos );

	// get Ogre::Real(mass) and Ogre::Vector3(inertia) of the body.
	void getMassMatrix( Ogre::Real& mass, Ogre::Vector3& inertia );

	// get invert mass + inertia for the body.
	void getInvMass( Ogre::Real& mass, Ogre::Vector3& inertia );

	// get omega of the body. 
	Ogre::Vector3 getOmega();

	// get velocity of the body.  in global coordinates.
	Ogre::Vector3 getVelocity();

	// is the objct frozen?  1 = yes, 0 = no
	int getSleepingState() { return NewtonBodyGetSleepingState( m_body ); }

	// get auto-freeze state for the body
	int getAutoFreeze() { NewtonBodyGetAutoFreeze( m_body ); }

	// get linear damping
	Ogre::Real getLinearDamping() { return (Ogre::Real)NewtonBodyGetLinearDamping( m_body ); }

	// get angular damping
	Ogre::Vector3 getAngularDamping();

	// freeze threshold
	void getFreezeThreshold( Ogre::Real& speed, Ogre::Real& omega ) { NewtonBodyGetFreezeTreshold( m_body, &speed, &omega ); }

	// utility function that gives the volume of a body.  useful for perfecting buoyancy settings.
	Ogre::Real getTotalVolume() { return (float)NewtonBodyGetTotalVolume( m_body ); }

	// add an impulse (relative change in velocity) to a body.  values are in world coordinates.
	void addImpulse( Ogre::Vector3& deltav, Ogre::Vector3& posit ) { NewtonAddBodyImpulse( m_body, &deltav.x, &posit.x ); }

	// add force to the body.  this function is only valid inside a ForceCallback function!
	void addForce( Ogre::Vector3& force ) { NewtonBodyAddForce( m_body, &force.x ); }

	// add torque to the body.  this function is only valid inside a ForceCallback function!
	void addTorque( Ogre::Vector3& torque ) { NewtonBodyAddTorque( m_body, &torque.x ); }

	// set the force for a body.  this function is only valid inside a ForceCallback function!
	void setForce( Ogre::Vector3& force ) { NewtonBodySetForce( m_body, &force.x ); }

	// set the torque for a body.  this function is only valid inside a ForceCallback function!
	void setTorque( Ogre::Vector3& torque ) { NewtonBodySetTorque( m_body, &torque.x ); }


	// helper function that adds a force (and resulting torque) to an object in global cordinates
	void addGlobalForce( Ogre::Vector3& force, Ogre::Vector3& pos );

	// helper function that adds a force (and resulting torque) to an object in local coordinates
	void addLocalForce( Ogre::Vector3& force, Ogre::Vector3& pos );
};




}

#endif
// _INCLUDE_OGRENEWT_BODY

