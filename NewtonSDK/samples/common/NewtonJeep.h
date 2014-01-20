//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

// NewtonJeep.h: interface for the NewtonJeep class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWTON_JEEP_H__DD5FB692_E86A_43F6_ABCA_6F889285D7FC__INCLUDED_)
#define AFX_NEWTON_JEEP_H__DD5FB692_E86A_43F6_ABCA_6F889285D7FC__INCLUDED_

#include <stdafx.h>
#include "OpenGlUtil.h"
#include "graphicSystem.h"
#include "NewtonVehicle.h"

class NewtonJeepTire;

class NewtonJeep: public NewtonVehicle
{
	public:
	NewtonJeep(NewtonWorld* nWorld, const dMatrix& matrix, NewtonApplyForceAndTorque externaforce);
	virtual ~NewtonJeep();
	
	virtual void Render() const;
	void SetSteering(dFloat value);
	void SetTireTorque(dFloat torque);
	void SetApplyHandBrakes (dFloat torque);

	private:
	static void  DestroyVehicle (const NewtonBody* body);
	static void  SetTransform (const NewtonBody* body, const dFloat* matrixPtr);

	static void  TireUpdate (const NewtonJoint* vehicle);
	void DoPhysicsAssembly(NewtonWorld* nWorld, NewtonApplyForceAndTorque externaforce);

	dVector m_boxSize;
	dVector m_boxOrigin;
	NewtonJeepTire* m_flTire;
	NewtonJeepTire* m_frTire;
	NewtonJeepTire* m_rlTire;
	NewtonJeepTire* m_rrTire;
	GLuint m_list;

	friend class NewtonJeepTire;

};

#endif 
