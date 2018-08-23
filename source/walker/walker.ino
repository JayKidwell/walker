
#include <Servo.h>
#include <String.h>
#include "Constants.h"
#include "Body.h"

// Define User Types below here or use a .h file
//
Body walker;


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//

// The setup() function runs once each time the micro-controller starts
void setup()
{
  walker = Body();
  walker.setup( "walker1" );
  delay( 1000 );
  
  walker.sit();
  delay( 1000 );
  
  walker.stand();
  delay( 1000 );
  
  walker.sit();
}

// Add the main program code into the continuous loop() function
void loop()
{


}
