//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

// CustomHinge.cpp: implementation of the CustomHinge class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "CustomHinge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MIN_JOINT_PIN_LENGTH	50.0f

CustomHinge::CustomHinge(const dVector& pivot, const dVector& pin, NewtonBody* child, NewtonBody* parent)
	:NewtonCustomJoint(6, child, parent)
{
	m_limitsOn = false;
	m_minAngle = -45.0f * 3.1416f / 180.0f;
	m_maxAngle =  45.0f * 3.1416f / 180.0f;

	// calculate the two local matrix of the pivot point
	CalculateLocalMatrix (pivot, pin, m_localMatrix0, m_localMatrix1);
}

CustomHinge::~CustomHinge()
{
}


void CustomHinge::EnableLimits(bool state)
{
	m_limitsOn = state;
}

void CustomHinge::SetLimis(dFloat minAngle, dFloat maxAngle)
{
	_ASSERTE (minAngle < 0.0f);
	_ASSERTE (maxAngle > 0.0f);

	m_minAngle = minAngle;
	m_maxAngle = maxAngle;
}



void CustomHinge::SubmitConstrainst ()
{
	dFloat angle;
	dFloat sinAngle;
	dFloat cosAngle;
	dMatrix matrix0;
	dMatrix matrix1;

	// calculate the position of the pivot point and the jacobian direction vectors, in global space. 
	CalculateGlobalMatrix (m_localMatrix0, m_localMatrix1, matrix0, matrix1);

	// Restrict the movemenet on the pivot point along all tree orthonormal direction
	NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_front[0]);
	NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_up[0]);
	NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_right[0]);
	
	// get a point along the pin axis at some resonable large distance from the pivot
	dVector q0 (matrix0.m_posit + matrix0.m_front.Scale(MIN_JOINT_PIN_LENGTH));
	dVector q1 (matrix1.m_posit + matrix1.m_front.Scale(MIN_JOINT_PIN_LENGTH));

	// two contraints row perpendicular to the pin vector
 	NewtonUserJointAddLinearRow (m_joint, &q0[0], &q1[0], &matrix0.m_up[0]);
	NewtonUserJointAddLinearRow (m_joint, &q0[0], &q1[0], &matrix0.m_right[0]);

	// if limit are enable ...
	if (m_limitsOn) {
		// the joint angle can be determine by getting the angle between any two non parallel vectors
		sinAngle = (matrix0.m_up * matrix1.m_up) % matrix0.m_front;
		cosAngle = matrix0.m_up % matrix1.m_up;
		angle = atan2f (sinAngle, cosAngle);

		if (angle < m_minAngle) {
			// get a point along the up vector and set a constraint  
			NewtonUserJointAddAngularRow (m_joint, 0.0f, &matrix0.m_front[0]);
			NewtonUserJointSetRowMaximunFriction (m_joint, 0.0f);
			
		} else if (angle > m_maxAngle) {
			NewtonUserJointAddAngularRow (m_joint, 0.0f, &matrix0.m_front[0]);
			NewtonUserJointSetRowMinimunFriction (m_joint, 0.0f);
		}
	}
 }
