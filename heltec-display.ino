#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Пины для I2C
#define I2C_SDA 15
#define I2C_SCL 4
#define I2C_RST 16

// Используем правильный класс из библиотеки SH110X
Adafruit_SH1106G display(128, 64, &Wire, I2C_RST);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Testing SH1106 display...");
  
  // Инициализация I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Сброс дисплея
  pinMode(I2C_RST, OUTPUT);
  digitalWrite(I2C_RST, LOW);
  delay(50);
  digitalWrite(I2C_RST, HIGH);
  delay(50);
  
  Serial.println("Initializing display...");
  
  // Пробуем инициализировать дисплей
  // Метод 1: без параметров
  if (!display.begin()) {
    Serial.println("Display init failed (method 1)");
    
    // Метод 2: с адресом
    if (!display.begin(0x3C, true)) {
      Serial.println("Display init failed (0x3C)");
      
      // Метод 3: с другим адресом
      if (!display.begin(0x3D, true)) {
        Serial.println("Display init completely failed");
        Serial.println("Possible issues:");
        Serial.println("1. Wrong pins");
        Serial.println("2. Display not powered");
        Serial.println("3. Display is SPI, not I2C");
        return;
      } else {
        Serial.println("Display found at 0x3D!");
      }
    } else {
      Serial.println("Display found at 0x3C!");
    }
  } else {
    Serial.println("Display initialized!");
  }
  
  // Очистка и вывод текста
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("HELLO!");
  display.println("Heltec S3");
  display.display();
  
  Serial.println("Text should be on display");
}

void loop() {
  delay(5000);
  
  // Мигание для проверки
  display.invertDisplay(true);
  delay(500);
  display.invertDisplay(false);
  
  Serial.println("Display inverted (if working)");
}
