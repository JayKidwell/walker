/*
  Joint.h - Library for servo driven single joint.
*/
#ifndef Joint_h
#define Joint_h

#include "Arduino.h"
#include "Constants.h"
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
  //
  // -- servo angle limits set in constructor
  int servoAngleMin;
  int servoAngleMax;
  //
  // -- current servo angle 0...180
  float servoAngleCurrent;
  //
  // -- desired servo angle, 0...180
  float servoAngleTarget;
  //
  // -- unit step for position
  float moveSpeedDegreesPerSecond;
  //
  // -- the millisecond time that the last loop started (used to calculate loop period)
  unsigned long lastLoopTimeMs;
  //
  // -- servo controller
  Servo myservo;
  //
  // -- true if the last movement command is complete
  bool _commandComplete;

public:
  //
  // -- constructor
  Joint();
  //
  // -- return true/false if the previous command is complete.
  bool commandComplete();
  //
  // -- move joint
  void move(float desiredServoAngle, float DegreesPerSecond);
  //
  // The setup() function runs once each time the micro-controller starts
  void setup(String jointName, int jointServoPin, int jointRangeMin, int jointRangeMax);
  //
  // The continuous loop() function
  void loop();
};
//
#endif // Joint_h define

