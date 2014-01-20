#pragma once
#include <ExampleApplication.h>

#include <OgreNewt.h>


#include "conveyorBelt.h"
#include "conveyorMatCallback.h"


class OgreNewtonApplication :
	public ExampleApplication
{
public:
	enum BodType { BT_BASIC, BT_CONVEYOR };

	OgreNewtonApplication(void);
	~OgreNewtonApplication(void);

protected:
	void createFrameListener();
	void createScene();

	void createMaterials();

private:
	OgreNewt::World* m_World;
	OgreNewt::MaterialID* mMatDefault;
	OgreNewt::MaterialID* mMatConveyor;
	OgreNewt::MaterialPair* mMatPairDefaultConveyor;
	conveyorMatCallback* mConveyorCallback;

	
	Ogre::SceneNode* msnCam;

	Ogre::FrameListener* mNewtonListener;

	std::vector<conveyorBelt*> mBelts;

};
