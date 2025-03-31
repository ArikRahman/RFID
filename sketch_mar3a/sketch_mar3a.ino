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

  if(input == "1"){
  
  

  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();
  Serial.println(content);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CI: ");
  lcd.print(content);

  
  content = "";

  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);

  delay(1000);
  }






if(input == "2"){ // Hardcoded hex string; remove any spaces if present.
  String data = "0A1B1C1D"; 
  data.replace(" ", "");  // Optional: remove spaces if needed

  // Convert the hex string to a 16-byte array (pad if necessary)
  int byteCount = data.length() / 2;
  byte writeData[16];  // Must be 16 bytes exactly
  
  // Convert every two hex characters into a byte
  for (int i = 0; i < byteCount; i++) {
    String byteStr = data.substring(i * 2, i * 2 + 2);
    writeData[i] = (byte) strtol(byteStr.c_str(), NULL, 16);
  }
  // Pad remaining bytes with zeros if data is less than 16 bytes
  for (int i = byteCount; i < 16; i++) {
    writeData[i] = 0;
  }

  // Check for a new card and read its UID before attempting write
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    Serial.println("No card present or failed to read card.");
    return;
  }
  
  byte blockAddress = 4;  // Adjust this to the correct block for your application
  MFRC522::StatusCode status;
  
  // Authenticate using the default key (0xFF 6 times)
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddress, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write the 16-byte block to the card
  status = mfrc522.MIFARE_Write(blockAddress, writeData, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Write() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  } else {
    Serial.println("Data written successfully!");
  }


  /*
  

new starts here

  */





/*



*/


  // Halt PICC and stop encryption on the PCD
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

}





}

  





