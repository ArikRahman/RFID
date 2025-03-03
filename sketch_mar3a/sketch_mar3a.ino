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

const char* ssid  = "yours";
const char* password = "yours123";
const int port = 80;
WiFiServer server(port);
WiFiClient  Client;


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
  WiFi.begin(ssid, password);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  lcd.setCursor(0, 1);

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) {
    delay(500);
  }
  if (i == 21) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ERR conn :(");

    Serial.println("Could not connect to");

    Serial.println(ssid);
    while (1) {
      delay(500);
    }
  }
  digitalWrite(LED_BUILTIN, HIGH);
  server.begin();
  server.setNoDelay(true);
  Serial.println(WiFi.localIP());
  Serial.println("port:" + String(port));
  lcd.clear();

  SPI.begin();
  mfrc522.PCD_Init();

  lcd.setCursor(0, 0);
  lcd.print("Deep 2023");

  digitalWrite(BUZZER, HIGH);
  delay(1000);
  digitalWrite(BUZZER, LOW);

}
void loop() {

  if (server.hasClient()) {
    if (!Client || !Client.connected()) {
      if (Client) {
        Client.stop();
      }
      Client = server.available();
    }
  }


  if (Client && Client.connected()) {
    if (Client.available()) {
      while (Client.available()) {
        char data = Client.read();
        Serial.println(data);
      }
    }
  } else {

  }

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

  sendDatatoWIFI();
  content = "";

  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);

  delay(1000);

}

void sendDatatoWIFI() {
  if (content != "") {
    int str_len = content.length() + 1;
    char char_array[str_len];
    content.toCharArray(char_array, str_len);
    // Serial.println(content);
    Serial.println(char_array);
    Client.write(char_array);
  }
}