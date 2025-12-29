#include <Wire.h>

void setup() {
  Serial.begin(115200);
  
  // Пробуем все возможные комбинации пинов
  int sdaPins[] = {15, 4, 17, 18, 8, 21};
  int sclPins[] = {4, 15, 18, 17, 9, 22};
  
  for (int i = 0; i < 6; i++) {
    Serial.print("\nTesting SDA:");
    Serial.print(sdaPins[i]);
    Serial.print(" SCL:");
    Serial.println(sclPins[i]);
    
    Wire.begin(sdaPins[i], sclPins[i]);
    delay(100);
    
    // Сканируем адреса
    scanI2C();
  }
}

void scanI2C() {
  byte error, address;
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Found at 0x");
      if(address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }
}

void loop() {}
