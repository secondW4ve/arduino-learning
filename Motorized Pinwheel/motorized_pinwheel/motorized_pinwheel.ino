#include <Stepper.h>

const int switchPin = 2;
int switchState = 0;

const int stepsPerRevolution = 2048;  // NUmber of steps revolution.

// initialize the stepper library on pins 8 through 11:
// Make sure the pin siquence should be 8,10,9,11 for Anticlockwise.

Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  pinMode(switchPin, INPUT);
  // set the speed at rpm:
  myStepper.setSpeed(10);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  switchState = digitalRead(switchPin);

  if (switchState == 1) {
    myStepper.step(48);
    Serial.println("After step");
    delay(2);
  } else {
    Serial.println("Not pressed");
  }
}