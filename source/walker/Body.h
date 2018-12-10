/*
  Body.h 
    - Library for managing the body of the walker
    - position (0,0,0) is the point between the front shoulder joints
    
  
*/
#ifndef Body_h
#define Body_h

#include "Arduino.h"
#include "Constants.h"
#include "Limb.h"
#include <String.h>
/*
* Body
*/
class Body {
private:
  //
  // -- name of this Body
  String name;
  //
  // -- limbs
  Limb frontRightLimb;
  Limb frontLeftLimb;
  Limb backRightLimb;
  Limb backLeftLimb;
  //
  // -- current position
  int bodyCurrentX;
  int bodyCurrentY;
  int bodyCurrentZ;
  //
  // -- target body position
  int bodyTargetX;
  int bodyTargetY;
  int bodyTargetZ;
public:
  //
  // -- constructor
  Body();
  //
  // -- return true/false if the previous command is complete.
  // -- commands are executed in microsteps with each loop call
  bool commandComplete();
  //
  // -- sit the body, moving the body down to 0,0,0
  void sit();
  //
  // -- stand the body, moving it up to 0, 0, 
  void stand();
  //
  // -- move body to the coordinte during the durationMsec time. x,y,z are in mm with respect to center between front shoulder joint (0,0,0).
  void walk(int x, int y, int durationMsec);
  //
  // The setup() function runs once each time the micro-controller starts
  public: void setup(String name);
  //
  // The continuous loop() function
  public: void loop();
};
//
#endif // Body_h define

