#include "SevSeg.h"
SevSeg sevseg; 

byte VRXPin = A0;
byte VRYPin = A1;
byte SW_PIN = A2;
int counter;

int centerX = -1;
int centerY = -1;
int maxX = 0;
int minX = 1024;
int maxY = 0;
int minY = 1024;

bool settingTimer = true;
bool displayState = true;

unsigned long lastUpdateTime = 0;
unsigned long lastInputCheckTime = 0;

const unsigned long blinkInterval = 500;

int minutes1 = 0;
int minutes2 = 0;
int seconds1 = 0;
int seconds2 = 0;
int selectedNumber = 0;

const unsigned long dotBlinkInterval = 500;
const unsigned long debounceInterval = 300;

unsigned long lastClockUpdate = 0;
unsigned long lastDotBlinkUpdate = 0;

bool dotClockState = true;

bool editingMode = false;

byte digits[] = {
  0x3F, // '0'
  0x06, // '1'
  0x5B, // '2'
  0x4F, // '3'
  0x66, // '4'
  0x6D, // '5'
  0x7D, // '6'
  0x07, // '7'
  0x7F, // '8'
  0x6F // '9'
};

byte digitsWithDot[] = {
  0x3F | 0x80, // '0' with dot
  0x06 | 0x80, // '1' with dot
  0x5B | 0x80, // '2' with dot
  0x4F | 0x80, // '3' with dot
  0x66 | 0x80, // '4' with dot
  0x6D | 0x80, // '5' with dot
  0x7D | 0x80, // '6' with dot
  0x07 | 0x80, // '7' with dot
  0x7F | 0x80, // '8' with dot
  0x6F | 0x80  // '9' with dot
};

unsigned long currentIndex = 0;

void setup(){
  byte numDigits = 4;
  byte digitPins[] = {10, 11, 12, 13};
  byte segmentPins[] = {9, 2, 3, 5, 6, 8, 7, 4};
  bool resistorsOnSegments = true;

  byte hardwareConfig = COMMON_CATHODE; 
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);
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

// 0x00 - off
// 0x01 - segment A
// 0x02 - segment B
// 0x03 - segment A + segment B
// 0x04 - segment C
// 0x05 - segment A + segment C
// 0x06 - segment B + segment C
// 0x07 - segment A + segment B + segment C
// 0x08 - segment D

void loop() {
  unsigned long now = millis();
  sevseg.setSegmentsDigit(0, digits[currentIndex]);
  sevseg.setSegmentsDigit(2, digitsWithDot[currentIndex]);
  sevseg.refreshDisplay();
  if (now - lastUpdateTime >= 1000) {
    lastUpdateTime = now;
    currentIndex = currentIndex == 9 ? 0 : currentIndex + 1;
  }
 // Array of hex values for digits 0-9 with the dot included
  // if (settingTimer) {
  //    if (now - lastBlinkTime >= blinkInterval) {
  //     lastBlinkTime = now;
  //     displayState = !displayState;
  //     if (displayState) {
  //       int timer = 1000 * minutes1 + 100 * minutes2 + 10 * seconds1 + seconds2;
  //       sevseg.setNumber(timer, 2);
  //       sevseg.setSegmentsDigit(0, 1);
  //     } else {
  //       sevseg.blank();
  //     }
  //    }

  //    if (now - lastInputCheckTime >= debounceInterval) {
  //     lastInputCheckTime = now;
  //     int posX = analogRead(VRXPin);
  //     int posY = analogRead(VRYPin);
  //    }
  // }
//  static bool lastButtonState = HIGH;
//  int btnState = digitalRead(SW_PIN);
//  if (btnState == LOW && lastButtonState == HIGH) {
//    editingMode = !editingMode;
//    lastButtonState = LOW;
//    delay(50);
//    if (!editingMode) {
//      unsigned long now = millis();
//      lastClockUpdate = now;
//      lastDotBlinkUpdate = now;
//    }
//  } else if (btnState == HIGH) {
//    lastButtonState = HIGH;
//  }
//
//  unsigned long currentTime = millis();
//
//  if (editingMode) {
//    if (currentTime - lastBlinkTime >= blinkInterval) {
//      lastBlinkTime = currentTime;
//      displayState = !displayState;
//
//      if (displayState) {
//        sevseg.setNumber(counter);
//      } else {
//        sevseg.blank();
//      }
//    }
//
//    if (currentTime - lastInputCheckTime >= debounceInterval) {
//      lastInputCheckTime = currentTime;
//      int xPos = analogRead(VRXPin);
//      if (xPos > centerX + 100) {
//        counter = counter == 0 ? 9 : counter - 1;
//      } else if (xPos < centerX - 100) {
//        counter = counter == 9 ? 0 : counter + 1;
//      }
//    }
//  } else {
//    if (currentTime - lastClockUpdate >= 1000) {
//      counter = counter == 9 ? 0 : counter + 1;
//      sevseg.setNumber(counter);
//      lastClockUpdate = millis();
//    }
//    if (currentTime - lastDotBlinkUpdate >= dotBlinkInterval) {
//      sevseg.setNumber(counter, dotClockState ? false : true);
//      dotClockState = !dotClockState;
//      lastDotBlinkUpdate = millis();
//    }
//  }
//  sevseg.refreshDisplay();
}
