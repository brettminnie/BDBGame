//! bdb::pyhsics::CConfig extends ogre config
/*!
  Config file class used to read in config values
  extends the ogre class by allowing multiple values via the cunning use of the | character
  
  Coded by Brett Minnie

  Revision History
  
  Revision 1.0 - Started 23/10/2005

  @Todo: Move this to a seperate dll once i remeber how to attach them
*/

#ifndef __bdbconfigfile_h_
#define __bdbconfigfile_h_

#include "OgreConfigFile.h"

namespace bdb{
  namespace common{
    class CConfig: public Ogre::ConfigFile{
    };
  }
}

#endif