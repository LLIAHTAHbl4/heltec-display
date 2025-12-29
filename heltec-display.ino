// Heltec V3.1 - Отладка I2C подключения
#include <Wire.h>
#include "SH1106Wire.h"

// Пробуем разные варианты пинов
// Вариант 1: GPIO21 и GPIO22 (часто работают)
#define I2C_SDA     21
#define I2C_SCL     22

// Вариант 2: GPIO4 и GPIO5
// #define I2C_SDA     4
// #define I2C_SCL     5

// Вариант 3: GPIO26 и GPIO27  
// #define I2C_SDA     26
// #define I2C_SCL     27

// Остальные пины
#define OLED_RST    21    // Reset (может конфликтовать с SDA!)
#define OLED_VEXT   10    // Питание дисплея
#define OLED_ADDR   0x3C  // Адрес дисплея

SH1106Wire display(OLED_ADDR, I2C_SDA, I2C_SCL);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.println("==========================================");
  Serial.println("Heltec V3.1 - I2C DEBUG");
  Serial.print("SDA: GPIO"); Serial.println(I2C_SDA);
  Serial.print("SCL: GPIO"); Serial.println(I2C_SCL);
  Serial.println("==========================================");
  
  // Включаем питание дисплея
  pinMode(OLED_VEXT, OUTPUT);
  digitalWrite(OLED_VEXT, LOW);
  delay(100);
  
  // Сброс дисплея (если RST не совпадает с SDA)
  if (OLED_RST != I2C_SDA) { // Проверяем конфликт пинов
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, LOW);
    delay(50);
    digitalWrite(OLED_RST, HIGH);
    delay(50);
  }
  
  // Инициализация I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(100);
  
  // === СКАНИРУЕМ I2C ШИНУ ===
  Serial.println("\n=== I2C SCAN ===");
  byte error, address;
  int devices = 0;
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      devices++;
      
      if (address == 0x3C) Serial.println("  ^ This is the OLED display");
      if (address == 0x68) Serial.println("  ^ This is MPU6050");
      if (address == 0x69) Serial.println("  ^ This is MPU6050 (alt address)");
    }
  }
  
  if (devices == 0) {
    Serial.println("No I2C devices found!");
    Serial.println("Check wiring and pins");
  }
  
  // === ПРОБУЕМ ИНИЦИАЛИЗИРОВАТЬ ДИСПЛЕЙ ===
  Serial.println("\n=== OLED TEST ===");
  if (display.init()) {
    Serial.println("OLED init: SUCCESS");
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.clear();
    
    // Выводим результаты сканирования
    display.drawString(0, 0, "I2C Scan Result:");
    if (devices == 0) {
      display.drawString(0, 12, "No devices!");
      display.drawString(0, 24, "SDA:" + String(I2C_SDA));
      display.drawString(0, 36, "SCL:" + String(I2C_SCL));
    } else {
      display.drawString(0, 12, "Found: " + String(devices));
      display.drawString(0, 24, "Check Serial");
    }
    display.display();
  } else {
    Serial.println("OLED init: FAILED");
  }
  
  delay(5000);
  
  // === ПРОБУЕМ НАЙТИ MPU6050 ===
  Serial.println("\n=== MPU6050 TEST ===");
  testMPU6050();
}

void testMPU6050() {
  // Пробуем адрес 0x68
  Wire.beginTransmission(0x68);
  byte error = Wire.endTransmission();
  
  Serial.print("MPU6050 at 0x68: ");
  if (error == 0) {
    Serial.println("FOUND!");
  } else {
    Serial.print("NOT FOUND (error ");
    Serial.print(error);
    Serial.println(")");
  }
  
  // Пробуем адрес 0x69 (альтернативный)
  Wire.beginTransmission(0x69);
  error = Wire.endTransmission();
  
  Serial.print("MPU6050 at 0x69: ");
  if (error == 0) {
    Serial.println("FOUND!");
  } else {
    Serial.print("NOT FOUND (error ");
    Serial.print(error);
    Serial.println(")");
  }
  
  // Обновляем дисплей
  display.clear();
  display.drawString(0, 0, "MPU6050 Test:");
  display.drawString(0, 12, "0x68: " + String((error == 0) ? "OK" : "NO"));
  display.drawString(0, 24, "0x69: " + String((error == 0) ? "OK" : "NO"));
  display.drawString(0, 36, "SDA:" + String(I2C_SDA));
  display.drawString(0, 48, "SCL:" + String(I2C_SCL));
  display.display();
}

void loop() {
  // Простая анимация
  static int counter = 0;
  
  display.clear();
  display.drawString(0, 0, "Heltec V3.1");
  display.drawString(0, 12, "I2C Debug Mode");
  display.drawString(0, 24, "Counter: " + String(counter));
  display.drawString(0, 36, "SDA:" + String(I2C_SDA));
  display.drawString(0, 48, "SCL:" + String(I2C_SCL));
  display.display();
  
  Serial.println("Counter: " + String(counter));
  counter++;
  
  delay(1000);
}
