#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int switchPin = 6;
int switchState = 0;
int prevSwitchState = 0;
int reply;

String answers[8] = {
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday'",
  "Sunday",
  "?"
};

void setup() {
  lcd.begin(16, 2);
  pinMode(switchPin, INPUT);

  lcd.print("Day guesser");
}

void loop() {
  switchState = digitalRead(switchPin);

  if (switchState != prevSwitchState) {
    if (switchState == LOW) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Is it");
      lcd.setCursor(0, 1);
      String answer = answers[random(8)];
      lcd.print(answer);
      lcd.setCursor(answer.length(), 1);
      lcd.print("?");
    }
  }

  prevSwitchState = switchState;
}
