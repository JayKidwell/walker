/**********************************************************************
* Filename    : Blink
* Description : Make an led blinking.
* Auther      : www.freenove.com
* Modification: 2020/07/11
**********************************************************************/
#define PIN_LED  15
#define PIN_BUTTON  13
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin PIN_LED as an output.
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
}

// the loop function runs over and over again forever
void loop() {
  while (digitalRead( PIN_BUTTON )==HIGH) {}
  toggleLed( PIN_LED );
  delay( 20 );
  while (digitalRead( PIN_BUTTON )==LOW) {}
  delay( 20 );
}
//
void toggleLed( int pin ) {
  digitalWrite( pin, !digitalRead( pin ));
}
