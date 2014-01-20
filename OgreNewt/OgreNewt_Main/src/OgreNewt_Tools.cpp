#include "OgreNewt_Tools.h"
#include "Line3D.h"

namespace OgreNewt
{

	namespace Converters
	{
	
	// Take a Newton matrix and create a Quaternion + Position_vector
	void MatrixToQuatPos( const float* matrix, Ogre::Quaternion& quat, Ogre::Vector3 &pos )
	{
		// this takes a matrix returned by Newton, and creates a Quaternion
		// and position Vector3, which is more meaningful for Ogre.
		quat = Quaternion( Matrix3(	matrix[0], matrix[4], matrix[8],
						matrix[1], matrix[5], matrix[9],
						matrix[2], matrix[6], matrix[10] ) );
	
		pos = Vector3( matrix[12], matrix[13], matrix[14] );
	}

	// Take a Quaternion and Position Matrix and create a Newton-happy float matrix!
	void QuatPosToMatrix( const Ogre::Quaternion& quat, const Ogre::Vector3 &pos, float* matrix )
	{
		// this takes a Quaternion and a Vector3 and creates a float array
		// which is more meaningful to Newton.
		Matrix3 rot;
		Vector3 xcol, ycol, zcol;
		
		quat.ToRotationMatrix( rot );	// creates a 3x3 rotation matrix from the Quaternion.

		xcol = rot.GetColumn(0);
		ycol = rot.GetColumn(1);
		zcol = rot.GetColumn(2);
	
		// now fill the final matrix with the appropriate data:
		matrix[0] = xcol.x;
		matrix[1] = xcol.y;
		matrix[2] = xcol.z;
		matrix[3] = 0.0f;
	
		matrix[4] = ycol.x;
		matrix[5] = ycol.y;
		matrix[6] = ycol.z;
		matrix[7] = 0.0f;
	
		matrix[8] = zcol.x;
		matrix[9] = zcol.y;
		matrix[10] = zcol.z;
		matrix[11] = 0.0f;
	
		matrix[12] = pos.x;
		matrix[13] = pos.y;
		matrix[14] = pos.z;
		matrix[15] = 1.0;
	}

	} // end namespace "converters"

	namespace MomentOfInertia
	{

		Ogre::Vector3 CalcSphereSolid( Ogre::Real mass, Ogre::Real radius )
		{
			Ogre::Vector3 inertia;

			inertia.x = (2.0f * mass * (radius * radius)) / 5.0;
			inertia.y = inertia.z = inertia.x;

			return inertia;
		}

		Ogre::Vector3 CalcSphereHollow(Ogre::Real mass, Ogre::Real radius )
		{
			Ogre::Vector3 inertia;

			inertia.x = (2.0f * mass * (radius * radius)) / 3.0;
			inertia.y = inertia.z = inertia.x;

			return inertia;
		}

		Ogre::Vector3 CalcBoxSolid(Ogre::Real mass, Ogre::Vector3 size )
		{
			Ogre::Vector3 inertia;

			inertia.x = (mass * ((size.y * size.y) + (size.z * size.z))) / 12.0f;
			inertia.y = (mass * ((size.x * size.x) + (size.z * size.z))) / 12.0f;
			inertia.z = (mass * ((size.y * size.y) + (size.x * size.x))) / 12.0f;

			return inertia;
		}

		Ogre::Vector3 CalcCylinderSolid(Ogre::Real mass, Ogre::Real radius, Ogre::Real height )
		{
			Ogre::Vector3 inertia;

			inertia.x = inertia.y = (mass * ((3.0f*(radius * radius)) + (height * height))) / 12.0f;
			inertia.z = (mass * (radius * radius)) / 2.0f;

			return inertia;
		}


	} // end of namespace "MomentOfInertia"


	//////////////////////////////////////////////////////////
	// DEUBBER FUNCTIONS
	//////////////////////////////////////////////////////////
	Debugger::Debugger()
	{
		m_debuglines = NULL;
		m_debugnode = NULL;
	}

	Debugger::~Debugger()
	{
	}

	Debugger& Debugger::getSingleton()
	{
		static Debugger instance;
		return instance;
	}
		
	void Debugger::init( Ogre::SceneManager* smgr )
	{
		m_debugnode = smgr->getRootSceneNode()->createChildSceneNode("__OgreNewt__Debugger__");
	}


	void Debugger::showLines( OgreNewt::World* world )
	{
		// erase any existing lines!
		if (m_debuglines)
		{
			m_debugnode->detachAllObjects();
			delete m_debuglines;
			m_debuglines = NULL;
		}

		m_debuglines = new Line3D();
		// make the new lines.
		NewtonWorldForEachBodyDo( world->getNewtonWorld(), newtonPerBody );
		m_debuglines->drawLines();

		m_debugnode->attachObject( m_debuglines );

	}

	void Debugger::hideLines()
	{
		// erase any existing lines!
		if (m_debuglines)
		{
			m_debugnode->detachAllObjects();
			delete m_debuglines;
			m_debuglines = NULL;
		}
	}




	void _cdecl Debugger::newtonPerBody( const NewtonBody* body )
	{
		NewtonBodyForEachPolygonDo( body, newtonPerPoly );
	}

	void _cdecl Debugger::newtonPerPoly( const NewtonBody* body, int vertexCount, const float* faceVertec, int id )
	{
		Ogre::Vector3 p0, p1;

		int i= vertexCount - 1;
		p0 = Ogre::Vector3( faceVertec[(i*3) + 0], faceVertec[(i*3) + 1], faceVertec[(i*3) + 2] );

		for (i=0;i<vertexCount;i++)
		{
			p1 = Ogre::Vector3( faceVertec[(i*3) + 0], faceVertec[(i*3) + 1], faceVertec[(i*3) + 2] );
			Debugger::getSingleton().m_debuglines->addPoint( p0 );
			Debugger::getSingleton().m_debuglines->addPoint( p1 );

			p0 = p1;
		}
	}



	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////

		// Allocate Memory for Newton world through Ogre Memory Manager.
	void* _allocateMemory(int sizeInBytes)
	{
#ifdef _DEBUG
		return Ogre::MemoryManager::sMemManager.allocMem(__FILE__, __LINE__, __FUNCTION__,Ogre::m_alloc_malloc, sizeInBytes, 0);
#else
		return Ogre::MemoryManager::sMemManager.allocMem(__FILE__, __LINE__, sizeInBytes );
#endif
	}

	// free Memory for Newton through Ogre Memory Manager.
	void _freeMemory(void* ptr, int sizeInBytes)
	{
#ifdef _DEBUG
		Ogre::MemoryManager::sMemManager.dllocMem(__FILE__, __LINE__, __FUNCTION__, Ogre::m_alloc_free, ptr,0);
#else
		Ogre::MemoryManager::sMemManager.dllocMem(__FILE__, __LINE__, ptr);
#endif
	}

}