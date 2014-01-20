//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

#include <stdafx.h>
#include "tutorial.h"
#include "HiResTimer.h"
#include "Materials.h"
#include "LevelPrimitive.h"
#include "graphicSystem.h"
#include "RenderPrimitive.h"

CHiResTimer timer;


static NewtonWorld* nWorld;


#define GRAVITY		-10.0f
#define CAMERA_SPEED 4.0f

static dVector cameraDir (1.0f, 0.0f, 0.0f);
static dVector cameraEyepoint (0.0f, 5.0f, 0.0f);

static int g_activeBodies = 0;
bool debugLinesMode = false;

int hightMap_debugCount;
dVector hightMap_debugLines[1024][2];


// set this to 0 for more realistic game (the stack will bever go to sleep mode)
#define AUTO_SLEEP_MODE 1
static bool sleepMode = AUTO_SLEEP_MODE;


static void CleanUp ();
static void  Keyboard();
static dFloat FindFloor (dFloat x, dFloat z);
static void*  PhysicsAlloc (int sizeInBytes);
static void  PhysicsFree (void *ptr, int sizeInBytes);
static void  PhysicsBodyDestructor (const NewtonBody* body);
static void  PhysicsSetTransform (const NewtonBody* body, const dFloat* matrix);

static void  PhysicsApplyPickForce (const NewtonBody* body);
static void  PhysicsApplyGravityForce (const NewtonBody* body);

static dFloat  RayCastFilter (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam);
static dFloat  RayCastPlacement (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam);

static void  DebugShowCollision ();
static void  DebugShowBodyCollision (const NewtonBody* body);
static void DebugShowGeometryCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertec, int id);


enum PrimitiveType
{
	_BOX_PRIMITIVE,
	_CONE_PRIMITIVE,
	_SPHERE_PRIMITIVE,
	_CYLINDER_PRIMITIVE,
	_CAPSULE_PRIMITIVE,
	_CHAMFER_CYLINDER_PRIMITIVE,
	_RANDOM_COMVEX_HULL_PRIMITIVE,
};

void CreateBuildingStruture_1 ();
void CreateBuildingStruture_2 ();
void AddBlockObject (dFloat mass, const dMatrix& matrix, const dVector& size, PrimitiveType type);

static dFloat pickedParam;
static dVector pickedForce;
static dVector attachmentPoint;
static NewtonBody* pickedBody;



//********************************************************************
// 
//	Newton Tutorial 7 Using ray cast and other utility functions
//
//********************************************************************
int main(int argc, char **argv)
{
	// initilize opengl	
	InitOpenGl (argc, argv, "Newton Tutotial 7: Using Utility functions", DrawScene);

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

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	dVector force (0.0f, mass * GRAVITY, 0.0f);
	NewtonBodySetForce (body, &force.m_x);
}

void PhysicsApplyPickForce (const NewtonBody* body)
{
	
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dVector veloc;
	dVector omega;
	dMatrix matrix;

	PhysicsApplyGravityForce (body);

	NewtonBodyGetVelocity(body, &veloc[0]);

	NewtonBodyGetOmega(body, &omega[0]);
	NewtonBodyGetVelocity(body, &veloc[0]);
	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	

	dVector force (pickedForce.Scale (mass * 100.0f));
	dVector dampForce (veloc.Scale (10.0f * mass));
	force -= dampForce;

	NewtonBodyGetMatrix(body, &matrix[0][0]);
	dVector point (matrix.RotateVector (attachmentPoint));
	dVector torque (point * force);

//	dVector torqueDamp (omega.Scale (mass * 0.1f));
//	torque -= torqueDamp;


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
	dMatrix mat (*((dMatrix*)matrix));

// get the collision Matrix
dMatrix collisionScaleMatrix;
NewtonCollision* collision;
collision = NewtonBodyGetCollision (body);
NewtonConvexHullModifierGetMatrix (collision, &collisionScaleMatrix[0][0]);
mat = collisionScaleMatrix * mat;

	primitive->SetMatrix (mat);
}

// count the number of blocks actives in the scene
static void  TrackActivesBodies (const NewtonBody* body, unsigned state)
{
	g_activeBodies += (state == 1) ? 1 : -1;
}

// chane the auto sleep mode for this body and activate the body
static void  ToggleAutoSleep (const NewtonBody* body)
{
	NewtonBodySetAutoFreeze (body, sleepMode);
	NewtonWorldUnfreezeBody (nWorld, body);
}


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


void  DebugShowBodyCollision (const NewtonBody* body)
{
	NewtonBodyForEachPolygonDo (body, DebugShowGeometryCollision);
}


void  DebugShowCollision ()
{
	
	glDisable (GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);

	glColor3f(1.0f, 1.0f, 0.0f);
	NewtonWorldForEachBodyDo (nWorld, DebugShowBodyCollision);

	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < hightMap_debugCount; i ++) {
		glVertex3f (hightMap_debugLines[i][0].m_x, hightMap_debugLines[i][0].m_y, hightMap_debugLines[i][0].m_z);
		glVertex3f (hightMap_debugLines[i][1].m_x, hightMap_debugLines[i][1].m_y, hightMap_debugLines[i][1].m_z);
	}

	glEnd();
	hightMap_debugCount = 0;
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
	glEnable (GL_LIGHTING);
	GraphicManager::Iterator iter (GraphicManager::GetManager());
	for (iter.Begin(); iter; iter ++) {
		glPushMatrix();
		iter.GetNode()->GetInfo()->Render();
		glPopMatrix();
	}

	// if debug display is on, scane all ridid bodies and display the collision geomtry in wire frame
	if (debugLinesMode) {
		DebugShowCollision ();
	}

	dVector color (1.0, 1.0f, 1.0f);
	Print (color, 4,  4, "fps %f     active bodies %d", 1.0f / (timeStep + 1.0e-6f), g_activeBodies);
	Print (color, 4, 34, "f1 - auto sleep on/off      f2 - show collision boxes");
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
		if (intersetParam <  pickedParam) {
			pickedParam = intersetParam;
			pickedBody = (NewtonBody*)body;
		}
	}

	return intersetParam;
}


dFloat  RayCastPlacement (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam)
{
	dFloat* paramPtr;

	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	if (mass == 0.0f) {
		// pick any static rigid nody
		paramPtr = (dFloat*)userData;
		paramPtr[0] = intersetParam;
		return intersetParam;
	}
	// else continue the search
	return 1.0;
}

dFloat FindFloor (dFloat x, dFloat z)
{
	dFloat parameter;

	// shot a vertical ray from a high altitude and colleted the interstion partameter.
	dVector p0 (x, 1000.0f, z); 
	dVector p1 (x, -1000.0f, z); 

	parameter = 1.2f;
	NewtonWorldRayCast (nWorld, &p0[0], &p1[0], RayCastPlacement, &parameter);
	_ASSERTE (parameter < 1.0f);

	// the intesrtion is the intepolater value
	return 1000.0f - 2000.0f * parameter;
}



//	Keyboard handler. 
void Keyboard()
{
	// check for termination
	if (dGetKeyState (VK_ESCAPE) & 0x8000) {
		exit(0);
	}


	static unsigned prevF1Key = ~(dGetKeyState (VK_F1) & 0x8000);
	unsigned f1Key = dGetKeyState (VK_F1) & 0x8000;
	if (prevF1Key & f1Key) {
		// toggle auto sleep mode when press F1
		sleepMode = ! sleepMode;
		NewtonWorldForEachBodyDo (nWorld, ToggleAutoSleep);
	}
	prevF1Key = ~f1Key;

	// show debug information when press F2
	static unsigned prevF2Key = ~(dGetKeyState (VK_F2) & 0x8000);
	unsigned f2Key = dGetKeyState (VK_F2) & 0x8000;
	if (prevF2Key & f2Key) {
		// toggle debug line mode 
		debugLinesMode = ! debugLinesMode;
	}
	prevF2Key = ~f2Key;


	// read the mouse position and set the camera direction
	static MOUSE_POINT mouse0;
	static dFloat yawAngle = 0.0f;
	static dFloat rollAngle = 0.0f;
	static bool mousePickMode = false;
	static int mouseLeftKey0 = dGetKeyState (VK_LBUTTON) & 0x8000;

	int mouseLeftKey1;
	MOUSE_POINT mouse1;

	GetCursorPos(mouse1);
	mouseLeftKey1 = dGetKeyState (VK_LBUTTON) & 0x8000;
	if (mouseLeftKey1) {
		if (!mouseLeftKey0) {
			dVector p0 (ScreenToWorld(mouse1.x, mouse1.y, 0));
			dVector p1 (ScreenToWorld(mouse1.x, mouse1.y, 1));

			pickedBody = NULL;
			pickedParam = 1.1f;
			NewtonWorldRayCast(nWorld, &p0[0], &p1[0], RayCastFilter, NULL);
			if (pickedBody) {
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

		if (mousePickMode) {
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
	mouse0 = mouse1;
	mouseLeftKey0 = mouseLeftKey1;


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




// create physics scene
void InitScene()
{
	BoxPrimitive* blockFloor;
	NewtonBody* blockFloorBody;
	NewtonCollision* collision;

	// /////////////////////////////////////////////////////////////////////
	//
	// create the newton world
	nWorld = NewtonCreate (PhysicsAlloc, PhysicsFree);

	// Set the adaptive solve for compromize trade off speed/stability combination
	NewtonSetSolverModel (nWorld, 1);

	// used the adaptive friction model
	NewtonSetFrictionModel (nWorld, 1);

	// Set the termination function
	atexit(CleanUp); 

	int defaulftID;
	defaulftID = NewtonMaterialGetDefaultGroupID(nWorld);
	//NewtonMaterialSetDefaultSoftness (nWorld, defaulftID, defaulftID, 0.05f);
	NewtonMaterialSetDefaultElasticity (nWorld, defaulftID, defaulftID, 0.1f);
	NewtonMaterialSetDefaultFriction (nWorld, defaulftID, defaulftID, 0.9f, 0.5f);

	SetupMaterials (nWorld);

	// /////////////////////////////////////////////////////////////////////
	//
	// create a floor to hold the block world
	dVector floorSize (100.0f, 2.0f, 100.0f);
	dMatrix location (GetIdentityMatrix());
	location.m_posit.m_y = 1.0f; 
	
	// create a box for floor 
	blockFloor = new BoxPrimitive (location, floorSize, g_ballTexture);

	// create the the floor collision, and body with default values
	collision = NewtonCreateBox (nWorld, floorSize.m_x, floorSize.m_y, floorSize.m_z, NULL); 
	blockFloorBody = NewtonCreateBody (nWorld, collision);

	NewtonBodySetMaterialGroupID (blockFloorBody, woodID);
	NewtonReleaseCollision (nWorld, collision);

	// set the transformation for this rigid body
	NewtonBodySetMatrix (blockFloorBody, &location[0][0]);

	// save the pointer to the graphic object with the body.
	NewtonBodySetUserData (blockFloorBody, blockFloor);

	// set a destrutor for this rigid body
	NewtonBodySetDestructorCallback (blockFloorBody, PhysicsBodyDestructor);


	// postion the camera relatione to the building struct
	cameraEyepoint.m_y = FindFloor (cameraEyepoint.m_x, cameraEyepoint.m_z) + 10.0f;

	// /////////////////////////////////////////////////////////////////////
	//
	// build a standard block stack of 20 * 3 boxes for a total of 60
	CreateBuildingStruture_1 ();
	//CreateBuildingStruture_2 ();
}


dFloat RandomVariable(dFloat amp)
{
	return amp * (dFloat (dRand() + dRand()) / dRAND_MAX - 1.0f) * 0.5f;
}


void AddBlockObject (dFloat mass, const dMatrix& srcMatrix, const dVector& size, PrimitiveType type)
{
	dFloat Ixx = 0.0f;
	dFloat Iyy = 0.0f;
	dFloat Izz = 0.0f;
	NewtonBody* blockBoxBody = NULL;
	RenderPrimitive* blockBox = NULL;
	NewtonCollision* collision = NULL;

	// set realistic mass and inertia matrix for each block

	dMatrix matrix (srcMatrix);
	switch (type) 
	{

		case _SPHERE_PRIMITIVE:
		{

			dFloat r = size.m_x * 0.5f;
			Ixx = 0.4f * mass * r * r;
			Iyy = Ixx;
			Izz = Ixx;
			
			// create the collision 
			collision = NewtonCreateSphere (nWorld, r, r, r, NULL); 

			// create a graphic box
			blockBox = new SpherePrimitive (matrix, r, r, r);
			
			break;
		}

		case _BOX_PRIMITIVE:
		{
			Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
			Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
			Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

			// create the collision 
			collision = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, NULL); 

			// create a graphic box
			blockBox = new BoxPrimitive (matrix, size);
			break;
		}

		case _CONE_PRIMITIVE:
		{
			dFloat h = size.m_x;
			dFloat r = size.m_y;

			Ixx = mass * ((3.0f / 10.0f) * r * r);
			Iyy = mass * ((3.0f / 80.0f) * h * h + (3.0f / 20.0f) * r * r);
			Izz = mass * ((3.0f / 80.0f) * h * h + (3.0f / 20.0f) * r * r);

			//matrix = dgRollMatrix (3.0) * matrix;
			
			// create the collision 
			collision = NewtonCreateCone (nWorld, r, h, NULL); 

			// create a graphic box
			blockBox = new ConePrimitive (matrix, r, h);
			break;
		}

		case _CYLINDER_PRIMITIVE:
		{
			Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
			Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
			Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

			//matrix = dgRollMatrix (3.1416f * 0.5f) * matrix;

			// create the collision 
			collision = NewtonCreateCylinder (nWorld, size.m_y, size.m_x, NULL); 

			// create a graphic box
			blockBox = new CylinderPrimitive (matrix, size.m_y, size.m_x);
			break;
		}

		case _CAPSULE_PRIMITIVE:
		{

			Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
			Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
			Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

			// create the collision 
			collision = NewtonCreateCapsule (nWorld, size.m_y, size.m_x, NULL); 

			// create a graphic box
			blockBox = new CapsulePrimitive (matrix, size.m_y, size.m_x);
			break;
		}

		case _CHAMFER_CYLINDER_PRIMITIVE:
		{
			Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
			Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
			Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

			// create the collision 
			matrix = dgRollMatrix (3.1416f * 0.5f) * matrix;
			collision = NewtonCreateChamferCylinder (nWorld, size.m_x * 0.5f, size.m_y, NULL); 

			// create a graphic box
			blockBox = new ChamferCylinderPrimitive (matrix, size.m_x * 0.5f, size.m_y);
			break;
		}


		case _RANDOM_COMVEX_HULL_PRIMITIVE:
		{
			#define SAMPLE_COUNT 500
			Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
			Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
			Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

			// Create a clouds of rondom point around the origin
			dVector cloud [SAMPLE_COUNT];
			int i;
			// make sure that at least the top and bottom are present
			cloud [0] = dVector ( size.m_x * 0.5f, -0.3f,  0.0f);
			cloud [1] = dVector ( size.m_x * 0.5f,  0.3f,  0.3f);
			cloud [2] = dVector ( size.m_x * 0.5f,  0.3f, -0.3f);
			cloud [3] = dVector (-size.m_x * 0.5f, -0.3f,  0.0f);
			cloud [4] = dVector (-size.m_x * 0.5f,  0.3f,  0.3f);
			cloud [5] = dVector (-size.m_x * 0.5f,  0.3f, -0.3f);

			// populate the cloud with pseudo gaussian random points
			for (i = 6; i < SAMPLE_COUNT; i ++) {
				cloud [i].m_x = RandomVariable(size.m_x);
				cloud [i].m_y = RandomVariable(size.m_y * 2.0f);
				cloud [i].m_z = RandomVariable(size.m_z * 2.0f);
			}
			
			collision = NewtonCreateConvexHull (nWorld, SAMPLE_COUNT, &cloud[0].m_x, sizeof (dVector), NULL); 
			
			// create a graphic box
			blockBox = new ConvexHullPrimitive (matrix, nWorld, collision);
			break;
		}
	}

	//create the rigid body
	blockBoxBody = NewtonCreateBody (nWorld, collision);
  
	// activate corilies
//	NewtonBodyCoriolisForcesMode (blockBoxBody, 1);

	// save the pointer to the graphic object with the body.
	NewtonBodySetUserData (blockBoxBody, blockBox);

	// asign the wood id
	NewtonBodySetMaterialGroupID (blockBoxBody, woodID);

	// Set auto freeze off
	NewtonBodySetAutoFreeze (blockBoxBody, sleepMode);

	// set a call back to frach the auto active state of this body
	NewtonBodySetAutoactiveCallback (blockBoxBody, TrackActivesBodies);

	// set a destrutor for this rigid body
	NewtonBodySetDestructorCallback (blockBoxBody, PhysicsBodyDestructor);

	// set the tranform call back function
	NewtonBodySetTransformCallback (blockBoxBody, PhysicsSetTransform);

	// set the force and torque call back funtion
	NewtonBodySetForceAndTorqueCallback (blockBoxBody, PhysicsApplyGravityForce);

	// set the mass matrix
	NewtonBodySetMassMatrix (blockBoxBody, mass, Ixx, Iyy, Izz);

	// set the matrix for tboth the rigid nody and the graphic body
	NewtonBodySetMatrix (blockBoxBody, &matrix[0][0]);
	PhysicsSetTransform (blockBoxBody, &matrix[0][0]);


dFloat xxx[4];
xxx[3] = NewtonBodyGetLinearDamping (blockBoxBody);
NewtonBodyGetAngularDamping (blockBoxBody, xxx);


	// release the collsion geometry when not need it
	NewtonReleaseCollision (nWorld, collision);
}


PrimitiveType GetRandType()
{
//return _SPHERE_PRIMITIVE;
//return _BOX_PRIMITIVE;
//return _CONE_PRIMITIVE;
//return _CYLINDER_PRIMITIVE;
//return _CAPSULE_PRIMITIVE;
//return _CHAMFER_CYLINDER_PRIMITIVE;
//return _RANDOM_COMVEX_HULL_PRIMITIVE;			  

	static int index = -1;
	PrimitiveType probability[] = {_BOX_PRIMITIVE, 
//								   _SPHERE_PRIMITIVE, 
								   _CONE_PRIMITIVE,
								   _RANDOM_COMVEX_HULL_PRIMITIVE,
								   _CYLINDER_PRIMITIVE,
								   _RANDOM_COMVEX_HULL_PRIMITIVE,
//								   _CHAMFER_CYLINDER_PRIMITIVE, 
//								   _CAPSULE_PRIMITIVE,
	};

	
	
//	index = (dRand () >> 5) % sizeof (probability) / sizeof (int);
	index = (index + 1) % (sizeof (probability) / sizeof (int));
	return probability[index];
}


void CreateBuildingStruture_1 ()
{
	int i;
	dFloat plankMass;
	dFloat columnMass;
	


	// /////////////////////////////////////////////////////////////////////
	//
	// Build a parking lot type structure
	 
	dVector columnBoxSize (3.0f, 1.0f, 1.0f);
	dVector plankBoxSize (6.0f, 1.0f, 6.0f);

	// create the stack
	dMatrix baseMatrix (GetIdentityMatrix());

	// get the floor position in forn o fthe camera
	baseMatrix.m_posit.m_x = cameraEyepoint.m_x + 20.0f;
	baseMatrix.m_posit.m_z = cameraEyepoint.m_z + 4.0f;
	baseMatrix.m_posit.m_y = FindFloor (baseMatrix.m_posit.m_x, baseMatrix.m_posit.m_z); 
	baseMatrix.m_posit.m_y += columnBoxSize.m_x / 2.0f;
	

	// set realistic (extremes in this case for 24 bits presision) masses for the diffent components
	// note how the newton engine handle differents masses ratios without compromising stability, 
	// we recomend the applycation keep this ration under 100 for contacts and 50 for joints 
	columnMass = 1.0f;
	//plankMass = 200.0f;
	plankMass = 20.0f;

	dMatrix columAlignment (dgRollMatrix(3.1416f * 0.5f));
	for (i = 0; i < 10; i ++) { 
		dMatrix matrix(columAlignment * baseMatrix);
		// add the 4 column
		matrix.m_posit.m_x -=  (columnBoxSize.m_z - plankBoxSize.m_x) * 0.5f;
		matrix.m_posit.m_z -=  (columnBoxSize.m_z - plankBoxSize.m_z) * 0.5f;
		AddBlockObject (columnMass, matrix, columnBoxSize, GetRandType());

		matrix.m_posit.m_x += columnBoxSize.m_z - plankBoxSize.m_x;
		AddBlockObject (columnMass, matrix, columnBoxSize, GetRandType());

		matrix.m_posit.m_z += columnBoxSize.m_z - plankBoxSize.m_z;		
		AddBlockObject (columnMass, matrix, columnBoxSize, GetRandType());
		
		matrix.m_posit.m_x -= columnBoxSize.m_z - plankBoxSize.m_x;
		AddBlockObject (columnMass, matrix, columnBoxSize, GetRandType());

		// add a plank
		baseMatrix.m_posit.m_y += (columnBoxSize.m_x + plankBoxSize.m_y) * 0.5f * 1.0f;
		AddBlockObject (plankMass, baseMatrix, plankBoxSize, _BOX_PRIMITIVE);

		// set up for another level
		baseMatrix.m_posit.m_y += (columnBoxSize.m_x + plankBoxSize.m_y) * 0.5f * 1.0f;
	}

}


void CreateBuildingStruture_2 ()
{
	int i;
	int j;
	dFloat gap;
	dFloat mass;

	// /////////////////////////////////////////////////////////////////////
	//
	// build a standard block stack of 20 * 3 boxes for a total of 60

	// set the jenga rectangle using the golden ratio for base (1 * 0.5f, 1.62 * 0.5f, 2.0f)
	dVector blockBoxSize (0.8f, 0.5f, 0.8f * 3.0f);

	// create the stack
	dMatrix baseMatrix (GetIdentityMatrix());

	// for the elvation of the floor at the stack position
	baseMatrix.m_posit.m_x = cameraEyepoint.m_x + 12.0f;

	baseMatrix.m_posit.m_y = FindFloor (baseMatrix.m_posit.m_x, baseMatrix.m_posit.m_z); 
	baseMatrix.m_posit.m_y += blockBoxSize.m_y / 2.0f;


	// set realistic mass and inetian matrix for each block
	mass = 5.0f;

	// create a 90 degree rotation matrix
	dMatrix rotMatrix (dgYawMatrix (3.1416f * 0.5f));

	// separate a bit the block alone the horizontal direction
	gap = 0.01f;
	for (i = 0; i < 20; i ++) { 
		dMatrix matrix(baseMatrix);
		matrix.m_posit -= matrix.m_front.Scale (blockBoxSize.m_x - gap); 

		for (j = 0; j < 3; j ++) { 
			AddBlockObject (mass, matrix, blockBoxSize, _BOX_PRIMITIVE);
			matrix.m_posit += matrix.m_front.Scale (blockBoxSize.m_x + gap);  
		}

		baseMatrix = rotMatrix * baseMatrix;
		baseMatrix.m_posit += matrix.m_up.Scale (blockBoxSize.m_y * 0.99f);
	}
}

