#ifndef __GAMEMAIN_H_
#define __GAMEMAIN_H_
#include "ogre.h"

namespace bdb{
  namespace game{
    class BDBGame{
      public:
        BDBGame();
        ~BDBGame();
        bool Initialise();
      private:
        Ogre::Root m_bdbRoot;
        Ogre::RenderWindow* m_ptrRenderWindow;
        Ogre::Camera* mCamera;
        Ogre::SceneManager* mSceneMgr;
        
    };
  }
}
#endif