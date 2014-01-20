//! bdb::gfx::CBdbMovableEntity
/*!
  Movable Entity class that inherits from the ogre Movable Entity type
  The mesh is stored in the Ogre::Entity super class
  
  Coded by Brett Minnie

  Revision History
  
  Revision 1.0 - Started 07/11/2005

  @Todo: *Move this to a seperate dll once i remeber how to attach them
         *Move most of the variables into private vars and then write accessors for them
*/

#ifndef __bdbmovableentity_h_
#define __bdbmovableentity_h_


namespace bdb{
  namespace common{
  
    class CBdbMovableEntity /*:: public Ogre::MovableObject*/{
      public:
        CBdbMovableEntity(){}
        ~CBdbMovableEntity(){}
        
        //!The name of the object
        std::string       m_szObjectName;
        //Ogre::SceneNode*  m_ptrSceneNode;
        //!The scene node it is attached too
        void*             m_ptrSceneNode;
        //!Is is visible
        bool              m_bVisible;
        //!Does it cast shadows
        bool              m_bShadowCaster;
    };
  }
}

#endif