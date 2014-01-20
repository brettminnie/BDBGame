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
#include "HeightFieldCollision.h"

CHiResTimer timer;


static NewtonWorld* nWorld;


#define OBJEST_COUNT	3
//#define OBJEST_COUNT	1
#define GRAVITY		-50.0f
#define CAMERA_SPEED 24.0f

#define SPACING		 60.0f

static dVector cameraDir (1.0f, 0.0f, 0.0f);
static dVector cameraEyepoint (400, 50.0f, 400);

bool hideObjects = false;
bool debugLinesMode = false;

int hightMap_debugCount;
dVector hightMap_debugLines[1024 * 4][2];


static void CleanUp ();
static void  Keyboard();
static void*  PhysicsAlloc (int sizeInBytes);
static void  PhysicsFree (void *ptr, int sizeInBytes);
//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// simple 4d vector class
//********************************************************************

static void  PhysicsBodyDestructor (const NewtonBody* body);
static void  PhysicsApplyGravityForce (const NewtonBody* body);
static void  PhysicsSetTransform (const NewtonBody* body, const dFloat* matrix);


static void AddBodies();
static void LoadHeighFielsMesh ();
static void  PhysicsApplyPickForce (const NewtonBody* body);
static dFloat  RayCastPlacement (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam);
static dFloat  RayCastFilter (const NewtonBody* body, const dFloat* normal, int collisionID, void* userData, dFloat intersetParam);
static dFloat FindFloor (dFloat x, dFloat z);

static void  DebugShowCollision ();
static void  DebugShowBodyCollision (const NewtonBody* body);
static void DebugShowGeometryCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertec, int id);


static dFloat pickedParam;
static dVector pickedForce;
static dVector attachmentPoint;
static NewtonBody* floorBody;
static NewtonBody* pickedBody;


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


//********************************************************************
// 
//	Newton Tutorial 2 Using Callbacks
//
//********************************************************************
int main(int argc, char **argv)
{
	// initilize opengl	
	InitOpenGl (argc, argv, "Newton Tutotial 8: implemeting height map mesh", DrawScene);
	

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

// set the tranform and all child object transform
void PhysicsSetTransform (const NewtonBody* body, const dFloat* matrix)
{
	RenderPrimitive* primitive;
	NewtonCollision* collision;
	

	// get the graphic object form the rigid body
	primitive = (RenderPrimitive*) NewtonBodyGetUserData (body);

	// set the transformation matrix for this rigid body
	dMatrix mat (*((dMatrix*)matrix));

	// Warp the graphica part to match the collision geometry
	dMatrix collisionScaleMatrix;
	collision = NewtonBodyGetCollision (body);

	// animate the Scale anglue
	primitive->m_modifierScaleAngleX = fmodf (primitive->m_modifierScaleAngleX + 0.002f, 6.2832f);
	primitive->m_modifierScaleAngleY = fmodf (primitive->m_modifierScaleAngleY + 0.002f, 6.2832f);
	primitive->m_modifierScaleAngleZ = fmodf (primitive->m_modifierScaleAngleZ + 0.002f, 6.2832f);

	primitive->m_modifierSkewAngleY = fmodf (primitive->m_modifierSkewAngleY + 0.002f, 6.2832f);

	// get the Modifier scake values matrix 
	NewtonConvexHullModifierGetMatrix (collision, &collisionScaleMatrix[0][0]);
	collisionScaleMatrix[0][0] = 0.5f * (1.0f + 0.5f * sinf (primitive->m_modifierScaleAngleX));
	collisionScaleMatrix[1][1] = 0.5f * (1.0f + 0.5f * sinf (primitive->m_modifierScaleAngleY));
	collisionScaleMatrix[2][2] = 0.5f * (1.0f + 0.5f * sinf (primitive->m_modifierScaleAngleZ));

	// also skew the shape
	collisionScaleMatrix[0][1] = 0.5f * (1 + sinf (primitive->m_modifierSkewAngleY));
//	collisionScaleMatrix[0][2] = 0.5f (1 + sinf (primitive->m_modifierSkewAngleY));
//	collisionScaleMatrix[0][1] = 1.0f;

	// copy the matrix back into the collision
	NewtonConvexHullModifierSetMatrix (collision, &collisionScaleMatrix[0][0]);

	// applyc the Modifier to teh geometry too
	mat = collisionScaleMatrix * mat;

	primitive->SetMatrix (mat);
}


static void DebugShowGeometryCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertec, int id)
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


static void  DebugShowBodyCollision (const NewtonBody* body)
{
	NewtonBodyForEachPolygonDo (body, DebugShowGeometryCollision);
}


static void  DebugShowCollision ()
{
	int i;


	glDisable (GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 0.0f);
	NewtonWorldForEachBodyDo (nWorld, DebugShowBodyCollision);

	glColor3f(1.0f, 1.0f, 1.0f);
	for (i = 0; i < hightMap_debugCount; i ++) {
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
	unsigned physicsTime;

	// get the time step
	timeStep = timer.GetElapsedSeconds();


	// updtate the Newton physics world
	physicsTime = timer.GetTimeInMiliseconds();
	NewtonUpdate (nWorld, timeStep);
	physicsTime = timer.GetTimeInMiliseconds() - physicsTime;



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


	dVector color (1.0, 1.0f, 1.0f);
	Print (color, 4, 4, "fps %f  physics(ms) %d", 1.0f / (timeStep + 1.0e-6f), physicsTime);
	Print (color, 4, 34, "f1 - show collition mesh");
	Print (color, 4, 64, "f2 - hide model");
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



	// read the mouse position and set the camera direction
	static MOUSE_POINT mouse0;
	static dFloat yawAngle = 0.0f * 3.1416 / 180.0f;
	static dFloat rollAngle = 0.0f;

	MOUSE_POINT mouse1;
	GetCursorPos(mouse1);

	static bool mousePickMode = false;

	int mouseLeftKey1;
	static int mouseLeftKey0 = dGetKeyState (VK_LBUTTON) & 0x8000;
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
			NewtonWorldUnfreezeBody (nWorld, pickedBody);
			NewtonBodySetAutoFreeze (pickedBody, 1);
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

	HeightFieldCollision *level;

	// /////////////////////////////////////////////////////////////////////
	//
	// create the newton world
	nWorld = NewtonCreate (PhysicsAlloc, PhysicsFree);

	// Set the termination function
	atexit(CleanUp); 

	int defaulftID;
	defaulftID = NewtonMaterialGetDefaultGroupID(nWorld);
	NewtonMaterialSetDefaultSoftness (nWorld, defaulftID, defaulftID, 0.05f);
	NewtonMaterialSetDefaultElasticity (nWorld, defaulftID, defaulftID, 0.1f);
	NewtonMaterialSetDefaultFriction (nWorld, defaulftID, defaulftID, 0.9f, 0.5f);

	// need materiakls for this projects
	SetupMaterials (nWorld);

	// /////////////////////////////////////////////////////////////////////
	//
	// load a heigh field mesh
	level = new HeightFieldCollision (nWorld, PhysicsBodyDestructor);

	// set the material group id to the terrain
	NewtonBodySetMaterialGroupID (level->GetRigidBody(), levelID);


	// /////////////////////////////////////////////////////////////////////
	//
	// add some rigid bodies over the heigh map
	AddBodies ();

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

	dFloat mass;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	if (mass == 0.0f) {
		// if the ray hit a static body, consided it the floors
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


dFloat RandomVariable(dFloat amp)
{
	return amp * (dFloat (dRand() + dRand()) / dRAND_MAX - 1.0f) * 0.5f;
}


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
	switch (type) 
	{

		case _SPHERE_PRIMITIVE:
		{
			//dFloat r = size.m_x * 0.5f;
			//Ixx = 0.4f * mass * r * r;
			//Iyy = Ixx;
			//Izz = Ixx;

			dVector size1 (size.m_x, size.m_y + RandomVariable(size.m_y * 2.0f), size.m_z + RandomVariable(size.m_z * 2.0f));  

			//dFloat r = size.m_x * 0.5f;
			//Ixx = 0.4f * mass * r * r;
			//Iyy = Ixx;
			//Izz = Ixx;

			Ixx = mass * (size1.m_y * size1.m_y + size1.m_z * size1.m_z) / 12.0f;
			Iyy = mass * (size1.m_x * size1.m_x + size1.m_z * size1.m_z) / 12.0f;
			Izz = mass * (size1.m_x * size1.m_x + size1.m_y * size1.m_y) / 12.0f;

			// create the collision 
			collision = NewtonCreateSphere (nWorld, size1.m_x, size1.m_y, size1.m_z, NULL); 

			// create a graphic box
			blockBox = new SpherePrimitive (matrix, size1.m_x, size1.m_y, size1.m_z);
			//blockBox = new EllipsePrimitive (matrix, nWorld, collision, g_ballTexture);
			
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

			matrix = dgRollMatrix (3.0) * matrix;
			
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
			#define SAMPLE_COUNT 100
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


	// add a Collision Tranfform modifiier to show of a collision Gemomtry feature 
	NewtonCollision* collisionModifier;
	collisionModifier = NewtonCreateConvexHullModifier (nWorld, collision);
	NewtonReleaseCollision (nWorld, collision);		
	collision = collisionModifier;
	dMatrix scaleMatrix (GetIdentityMatrix());
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

	// Set the damping coeficient to the minimun allowed by the engine
	dFloat damp[3];
	damp[0] = 0.0f;
	damp[1] = 0.0f;
	damp[2] = 0.0f;
	NewtonBodySetLinearDamping (blockBoxBody, 0.0f);
	NewtonBodySetAngularDamping (blockBoxBody, damp);

	damp[0] = NewtonBodyGetLinearDamping (blockBoxBody);
	NewtonBodyGetAngularDamping (blockBoxBody, damp);

}


class CrossPrimitive: public RenderPrimitive 
{
	public:
	CrossPrimitive (const dMatrix& matrix, const dVector& size)
		:RenderPrimitive  (matrix), m_yawMatrix (dgYawMatrix (3.1416f * 0.5f)), m_rollMatrix (dgRollMatrix (3.1416f * 0.5f)) 
	{
		m_box0 = new BoxPrimitive (GetIdentityMatrix(), size); 
		m_box1 = new BoxPrimitive (m_yawMatrix, size); 
		m_box2 = new BoxPrimitive (m_rollMatrix, size); 
	}

	~CrossPrimitive ()
	{
		delete m_box0; 
		delete m_box1; 
		delete m_box2; 
	}

	void Render() const
	{
	}

	// add force and torque to rigid body
	static void  SetTransform (const NewtonBody* body, const dFloat* matrixPtr)
	{
		CrossPrimitive* cross;
		PhysicsSetTransform (body, matrixPtr);

		// get the graphic object form the rigid body
		cross = (CrossPrimitive*) NewtonBodyGetUserData (body);

		dMatrix matrix (cross->GetMatrix());
		cross->m_box0->SetMatrix(matrix);  

		dMatrix yawMatrix (cross->m_yawMatrix * matrix);
		cross->m_box1->SetMatrix(yawMatrix);  

		dMatrix rollMatrix (cross->m_rollMatrix * matrix);
		cross->m_box2->SetMatrix(rollMatrix);  
	}

	dMatrix m_yawMatrix; 
	dMatrix m_rollMatrix;

	BoxPrimitive* m_box0;
	BoxPrimitive* m_box1;
	BoxPrimitive* m_box2;
};


void AddCrosses()
{
	int i;
	int j;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;

	dFloat x;
	dFloat z;
	NewtonBody* body;
	RenderPrimitive* obj;
	NewtonCollision* collision;
	NewtonCollision* collisionArray[3];

	dVector size (18.0f, 2.0f, 2.0f);

	// create the collision 
	collisionArray[0] = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, NULL); 

	dMatrix yawMatrix (dgYawMatrix (3.1416f * 0.5f)); 
	collisionArray[1] = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, &yawMatrix[0][0]); 

	dMatrix rollMatrix (dgRollMatrix (3.1416f * 0.5f)); 
	collisionArray[2] = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, &rollMatrix[0][0]); 


	mass = 100.0f;
	Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
	Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
	Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

	Ixx = Ixx + Iyy + Izz;

	dMatrix matrix (GetIdentityMatrix());
	dVector origin (cameraEyepoint + cameraDir.Scale (80.0f));

	for (i = 0; i < 2; i ++) {
		x = origin.m_x + (i - OBJEST_COUNT / 2) * SPACING;
		for (j = 0; j < 2; j ++) {
			z = origin.m_z + (j - OBJEST_COUNT / 2) * SPACING;

			// find a position above the floor
			matrix.m_posit.m_x = x;
			matrix.m_posit.m_y = FindFloor (x, z) + 120.0f;
			matrix.m_posit.m_z = z;


			// create a graphic box
			obj = new CrossPrimitive (matrix, size);

			//create the rigid body
			collision = NewtonCreateCompoundCollision (nWorld, 3, collisionArray);
			body = NewtonCreateBody (nWorld, collision);
			NewtonReleaseCollision (nWorld, collision);

			// save the pointer to the graphic object with the body.
			NewtonBodySetUserData (body, obj);

			// set a destrutor for this rigid body
			NewtonBodySetDestructorCallback (body, PhysicsBodyDestructor);

			// set the tranform call back function
			NewtonBodySetTransformCallback (body, CrossPrimitive::SetTransform);

			// set the force and torque call back funtion
			NewtonBodySetForceAndTorqueCallback (body, PhysicsApplyGravityForce);

			// set the mass matrix
			NewtonBodySetMassMatrix (body, mass, Ixx, Ixx, Ixx);

			// set the matrix for tboth the rigid nody and the graphic body
			NewtonBodySetMatrix (body, &matrix[0][0]);
			PhysicsSetTransform (body, &matrix[0][0]);
		}
	}

	// release the collsion geometry when not need it
	NewtonReleaseCollision (nWorld, collisionArray[0]);
	NewtonReleaseCollision (nWorld, collisionArray[1]);
	NewtonReleaseCollision (nWorld, collisionArray[2]);

}



void AddSpheres()
{
	int i;
	int j;
	dFloat mass;

	dFloat x;
	dFloat z;
  
	dVector size (4.0f, 4.0f, 4.0f);

	mass = 100.0f;
	dMatrix matrix (GetIdentityMatrix());
	dVector origin (cameraEyepoint + cameraDir.Scale (80.0f));

	for (i = 0; i < OBJEST_COUNT; i ++) {
		x = origin.m_x + (i - OBJEST_COUNT / 2) * SPACING;
		for (j = 0; j < OBJEST_COUNT; j ++) {
			z = origin.m_z + (j - OBJEST_COUNT / 2) * SPACING;

			// find a position above the floor
			matrix.m_posit.m_x = x;
			matrix.m_posit.m_y = FindFloor (x, z) + 30.0f;
			matrix.m_posit.m_z = z;

			AddPrimitiveObject (mass, matrix, size, _SPHERE_PRIMITIVE);
		}
	}
 }


void AddBoxes()
{
	int i;
	int j;
	dFloat mass;

	dFloat x;
	dFloat z;
  
	dVector size (8.0f, 5.0f, 7.0f);

	mass = 100.0f;
	dMatrix matrix (GetIdentityMatrix());
	dVector origin (cameraEyepoint + cameraDir.Scale (80.0f));

	for (i = 0; i < OBJEST_COUNT; i ++) {
		x = origin.m_x + (i - OBJEST_COUNT / 2) * SPACING;
		for (j = 0; j < OBJEST_COUNT; j ++) {
			z = origin.m_z + (j - OBJEST_COUNT / 2) * SPACING;

			// find a position above the floor
			matrix.m_posit.m_x = x;
			matrix.m_posit.m_y = FindFloor (x, z) + 20.0f;
			matrix.m_posit.m_z = z;

			AddPrimitiveObject (mass, matrix, size, _BOX_PRIMITIVE);
		}
	}
}


void AddCones()
{
	int i;
	int j;
	dFloat mass;

	dFloat x;
	dFloat z;
  
	dVector size (7.0f, 5.0f, 5.0f);

	mass = 50.0f;
	dMatrix matrix (GetIdentityMatrix());
	dVector origin (cameraEyepoint + cameraDir.Scale (80.0f));

	for (i = 0; i < OBJEST_COUNT; i ++) {
		x = origin.m_x + (i - OBJEST_COUNT / 2) * SPACING;
		for (j = 0; j < OBJEST_COUNT; j ++) {
			z = origin.m_z + (j - OBJEST_COUNT / 2) * SPACING;

			// find a position above the floor
			matrix.m_posit.m_x = x;
			matrix.m_posit.m_y = FindFloor (x, z) + 40.0f;
			matrix.m_posit.m_z = z;

			AddPrimitiveObject (mass, matrix, size, _CONE_PRIMITIVE);
		}
	}
}


void AddCylinder()
{
	int i;
	int j;
	dFloat mass;

	dFloat x;
	dFloat z;
  
	dVector size (8.0f, 4.0f, 4.0f);

	mass = 50.0f;
	dMatrix matrix (GetIdentityMatrix());
	dVector origin (cameraEyepoint + cameraDir.Scale (80.0f));

	for (i = 0; i < OBJEST_COUNT; i ++) {
		x = origin.m_x + (i - OBJEST_COUNT / 2) * SPACING;
		for (j = 0; j < OBJEST_COUNT; j ++) {
			z = origin.m_z + (j - OBJEST_COUNT / 2) * SPACING;

			// find a position above the floor
			matrix.m_posit.m_x = x;
			matrix.m_posit.m_y = FindFloor (x, z) + 60.0f;
			matrix.m_posit.m_z = z;

			AddPrimitiveObject (mass, matrix, size, _CYLINDER_PRIMITIVE);
		}
	}
}

void AddCapsules()
{
	int i;
	int j;
	dFloat mass;

	dFloat x;
	dFloat z;
  
	dVector size (10.0f, 3.0f, 3.0f);

	mass = 50.0f;
	dMatrix matrix (GetIdentityMatrix());
	dVector origin (cameraEyepoint + cameraDir.Scale (80.0f));

	for (i = 0; i < OBJEST_COUNT; i ++) {
		x = origin.m_x + (i - OBJEST_COUNT / 2) * SPACING;
		for (j = 0; j < OBJEST_COUNT; j ++) {
			z = origin.m_z + (j - OBJEST_COUNT / 2) * SPACING;

			// find a position above the floor
			matrix.m_posit.m_x = x;
			matrix.m_posit.m_y = FindFloor (x, z) + 80.0f;
			matrix.m_posit.m_z = z;

			AddPrimitiveObject (mass, matrix, size, _CAPSULE_PRIMITIVE);
		}
	}
}


void AddChamferCylinder()
{
	int i;
	int j;
	dFloat mass;

	dFloat x;
	dFloat z;
  
	dVector size (10.0f, 5.0f, 5.0f);

	mass = 50.0f;
	dMatrix matrix (GetIdentityMatrix());
	dVector origin (cameraEyepoint + cameraDir.Scale (80.0f));

	for (i = 0; i < OBJEST_COUNT; i ++) {
		x = origin.m_x + (i - OBJEST_COUNT / 2) * SPACING;
		for (j = 0; j < OBJEST_COUNT; j ++) {
			z = origin.m_z + (j - OBJEST_COUNT / 2) * SPACING;

			// find a position above the floor
			matrix.m_posit.m_x = x;
			matrix.m_posit.m_y = FindFloor (x, z) + 100.0f;
			matrix.m_posit.m_z = z;

			AddPrimitiveObject (mass, matrix, size, _CHAMFER_CYLINDER_PRIMITIVE);
		}
	}
}


void AddRandomHull()
{
	int i;
	int j;
	dFloat mass;

	dFloat x;
	dFloat z;
  
	dVector size (10.0f, 5.0f, 5.0f);

	mass = 50.0f;
	dMatrix matrix (GetIdentityMatrix());
	dVector origin (cameraEyepoint + cameraDir.Scale (80.0f));

	for (i = 0; i < OBJEST_COUNT; i ++) {
		x = origin.m_x + (i - OBJEST_COUNT / 2) * size.m_z * 6.0f;
		for (j = 0; j < OBJEST_COUNT; j ++) {
			z = origin.m_z + (j - OBJEST_COUNT / 2) * size.m_z * 6.0f;

			// find a position above the floor
			matrix.m_posit.m_x = x;
			matrix.m_posit.m_y = FindFloor (x, z) + 120.0f;
			matrix.m_posit.m_z = z;

			AddPrimitiveObject (mass, matrix, size, _RANDOM_COMVEX_HULL_PRIMITIVE);
		}
	}
}


void AddBodies()
{
	AddBoxes();
	AddCones();
	AddSpheres();
	AddCapsules();
//	AddCrosses();
	AddCylinder();
	AddRandomHull();
	AddChamferCylinder();
} 
