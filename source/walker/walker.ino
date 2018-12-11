
#include <Servo.h>
#include <String.h>
#include "Constants.h"
#include "Body.h"
//
// Consider integrating async servo controller
// https://github.com/netlabtoolkit/VarSpeedServo
//
// Define User Types below here or use a .h file
//
Body walkerBody;
//
// -- loop counter to simulate motion during development
int loopCnt;
//
// -- stepCnt, each step is a different movement
int debugStepCnt;
//
// The setup() function runs once each time the micro-controller starts
void setup(){
    Serial.begin( SerialBaudRate );
    Serial.println("Walker setup...");
    walkerBody = Body();
    walkerBody.setup( "walker1" );
    //
    // -- initialize
    loopCnt = 0;
    debugStepCnt = 0;
    Serial.println("exit setup");
}
//
// -- send instructions to body and call sub objects
void loop(){
    if (walkerBody.commandComplete()) {
        if (debugStepCnt == 0) {
            // park joints
            walkerBody.frontRightLimb.wrist.move( WRIST_ANGLE_PARK, 10 );
            walkerBody.frontRightLimb.elbow.move( ELBOW_ANGLE_PARK, 10 );
            walkerBody.frontRightLimb.shoulder.move( SHOULDER_ANGLE_PARK, 10 );
        } else if ( debugStepCnt == 1 ) {
            // wrist full down
            walkerBody.frontRightLimb.wrist.move( WRIST_ANGLE_MIN, 30 );
        } else if ( debugStepCnt == 2 ) {
            // wrist full up
            walkerBody.frontRightLimb.wrist.move( WRIST_ANGLE_MAX, 30 );
        } else if ( debugStepCnt == 3 ) {
            // wrist park
            walkerBody.frontRightLimb.wrist.move( WRIST_ANGLE_PARK, 30 );
        }
        debugStepCnt++;
    }
    walkerBody.loop();
    loopCnt++;
    //delay(100);
}
