/*
 * Проект для Heltec WiFi Kit 32 V3.1
 * Датчики: AHT10 (AC5600) и SHT31-D
 */

#include "SH1106Wire.h"
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_SHT31.h>

// --- Константы дисплея ---
#define OLED_VEXT     10
#define OLED_RST      21
#define OLED_SDA      17
#define OLED_SCL      18
#define OLED_ADDR     0x3C

// --- Константы I2C для датчиков ---
#define I2C_SDA       4
#define I2C_SCL       5
#define AHT10_ADDR    0x38
#define SHT31_ADDR    0x44

// --- Объекты ---
SH1106Wire display(OLED_ADDR, OLED_SDA, OLED_SCL);
Adafruit_AHTX0 aht;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

// --- Переменные ---
bool ahtFound = false;
bool shtFound = false;
unsigned long lastUpdate = 0;
const int UPDATE_INTERVAL = 2000;

void setup() {
  Serial.begin(115200);
  delay(100);
  
  // Инициализация дисплея
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
  
  // Начальный экран
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Heltec V3.1");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "Инициализация...");
  display.display();
  
  Serial.println("\n=== Heltec WiFi Kit 32 V3.1 ===");
  Serial.println("Инициализация...");
  
  // Инициализация датчиков
  Wire1.begin(I2C_SDA, I2C_SCL);
  
  // AHT10
  Serial.print("Поиск AHT10... ");
  if (aht.begin(&Wire1, AHT10_ADDR)) {
    ahtFound = true;
    Serial.println("OK");
  } else {
    Serial.println("НЕТ");
  }
  
  // SHT31
  Serial.print("Поиск SHT31... ");
  if (sht31.begin(SHT31_ADDR, &Wire1)) {
    shtFound = true;
    Serial.println("OK");
  } else if (sht31.begin(0x45, &Wire1)) {
    shtFound = true;
    Serial.println("OK (0x45)");
  } else {
    Serial.println("НЕТ");
  }
  
  // Статус на дисплее
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Датчики:");
  display.setFont(ArialMT_Plain_10);
  
  int yPos = 20;
  if (ahtFound) {
    display.drawString(0, yPos, "✓ AHT10: OK");
    yPos += 12;
  } else {
    display.drawString(0, yPos, "✗ AHT10: НЕТ");
    yPos += 12;
  }
  
  if (shtFound) {
    display.drawString(0, yPos, "✓ SHT31: OK");
  } else {
    display.drawString(0, yPos, "✗ SHT31: НЕТ");
  }
  
  display.display();
  delay(2000);
}

void loop() {
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();
    
    float tempAHT = NAN, humidityAHT = NAN;
    float tempSHT = NAN, humiditySHT = NAN;
    
    // Чтение AHT10
    if (ahtFound) {
      sensors_event_t humidity, temp;
      if (aht.getEvent(&humidity, &temp)) {
        tempAHT = temp.temperature;
        humidityAHT = humidity.relative_humidity;
      }
    }
    
    // Чтение SHT31
    if (shtFound) {
      tempSHT = sht31.readTemperature();
      humiditySHT = sht31.readHumidity();
    }
    
    // Вывод в Serial
    Serial.println("\n=== Данные датчиков ===");
    
    if (ahtFound && !isnan(tempAHT)) {
      Serial.printf("AHT10:  %.1f°C, %.1f%%\n", tempAHT, humidityAHT);
    } else {
      Serial.println("AHT10:  Нет данных");
    }
    
    if (shtFound && !isnan(tempSHT)) {
      Serial.printf("SHT31:  %.1f°C, %.1f%%\n", tempSHT, humiditySHT);
    } else {
      Serial.println("SHT31:  Нет данных");
    }
    
    // Вывод на дисплей
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Датчики:");
    
    display.setFont(ArialMT_Plain_10);
    int yPos = 20;
    
    if (ahtFound && !isnan(tempAHT)) {
      display.drawString(0, yPos, "AHT10:");
      display.drawString(40, yPos, String(tempAHT, 1) + "C");
      display.drawString(80, yPos, String(humidityAHT, 1) + "%");
      yPos += 12;
    }
    
    if (shtFound && !isnan(tempSHT)) {
      display.drawString(0, yPos, "SHT31:");
      display.drawString(40, yPos, String(tempSHT, 1) + "C");
      display.drawString(80, yPos, String(humiditySHT, 1) + "%");
      yPos += 12;
    }
    
    if (ahtFound && shtFound && !isnan(tempAHT) && !isnan(tempSHT)) {
      float avgTemp = (tempAHT + tempSHT) / 2;
      float avgHum = (humidityAHT + humiditySHT) / 2;
      display.drawString(0, yPos, "Среднее:");
      display.drawString(50, yPos, String(avgTemp, 1) + "C");
      display.drawString(90, yPos, String(avgHum, 1) + "%");
    }
    
    display.drawString(0, 55, "Обновлено: " + String(millis()/1000) + "с");
    display.display();
  }
  
  delay(100);
}
