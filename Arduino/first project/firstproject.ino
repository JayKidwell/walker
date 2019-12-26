// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       first project.ino
    Created:	8/3/2018 11:05:34 PM
    Author:     LAPTOP-FPUPHV09\jay
*/

// Define User Types below here or use a .h file
//


// Define Function Prototypes that use User Types below here or use a .h file
//


// Define Functions below here or use other .ino or cpp files
//


// the setup function runs once when you press reset or power the board
void setup() {
	// initialize digital pin LED_BUILTIN as an output.
	pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
	dot();
	dot();
	dot();
	dash();
	dash();
	dash();
	dot();
	dot();
	dot();
	delay(2000);
}
//
//
//
void dot() {
	delay(250);                       // wait for a second
	digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
	delay(250);                       // wait for a second
	digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
}
// 
//
//
void dash() {
	delay(500);                       // wait for a second
	digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
	delay(500);                       // wait for a second
	digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
}
