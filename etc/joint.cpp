

/*
* A single join in a limb
*/
class Joint {
  //
  // -- name of this joint
  //char name[];
  String name;
  //
  // -- servo position values (actual values sent to servo)
  int rangeMin = 0;
  int rangeMax = 180;
  //
  // -- current virtual position 0...99, scaled to rangeMin...rangeMax
  float positionCurrent = 50.0;
  //
  // -- desired virtual position 0...99, scaled to rangeMin...rangeMax
  float targetPosition = 50.0;
  //
  // -- unit step for position
  float speed;
  //
  // -- servo controller
  Servo myservo;
  //
  // -- constructor
public:
  Joint(char name[], int servoPin, int rangeMin, int rangeMax) {
    //
    // -- initialize state
    this->name = name;
    this->rangeMin = rangeMin;
    this->rangeMax = rangeMax;
    //
    // -- attach servo
    myservo.attach(servoPin);
  };
  //
  // -- move joint
public: 
  void move(float position, float speed) {
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
  //
  // -- called each loop, moves the limb toward the target position
  // -- for now simple step each update call
  public: void loop() {
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
};
