
// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>

AF_DCMotor motor1(1);
AF_DCMotor motor2(2);

int loopCnt;

void setup()  
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("setup");
  //
  // turn on motor1
  motor1.setSpeed(0); 
  motor1.run(RELEASE);
  //
  // turn on motor2
  //motor2.setSpeed(0); 
  //motor2.run(RELEASE);
  //
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  //
  loopCnt = 0;
}

void loop()
{  
    Serial.println("loop " + String( loopCnt ));
    uint8_t i;
    //
    if ( loopCnt==0 ) {
      //
      // -- special case phase 4, bring motor 1 up to forward-full
      Serial.println("startup motor 1");
      motor1.run(FORWARD);
      for (i=0; i<255; i++) {
        motor1.setSpeed(i); 
        delay(10);
      }
    }
    //
    Serial.println("phase 1 - motor1 forward-stop, motor2 forward-full");
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    for (i=0; i<255; i++) {
      motor1.setSpeed(255-i);  
      motor2.setSpeed(i);  
      delay(10);
    }
    //
    Serial.println("phase 2 - motor1 reverse-full, motor2 forward-stop");
    motor1.run(BACKWARD);
    motor2.run(FORWARD);
    for (i=0; i<255; i++) {
      motor1.setSpeed(i);  
      motor2.setSpeed(255-i);  
      delay(10);
    }
    //
    Serial.println("phase 3 - motor1 reverse-stop, motor2 reverse-full");
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    for (i=0; i<255; i++) {
      motor1.setSpeed(255-i);  
      motor2.setSpeed(i);  
      delay(10);
    }
    //
    Serial.println("phase 4 - motor1 forward-full, motor2 reverse-stop");
    motor1.run(FORWARD);
    motor2.run(BACKWARD);
    for (i=0; i<255; i++) {
      motor1.setSpeed(i);  
      motor2.setSpeed(255-i);  
      delay(10);
    }
    //
    loopCnt += 1;
}
