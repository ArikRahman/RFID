#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <require_cpp11.h>

// Winter showcase version
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);



#define SS_PIN D3
#define RST_PIN D4
int BUZZER = D8;

/*
  lcd module:
  5v       pin -> to -> 5v
  gnd      pin -> to -> gnd
  scl      pin -> to -> D1
  sda      pin -> to -> D2

  rfid card module :
  3.3v     pin -> to -> 3.3v
  gnd      pin -> to -> gnd

  sda(ss)  pin -> to -> D3
  rst      pin -> to -> D4

  sck      pin -> to -> D5
  mosi     pin -> to -> D7
  miso     pin -> to -> D6

  buzzer:
  buz      pin -> to -> D8
*/

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

String content = "";
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  WiFi.mode(WIFI_STA);

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  
  Serial.println("Connecting to ");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  lcd.setCursor(0, 1);

  
 
  SPI.begin();
  mfrc522.PCD_Init();

  lcd.setCursor(0, 0);
  lcd.print("Deep 2023");


  delay(1000);
  

}
void loop() {

  

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter an input...");
  String input = Serial.readStringUntil('\n');
  input.trim();
  Serial.print("Received input: '");
  Serial.print(input);
  Serial.println("'");

  if (input == "1") {
  // Check if a card is present and read its UID
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Build the UID string
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println(content);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CI: ");
  lcd.print(content);

  // Halt the card and stop encryption so that it can be re-detected
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
  content = "";

  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);

  delay(1000);
}

if (input == "2") {
  String data = "0A1B1C1D";
  data.replace(" ", "");  // Remove spaces if any

  // Convert hex string to a 16-byte array, padding with zeros if needed
  int byteCount = data.length() / 2;
  byte writeData[16];  // We need exactly 16 bytes for a block

  for (int i = 0; i < byteCount; i++) {
    String byteStr = data.substring(i * 2, i * 2 + 2);
    writeData[i] = (byte) strtol(byteStr.c_str(), NULL, 16);
  }
  for (int i = byteCount; i < 16; i++) {
    writeData[i] = 0;
  }

  // Ensure a card is present and read its UID for authentication
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    Serial.println("No card present or failed to read card.");
    return;
  }

  byte blockAddress = 0;  // Set the block address to write to
  MFRC522::StatusCode status;
  
  // Authenticate with the card using the default key
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddress, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write the block
  status = mfrc522.MIFARE_Write(blockAddress, writeData, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Write() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  } else {
    Serial.println("Data written successfully!");
  }

  // Halt and stop crypto to finalize the transaction
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

}






  





