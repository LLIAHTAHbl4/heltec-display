#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== ULTIMATE DISPLAY TEST ===");
  
  // Все возможные пины питания дисплея
  int powerPins[] = {2, 5, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33};
  
  // Все возможные пары I2C пинов
  int i2cPairs[][2] = {
    {15, 4}, {4, 15}, 
    {17, 18}, {18, 17},
    {21, 22}, {22, 21},
    {8, 9}, {9, 8},
    {1, 2}, {2, 1},
    {6, 7}, {7, 6},
    {10, 11}, {11, 10},
    {33, 32}, {32, 33}
  };
  
  // Перебираем все комбинации
  for (int p = 0; p < 17; p++) {
    int powerPin = powerPins[p];
    
    for (int i = 0; i < 14; i++) {
      int sda = i2cPairs[i][0];
      int scl = i2cPairs[i][1];
      
      Serial.print("\nTest: Power=");
      Serial.print(powerPin);
      Serial.print(" SDA=");
      Serial.print(sda);
      Serial.print(" SCL=");
      Serial.println(scl);
      
      // Пробуем включить питание
      pinMode(powerPin, OUTPUT);
      digitalWrite(powerPin, HIGH);
      delay(100);
      
      // Пробуем I2C
      Wire.begin(sda, scl);
      delay(50);
      
      // Проверяем адреса
      Wire.beginTransmission(0x3C);
      bool found3C = (Wire.endTransmission() == 0);
      
      Wire.beginTransmission(0x3D);
      bool found3D = (Wire.endTransmission() == 0);
      
      if (found3C || found3D) {
        Serial.println("*** DISPLAY FOUND! ***");
        if (found3C) Serial.println("Address: 0x3C");
        if (found3D) Serial.println("Address: 0x3D");
        Serial.print("Use: Power=");
        Serial.print(powerPin);
        Serial.print(" SDA=");
        Serial.print(sda);
        Serial.print(" SCL=");
        Serial.println(scl);
        Serial.println("=== STOP TEST ===");
        while(1); // Останавливаем
      }
      
      // Выключаем питание
      digitalWrite(powerPin, LOW);
      delay(50);
    }
  }
  
  Serial.println("\n=== NO DISPLAY FOUND ===");
  Serial.println("Check physically:");
  Serial.println("1. Is display connected?");
  Serial.println("2. Look for jumpers on back");
  Serial.println("3. Maybe display is broken");
}

void loop() {}
