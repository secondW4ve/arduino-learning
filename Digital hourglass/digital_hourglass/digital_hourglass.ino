const int switchPin = 8;

unsigned long previousTime = 0;

int switchState = 0;
int prevSwitchState = 0;

int ledPin = 2;

int interval = 3 * 1000;

void setup() {
  for (int i = 2; i < 8; i++) {
    pinMode(i, OUTPUT);
  }

  pinMode(switchPin, INPUT);
}

void loop() {
  unsigned long currentWorkTime = millis();

  if (currentWorkTime - previousTime > interval) {
    previousTime = currentWorkTime;

    digitalWrite(ledPin, HIGH);
    ledPin++;

    if (ledPin == 8) {
      tone(12, 5000);

    }
  }

  switchState = digitalRead(switchPin);

  if (switchState != prevSwitchState) {
    for (int i = 2; i < 8; i++) {
      digitalWrite(i, LOW);
      noTone(12);
    }

    ledPin = 2;
    previousTime = currentWorkTime;
  }

  prevSwitchState = switchState;
}
