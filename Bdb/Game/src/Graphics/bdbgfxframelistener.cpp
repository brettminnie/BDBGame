#include "bdbgfxframelistener.h"
//#using namespace bdb::graphics;
namespace bdb{
  namespace graphics{
    CFrameListener::CFrameListener(
      Ogre::RenderWindow* win, Ogre::Camera* cam, Ogre::Degree degRotation, Ogre::Real rMoveSpeed = 100){
        
      m_degRotateSpeed        = degRotation;
      m_rMoveSpeed            = rMoveSpeed;
	    m_ptrCamera             = cam;
      m_ptrWindow             = win;
      m_bStatsOn              = true;
	    m_uiNumScreenShots      = 0;
	    m_rTimeUntilNextToggle  = 0;
      m_iSceneDetailIndex     = 0;
      m_fMoveScale            = 0.0f;
      m_dRotScale             = 0.0f;
	    m_3dVTranslateVector    = Ogre::Vector3::ZERO;
      m_iAniso                = 1;
      m_Filtering             = Ogre::TFO_TRILINEAR;
      m_bInvertMouse          = false;
    }

    CFrameListener::~CFrameListener(){
    }

    void CFrameListener::moveCamera(){
      // Make all the changes to the camera
      // Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW (e.g. airplane)
      m_ptrCamera->yaw(m_radRotX);
      m_ptrCamera->pitch(m_radRotY);
      m_ptrCamera->moveRelative(m_3dVTranslateVector);
    }
  }
}
