/* 
	OgreNewt Library

	Ogre implementation of Newton Game Dynamics SDK

	OgreNewt version 0.03


		by Walaber

*/

#ifndef _INCLUDE_OGRENEWT_BODYITERATOR
#define _INCLUDE_OGRENEWT_BODYITERATOR

#include <newton.h>

#include "OgreNewt_World.h"
#include "OgreNewt_Body.h"


// OgreNewt namespace.  all functions and classes use this namespace.
namespace OgreNewt
{

/*
	CLASS DEFINITION:

		BodyIterator

	USE:
		this class is an easy way to loop through all bodies in the world, performing some kind of action.
*/
// OgreNewt::BodyIterator
class BodyIterator
{
public:
	typedef void(_cdecl *IteratorCallback)( Body* me );

	void Init( OgreNewt::World* world ) { m_world = world; }

	void go( IteratorCallback callback )
	{
		m_callback = callback;

		NewtonWorldForEachBodyDo( m_world->getNewtonWorld(), newtonIterator );
	}

	static BodyIterator& getSingleton()
	{
		static BodyIterator instance;
		return instance;
	}

protected:

	BodyIterator()
	{
		m_world = NULL;
		m_callback = NULL;
	}

	~BodyIterator() {}

	static void _cdecl newtonIterator( const NewtonBody* body )
	{
		OgreNewt::Body* bod = (OgreNewt::Body*)NewtonBodyGetUserData( body );

		(*getSingleton().m_callback)( bod );
	}

	OgreNewt::World* m_world;
	IteratorCallback m_callback;
    
};


}	// end NAMESPACE OgreNewt

#endif	// _INCLUDE_OGRENEWT_BODYITERATOR