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


void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  WiFi.mode(WIFI_STA);
  
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


