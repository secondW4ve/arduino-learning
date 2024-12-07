const int ledPin = 13;
const int pirPin = 2;
int pirState = 0;
int val = 0; // variable for reading the pin status
const int minimummMiliSecsLowForInactive = 500;
long unsigned int timeLow;
boolean takeLowTime;

const int calibrationTimeSecs = 60;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(9600);

  Serial.print("calibrating sensor ");
  int ledStatus = LOW;
  for(int i = 0; i < calibrationTimeSecs; i++){
    Serial.print(".");
    digitalWrite(ledPin, ledStatus);
    ledStatus = !ledStatus;
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

void loop() {
  val = digitalRead(pirPin);
  Serial.print(val);
  if (val == HIGH) {
    Serial.println("Motion.");
    digitalWrite(ledPin, HIGH);
    if (pirState) {
      pirState = 0;
      delay(50);
    }
    takeLowTime = true;
  } else {
    Serial.println("No motion");
    digitalWrite(ledPin, LOW);
    if (takeLowTime) {
      timeLow = millis();
      takeLowTime = false;
    }

    if (!pirState && millis()-timeLow > minimummMiliSecsLowForInactive) {
      pirState = true;
      Serial.println("Motion ended");
      delay(50);
    }
  }
}
