
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
    Serial.begin( SerialBaudRate );
    Serial.println("Walker setup...");
    walkerBody = Body();
    walkerBody.setup( "walker1" );
    //
    // -- initialize
    loopCnt = 0;
    Serial.println("exit setup");
}
//
// -- send instructions to body and call sub objects
void loop(){
    if (loopCnt == 0) {
        Serial.println("loopCnt=0");
        walkerBody.sit();
    } else if ( loopCnt == 1000 ) {
        walkerBody.stand();
    } else if ( loopCnt == 2000 ) {
        loopCnt = -1;
    }
    walkerBody.loop();
    loopCnt++;
    delay(1000);
}
