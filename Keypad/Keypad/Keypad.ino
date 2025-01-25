#include <Keypad.h>
#include <LedControl.h>

#define ROWS 4
#define COLS 4

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte gLed[8] = {
  0b00000000,
  0b00111110,
  0b01000000,
  0b01000000,
  0b01001110,
  0b01000010,
  0b00111110,
  0b00000000
};
byte oLed[8] = {
  0b00000000,
  0b01111110,
  0b01000010,
  0b01000010,
  0b01000010,
  0b01000010,
  0b01111110,
  0b00000000
};
byte exclamationLed[8] = {
  0b00000000,
  0b00010000,
  0b00010000,
  0b00010000,
  0b00010000,
  0b00000000,
  0b00010000,
  0b00000000
};

byte colPins[ROWS] = {5, 4, 3, 2};
byte rowPins[COLS] = {9, 8, 7, 6};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LedControl lc = LedControl(11, 13, 10, 1);

void setup() {
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.setIntensity(0, 6);
  lc.clearDisplay(0);
  delay(50);

  displaySymbol(gLed);
  delay(500);
  displaySymbol(oLed);
  delay(500);
  displaySymbol(exclamationLed);
  delay(500);

  lc.clearDisplay(0);
}

void loop() {
  // Display a smiley face pattern
  char key = keypad.getKey();

  if (key) {
    byte rowsToDisplay[8];
    getRowsByKey(key, rowsToDisplay);
    displaySymbol(rowsToDisplay);
    delay(50);
  }
}

void displaySymbol(byte rows[8]) {
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, rows[row]);
  }
}

void getRowsByKey(char key, byte (&rows)[8]) {
  switch (key)  {
    case '1':
      rows[0] = 0b00000000;
      rows[1] = 0b00000100;
      rows[2] = 0b00001100;
      rows[3] = 0b00010100;
      rows[4] = 0b00000100;
      rows[5] = 0b00000100;
      rows[6] = 0b00000100;
      rows[7] = 0b00000000;
      break;
    case '2':
      rows[0] = 0b00000000;
      rows[1] = 0b00011100;
      rows[2] = 0b00100100;
      rows[3] = 0b00000100;
      rows[4] = 0b00001000;
      rows[5] = 0b00010000;
      rows[6] = 0b00111100;
      rows[7] = 0b00000000;
      break;
    case '3':
      rows[0] = 0b00000000;
      rows[1] = 0b00011100;
      rows[2] = 0b00100100;
      rows[3] = 0b00001000;
      rows[4] = 0b00001000;
      rows[5] = 0b00100100;
      rows[6] = 0b00011100;
      rows[7] = 0b00000000;
      break;
    case '4':
      rows[0] = 0b00000000;
      rows[1] = 0b00000100;
      rows[2] = 0b00001100;
      rows[3] = 0b00010100;
      rows[4] = 0b00111110;
      rows[5] = 0b00000100;
      rows[6] = 0b00000100;
      rows[7] = 0b00000000;
      break;
    case '5':
      rows[0] = 0b00000000;
      rows[1] = 0b01111110;
      rows[2] = 0b01000000;
      rows[3] = 0b01111100;
      rows[4] = 0b00000010;
      rows[5] = 0b01000010;
      rows[6] = 0b00111100;
      rows[7] = 0b00000000;
      break;
    case '6':
      rows[0] = 0b00000000;
      rows[1] = 0b00111110;
      rows[2] = 0b01000000;
      rows[3] = 0b01111100;
      rows[4] = 0b01000010;
      rows[5] = 0b01000010;
      rows[6] = 0b01111100;
      rows[7] = 0b00000000;
      break;
    case '7':
      rows[0] = 0b00000000;
      rows[1] = 0b01111110;
      rows[2] = 0b00000100;
      rows[3] = 0b00001000;
      rows[4] = 0b00010000;
      rows[5] = 0b00100000;
      rows[6] = 0b01000000;
      rows[7] = 0b00000000;
      break;
    case '8':
      rows[0] = 0b00000000;
      rows[1] = 0b01111110;
      rows[2] = 0b01000010;
      rows[3] = 0b01111110;
      rows[4] = 0b01000010;
      rows[5] = 0b01000010;
      rows[6] = 0b01111110;
      rows[7] = 0b00000000;
      break;
    case '9':
      rows[0] = 0b00000000;
      rows[1] = 0b01111110;
      rows[2] = 0b01000010;
      rows[3] = 0b01111110;
      rows[4] = 0b00000010;
      rows[5] = 0b00000010;
      rows[6] = 0b01111100;
      rows[7] = 0b00000000;
      break;
    case '*':
      rows[0] = 0b00000000;
      rows[1] = 0b01000010;
      rows[2] = 0b00100100;
      rows[3] = 0b01111110;
      rows[4] = 0b00011000;
      rows[5] = 0b00100100;
      rows[6] = 0b01000010;
      rows[7] = 0b00000000;
      break;
    case '0':
      rows[0] = 0b00000000;
      rows[1] = 0b00111100;
      rows[2] = 0b00100100;
      rows[3] = 0b00100100;
      rows[4] = 0b00100100;
      rows[5] = 0b00100100;
      rows[6] = 0b00111100;
      rows[7] = 0b00000000;
      break;
    case '#':
      rows[0] = 0b00000000;
      rows[1] = 0b00100100;
      rows[2] = 0b01111110;
      rows[3] = 0b00100100;
      rows[4] = 0b00100100;
      rows[5] = 0b01111110;
      rows[6] = 0b00100100;
      rows[7] = 0b00000000;
      break;
    case 'A':
      rows[0] = 0b00000000;
      rows[1] = 0b00011000;
      rows[2] = 0b00100100;
      rows[3] = 0b01000010;
      rows[4] = 0b01111110;
      rows[5] = 0b01000010;
      rows[6] = 0b01000010;
      rows[7] = 0b00000000;
      break;
    case 'B':
      rows[0] = 0b00000000;
      rows[1] = 0b01110000;
      rows[2] = 0b01001000;
      rows[3] = 0b01111100;
      rows[4] = 0b01000100;
      rows[5] = 0b01000100;
      rows[6] = 0b01111000;
      rows[7] = 0b00000000;
      break;
    case 'C':
      rows[0] = 0b00000000;
      rows[1] = 0b00011110;
      rows[2] = 0b00100000;
      rows[3] = 0b01000000;
      rows[4] = 0b01000000;
      rows[5] = 0b00100000;
      rows[6] = 0b00011110;
      rows[7] = 0b00000000;
      break;
    case 'D':
      rows[0] = 0b00000000;
      rows[1] = 0b01111000;
      rows[2] = 0b01000100;
      rows[3] = 0b01000010;
      rows[4] = 0b01000010;
      rows[5] = 0b01000100;
      rows[6] = 0b01111000;
      rows[7] = 0b00000000;
      break;
    default:
      rows[0] = 0b00000000;
      rows[1] = 0b00010000;
      rows[2] = 0b00010000;
      rows[3] = 0b00010000;
      rows[4] = 0b00010000;
      rows[5] = 0b00010000;
      rows[6] = 0b00000000;
      rows[7] = 0b00010000;
      break;
  }
}
