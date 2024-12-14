#include "SevSeg.h"
SevSeg sevseg; 

byte VRXPin = A0;
byte VRYPin = A1;
byte SW_PIN = A2;
int counter;

int centerX = -1;
int centerY = -1;
int maxX = 1023;
int minX = 0;
int maxY = 1023;
int minY = 0;

bool settingTimer = true;
bool displayState = true;

unsigned long lastUpdateTime = 0;
unsigned long lastBlinkTime = 0;
unsigned long lastInputCheckTime = 0;

const unsigned long blinkInterval = 500;

int timer[] = { 0, 0, 0, 0 };
// int minutes1 = 0;
// int minutes2 = 0;
// int seconds1 = 0;
// int seconds2 = 0;
int selectedNumber = 0;

const unsigned long dotBlinkInterval = 500;
const unsigned long debounceInterval = 300;

unsigned long lastClockUpdate = 0;
unsigned long lastDotBlinkUpdate = 0;
bool dotClockState = true;

bool boom = false;

byte digitOff = 0x00;
byte digitOffDotOn = 0x80;
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
  0x6F  // '9'
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
  for (int i = 0; i < 4; i++) {
    sevseg.setSegmentsDigit(timer[i], digits[0]);
  }
  sevseg.refreshDisplay();
}

void loop() {
  if (boom) {
    sevseg.setSegmentsDigit(0, digits[8]);
    sevseg.setSegmentsDigit(1, digits[8]);
    sevseg.setSegmentsDigit(2, digits[8]);
    sevseg.setSegmentsDigit(3, digits[8]);
    sevseg.refreshDisplay();
    return;
  }
  unsigned long now = millis();
  int btnState = digitalRead(SW_PIN);
  if (btnState == LOW) {
    settingTimer = false;
  }

  if (settingTimer) {
    if (now - lastBlinkTime >= blinkInterval) {
      lastBlinkTime = now;
      displayState = !displayState;
      if (displayState) {
        sevseg.setSegmentsDigit(selectedNumber, selectedNumber == 1 ? digitsWithDot[timer[selectedNumber]] : digits[timer[selectedNumber]]);
      } else {
        sevseg.setSegmentsDigit(selectedNumber, selectedNumber == 1 ? digitOffDotOn : digitOff);
      }
    }

    if (now - lastInputCheckTime >= debounceInterval) {
      lastInputCheckTime = now;
      int posX = analogRead(VRXPin);
      int posY = analogRead(VRYPin);

      if (posX > centerX + 400) {
        timer[selectedNumber] = timer[selectedNumber] == 0 ? 0 : timer[selectedNumber] - 1;
      } else if (posX < centerX - 400) {
        timer[selectedNumber] = timer[selectedNumber] == 9 ? 0 : timer[selectedNumber] + 1;
      } else if (posY > centerY + 400) {
        selectedNumber = selectedNumber == 0 ? 3 : selectedNumber - 1;
      } else if (posY < centerY - 400) {
        selectedNumber = selectedNumber == 3 ? 0 : selectedNumber + 1;
      }
    }

    for (int i = 0; i < 4; i++) {
      if (i != selectedNumber) {
        sevseg.setSegmentsDigit(i, i == 1 ? digitsWithDot[timer[i]] : digits[timer[i]]);
      }
    }
  } else {
    if (now - lastClockUpdate > 1000) {
      lastClockUpdate = now;
      if (timer[0] == 0 && timer[1] == 0 && timer[2] == 0 && timer[3] == 0) {
        Serial.println(F("BOOOOOM!!!!"));
        sevseg.setSegmentsDigit(0, digits[8]);
        sevseg.setSegmentsDigit(1, digits[8]);
        sevseg.setSegmentsDigit(2, digits[8]);
        sevseg.setSegmentsDigit(3, digits[8]);
        boom = true;
        return;
      }
      timer[3] = timer[3] == 0 ? 9 : timer[3] - 1;
      if (timer[3] == 9) {
        timer[2] = timer[2] == 0 ? 5 : timer[2] - 1;

        if (timer[2] == 5) {
          timer[1] = timer[1] == 0 ? 9 : timer[1] - 1;

          if (timer[1] == 9 && timer[0] != 0) {
            timer[0] = timer[0] - 1;
          }
        }
      }

      for (int i = 0; i < 4; i++) {
        sevseg.setSegmentsDigit(i, digits[timer[i]]);
      }
    }

    if (now - lastDotBlinkUpdate > dotBlinkInterval) {
      lastDotBlinkUpdate = now;
      dotClockState = !dotClockState;

      if (!dotClockState) {
        sevseg.setSegmentsDigit(1, digits[timer[1]]);
      } else {
        sevseg.setSegmentsDigit(1, digitsWithDot[timer[1]]);
      }
    }
  }

  sevseg.refreshDisplay();
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
