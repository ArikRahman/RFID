#include <Wire.h>
#include <Adafruit_INA219.h>
#include <LiquidCrystal_I2C.h>

const int PHOTODIODE_PIN = A0;
const int LED_PIN = 13;

Adafruit_INA219 ina219;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int PHOTODIODE_THRESHOLD = 300;
volatile unsigned long pulseCount = 0;
unsigned long lastTime = 0;
float rpm = 0.0;

void setup() {
  Serial.begin(9600);
  ina219.begin();
  lcd.begin();
  lcd.backlight();
  pinMode(PHOTODIODE_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  lcd.setCursor(0, 0);
  lcd.print("Motor Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();
}

void loop() {
  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();
  float voltage = busvoltage + (shuntvoltage / 1000);
  float power = (voltage * current_mA) / 1000;
  
  int photodiodeValue = analogRead(PHOTODIODE_PIN);
  if (photodiodeValue > PHOTODIODE_THRESHOLD && !digitalRead(LED_PIN)) {
    pulseCount++;
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
  }
  
  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {
    rpm = (pulseCount * 60.0) / 1.0;
    pulseCount = 0;
    lastTime = currentTime;
  }
  
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage, 1);
  lcd.print("V I:");
  lcd.print(current_mA / 1000, 2);
  lcd.print("A  ");
  
  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(power, 1);
  lcd.print("W RPM:");
  lcd.print(rpm, 0);
  lcd.print("  ");
  
  Serial.print(voltage);
  Serial.print(",");
  Serial.print(current_mA / 1000);
  Serial.print(",");
  Serial.print(power);
  Serial.print(",");
  Serial.println(rpm);
  
  delay(500);
}

void displayEngineSpecs() {
  float wheelDiameter = 0.5;
  float maxRPM = 300.0;
  float generatorVoltage = 24.0;
  float generatorCurrent = 5.0;
  float maxPower = generatorVoltage * generatorCurrent;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wheel:");
  lcd.print(wheelDiameter, 1);
  lcd.print("m RPM:");
  lcd.print(maxRPM, 0);
  
  lcd.setCursor(0, 1);
  lcd.print("Max:");
  lcd.print(maxPower, 0);
  lcd.print("W");
  
  Serial.println("Engine Specifications:");
  Serial.print(wheelDiameter);
  Serial.println("m");
  Serial.print(maxRPM);
  Serial.println("RPM");
  Serial.print(generatorVoltage);
  Serial.println("V");
  Serial.print(generatorCurrent);
  Serial.println("A");
  Serial.print(maxPower);
  Serial.println("W");
  
  delay(5000);
}