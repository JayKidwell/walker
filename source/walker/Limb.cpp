
#include "Arduino.h"
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
  this->rangeMin = rangeMin;
  this->rangeMax = rangeMax;
  //
  // -- create joints
  wrist = new Joint(String name, int servoPin, int rangeMin, int rangeMax);
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
    Serial.println("update(), currentServoValue [" + String(currentServoValue) + "], positionCurrent [" + String(positionCurrent) + "], speed [" + String(moveSpeed) + "], targetPosition [" + String(targetPosition) + "]");
  }
}


