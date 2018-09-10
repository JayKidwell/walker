
#include "Arduino.h"
#include "Constants.h"
#include "Joint.h"
#include "Limb.h"
#include "Body.h"
#include <Servo.h>
#include <String.h>
//
// -- Limb Constructor
Body::Body() {};
//
// -- Limb setup
void Body::setup(String bodyName) {
  //
  // -- initialize state
  name = bodyName;
  //
  // -- create limbs
  frontRightLimb = Limb();
  frontRightLimb.setup( name + "-frontRightLimb", servoPinFrontRightWrist, servoPinFrontRightElbow, servoPinFrontRightShoulder );
  frontLeftLimb = Limb();
  frontLeftLimb.setup( name + "-frontLeftLimb", servoPinFrontLeftWrist, servoPinFrontLeftElbow, servoPinFrontLeftShoulder );
  backRightLimb = Limb();
  backRightLimb.setup( name + "-backRightLimb", servoPinBackRightWrist, servoPinBackRightElbow, servoPinBackRightShoulder );
  backLeftLimb = Limb();
  backLeftLimb.setup( name + "-backLeftLimb", servoPinBackLeftWrist, servoPinBackLeftElbow, servoPinBackLeftShoulder );  
};
//
// -- sit
void Body::sit() {
  //
  // -- debug
  Serial.println("body[" + name + "].sit()");
}
//
// -- stand
void Body::stand() {
  //
  // -- debug
  Serial.println("body[" + name + "].stand()");
}
//
// -- walk
void Body::walk(int x, int y, int durationMsec) {
  //
  // -- debug
  Serial.println("body[" + name + "].walk(x:" + x + ", y:" + y + ", " + durationMsec + " msec)");
}
void Body::loop() {
  //
  // -- debug
  Serial.println("body[" + name + "].loop");
  frontRightLimb.loop();
  frontLeftLimb.loop();
  backRightLimb.loop();
  backLeftLimb.loop();
}



