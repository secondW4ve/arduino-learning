#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <stdlib.h>  // For malloc and free
#include <string.h>  // For memset

#define RST_PIN           9
#define SS_PIN            10

#define SECTORS           16
#define BLOCKS_PER_SECTOR  4
#define BYTES_PER_BLOCK   16
#define READ_BUFFER_SIZE  18

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

MFRC522 rfidReader(SS_PIN, RST_PIN);
typedef struct {
  byte size;
  byte uidByte[10];
  byte		sak;
} Uid;

typedef struct {
  bool zeroBlock;
  byte* blockData;
} Block;

MFRC522::MIFARE_Key keyA;
Uid masterUid;
Block masterCardData[64];
bool errorDuringReading = false;
bool errorDuringWriting = false;
int retries = 0;

bool masterDataRead = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();

  rfidReader.PCD_Init();
  delay(4);
  for (int i = 0; i < 6; i++) {
    keyA.keyByte[i] = 0xFF;
  }

  rfidReader.PCD_DumpVersionToSerial();

  lcd.begin(16, 2);
  printDefaultMessageToLCD();
}

void loop() {
	if (!rfidReader.PICC_IsNewCardPresent()) {
		return;
	}
  if (!rfidReader.PICC_ReadCardSerial()) {
    return;
  }

  if (!masterDataRead) {
    Serial.print(F("Free memory before reading: "));
    Serial.println(countFreeMemory());
    Serial.println(F("Reading master PICC..."));
    Serial.println(F("Start coping master uid..."));
    for (int i = 0; i < rfidReader.uid.size; i++) {
      masterUid.uidByte[i] = rfidReader.uid.uidByte[i];
    }
    masterUid.size = rfidReader.uid.size;
    Serial.println(masterUid.size);
    masterUid.sak = rfidReader.uid.sak;    
    printPICCTypeToLCD(masterUid.sak);
    printPICCUidInfoToSerial(masterUid);
    printPICCTypeToSerial(masterUid.sak);
    Serial.println(F("Reading master Uid done."));
    Serial.println(F("Start reading master card data..."));
    readCardData();
    // rfidReader.PICC_DumpToSerial(&rfidReader.uid);
    if (errorDuringReading) {
      Serial.println(F("Data wasn't read. Please try again!"));
      errorDuringReading = false;
      masterDataRead = false;
    } else {
      Serial.println(F("Reading master card data done."));
      Serial.println(F("==============Master card==============="));
      printPICCTypeToLCD(masterUid.sak);
      printPICCUidInfoToSerial(masterUid);
      printPICCTypeToSerial(masterUid.sak);
      printCardDataToSerial(masterCardData);
      masterDataRead = true;
      // rfidReader.PICC_DumpToSerial(&rfidReader.uid);
    }

    // Halt PICC and stop encryption
    rfidReader.PICC_HaltA();
    rfidReader.PCD_StopCrypto1();
    Serial.println(F("Going into 5 sec delay. Remove the tag"));
    delay(5000);
    Serial.println(F("Delay passed."));
    Serial.print(F("Free memory after reading: "));
    Serial.println(countFreeMemory());
  } else {
    errorDuringWriting = false;
    retries++;
    Serial.println(F("Reading slave"));
    // Serial.println(countFreeMemory());
    Serial.println(F("Start coping slave uid data..."));
    // // Serial.println(F("Reading slave PICC common data..."));
    // Uid slaveUid;
    // for (int i = 0; rfidReader.uid.size; i++) {
    //   slaveUid.uidByte[i] = rfidReader.uid.uidByte[i];
    // }
    // slaveUid.sak = rfidReader.uid.sak;
    // slaveUid.size = rfidReader.uid.size;
    // // Serial.println(F("Reading slave Uid is done."));
    Serial.println(F("==============Slave card================="));
    // // printPICCTypeToLCD(slaveUid.sak);
    // printPICCUidInfoToSerial(slaveUid);
    // printPICCTypeToSerial(slaveUid.sak);
    Serial.println(F("Starting to write data to slave card..."));
    // writeDataToCard();
    setNewUid();
    // Serial.print(F("Error during writing and retries: "));
    // Serial.print(errorDuringWriting);
    // Serial.println(retries);
    // if (errorDuringWriting && retries < 3) {
    //   Serial.println(F("Writing failed, will go to retry in 5 secs"));
    //   rfidReader.PICC_HaltA();
    //   rfidReader.PCD_StopCrypto1();
    //   Serial.println(F("Going into 5 sec delay. Remove the tag"));
    //   delay(5000);
    //   Serial.println(F("Delay passed."));
    // } else {
    //   if (retries == 3 && errorDuringWriting) {
    //     Serial.println(F("Writing failed. Clean memory. Do all process again"));
    //   } else {
    //     Serial.println(F("Writing has done!"));
      rfidReader.PICC_HaltA();
      rfidReader.PCD_StopCrypto1();
      masterDataRead = false;
      Serial.println(F("Going into 5 sec delay. Remove the tag"));
      delay(5000);
      Serial.println(F("Delay passed."));
      freeAllocatedMemory();
    // }
  }
}

void readCardData() {
  for (int i = 0; i < SECTORS * BLOCKS_PER_SECTOR; i++) {
    MFRC522::StatusCode statusCode;
    statusCode = rfidReader.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_B,
      i,
      &keyA,
      &(rfidReader.uid)  
    );

    if (statusCode != MFRC522::STATUS_OK) {
      Serial.print(F("Block "));
      Serial.print(i);
      Serial.print(F(" is not authenticated. Error code: "));
      Serial.println(rfidReader.GetStatusCodeName(statusCode));
      errorDuringReading = true;
      break;
    }

    byte buffer[READ_BUFFER_SIZE];
    byte size = sizeof(buffer);
    statusCode = rfidReader.MIFARE_Read(i, buffer, &size);
    if (statusCode != MFRC522::STATUS_OK) {
      Serial.print(F("Block "));
      Serial.print(i);
      Serial.print(F(" is not read. Error code: "));
      Serial.println(rfidReader.GetStatusCodeName(statusCode));
      errorDuringReading = true;
      break;
    }

    bool zeroBlock = true;
    for (int j = 0; j < BYTES_PER_BLOCK; j++) {
      if (buffer[j] != 0) {
        zeroBlock = false;
        break;
      }
    }
    if (zeroBlock) {
      masterCardData[i].zeroBlock = true;
      masterCardData[i].blockData = NULL;
    } else {
      masterCardData[i].zeroBlock = false;
      int res = copyBlockToBuffer(&masterCardData[i], buffer);
      if (res != 0) {
        Serial.print(F("Copying block "));
        Serial.print(i);
        Serial.println(F(" failed. Skippind it..."));
      }
    }
  }
}

void setNewUid() {
  bool result = rfidReader.MIFARE_SetUid(masterUid.uidByte, masterUid.size, true);
  if (result) {
    Serial.println(F("Setting uid was successfull"));
  } else {
    Serial.println(F("Setting uid failed"));
  }
}

void writeDataToCard() {
  for (int i = 0; i < SECTORS * BLOCKS_PER_SECTOR; i++) {
    MFRC522::StatusCode statusCode;
    statusCode = rfidReader.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A,
      i,
      &keyA,
      &(rfidReader.uid)  
    );

    if (statusCode != MFRC522::STATUS_OK) {
      Serial.print(F("Block "));
      Serial.print(i);
      Serial.print(F(" is not authenticated. Error code: "));
      Serial.println(rfidReader.GetStatusCodeName(statusCode));
      errorDuringWriting = true;
      break;
    }

    byte buffer[BYTES_PER_BLOCK];
    byte size = sizeof(buffer);
    if (masterCardData[i].zeroBlock) {
      memset(buffer, 0, size);
    } else {
      memcpy(buffer, masterCardData[i].blockData, BYTES_PER_BLOCK);
    }
    Serial.println(F("DEBUG: DATA TO BE WRITTEN: "));
    Serial.print(F("blockAddr: "));
    Serial.println(i);
    Serial.print(F("size: "));
    Serial.println(size);
    Serial.print(F("buffer: "));
    for (int j = 0; j < size; j++) {
      char buff[3];
      sprintf(buff, "%02X", buffer[j]);
      Serial.print(buff);
      Serial.print(F(" "));
    }
    Serial.println();
    statusCode = rfidReader.MIFARE_Write(i, buffer, size);
    if (statusCode != MFRC522::STATUS_OK) {
      Serial.print(F("Block "));
      Serial.print(i);
      Serial.print(F(" is not written. Error code: "));
      Serial.println(rfidReader.GetStatusCodeName(statusCode));
      errorDuringWriting = true;
      break;
    }
  }
}

int copyBlockToBuffer(Block *block, byte* buffer) {
  block->blockData = (byte*)malloc(BYTES_PER_BLOCK * sizeof(byte));

  if (block->blockData == NULL) {
    return 1;
  }

  memcpy(block->blockData, buffer, BYTES_PER_BLOCK);
  return 0;
}

void printPICCTypeToLCD(byte sak) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Card type:"));
  lcd.setCursor(0, 1);
  int piccType = rfidReader.PICC_GetType(sak);
  lcd.print(rfidReader.PICC_GetTypeName(piccType));
  delay(1000);
  printDefaultMessageToLCD();
}

void printPICCTypeToSerial(byte sak) {
  Serial.print(F("Card type: "));
  int piccType = rfidReader.PICC_GetType(sak);
  Serial.print(rfidReader.PICC_GetTypeName(piccType));
  Serial.println();
}

void printPICCUidInfoToSerial(Uid uid) {
  Serial.print(F("Card uid: "));
  for (int i = 0; i < uid.size; i++) {
    char buffer[3];
    sprintf(buffer, "%02X", uid.uidByte[i]);
    Serial.print(buffer);
    Serial.print(F(" "));
  }
  Serial.println();
  Serial.print(F("Uid sak: "));
  Serial.println(uid.sak);
}

void printCardDataToSerial(Block cardData[SECTORS * BLOCKS_PER_SECTOR]) {
  Serial.println(F("Sector Block   0  1  2  3   4  5  6  7   8  9 10 11  12 13 14 15"));
  for (int i = SECTORS-1; i >= 0; i--) {
    Serial.print(F("  "));
    if (i < 10) {
      Serial.print(F(" "));
    }
    Serial.print(i);
    Serial.print(F("     "));
    for (int j = BLOCKS_PER_SECTOR-1; j >= 0 ; j--) {
      if (j != 3) {
        Serial.print("         ");
      }
      int blockNumber = i*4 + j;
      if (blockNumber < 10) {
        Serial.print(F(" "));
      }
      Serial.print(blockNumber);
      Serial.print(F("   "));
      for (int b = 0; b < BYTES_PER_BLOCK; b++) {
        char buffer[3];
        if (cardData[blockNumber].zeroBlock == true) {
          sprintf(buffer, "%02X", 0);
        } else {
          sprintf(buffer, "%02X", cardData[blockNumber].blockData[b]);
        }
        Serial.print(buffer);
        Serial.print(F(" "));
        if ((b + 1) % 4 == 0) {
          Serial.print(F(" "));
        }
      }
      Serial.println();
    }
  }
}

void printDefaultMessageToLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Ready to scan:"));
}

void freeAllocatedMemory() {
  for (int i = 0; i < SECTORS * BLOCKS_PER_SECTOR; i++) {
    if (masterCardData[i].blockData != NULL) {
      free(masterCardData[i].blockData);
      masterCardData[i].blockData = NULL;
    }
  }
}

int countFreeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

