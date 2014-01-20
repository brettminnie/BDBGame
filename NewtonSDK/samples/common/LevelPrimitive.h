//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

// LevelPrimitive.h: interface for the LevelPrimitive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEVELPRIMITIVE_H__211387E2_8B23_49AB_9F18_1746C49EBFD0__INCLUDED_)
#define AFX_LEVELPRIMITIVE_H__211387E2_8B23_49AB_9F18_1746C49EBFD0__INCLUDED_

#include "Newton.h"
#include "RenderPrimitive.h"



class LevelPrimitive : public RenderPrimitive  
{
	public:
	LevelPrimitive(const char* name, NewtonWorld* nWorld, NewtonTreeCollisionCallback callback);
	virtual ~LevelPrimitive();
	void Render() const;

	GLuint m_list;
	NewtonBody* m_level;
};

#endif 
