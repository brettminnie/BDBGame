//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

// CustomCorkScrew.cpp: implementation of the CustomCorkScrew class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "CustomCorkScrew.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MIN_JOINT_PIN_LENGTH	50.0f

CustomCorkScrew::CustomCorkScrew(const dVector& pivot, const dVector& pin, NewtonBody* child, NewtonBody* parent)
	:NewtonCustomJoint(6, child, parent)
{
	m_limitsOn = false;
	m_minDist = -1.0f;
	m_maxDist =  1.0f;

	m_angularmotorOn = true;
	m_angularDamp = 0.1f;
	m_angularAccel = 5.0f;

	// calculate the two local matrix of the pivot point
	CalculateLocalMatrix (pivot, pin, m_localMatrix0, m_localMatrix1);
}

CustomCorkScrew::~CustomCorkScrew()
{
}


void CustomCorkScrew::EnableLimits(bool state)
{
	m_limitsOn = state;
}


void CustomCorkScrew::SetLimis(dFloat minDist, dFloat maxDist)
{
	_ASSERTE (minDist < 0.0f);
	_ASSERTE (maxDist > 0.0f);

	m_minDist = minDist;
	m_maxDist = maxDist;
}


void CustomCorkScrew::SubmitConstrainst ()
{
	dFloat dist;
	dMatrix matrix0;
	dMatrix matrix1;

	// calculate the position of the pivot point and the jacobiam direction vectors, in global space. 
	CalculateGlobalMatrix (m_localMatrix0, m_localMatrix1, matrix0, matrix1);

	// Restrict the movemenet on the pivot point along all tree orthonormal direction
	dVector p0 (matrix0.m_posit);
	dVector p1 (matrix1.m_posit + matrix1.m_front.Scale ((p0 - matrix1.m_posit) % matrix1.m_front));

	NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &matrix0.m_up[0]);
	NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &matrix0.m_right[0]);
	
	// get a point along the pin axis at some resonable large distance from the pivot
	dVector q0 (p0 + matrix0.m_front.Scale(MIN_JOINT_PIN_LENGTH));
	dVector q1 (p1 + matrix1.m_front.Scale(MIN_JOINT_PIN_LENGTH));

	// two contraints row perpendiculate to the hinge pin
 	NewtonUserJointAddLinearRow (m_joint, &q0[0], &q1[0], &matrix0.m_up[0]);
	NewtonUserJointAddLinearRow (m_joint, &q0[0], &q1[0], &matrix0.m_right[0]);

	// if limit are enable ...
	if (m_limitsOn) {
		dist = (matrix0.m_posit - matrix1.m_posit) % matrix0.m_front;
		if (dist < m_minDist) {
			// get a point along the up vector and set a constraint  
			NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix0.m_posit[0], &matrix0.m_front[0]);
			// allow the object to return but not to kicp going forward
			NewtonUserJointSetRowMinimunFriction (m_joint, 0.0f);
			
		} else if (dist > m_maxDist) {
			// get a point along the up vector and set a constraint  
			NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix0.m_posit[0], &matrix0.m_front[0]);
			// allow the object to return but not to kicp going forward
			NewtonUserJointSetRowMaximunFriction (m_joint, 0.0f);
		}
	}

	if (m_angularmotorOn) {
		dFloat relOmega;
		dFloat relAccel;
		dVector omega0 (0.0f, 0.0f, 0.0f);
		dVector omega1 (0.0f, 0.0f, 0.0f);


		// get reletive angular velocity
		NewtonBodyGetOmega(m_body0, &omega0[0]);
		if (m_body1) {
			NewtonBodyGetOmega(m_body1, &omega1[0]);
		}

		// calculate the derired accelration
		relOmega = (omega0 - omega1) % matrix0.m_front;
		relAccel = m_angularAccel - m_angularDamp * relOmega;

		// if the motor capabiotity is on set and ang
		NewtonUserJointAddAngularRow (m_joint, 0.0f, &matrix0.m_front[0]);
		
		// set the acceltation for the joint row
		NewtonUserJointSetRowAcceleration (m_joint, relAccel);
	}
 }
