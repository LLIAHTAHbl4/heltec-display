/*
 * Heltec WiFi Kit 32 V3.1
 * AHT10 + SHT31 Sensors
 */

#include "SH1106Wire.h"
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_SHT31.h>

// Display
#define OLED_VEXT 10
#define OLED_RST 21
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_ADDR 0x3C

// Sensors I2C
#define I2C_SDA 4
#define I2C_SCL 5
#define AHT10_ADDR 0x38
#define SHT31_ADDR 0x44

SH1106Wire display(OLED_ADDR, OLED_SDA, OLED_SCL);
Adafruit_AHTX0 aht;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

bool ahtFound = false;
bool shtFound = false;
unsigned long lastUpdate = 0;
const int UPDATE_INTERVAL = 2000;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  // Display init
  pinMode(OLED_VEXT, OUTPUT);
  digitalWrite(OLED_VEXT, LOW);
  delay(100);
  
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  
  Wire.begin(OLED_SDA, OLED_SCL);
  display.init();
  display.flipScreenVertically();
  
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Heltec V3.1");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "Initializing...");
  display.display();
  
  Serial.println("\n=== Heltec WiFi Kit 32 V3.1 ===");
  
  // Sensors I2C
  Wire1.begin(I2C_SDA, I2C_SCL);
  
  // AHT10
  Serial.print("AHT10... ");
  if (aht.begin(&Wire1, AHT10_ADDR)) {
    ahtFound = true;
    Serial.println("OK");
  } else Serial.println("NO");
  
  // SHT31
  Serial.print("SHT31... ");
  if (sht31.begin(SHT31_ADDR, &Wire1)) {
    shtFound = true;
    Serial.println("OK");
  } else if (sht31.begin(0x45, &Wire1)) {
    shtFound = true;
    Serial.println("OK (0x45)");
  } else Serial.println("NO");
  
  // Display status
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Sensors:");
  display.setFont(ArialMT_Plain_10);
  
  int y = 20;
  if (ahtFound) {
    display.drawString(0, y, "✓ AHT10: OK");
    y += 12;
  }
  if (shtFound) {
    display.drawString(0, y, "✓ SHT31: OK");
  }
  
  display.display();
  delay(2000);
}

void loop() {
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();
    
    float tempAHT = NAN, humAHT = NAN;
    float tempSHT = NAN, humSHT = NAN;
    
    // Read AHT10
    if (ahtFound) {
      sensors_event_t humidity, temp;
      if (aht.getEvent(&humidity, &temp)) {
        tempAHT = temp.temperature;
        humAHT = humidity.relative_humidity;
      }
    }
    
    // Read SHT31
    if (shtFound) {
      tempSHT = sht31.readTemperature();
      humSHT = sht31.readHumidity();
    }
    
    // Serial output
    Serial.println("\n=== Sensor Data ===");
    
    if (ahtFound && !isnan(tempAHT)) {
      Serial.printf("AHT10:  %.1f°C, %.1f%%\n", tempAHT, humAHT);
    }
    
    if (shtFound && !isnan(tempSHT)) {
      Serial.printf("SHT31:  %.1f°C, %.1f%%\n", tempSHT, humSHT);
    }
    
    // Display output
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Sensors:");
    
    display.setFont(ArialMT_Plain_10);
    int y = 20;
    
    if (ahtFound && !isnan(tempAHT)) {
      display.drawString(0, y, "AHT10:");
      display.drawString(40, y, String(tempAHT, 1) + "C");
      display.drawString(80, y, String(humAHT, 1) + "%");
      y += 12;
    }
    
    if (shtFound && !isnan(tempSHT)) {
      display.drawString(0, y, "SHT31:");
      display.drawString(40, y, String(tempSHT, 1) + "C");
      display.drawString(80, y, String(humSHT, 1) + "%");
    }
    
    display.display();
  }
  
  delay(100);
}
