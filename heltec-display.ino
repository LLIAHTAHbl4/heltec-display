#include <Wire.h>
#include "SH1106Wire.h"  // Библиотека для SH1106!

// Пины ПО ДОКУМЕНТАЦИИ Heltec:
#define I2C_SDA     17
#define I2C_SCL     18  
#define OLED_RST    21  // По документации!
#define OLED_VEXT   10  // По документации! Питание дисплея

SH1106Wire display(0x3C, I2C_SDA, I2C_SCL);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== HELTEC V3.1 OFFICIAL CODE ===");
  
  // 1. ВКЛЮЧАЕМ питание дисплея через VEXT (Active LOW!)
  pinMode(OLED_VEXT, OUTPUT);
  digitalWrite(OLED_VEXT, LOW);  // LOW = питание ВКЛЮЧЕНО
  Serial.println("VEXT: ON (LOW)");
  delay(100);
  
  // 2. СБРОС дисплея
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  Serial.println("RST: Done");
  
  // 3. Инициализация I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // 4. Инициализация дисплея SH1106
  Serial.println("Init SH1106...");
  display.init();
  
  // 5. Настройки дисплея
  display.flipScreenVertically();  // Важно для Heltec!
  display.setFont(ArialMT_Plain_16);
  display.clear();
  
  // 6. Вывод текста
  display.drawString(0, 0, "HELTEC V3.1");
  display.drawString(0, 20, "OFFICIAL");
  display.drawString(0, 40, "WORKING!");
  display.display();
  
  Serial.println("Display should show text!");
}

void loop() {
  // Мигание для проверки
  display.invertDisplay();
  delay(500);
  display.normalDisplay();
  delay(500);
  
  Serial.println("Display blinking (if working)");
}
