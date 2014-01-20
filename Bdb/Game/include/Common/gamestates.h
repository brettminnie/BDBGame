#ifndef __GAMESTATES_H_
#define __GAMESTATES_H_

//Enumeration for Game States
namespace bdb{
  namespace common{
    typedef enum{
      BDB_GAME_INITALISING,
      BDB_GAME_CONFIGUI,
      BDB_GAME_LOADSTATE,
      BDB_GAME_SAVECURRENT
    }enum_bdb_gamestate;
  }
}

static unsigned int g_iLastState;
static std::stack<int> g_arrGameStateStack;

bool InitGameStackStack(){
  try{
    
  }
  catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
         MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
         std::cerr << "An exception has occured: " <<
            e.getFullDescription().c_str() << std::endl;
#endif
      }
}
#endif