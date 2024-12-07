#include "SevSeg.h"
SevSeg sevseg; 

byte buttonPin = 12;
byte VRXPin = A0;
byte VRYPin = A1;
byte SW_PIN = 10;
int counter;

int centerX = -1;
int centerY = -1;
int maxX = 0;
int minX = 1024;
int maxY = 0;
int minY = 1024;

bool displayState = true;

unsigned long lastBlinkTime = 0;
unsigned long lastInputCheckTime = 0;

const unsigned long blinkInterval = 500;
const unsigned long debounceInterval = 300;

bool editingMode = false;

void setup(){
  byte numDigits = 1;
  byte digitPins[] = {};
  byte segmentPins[] = {6, 5, 2, 3, 4, 7, 8, 9};
  bool resistorsOnSegments = true;

  byte hardwareConfig = COMMON_CATHODE; 
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(SW_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println(F("Now don't touch joystick. Reading normalState..."));
  delay(3000);
  centerX = analogRead(VRXPin);
  centerY = analogRead(VRYPin);
  Serial.print(F("Normal state read: "));
  Serial.print(F("center X: "));
  Serial.print(centerX);
  Serial.print(F(", center Y: "));
  Serial.print(centerY);
  Serial.println();
}

void loop() {
  static bool lastButtonState = HIGH;
  int btnState = digitalRead(SW_PIN);
  if (btnState == LOW && lastButtonState == HIGH) {
    editingMode = !editingMode;
    lastButtonState = LOW;
    delay(50);
  } else if (btnState == HIGH) {
    lastButtonState = HIGH;
  }

  unsigned long currentTime = millis();

  if (editingMode) {
    if (currentTime - lastBlinkTime >= blinkInterval) {
      lastBlinkTime = currentTime;
      displayState = !displayState;

      if (displayState) {
        sevseg.setNumber(counter);
      } else {
        sevseg.blank();
      }
    }

    if (currentTime - lastInputCheckTime >= debounceInterval) {
      lastInputCheckTime = currentTime;
      int xPos = analogRead(VRXPin);
      if (xPos > centerX + 100) {
        counter = counter == 0 ? 9 : counter - 1;
      } else if (xPos < centerX - 100) {
        counter = counter == 9 ? 0 : counter + 1;
      }
    }
  } else {
    sevseg.setNumber(counter);
  }
  sevseg.refreshDisplay();
}