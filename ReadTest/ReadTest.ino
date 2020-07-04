 /**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * This sample shows how to read and write data blocks on a MIFARE Classic PICC
 * (= card/tag).
 * 
 * BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).
 * 
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * 
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         PA3           // Configurable, see typical pin layout above
#define SS_PIN          PA4          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key defaultKey;

// My test keys orderred by sector (hard-coded)
// NOTE: Synchronize the NR_KNOWN_KEYS define with the defaultKeys[] array
#define NR_MY_KEYS   16
byte myKeysA[NR_MY_KEYS][MFRC522::MF_KEY_SIZE] =  {
    {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5},
    {0xb6, 0xf0, 0xfc, 0x87, 0xf5, 0x7f},
    {0x58, 0x88, 0x18, 0x0a, 0xdb, 0xe6},
    {0x64, 0xea, 0x31, 0x7b, 0x7a, 0xbd},
    {0x89, 0x89, 0x89, 0x89, 0x08, 0x23},
    {0x89, 0x89, 0x89, 0x89, 0x17, 0x89},
    {0x89, 0x89, 0x89, 0x89, 0x30, 0x89},
    {0xb6, 0xe5, 0x6b, 0xad, 0x20, 0x6a},
    {0x4d, 0x10, 0x95, 0xf1, 0xaf, 0x34},
    {0x89, 0x10, 0x89, 0x89, 0x89, 0x89},
    {0x89, 0x63, 0x89, 0x89, 0x89, 0x89},
    {0x89, 0x01, 0x63, 0x89, 0x89, 0x89},
    {0x89, 0x89, 0x27, 0x63, 0x89, 0x89},
    {0x89, 0x89, 0x89, 0x06, 0x38, 0x89},
    {0x89, 0x89, 0x89, 0x42, 0x89, 0x89},
    {0x89, 0x89, 0x89, 0x04, 0x89, 0x89}  
};

byte myKeysB[NR_MY_KEYS][MFRC522::MF_KEY_SIZE] =  {
    {0x24, 0x81, 0x11, 0x8e, 0x53, 0x55},
    {0xe4, 0xfd, 0xac, 0x29, 0x2b, 0xed},
    {0xd5, 0x72, 0xc9, 0x49, 0x11, 0x37},
    {0xa3, 0x9a, 0x28, 0x62, 0x85, 0xdb},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x8f, 0xe6, 0xfa, 0x23, 0x0c, 0x69},
    {0x1a, 0xd2, 0xf9, 0x9b, 0xb9, 0xe9},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  
};

/**
 * Initialize.
 */
void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        defaultKey.keyByte[i] = 0xFF;
    }

    Serial.println(F("Scan a MIFARE Classic PICC to demonstrate read."));
    Serial.print(F("Using key (for A and B):"));
    dump_byte_array(defaultKey.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.println();
    
}

/**
 * Main loop.
 */
void loop() {
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    
    byte sector  = 0;
    byte readBlock   = 1;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    for (byte sector = 0; sector < 16; sector++) {
          readBlock = (sector*4) + 1;
          
          Serial.println(F("READBLOCK..."));
          Serial.println(F(readBlock));

          
          // Prepare key
          MFRC522::MIFARE_Key key;
          // Copy my key into the MIFARE_Key structure
          for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
              key.keyByte[i] = myKeysB[sector][i];
          }

          
          // Authenticate using key A
          Serial.println(F("Authenticating using key A..."));
          status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, readBlock, &key, &(mfrc522.uid));
          if (status != MFRC522::STATUS_OK) {
              Serial.print(F("PCD_Authenticate() failed: "));
              Serial.println(mfrc522.GetStatusCodeName(status));
              //return;
          }else{
            // Dump the sector data
            Serial.println(F("Current data in sector:"));
            dumpSectorWithKeyB(&key, sector);
          }
          
          
          
          //ONLY for Key A
          //mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
          Serial.println();

    }
    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}

boolean dumpSectorWithKeyB(MFRC522::MIFARE_Key *key, byte sector)
{
    boolean result = false;
    byte buffer[18];
    byte firstBlock = (sector*4); // + 1;
    MFRC522::StatusCode status;
    
    // http://arduino.stackexchange.com/a/14316
    //if ( ! mfrc522.PICC_IsNewCardPresent())
    //    return false;
    //if ( ! mfrc522.PICC_ReadCardSerial())
    //    return false;
    // Serial.println(F("Authenticating using key B..."));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, firstBlock, key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
         Serial.print(F("PCD_Authenticate() failed: "));
        // Serial.println(mfrc522.GetStatusCodeName(status));
        return false;
    }

    for(byte i = 4; i > 0; i--){
      
    
          // Read block
          byte byteCount = sizeof(buffer);
          status = mfrc522.MIFARE_Read(firstBlock + (i -1), buffer, &byteCount);
          if (status != MFRC522::STATUS_OK) {
               Serial.print(F("MIFARE_Read() failed: "));
              // Serial.println(mfrc522.GetStatusCodeName(status));
          }
          else {
              // Successful read
              result = true;
              Serial.print(F("Success with key B:"));
              dump_byte_array((*key).keyByte, MFRC522::MF_KEY_SIZE);
              Serial.println();
              // Dump block data
              Serial.print(F("Block ")); Serial.print(firstBlock + (i-1)); Serial.print(F(":"));
              dump_byte_array(buffer, 16);
              Serial.println();
          }
          Serial.println();
    }

    //mfrc522.PICC_HaltA();       // Halt PICC
    //mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
    return result;
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
