
#include "Arduino.h"
#include "Constants.h"
#include "Joint.h"
#include <VarSpeedServo.h>
//#include <Servo.h>
#include <String.h>
//
// -- Joint constructor
//
Joint::Joint() {};
//
// ====================================================================================================
// -- Joint setup
void Joint::setup(String jointName, int jointservoChannel, int jointServoAngleMin, int jointServoAngleMax){
    lastLoopTimeMs = millis();
    name = jointName;
    if ( jointServoAngleMin < 0 ) {  
        servoAngleMin = 0;
    } else if ( jointServoAngleMin > 180 ) {
        servoAngleMin = 180;
    } else {
        servoAngleMin = jointServoAngleMin;
    }
    if ( jointServoAngleMax < servoAngleMin ) {  
        servoAngleMax = servoAngleMin;
    } else if ( jointServoAngleMin > 180 ) {
        servoAngleMax = 180;
    } else {
        servoAngleMax = jointServoAngleMax;
    }
    //
    // -- attach servo
    myservo.attach( jointservoChannel );
};
//
// ====================================================================================================
// -- move joint
// -- desiredServoAngle is a float representing the servo angle.
//      range 0 to 180
// -- DegreesPerSecond is a float that represents degrees per second.
//      range is 1 to 180
//      ex - DegreesPerSecond==1 moves 1 degree per second
//      the movement each loop is calculated as that needed for the previous loop period (Milli-lastMilli)
void Joint::move(float desiredServoAngle, float DegreesPerSecond) {
    //
    // -- start a new command
    Serial.println("joint[" + name + "].move, desiredServoAngle [" + String(desiredServoAngle) + "], DegreesPerSecond [" + String(DegreesPerSecond) + "]");
    _commandComplete = false;
    //
    // -- set target desiredServoAngle and DegreesPerSecond
    if (desiredServoAngle < 0) {
        servoAngleTarget = 0;
    } else if (desiredServoAngle > 180) {
        servoAngleTarget = 180;
    } else {
        servoAngleTarget = desiredServoAngle;
    }
    if (DegreesPerSecond < 1) {
        moveSpeedDegreesPerSecond = 1;
    } else if (DegreesPerSecond > 180) {
        moveSpeedDegreesPerSecond = 180;
    } else {
        moveSpeedDegreesPerSecond = DegreesPerSecond;
    }
};
//
// ====================================================================================================
// -- execute this objects micro-movement and call sub objects
void Joint::loop() {
    //
    // -- calculate millisecondsPerLoop in milliseconds
    unsigned long timeMs = millis();
    unsigned long millisecondsPerLoop = timeMs - lastLoopTimeMs;

    if (servoAngleCurrent == servoAngleTarget) {
        //
        // -- move is complete
        if (!_commandComplete) {
            Serial.println("joint[" + name + "].loop(), commandComplete, servoAngleTarget [" + String(servoAngleTarget) + "]");
            _commandComplete = true;
        }
    } else {
        //
        // -- calculate moveSpeedDegreesPerLoop
        float moveSpeedDegreesPerLoop = ( moveSpeedDegreesPerSecond * millisecondsPerLoop ) / 1000;
        //
        // -- calculate new servoAngleCurrent
        if (servoAngleCurrent < servoAngleTarget) {
            Serial.println("joint[" + name + "].loop(), move up, servoAngleTarget [" + String(servoAngleTarget) + "], servoAngleCurrent [" + String(servoAngleCurrent) + "], millisecondsPerLoop [" + String(millisecondsPerLoop) + "], moveSpeedDegreesPerLoop [" + String(moveSpeedDegreesPerLoop) + "]");
            servoAngleCurrent += moveSpeedDegreesPerLoop;
            if (servoAngleCurrent > servoAngleTarget) servoAngleCurrent = servoAngleTarget;
        } else {
            Serial.println("joint[" + name + "].loop(), move down, servoAngleTarget [" + String(servoAngleTarget) + "], servoAngleCurrent [" + String(servoAngleCurrent) + "], millisecondsPerLoop [" + String(millisecondsPerLoop) + "], moveSpeedDegreesPerLoop [" + String(moveSpeedDegreesPerLoop) + "]");
            servoAngleCurrent -= moveSpeedDegreesPerSecond;
            if (servoAngleCurrent < servoAngleTarget) servoAngleCurrent = servoAngleTarget;
        }
        //
        // -- move joint to new servoAngleCurrent, simple linear motion
        myservo.write(servoAngleCurrent);
        //
        // -- set time for next loop
        lastLoopTimeMs = timeMs;
    }
}
//
// ====================================================================================================
// -- move joint
bool Joint::commandComplete() {
    return _commandComplete;
};
//
// ====================================================================================================
// -- from adafruit pwm servo example
//    
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  // 1,000,000 us per second, 60Hz
  pulselength = 1000000;   
  pulselength /= 60;
  Serial.print(pulselength); 
  Serial.println(" us per period"); 
  //
  // 12 bits of resolution
  pulselength /= 4096;  
  Serial.print(pulselength); 
  Serial.println(" us per bit"); 
  //
  // convert to us
  pulse *= 1000000;  
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

