// Код для Heltec WiFi Kit 32 V3.1
#include <Wire.h>
#include "SH1106Wire.h"

// Пины для Heltec V3.1
#define I2C_SDA 15  // GPIO15
#define I2C_SCL 4   // GPIO4
#define OLED_RST 16 // GPIO16

SH1106Wire display(0x3C, I2C_SDA, I2C_SCL);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Heltec V3.1 starting...");

  // Сброс дисплея
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);

  // Инициализация дисплея
  Wire.begin(I2C_SDA, I2C_SCL);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  
  // Очистка и вывод
  display.clear();
  display.drawString(0, 0, "HELLO!");
  display.drawString(0, 20, "Heltec V3.1");
  display.drawString(0, 40, "Working!");
  display.display();
  
  Serial.println("Display initialized!");
}

void loop() {
  Serial.println("Running...");
  delay(1000);
}
