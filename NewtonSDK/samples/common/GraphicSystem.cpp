//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

#include <stdafx.h>
#include "graphicSystem.h"
#include "RenderPrimitive.h"


#ifdef _MSC_VER
	#if (_MSC_VER < 1300) 
		//VC7 or later, building with pre-VC7 runtime libraries
		//defined by VC6 C libs
		extern "C" long _ftol (double); 
		extern "C" long _ftol2( double dblSource ) 
		{ 
			return _ftol( dblSource ); 
		}
	#endif
#endif


GraphicManager::GraphicManager ()
	:dList<RenderPrimitive*>()
{
}

GraphicManager::~GraphicManager ()
{
	while (GetCount()) {
		RemoveObject(GetFirst()->GetInfo());
	}
}

void GraphicManager::AddObject(RenderPrimitive *obj)
{
	Append (obj);
	obj->m_node = GetLast();
}

void GraphicManager::RemoveObject(RenderPrimitive *obj)
{
	if (obj->m_node) {
		Remove ((dListNode*) obj->m_node);
		obj->m_node = NULL;
	}
}

GraphicManager& GraphicManager::GetManager()
{
	static GraphicManager manager;
	return manager;
}


