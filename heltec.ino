// Простой код для ESP32 с дисплеем SH1106
#include <Wire.h>
#include "SH1106Wire.h"

// Пины для Heltec WiFi Kit 32 V3.1
#define OLED_SDA 15
#define OLED_SCL 4
#define OLED_RST 16

SH1106Wire display(0x3C, OLED_SDA, OLED_SCL);

void setup() {
  // Настройка пина сброса
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  
  // Инициализация дисплея
  display.init();
  display.flipScreenVertically();
  display.clear();
  
  // Вывод текста
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Heltec V3.1");
  display.drawString(0, 20, "Привет!");
  display.drawString(0, 40, "GitHub OK");
  
  display.display();
}

void loop() {
  delay(1000);
}
