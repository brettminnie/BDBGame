//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple 4d vector class
//********************************************************************

#include <stdafx.h>
#include "tutorial.h"
#include "HiResTimer.h"
#include "RenderPrimitive.h"


CHiResTimer timer;
BoxPrimitive box (GetIdentityMatrix(), dVector (1.0f, 1.0f, 1.0f), unsigned (-1));


#define CAMERA_SPEED 1.0f

static NewtonWorld* nWorld;
static NewtonBody* ribidBodyBox; 

static dVector cameraDir (0.0f, 0.0f, -1.0f);
static dVector cameraEyepoint (0.0f, 0.0f, 4.0f);


static void CleanUp ();
static void Keyboard();


//********************************************************************
// 
//	Newton Tutorial 1 Hello world
//
//********************************************************************
int main(int argc, char **argv)
{
	// initilize opengl	
	InitOpenGl (argc, argv, "Newton Tutotial 1: Getting Started", DrawScene);

	// create the physics scene
	InitScene();
	
	// Execute opengl main loop for ever
	glutMainLoop();   

	return 0;
} 

// destroy the world and every thing in it
void CleanUp ()
{
	// destroy the Newton world
	NewtonDestroy (nWorld);
}


// create physics scene
void InitScene()
{
	NewtonCollision* collision;


	// make sure the box has a texture, use the default texture
	box.SetMatrix(GetIdentityMatrix());
	box.SetTexture (unsigned(-1));

	// create the newton world
	nWorld = NewtonCreate (NULL, NULL);


	// Set the termination function
	atexit(CleanUp); 

	// create the collsion shape
	collision = NewtonCreateBox (nWorld, box.m_size.m_x * 2.0f, box.m_size.m_y * 2.0f, box.m_size.m_z * 2.0f, NULL); 


	// create the ridid body
	ribidBodyBox = NewtonCreateBody (nWorld, collision);


	// Get Rid of the collision
	NewtonReleaseCollision (nWorld, collision);

	// set the body mass and inertia
	NewtonBodySetMassMatrix (ribidBodyBox, 1.0f, 1.0f, 1.0f, 1.0f);

	// set the trasformation matrix
	dMatrix matrix (box.GetMatrix());
	matrix.m_posit.m_x = 0.0f;
	matrix.m_posit.m_y = 1.0f;
	matrix.m_posit.m_z = 0.0f;
	NewtonBodySetMatrix (ribidBodyBox, &matrix[0][0]);

	// animate the body by setting the angular veocity
	dVector omega (5.0f, 5.0f, 5.0f);
	NewtonBodySetOmega (ribidBodyBox, &omega[0]); 
}

// DrawScene()
void DrawScene ()
{
	dFloat timeStep;


	// get the time step
	timeStep = timer.GetElapsedSeconds();

	// updtate the Newton physics world
	NewtonUpdate (nWorld, timeStep);

	// read keboard controls;
	Keyboard();

	// move the camera
	dVector target (cameraEyepoint + cameraDir);
	SetCamera (cameraEyepoint, target);

	// get the matrix from the rigid body
	dMatrix matrix;
	NewtonBodyGetMatrix(ribidBodyBox, &matrix[0][0]);

	// set the matrix of the visual body
	box.SetMatrix (matrix);

	// render the object
	glPushMatrix();
	box.Render();
	glPopMatrix();
} 



//	Keyboard handler. 
void Keyboard()
{
	// check for termination
	if (dGetKeyState (VK_ESCAPE) & 0x8000) {
		exit(0);
	}

	// read the mouse position and set the camera direction
	static MOUSE_POINT mouse0;
	static dFloat yawAngle = 90.0f * 3.1416 / 180.0f;
	static dFloat rollAngle = 0.0f;

	MOUSE_POINT mouse1;
	GetCursorPos(mouse1);

	if (dGetKeyState (VK_LBUTTON) & 0x8000) {
		if (mouse1.x > (mouse0.x + 1)) {
			yawAngle += 1.0f * 3.1416 / 180.0f;
			if (yawAngle > (360.0f * 3.1416 / 180.0f)) {
				yawAngle -= (360.0f * 3.1416 / 180.0f);
			}
		} else if (mouse1.x < (mouse0.x - 1)) {
			yawAngle -= 1.0f * 3.1416 / 180.0f;
			if (yawAngle < 0.0f) {
				yawAngle += (360.0f * 3.1416 / 180.0f);
			}
		}

		if (mouse1.y > (mouse0.y + 1)) {
			rollAngle += 1.0f * 3.1416 / 180.0f;
			if (rollAngle > (80.0f * 3.1416 / 180.0f)) {
				rollAngle = 80.0f * 3.1416 / 180.0f;
			}
		} else if (mouse1.y < (mouse0.y - 1)) {
			rollAngle -= 1.0f * 3.1416 / 180.0f;
			if (rollAngle < -(80.0f * 3.1416 / 180.0f)) {
				rollAngle = -80.0f * 3.1416 / 180.0f;
			}
		}
		dMatrix cameraDirMat (dgRollMatrix(rollAngle) * dgYawMatrix(yawAngle));
		cameraDir = cameraDirMat.m_front;
	}

	mouse0 = mouse1;


	// camera control
	if (dGetKeyState ('W') & 0x8000) {
		cameraEyepoint += cameraDir.Scale (CAMERA_SPEED / 60.0f);
	} else if (dGetKeyState ('S') & 0x8000) {
		cameraEyepoint -= cameraDir.Scale (CAMERA_SPEED / 60.0f);
	}

	if (dGetKeyState ('D') & 0x8000) {
		dVector up (0.0f, 1.0f, 0.0f);
		dVector right (cameraDir * up);
		cameraEyepoint += right.Scale (CAMERA_SPEED / 60.0f);
	} else if (dGetKeyState ('A') & 0x8000) {
		dVector up (0.0f, 1.0f, 0.0f);
		dVector right (cameraDir * up);
		cameraEyepoint -= right.Scale (CAMERA_SPEED / 60.0f);
	}
} 



