//! bdb::gfx::CFrameListener
/*!
  Frame listener class to update the frames
  Based on the Ogre example frame listener
  
  Coded by Brett Minnie

  Revision History
  
  Revision 1.0 - Started 23/10/2005

  @Todo: *Move this to a seperate dll once i remeber how to attach them
         *Move most of the variables into private vars and then write accessors for them
*/
#ifndef __bdbgfxframelistener_h_
#define __bdbgfxframelistener_h_

#include "Graphics.h"
#include "Ogre.h"
#include "OgreStringConverter.h"
#include "OgreException.h"

namespace bdb{
  namespace graphics{
    class BDBGRAPHICS CFrameListener:public Ogre::FrameListener{
      protected:
	      int                         m_iSceneDetailIndex ;
        Ogre::Real                  m_rMoveSpeed;
        Ogre::Degree                m_degRotateSpeed;
        Ogre::EventProcessor*       m_ptrEventProcessor;
        Ogre::InputReader*          m_ptrInputDevice;
        Ogre::Camera*               m_ptrCamera;
        Ogre::Vector3               m_3dVTranslateVector;
        Ogre::RenderWindow*         m_ptrWindow;
        bool                        m_bStatsOn;
        unsigned int                m_uiNumScreenShots;
        float                       m_fMoveScale;
        Ogre::Degree                m_dRotScale;
        // just to stop toggles flipping too fast
        Ogre::Real                  m_rTimeUntilNextToggle ;
        Ogre::Radian                m_radRotX;
        Ogre::Radian                m_radRotY;
        Ogre::TextureFilterOptions  m_Filtering;
        int                         m_iAniso;
        bool                        m_bInvertMouse;

    //Constructor 
    public: 
      //! Constructor
      CFrameListener(
        Ogre::RenderWindow* win,  /*!Ptr to the rendering surface*/
        Ogre::Camera* cam,        /*!Ptr to the default camera*/
        Ogre::Degree degRotation, /*!The amount the camera will rotate*/
        Ogre::Real rMoveSpeed     /*!Movement speed of the camera in world units per update*/
      );      

   
      //! Destructor
      virtual ~CFrameListener();
      
      //!X&Y rotations
      inline void RotateLeft(){m_ptrCamera->yaw(m_dRotScale);}
      inline void RotateRight(){m_ptrCamera->yaw(-m_dRotScale);}
      inline void RotateUp(){m_ptrCamera->yaw((m_bInvertMouse)?-m_dRotScale:m_dRotScale);}
      void RotateDown(){m_ptrCamera->yaw((m_bInvertMouse)?m_dRotScale:-m_dRotScale);}
      //!X & Z Translations
      inline void MoveForward(){ m_3dVTranslateVector.z = -m_fMoveScale;}
      inline void MoveBackward(){m_3dVTranslateVector.z = m_fMoveScale;}
      inline void MoveLeft(){m_3dVTranslateVector.x = -m_fMoveScale;}
      inline void MoveRight(){m_3dVTranslateVector.x = m_fMoveScale;}
      //!Y Translations
      void MoveUp(){};
      void MoveDown(){};
      
      //!updates the camera position based on the X, Y & Z translations
      void moveCamera();

    };
  }
}
#endif