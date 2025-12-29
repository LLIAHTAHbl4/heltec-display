#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Пины для SPI дисплея SH1106
#define OLED_MOSI   13  // SDI
#define OLED_CLK    14  // SCL
#define OLED_DC     27  // DC
#define OLED_CS     12  // CS
#define OLED_RESET  33  // RES

Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Testing SPI display...");
  
  // Инициализация SPI дисплея
  if (!display.begin()) {
    Serial.println("SPI display init failed!");
    while(1);
  }
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println("SPI TEST");
  display.println("HELLO!");
  display.display();
  
  Serial.println("SPI display should work!");
}

void loop() {
  delay(1000);
}
