//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

// NewtonVehicle.cpp: implementation of the NewtonVehicle class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "NewtonVehicle.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NewtonVehicle::NewtonVehicle(const dMatrix& matrix)
  :RenderPrimitive (matrix)
{
}

NewtonVehicle::~NewtonVehicle()
{
}

NewtonBody* NewtonVehicle::GetRigidBody() const
{
	return m_vehicleBody;
}
