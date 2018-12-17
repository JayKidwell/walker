
#include "Arduino.h"
#include "Constants.h"
#include "Joint.h"
#include "Limb.h"
#include <VarSpeedServo.h>
//#include <Servo.h>
#include <String.h>
//
// -- Limb Constructor
Limb::Limb() {};
//
// ====================================================================================================
// -- Limb setup
void Limb::setup(String limbName, int limbservoChannelWrist, int limbservoChannelElbow, int limbservoChannelShoulder) {
  //
  // -- initialize state
  name = limbName;
  //
  // -- create joints
  wrist = Joint();
  wrist.setup( limbName + "-wrist", limbservoChannelWrist, WRIST_ANGLE_MIN, WRIST_ANGLE_MAX );
  elbow = Joint();
  elbow.setup( limbName + "-elbow", limbservoChannelElbow, ELBOW_ANGLE_MIN, ELBOW_ANGLE_MAX );
  shoulder = Joint();
  shoulder.setup( limbName + "-shoulder", limbservoChannelShoulder, SHOULDER_ANGLE_MIN, SHOULDER_ANGLE_MAX );
};
//
// ====================================================================================================
// -- execute this objects micro-movement and call sub objects
void Limb::loop() {
  //
  // -- debug
  wrist.loop();
  elbow.loop();
  shoulder.loop();
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
