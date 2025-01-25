byte sensorPin = A0;
byte buzzerPin = 9;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting calibration..."));
  Serial.println(F(""))
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  int waterLevel = analogRead(sensorPin);

  Serial.print(F("Water level: "));
  Serial.println(waterLevel);
  if (waterLevel >= 670) {
    tone(buzzerPin, 1000, 1000);
    delay(500);
    noTone(buzzerPin);
  }
  delay(100);
}
