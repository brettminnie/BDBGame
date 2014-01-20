//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

// LevelPrimitive.cpp: implementation of the LevelPrimitive class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "LevelPrimitive.h"
#include "OpenGlUtil.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LevelPrimitive::LevelPrimitive(const char* name, NewtonWorld* nWorld, NewtonTreeCollisionCallback levelCallback)
	:RenderPrimitive(GetIdentityMatrix())
{
	int i;
	int count;
	FILE * file;
	dVector point[32];
	NewtonCollision* collision;
	

	// create the collsion tree geometry
	collision = NewtonCreateTreeCollision(nWorld, levelCallback);

	// prepare to create collsion geometry
	NewtonTreeCollisionBeginBuild(collision);


	// open the level data
	char fullPathName[256];
	#ifdef _MSC_VER		
		sprintf (fullPathName, "%s", name);
	#else
		sprintf (fullPathName, "../../../%s", name);
	#endif	

	file = fopen (fullPathName, "rb");

	m_list = glGenLists(1);
	glNewList (m_list, GL_COMPILE);

	// load all fases
	for (fscanf (file, "%d", &count); count; fscanf (file, "%d", &count)) {
		// read the face
		for (i = 0; i < count; i ++) {
			fscanf (file, "%*[^(]s%c");
			fscanf (file, "%*c");
			float x, y, z;
			fscanf (file, "%f%f%f", &x, &y, &z);
			point[i].m_x = x;
			point[i].m_y = y;
			point[i].m_z = z;

			fscanf (file, "%*[^)]s");
			fscanf (file, "%*c");
		}

		// add this face to the collsion tree
		NewtonTreeCollisionAddFace(collision, count, &point[0].m_x, sizeof (dVector), 1);

		// add this face to the display list
		glBegin(GL_POLYGON);

		// calculate the normal
		dVector normal ((point[1] - point[0]) * (point[2] - point[0]));
		normal = normal.Scale (1.0f / sqrtf (normal % normal));
		glNormal3f(normal.m_x, normal.m_y, normal.m_z);

		// submit the face
		for (i = 0; i < count; i ++) {
			glVertex3f(point[i].m_x, point[i].m_y, point[i].m_z);
		}
		// end the list
		glEnd();
	}
	glEndList ();

	// close file
	fclose (file);

	// finalize the collition tree build
	NewtonTreeCollisionEndBuild(collision, 1);
//	NewtonTreeCollisionEndBuild(collision, 0);

	// create the level ridif body
	m_level = NewtonCreateBody(nWorld, collision);

	// release the collision tree (this way the application does not have to do book keeping of Newton objects
	NewtonReleaseCollision (nWorld, collision);

	// set the global position of this body
	NewtonBodySetMatrix (m_level, &m_matrix[0][0]); 

	
	dVector boxP0; 
	dVector boxP1; 
	// get the position of the aabb of this geometry
	NewtonCollisionCalculateAABB (collision, &m_matrix[0][0], &boxP0.m_x, &boxP1.m_x); 

	// add some extra padding the world size
	boxP0.m_x -= 10.0f;
	boxP0.m_y -= 10.0f;
	boxP0.m_z -= 10.0f;
	boxP1.m_x += 10.0f;
	boxP1.m_y += 10.0f;
	boxP1.m_z += 10.0f;

	// set the world size
	NewtonSetWorldSize (nWorld, &boxP0.m_x, &boxP1.m_x); 
}

LevelPrimitive::~LevelPrimitive()
{
	glDeleteLists(m_list, 1);

}

void LevelPrimitive::Render() const
{
	// set the color of the cube's surface
	GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
	
	glMultMatrix(&m_matrix[0][0]);

	glDisable(GL_TEXTURE_2D);
	glCallList (m_list);
}
	
