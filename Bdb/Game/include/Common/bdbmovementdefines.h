//! bdb::common::movementdefines
/*!
  A bunch of enums that we can use to link them to the keypresses
  
  Coded by Brett Minnie

  Revision History
  
  Revision 1.0 - Started 23/10/2005

  @Todo: Move this to a seperate dll once i remeber how to attach them
*/

#ifndef __bdbmovementdefines_h_
#define __bdbmovementdefines_h_

namespace bdb{
  namespace common{
 //!An enum of the movement keypresses we are expecting to act on
    typedef enum{
      BDB_MOVE_STEPFORWARD,
      BDB_MOVE_STEPBACKWARD,
      BDB_MOVE_STRAFELEFT,
      BDB_MOVE_STRAFERIGHT,
      BDB_MOVE_TURNLEFT,
      BDB_MOVE_TURNRIGHT,
      BDB_MOVE_LOOKUP,
      BDB_MOVE_LOOKDOWN,
      BDB_MOVE_LOOKCENTER,
      BDB_MOVE_JUMP,
      BDB_MOVE_CROUCH,
      BDB_MOVE_ATTACK
    }enum_bdb_move;
    
    //!controller key enum
    typedef enum{
      BDB_CONTROL_OPTIONS,
      BDB_CONTROL_PAUSE,
      BDB_CONTROL_INVENTORY,
      BDB_CONTROL_JOURNAL,
      BDB_CONTROL_MAP,
      BDB_CONTROL_CHAT,
      BDB_CONTROL_ACTIVATE,
      BDB_CONTROL_USE,
      BDB_CONTROL_MOUSETOGGLE,
      BDB_CONTROL_MAPMARK,
      BDB_CONTROL_SPELLBOOK,
      BDB_CONTROL_WEAPONSLOTMELEE,
      BDB_CONTROL_WEAPONSLOTRANGED,
      BDB_CONTROL_SCREENSHOT,
      BDB_CONTROL_STARTRECORD,
      BDB_CONTROL_STOPRECORD,
      BDB_CONTROL_QUICKLSAVE,
      BDB_CONTROL_QUICKLOAD,
      BDB_CONTROL_CONSOLE
    }bdb_enum_control;

    const Ogre::Degree G_MAXLOOKDOWN  = Ogre::Degree(43);
    const Ogre::Degree G_MAXLOOKUP    = Ogre::Degree(43);
    const Ogre::Degree G_MAXFOV       = Ogre::Degree(90);
  }
}

#endif