//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************


// CustomConeLimitedBallAndSocket.cpp: implementation of the CustomConeLimitedBallAndSocket class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "CustomConeLimitedBallAndSocket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#define MIN_JOINT_PIN_LENGTH	50.0f

CustomConeLimitedBallAndSocket::CustomConeLimitedBallAndSocket(
   dFloat twistAngle, 
   dFloat coneAngle, 
   const dVector& coneDir, 
   const dVector& pivot, 
   NewtonBody* child, 
   NewtonBody* parent)
	:CustomBallAndSocket(pivot, child, parent)
{
	m_coneAngle = coneAngle; 
	m_coneAngle = twistAngle;

	m_cosConeAngle = cosf (m_coneAngle);

	// Recalculate local matrices so that the front vector align with the cone pin
	CalculateLocalMatrix (pivot, coneDir, m_localMatrix0, m_localMatrix1);
}

CustomConeLimitedBallAndSocket::~CustomConeLimitedBallAndSocket()
{

}


void CustomConeLimitedBallAndSocket::SubmitConstrainst ()
{
	dFloat coneCos;
	dFloat angleError;
	dMatrix matrix0;
	dMatrix matrix1;

	// add the tree rows to keep the pivot in place
	// calculate the position of the pivot point and the jacobians direction vectors, in global space. 
	CalculateGlobalMatrix (m_localMatrix0, m_localMatrix1, matrix0, matrix1);

	// Restrict the movemnet on the poivot point along all tree orthonormal direction
	NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_front[0]);
	NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_up[0]);
	NewtonUserJointAddLinearRow (m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix0.m_right[0]);

	// ///////////////////////////////////////////////////////////////////
	//
	// add a row to keep the child body inside the cone limit
	//
	// The child is inside the cone if the cosin of teh angle beween the pin and 
	coneCos = matrix0.m_front % matrix1.m_front;
	if (coneCos < m_cosConeAngle) {

		// the child body has violated the cone limit we need to stop it from keep moving 
		// for that we are going to pick a point along the the child body front vector
 		dVector p0 (matrix0.m_posit + matrix0.m_front.Scale(MIN_JOINT_PIN_LENGTH));

		// get a vectors perpendicular to the plane of motiom
		dVector lateralDir (matrix0.m_front * matrix1.m_front);
		dVector unitLateralDir = lateralDir.Scale (1.0f / sqrtf (lateralDir % lateralDir));

		// now we will add a constraint row along the lateral direction, 
		// this will add stability as it will preven the child bidy from moving sideways
		NewtonUserJointAddLinearRow (m_joint, &p0[0], &p0[0], &unitLateralDir[0]);

		// now we need to calculate the exceded angle error (the cross product = sin (angle)
		angleError = asinf (unitLateralDir % lateralDir) - m_coneAngle;

		// calculate the unit vector tanget to the trayectory
		dVector tangentDir (unitLateralDir * matrix0.m_front);
		NewtonUserJointAddLinearRow (m_joint, &p0[0], &p0[0], &tangentDir[0]);

		//we nee to allow the body to mo in opositet deiretion to the penemtration
		// that can by done by settin the min friction to zero
		NewtonUserJointSetRowMinimunFriction (m_joint, 0.0f);
	}
}
