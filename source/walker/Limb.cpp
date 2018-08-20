
#include "Arduino.h"
#include "Constants.h"
#include "Joint.h"
#include "Limb.h"
#include <Servo.h>
#include <String.h>
//
// -- Limb Constructor
Limb::Limb(String name, int servoPinWrist, int servoPinElbow, int servoPinShoulder); {
  //
  // -- initialize state
  this->name = name;
  //
  // -- create joints
  wrist = new Joint( name + "-wrist", servoPinWrist, WRIST_RANGE_MIN, WRIST_RANGE_MAX )
  elbow = new Joint( name + "-elbow", servoPinElbow, ELBOW_RANGE_MIN, ELBOW_RANGE_MAX )
  shoulder = new Joint( name + "-shoulder", servoPinShoulder, SHOULDER_RANGE_MIN, SHOULDER_RANGE_MAX )
};
  //
  // -- move hand to the coordinte during the durationMsec time. x,y,z are in mm with respect to the shoulder joint (0,0,0) is the joint.
void moveHandLinear(int x, int y, int z, int durationMsec) {
  //
  // -- debug
  Serial.println("limb[" + name + "].moveHandLinear(" + x + "," + y + "," + z + "," + durationMsec + ")");
  }
void Limb::loop() {
  //
  // -- debug
  Serial.println("limb[" + name + "].loop");
}



