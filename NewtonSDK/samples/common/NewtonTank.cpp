//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

// NewtonTank.cpp: implementation of the NewtonTank class.
//
//////////////////////////////////////////////////////////////////////
#include <stdafx.h>
#include "NewtonTank.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int vehicleID; 
extern bool hideVehicle;


class NewtonTankTire: public ChamferCylinderPrimitive
{
	public:
	NewtonTankTire (NewtonTank* parent, int index, const dMatrix& localMatrix, dFloat radius, dFloat width, dFloat side)
		:ChamferCylinderPrimitive (GetIdentityMatrix(), radius, width)
	{
		m_parent = parent;

		m_side = side;

		// set tire dimensions
		m_width = width;
		m_radius = radius;

		// reposition the tire to  pivoy around the tire local axis
		SetMatrix (localMatrix * parent->GetMatrix());
	}

	~NewtonTankTire()
	{
	}

	void PhysicsSetup ()
	{
		dFloat tireMass; 
		dFloat tireSuspesionShock;
		dFloat tireSuspesionSpring; 
		dFloat tireSuspesionLength;

		// add vehicle tires
		tireMass = 2.0f; 
		tireSuspesionShock = 0.5f;
		tireSuspesionSpring = 6.0f; 
		tireSuspesionLength = 0.25f;

 
		// set the front right tire in local scale
		dMatrix parentMatrix (m_parent->GetMatrix());
		dMatrix tirePosition (GetMatrix() * parentMatrix.Inverse());

		// the tire will spin around the lateral axis of the same tire space
		dVector tirePin (0.0, 0.0, 1.0f);

		// add the tire and set this as the user data
		NewtonVehicleAddTire (m_parent->m_vehicleJoint, &tirePosition[0][0], &tirePin[0], tireMass, m_width * 1.25f, m_radius, 
							  tireSuspesionShock, tireSuspesionSpring, tireSuspesionLength, this, TIRE_COLLITION_ID);
	}

	// this function represent the complete tie model for the vehicle rear tire
	void SetTirePhysics (const NewtonJoint* vehicle, unsigned tireId)
	{
		dFloat omega;
		dFloat speed;
		dFloat acceleration;
		dFloat brakeAcceleration;

		// get the tire angular velocity
		omega = NewtonVehicleGetTireOmega (vehicle, tireId);

		// if brake are applyed ....
		if (m_parent->m_breakValue > 0.0f) {
			// ask Newton for the precise acceleration needed to stop the tire
			brakeAcceleration = NewtonVehicleTireCalculateMaxBrakeAcceleration (vehicle, tireId);

			// tell Newton you want this tire stoped but only if the trque need it is less than 
			// the brakes pad can withstand (assume max brake pad torque is 500 newton * meter)
   			NewtonVehicleTireSetBrakeAcceleration (vehicle, tireId, brakeAcceleration, 500.0f * m_parent->m_breakValue);
		} else {

			// calculate the tire torque desire acceleration
			acceleration = m_parent->m_fowardTorque +  m_parent->m_steerTorque * m_side + 0.5f * omega;
			// the brakes pad can withstand (assume max brake pad torque is 500 newton * meter)
			NewtonVehicleTireSetBrakeAcceleration (vehicle, tireId, acceleration, 1000.0f);
		}

		// set some side slipe as funtion of the linear speed 
		speed = NewtonVehicleGetTireLongitudinalSpeed (vehicle, tireId);
		NewtonVehicleSetTireMaxSideSleepSpeed (vehicle, tireId, speed * 1.0e-3f);
	}



	void Render() const
	{

		// tire primitive are align different do 
		static dMatrix localMatrix (dgYawMatrix (3.1416 * 0.5f));
		if (!hideVehicle) {
			//ChamferCylinderPrimitive::Render();

			GLfloat cubeColor[] = { 1.0f, 1.0f, 1.0f, 1.0 };
			glMaterialfv(GL_FRONT, GL_SPECULAR, cubeColor);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cubeColor);
			glMaterialf(GL_FRONT, GL_SHININESS, 50.0);

			dMatrix matrix (localMatrix * m_matrix);
			glMultMatrix(&matrix[0][0]);

			// set up the cube's texture
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glBindTexture(GL_TEXTURE_2D, g_tireTexture);
			glCallList (m_list);
		}
	}


	dFloat m_width;
	dFloat m_radius;
	dFloat m_side;
	NewtonTank* m_parent;
};



#define TANK_LENGTH		2.0f
#define TANK_HEIGHT		0.5f
#define TANK_WIDTH		1.0f

// vehicle chassis class
NewtonTank::NewtonTank(NewtonWorld* nWorld, const dMatrix& matrix, NewtonApplyForceAndTorque externaforce)
	:NewtonVehicle (matrix)
{
	static dFloat TankBody[][3] =
	{
		{  TANK_LENGTH,  TANK_HEIGHT, TANK_WIDTH},
		{  TANK_LENGTH,  TANK_HEIGHT,-TANK_WIDTH},
		{ -TANK_LENGTH,  TANK_HEIGHT,-TANK_WIDTH},
		{ -TANK_LENGTH,  TANK_HEIGHT, TANK_WIDTH},
					 							
		{  TANK_LENGTH, -TANK_HEIGHT, TANK_WIDTH},
		{  TANK_LENGTH, -TANK_HEIGHT,-TANK_WIDTH},
		{ -TANK_LENGTH, -TANK_HEIGHT,-TANK_WIDTH},
		{ -TANK_LENGTH, -TANK_HEIGHT, TANK_WIDTH},

		{ TANK_LENGTH * 1.2f,  TANK_HEIGHT * 0.8f,  TANK_WIDTH * 0.8f},
		{ TANK_LENGTH * 1.2f,  TANK_HEIGHT * 0.8f, -TANK_WIDTH * 0.8f},
		{-TANK_LENGTH * 1.2f,  TANK_HEIGHT * 0.8f, -TANK_WIDTH * 0.8f},
		{-TANK_LENGTH * 1.2f,  TANK_HEIGHT * 0.8f,  TANK_WIDTH * 0.8f},
		 					   										
		{ TANK_LENGTH * 1.2f, -TANK_HEIGHT * 0.8f,  TANK_WIDTH * 0.8f},
		{ TANK_LENGTH * 1.2f, -TANK_HEIGHT * 0.8f, -TANK_WIDTH * 0.8f},
		{-TANK_LENGTH * 1.2f, -TANK_HEIGHT * 0.8f, -TANK_WIDTH * 0.8f},
		{-TANK_LENGTH * 1.2f, -TANK_HEIGHT * 0.8f,  TANK_WIDTH * 0.8f},
	};

	int i;
	dFloat Ixx;
	dFloat Iyy;
	dFloat Izz;
	dFloat mass;
	NewtonCollision* vehicleCollision;

	// create the vehicle body from the hull of a hull collision
	dMatrix collisioMatrix (GetIdentityMatrix ());
	collisioMatrix.m_posit.m_y = 0.5f;
	vehicleCollision = NewtonCreateConvexHull (nWorld, 16, &TankBody[0][0], 3 * sizeof (dFloat), &collisioMatrix[0][0]);
	GetShapeFromCollision (nWorld, vehicleCollision);

	// calculate the bbox
	dVector minBox (1.0e10f, 1.0e10f, 1.0e10f); 
	dVector maxBox (-1.0e10f, -1.0e10f, -1.0e10f); 
	for (i = 0; i < 16; i ++) {
		 dFloat x = TankBody[i][0];
		 dFloat y = TankBody[i][1];
		 dFloat z = TankBody[i][2];
	
		 minBox.m_x = min (minBox.m_x, x);
		 minBox.m_y = min (minBox.m_y, y);
		 minBox.m_z = min (minBox.m_z, z);
		 maxBox.m_x = max (maxBox.m_x, x);
		 maxBox.m_y = max (maxBox.m_y, y);
		 maxBox.m_z = max (maxBox.m_z, z);
	}

	m_boxSize = maxBox - minBox;
	m_boxOrigin = (maxBox + minBox).Scale (0.5f);

	// get the vehicle size and collision origin
	m_boxSize = maxBox - minBox;
	m_boxOrigin = (maxBox + minBox).Scale (0.5f);

	
	// get inicial value for teh tank tire placement
	dMatrix localMatrix (GetIdentityMatrix());
	int count = (sizeof (m_tires) / sizeof (NewtonTankTire*)) / 2;
	dFloat lenght = 2.0 * TANK_LENGTH;
	dFloat space = lenght / (count - 1);

	// prcedurally place tires on the right side of the tank
	localMatrix.m_posit.m_z = -TANK_WIDTH * 1.2f;
	localMatrix.m_posit.m_y = -TANK_HEIGHT * 0.1f;
	for (i = 0; i < int (sizeof (m_tires) / sizeof (NewtonTankTire*)) / 2; i ++) {
		localMatrix.m_posit.m_x = i * space - lenght / 2;
		m_tires[i] = new NewtonTankTire (this, i, localMatrix, 0.4f, 0.5f, 1.0f);
	}

	// prcedurally place tires on the left side of the tank
	localMatrix.m_posit.m_z = TANK_WIDTH * 1.2f;
	for (; i < int (sizeof (m_tires) / sizeof (NewtonTankTire*)); i ++) {
		localMatrix.m_posit.m_x = (i - count) * space - lenght / 2;
		m_tires[i] = new NewtonTankTire (this, i, localMatrix, 0.4f, 0.5f, -1.0f);
	}


	m_breakValue = 0.0f;
	m_steerTorque = 0.0f;
	m_fowardTorque = 0.0f;

	// crate the physics for this vehicle
    mass = 100.0f;
	dVector size (m_boxSize);
	Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
	Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
	Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

	// ////////////////////////////////////////////////////////////////
	//
	//create the rigid body
	m_vehicleBody = NewtonCreateBody (nWorld, vehicleCollision);

	// save the pointer to the graphic object with the body.
	NewtonBodySetUserData (m_vehicleBody, this);

	// set the material group id for vehicle
	NewtonBodySetMaterialGroupID (m_vehicleBody, vehicleID);

	// set a destrutor for this rigid body
	NewtonBodySetDestructorCallback (m_vehicleBody, DestroyVehicle);

	// set the transform call back function
	NewtonBodySetTransformCallback (m_vehicleBody, SetTransform);

	// set the force and torque call back funtion
	NewtonBodySetForceAndTorqueCallback (m_vehicleBody, externaforce);

	// set the mass matrix
	NewtonBodySetMassMatrix (m_vehicleBody, mass, Ixx, Iyy, Izz);

	// set the matrix for both the rigid body and the graphic body
	NewtonBodySetMatrix (m_vehicleBody, &m_matrix[0][0]);


	// ////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//  second we need to add a vehicle joint to the body
	dVector updir (GetMatrix().m_up);
	m_vehicleJoint = NewtonConstraintCreateVehicle (nWorld, &updir[0], m_vehicleBody); 

	// Set the vehicle control functions
	NewtonVehicleSetTireCallback (m_vehicleJoint, TireUpdate);

	for (i = 0; i < int (sizeof (m_tires) / sizeof (NewtonTankTire*)); i ++) {
		m_tires[i]->PhysicsSetup();
	}

	// after adding all tires the application must call balance tires to finish up the vehicle construction
	NewtonVehicleBalanceTires (m_vehicleJoint, GRAVITY);

	// release the collision 
	NewtonReleaseCollision (nWorld, vehicleCollision);	
}


NewtonTank::~NewtonTank()
{
	int i;
 	for (i = 0; i < int (sizeof (m_tires) / sizeof (NewtonTankTire*)); i ++) {
		delete m_tires[i];
	}
}

// rigid body destructor
void NewtonTank::DestroyVehicle (const NewtonBody* body)
{
	NewtonTank* vehicle;

	// get the graphic object form the rigid body
	vehicle = (NewtonTank*) NewtonBodyGetUserData (body);

	// destroy the graphic object
	delete vehicle;
}



void NewtonTank::GetShapeFromCollision (const NewtonBody* body, int vertexCount, const dFloat* faceVertex, int id)
{
	int i;
	ConvexHullPrimitive* primitive;

	primitive = (ConvexHullPrimitive*) NewtonBodyGetUserData (body);

	glBegin(GL_POLYGON);

	// claculate the face normal for this polygon
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


void NewtonTank::GetShapeFromCollision (NewtonWorld* nWorld, NewtonCollision* collision)
{
	NewtonBody *body;

	m_list = glGenLists(1);
	glNewList (m_list, GL_COMPILE);

	body = NewtonCreateBody (nWorld, collision);
	NewtonBodySetUserData (body, this);

	NewtonBodyForEachPolygonDo (body, GetShapeFromCollision);

	glEndList ();

	NewtonDestroyBody(nWorld, body);
}


void NewtonTank::Render() const
{
	if (!hideVehicle) {
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
}


void NewtonTank::SetApplyHandBrakes (dFloat value)
{
	m_breakValue = value;
}


void NewtonTank::SetTireTorque(dFloat value)
{
	if (value > 0.0f) {
		value = 15.0f;
	} else if (value < 0.0f) {
		value = -10.0f;
	} else {
		value = 0.0f;
	}

	m_fowardTorque = value;
}



void NewtonTank::SetSteering(dFloat value)
{
	if (value > 0.0f) {
		value = -10.0f;
	} else if (value < 0.0f) {
		value = 10.0f;
	} else {
		value = 0.0f;
	}
	m_steerTorque = value;
}


void NewtonTank::TireUpdate (const NewtonJoint* vehicle)
{
	int tyreId;
	NewtonTankTire* tire;

	tire = NULL;

	// iterate trough each tire applying the tire dynamics
	for (tyreId = NewtonVehicleGetFirstTireID (vehicle); tyreId; tyreId = NewtonVehicleGetNextTireID (vehicle, tyreId)) {
		// get the graphic object and set the transformation matrix 
		tire = (NewtonTankTire*) NewtonVehicleGetTireUserData (vehicle, tyreId);
		tire->SetTirePhysics (vehicle, tyreId);
	}

	tire->m_parent->m_breakValue = 0.0f;
}



// Set the vehicle matrix and all tire matrices
void NewtonTank::SetTransform (const NewtonBody* body, const dFloat* matrixPtr)
{
	int tyreId;
	NewtonTankTire* tire;
	NewtonTank* vehicle;

	// get the graphic object form the rigid body
	vehicle = (NewtonTank*) NewtonBodyGetUserData (body);

	// set the transformation matrix for this rigid body
	dMatrix& matrix = *((dMatrix*)matrixPtr);
	vehicle->SetMatrix (matrix);

    //Set the global matrix for each tire
	for (tyreId = NewtonVehicleGetFirstTireID (vehicle->m_vehicleJoint); tyreId; tyreId = NewtonVehicleGetNextTireID (vehicle->m_vehicleJoint, tyreId)) {

		// get the graphic object and set the transformation matrix 
		tire = (NewtonTankTire*) NewtonVehicleGetTireUserData (vehicle->m_vehicleJoint, tyreId);
		NewtonVehicleGetTireMatrix (vehicle->m_vehicleJoint, tyreId, &tire->GetMatrix()[0][0]);
	}
}

