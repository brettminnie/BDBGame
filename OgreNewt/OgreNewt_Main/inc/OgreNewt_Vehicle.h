/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/
#ifndef _INCLUDE_OGRENEWT_VEHICLE
#define _INCLUDE_OGRENEWT_VEHICLE

#include <newton.h>
#include "OgreNewt_Body.h"
#include "OgreNewt_Joint.h"
#include "OgreNewt_World.h"

// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{

/*
	CLASS DEFINITION:

		Vehicle 

	USE:
		this class represents a basic vehicle, meant to be inherited by the user, with functionality added.
*/
// OgreNewt::Vehicle
	class Vehicle
	{
	public:
		// secondary class: Tire.  this represents a basic tire.  you add tires by simply creating new tire objects, and attaching them to the vehicle.
		class Tire
		{
		public:
			Tire( OgreNewt::Vehicle* vehicle, Ogre::Quaternion localorient, Ogre::Vector3 localpos, Ogre::Vector3 pin,
				Ogre::Real mass, Ogre::Real width, Ogre::Real radius, Ogre::Real susShock, Ogre::Real susSpring, Ogre::Real susLength, int colID = 0);	// constructor.

			virtual ~Tire();	// destructor.

			// attach a scenenode to the tire!
			void attachToNode( Ogre::SceneNode* node ) { m_node = node; }

			// update the position of the tire.  this must be called to update the attached scene node to the position of the tire!
			void updateNode();

			// get the Newton ID for this tire.
			int getNewtonID() { return m_tireid; }

			// get the parent vehicle.
			OgreNewt::Vehicle* getVehicle() { return m_vehicle; }

			// get Ogre::SceneNode.
			Ogre::SceneNode* getOgreNode() { return m_node; }

			//////////////////////////////////////////////////////////////////////
			// Newton functions
			
			// is the tire airborne?
			int isAirBorne() { return NewtonVehicleTireIsAirBorne( m_vehicle->getNewtonVehicle(), m_tireid ); }

			// has the tire lost side grip?
			int lostSideGrip() { return NewtonVehicleTireLostSideGrip( m_vehicle->getNewtonVehicle(), m_tireid ); }

			// has the tire lost traction?
			int lostTraction() { return NewtonVehicleTireLostTraction( m_vehicle->getNewtonVehicle(), m_tireid ); }

			// get the rotational velocity of the tire
			Ogre::Real getOmega() { return (Ogre::Real)NewtonVehicleGetTireOmega( m_vehicle->getNewtonVehicle(), m_tireid ); }

			// get the load on the tire (along the suspension normal )
			Ogre::Real getNormalLoad() { return (Ogre::Real)NewtonVehicleGetTireNormalLoad( m_vehicle->getNewtonVehicle(), m_tireid ); }

			// get the current steering angle for this tire
			Ogre::Radian getSteeringAngle() { return Ogre::Radian( NewtonVehicleGetTireSteerAngle( m_vehicle->getNewtonVehicle(), m_tireid ) ); }

			// get the lateral speed of the tire (sideways)
			Ogre::Real getLateralSpeed() { return NewtonVehicleGetTireLateralSpeed( m_vehicle->getNewtonVehicle(), m_tireid ); }

			// get the longitudinal speed of the tire (forward/backward)
			Ogre::Real getLongitudinalSpeed() { return NewtonVehicleGetTireLongitudinalSpeed( m_vehicle->getNewtonVehicle(), m_tireid ); }

			// get the location and orientation of the tire (in global space).
			void getPositionOrientation( Ogre::Quaternion& orient, Ogre::Vector3& pos );

			// set the torque for this tire.  this must be called in the tire callback!
			void setTorque( Ogre::Real torque ) { NewtonVehicleSetTireTorque( m_vehicle->getNewtonVehicle(), m_tireid, torque ); }

			// set the steering angle for the tire.  this must be called in the tire callback.
			void setSteeringAngle( Ogre::Radian angle ) { NewtonVehicleSetTireSteerAngle( m_vehicle->getNewtonVehicle(), m_tireid, angle.valueRadians() ); }

			// calculate the max brake acceleration to stop the tires.
			Ogre::Real calculateMaxBrakeAcceleration() { return (Ogre::Real)NewtonVehicleTireCalculateMaxBrakeAcceleration( m_vehicle->getNewtonVehicle(), m_tireid ); }

			// set the brake acceleration
			void setBrakeAcceleration( Ogre::Real accel, Ogre::Real limit ) { NewtonVehicleTireSetBrakeAcceleration( m_vehicle->getNewtonVehicle(), m_tireid, (float)accel, (float)limit ); }

			// max side slip speed
			void setMaxSideSlipSpeed( Ogre::Real speed ) { NewtonVehicleSetTireMaxSideSleepSpeed( m_vehicle->getNewtonVehicle(), m_tireid, (float)speed ); }

			// set side slip coefficient
			void setSideSlipCoefficient( Ogre::Real coefficient ) { NewtonVehicleSetTireSideSleepCoeficient( m_vehicle->getNewtonVehicle(), m_tireid, (float)coefficient ); }

			// max longitudinal slip speed
			void setMaxLongitudinalSlipSpeed( Ogre::Real speed ) { NewtonVehicleSetTireMaxLongitudinalSlideSpeed( m_vehicle->getNewtonVehicle(), m_tireid, (float)speed ); }

			// set longitudinal slip coefficient
			void setLongitudinalSlipCoefficient( Ogre::Real coefficient ) { NewtonVehicleSetTireLongitudinalSlideCoeficient( m_vehicle->getNewtonVehicle(), m_tireid, (float)coefficient ); }




		private:
			OgreNewt::Vehicle* m_vehicle;
			int m_tireid;

			Ogre::SceneNode* m_node;

		};


		Vehicle() { m_vehicle = NULL; }		// constructor
		virtual ~Vehicle();		// destructor (can be overridden if necessary).

		void init( OgreNewt::Body* chassis, Ogre::Vector3 updir );		// must be called to setup the vehicle.  this function calls the
																		// virtual function "Setup()", which should be overridden by the user.

		void destroy();		// destroy the vehicle, including the chassis body.

		virtual void setup() = 0;	// MUST BE OVERRIDDEN BY USER!


		virtual void userCallback() {}	// should be overridden to provide vehicle control / steering / etc.


		// get the chassis body.
		OgreNewt::Body* getChassisBody() { return m_chassis; }

		// get the NewtonJoint for the vehicle.
		NewtonJoint* getNewtonVehicle() { return m_vehicle; }

		//////////////////////////////////////////////////////////////
		// Newton Vehicle functions.

		// reset the vehicle (stop all tires)
		void reset() { NewtonVehicleReset( m_vehicle ); }

		// get a pointer to the first tire in the vehicle.  this should be used in the userCallback().
		OgreNewt::Vehicle::Tire* getFirstTire();

		// get a pointer to the next tire in the vehicle.  this can be used to loop through all tires in the userCallback().
		OgreNewt::Vehicle::Tire* getNextTire( OgreNewt::Vehicle::Tire* current_tire );

		// balance tires.  this is the last function that should be called after all tires have been added, to finalize the vehicle.
		void balanceTires( Ogre::Real gravityMag ) { NewtonVehicleBalanceTires( m_vehicle, gravityMag ); }


	private:
		// callback for newton... it calls the userCallback() function for you.
		static void _cdecl newtonCallback( const NewtonJoint* me );

		static void _cdecl newtonDestructor( const NewtonJoint* me );

		NewtonJoint* m_vehicle;
		OgreNewt::Body* m_chassis;

	};



}	// end NAMESPACE OgreNewt


#endif	// _INCLUDE_OGRENEWT_VEHICLE