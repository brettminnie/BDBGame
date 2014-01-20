/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/
#ifndef _INCLUDE_OGRENEWT_CONTACTCALLBACK
#define _INCLUDE_OGRENEWT_CONTACTCALLBACK

#include <newton.h>
#include "OgreNewt_Body.h"

// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{


/*
	CLASS DEFINITION:

		ContactCallback

	USE:
		this class is for creating custom behavior between material GroupIDs.
		this class must be inherited, and the ContactProcess function created, and then
		added to a MaterialPair class.
*/
// OgreNewt::ContactCallback
class ContactCallback
{
 protected:

	NewtonMaterial* m_material;
	NewtonContact* m_contact;

	OgreNewt::Body* m_body0;
	OgreNewt::Body* m_body1;

public:

	ContactCallback();	// constructor
	~ContactCallback();					// desctructor

	// basic contact control commands...
	void disableContact() { NewtonMaterialDisableContact( m_material ); }
	Ogre::Real getCurrentTimestep() { return (Ogre::Real)NewtonMaterialGetCurrentTimestep( m_material ); }
	unsigned getContactFaceAttribute() { return NewtonMaterialGetContactFaceAttribute( m_material ); }
	unsigned getBodyCollisionID( OgreNewt::Body* body ) { return NewtonMaterialGetBodyCollisionID( m_material, body->getNewtonBody() ); }
	Ogre::Real getContactNormalSpeed() { return (Ogre::Real)NewtonMaterialGetContactNormalSpeed( m_material, m_contact ); }
	Ogre::Vector3 getContactForce();
	void getContactPositionAndNormal( Ogre::Vector3& pos, Ogre::Vector3& norm );
	void getContactTangentDirections( Ogre::Vector3& dir0, Ogre::Vector3& dir1 );
	Ogre::Real getContactTangentSpeed( int index ) { return (Ogre::Real)NewtonMaterialGetContactTangentSpeed( m_material, m_contact, index ); }

	void setContactSoftness( Ogre::Real softness ) { NewtonMaterialSetContactSoftness( m_material, (float)softness ); }
	void setContactElasticity( Ogre::Real elasticity ) { NewtonMaterialSetContactElasticity( m_material, (float)elasticity ); }
	void setContactFrictionState( int state, int index ) { NewtonMaterialSetContactFrictionState( m_material, state, index ); }
	void setContactStaticFrictionCoef( Ogre::Real coef, int index ) { NewtonMaterialSetContactStaticFrictionCoef( m_material, (float)coef, index ); }
	void setContactKineticFrictionCoef( Ogre::Real coef, int index ) { NewtonMaterialSetContactKineticFrictionCoef( m_material, (float)coef, index ); }
	void setContactTangentAcceleration( Ogre::Real accel, int index ) { NewtonMaterialSetContactTangentAcceleration( m_material, (float)accel, index ); }
	void rotateTangentDirections( Ogre::Vector3& dir ) { NewtonMaterialContactRotateTangentDirections( m_material, &dir.x ); }


	// protected callbacks are the same for all implementations...
	static int _cdecl contactBegin( const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1 );
	static int _cdecl contactProcess( const NewtonMaterial* material, const NewtonContact* contact );
	static void _cdecl contactEnd( const NewtonMaterial* material );


	// user-defined callback function.
	
	// this function is called when 2 bodies AABB overlap.  they have not yet collided, but *may* do so this loop.
	// at this point, m_body0 and m_body1 are defined, but the contact isn't yet valid, so none of the member functions
	// can be called yet.  they must be called from the userProcess() function.
	// return 0 to ignore the collision, 1 to allow it.
	virtual int userBegin() { return 1; }

	// user process function.  is called for each contact between the 2 bodies.  all member functions are valid from
	// within this function, and will affect the current contact.  return 0 to ignore the collision, 1 to allow it.
	virtual int userProcess() { return 1; }

	// called after all contacts between the 2 bodies have been processed. no member functions should be called from within this
	// function either, as all contacts have been processed at this point.
	virtual void userEnd() {  }
};


}	// end NAMESPACE OgreNewt

#endif
// _INCLUDE_OGRENEWT_CONTACTCALLBACK

