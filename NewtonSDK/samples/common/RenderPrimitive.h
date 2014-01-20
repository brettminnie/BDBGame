//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

// RenderPrimitive.h: interface for the RenderPrimitive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERPRIMITIVE_H__0C9BDF8F_FF8D_4F5A_9692_4E76E3C8878A__INCLUDED_)
#define AFX_RENDERPRIMITIVE_H__0C9BDF8F_FF8D_4F5A_9692_4E76E3C8878A__INCLUDED_

#include <stdafx.h>
#include "dSceneNode.h"

struct NewtonWorld;



class RenderPrimitive: public dSceneNode
{
	public:
	RenderPrimitive(const dMatrix& matrix, int texture = -1);

	virtual ~RenderPrimitive();
	virtual void Render()  const = 0;

//	virtual void SetMatrix (const dMatrix& matrix);
//	dMatrix GetMatrix () const;

	void SetTexture (int ID);

	public:
//	dMatrix m_matrix;
	void *m_node;
	unsigned m_texture;


	dFloat m_modifierScaleAngleX;
	dFloat m_modifierScaleAngleY;
	dFloat m_modifierScaleAngleZ;

	dFloat m_modifierSkewAngleY;
};


class BoxPrimitive: public RenderPrimitive
{
	public:
	BoxPrimitive(const dMatrix& matrix, const dVector& size, int texture = -1);
	void Render() const;
	
	dVector m_size;
};

class SpherePrimitive: public RenderPrimitive
{
	public:
	SpherePrimitive(const dMatrix& matrix, dFloat radiusX, dFloat radiusY, dFloat radiusZ, int texture = -1);
	void Render() const;
	
	dFloat m_radiusX;
	dFloat m_radiusY;
	dFloat m_radiusZ;
};


class CylinderPrimitive: public RenderPrimitive
{
	public:
	CylinderPrimitive(const dMatrix& matrix, dFloat radius, dFloat height, int texture = -1);
	void Render() const;
	
	dFloat m_radius;
	dFloat m_height;
};

class ConePrimitive: public RenderPrimitive
{
	public:
	ConePrimitive(const dMatrix& matrix, dFloat radius, dFloat height, int texture = -1);
	void Render() const;
	
	dFloat m_radius;
	dFloat m_height;
};


class CapsulePrimitive: public RenderPrimitive
{
	public:
	CapsulePrimitive(const dMatrix& matrix, dFloat radius, dFloat height, int texture = -1);
	void Render() const;
	
	dFloat m_radius;
	dFloat m_height;
};


class ChamferCylinderPrimitive: public RenderPrimitive
{
	public:
	ChamferCylinderPrimitive(const dMatrix& matrix, dFloat radius, dFloat height, int texture = -1);
	void Render() const;
	GLuint m_list;
};


class ConvexHullPrimitive: public RenderPrimitive
{
	public:
	ConvexHullPrimitive(const dMatrix& matrix, NewtonWorld* nWorld, NewtonCollision* collision, int texture = -1);
	void Render() const;

	static void GetShapeFromCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertex, int id);
	
	GLuint m_list;
};


#endif 
