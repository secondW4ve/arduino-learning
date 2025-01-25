#define CRSF_ADDRESS_FLIGHT_CONTROLLER 0XC8 // Flight Controler
#define CRSF_FRAME_LENGTH 24 // length of type + payload + crc
#define CRSF_PACKET_SIZE 26
#define CRSF_MAX_CHANNEL 16
#define CRSF_PACKETTYPE_RC_CHANNELS_DATA 0X16

int printCounter = 0;
uint8_t* inBuffer = nullptr;
uint8_t crsfData[CRSF_PACKET_SIZE];
uint16_t* channels = nullptr;
unsigned long lastPacketReceived = 0;
uint16_t m_channels[CRSF_MAX_CHANNEL];

void setup() {
  Serial2.begin(115200);
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
}

void loop() {
  uint8_t packetLength, inData, bufferIndex = 0;
  while (Serial2.available() > 0) {
    inData = Serial2.read();
    if (bufferIndex == 0) {
      if (inData == CRSF_ADDRESS_FLIGHT_CONTROLLER) {
        uint8_t addressByte = inData;
        inData = Serial2.read();
        packetLength = inData;
        if (packetLength < 2 || packetLength > 64) {
          // Serial.println("Invalid packet length, skipping...");
          bufferIndex = 0;
          // delete[] inBuffer;
          continue;
        }
        // inBuffer[bufferIndex++] = inData;
        inData = Serial2.read();
        Serial.print("Packet type: ");
        Serial.println(inData, HEX);
        if (inData != CRSF_PACKETTYPE_RC_CHANNELS_DATA) {
          Serial.println("Skipping non-RC Channel Data packet...");
          bufferIndex = 0;
          delete[] inBuffer;
          continue;
        }
        Serial.print("Size: ");
        Serial.println(packetLength);
        inBuffer = new uint8_t[packetLength + 2];
        // We need to read only RC Channel Data
        inBuffer[bufferIndex++] = addressByte;
        inBuffer[bufferIndex++] = packetLength;
        inBuffer[bufferIndex++] = inData;
      } else {
        uint8_t length = Serial2.read();
        for (int i = 0; i < packetLength - 1; i++) {
          Serial2.read();
        }
        Serial.print(F("Packet for "));
        Serial.print(inData, HEX);
        Serial.print(F(" received, size: "));
        Serial.println(length);
        bufferIndex = 0;
        continue;
      }
    } else if (bufferIndex > 1 && bufferIndex < packetLength + 1) {
      inBuffer[bufferIndex++] = inData;
    } else if (bufferIndex == packetLength + 1) {
      Serial.println("Last byte");
      inBuffer[bufferIndex++] = inData;
      uint8_t crc = crsf_crc8(&inBuffer[2], inBuffer[1] - 1);
      Serial.print("Counted CRC: ");
      Serial.println(crc);
      Serial.print("CRC in buffer: ");
      Serial.println(inBuffer[packetLength + 1]);
      // delete[] inBuffer;
      // // delete[] crsfData;
      // bufferIndex = 0;
      // packetLength = 0;
      // Probably is not correct
      if (crc == inBuffer[packetLength + 1]) {
        memcpy(crsfData, inBuffer, packetLength + 2);
        // lastPacketReceived = millis();
        updateChannels();
        Serial.print(F("Channel 5 data: "));
        Serial.print(m_channels[4]);
        Serial.print(F(" channel 3 data: "));
        Serial.print(m_channels[2]);
        if (m_channels[4] < 1000) {
          digitalWrite(2, HIGH);
        } else {
          digitalWrite(2, LOW);
        }
        Serial.println();
      } else {
        Serial.println(F("CRC doesn't match"));
      }
      delete[] inBuffer;
      inBuffer = nullptr;
      bufferIndex = 0;
      packetLength = 0;
    }
  }
}

uint8_t crsf_crc8(const uint8_t *ptr, uint8_t len)
{
    static const uint8_t crsf_crc8tab[256] = {
        0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
        0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
        0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
        0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
        0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
        0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
        0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
        0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
        0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
        0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
        0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
        0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
        0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
        0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
        0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
        0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9};

    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++)
    {
        crc = crsf_crc8tab[crc ^ *ptr++];
    }
    return crc;
}


void updateChannels()
{

        size_t bitOffset = 0;
    
    for (size_t i = 0; i < 16; ++i) {
        size_t byteOffset = bitOffset / 8;
        size_t bitStart = bitOffset % 8;

        // Unpack the 11-bit channel value
        m_channels[i] = (
            (crsfData[3 + byteOffset] >> bitStart) |
            (crsfData[4 + byteOffset] << (8 - bitStart))
        ) & 0x07FF;

        bitOffset += 11;
    }
}
