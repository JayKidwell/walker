
#include "Arduino.h"
#include "Constants.h"
#include "Joint.h"
#include <Servo.h>
#include <String.h>
//
// -- Joint constructor
//
Joint::Joint() {};
//
// -- Joint setup
//
void Joint::setup(String jointName, int jointServoPin, int jointRangeMin, int jointRangeMax){
  name = jointName;
  rangeMin = jointRangeMin;
  rangeMax = jointRangeMax;
  //
  // -- attach servo
  myservo.attach( jointServoPin );
};
//
// -- move joint
//
void Joint::move(float position, float speed) {
  //
  // -- set target position and speed
  if (position < 0) {
    targetPosition = 0;
  } else if (position > 99) {
    targetPosition = 99;
  } else {
    targetPosition = position;
  }
  if (speed < 0) {
    moveSpeed = 0;
  } else if (speed > 99) {
    moveSpeed = 99;
  } else {
    moveSpeed = speed;
  }
};
void Joint::loop() {
  //
  // -- debug
  Serial.println("joint[" + name + "].loop");
  if (positionCurrent != targetPosition) {
    //
    // -- calculate new position
    if (positionCurrent < targetPosition) {
      positionCurrent += moveSpeed;
      if (positionCurrent > targetPosition) positionCurrent = targetPosition;
    } else {
      positionCurrent -= moveSpeed;
      if (positionCurrent < targetPosition) positionCurrent = targetPosition;
    }
    //
    // -- move joint to position, simple linear motion
    int currentServoValue = int(rangeMin + (((rangeMax - rangeMin) * positionCurrent) / 100.0));
    //myservo.write(currentServoValue);
    //
    // -- debug
    Serial.println("loop(), currentServoValue [" + String(currentServoValue) + "], positionCurrent [" + String(positionCurrent) + "], speed [" + String(moveSpeed) + "], targetPosition [" + String(targetPosition) + "]");
  }
}
