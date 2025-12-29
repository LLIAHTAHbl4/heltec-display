// ОФИЦИАЛЬНЫЙ код для Heltec WiFi Kit 32 V3.1
// По документации: https://heltec.org/project/wifi-kit-32-v3/

#include <Wire.h>
#include "SH1106Wire.h"

// ТОЧНЫЕ пины по документации Heltec:
#define I2C_SDA     17    // GPIO17 - SDA
#define I2C_SCL     18    // GPIO18 - SCL  
#define OLED_RST    21    // GPIO21 - Reset
#define OLED_VEXT   10    // GPIO10 - Питание дисплея (LOW = ON)

// Адрес дисплея
#define OLED_ADDR   0x3C

// Создаем объект дисплея SH1106
SH1106Wire display(OLED_ADDR, I2C_SDA, I2C_SCL);

void setup() {
  Serial.begin(115200);
  while (!Serial); // Ждем Serial
  
  Serial.println();
  Serial.println("=================================");
  Serial.println("HELTEC WiFi Kit 32 V3.1");
  Serial.println("Official firmware by Heltec");
  Serial.println("=================================");
  
  // ШАГ 1: Включаем питание дисплея через VEXT
  Serial.println("[1] Powering display via VEXT...");
  pinMode(OLED_VEXT, OUTPUT);
  digitalWrite(OLED_VEXT, LOW);   // LOW = питание ВКЛЮЧЕНО
  delay(100);
  
  // ШАГ 2: Сбрасываем дисплей
  Serial.println("[2] Resetting display...");
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  
  // ШАГ 3: Инициализация I2C
  Serial.println("[3] Starting I2C...");
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // ШАГ 4: Инициализация дисплея SH1106
  Serial.println("[4] Initializing SH1106 display...");
  display.init();
  
  // ШАГ 5: Настройка дисплея (важно для Heltec!)
  display.flipScreenVertically();  // Переворачиваем для правильной ориентации
  display.setFont(ArialMT_Plain_16);
  
  // ШАГ 6: Очистка и вывод
  Serial.println("[5] Drawing on display...");
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "HELTEC V3.1");
  display.drawString(0, 20, "WiFi Kit 32");
  display.drawString(0, 40, "OLED WORKS!");
  display.display();
  
  Serial.println("=================================");
  Serial.println("Display initialization COMPLETE!");
  Serial.println("Check the OLED screen!");
  Serial.println("=================================");
}

void loop() {
  // Простая анимация
  static int counter = 0;
  
  // Обновляем только часть экрана
  display.setColor(BLACK);
  display.fillRect(0, 40, 128, 24);
  display.setColor(WHITE);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 40, "Count: " + String(counter));
  display.display();
  
  Serial.println("Counter: " + String(counter));
  counter++;
  
  // Мигание инверсией
  display.invertDisplay();
  delay(200);
  display.normalDisplay();
  
  delay(800);
}
