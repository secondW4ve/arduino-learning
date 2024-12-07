#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define RST_PIN           9
#define SS_PIN            10

#define SECTORS           16
#define BLOCKS_PER_SECOR  4
#define BYTES_PER_BLOCK   16

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

MFRC522 rfidReader(SS_PIN, RST_PIN);
typedef struct {
    byte size;
    byte uidByte[10];
    byte		sak;
} Uid;
byte keyA[MFRC522::MF_KEY_SIZE] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

Uid masterUid;
byte masterCardData[16][4][16];

void setup() {
  Serial.begin(9600);
  SPI.begin();

  rfidReader.PCD_Init();
  delay(4);

  rfidReader.PCD_DumpVersionToSerial();

  lcd.begin(16, 2);
  printDefaultMessage();
}

void loop() {
	if (!rfidReader.PICC_IsNewCardPresent()) {
		return;
	}
  if (!rfidReader.PICC_ReadCardSerial()) {
    return;
  }

  for (int i = 0; i < rfidReader.uid.size; i++) {
    masterUid.uidByte[i] = rfidReader.uid.uidByte[i];
  }
  masterUid.size = rfidReader.uid.size;
  masterUid.sak = rfidReader.uid.sak;

  rfidReader.PICC_DumpToSerial(&(rfidReader.uid));

  printPICCType();

  readCardData();
}

void readCardData() {
  for (int i = 0; i < SECTORS; i++) {
    for (int j = 0; j < BLOCKS_PER_SECOR; j++) {
      for (int b = 0; b < BYTES_PER_BLOCK; b++) {
        MFRC522::StatusCode authCode = rfidReader.PCD_Authenticate(
          MFRC522::PICC_CMD_MF_AUTH_KEY_A,
          j,
          &keyA,
          &(rfidReader.uid)  
        );
      }
    }
  }
}

void printPICCType() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card type:");
  lcd.setCursor(0, 1);
  int piccType = rfidReader.PICC_GetType(rfidReader.uid.sak);
  lcd.print(rfidReader.PICC_GetTypeName(piccType));
  delay(2000);
  printDefaultMessage();
}

void printDefaultMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready to scan:");
}

