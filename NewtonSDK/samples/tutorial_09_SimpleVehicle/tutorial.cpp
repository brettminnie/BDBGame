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
#include "graphicSystem.h"
#include "NewtonJeep.h"
#include "NewtonTank.h"
#include "RenderPrimitive.h"
#include "HeightFieldCollision.h"

CHiResTimer timer;


static NewtonWorld* nWorld;


#define CAMERA_SPEED	10.0f

static NewtonVehicle* player;
static NewtonVehicle* TankVehicle;
static NewtonVehicle* JeepVehicle;

static dVector cameraDir (1.0f, 0.0f, 0.0f);
static dVector cameraEyepoint (400, 50.0f, 400);
static void CalculateTrackingCamera (dFloat rollAngle, dFloat yawAngle);


bool debugLinesMode = false;
bool hideVehicle = false;

int hightMap_debugCount;
dVector hightMap_debugLines[1024][2];



static void CleanUp ();
static void  Keyboard();
static void*  PhysicsAlloc (int sizeInBytes);
static void  PhysicsFree (void *ptr, int sizeInBytes);
static void  PhysicsBodyDestructor (const NewtonBody* body);
static void  PhysicsApplyGravityForce (const NewtonBody* body);
static void  PhysicsSetTransform (const NewtonBody* body, const dFloat* matrix);

//static void AddBodies();
//static void LoadHeighFielsMesh ();
static void  PhysicsApplyPickForce (const NewtonBody* body);
static dFloat  RayCastFilter (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam);

static void  DebugShowCollision ();
static void  DebugShowBodyCollision (const NewtonBody* body);
static void DebugShowGeometryCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertec, int id);


static dFloat pickedParam;
static dVector pickedForce;
static dVector attachmentPoint;
//static NewtonBody* floorBody;
static NewtonBody* pickedBody;



//********************************************************************
// 
//	Newton Tutorial 2 Using Callbacks
//
//********************************************************************
int main(int argc, char **argv)
{
	// initilize opengl	
	InitOpenGl (argc, argv, "Newton Tutotial 9: implementing a simple vehicle", DrawScene);
	

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

void  PhysicsApplyPickForce (const NewtonBody* body)
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

	dVector torqueDamp (omega.Scale (mass * 0.1f));

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
	int i;
	glColor3f(1.0f, 1.0f, 1.0f);         

	glDisable (GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
	NewtonWorldForEachBodyDo (nWorld, DebugShowBodyCollision);
	for (i = 0; i < hightMap_debugCount; i ++) {
		glVertex3f (hightMap_debugLines[i][0].m_x, hightMap_debugLines[i][0].m_y, hightMap_debugLines[i][0].m_z);
		glVertex3f (hightMap_debugLines[i][1].m_x, hightMap_debugLines[i][1].m_y, hightMap_debugLines[i][1].m_z);
		
	}
	glEnd();

	hightMap_debugCount = 0;
}


//static int debug_count;
//static dVector m_debug[1024 * 4][2];



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
	Print (color, 4, 4, "fps %f", 1.0f / (timeStep + 1.0e-6f));
	Print (color, 4, 24, "f1 - show collition mesh");
	Print (color, 4, 44, "f2 - hide model");
	Print (color, 4, 64, "f3 - toggle vehicle");
} 












dFloat  RayCastFilter (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam)
{
	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	if (mass > 0.0f) {
		if (intersetParam <  pickedParam) {
			pickedParam = intersetParam;
			pickedBody = (NewtonBody*)body;

			//dMatrix matrix;
			//NewtonBodyGetMatrix(body, &matrix[0][0]);
			//dVector pushVeloc (normal[0], normal[1], normal[2]);
			//pushVeloc = pushVeloc.Scale (-20.0f);
			//NewtonAddBodyImpulse(body, &pushVeloc[0], &matrix.m_posit[0]);
		}
	}

	return intersetParam;

}

dFloat  RayCastPlacement (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam)
{
	dFloat* paramPtr;

	paramPtr = (dFloat*)userData;
	paramPtr[0] = intersetParam;
	return intersetParam;
}


static dFloat FindFloor (dFloat x, dFloat z)
{
	dFloat parameter;
	dVector p0 (x, 1000.0f, z); 
	dVector p1 (x, -1000.0f, z); 

	parameter = 1.2f;
	NewtonWorldRayCast (nWorld, &p0[0], &p1[0], RayCastPlacement, &parameter);
	_ASSERTE (parameter < 1.0f);

	return 1000.0f - 2000.0f * parameter;
}


static void CalculateTrackingCamera (dFloat rollAngle, dFloat yawAngle)
{

	// build a rotation matrix by doing a grand smith projection
	dVector front (player->GetMatrix().m_front);
	front.m_y = 0.0f;
	dMatrix matrix (GetIdentityMatrix()); 

	matrix.m_front = front.Scale (1.0f / sqrtf (front % front));
	matrix.m_right = matrix.m_front * matrix.m_up;

	dMatrix cameraDirMatrix (dgRollMatrix(rollAngle) * dgYawMatrix(yawAngle) * matrix);
	cameraDir = cameraDirMatrix.m_front;

	cameraEyepoint = player->GetMatrix().m_posit;
	cameraEyepoint.m_y += 1.0f;

	cameraEyepoint -= cameraDir.Scale (6.0f);
}

//	Keyboard handler. 
void Keyboard()
{
	// check for termination
	if (dGetKeyState (VK_ESCAPE) & 0x8000) {
		exit(0);
	}

	// show debug information when press F1
	static unsigned prevF1Key = ~(dGetKeyState (VK_F1) & 0x8000);
	unsigned f1Key = dGetKeyState (VK_F1) & 0x8000;
	if (prevF1Key & f1Key) {
		// toggle debug line mode 
		debugLinesMode = ! debugLinesMode;
	}
	prevF1Key = ~f1Key;


	static unsigned prevF2Key = ~(dGetKeyState (VK_F2) & 0x8000);
	unsigned f2Key = dGetKeyState (VK_F2) & 0x8000;
	if (prevF2Key & f2Key) {
		// toggle debug line mode 
		hideVehicle = ! hideVehicle;
	}
	prevF2Key = ~f2Key;


	static unsigned prevF3Key = ~(dGetKeyState (VK_F3) & 0x8000);
	unsigned f3Key = dGetKeyState (VK_F3) & 0x8000;
	if (prevF3Key & f3Key) {
		// toggle debug line mode 
		player = (player == TankVehicle) ? JeepVehicle : TankVehicle;
	}
	prevF3Key = ~f3Key;


	// read the mouse position and set the camera direction
	static MOUSE_POINT mouse0;
	static dFloat yawAngle = 0.0f * 3.1416 / 180.0f;
	static dFloat rollAngle = 0.0f;

	MOUSE_POINT mouse1;
	GetCursorPos(mouse1);
  	
	int mouseLeftKey1;
	static bool mousePickMode = false;
	static int mouseLeftKey0 = dGetKeyState (VK_LBUTTON) & 0x8000;

	// this section control the camera object picking
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
				NewtonBodySetAutoFreeze (pickedBody, 0);
				NewtonWorldUnfreezeBody (nWorld, pickedBody);
			}
		}

		if (mousePickMode) {
			// init pick mode
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
			// camera in tracking mode
			if (mouse1.x > (mouse0.x + 1)) {
				yawAngle += 2.0f * 3.1416 / 180.0f;
				if (yawAngle > (360.0f * 3.1416 / 180.0f)) {
					yawAngle -= (360.0f * 3.1416 / 180.0f);
				}
			} else if (mouse1.x < (mouse0.x - 1)) {
				yawAngle -= 2.0f * 3.1416 / 180.0f;
				if (yawAngle < 0.0f) {
					yawAngle += (360.0f * 3.1416 / 180.0f);
				}
			}

			if (mouse1.y > (mouse0.y + 1)) {
				rollAngle += 2.0f * 3.1416 / 180.0f;
				if (rollAngle > (80.0f * 3.1416 / 180.0f)) {
					rollAngle = 80.0f * 3.1416 / 180.0f;
				}
			} else if (mouse1.y < (mouse0.y - 1)) {
				rollAngle -= 2.0f * 3.1416 / 180.0f;
				if (rollAngle < -(80.0f * 3.1416 / 180.0f)) {
					rollAngle = -80.0f * 3.1416 / 180.0f;
				}
			}
			CalculateTrackingCamera(rollAngle, yawAngle);
		}
	} else {
		mousePickMode = false;
		if (pickedBody) {
			NewtonWorldUnfreezeBody (nWorld, pickedBody);
			NewtonBodySetAutoFreeze (pickedBody, 0);
			NewtonBodySetForceAndTorqueCallback (pickedBody, PhysicsApplyGravityForce);
			pickedBody = NULL;
		}
		CalculateTrackingCamera(rollAngle, yawAngle);
	}
	mouseLeftKey0 = mouseLeftKey1;
	mouse0 = mouse1;


	// vehicle controls

	// sterring	
	if (dGetKeyState ('D') & 0x8000) {
		player->SetSteering (-1.0f);
	} else if (dGetKeyState ('A') & 0x8000) {
		player->SetSteering (1.0f);
	} else {
	    player->SetSteering (0.0f);
	}
	
	// forward torque
	if (dGetKeyState ('W') & 0x8000) {
		player->SetTireTorque(1.0f);
	} else if (dGetKeyState ('S') & 0x8000) {
		player->SetTireTorque(-1.0f);
	} else {
		player->SetTireTorque(0.0f);
	}

	 // hand brakes torque
	if (dGetKeyState (' ') & 0x8000) {
		player->SetApplyHandBrakes(1.0f);
	} else {
		player->SetApplyHandBrakes(0.0f);
	}

} 






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


void AddPrimitiveObject (dFloat mass, const dMatrix& srcMatrix, const dVector& size, PrimitiveType type)
{
	dFloat Ixx = 0.0f;
	dFloat Iyy = 0.0f;
	dFloat Izz = 0.0f;
	NewtonBody* blockBoxBody = NULL;
	RenderPrimitive* blockBox = NULL;
	NewtonCollision* collision = NULL;

	// set realistic mass and inertia matrix for each block

	dMatrix matrix (srcMatrix);



	Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
	Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
	Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

	// create the collision 
	collision = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, NULL); 

			// create a graphic box
	blockBox = new BoxPrimitive (matrix, size);



	// add a Collision Tranfform modifiier to show of a collision Gemomtry feature 
	NewtonCollision* collisionModifier;
	collisionModifier = NewtonCreateConvexHullModifier (nWorld, collision);
	NewtonReleaseCollision (nWorld, collision);		
	collision = collisionModifier;
	dMatrix scaleMatrix (GetIdentityMatrix());
	//scaleMatrix.m_front.m_x = 1.0f;
	//scaleMatrix.m_front.m_y = 1.0f;
	//scaleMatrix.m_up.m_y    = 2.0f;
	//scaleMatrix.m_right.m_z = 1.0f;
	// set the modifier matrix to identity
	NewtonConvexHullModifierSetMatrix (collision, &scaleMatrix[0][0]);

	//create the rigid body
	blockBoxBody = NewtonCreateBody (nWorld, collision);
  
	// activate corilies
//	NewtonBodyCoriolisForcesMode (blockBoxBody, 1);

	// save the pointer to the graphic object with the body.
	NewtonBodySetUserData (blockBoxBody, blockBox);

	// Set auto freeze off
//	NewtonBodySetAutoFreeze (blockBoxBody, sleepMode);

	// set a call back to frach the auto active state of this body
//	NewtonBodySetAutoactiveCallback (blockBoxBody, TrackActivesBodies);

	// set a destrutor for this rigid body
	NewtonBodySetDestructorCallback (blockBoxBody, PhysicsBodyDestructor);

	// set the tranform call back function
	NewtonBodySetTransformCallback (blockBoxBody, PhysicsSetTransform);

	// set the force and torque call back funtion
	NewtonBodySetForceAndTorqueCallback (blockBoxBody, PhysicsApplyGravityForce);

	// set the body to be wood
	NewtonBodySetMaterialGroupID (blockBoxBody, woodID);

	// set the mass matrix
	NewtonBodySetMassMatrix (blockBoxBody, mass, Ixx, Iyy, Izz);

	// set the matrix for tboth the rigid nody and the graphic body
	NewtonBodySetMatrix (blockBoxBody, &matrix[0][0]);
	PhysicsSetTransform (blockBoxBody, &matrix[0][0]);

	// release the collsion geometry when not need it
	NewtonReleaseCollision (nWorld, collision);
}



void AddBoxes()
{
	int i;
	int j;
	dFloat mass;

	dFloat x;
	dFloat z;
  
	dVector size (1.0f, 0.25f, 1.2f);

	mass = 10.0f;
	dMatrix matrix (GetIdentityMatrix());
	dVector origin (cameraEyepoint + cameraDir.Scale (10.0f));

	for (i = 0; i < 5; i ++) {
		x = origin.m_x + (i - 2 / 2) * 2;
		for (j = 0; j < 5; j ++) {
			z = origin.m_z + (j - 2 / 2) * 2;

			// find a position above the floor
			matrix.m_posit.m_x = x;
			matrix.m_posit.m_y = FindFloor (x, z) + 3.0f;
			matrix.m_posit.m_z = z;

			AddPrimitiveObject (mass, matrix, size, _BOX_PRIMITIVE);
		}
	}
}




// create physics scene
void InitScene()
{
	HeightFieldCollision *level;

	// /////////////////////////////////////////////////////////////////////
	//
	// create the newton world
	nWorld = NewtonCreate (PhysicsAlloc, PhysicsFree);

	// Set the termination function
	atexit(CleanUp); 


	// need materiakls for this projects
	SetupMaterials (nWorld);

	// /////////////////////////////////////////////////////////////////////
	//
	// load a heigh field mesh
	level = new HeightFieldCollision (nWorld, PhysicsBodyDestructor);

	// set the material group id to the terrain
	NewtonBodySetMaterialGroupID (level->GetRigidBody(), levelID);

	// set the camera origin 10 meter above the gorund
	cameraEyepoint.m_y = FindFloor (cameraEyepoint.m_x, cameraEyepoint.m_z) + 3.0f;

	// /////////////////////////////////////////////////////////////////////
	//
	// place a vecicle in forn of the eyepoint
	dMatrix matrix (GetIdentityMatrix());
	matrix.m_posit = cameraEyepoint + cameraDir.Scale (5.0f);
	matrix.m_posit.m_y = FindFloor (matrix.m_posit.m_x, matrix.m_posit.m_z) + 2.0f;

	JeepVehicle = new NewtonJeep (nWorld, matrix, PhysicsApplyGravityForce);
	//this is the player, Set auto freeze off
	NewtonBodySetAutoFreeze (JeepVehicle->GetRigidBody(), 0);

	matrix.m_posit.m_x += 4.0f; 
	matrix.m_posit.m_z += 4.0f; 
	matrix.m_posit.m_y = FindFloor (matrix.m_posit.m_x, matrix.m_posit.m_z) + 2.0f;

	TankVehicle = new NewtonTank (nWorld, matrix, PhysicsApplyGravityForce);
	//this is the player, Set auto freeze off
	NewtonBodySetAutoFreeze (TankVehicle->GetRigidBody(), 0);

	// set the player to point to this vehicle
	player = JeepVehicle;

	// add some more vehicles to the scene
	int i;
	int j;
	dFloat z;
	z = matrix.m_posit.m_z - 15.0f; 
	for (j = 0; j < 2; j ++) {
		matrix.m_posit.m_x += 7.0f;
		matrix.m_posit.m_z = z;
		for (i = 0; i < 2; i ++) {
			matrix.m_posit.m_z += 7.0f;
			matrix.m_posit.m_y = FindFloor (matrix.m_posit.m_x, matrix.m_posit.m_z) + 2.0f;
			new NewtonJeep (nWorld, matrix, PhysicsApplyGravityForce);
		}
	}

	z = matrix.m_posit.m_z - 20.0f; 
	for (j = 0; j < 2; j ++) {
		matrix.m_posit.m_x += 5.0f;
		matrix.m_posit.m_z = z;
		for (i = 0; i < 2; i ++) {
			matrix.m_posit.m_z += 10.0f;
			matrix.m_posit.m_y = FindFloor (matrix.m_posit.m_x, matrix.m_posit.m_z) + 2.0f;
			//uncommnet this to see more tanks
			new NewtonTank (nWorld, matrix, PhysicsApplyGravityForce);
		}
	}

	AddBoxes();


#if 0
dSceneNode* tank;
//matrix.m_posit.m_y += -1.8f;
//matrix.m_posit.m_x += 6.0f;
//tank = new tank (nWorld, matrix, PhysicsApplyGravityForce, "t72a.dg");
dLoaderContext context;
tank = new dSceneNode ();
tank->LoadFromFile("t72a.dg", context);
#endif
}




