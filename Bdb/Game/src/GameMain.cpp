#include "GameMain.h"

namespace bdb{
  namespace game{
    
    BDBGame::BDBGame(){
      m_bdbRoot = null;
      m_ptrRenderWindow = null;
      mSceneMgr = m_bdbRoot.getSceneManager(ST_GENERIC);
    }
    
    BDBGame::~BDBGame(){
      if(m_ptrRenderWindow){
        delete m_ptrRenderWindow;
        m_ptrRenderWindow = null;
      }
      if(mSceneMgr){
        delete mSceneMgr;
        mSceneMgr = null;
      }
    }
    
    
    bool BDBGame::Initialise(){
      m_bdbRoot = Ogre::Root();
      m_ptrRenderWindow = m_bdbRoot.createRenderWindow("MainWindow",800,600,true);
      return true;
    }

    
  }
}