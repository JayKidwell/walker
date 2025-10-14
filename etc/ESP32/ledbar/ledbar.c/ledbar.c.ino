/**********************************************************************
* Filename    : Blink
* Description : Make an led blinking.
* Auther      : www.freenove.com
* Modification: 2020/07/11
**********************************************************************/
#define PIN_LED  15
#define PIN_BUTTON  13
byte ledBar[] = {12,2,0,4,5,18,19,21,22,23};
byte ledOnPtr = 0;
//
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin PIN_LED as an output.
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  // ledbar
  for (int pin=0; pin<sizeof(ledBar); pin++ ) {
    pinMode( ledBar[ pin ], OUTPUT );
  }
}

// the loop function runs over and over again forever
void loop() {
  for (int pin=0; pin<sizeof(ledBar); pin++ ) {
    if( pin==ledOnPtr ) {
      setLed( ledBar[pin] );
    } else {
      clearLed( ledBar[pin] );
    }
  }
  delay( 100 );
  ledOnPtr++;
  if( ledOnPtr>=sizeof(ledBar) ) { ledOnPtr=0; }
  //while (digitalRead( PIN_BUTTON )==HIGH) {}
  //toggleLed( PIN_LED );
  //delay( 20 );
  //while (digitalRead( PIN_BUTTON )==LOW) {}
  //delay( 20 );
}
//
void setLed( int pin ) {
  digitalWrite( pin, HIGH);
}
//
void clearLed( int pin ) {
  digitalWrite( pin, LOW);
}
//
void toggleLed( int pin ) {
  digitalWrite( pin, !digitalRead( pin ));
}
