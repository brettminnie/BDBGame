/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/
#ifndef _INCLUDE_OGRENEWT_COLLISIONPRIMITIVES
#define _INCLUDE_OGRENEWT_COLLISIONPRIMITIVES

#include <newton.h>
#include "OgreNewt_World.h"
#include "OgreNewt_Collision.h"

// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{

	namespace CollisionPrimitives
	{

		// standard primitive Box.
		class Box : public OgreNewt::Collision
		{
		public:
			Box( World* world, Ogre::Vector3 size, 
				Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY, Ogre::Vector3 pos = Ogre::Vector3::ZERO );	// constructor

			~Box();																// destructor
		};

		// standard primitive Ellipsoid.  for a sphere, pass the same radius for all 3 axis.
		class Ellipsoid : public OgreNewt::Collision
		{
		public:
			Ellipsoid(World* world, Ogre::Vector3 size, 
				Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY, Ogre::Vector3 pos = Ogre::Vector3::ZERO );	// constructor

			~Ellipsoid();																// destructor
		};

		// standard primitive cylinder.  default aligned along the local X axis (x=height)
		class Cylinder : public OgreNewt::Collision
		{
		public:
			Cylinder( World* world, Ogre::Real radius, Ogre::Real height, 
				Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY, Ogre::Vector3 pos = Ogre::Vector3::ZERO );	// constructor

			~Cylinder();															// destructor
		};

		// standard primitive capsule.  default aligned along the local X axis (x=height)
		class Capsule : public OgreNewt::Collision
		{
		public:
			Capsule( World* world, Ogre::Real radius, Ogre::Real height, 
				Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY, Ogre::Vector3 pos = Ogre::Vector3::ZERO );	// constructor

			~Capsule();																// destructor
		};

		// standard primitive cone.  default aligned along the local X axis (x=height)
		class Cone : public OgreNewt::Collision
		{
		public:
			Cone( World* world, Ogre::Real radius, Ogre::Real height, 
				Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY, Ogre::Vector3 pos = Ogre::Vector3::ZERO );	// constructor

			~Cone();															// destructor
		};

		// standard primitive chamfer cylinder (closed donut). aligned along local X axis (x=height)
		class ChamferCylinder : public OgreNewt::Collision
		{
		public:
			ChamferCylinder( World* world, Ogre::Real radius, Ogre::Real height, 
				Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY, Ogre::Vector3 pos = Ogre::Vector3::ZERO );	// constructor

			~ChamferCylinder();																// destructor
		};	

		// ConvexHull primitive - "wrap" around a set cloud of vertices.  Pass a SceneNode with an attached Entity.
		class ConvexHull : public OgreNewt::Collision
		{
		public:
			ConvexHull( World* world, Ogre::SceneNode* node, 
				Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY, Ogre::Vector3 pos = Ogre::Vector3::ZERO );	// constructor

			ConvexHull( World* world, Ogre::Vector3* verts, int vertcount,
				Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY, Ogre::Vector3 pos = Ogre::Vector3::ZERO );	// constructor

			~ConvexHull();														// destructor
		};

		



		// TreeCollision - complex polygonal collision - MASS MUST BE ZERO.
		class TreeCollision : public OgreNewt::Collision
		{
		public:
			TreeCollision( World* world, Ogre::SceneNode* node, bool optimize );				// constructor

			~TreeCollision();																	// destructor
		};

		////////////////////////////////////////////////////////
		//	COMPOUND COLLISION!
		class CompoundCollision : public OgreNewt::Collision
		{
		public:
			CompoundCollision( World* world, std::vector<OgreNewt::Collision*> col_array );			// constructor
			~CompoundCollision();
		};


		////////////////////////////////////////////////////////
		// supplemental primitives built from convex hulls
		////////////////////////////////////////////////////////

		// Pyramid primitive - 4-sided base, comes to a single point. base is aligned on XZ plane. made from Convex Hull internally.
		class Pyramid : public OgreNewt::Collision
		{
		public:
			Pyramid( World* world, Ogre::Vector3 size,
				Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY, Ogre::Vector3 pos = Ogre::Vector3::ZERO );	// constructor

			~Pyramid();
		};



	}	// end namespace CollisionPrimitives

}// end namespace OgreNewt

#endif	// _INCLUDE_OGRENEWT_COLLISIONPRIMITIVES