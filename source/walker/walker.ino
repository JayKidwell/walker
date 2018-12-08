
#include <Servo.h>
#include <String.h>
#include "Constants.h"
#include "Body.h"
// Define User Types below here or use a .h file
//
Body walkerBody;
//
// -- loop counter to simulate motion during development
int loopCnt;
//
// The setup() function runs once each time the micro-controller starts
void setup(){
    Serial.begin( 9600 );
    Serial.println("123412341243");
    walkerBody = Body();
    walkerBody.setup( "walker1" );
    //
    // -- initialize
    loopCnt = 0;
}

// Add the main program code into the continuous loop() function
void loop(){
  if (loopCnt == 0) {
    walkerBody.sit();
  } else if ( loopCnt == 1000 ) {
    walkerBody.stand();
  } else if ( loopCnt == 2000 ) {
    loopCnt = -1;
  }
  walkerBody.loop();
  loopCnt++;
}
