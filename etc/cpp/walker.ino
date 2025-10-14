

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <String.h>
#include "Constants.h"
#include "Body.h"
//
// x Consider integrating async servo controller
// x https://github.com/netlabtoolkit/VarSpeedServo
// x no, code doesnt build, might be based on older arduino libraries
//
// Consider PCA9685 PWM Servo Driver Library
// http://wiki.sunfounder.cc/index.php?title=PCA9685_16_Channel_12_Bit_PWM_Servo_Driver
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
// -- driver to pcs9685 16-channel pulse width modulation (pwm) board
Adafruit_PWMServoDriver pwm;
//
// The setup() function runs once each time the micro-controller starts
void setup(){
    //
    // -- setup serial monitor
    Serial.begin( SerialBaudRate );
    //
    // -- setup pwm, this is the maximum PWM frequency, servos use 60Hz, use the default address 0x40
    Serial.println("pcs9685 setup ...");
    pwm = Adafruit_PWMServoDriver()
    pwm.begin();
    pwm.setPWMFreq( 60 );
    // recommended, no explaination
    delay(10);

    //
    // -- setup i2c wire library
    // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
    // some i2c devices dont like this so much so if you're sharing the bus, watch out for this!
    Serial.println("i2c setup...");
    Wire.setClock(400000);
    //
    Serial.println("Walker setup...");
    walkerBody = Body();
    walkerBody.setup( "walker1", pwm );
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
