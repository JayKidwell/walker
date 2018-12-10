
#include "Arduino.h"
#include "Constants.h"
#include "Joint.h"
#include "Limb.h"
#include <Servo.h>
#include <String.h>
//
// -- Limb Constructor
Limb::Limb() {};
//
// ====================================================================================================
// -- Limb setup
void Limb::setup(String limbName, int limbServoPinWrist, int limbServoPinElbow, int limbServoPinShoulder) {
  //
  // -- initialize state
  name = limbName;
  //
  // -- create joints
  wrist = Joint();
  wrist.setup( limbName + "-wrist", limbServoPinWrist, WRIST_RANGE_MIN, WRIST_RANGE_MAX );
  //elbow = Joint();
  //elbow.setup( limbName + "-elbow", limbServoPinElbow, ELBOW_RANGE_MIN, ELBOW_RANGE_MAX );
  //shoulder = Joint();
  //shoulder.setup( limbName + "-shoulder", limbServoPinShoulder, SHOULDER_RANGE_MIN, SHOULDER_RANGE_MAX );
};
//
// ====================================================================================================
// -- execute this objects micro-movement and call sub objects
void Limb::loop() {
  //
  // -- debug
  Serial.println("limb[" + name + "].loop");
  wrist.loop();
  //elbow.loop();
  //shoulder.loop();
}
//
// ====================================================================================================
// -- move hand to the coordinte during the durationMsec time. x,y,z are in mm with respect to the shoulder joint (0,0,0) is the joint.
void Limb::moveHandLinear(int x, int y, int z, int durationMsec) {
  //
  // -- debug
  Serial.println("limb[" + name + "].moveHandLinear(" + x + "," + y + "," + z + "," + durationMsec + ")");
}
//
// ====================================================================================================
// -- returns true/false if this object's latest command is complete
bool Limb::commandComplete() {
    //
    // -- this limb command is complete if all it's joints are complete
    return wrist.commandComplete();
}
