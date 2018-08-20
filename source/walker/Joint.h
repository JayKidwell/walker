/*
  Joint.h - Library for servo driven single joint.
*/
#ifndef Joint_h
#define Joint_h

#include "Arduino.h"
#include "Constants.h"
#include "Limb.h"
#include <Servo.h>
#include <String.h>
/*
* A single join in a limb
*/
class Joint {
private:
  //
  // -- name of this joint
  String name;
  //char name[];
  //
  // -- servo position values (actual values sent to servo)
  int rangeMin;
  int rangeMax;
  //
  // -- current virtual position 0...99, scaled to rangeMin...rangeMax
  float positionCurrent;
  //
  // -- desired virtual position 0...99, scaled to rangeMin...rangeMax
  float targetPosition;
  //
  // -- unit step for position
  float moveSpeed;
  //
  // -- servo controller
  Servo myservo;
public:
  //
  // -- constructor
  Joint(String name, int servoPin, int rangeMin, int rangeMax);
  //
  // -- move joint
  void move(float position, float speed);
  //
  // The setup() function runs once each time the micro-controller starts
  public: void setup();
  //
  // The continuous loop() function
  public: void loop();
};
//
#endif // Joint_h define

