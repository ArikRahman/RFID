//Roshan version
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


MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

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

    lcd.println("Could not connect to");

    lcd.println(ssid);
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
  lcd.print("Deep 2025");

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



  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return; // No card detected
  }

  // Read UID (Unique Identifier) of the card
  String cardUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardUID += String(mfrc522.uid.uidByte[i], HEX);
  }
  lcd.println("Card UID: " + cardUID);

  // Ask the user if they want to read or write
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("Do you want to");
  lcd.setCursor(0,1);
  lcd.println("read or write? (r/w)");
  while (Serial.available() == 0) {Serial.println("input not given");} // Wait for user input

  char userChoice = Serial.read();  // Block number to read/write (modify as needed)
  byte blockNumber = 4;
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; // Default key


  if (userChoice == 'w') {
    // Write data to the card
    lcd.clear();
    lcd.println("  Enter data to ");
    lcd.setCursor(0,1);
    lcd.println(" write (max 16): ");
    String dataToWrite = "";
    while (Serial.available() == 0) {} // Wait for user input
    while (Serial.available() > 0) {
      char c = Serial.read();
      dataToWrite += c;
    }

    // Ensure the data is 16 bytes
    if (dataToWrite.length() < 16) {
      dataToWrite += String(' ', 16 - dataToWrite.length());
    }

    byte buffer[16];
    dataToWrite.getBytes(buffer, 16);

    if (mfrc522.MIFARE_Write(blockNumber, buffer, 16) == MFRC522::STATUS_OK) {
      lcd.println("Data written successfully!");
    } else {
      lcd.println("Failed to write data.");
    }

  } else if (userChoice == 'r') {
    // Read data from the card
    lcd.clear();
    byte buffer[18];  // Data buffer (16 bytes + 2 CRC bytes)
    byte size = sizeof(buffer);

    if (mfrc522.MIFARE_Read(blockNumber, buffer, &size) == MFRC522::STATUS_OK) {
      String dataRead = "";
      for (byte i = 0; i < 16; i++) {
        dataRead += (char)buffer[i];
      }
      lcd.println("Data read from wcard: " + dataRead);
    } else {
      lcd.println("Failed to read data.");
    }
  } else {
    lcd.println("Invalid choice. Please enter 'r' or 'w'.");
  }

  // Halt the PICC
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

}

