/*
  Joint.h - Library for servo driven single joint.
*/
#ifndef Joint_h
#define Joint_h

#include "Arduino.h"
#include <Servo.h>
#include <String.h>
/*
* A single join in a limb
*/
class Joint {
private:
  //
  // -- name of this joint
  //char _name[];
  String _name;
  //
  // -- servo position values (actual values sent to servo)
  int _rangeMin;
  int _rangeMax;
  //
  // -- current virtual position 0...99, scaled to rangeMin...rangeMax
  float _positionCurrent;
  //
  // -- desired virtual position 0...99, scaled to rangeMin...rangeMax
  float _targetPosition;
  //
  // -- unit step for position
  float _speed;
  //
  // -- servo controller
  Servo _myservo;
public:
  //
  // -- constructor
  Joint(char name[], int servoPin, int rangeMin, int rangeMax);
  //
  // -- move joint
  void move(float position, float speed);
  //
  // The setup() function runs once each time the micro-controller starts
  public: void setup();
  //
  // The continuous loop() function
  public: void loop();
//
#endif // Joint_h define

