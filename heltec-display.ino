// Тестовый код для Heltec ESP32-S3 с дисплеем
#include <Wire.h>
#include "SH1106Wire.h"

// Тестируем РАЗНЫЕ комбинации пинов:
// Попробуй по очереди каждую секцию:

// === ВАРИАНТ 1 (самый вероятный для Heltec S3) ===
// #define SDA_PIN 17
// #define SCL_PIN 18
// #define RST_PIN 21

// === ВАРИАНТ 2 ===
// #define SDA_PIN 15
// #define SCL_PIN 4
// #define RST_PIN 16

// === ВАРИАНТ 3 (стандартные I2C пины) ===
 #define SDA_PIN 8
 #define SCL_PIN 9
 #define RST_PIN -1  // без пина сброса

SH1106Wire display(0x3C, SDA_PIN, SCL_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== TESTING DISPLAY ===");
  
  // Пробуем инициализировать I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.print("I2C started on SDA:");
  Serial.print(SDA_PIN);
  Serial.print(" SCL:");
  Serial.println(SCL_PIN);
  
  // Пробуем сброс дисплея если пин определен
  if (RST_PIN > 0) {
    pinMode(RST_PIN, OUTPUT);
    digitalWrite(RST_PIN, LOW);
    delay(100);
    digitalWrite(RST_PIN, HIGH);
    delay(100);
    Serial.println("Display reset performed");
  }
  
  // Пробуем инициализировать дисплей
  Serial.println("Trying to init display...");
  
  // Вариант 1: Обычная инициализация
  display.init();
  
  // Вариант 2: С флипом
  // display.init();
  // display.flipScreenVertically();
  
  // Вариант 3: Без флипа
  // display.init();
  // display.resetDisplay();
  
  Serial.println("Display init attempted");
  
  // Очищаем и выводим текст
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "SDA:" + String(SDA_PIN));
  display.drawString(0, 20, "SCL:" + String(SCL_PIN));
  display.drawString(0, 40, "TEST 123");
  display.display();
  
  Serial.println("Text sent to display");
  Serial.println("=== TEST COMPLETE ===");
}

void loop() {
  delay(5000); // Ждем 5 секунд
  
  // Мигаем инверсией для проверки
  display.invertDisplay();
  delay(500);
  display.normalDisplay();
  
  Serial.println("Display inverted (if working)");
}
