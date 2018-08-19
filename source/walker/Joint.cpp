
#include "Arduino.h"
#include "Joint.h"
#include <Servo.h>
#include <String.h>
//
// -- Joint constructor
//
public: 
  Joint::Joint(char name[], int servoPin, int rangeMin, int rangeMax) {
    //
    // -- initialize state
    _name = name;
    _rangeMin = rangeMin;
    _rangeMax = rangeMax;
    //
    // -- attach servo
    myservo.attach(servoPin);
  };
  //
  // -- move joint
  //
  void Joint::move(float position, float speed) {
    //
    // -- set target position and speed
    if (position < 0) {
      this->targetPosition = 0;
    } else if (position > 99) {
      this->targetPosition = 99;
    } else {
      this->targetPosition = position;
    }
    if (speed < 0) {
      this->speed = 0;
    } else if (speed > 99) {
      this->speed = 99;
    } else {
      this->speed = speed;
    }
  };
  void Joint::loop() {
    if (positionCurrent != targetPosition) {
      //
      // -- calculate new position
      if (positionCurrent < targetPosition) {
        positionCurrent += speed;
        if (positionCurrent > targetPosition) positionCurrent = targetPosition;
      } else {
        positionCurrent -= speed;
        if (positionCurrent < targetPosition) positionCurrent = targetPosition;
      }
      //
      // -- move joint to position, simple linear motion
      int currentServoValue = int(rangeMin + (((rangeMax - rangeMin) * positionCurrent) / 100.0));
      //myservo.write(currentServoValue);
      //
      // -- debug
      Serial.println("update(), currentServoValue [" + String(currentServoValue) + "], positionCurrent [" + String(positionCurrent) + "], speed [" + String(speed) + "], targetPosition [" + String(targetPosition) + "]");
    }
  }


