#include ".\ogrenewtonframelistener.h"


#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUI.h>
#include <OgreMemoryMacros.h>


OgreNewtonFrameListener::OgreNewtonFrameListener(RenderWindow* win, Camera* cam, SceneManager* mgr, OgreNewt::World* W, SceneNode* ncam) :
		ExampleFrameListener(win,cam)
{
	m_World = W;
	msnCam = ncam;
	mSceneMgr = mgr;

	dragging = false;

	// setup 3D line node.
	mDragLineNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mDragLine = NULL;

}

OgreNewtonFrameListener::~OgreNewtonFrameListener(void)
{
}

bool OgreNewtonFrameListener::frameStarted(const FrameEvent &evt)
{
	mInputDevice->capture();

	// ----------------------------------------
	// CAMERA CONTROLS
	// ----------------------------------------
	if ((mInputDevice->isKeyDown(Ogre::KC_LSHIFT)) || (mInputDevice->isKeyDown(Ogre::KC_RSHIFT)))
	{
		Vector3 trans, strafe, vec;
		Quaternion quat;

		quat = msnCam->getOrientation();

		vec = Vector3(0.0,0.0,-0.5);
		trans = quat * vec;

		vec = Vector3(0.5,0.0,0.0);
		strafe = quat * vec;

		msnCam->pitch( Degree(mInputDevice->getMouseRelativeY() * -0.5) );
		msnCam->yaw( Degree(mInputDevice->getMouseRelativeX() * -0.5) );

		if (mInputDevice->isKeyDown(Ogre::KC_UP))
			msnCam->translate(trans);

		if (mInputDevice->isKeyDown(Ogre::KC_DOWN))
			msnCam->translate(trans * -1.0);

		if (mInputDevice->isKeyDown(Ogre::KC_LEFT))
			msnCam->translate(strafe * -1.0);

		if (mInputDevice->isKeyDown(Ogre::KC_RIGHT))
			msnCam->translate(strafe);
	}


	// ----------------------------------------
	// DRAGGING!
	// ----------------------------------------

	if (!dragging)
	{

		//user pressing the left mouse button?
		if (mInputDevice->getMouseButton(0))
		{
			// perform a raycast!
			// start at the camera, and go for 100 units in the Z direction.
			Ogre::Vector3 start, end;

			CEGUI::Point mouse = CEGUI::MouseCursor::getSingleton().getPosition();
			CEGUI::Renderer* rend = CEGUI::System::getSingleton().getRenderer();
	
			Ogre::Real mx,my;
			mx = mouse.d_x / rend->getWidth();
			my = mouse.d_y / rend->getHeight();
			Ogre::Ray camray = mCamera->getCameraToViewportRay(mx,my);

			start = camray.getOrigin();
			end = camray.getPoint( 100.0 );

			OgreNewt::BasicRaycast* ray = new OgreNewt::BasicRaycast( m_World, start, end );
			OgreNewt::BasicRaycast::BasicRaycastInfo info = ray->getFirstHit();

			if (info.mBody)
			{
				// a body was found.  first let's find the point we clicked, in local coordinates of the body.
				

				// while dragging, make sure the body can't fall asleep.
				info.mBody->unFreeze();
				info.mBody->setAutoFreeze(0);

				Ogre::Vector3 bodpos;
				Ogre::Quaternion bodorient;

				info.mBody->getPositionOrientation( bodorient, bodpos );

				// info.mDistance is in the range [0,1].
				Ogre::Vector3 globalpt = camray.getPoint( 100.0 * info.mDistance );
				Ogre::Vector3 localpt = bodorient.Inverse() * (globalpt - bodpos);

				// now we need to save this point to apply the spring force, I'm using the userData of the bodies in this example.
				info.mBody->setUserData( this );

				// now change the force callback from the standard one to the one that applies the spring (drag) force.
				info.mBody->setCustomForceAndTorqueCallback( OgreNewtonFrameListener::dragCallback );

				// save the relevant drag information.
				dragBody = info.mBody;
				dragDist = (100.0 * info.mDistance);
				dragPoint = localpt;

				dragging = true;
			}

			delete ray;

		}

		if (mDragLine)
			remove3DLine();
	}
	else
	{
		// currently dragging!
		if (!mInputDevice->getMouseButton(0))
		{
			// no longer holding mouse button, so stop dragging!
			// remove the special callback, and put it back to standard gravity.
			dragBody->setStandardForceCallback();
			dragBody->setAutoFreeze(1);

			dragBody = NULL;
			dragPoint = Ogre::Vector3::ZERO;
			dragDist = 0.0;

			dragging = false;
		}
	}




	if (mInputDevice->isKeyDown(Ogre::KC_ESCAPE))
		return false;

	return true;
}



void OgreNewtonFrameListener::dragCallback( OgreNewt::Body* me )
{
	// this body is being dragged by the mouse, so apply a spring force.
	OgreNewtonFrameListener* fl = (OgreNewtonFrameListener*)me->getUserData();

	// first find the global point the mouse is at...
	CEGUI::Point mouse = CEGUI::MouseCursor::getSingleton().getPosition();
	CEGUI::Renderer* rend = CEGUI::System::getSingleton().getRenderer();
	
	Ogre::Real mx,my;
	mx = mouse.d_x / rend->getWidth();
	my = mouse.d_y / rend->getHeight();
	Ogre::Ray camray = fl->mCamera->getCameraToViewportRay( mx, my );

	Ogre::Vector3 campt = camray.getPoint( fl->dragDist );

	// now find the global point on the body:
	Ogre::Quaternion bodorient;
	Ogre::Vector3 bodpos;

	me->getPositionOrientation( bodorient, bodpos );

	Ogre::Vector3 bodpt = (bodorient * fl->dragPoint) + bodpos;

	// apply the spring force!
	Ogre::Vector3 inertia;
	Ogre::Real mass;

	me->getMassMatrix( mass, inertia );

	Ogre::Vector3 dragForce = ((campt - bodpt) * mass * 8.0) - me->getVelocity();

	// draw a 3D line between these points for visual effect :)
	if (fl->mDragLine)
	{
		// line exists, remove it.
		fl->remove3DLine();
	}
	fl->mDragLine = new Line3D();
	fl->mDragLine->addPoint( campt );
	fl->mDragLine->addPoint( bodpt );
	fl->mDragLine->drawLines();
	fl->mDragLineNode->attachObject( fl->mDragLine );


	// Add the force!
	me->addGlobalForce( dragForce, bodpt );

	Ogre::Vector3 gravity = Ogre::Vector3(0,-9.8,0) * mass;
	me->addForce( gravity );

}


void OgreNewtonFrameListener::remove3DLine()
{
	while (mDragLineNode->numAttachedObjects() > 0)
	{
		Ogre::MovableObject* obj = mDragLineNode->detachObject(static_cast<unsigned short>(0));
	}

	// delete the line.
	delete mDragLine;

	mDragLine = NULL;
}

