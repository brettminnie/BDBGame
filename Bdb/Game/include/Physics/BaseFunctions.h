//! bdb::pyhsics::basefunctions
/*!
  generic physics stuff thats good enough for now, should eventually get replaced
  by a real lib ;)
  
  Coded by Brett Minnie

  Revision History
  
  Revision 1.0 - Started 01/11/2005

  @Todo: Move this to a seperate dll once i remeber how to attach them
*/
#ifndef __basefunctions_h_
#define __basefunctions_h_

namespace bdb{
  namespace physics{
  //Needed cd, velocity etc
  class CVelocity{
    public:
      CVelocity();
      virtual ~CVelocity(){}
      
      inline Ogre::Real CalcVelocity(){
        return Ogre::Real(m_Distance/m_Time);
      }
      inline Ogre::Real CalcVelocity(Ogre::Real fDistance, Ogre::Real fTime){
        return Ogre::Real(fDistance/fTime);
      }
      inline Ogre::Real CalcTime(){
        return Ogre::Real(m_Distance/m_Velocity);
      }
      inline Ogre::Real CalcTime(Ogre::Real fDistance, Ogre::Real fVelocity){
        return Ogre::Real(m_Distance/m_Velocity);
      }
      inline Ogre::Real CalcDistance();
      inline Ogre::Real CalcDistance(Ogre::Real fTime, Ogre::Real fVelocity);
    
      inline void 
    private:
      Ogre::Real m_Distance;
      Ogre::Real m_Velocity;
      Ogre::Real m_Time;
  }
}
#endif