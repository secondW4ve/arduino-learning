#include <DHT11.h>

const int sensorPin = 2;
const float baseTemp = 30.0;

DHT11 dht11(sensorPin);

struct LedsState {
  int first;
  int second;
  int third;
};

void switchLeds(LedsState);

void setup() {
  Serial.begin(9600);

  for (int pinNbr = 4; pinNbr < 7; pinNbr++) {
    pinMode(pinNbr, OUTPUT);
    digitalWrite(pinNbr, LOW);
  }
}

void loop() {
  int temp = dht11.readTemperature();

  if (temp == DHT11::ERROR_CHECKSUM || temp == DHT11::ERROR_TIMEOUT) {
    // Print error message based on the error code.
    Serial.println(DHT11::getErrorString(temp));
  } else {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" Celc");

    if (temp <= baseTemp) {
      switchLeds({.first = 0, .second = 0, .third = 0});
    } else if (temp >= baseTemp + 1 && temp < baseTemp + 2) {
      switchLeds({.first = 1, .second = 0, .third = 0});
    } else if (temp >= baseTemp + 2 && temp < baseTemp + 3) {
      switchLeds({.first = 1, .second = 1, .third = 0});
    } else {
      switchLeds({.first = 1, .second = 1, .third = 1});
    }

    delay(1);
  }
}


void switchLeds(LedsState state) {
  digitalWrite(4, state.first);
  digitalWrite(5, state.second);
  digitalWrite(6, state.third);
}
