#include <Stepper.h>

Stepper myStepper(100, 8, 10, 9, 11);

const int potPin = A0;
const int onOffSwitchPin = 4;
const int directionSwitchPin = 2;

int onOffState = 0;
int prevOnOffState = 0;
int directionState = 0;
int prevDirectionState = 0;

int motorEnabled = 0;
int motorSpeed = 0;
int motorDirection = 1;

void setup() {
  pinMode(onOffSwitchPin, INPUT);
  pinMode(directionSwitchPin, INPUT);

  myStepper.setSpeed(30);
  Serial.begin(9600);
}

void loop() {
  onOffState = digitalRead(onOffSwitchPin);
  delay(1);
  directionState = digitalRead(directionSwitchPin);

  int potValue = analogRead(potPin);

  if (onOffState != prevOnOffState && onOffState == HIGH) {
    motorEnabled = !motorEnabled;
  }

  if (directionState != prevDirectionState && directionState == HIGH) {
    motorDirection = !motorDirection;
  }

  Serial.print("on/off: ");
  Serial.print(onOffState);
  Serial.print(", direction: ");
  Serial.print(directionState);
  Serial.print(", Pot: ");
  Serial.print(motorSpeed);
  Serial.print(", Motor enabled: ");
  Serial.print(motorEnabled);
  Serial.print(", Motor direction: ");
  Serial.println(motorDirection);

  // myStepper.setSpeed(map(motorSpeed, 0, 1023, 0, 10));
  motorSpeed = map(potValue, 0, 1023, 0, 100);

  if (motorEnabled == 1) {
    if (motorSpeed > 0) {
      myStepper.setSpeed(motorSpeed);
    }
    if (motorDirection == 1) {
      myStepper.step(2);
    } else {
      myStepper.step(-2);
    }
    delay(2);
  }

  prevDirectionState = directionState;
  prevOnOffState = onOffState;
}
