//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

// CustomUniversal.cpp: implementation of the CustomUniversal class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "CustomUniversal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MIN_JOINT_PIN_LENGTH	50.0f

CustomUniversal::CustomUniversal(const dVector& pivot, const dVector& pin0, const dVector& pin1, NewtonBody* child, NewtonBody* parent)
	:NewtonCustomJoint(6, child, parent)
{
	dMatrix matrix0;

	m_limitsOn = true;
	m_minAngle = -45.0f * 3.1416f / 180.0f;
	m_maxAngle =  45.0f * 3.1416f / 180.0f;

	m_angularmotorOn = true;
	m_angularDamp = 0.1f;
	m_angularAccel = 5.0f;

	// Get the global matrices of each rigid body.
	NewtonBodyGetMatrix(m_body0, &matrix0[0][0]);

	dMatrix matrix1 (GetIdentityMatrix());
	if (m_body1) {
		NewtonBodyGetMatrix(m_body1, &matrix1[0][0]);
	}

	// create a global matrix at the pivot point with front vector algined to the drin vector
	dMatrix pinAndPivotMatrix;
	pinAndPivotMatrix.m_front = pin0.Scale (1.0f / sqrtf (pin0 % pin0));
	pinAndPivotMatrix.m_right = pin0 * pin1;
	pinAndPivotMatrix.m_right = pinAndPivotMatrix.m_right.Scale (1.0f / sqrtf (pinAndPivotMatrix.m_right % pinAndPivotMatrix.m_right));
	pinAndPivotMatrix.m_up = pinAndPivotMatrix.m_right * pinAndPivotMatrix.m_front;

	pinAndPivotMatrix.m_posit = pivot;

	// calcultate the relative matrix of the pin and pivot on each body
 	m_localMatrix0 = pinAndPivotMatrix * matrix0.Inverse();
	m_localMatrix1 = pinAndPivotMatrix * matrix1.Inverse();
}

CustomUniversal::~CustomUniversal()
{
}


void CustomUniversal::EnableLimits(bool state)
{
	m_limitsOn = state;
}


void CustomUniversal::SetLimis(dFloat minAngle, dFloat maxAngle)
{
	_ASSERTE (minAngle < 0.0f);
	_ASSERTE (maxAngle > 0.0f);

	m_minAngle = minAngle;
	m_maxAngle = maxAngle;
}


void CustomUniversal::SubmitConstrainst ()
{
	dFloat angle;
	dFloat sinAngle;
	dFloat cosAngle;
	dMatrix matrix0;
	dMatrix matrix1;

	// calculate the position of the pivot point and the jacoviam direction vectors, in global space. 
	CalculateGlobalMatrix (m_localMatrix0, m_localMatrix1, matrix0, matrix1);

	// get the pin fixed to the first body
	const dVector& dir0 = matrix0.m_front;
	// get the pin fixed to the secund body
	const dVector& dir1 = matrix1.m_up;

	// contruct an horthgonal coordentate system with these two vectors
	dVector dir2 (dir0 * dir1);
	dVector dir3 (dir2 * dir0);
	dir3 = dir3.Scale (1.0f / sqrtf (dir3 % dir3));

	const dVector& p0 = matrix0.m_posit;
	const dVector& p1 = matrix1.m_posit;

	dVector q0 (p0 + dir3.Scale(MIN_JOINT_PIN_LENGTH));
	dVector q1 (p1 + dir1.Scale(MIN_JOINT_PIN_LENGTH));

	NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &dir0[0]);
	NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &dir1[0]);
	NewtonUserJointAddLinearRow (m_joint, &p0[0], &p1[0], &dir2[0]);
	NewtonUserJointAddLinearRow (m_joint, &q0[0], &q1[0], &dir0[0]);


	// if limit are enable ...
	if (m_limitsOn) {
		sinAngle = (matrix0.m_front * matrix1.m_front) % matrix1.m_up;
		cosAngle = matrix0.m_front % matrix1.m_front;
		angle = atan2f (sinAngle, cosAngle);

		if (angle < m_minAngle) {
			// get a point along the up vector and set a constraint  
			NewtonUserJointAddAngularRow (m_joint, 0.0f, &matrix1.m_up[0]);
			NewtonUserJointSetRowMaximunFriction (m_joint, 0.0f);
		} else if (angle > m_maxAngle) {
			NewtonUserJointAddAngularRow (m_joint, 0.0f, &matrix1.m_up[0]);
			NewtonUserJointSetRowMinimunFriction (m_joint, 0.0f);
		}
	}

	if (m_angularmotorOn) {
		dFloat relOmega;
		dFloat relAccel;
		dVector omega0 (0.0f, 0.0f, 0.0f);
		dVector omega1 (0.0f, 0.0f, 0.0f);

		//	sinAngle = (matrix1.m_up * matrix0.m_up) % matrix0.m_front;
		//	cosAngle = matrix0.m_up % matrix1.m_up;
		//	m_angle0 = atan2f (sinAngle, cosAngle);

		// get reletive angular velocity
		NewtonBodyGetOmega(m_body0, &omega0[0]);
		if (m_body1) {
			NewtonBodyGetOmega(m_body1, &omega1[0]);
		}

		// calculate the desired acceleration
		relOmega = (omega0 - omega1) % matrix0.m_front;
		relAccel = m_angularAccel - m_angularDamp * relOmega;

		// add and angular constraunt row to that will set the relative acceleration to zero
		NewtonUserJointAddAngularRow (m_joint, 0.0f, &matrix0.m_front[0]);
		
		// override the joint acceleration.
		NewtonUserJointSetRowAcceleration (m_joint, relAccel);
	}

 }
