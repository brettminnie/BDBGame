//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

// NewtonTank.h: interface for the NewtonTank class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWTON_TANK_H__DD5FB692_E86A_43F6_ABCA_6F889285D7FC__INCLUDED_)
#define AFX_NEWTON_TANK_H__DD5FB692_E86A_43F6_ABCA_6F889285D7FC__INCLUDED_

#include <stdafx.h>
#include "OpenGlUtil.h"
#include "graphicSystem.h"
#include "NewtonVehicle.h"


class NewtonTankTire;

class NewtonTank: public NewtonVehicle
{
	public:
	NewtonTank(NewtonWorld* nWorld, const dMatrix& matrix, NewtonApplyForceAndTorque externaforce);
	virtual ~NewtonTank();


	virtual void Render() const;
	void SetSteering(dFloat value);
	void SetTireTorque(dFloat torque);
	void SetApplyHandBrakes (dFloat value);


	private:
	static void  DestroyVehicle (const NewtonBody* body);
	static void  SetTransform (const NewtonBody* body, const dFloat* matrixPtr);
	static void  TireUpdate (const NewtonJoint* vehicle);

	static void GetShapeFromCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertex, int id);
	void GetShapeFromCollision (NewtonWorld* nWorld, NewtonCollision* collision);

	GLuint m_list;
	dVector m_boxSize;
	dVector m_boxOrigin;
	
	NewtonTankTire* m_tires[12];
 		
	dFloat m_breakValue;
	dFloat m_steerTorque;
	dFloat m_fowardTorque;

	friend class NewtonTankTire;
};


#endif 
