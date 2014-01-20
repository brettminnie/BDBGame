

	How can I make use Newton in Cpp with as Callbacks?

This is a problem that many novice C++ programmers when facer with a library with C interface. Some time this lead them to dismissed the library al together thinking there is to way is can be used in C++ as a object oriented class, when it is just the opposite, a library with a C interface is by far more flexible to integrate with a C++ application than a one with a C++ interface.

This one possible solution to the problem:

-	Create you own base call representing you rigid body.
-	Add a private static function for each function call back of the rigid body.
-	Add a pure virtual function for each function call back of the rigid body
-	Make the constructor private
-	Add a pointed to the rigid body as private member.
-	Wrap of the functionality of the rigid body with equivalents oublic member function.

Here is an example:

// Header file for a Object oriented encapsulation of a Newton Rigid body

class MyMatrix;		// an object that encapsulates the application matrix 
class MyWorld;      // an object encapsulation of the Newton world.


// Ridig nody base class 
class MyRididBody  
{
	protected:
	// dommy contructor
	MyRididBody ();

	// this effective construtor
	void Int (MyWorld* world, NewtonCollision* collision); 
	virtual ~ MyRididBody();

	// public interface 
	public:
	 
	void SetMatrix (const MyMatrix& matrix);
	const MyMatrix& GetMatrix SetMatrix () const;

	void SetCollision (const MyCollision* collision);
	MyCollision*  GetCollision ();



	protected:
	// Rigid bodies events 
	virtual void ApplyForceAndTorque () = 0;
	virtual void AutoActivation (unsigned state) = 0;
	virtual void SetTransform(const MyMatrix& matrix) = 0;


	// private
	static void AutoDestruction (const NewtonBody* body);
	static void ApplyForceAndTorque (const NewtonBody* body);
	static void SetTransform (const NewtonBody* body, const dFloat* matrix);
	static void AutoAutovation (const NewtonBody* body, unsigned state);

	NewtonBody* m_body;
	// other member if desired here
	...

};


// Code implementation of a Newton Reid Body encapsulation
#include "MyRidifBody.h"


MyRididBody::MyRididBody ()
	:m_body (NULL)
{
}

// this is the effective call contrution, it must be call be each derive class from the //constrution
void MyRididBody::Init (MyWorld* world, NewtonCollision* collision)
{
	// create the ridid body
	m_body = NewtonCreateBody (world->GetWorld(), NewtonCollision);

	// store the pointer to the class with the Newton Objects
	NewtonBodySetUserData  (m_body, this);

	// hook the events call backs
	NewtonBodySetTransformCallback (m_body, SetTransform);
	NewtonBodySetDestructorCallback (m_body, AutoDestruction);
	NewtonBodySetAutoactiveCallback (m_body, AutoAutovation);	NewtonBodySetForceAndTorqueCallback (m_body, ApplyForceAndTorque);
}



MyRididBody::~MyRididBody ()
{
//if the joint has user data it means the application is detroiyb the joint
	if (NewtonBodyGetUserData (m_body)) {

		// set the joint call to NULL to prevent infinite recursion 
		NewtonBodySetDestructorCallback (m_body, NULL);  

		// destroy this rigid body
		NewtonDestroyBody (NewtonBodyGetWorld (m_body));
	}
}

// This function can be call from the Newton World to destroy a body	
void MyRididBody::AutoDestruction ()
{
	MyRididBody* body;  

	// get the pointer to the body class
	body = (MyRididBody *) NewtonBodySetUserData (me);  

	// set the joint call to NULL to prevent infinite recursion
	NewtonBodySetDestructorCallback (me, NULL);  
	NewtonBodySetUserData  (m_body, NULL);

	// delete the joint class
	delete body;
}


void MyRididBody::SetTransform (const NewtonBody* body, const dFloat* matrix)
{
	MyRididBody* body;  

	// get the pointer to the body class
	body = (MyRididBody *) NewtonBodySetUserData (me);  

	_ASSERTE (body);

	const MyMatrix& matrix = *((const MyMatrix*) (matrix));
	body-> SetTransform(matrix);
}


void MyRididBody::ApplyForceAndTorque (const NewtonBody* body)
{
	MyRididBody* body;  

	// get the pointer to the body class
	body = (MyRididBody *) NewtonBodySetUserData (me);  

	_ASSERTE (body);

	body-> ApplyForceAndTorque()
}


// the rest of the implemetation is very simular.
...
...




// here is and example of a gravity Box

Class MyBox: public MyBody
{
	public:
	MyBox (MyWorld* world, const MyMatrix& location, const MyVector& size, dFloat mass)	
		:MyBody ()
	{

		NewtonCollision* collision;


		// create the collision and initilize the object
		collision = NewtonCreateBox (nWorld, size.m_x, size.m_y, size.m_z, NULL); 
		Init (world, collision);
		NewtonReleaseCollision (nWorld, collision);


		// to initilize the rest of the object you can call member of the base call
		// or call newton funtion directly 
		dFloat Ixx;
		dFloat Iyy;
		dFloat Izz;
		Ixx = mass * (size.m_y * size.m_y + size.m_z * size.m_z) / 12.0f;
		Iyy = mass * (size.m_x * size.m_x + size.m_z * size.m_z) / 12.0f;
		Izz = mass * (size.m_x * size.m_x + size.m_y * size.m_y) / 12.0f;

		NewtonBodySetMassMatrix (blockBoxBody, mass, Ixx, Iyy, Izz);


		NewtonBodySetMatrix (m_body, &location[0][0]);
		PhysicsSetTransform (m_body, &location[0][0]);

		// more initializtion
		...
	}


	~MyBox ()
	{
		// nothing for this object
	}


		
	void ApplyForceAndTorque ()
	{
		// add the gravity force to this body

		dFloat mass;
		dFloat Ixx;
		dFloat Iyy;
		dFloat Izz;

		NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
		dVector force (0.0f, mass * GRAVITY, 0.0f);
		NewtonBodySetForce (body, &force.m_x);
	}




}
