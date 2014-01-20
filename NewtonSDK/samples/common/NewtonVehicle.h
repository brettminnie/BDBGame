//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

// NewtonVehicle.h: interface for the NewtonVehicle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWTONVEHICLE_H__DD5FB692_E86A_43F6_ABCA_6F889285D7FC__INCLUDED_)
#define AFX_NEWTONVEHICLE_H__DD5FB692_E86A_43F6_ABCA_6F889285D7FC__INCLUDED_

#include <stdafx.h>
#include "OpenGlUtil.h"
#include "graphicSystem.h"
#include "RenderPrimitive.h"

#define GRAVITY	   -10.0f
#define TIRE_COLLITION_ID	0x100
#define CHASIS_COLLITION_ID	0x200


class NewtonVehicle: public RenderPrimitive
{
	public:
	NewtonVehicle(const dMatrix& matrix);
	virtual ~NewtonVehicle();

	virtual void SetSteering(dFloat value) = 0;
	virtual void SetTireTorque(dFloat torque) = 0;
	virtual void SetApplyHandBrakes (dFloat value) = 0;

	NewtonBody* GetRigidBody() const;
//	virtual void Render() const;


	protected:
	NewtonBody* m_vehicleBody;
	NewtonJoint* m_vehicleJoint;
};


#endif 
