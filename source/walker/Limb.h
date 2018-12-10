/*
  Limb.h - Library for managing three joints for each arm
*/
#ifndef Limb_h
#define Limb_h

#include "Arduino.h"
#include "Constants.h"
#include "Joint.h"
#include <String.h>
/*
* A single join in a limb
*/
class Limb {
private:
  //
  // -- name of this limb
  String name;
  //
  // -- joints
  Joint wrist;
  Joint elbow;
  Joint shoulder;
  //
  // -- current hand position
  int handCurrentX;
  int handCurrentY;
  int handCurrentZ;
  //
  // -- target hand position
  int handTargetX;
  int handTargetY;
  int handTargetZ;
public:
  //
  // -- constructor
  Limb();
  //
  // -- return true/false if the previous command is complete.
  bool commandComplete();
  //
  // -- move hand to the coordinte during the durationMsec time. x,y,z are in mm with respect to the shoulder joint (0,0,0) is the joint.
  void moveHandLinear(int x, int y, int z, int durationMsec);
  //
  // The setup() function runs once each time the micro-controller starts
  public: void setup(String name, int servoPinWrist, int servoPinElbow, int servoPinShoulder);
  //
  // The continuous loop() function
  public: void loop();
};
//
#endif // Limb_h define

