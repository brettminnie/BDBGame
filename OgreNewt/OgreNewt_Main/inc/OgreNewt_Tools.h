/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/
#ifndef _INCLUDE_OGRENEWT_TOOLS
#define _INCLUDE_OGRENEWT_TOOLS


#include <Ogre.h>
#include <Newton.h>
#include "OgreNewt_World.h"
#include "Line3D.h"
using namespace Ogre;

namespace OgreNewt
{

	namespace Converters
	{
		// Take a Newton matrix and create a Quaternion + Position_vector
		void MatrixToQuatPos( const float* matrix, Quaternion& quat, Vector3 &pos );
	

		// Take a Quaternion and Position Matrix and create a Newton-happy float matrix!
		void QuatPosToMatrix( const Quaternion& quat, const Vector3 &pos, float* matrix );
	}

	namespace MomentOfInertia
	{

		Ogre::Vector3 CalcSphereSolid( Ogre::Real mass, Ogre::Real radius );
		Ogre::Vector3 CalcSphereHollor(Ogre::Real mass, Ogre::Real radius );
		
		Ogre::Vector3 CalcBoxSolid(Ogre::Real mass, Ogre::Vector3 size );

		Ogre::Vector3 CalcCylinderSolid(Ogre::Real mass, Ogre::Real radius, Ogre::Real height );
		
	}

	//This class implements a debug view of the Newton world.  it is a Singleton! only make one!
	class Debugger
	{
	private:
		Ogre::SceneNode* m_debugnode;
		Line3D* m_debuglines;
		
		Debugger();
		~Debugger();

		static void _cdecl newtonPerBody( const NewtonBody* body );
		static void _cdecl newtonPerPoly( const NewtonBody* body, int vertexCount, const float* faceVertec, int id );
	
	public:
		static Debugger& getSingleton();

		void init( Ogre::SceneManager* smgr );

		void showLines( OgreNewt::World* world );
		void hideLines();
	
	};

	// Allocate Memory for Newton world through Ogre Memory Manager.
	void* _allocateMemory(int sizeInBytes);

	// free Memory for Newton through Ogre Memory Manager.
	void _freeMemory(void* ptr, int sizeInBytes);

}	// end NAMESPACE OgreNewt

#endif