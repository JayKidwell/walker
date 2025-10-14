

// loop counter for debug monitor
int loopCnt;
//
// -- Servo connected p pulse-width-modulated single (longer width = brighter)
int ServoPin = 11;
//
// -- force resistor on pin 0
int fsrAnalogPin = 0;
//
// -- servo min and max values
int forceMin = 0;
int forceMax = 1023;
//
// -- servo min and max values
int serverMin = 40;
int serverMax = 240;
//
// -- noise filter depth
int filterWeight = 5;
int filteredInput = 0;
//
// -- Arduino calls this setup method once when the device initializes
void setup() {
  //
  // -- initialize the serial port to display data on the computer monitor
  Serial.begin(9600);
  while (!Serial) {
    ; // -- wait for serial port to connect. Needed for native USB port only
  }
  //
  // -- send message to debug monitor
  Serial.println("setup");
  //
  // -- setup the LED pin as an output pin  
  pinMode(ServoPin, OUTPUT);
}
//
// -- Arduino calls loop repeatedly
void loop() {
  //
  // -- begin a pass through the loop
  // Serial.print(", loop:" + String( loopCnt ));
  //
  // -- read in analog value from force resister and output do monitor
  int forceInput = analogRead(fsrAnalogPin);
  Serial.print(forceInput);
  //
  // -- noise filter
  filteredInput = ((filteredInput * filterWeight) + forceInput)/(filterWeight+1);
  Serial.print(",");
  Serial.print(filteredInput);
  //
  // -- weight the input
  //long filteredInput = ( (long)filteredInput * (long)filteredInput ) / (long)1048576;
  //
  // -- convert reading 0-1023 to output 0-255, and write servo position
  int ServoPosition;
  ServoPosition = map((int)filteredInput, forceMin, forceMax, serverMin, serverMax);
  ServoPosition = constrain( ServoPosition, serverMin, serverMax );
  
  Serial.print(",");
  Serial.print(ServoPosition);
  //
  // LED gets brighter the harder you press
  analogWrite(ServoPin, ServoPosition);
  //
  // -- count the loop pass
  loopCnt += 1;
  //
  // -- monitor new line
  Serial.println("");
}
