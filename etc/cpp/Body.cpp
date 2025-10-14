
#include "Arduino.h"
#include "Constants.h"
#include "Joint.h"
#include "Limb.h"
#include "Body.h"
#include <VarSpeedServo.h>
//#include <Servo.h>
#include <String.h>
//
// -- Limb Constructor
Body::Body() {};
//
// ====================================================================================================
// -- Limb setup
void Body::setup(String bodyName) {
    //
    // -- initialize state
    name = bodyName;
    //
    // -- create limbs
    frontRightLimb = Limb();
    frontRightLimb.setup( name + "-frontRightLimb", servoChannelFrontRightWrist, servoChannelFrontRightElbow, servoChannelFrontRightShoulder );
//  frontLeftLimb = Limb();
//  frontLeftLimb.setup( name + "-frontLeftLimb", servoChannelFrontLeftWrist, servoChannelFrontLeftElbow, servoChannelFrontLeftShoulder );
//  backRightLimb = Limb();
//  backRightLimb.setup( name + "-backRightLimb", servoChannelBackRightWrist, servoChannelBackRightElbow, servoChannelBackRightShoulder );
//  backLeftLimb = Limb();
//  backLeftLimb.setup( name + "-backLeftLimb", servoChannelBackLeftWrist, servoChannelBackLeftElbow, servoChannelBackLeftShoulder );  
};
//
// ====================================================================================================
// -- debug, move wrist
/* void Body::moveWrist(float desiredServoAngle, float degreesPerSecond) {
    //
    // -- debug
    Serial.println("body[" + name + "].moveWrist()");
    frontRightLimb.moveWrist( desiredServoAngle, degreesPerSecond);
}
 *///
// ====================================================================================================
// -- sit
void Body::sit() {
    //
    // -- debug
    Serial.println("body[" + name + "].sit()");
}
//
// ====================================================================================================
// -- stand
void Body::stand() {
    //
    // -- debug
    Serial.println("body[" + name + "].stand()");
}
//
// ====================================================================================================
// -- walk
void Body::walk(int x, int y, int durationMsec) {
    //
    // -- debug
    Serial.println("body[" + name + "].walk(x:" + x + ", y:" + y + ", " + durationMsec + " msec)");
}
//
// ====================================================================================================
// -- true if all objects have finished thier movement instructions
bool Body::commandComplete() {
    return frontRightLimb.commandComplete();
    //return frontRightLimb.commandComplete() && frontLeftLimb.commandComplete() && backRightLimb.commandComplete() && backLeftLimb.commandComplete();
}
//
// ====================================================================================================
// -- execute this objects micro-movement and call sub objects
void Body::loop() {
    //
    // -- debug
    frontRightLimb.loop();
    //frontLeftLimb.loop();
    //backRightLimb.loop();
    //backLeftLimb.loop();
}
