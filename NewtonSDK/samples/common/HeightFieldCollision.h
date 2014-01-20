//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple 4d vector class
//********************************************************************

// HeightFieldCollision.h: interface for the HeightFieldCollision class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEIGHTFIELDCOLLISION_H__6518E22E_3B44_441D_BB92_9732FF065C7B__INCLUDED_)
#define AFX_HEIGHTFIELDCOLLISION_H__6518E22E_3B44_441D_BB92_9732FF065C7B__INCLUDED_


#include "Newton.h"
#include "RenderPrimitive.h"

#define HEIGHT_SIZE			(256 + 1)

#define CELL_SIZE			4.0f
#define HIGHTSCALE_SIZE		1.0f
#define TEXTURE_SCALE		(1.0f / 32.0f)

#define MAX_COLLIDING_FACES	128

class HeightFieldCollision : public RenderPrimitive   
{
	public:
	HeightFieldCollision(NewtonWorld* nWorld, NewtonBodyDestructor destructor);
	virtual ~HeightFieldCollision();

	NewtonBody* GetRigidBody() const;

	void Render() const;

	private:

	dFloat RayCastTriangle (const dVector& p0, const dVector& dp, const dVector& origin, const dVector& e1, const dVector& e2);
	dFloat RayCastCell (int xIndex0, int zIndex0, const dVector& p0, const dVector& dp, dVector& normalOut);
	void CalculateMinExtend2d (const dVector& p0, const dVector& p1, dVector& boxP0, dVector& boxP1);
	void CalculateMinExtend3d (const dVector& p0, const dVector& p1, dVector& boxP0, dVector& boxP1);
	bool ClipRay2d (dVector& p0, dVector& p1, const dVector& boxP0, const dVector& boxP1); 




	GLuint m_list;
	NewtonBody* m_level;
	dVector m_minBox;
	dVector m_maxBox;
	dFloat m_heightField[HEIGHT_SIZE][HEIGHT_SIZE];

//	int m_lastFaceCount;
	int m_attribute[MAX_COLLIDING_FACES];
	int m_faceIndices[MAX_COLLIDING_FACES];
	int m_indexArray[MAX_COLLIDING_FACES * 3];
	dVector m_collisionVertex[MAX_COLLIDING_FACES * 2];

	static void MeshCollisionCollideCallback (NewtonUserMeshCollisionCollideDesc* collideDescData);
	static dFloat UserMeshCollisionRayHitCallback (NewtonUserMeshCollisionRayHitDesc* lineDescData);
};

#endif 
