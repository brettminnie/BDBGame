//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

// RenderPrimitive.cpp: implementation of the RenderPrimitive class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "OpenGlUtil.h"
#include "graphicSystem.h"
#include "RenderPrimitive.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


RenderPrimitive::RenderPrimitive(const dMatrix& matrix, int texture)
	:dSceneNode()
{
	SetMatrix (matrix);
	m_texture = (texture == -1) ? g_cubeTexture: texture;
	GraphicManager::GetManager().AddObject (this);

	m_modifierSkewAngleY = dFloat (dRand () % 360) / 6.2832f;
	m_modifierScaleAngleX = dFloat (dRand () % 360) / 6.2832f;
	m_modifierScaleAngleY = dFloat (dRand () % 360) / 6.2832f;
	m_modifierScaleAngleZ = dFloat (dRand () % 360) / 6.2832f;
}

RenderPrimitive::~RenderPrimitive()
{
	GraphicManager::GetManager().RemoveObject (this);
}

void RenderPrimitive::SetTexture (int ID)
{
	 m_texture = (ID == -1) ? g_cubeTexture: ID;
}



BoxPrimitive::BoxPrimitive(const dMatrix& matrix, const dVector& size, int texture)
	:RenderPrimitive (matrix, texture), m_size (size.Scale(0.5f))
{
}



//  Draws the rotating cube
void BoxPrimitive::Render()  const
{

	// set the color of the cube's surface
	GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
	
	glMultMatrix(&m_matrix[0][0]);
	
	// set up the cube's texture
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	
	// the cube will just be drawn as six quads for the sake of simplicity
	// for each face, we specify the quad's normal (for lighting), then
	// specify the quad's 4 vertices and associated texture coordinates
	glBegin(GL_QUADS);
	// front
	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-m_size.m_x, -m_size.m_y, m_size.m_z);
	glTexCoord2f(1.0, 0.0); glVertex3f( m_size.m_x, -m_size.m_y, m_size.m_z);
	glTexCoord2f(1.0, 1.0); glVertex3f( m_size.m_x,  m_size.m_y, m_size.m_z);
	glTexCoord2f(0.0, 1.0); glVertex3f(-m_size.m_x,  m_size.m_y, m_size.m_z);
	
	// back
	glNormal3f(0.0, 0.0, -1.0);
	glTexCoord2f(0.0, 0.0); glVertex3f( m_size.m_x, -m_size.m_y, -m_size.m_z);
	glTexCoord2f(1.0, 0.0); glVertex3f(-m_size.m_x, -m_size.m_y, -m_size.m_z);
	glTexCoord2f(1.0, 1.0); glVertex3f(-m_size.m_x,  m_size.m_y, -m_size.m_z);
	glTexCoord2f(0.0, 1.0); glVertex3f( m_size.m_x,  m_size.m_y, -m_size.m_z);
	
	// top
	glNormal3f(0.0, 1.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-m_size.m_x,  m_size.m_y,  m_size.m_z);
	glTexCoord2f(1.0, 0.0); glVertex3f( m_size.m_x,  m_size.m_y,  m_size.m_z);
	glTexCoord2f(1.0, 1.0); glVertex3f( m_size.m_x,  m_size.m_y, -m_size.m_z);
	glTexCoord2f(0.0, 1.0); glVertex3f(-m_size.m_x,  m_size.m_y, -m_size.m_z);
	
	// bottom
	glNormal3f(0.0, -1.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-m_size.m_x, -m_size.m_y, -m_size.m_z);
	glTexCoord2f(1.0, 0.0); glVertex3f( m_size.m_x, -m_size.m_y, -m_size.m_z);
	glTexCoord2f(1.0, 1.0); glVertex3f( m_size.m_x, -m_size.m_y,  m_size.m_z);
	glTexCoord2f(0.0, 1.0); glVertex3f(-m_size.m_x, -m_size.m_y,  m_size.m_z);
	
	// left
	glNormal3f(-1.0, 0.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(-m_size.m_x, -m_size.m_y, -m_size.m_z);
	glTexCoord2f(1.0, 0.0); glVertex3f(-m_size.m_x, -m_size.m_y,  m_size.m_z);
	glTexCoord2f(1.0, 1.0); glVertex3f(-m_size.m_x,  m_size.m_y,  m_size.m_z);
	glTexCoord2f(0.0, 1.0); glVertex3f(-m_size.m_x,  m_size.m_y, -m_size.m_z);
	
	// right
	glNormal3f(1.0, 0.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(m_size.m_x,  -m_size.m_y,  m_size.m_z);
	glTexCoord2f(1.0, 0.0); glVertex3f(m_size.m_x,  -m_size.m_y, -m_size.m_z);
	glTexCoord2f(1.0, 1.0); glVertex3f(m_size.m_x,   m_size.m_y, -m_size.m_z);
	glTexCoord2f(0.0, 1.0); glVertex3f(m_size.m_x,   m_size.m_y,  m_size.m_z);
	
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
} 



SpherePrimitive::SpherePrimitive(const dMatrix& matrix, dFloat radiusX, dFloat radiusY, dFloat radiusZ, int texture)
	:RenderPrimitive (matrix, texture) 
{
	m_radiusX = radiusX;
	m_radiusY = radiusY;
	m_radiusZ = radiusZ;
}

//  Draws the rotating sphere
void SpherePrimitive::Render()  const
{
	GLUquadricObj *pObj;

	// set the color of the cube's surface
	GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);

	dMatrix mat (m_matrix);
	mat.m_front = mat.m_front.Scale (m_radiusX);
	mat.m_up    = mat.m_up.Scale (m_radiusY);
	mat.m_right = mat.m_right.Scale (m_radiusZ);
	glMultMatrix(&mat[0][0]);
	
	// set up the cube's texture
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, g_ballTexture);
	
	// Get a new Quadric off the stack
	pObj = gluNewQuadric();				
	// Get a new Quadric off the stack
	gluQuadricTexture(pObj, true);						

	gluSphere(pObj, 1.0f, 20, 20);					

	gluDeleteQuadric(pObj);	
} 


CylinderPrimitive::CylinderPrimitive(const dMatrix& matrix, dFloat radius, dFloat height, int texture)
	:RenderPrimitive (matrix, texture) 
{
	m_radius = radius;
	m_height = height;
}



//  Draws the rotating sphere
void CylinderPrimitive::Render()  const
{
	GLUquadricObj *pObj;

	// set the color of the cube's surface
	GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);


	// opengl cylinder are aligned alogn the z axis, we want it along the x axis, we craete a rotation matrix
	// to do the alignmen
	dMatrix matrix (dgYawMatrix (3.1416f * 0.5f));
	matrix.m_posit = matrix.RotateVector (dVector (0.0f, 0.0f, -m_height * 0.5f));
	matrix = matrix * m_matrix;

	glMultMatrix(&matrix[0][0]);
//	glPushMatrix();
	
	// set up the cube's texture
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	
	// Get a new Quadric off the stack
	pObj = gluNewQuadric();				
	gluQuadricTexture(pObj, true);						
	gluCylinder(pObj, m_radius, m_radius, m_height, 20, 2);

	// render the caps
	gluQuadricOrientation(pObj, GLU_INSIDE);
	gluDisk(pObj, 0.0f, m_radius, 20, 1);

 	glTranslatef (0.0f, 0.0f, m_height);
 	gluQuadricOrientation(pObj, GLU_OUTSIDE);
	gluDisk(pObj, 0.0f, m_radius, 20, 1);


//	glPopMatrix();
	gluDeleteQuadric(pObj);	
} 





CapsulePrimitive::CapsulePrimitive(const dMatrix& matrix, dFloat radius, dFloat height, int texture)
	:RenderPrimitive (matrix, texture) 
{
	m_radius = radius;
	m_height = height - radius * 2.0f;
}



//  Draws the rotating sphere
void CapsulePrimitive::Render()  const
{
	GLUquadricObj *pObj;

	// set the color of the cube's surface
	GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);


	// opengl cylinder are aligned alogn the z axis, we want it along the x axis, we craete a rotation matrix
	// to do the alignmen
	dMatrix matrix (dgYawMatrix (3.1416f * 0.5f));
	matrix.m_posit = matrix.RotateVector (dVector (0.0f, 0.0f, -m_height * 0.5f));
	matrix = matrix * m_matrix;

	glMultMatrix(&matrix[0][0]);
//	glPushMatrix();
	
	// set up the cube's texture
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	
	// Get a new Quadric off the stack
	pObj = gluNewQuadric();				
	gluQuadricTexture(pObj, true);						
	gluCylinder(pObj, m_radius, m_radius, m_height, 20, 2);

	// render the caps
//	gluQuadricOrientation(pObj, GLU_INSIDE);
//	gluDisk(pObj, 0.0f, m_radius, 20, 1);
	gluSphere(pObj, m_radius, 20, 20);

 	glTranslatef (0.0f, 0.0f, m_height);
 //	gluQuadricOrientation(pObj, GLU_OUTSIDE);
//	gluDisk(pObj, 0.0f, m_radius, 20, 1);
	gluSphere(pObj, m_radius, 20, 20);


//	glPopMatrix();
	gluDeleteQuadric(pObj);	
} 


ConePrimitive::ConePrimitive(const dMatrix& matrix, dFloat radius, dFloat height, int texture)
	:RenderPrimitive (matrix, texture) 
{
	m_radius = radius;
	m_height = height;
}



//  Draws the rotating sphere
void ConePrimitive::Render()  const
{
	GLUquadricObj *pObj;

	// set the color of the cube's surface
	GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);


	// opengl cylinder are aligned alogn the z axis, we want it along the x axis, we craete a rotation matrix
	// to do the alignmen
	dMatrix matrix (dgYawMatrix (3.1416f * 0.5f));
	matrix.m_posit = matrix.RotateVector (dVector (0.0f, 0.0f, -m_height * 0.5f));
	matrix = matrix * m_matrix;

	glMultMatrix(&matrix[0][0]);
//	glPushMatrix();
	
	// set up the cube's texture
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	
	// Get a new Quadric off the stack
	pObj = gluNewQuadric();				
	gluQuadricTexture(pObj, true);						
	gluCylinder(pObj, m_radius, 0.0f, m_height, 20, 2);

	// render the caps
	gluQuadricOrientation(pObj, GLU_INSIDE);
	gluDisk(pObj, 0.0f, m_radius, 20, 1);
  
//	glPopMatrix();
	gluDeleteQuadric(pObj);	
} 

ChamferCylinderPrimitive::ChamferCylinderPrimitive(const dMatrix& matrix, dFloat radius, dFloat height, int texture)
	:RenderPrimitive (matrix, texture) 
{
	int i;
	int j;
	int slices = 16;
	int breaks = 16;

	dFloat sliceStep;
	dFloat sliceAngle;

	dFloat breakStep;
//	dFloat breakAngle;
	dFloat textuteScale;


	radius = radius - height * 0.5f;
	if (radius < 0.1f * height) {
		radius = 0.1f * height;
	}

	
	sliceStep = 2.0f * 3.1416f / slices; 
	sliceAngle = 0.0f;

	m_list = glGenLists(1);
	glNewList (m_list, GL_COMPILE);


	breakStep = 2.0f * 3.1416f / breaks;
	dMatrix rot (dgPitchMatrix (breakStep));	

	height *= 0.5f;

	textuteScale = 1.0f;

	for (j = 0; j < slices; j ++) {
		dVector p0 (height * cosf(sliceAngle),             0.0f, radius + height * sinf(sliceAngle));
		dVector p1 (height * cosf(sliceAngle + sliceStep), 0.0f, radius + height * sinf(sliceAngle + sliceStep));

		dVector p0uv (cosf(sliceAngle) * 0.5f, 0.0f, sinf(sliceAngle) * 0.5f);
		dVector p1uv (cosf(sliceAngle + sliceStep) * 0.5f, 0.0f, sinf(sliceAngle + sliceStep) * 0.5f);

		for (i = 0; i < breaks; i ++) {
			dVector q0 (rot.RotateVector (p0));
			dVector q1 (rot.RotateVector (p1));

			dVector q0uv (rot.RotateVector (p0uv));
			dVector q1uv (rot.RotateVector (p1uv));


			glBegin(GL_POLYGON);

			dVector normal ((p0 - q0) * (q1 - q0));
			normal = normal.Scale (1.0f / sqrtf (normal % normal));
			glNormal3f(normal.m_x, normal.m_y, normal.m_z);

			glVertex3f(p0.m_x, p0.m_y, p0.m_z);
			glTexCoord2f(dFloat (i + 0) / breaks, dFloat (j + 0) / slices);

			glVertex3f(p1.m_x, p1.m_y, p1.m_z);
			glTexCoord2f(dFloat (i + 1) / breaks, dFloat (j + 0) / slices);
			

			glVertex3f(q1.m_x, q1.m_y, q1.m_z);
			glTexCoord2f(dFloat (i + 1) / breaks, dFloat (j + 1) / slices);

			glVertex3f(q0.m_x, q0.m_y, q0.m_z);
			glTexCoord2f(dFloat (i + 0) / breaks, dFloat (j + 1) / slices);

			glEnd();

			p0 = q0;
			p1 = q1;

			p0uv = q0uv;
			p1uv = q1uv;
		}

		sliceAngle += sliceStep;
	}

	glBegin(GL_POLYGON);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	dVector p0 (-height, 0.0f, radius);
	dVector uv0 (0.0, 0.5f, 0.0f);
	for (i = 0; i < breaks; i ++) {
		glVertex3f(p0.m_x, p0.m_y, p0.m_z);
		p0 = rot.UnrotateVector (p0);

		glTexCoord2f(uv0.m_y + 0.5f, uv0.m_z + 0.5f);
		uv0 = rot.RotateVector (uv0);
	}
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3f(1.0f, 0.0f, 0.0f);
	dVector p1 (height, 0.0f, radius);
	dVector uv1 (0.0, 0.5f, 0.0f);
	for (i = 0; i < breaks; i ++) {
		glVertex3f(p1.m_x, p1.m_y, p1.m_z);
		p1 = rot.RotateVector (p1);

		glTexCoord2f(uv1.m_y + 0.5f, uv1.m_z + 0.5f);
		uv1 = rot.RotateVector (uv1);
	}
	glEnd();

	glEndList ();

}



//  Draws the rotating sphere
void ChamferCylinderPrimitive::Render()  const
{
	// set the color of the cube's surface
	GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	
	glMultMatrix(&m_matrix[0][0]);

	glCallList (m_list);
} 


ConvexHullPrimitive::ConvexHullPrimitive(const dMatrix& matrix, NewtonWorld* nWorld, NewtonCollision* collision, int texture)
	:RenderPrimitive (matrix, texture) 
{
	// for this geometry we will create a dommy ridig body and use the debug collision to extract 
	// the collision shape of the geometry, then whe will destory the rigid body

	NewtonBody *body;

	m_list = glGenLists(1);
	glNewList (m_list, GL_COMPILE);

	body = NewtonCreateBody (nWorld, collision);
	NewtonBodySetUserData (body, this);

	NewtonBodyForEachPolygonDo (body, GetShapeFromCollision);

	glEndList ();

	NewtonDestroyBody(nWorld, body);
}


void ConvexHullPrimitive::GetShapeFromCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertex, int id)
{
	int i;
	ConvexHullPrimitive* primitive;

	primitive = (ConvexHullPrimitive*) NewtonBodyGetUserData (body);

	glBegin(GL_POLYGON);

	// calculate the face normal for this polygon
	dVector normal (0.0f, 0.0f, 0.0f);
	dVector p0 (faceVertex[0 * 3 + 0], faceVertex[0 * 3 + 1], faceVertex[0 * 3 + 2]);
	dVector p1 (faceVertex[1 * 3 + 0], faceVertex[1 * 3 + 1], faceVertex[1 * 3 + 2]);
	for (i = 2; i < vertexCount; i ++) {
		dVector p2 (faceVertex[i * 3 + 0], faceVertex[i * 3 + 1], faceVertex[i * 3 + 2]);
		dVector dp0 (p1 - p0);
		dVector dp1 (p2 - p0);
		normal += dp0 * dp1;
	}

	normal = normal.Scale (sqrtf (1.0f / (normal % normal)));

	// submit the polygon to open gl
	glNormal3f(normal.m_x, normal.m_y, normal.m_z);
	for (i = 0; i < vertexCount; i ++) {
		dVector p0 (faceVertex[i * 3 + 0], faceVertex[i * 3 + 1], faceVertex[i * 3 + 2]);
		glVertex3f(p0.m_x, p0.m_y, p0.m_z);

		// calculate pseudo spherical mapping
		p0 = p0.Scale (1.0f / sqrt (p0 % p0));
		glTexCoord2f(asinf (p0.m_x) / 3.1416f + 0.5f, asinf (p0.m_z) / 3.1416f + 0.5f);
	}
	glEnd();
}



void ConvexHullPrimitive::Render() const
{
	// set the color of the cube's surface
	GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	
	glMultMatrix(&m_matrix[0][0]);

	glCallList (m_list);
}
	



