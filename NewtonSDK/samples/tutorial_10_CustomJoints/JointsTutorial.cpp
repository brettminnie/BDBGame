//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

// JointsTutorial.cpp: implementation of the JointsTutorial class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "tutorial.h"
#include "Materials.h"
#include "JointsTutorial.h"
#include "CustomHinge.h"
#include "CustomSlider.h"
#include "CustomUniversal.h"
#include "CustomCorkScrew.h"
#include "CustomBallAndSocket.h"
#include "CustomConeLimitedBallAndSocket.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


/*

struct JointControlStruct
{
	dFloat m_min;
	dFloat m_max;
};

static void DestroyJointControlStruct(const NewtonJoint* slider)
{
	JointControlStruct* limits;
	limits = (JointControlStruct*) NewtonJointGetUserData (slider);
	delete limits; 
}

static unsigned SliderUpdateEvent (const NewtonJoint* slider, NewtonHingeSliderUpdateDesc* desc)
{
	dFloat distance;
	JointControlStruct* limits;

	limits = (JointControlStruct*) NewtonJointGetUserData (slider);

	distance = NewtonSliderGetJointPosit (slider);
	if (distance < limits->m_min) {
		// if the distance is smaller than the predifine intervale, stop the slider
		desc->m_accel = NewtonSliderCalculateStopAccel (slider, desc, limits->m_min);
		return 1;
	} else if (distance > limits->m_max) {
		// if the distance is larger than the predifine intervale, stop the slider
		desc->m_accel = NewtonSliderCalculateStopAccel (slider, desc, limits->m_max);
		return 1;
	}

	// no action need it if the joint angle is with the limits
	return 0;
}

static unsigned CorkScrewUpdateEvent (const NewtonJoint* screw, NewtonHingeSliderUpdateDesc* desc)
{
	unsigned retCode; 
	dFloat distance;
	dFloat omega;
	JointControlStruct* limits;

	// no action need it if the joint angle is with the limits
	retCode = 0;

	limits = (JointControlStruct*) NewtonJointGetUserData (screw);

	// the first entry in NewtonHingeSliderUpdateDesc control the screw linear acceleration 
	distance = NewtonCorkscrewGetJointPosit (screw);
	if (distance < limits->m_min) {
		// if the distance is smaller than the predifine intervale, stop the slider
		desc[0].m_accel = NewtonCorkscrewCalculateStopAccel (screw, &desc[0], limits->m_min);
		retCode |= 1;
	} else if (distance > limits->m_max) {
		// if the distance is larger than the predifine intervale, stop the slider
		desc[0].m_accel = NewtonCorkscrewCalculateStopAccel (screw, &desc[0], limits->m_max);
		retCode |= 1;
	}

	// the second entry in NewtonHingeSliderUpdateDesc control the screw angular acceleration. 
	// Make s small screw motor by setting the angular accelration of the screw axis
	// We are not going to limit the angular rotation of the screw, but is we did we should or return code with 2,
	omega = NewtonCorkscrewGetJointOmega (screw);
	desc[1].m_accel = 2.5f - 0.2f * omega;

	// or with 0x10 to tell newton this axis is active
	retCode |= 2;
	
	// return the code
	return retCode;

}


static unsigned UniversalUpdateEvent (const NewtonJoint* screw, NewtonHingeSliderUpdateDesc* desc)
{
	unsigned retCode; 
	dFloat angle;
	dFloat omega;
	JointControlStruct* limits;

	// no action need it if the joint angle is with the limits
	retCode = 0;

// chane this to #if 0 put a double limit on the universal joint
#if 1
	// the first entry in is the axis fixes of the spinning beat
	// apply a simpe motor to this object
	omega = NewtonUniversalGetJointOmega0 (screw);
	desc[0].m_accel = -4.5f - 0.2f * omega;
	// or with 0x10 to tell newton this axis is active
	retCode |= 1;

#else
	// put a top limit on the joint
	limits = (JointControlStruct*) NewtonJointGetUserData (screw);
	angle = NewtonUniversalGetJointAngle0 (screw);
	if (angle < limits->m_min) {
		// if the distance is smaller than the predifine intervale, stop the slider
		desc[0].m_accel = NewtonUniversalCalculateStopAlpha0 (screw, &desc[0], limits->m_min);
		retCode |= 1;
	} else if (angle > limits->m_max) {
		// if the distance is larger than the predifine intervale, stop the slider
		desc[0].m_accel = NewtonUniversalCalculateStopAlpha0 (screw, &desc[0], limits->m_max);
		retCode |= 1;
	}
#endif
	limits = (JointControlStruct*) NewtonJointGetUserData (screw);

	// the first entry in NewtonHingeSliderUpdateDesc control the screw linear acceleration 
	angle = NewtonUniversalGetJointAngle1 (screw);
	if (angle < limits->m_min) {
		// if the distance is smaller than the predifine intervale, stop the slider
		desc[1].m_accel = NewtonUniversalCalculateStopAlpha1 (screw, &desc[0], limits->m_min);
		retCode |= 2;
	} else if (angle > limits->m_max) {
		// if the distance is larger than the predifine intervale, stop the slider
		desc[1].m_accel = NewtonUniversalCalculateStopAlpha1 (screw, &desc[0], limits->m_max);
		retCode |= 2;
	}
	
	// return the code
	return retCode;

}
*/


// create a rope of boxes
void AddRope (NewtonWorld* nWorld)
{
	int i;
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	NewtonBody* link0;
	NewtonBody* link1;
	NewtonCustomJoint* joint;
	
	NewtonCollision* collision;
	RenderPrimitive* visualObject;


	dVector size (2.0f, 0.25f, 0.25f);

	// calculate a acurate momenet of inertia
	mass = 2.0f;
	Ixx = 0.7f * mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
	Iyy = 0.7f * mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
	Izz = 0.7f * mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;


	// create 100 tack of 10 boxes each
	//dMatrix location (GetIdentityMatrix());
	dMatrix location (dgRollMatrix(3.1426f * 0.5f));
	location.m_posit.m_y = 11.5f; 
	location.m_posit.m_z = -5.0f; 

	// create a collision primitive to be shared by all links
	collision = NewtonCreateCapsule (nWorld, size.m_y, size.m_x, NULL);
	link0 = NULL;

	// create a lon vertical rope with limits
	for (i = 0; i < 7; i ++) {
		// create the a graphic character (use a visualObject as our body
		visualObject = new CapsulePrimitive (location, size.m_y, size.m_x);

		//create the rigid body
		link1 = NewtonCreateBody (nWorld, collision);


		// add some damping to each link
		NewtonBodySetLinearDamping (link1, 0.2f);
		dVector angularDamp (0.2f, 0.2f, 0.2f);
		NewtonBodySetAngularDamping (link1, &angularDamp.m_x);

		// Set Material Id for this object
		NewtonBodySetMaterialGroupID (link1, woodID);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (link1, visualObject);

		// set a destrutor for this rigid body
		NewtonBodySetDestructorCallback (link1, PhysicsBodyDestructor);

		// set the tranform call back function
		NewtonBodySetTransformCallback (link1, PhysicsSetTransform);

		// set the force and torque call back funtion
		NewtonBodySetForceAndTorqueCallback (link1,PhysicsApplyGravityForce);

		// set the mass matrix
		NewtonBodySetMassMatrix (link1, mass, Ixx, Iyy, Izz);

		// set the matrix for tboth the rigid nody and the graphic body
		NewtonBodySetMatrix (link1, &location[0][0]);
		PhysicsSetTransform (link1, &location[0][0]);

		dVector pivot (location.m_posit);
		pivot.m_y += (size.m_x - size.m_y) * 0.5f;

		dFloat coneAngle = 2.0 * 3.1416f / 180.0f;
		dFloat twistAngle = 2.0 * 3.1416f / 180.0f;
		dVector pin (location.m_front.Scale (-1.0f));

		joint = new CustomConeLimitedBallAndSocket(twistAngle, coneAngle, pin, pivot, link1, link0);

		link0 = link1;
		location.m_posit.m_y -= (size.m_x - size.m_y);
	}


	// vrete a short horizontal rope with limits
	location = GetIdentityMatrix();
	location.m_posit.m_y = 2.5f; 
	location.m_posit.m_z = -7.0f; 
	link0 = NULL;
	for (i = 0; i < 3; i ++) {
		// create the a graphic character (use a visualObject as our body
		visualObject = new CapsulePrimitive (location, size.m_y, size.m_x);

		//create the rigid body
		link1 = NewtonCreateBody (nWorld, collision);

		// add some damping to each link
		NewtonBodySetLinearDamping (link1, 0.2f);
		dVector angularDamp (0.2f, 0.2f, 0.2f);
		NewtonBodySetAngularDamping (link1, &angularDamp.m_x);

		// Set Material Id for this object
		NewtonBodySetMaterialGroupID (link1, woodID);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (link1, visualObject);

		// make sure it is active
		NewtonWorldUnfreezeBody (nWorld, link1);
		//NewtonBodySetAutoFreeze (link1, 0);

		// set a destrutor for this rigid body
		NewtonBodySetDestructorCallback (link1, PhysicsBodyDestructor);

		// set the tranform call back function
		NewtonBodySetTransformCallback (link1, PhysicsSetTransform);

		// set the force and torque call back funtion
		NewtonBodySetForceAndTorqueCallback (link1,PhysicsApplyGravityForce);

		// set the mass matrix
		NewtonBodySetMassMatrix (link1, mass, Ixx, Iyy, Izz);

		// set the matrix for tboth the rigid nody and the graphic body
		NewtonBodySetMatrix (link1, &location[0][0]);
		PhysicsSetTransform (link1, &location[0][0]);

		dVector pivot (location.m_posit);
		pivot.m_x += (size.m_x - size.m_y) * 0.5f;

		dFloat coneAngle = 10.0 * 3.1416f / 180.0f;
		dFloat twistAngle = 10.0 * 3.1416f / 180.0f;
		dVector pin (location.m_front.Scale (-1.0f));
		joint = new CustomConeLimitedBallAndSocket(twistAngle, coneAngle, pin, pivot, link1, link0);

		link0 = link1;
		location.m_posit.m_x -= (size.m_x - size.m_y);
	}

	// release the collision geometry when not need it
	NewtonReleaseCollision (nWorld, collision);
}


/*
static unsigned DoubleDoorUserCallback (const NewtonJoint* hinge, NewtonHingeSliderUpdateDesc* desc)
{
	dFloat angle;
	const dFloat angleLimit = 90.0f;

	angle = NewtonHingeGetJointAngle (hinge);
	if (angle > ((angleLimit / 180.0f) * 3.1416f)) {
		// if the joint angle is large than the predifine intervale, stop the hinge
		desc->m_accel = NewtonHingeCalculateStopAlpha (hinge, desc, (angleLimit / 180.0f) * 3.1416f);
		return 1;
	} else if (angle < ((-angleLimit / 180.0f) * 3.1416f)) {
		// if the joint angle is large than the predifine intervale, stop the hinge
		desc->m_accel = NewtonHingeCalculateStopAlpha (hinge, desc, (-angleLimit / 180.0f) * 3.1416f);
		return 1;
	}

	// no action need it if the joint angle is with the limits
	return 0;
}
*/


void AddDoubleSwingDoors (NewtonWorld* nWorld)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	NewtonBody* link0;
	NewtonBody* link1;
	CustomHinge* joint;
	BoxPrimitive* visualObject;
	NewtonCollision* collision;

	dVector size (2.0f, 5.0f, 0.5f);

	// calculate a acurate momenet of inertia
	mass = 5.0f;
	Ixx = 0.7f * mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
	Iyy = 0.7f * mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
	Izz = 0.7f * mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

	// create 100 tack of 10 boxes each
	dMatrix location (GetIdentityMatrix());
	location.m_posit.m_x = -2.0f; 
	location.m_posit.m_y =  3.0f; 
	location.m_posit.m_z = -2.0f; 

	// create a collision primitive to be shared by all links
	collision = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, NULL); 

	// make first wing
	{
		// create the a graphic character (use a visualObject as our body
		visualObject = new BoxPrimitive (location, size);

		//create the rigid body
		link1 = NewtonCreateBody (nWorld, collision);

		// Set Material Id for this object
		NewtonBodySetMaterialGroupID (link1, woodID);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (link1, visualObject);

		// set a destrutor for this rigid body
		NewtonBodySetDestructorCallback (link1, PhysicsBodyDestructor);

		// set the tranform call back function
		NewtonBodySetTransformCallback (link1, PhysicsSetTransform);

		// set the force and torque call back funtion
		NewtonBodySetForceAndTorqueCallback (link1,PhysicsApplyGravityForce);

		// set the mass matrix
		NewtonBodySetMassMatrix (link1, mass, Ixx, Iyy, Izz);

		// set the matrix for tboth the rigid nody and the graphic body
		NewtonBodySetMatrix (link1, &location[0][0]);
		PhysicsSetTransform (link1, &location[0][0]);

		dVector pivot (location.m_posit);
		dVector pin (location.m_up);
		pivot.m_x += size.m_x * 0.5f;

		// connect these two bodies by a ball and sockect joint
		joint = new CustomHinge (pivot, pin, link1, NULL);

		joint->EnableLimits (true);
		joint->SetLimis (-30.0f * 3.1416f/180.0f, 30.0f * 3.1416f/180.0f); 
	}


	// make second wing
	{

		location.m_posit.m_x -= size.m_x;

		// create the a graphic character (use a visualObject as our body
		visualObject = new BoxPrimitive (location, size);

		//create the rigid body
		link0 = NewtonCreateBody (nWorld, collision);

		// Set Material Id for this object
		NewtonBodySetMaterialGroupID (link0, woodID);

		// save the pointer to the graphic object with the body.
		NewtonBodySetUserData (link0, visualObject);

		// set a destrutor for this rigid body
		NewtonBodySetDestructorCallback (link0, PhysicsBodyDestructor);

		// set the tranform call back function
		NewtonBodySetTransformCallback (link0, PhysicsSetTransform);

		// set the force and torque call back funtion
		NewtonBodySetForceAndTorqueCallback (link0,PhysicsApplyGravityForce);

		// set the mass matrix
		NewtonBodySetMassMatrix (link0, mass, Ixx, Iyy, Izz);

		// set the matrix for tboth the rigid nody and the graphic body
		NewtonBodySetMatrix (link0, &location[0][0]);
		PhysicsSetTransform (link0, &location[0][0]);

		dVector pivot (location.m_posit);
		dVector pin (location.m_up);
		pivot.m_x += size.m_x * 0.5f;

		// connect these two bodies by a ball and sockect joint
		//joint = NewtonConstraintCreateHinge (nWorld, &pivot.m_x, &pin.m_x, link0, link1);
		joint = new CustomHinge (pivot, pin, link0, link1);

		joint->EnableLimits (true);
		joint->SetLimis (-30.0f * 3.1416f/180.0f, 30.0f * 3.1416f/180.0f); 
	}

	// release the collision geometry when not need it
	NewtonReleaseCollision (nWorld, collision);
}




void AddRollingBeats (NewtonWorld* nWorld)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	NewtonBody* bar;
	NewtonCollision* collision;

	dMatrix location (GetIdentityMatrix());
	location.m_posit.m_x =  5.0f; 
	location.m_posit.m_y =  2.0f; 
	location.m_posit.m_z = -2.0f; 
	dVector size (10.0f, 0.25f, 0.25f);

	bar = NULL;
	// /////////////////////////////////////////////////////////////////////////////////////             
	//
	// create a bar and attach it to the world with a hinge with limits
	//
	// ////////////////////////////////////////////////////////////////////////////////////
	{
		CustomHinge* joint;
		RenderPrimitive* visualObject;

		// create the a graphic character (use a visualObject as our body
		visualObject = new CylinderPrimitive (location, size.m_y, size.m_x);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateCylinder (nWorld, size.m_y, size.m_x, NULL); 

		// craete the bar body
		bar = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

		// attach graphic object to the rigid body
		NewtonBodySetUserData(bar, visualObject);

		// set a destructor function
		NewtonBodySetDestructorCallback (bar, PhysicsBodyDestructor);

		// set the tranform call back function
		NewtonBodySetTransformCallback (bar, PhysicsSetTransform);

		// set the force and torque call back funtion
		NewtonBodySetForceAndTorqueCallback (bar,PhysicsApplyGravityForce);


		// calculate a acurate momenet of inertia
		mass = 5.0f;
		Ixx = 0.7f * mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
		Iyy = 0.7f * mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
		Izz = 0.7f * mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

		// set the mass matrix
		NewtonBodySetMassMatrix (bar, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid nody and the graphic body
		NewtonBodySetMatrix (bar, &location[0][0]);
		PhysicsSetTransform (bar, &location[0][0]);

		dVector pin (0.0f, 1.0f, 0.0f);
		dVector pivot (location.m_posit);
		pivot.m_x -= size.m_x * 0.5f;
  
		// connect these two bodies by a ball and sockect joint
		//joint = NewtonConstraintCreateHinge (nWorld, &pivot.m_x, &pin.m_x, link0, link1);
		joint = new CustomHinge (pivot, pin, bar, NULL);

		// no limits
		//joint->EnableLimits (true);
		//joint->SetAngleLimis (-30.0f * 3.1416f/180.0f, 30.0f * 3.1416f/180.0f); 
	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a sliding visualObject with limits
		//
		NewtonBody* beat;
		CustomSlider* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);
		dVector beatSize (0.5f, 2.0f, 2.0f);

		beatLocation.m_posit.m_x += size.m_x * 0.25f;

		// create the a graphic character (use a visualObject as our body
		visualObject = new BoxPrimitive (beatLocation, beatSize);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateBox (nWorld, beatSize.m_x, beatSize.m_y, beatSize.m_z, NULL); 

		beat = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

			// attach graphic object to the rigid body
		NewtonBodySetUserData(beat, visualObject);

		// set a destyuctor function
		NewtonBodySetDestructorCallback (beat, PhysicsBodyDestructor);

		// set the tranform call back function
		NewtonBodySetTransformCallback (beat, PhysicsSetTransform);

		// set the force and torque call back funtion
		NewtonBodySetForceAndTorqueCallback (beat,PhysicsApplyGravityForce);


		// calculate a acurate momenet of inertia
		mass = 5.0f;
		Ixx = 0.7f * mass * (beatSize.m_y * beatSize.m_y + beatSize.m_z * beatSize.m_z) / 12.0f;
		Iyy = 0.7f * mass * (beatSize.m_x * beatSize.m_x + beatSize.m_z * beatSize.m_z) / 12.0f;
		Izz = 0.7f * mass * (beatSize.m_x * beatSize.m_x + beatSize.m_y * beatSize.m_y) / 12.0f;

		// set the mass matrix
		NewtonBodySetMassMatrix (beat, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid nody and the graphic body
		NewtonBodySetMatrix (beat, &beatLocation[0][0]);
		PhysicsSetTransform (beat, &beatLocation[0][0]);

		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_front);
		joint = new CustomSlider (pivot, pin, beat, bar);

		// claculate the minimum and maximum limit for this joints
		dFloat minLimits = ((location.m_posit.m_x - beatLocation.m_posit.m_x) - size.m_x * 0.5f);
		dFloat maxLimits = ((location.m_posit.m_x - beatLocation.m_posit.m_x) + size.m_x * 0.5f);

		joint->EnableLimits(true);
		joint->SetLimis (minLimits, maxLimits); 
	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a corkscrew visualObject with limits
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		NewtonBody* beat;
		CustomCorkScrew* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);
		dVector beatSize (0.5f, 1.25f, 1.25f);

		beatLocation.m_posit.m_x -= size.m_x * 0.25f;

		// create the a graphic character (use a visualObject as our body
		//visualObject = new BoxPrimitive (beatLocation, beatSize);
		visualObject = new ChamferCylinderPrimitive (beatLocation, beatSize.m_y, beatSize.m_x);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateChamferCylinder (nWorld, beatSize.m_y, beatSize.m_x, NULL); 

		beat = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

			// attach graphic object to the rigid body
		NewtonBodySetUserData(beat, visualObject);

		// set a destyuctor function
		NewtonBodySetDestructorCallback (beat, PhysicsBodyDestructor);

		// set the tranform call back function
		NewtonBodySetTransformCallback (beat, PhysicsSetTransform);

		// set the force and torque call back funtion
		NewtonBodySetForceAndTorqueCallback (beat,PhysicsApplyGravityForce);


		// calculate a acurate momenet of inertia
		mass = 5.0f;
		Ixx = 0.7f * mass * (beatSize.m_y * beatSize.m_y + beatSize.m_z * beatSize.m_z) / 12.0f;
		Iyy = 0.7f * mass * (beatSize.m_x * beatSize.m_x + beatSize.m_z * beatSize.m_z) / 12.0f;
		Izz = 0.7f * mass * (beatSize.m_x * beatSize.m_x + beatSize.m_y * beatSize.m_y) / 12.0f;

		// set the mass matrix
		NewtonBodySetMassMatrix (beat, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid nody and the graphic body
		NewtonBodySetMatrix (beat, &beatLocation[0][0]);
		PhysicsSetTransform (beat, &beatLocation[0][0]);

		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin (beatLocation.m_front);
		joint = new CustomCorkScrew (pivot, pin, beat, bar);

		// claculate the minimum and maximum limit for this joints
		dFloat minLimits = ((location.m_posit.m_x - beatLocation.m_posit.m_x) - size.m_x * 0.5f);
		dFloat maxLimits = ((location.m_posit.m_x - beatLocation.m_posit.m_x) + size.m_x * 0.5f);

		joint->EnableLimits(true);
		joint->SetLimis (minLimits, maxLimits); 
	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a universal joint visualObject with limits
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		NewtonBody* beat;
		CustomUniversal* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);
		dVector beatSize (0.5f, 1.25f, 1.25f);

		beatLocation.m_posit.m_x -= size.m_x * 0.5f;

		// create the a graphic character (use a visualObject as our body
		//visualObject = new BoxPrimitive (beatLocation, beatSize);
		visualObject = new ChamferCylinderPrimitive (beatLocation, beatSize.m_y, beatSize.m_x);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateChamferCylinder (nWorld, beatSize.m_y, beatSize.m_x, NULL); 

		beat = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

			// attach graphic object to the rigid body
		NewtonBodySetUserData(beat, visualObject);

		// set a destyuctor function
		NewtonBodySetDestructorCallback (beat, PhysicsBodyDestructor);

		// set the tranform call back function
		NewtonBodySetTransformCallback (beat, PhysicsSetTransform);

		// set the force and torque call back funtion
		NewtonBodySetForceAndTorqueCallback (beat,PhysicsApplyGravityForce);


		// calculate a acurate momenet of inertia
		mass = 5.0f;
		Ixx = 0.7f * mass * (beatSize.m_y * beatSize.m_y + beatSize.m_z * beatSize.m_z) / 12.0f;
		Iyy = 0.7f * mass * (beatSize.m_x * beatSize.m_x + beatSize.m_z * beatSize.m_z) / 12.0f;
		Izz = 0.7f * mass * (beatSize.m_x * beatSize.m_x + beatSize.m_y * beatSize.m_y) / 12.0f;

		// set the mass matrix
		NewtonBodySetMassMatrix (beat, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid nody and the graphic body
		NewtonBodySetMatrix (beat, &beatLocation[0][0]);
		PhysicsSetTransform (beat, &beatLocation[0][0]);

		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin0 (beatLocation.m_front);
		dVector pin1 (beatLocation.m_up);
		// tell this joint to destroiy its local private data when destroyed
		joint = new CustomUniversal (pivot, pin0, pin1, beat, bar);
	}


	{
		// ////////////////////////////////////////////////////////////////////////////////////
		//
		// add a universal joint visualObject with limits
		//
		// ////////////////////////////////////////////////////////////////////////////////////
		NewtonBody* beat;
		CustomUniversal* joint;
		RenderPrimitive* visualObject;
		dMatrix beatLocation (location);
		dVector beatSize (0.5f, 1.25f, 1.25f);

		beatLocation.m_posit.m_x = size.m_x;

		// create the a graphic character (use a visualObject as our body
		//visualObject = new BoxPrimitive (beatLocation, beatSize);
		visualObject = new ChamferCylinderPrimitive (beatLocation, beatSize.m_y, beatSize.m_x);

		// create a collision primitive to be shared by all links
		collision = NewtonCreateChamferCylinder (nWorld, beatSize.m_y, beatSize.m_x, NULL); 

		beat = NewtonCreateBody(nWorld, collision);
		NewtonReleaseCollision (nWorld, collision);

			// attach graphic object to the rigid body
		NewtonBodySetUserData(beat, visualObject);

		// set a destyuctor function
		NewtonBodySetDestructorCallback (beat, PhysicsBodyDestructor);

		// set the tranform call back function
		NewtonBodySetTransformCallback (beat, PhysicsSetTransform);

		// set the force and torque call back funtion
		NewtonBodySetForceAndTorqueCallback (beat,PhysicsApplyGravityForce);


		// calculate a acurate momenet of inertia
		mass = 5.0f;
		Ixx = 0.7f * mass * (beatSize.m_y * beatSize.m_y + beatSize.m_z * beatSize.m_z) / 12.0f;
		Iyy = 0.7f * mass * (beatSize.m_x * beatSize.m_x + beatSize.m_z * beatSize.m_z) / 12.0f;
		Izz = 0.7f * mass * (beatSize.m_x * beatSize.m_x + beatSize.m_y * beatSize.m_y) / 12.0f;

		// set the mass matrix
		NewtonBodySetMassMatrix (beat, mass, Ixx, Iyy, Izz);

		// set the matrix for both the rigid nody and the graphic body
		NewtonBodySetMatrix (beat, &beatLocation[0][0]);
		PhysicsSetTransform (beat, &beatLocation[0][0]);

		// set the pivot relative for the first bar
		dVector pivot (beatLocation.m_posit); 
		dVector pin0 (beatLocation.m_front.Scale(-1.0f));
		dVector pin1 (beatLocation.m_up);
		// tell this joint to destroiy its local private data when destroyed
		joint = new CustomUniversal (pivot, pin0, pin1, beat, bar);
	}

}