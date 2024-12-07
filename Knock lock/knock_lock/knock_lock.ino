#include <Servo.h>

Servo myServo;

const int light = A0;
const int switchPin = 2;
const int yellowLed = 3;
const int greenLed = 4;
const int redLed = 5;

int lightMax = 0;
int lightMin = 1023;

int lightVal;
int switchVal;

boolean locked = false;
int numberOfMoves = 0;

void setup() {
  myServo.attach(9);
  pinMode(yellowLed,  OUTPUT);
  pinMode(greenLed,  OUTPUT);
  pinMode(redLed,  OUTPUT);
  pinMode(switchPin, INPUT);
  Serial.begin(9600);

  digitalWrite(yellowLed, HIGH);
  Serial.println("Start calibrating...");
  while (millis() < 5000) {
    lightVal = analogRead(light);
    if (lightMax < lightVal) {
      lightMax = lightVal;
    }

    if (lightMin > lightVal) {
      lightMin = lightVal;
    }
  }
  Serial.println("Finish calibrating. Max and min values are:");
  Serial.print("Min: ");
  Serial.print(lightMin);
  Serial.print(" Max: ");
  Serial.println(lightMax);
  digitalWrite(yellowLed, LOW);

  digitalWrite(greenLed, HIGH);
  myServo.write(0);
  Serial.println("The box is unlocked!");
}

void loop() {
  if (!locked) {
    switchVal = digitalRead(switchPin);
    if (switchVal == HIGH) {
      locked = true;
      digitalWrite(greenLed, LOW);
      digitalWrite(redLed, HIGH);
      myServo.write(90);
      Serial.println("The box is locked!");
      delay(1000);
    }
  }


  if (locked) {
    lightVal = analogRead(light);
    Serial.print("Light val: ");
    Serial.println(lightVal);
    if (numberOfMoves < 3 && lightVal < lightMax - 10) {
      if (checkForMove(lightVal) == true) {
        numberOfMoves++;
      }
      Serial.print(3 - numberOfMoves);
      Serial.println(" moves left to open");
    }

    if (numberOfMoves >= 3) {
      locked = false;

      myServo.write(0);
      delay(20);
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, HIGH);
      Serial.println("The box is unlocked!");
      numberOfMoves = 0;
    }
  }
}

boolean checkForMove(int val) {
  if (val < lightMax - 50) {
    digitalWrite(yellowLed, HIGH);
    delay(50);
    digitalWrite(yellowLed, LOW);
    Serial.print("Move value: ");
    Serial.println(val);
    return true;
  } else {
    Serial.print("Bad move value: ");
    Serial.println(val);
    return false;
  }
}