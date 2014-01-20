//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

// CustomUniversal.h: interface for the CustomUniversal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMUNIVERSAL_H__B631F556_468B_4331_B7D7_F85ECF3E9ADE__INCLUDED_)
#define AFX_CUSTOMUNIVERSAL_H__B631F556_468B_4331_B7D7_F85ECF3E9ADE__INCLUDED_

#include "NewtonCustomJoint.h"

class CustomUniversal: public NewtonCustomJoint  
{
	public:
	CustomUniversal(const dVector& pivot, const dVector& pin0, const dVector& pin1, NewtonBody* child, NewtonBody* parent = NULL);
	virtual ~CustomUniversal();

	void EnableLimits(bool state);
	void SetLimis(dFloat minAngle, dFloat maxAngle);

	protected:
	virtual void SubmitConstrainst ();
	dMatrix m_localMatrix0;
	dMatrix m_localMatrix1;

	bool m_limitsOn;
	dFloat m_minAngle;
	dFloat m_maxAngle;

	bool m_angularmotorOn;
	dFloat m_angularDamp;
	dFloat m_angularAccel;
	
};

#endif // !defined(AFX_CUSTOMUNIVERSAL_H__B631F556_468B_4331_B7D7_F85ECF3E9ADE__INCLUDED_)
