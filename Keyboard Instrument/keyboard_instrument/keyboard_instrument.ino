int notes[] = {262, 294, 330, 349};

void setup() {
  Serial.begin(9600);
}

void loop() {
  int keyVal = analogRead(A0);
  // Serial.println(keyVal);

  if (keyVal <= 1023 && keyVal >= 1015) {
    tone(8, notes[0]);
  } else if (keyVal >= 990 && keyVal <= 1010) {
    tone(8, notes[1]);
  } else if (keyVal >= 505 && keyVal <= 530) {
    tone(8, notes[2]);
  } else if (keyVal >= 3 && keyVal <= 50) {
    tone(8, notes[3]);
  } else {
    Serial.println(keyVal);
    noTone(8);
  }
}
