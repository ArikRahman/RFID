#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN 22
#define SS_PIN  21

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  SPI.begin();
  rfid.PCD_Init();
  lcd.begin();  
  lcd.backlight();
  
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  lcd.setCursor(0, 0);
  lcd.print("RFID Ready");
  lcd.setCursor(0, 1);
  lcd.print("1:Read 2:Write");
  
  Serial.println("RFID Reader Ready");
  Serial.println("1. Read Card");
  Serial.println("2. Write Card");
  Serial.println("Enter choice (1 or 2):");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    Serial.print("Received input: '");
    Serial.print(input);
    Serial.println("'");
    
    if (input == "1") {
      readRFID();
    }
    else if (input == "2") {
      writeRFID();
    }
    else {
      Serial.println("Invalid choice. Enter 1 or 2:");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Invalid Choice");
      delay(2000);
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID Ready");
    lcd.setCursor(0, 1);
    lcd.print("1:Read 2:Write");
    
    Serial.println("\n1. Read Card");
    Serial.println("2. Write Card");
    Serial.println("Enter choice (1 or 2):");
  }
}

void readRFID() {
  lcd.clear();
  lcd.print("Scanning...");
  
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    Serial.println("No card detected");
    lcd.clear();
    lcd.print("No Card Found");
    delay(2000);
    return;
  }
  
  lcd.clear();
  lcd.print("Card Found");
  Serial.print("Card UID: ");
  dump_byte_array(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();
  
  MFRC522::StatusCode status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed");
    lcd.clear();
    lcd.print("Auth Failed");
    delay(2000);
    return;
  }
  
  byte buffer[18];
  byte len = 18;
  status = rfid.MIFARE_Read(1, buffer, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Read failed");
    lcd.clear();
    lcd.print("Read Failed");
    delay(2000);
    return;
  }
  
  lcd.clear();
  lcd.print("Data (HEX):");
  lcd.setCursor(0, 1);
  Serial.print("Data read (HEX): ");
  for (byte i = 0; i < 4; i++) {  // Limit to 4 bytes for LCD space
    if (i < 2) {  // Display first 2 bytes on LCD
      lcd.print(buffer[i] < 0x10 ? "0" : "");
      lcd.print(buffer[i], HEX);
      lcd.print(" ");
    }
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
  delay(3000);
  
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void writeRFID() {
  lcd.clear();
  lcd.print("Place Card...");
  
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    Serial.println("No card detected");
    lcd.clear();
    lcd.print("No Card Found");
    delay(2000);
    return;
  }
  
  lcd.clear();
  lcd.print("Enter Data:");
  Serial.println("Enter data to write (max 16 chars):");
  
  while (Serial.available() == 0) {
    delay(100);
  }
  
  String data = Serial.readStringUntil('\n');
  data.trim();
  Serial.print("Writing data: '");
  Serial.print(data);
  Serial.println("'");
  
  byte buffer[16];
  for (byte i = 0; i < 16; i++) {
    buffer[i] = (i < data.length()) ? data[i] : ' ';
  }
  
  MFRC522::StatusCode status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(rfid.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Authentication failed");
    lcd.clear();
    lcd.print("Auth Failed");
    delay(2000);
    return;
  }
  
  status = rfid.MIFARE_Write(1, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.println("Write failed");
    lcd.clear();
    lcd.print("Write Failed");
    delay(2000);
  } else {
    Serial.println("Write successful");
    lcd.clear();
    lcd.print("Write Success");
    delay(2000);
  }
  
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
