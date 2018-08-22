/*
  Limb.h - Library for managing three joints for each arm
  /
  Limbs do one thing at at time. 
  You send an instruction and they execute instructions on the joints to satisfy the limb instuction.
  Limb is controlled in x,y,z coordinate space, but controls joints in a joint-angle space
  Coordinate system:
    x in mm, front-to-back, front is positive x, 0 is at the shoulder joint
    y in mm, left-to-right, left is position, 0 is at the shoulder joint
    z in mm, top-to-bottom, top is positive, 0 is at the shoulder joint
  instuctions:
    moveHandLinear( x, y, z, duration ) 
      - move the tip of the hand to this position in mm wrt the shoulder joint
    (more later)
  
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

