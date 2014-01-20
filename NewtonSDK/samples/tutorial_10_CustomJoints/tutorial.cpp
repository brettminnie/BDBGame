//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple demo list vector class with iterators
//********************************************************************

#include <stdafx.h>
#include "tutorial.h"
#include "HiResTimer.h"
#include "Materials.h"
#include "graphicSystem.h"
#include "JointsTutorial.h"
#include "LevelPrimitive.h"
#include "RenderPrimitive.h"
#include "CharaterControl.h"
#include "DryRollingFriction.h"



CHiResTimer timer;


static NewtonWorld* nWorld;


static CharacterController* player;

dVector cameraDir (0.0f, 0.0f, -1.0f);
dVector cameraEyepoint (0.0f, 5.0f, 0.0f);

//static bool forceMode;
//static dVector cameraTorqueVector (0.0f, 0.0f, 0.0f); 

bool hideObjects = false;
bool debugLinesMode = false;
bool animateUpVector = false;


static void  CleanUp ();
static void  Keyboard();
static void* PhysicsAlloc (int sizeInBytes);
static void  PhysicsFree (void *ptr, int sizeInBytes);




static void PopulateLevel ();
static void AddBallsWithRollingFriction ();


static int debugCount = 0;
static dVector debugFace[1024][2];

static void  DebugShowCollision ();
static void  DebugShowBodyCollision (const NewtonBody* body);
static void DebugShowGeometryCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertec, int id);

void  PhysicsApplyPickForce (const NewtonBody* body);
void  PhysicsApplyGravityForce (const NewtonBody* body);


static dFloat  RayCastFilter (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam);
//static dFloat  RayCastPlacement (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam);

// set this to 0 for more realistic game (the stack will bever go to sleep mode)
#define AUTO_SLEEP_MODE 1
static bool sleepMode = AUTO_SLEEP_MODE;

static dFloat pickedParam;
static dVector pickedForce;
static dVector attachmentPoint;
static NewtonBody* pickedBody;


//********************************************************************
// 
//	Newton Tutorial 2 Using Callbacks
//
//********************************************************************
int main(int argc, char **argv)
{
	// initilize opengl	
	InitOpenGl (argc, argv, "Newton Tutotial 5: Using Joints", DrawScene);

	// create the physics scene
	InitScene();
	
	// Execute opengl main loop for ever
	glutMainLoop();   

	return 0;
} 

// memory allocation for Newton
void* PhysicsAlloc (int sizeInBytes)
{
	return malloc (sizeInBytes);
}

// memory de-allocation for Newton
void PhysicsFree (void *ptr, int sizeInBytes)
{
	free (ptr);
}

// destroy the world and every thing in it
void CleanUp ()
{
	// clear all resurces use with materials
	CleanUpMaterials (nWorld);

	// destroy the Newton world
	NewtonDestroy (nWorld);
}

// rigid body destructor
void PhysicsBodyDestructor (const NewtonBody* body)
{
	RenderPrimitive* primitive;

	// get the graphic object form the rigid body
	primitive = (RenderPrimitive*) NewtonBodyGetUserData (body);

	// destroy the graphic object
	delete primitive;
}

// set the tranformation of a rigid body
void PhysicsApplyGravityForce (const NewtonBody* body)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;

	// to apply external force we need to get the object mass
	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	dVector force (0.0f, -mass * 9.8f, 0.0f);

	// apply the force
	NewtonBodySetForce (body, &force.m_x);
}

// apply a object picking force
void PhysicsApplyPickForce (const NewtonBody* body)
{
	
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dVector veloc;
	dVector omega;
	dMatrix matrix;

	// frict apply the standard grabiyu force
	PhysicsApplyGravityForce (body);


	// object picking is done by applying a penaly force that will tend to drive to zero the distance 
	// between a pont in the body and a point on the line
	NewtonBodyGetVelocity(body, &veloc[0]);

	NewtonBodyGetOmega(body, &omega[0]);
	NewtonBodyGetVelocity(body, &veloc[0]);
	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);

	dVector force (pickedForce.Scale (mass * 100.0f));
	dVector dampForce (veloc.Scale (10.0f * mass));
	force -= dampForce;

	// calculate the force and torque in global space
	NewtonBodyGetMatrix(body, &matrix[0][0]);
	dVector point (matrix.RotateVector (attachmentPoint));
	dVector torque (point * force);

	NewtonBodyAddForce (body, &force.m_x);
	NewtonBodyAddTorque (body, &torque.m_x);
}


// add force and torque to rigid body
void PhysicsSetTransform (const NewtonBody* body, const dFloat* matrix)
{
	RenderPrimitive* primitive;

	// get the graphic object form the rigid body
	primitive = (RenderPrimitive*) NewtonBodyGetUserData (body);

	// set the transformation matrix for this rigid body
	dMatrix& mat = *((dMatrix*)matrix);
	primitive->SetMatrix (mat);
}




// create physics scene
void InitScene()
{
	LevelPrimitive *level;

	// /////////////////////////////////////////////////////////////////////
	//
	// create the newton world
	nWorld = NewtonCreate (PhysicsAlloc, PhysicsFree);

	// Set the termination function
	atexit(CleanUp); 

	// /////////////////////////////////////////////////////////////////////
	//
	// set up all material and  material interactions
	SetupMaterials (nWorld);

	// /////////////////////////////////////////////////////////////////////
	//
	// add some joint configurations
	AddRope (nWorld);
	AddRollingBeats (nWorld);
	AddDoubleSwingDoors (nWorld);

	// add some object to the backgorund
	PopulateLevel ();
	AddBallsWithRollingFriction ();

	// /////////////////////////////////////////////////////////////////////
	//
	// Load level goometry and add it to the display list 
	level = new LevelPrimitive ("level.dat", nWorld, NULL);

	// assign the comcrete id to the level geometry
	NewtonBodySetMaterialGroupID (level->m_level, levelID);


	// /////////////////////////////////////////////////////////////////////
	//
	// Create a charater controller
	dVector spawnSize (0.75f, 2.0f, 0.75f);
	dVector spawnPosit (0.0f, 10.0f, 0.0f);
	player = new CharacterController (nWorld, spawnPosit, spawnSize);

}


// show simole collision geometry in debug mode
void DebugShowGeometryCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertec, int id)
{
	int i;

	i = vertexCount - 1;
	dVector p0 (faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2]);
	for (i = 0; i < vertexCount; i ++) {
		dVector p1 (faceVertec[i * 3 + 0], faceVertec[i * 3 + 1], faceVertec[i * 3 + 2]);
		glVertex3f (p0.m_x, p0.m_y, p0.m_z);
		glVertex3f (p1.m_x, p1.m_y, p1.m_z);
 		p0 = p1;
	}
}

// show rigid body collision geometry
void  DebugShowBodyCollision (const NewtonBody* body)
{
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;

	// only show collision for dynamics objects
	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	if (mass > 0.0f) {
		NewtonBodyForEachPolygonDo (body, DebugShowGeometryCollision);
	}
}


// show all collision geometry is debug mode
void  DebugShowCollision ()
{
	int i;

	glDisable (GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 0.0f);
	NewtonWorldForEachBodyDo (nWorld, DebugShowBodyCollision);

	glColor3f(0.0f, 0.0f, 1.0f);
	for (i = 0; i < debugCount; i ++) {
		glVertex3f (debugFace[i][0].m_x, debugFace[i][0].m_y, debugFace[i][0].m_z);
		glVertex3f (debugFace[i][1].m_x, debugFace[i][1].m_y, debugFace[i][1].m_z);
	}
	glEnd();
}


// DrawScene()
void DrawScene ()
{
	dFloat timeStep;


	// get the time step
	timeStep = timer.GetElapsedSeconds();

	// updtate the Newton physics world
	NewtonUpdate (nWorld, timeStep);

	// read the keyboard
	Keyboard ();

	// move the camera
	dVector target (cameraEyepoint + cameraDir);
	SetCamera (cameraEyepoint, target);

	// render the scene
	if (!hideObjects) {
		glEnable (GL_LIGHTING);
		GraphicManager::Iterator iter (GraphicManager::GetManager());
		for (iter.Begin(); iter; iter ++) {
			glPushMatrix();
			iter.GetNode()->GetInfo()->Render();
			glPopMatrix();
		}
	}

	// if debug display is on, scane all ridid bodies and display the collision geomtry in wire frame
	if (debugLinesMode) {
		DebugShowCollision ();
	}


	dVector color (0.0, 0.0f, 1.0f);
	Print (color, 4, 4, "fps %f", 1.0f / (timeStep + 1.0e-6f));
	Print (color, 4, 34, "f1 - show collition mesh");
	Print (color, 4, 64, "f2 - hide model");
	Print (color, 4, 94, "f3 - toogle animation mode for upvector");
} 


dFloat  RayCastFilter (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	// skip statics rigid bodies
	if (mass != 0.0f) {
		if (intersetParam < pickedParam) {
			// fileter the player
			if ((RenderPrimitive*) NewtonBodyGetUserData (body) != player) {
				pickedParam = intersetParam;
				pickedBody = (NewtonBody*)body;
			}
		}
	}

	return intersetParam;
}

//	Keyboard handler. 
void  Keyboard()
{
	// check for termination
	if (dGetKeyState (VK_ESCAPE) & 0x8000) {
		exit(0);
	}

	// show debug information when press F1
	static unsigned prevF1Key = ~(dGetKeyState (VK_F1) & 0x8000);
	unsigned f1Key = dGetKeyState (VK_F1) & 0x8000;
	if (prevF1Key & f1Key) {
		// clear debugLines counter
		debugLinesMode = ! debugLinesMode;
	}
	prevF1Key = ~f1Key;


	static unsigned prevF2Key = ~(dGetKeyState (VK_F2) & 0x8000);
	unsigned f2Key = dGetKeyState (VK_F2) & 0x8000;
	if (prevF2Key & f2Key) {
		// toggle debug line mode 
		hideObjects = ! hideObjects;
	}
	prevF2Key = ~f2Key;

	static unsigned prevF3Key = ~(dGetKeyState (VK_F3) & 0x8000);
	unsigned f3Key = dGetKeyState (VK_F3) & 0x8000;
	if (prevF3Key & f3Key) {
		// toggle debug line mode 
		animateUpVector = ! animateUpVector;
	}
	prevF3Key = ~f3Key;


	// read the mouse position and set the camera direction
	static MOUSE_POINT mouse0;
	static dFloat yawAngle = 90.0f * 3.1416f / 180.0f;
	static dFloat rollAngle = 0.0f;
	static bool mousePickMode = false;
	static int mouseLeftKey0 = dGetKeyState (VK_LBUTTON) & 0x8000;

	int mouseLeftKey1;
	MOUSE_POINT mouse1;

	GetCursorPos(mouse1);
	mouseLeftKey1 = dGetKeyState (VK_LBUTTON) & 0x8000;

	// the object pick ha two mode...
	if (mouseLeftKey1) {
		// mouse button  is depressed
		if (!mouseLeftKey0) {

			// when the mouse button  is depressed the first time
			// we shot a ray to see if it hit a non filterd rigid body
			dVector p0 (ScreenToWorld(mouse1.x, mouse1.y, 0));
			dVector p1 (ScreenToWorld(mouse1.x, mouse1.y, 1));

			pickedBody = NULL;
			pickedParam = 1.1f;
			NewtonWorldRayCast(nWorld, &p0[0], &p1[0], RayCastFilter, NULL);
			if (pickedBody) {

				// the ray hit a body. we need to save the body picked and asignt the pick force funtion to the 
				// rigiod body
				dMatrix matrix;

				mousePickMode = true;
				NewtonBodyGetMatrix(pickedBody, &matrix[0][0]);
				dVector p (p0 + (p1 - p0).Scale (pickedParam));

				attachmentPoint = matrix.UntransformVector (p);
				NewtonBodySetForceAndTorqueCallback (pickedBody, PhysicsApplyPickForce);
				if (sleepMode) {
					NewtonBodySetAutoFreeze (pickedBody, 0);
					NewtonWorldUnfreezeBody (nWorld, pickedBody);
				}
			}
		}

		// if the mouse button  is kept down...,
		if (mousePickMode) {

			// the button  is still down we nee to update the distnace fron the object to the point
			// on the pick line
			dMatrix matrix;
			NewtonBodyGetMatrix(pickedBody, &matrix[0][0]);
			dVector p0 (ScreenToWorld(mouse1.x, mouse1.y, 0));
			dVector p1 (ScreenToWorld(mouse1.x, mouse1.y, 1));
			dVector p2 (matrix.TransformVector (attachmentPoint));

			dVector p ( p0 + (p1 - p0).Scale (pickedParam));
			pickedForce = p - p2;

			// show the pick points
			ShowMousePicking (p, p2);

		} else {
			// the mouse button  is still down but it did not pick a body it just want s to rotate the entire scene
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
	} else {
		// the mouse button was le go, we need to restore the rigid body to the original state.
		mousePickMode = false;
		if (pickedBody) {
			if (sleepMode) {
				NewtonWorldUnfreezeBody (nWorld, pickedBody);
				NewtonBodySetAutoFreeze (pickedBody, 1);
			}
			NewtonBodySetForceAndTorqueCallback (pickedBody, PhysicsApplyGravityForce);
			pickedBody = NULL;
		}
	}

	// save this state mouse button, for used in the next frame
	mouse0 = mouse1;
	mouseLeftKey0 = mouseLeftKey1;

	// apply a simple player imput control
	if (player) {
		dVector force (0.0f, 0.0f, 0.0f); 

		// camera control
		if (dGetKeyState ('W') & 0x8000) {
			force.m_x = 1.0f;
		} else if (dGetKeyState ('S') & 0x8000) {
			force.m_x = -1.0f;
		}

		if (dGetKeyState ('D') & 0x8000) {
			force.m_z = 1.0f;
		} else if (dGetKeyState ('A') & 0x8000) {
			force.m_z = -1.0f;
		}

		// only one jump per acapce key hit
		static unsigned prevSpace = ~(dGetKeyState (' ') & 0x8000);
		unsigned space = dGetKeyState (' ') & 0x8000;
		if (prevSpace & space) {
			force.m_y = 1.0f;
		}
		prevSpace = ~space;

		player->SetForce (force);

		cameraEyepoint = player->GetMatrix().m_posit;
		cameraEyepoint.m_y += 1.0f;

		cameraEyepoint -= cameraDir.Scale (4.0f);
	}

} 




void PopulateLevel ()
{
	NewtonCollision* collision;

	// set the initial size
	dVector size (1.0f, 1.0f, 1.0f);

	// create the collision 
	collision = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, NULL); 
		
	// populate the world with some boxes 
	dMatrix location (GetIdentityMatrix());
	location.m_posit.m_x = -10.0f; 

	dFloat damp[3];
	damp[0] = 0.0f;
	damp[1] = 0.0f;
	damp[2] = 0.0f;

	for (int k = 0; k < 10; k ++) { 
		location.m_posit.m_z = 0.0f; 
		for (int j = 0; j < 10; j ++) { 
			location.m_posit.m_y = 10.0f; 
			for (int i = 0; i < 1; i ++) {
				BoxPrimitive* box;
				NewtonBody* boxBody;

				// create a graphic box
				box = new BoxPrimitive (location, size);

				//create the rigid body
				boxBody = NewtonCreateBody (nWorld, collision);

				// set zero linear and angulra ddrag
				NewtonBodySetLinearDamping (boxBody, 0.0f);
				NewtonBodySetAngularDamping (boxBody, damp);

				//dVector omega(5, 0, 0);
				//NewtonBodySetOmega (boxBody, &omega[0]);

				// Set Material Id for this object
				if (dRand() & 1) {
					NewtonBodySetMaterialGroupID (boxBody, metalID);
				} else {
					NewtonBodySetMaterialGroupID (boxBody, woodID);
				}

				// save the pointer to the graphic object with the body.
				NewtonBodySetUserData (boxBody, box);

				// set a destrutor for this rigid body
				NewtonBodySetDestructorCallback (boxBody, PhysicsBodyDestructor);

				// set the tranform call back function
				NewtonBodySetTransformCallback (boxBody, PhysicsSetTransform);

				// set the force and torque call back funtion
				NewtonBodySetForceAndTorqueCallback (boxBody, PhysicsApplyGravityForce);

				// set the mass matrix
				NewtonBodySetMassMatrix (boxBody, 1.0f, 1.0f, 1.0f, 1.0f);

				// set the matrix for tboth the rigid nody and the graphic body
				NewtonBodySetMatrix (boxBody, &location[0][0]);
				PhysicsSetTransform (boxBody, &location[0][0]);

				location.m_posit.m_y += size.m_y * 2.0f;
			}
			location.m_posit.m_z -= size.m_z * 4.0f; 	
		}
		location.m_posit.m_x += size.m_x * 4.0f; 
	}

	// release the collsion geometry when not need it
	NewtonReleaseCollision (nWorld, collision);
}


void AddBallsWithRollingFriction ()
{
	NewtonCollision* collision;

	// set the initial size
	dVector size (1.0f, 1.0f, 1.0f);

		
	// populate the world with some boxes 
	dMatrix location (GetIdentityMatrix());
	location.m_posit.m_x = -12.0f; 
	location.m_posit.m_z = -2.0f; 

	dFloat damp[3];
	damp[0] = 0.0f;
	damp[1] = 0.0f;
	damp[2] = 0.0f;

	// it is very important that the maxx and the inerrtai are calculated correctly
	dFloat mass = 5.0f;
	dFloat r = size.m_x * 0.5f;
	dFloat Ixx = 0.4f * mass * r * r;
	dFloat Iyy = Ixx;
	dFloat Izz = Ixx;

	// create the collision 
	collision = NewtonCreateSphere (nWorld, r, r, r, NULL); 

			
	for (int k = 0; k < 5; k ++) { 
		location.m_posit.m_z = 0.0f; 
		for (int j = 0; j < 5; j ++) { 
			location.m_posit.m_y = 10.0f; 
			for (int i = 0; i < 1; i ++) {
				RenderPrimitive* box;
				NewtonBody* boxBody;

				// create a graphic box
				box = new SpherePrimitive (location, r, r, r);

				//create the rigid body
				boxBody = NewtonCreateBody (nWorld, collision);

				// set zero linear and angulra ddrag
				NewtonBodySetLinearDamping (boxBody, 0.0f);
				NewtonBodySetAngularDamping (boxBody, damp);

				//dVector omega(5, 0, 0);
				//NewtonBodySetOmega (boxBody, &omega[0]);

				// Set Material Id for this object
				if (dRand() & 1) {
					NewtonBodySetMaterialGroupID (boxBody, metalID);
				} else {
					NewtonBodySetMaterialGroupID (boxBody, woodID);
				}

				// save the pointer to the graphic object with the body.
				NewtonBodySetUserData (boxBody, box);

				// set a destrutor for this rigid body
				NewtonBodySetDestructorCallback (boxBody, PhysicsBodyDestructor);

				// set the tranform call back function
				NewtonBodySetTransformCallback (boxBody, PhysicsSetTransform);

				// set the force and torque call back funtion
				NewtonBodySetForceAndTorqueCallback (boxBody, PhysicsApplyGravityForce);

				// set the mass matrix
				NewtonBodySetMassMatrix (boxBody, mass, Ixx, Iyy, Izz);

				// set the matrix for tboth the rigid nody and the graphic body
				NewtonBodySetMatrix (boxBody, &location[0][0]);
				PhysicsSetTransform (boxBody, &location[0][0]);

				location.m_posit.m_y += size.m_y * 2.0f;

				// Add a dry rolling friction joint to the ball
				new DryRollingFriction (boxBody, r, 5.2f);

			}
			location.m_posit.m_z -= size.m_z * 4.0f; 	
		}
		location.m_posit.m_x += size.m_x * 4.0f; 
	}

	// release the collsion geometry when not need it
	NewtonReleaseCollision (nWorld, collision);
}








