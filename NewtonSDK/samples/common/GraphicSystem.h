//********************************************************************
// Newton Game dynamics 
// copyright 2000-2004
// By Julio Jerez
// VC: 6.0
// 
//********************************************************************

// OpenGlUtil.h: interface for the OpenGlUtil class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GRAPHIC_SYSTEM_1734uedfksd87i__
#define __GRAPHIC_SYSTEM_1734uedfksd87i__

#include <stdafx.h>

class RenderPrimitive;  

// make a small and simpe graphic manager
class GraphicManager: public dList<RenderPrimitive*>
{
	public:
	GraphicManager ();
	~GraphicManager ();
	void AddObject(RenderPrimitive *obj);
	void RemoveObject(RenderPrimitive *obj);
	static GraphicManager& GetManager();
};



#endif 
