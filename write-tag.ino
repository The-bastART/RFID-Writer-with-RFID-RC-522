/*
 * Write personal data of a MIFARE RFID card using a RFID-RC522 reader
 * Uses MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT. 
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
 * Hardware required:
 * Arduino
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>
#include <string.h>
//#define BUZ             6
#define RST_PIN         7           // Configurable, see typical pin layout above
#define SS_PIN          8          // Configurable, see typical pin layout above
int index=0;
char IDArr[8];
byte IDBy[4];

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

void serialEvent(){
  while(Serial.available()){
    char ID = Serial.read();
    IDArr[index++]=ID; 
  }
index=0;

  // Use 'nullptr' or 'NULL' for the second parameter.
  unsigned long number = strtoul( IDArr, nullptr, 16);

  for(int i=3; i>=0; i--)    // start with lowest byte of number
  {
    IDBy[i] = number & 0xFF;  // or: = byte( number);
    number >>= 8;            // get next byte into position
  }
  for(int i=0; i<4; i++)
  {
    Serial.print("0x");
    Serial.println(IDBy[i], HEX);
  }

  if (strstr(IDArr, "HA")!=NULL){
    IDBy[0]=0xCA;  
  }
}

void setup() {
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  Serial.println(F("Write personal data on a MIFARE PICC "));
  //pinMode(BUZ, OUTPUT);
}

void loop() {

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    //Serial.print("Looking for new Cards...");
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    Serial.print("Card selected!");
    return;
  }

  Serial.print(F("Card UID:"));    //Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print(F(" PICC type: "));   // Dump PICC type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  byte block[]={IDBy[0], IDBy[1], IDBy[2], IDBy[3]};
  MFRC522::StatusCode status;

  // Write block
  status = mfrc522.MIFARE_Ultralight_Write(4, block, 4);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else { 
    Serial.println(F("MIFARE_Write() success: "));
    IDBy[0]=0x00;
    IDBy[1]=0x00;
    IDBy[2]=0x00;
    IDBy[3]=0x00;
  }

  Serial.println(" ");
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  //tone(BUZ, 1000, 50);
  //delay(50);
  //noTone(BUZ);
 }
