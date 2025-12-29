// Heltec V3.1 - Стабильная версия (MPU6050 на основном I2C)
#include <Wire.h>
#include "SH1106Wire.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// === ПИНЫ ===
#define OLED_RST        21    // Reset дисплея
#define OLED_VEXT       10    // Питание дисплея (LOW = ON)
#define OLED_ADDR       0x3C  // Адрес дисплея

// Объекты (оба на одном I2C - пины 17,18)
SH1106Wire display(OLED_ADDR, 17, 18); // Дисплей на пинах 17,18
Adafruit_MPU6050 mpu;                  // MPU6050 тоже на пинах 17,18

// Данные
float accelX = 0, accelY = 0, accelZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;
float temp = 0;
bool displayOn = true;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.println("=== HELTEC V3.1 - SIMPLE ===");
  Serial.println("Display + MPU6050 on same I2C");
  
  // === ВКЛЮЧАЕМ ПИТАНИЕ ДИСПЛЕЯ ===
  pinMode(OLED_VEXT, OUTPUT);
  digitalWrite(OLED_VEXT, LOW); // ВКЛЮЧЕНО
  Serial.println("Display power: ON");
  delay(100);
  
  // === СБРОС ДИСПЛЕЯ ===
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  Serial.println("Display reset: DONE");
  
  // === ИНИЦИАЛИЗАЦИЯ I2C (один раз!) ===
  Wire.begin(17, 18); // Фиксированные пины
  Wire.setClock(100000); // 100kHz
  
  // === ИНИЦИАЛИЗАЦИЯ ДИСПЛЕЯ ===
  Serial.print("Initializing display... ");
  if (display.init()) {
    Serial.println("OK");
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    display.clear();
    
    // Показываем стартовый экран
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 0, "HELTEC V3.1");
    display.drawString(64, 20, "Starting...");
    display.display();
  } else {
    Serial.println("FAILED");
  }
  
  delay(1000);
  
  // === ИНИЦИАЛИЗАЦИЯ MPU6050 ===
  Serial.print("Initializing MPU6050... ");
  if (mpu.begin(0x68)) { // Адрес 0x68
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.println("OK");
    
    // Успешная инициализация
    display.clear();
    display.drawString(64, 0, "SYSTEM READY");
    display.drawString(64, 20, "MPU6050: OK");
    display.drawString(64, 40, "Reading data...");
    display.display();
  } else {
    Serial.println("FAILED");
    
    display.clear();
    display.drawString(64, 0, "MPU6050 ERROR");
    display.drawString(64, 20, "Check connection");
    display.drawString(64, 40, "Addr: 0x68");
    display.display();
  }
  
  delay(2000);
  
  // Очищаем для основного режима
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.display();
  
  Serial.println("AX\tAY\tAZ\tGX\tGY\tGZ\tTemp");
}

void loop() {
  // === ПОДДЕРЖИВАЕМ ПИТАНИЕ ДИСПЛЕЯ ===
  static unsigned long lastPowerCheck = 0;
  if (millis() - lastPowerCheck > 10000) { // Каждые 10 секунд
    digitalWrite(OLED_VEXT, LOW);
    lastPowerCheck = millis();
    Serial.println("Display power refreshed");
  }
  
  // === ЧТЕНИЕ ДАННЫХ С MPU6050 ===
  sensors_event_t a, g, temp_event;
  if (mpu.getEvent(&a, &g, &temp_event)) {
    // Сглаживание
    accelX = (accelX * 0.7) + (a.acceleration.x * 0.3);
    accelY = (accelY * 0.7) + (a.acceleration.y * 0.3);
    accelZ = (accelZ * 0.7) + (a.acceleration.z * 0.3);
    
    gyroX = (gyroX * 0.7) + (g.gyro.x * 0.3);
    gyroY = (gyroY * 0.7) + (g.gyro.y * 0.3);
    gyroZ = (gyroZ * 0.7) + (g.gyro.z * 0.3);
    
    temp = temp_event.temperature;
    
    // Serial вывод
    Serial.printf("%.2f\t%.2f\t%.2f\t", accelX, accelY, accelZ);
    Serial.printf("%.2f\t%.2f\t%.2f\t", gyroX, gyroY, gyroZ);
    Serial.printf("%.1fC\n", temp);
    
    // === ОБНОВЛЕНИЕ ДИСПЛЕЯ ===
    display.clear();
    
    // Акселерометр (верх)
    display.drawString(0, 0, "ACCEL:");
    display.drawString(0, 12, "X:" + String(accelX, 1));
    display.drawString(40, 12, "Y:" + String(accelY, 1));
    display.drawString(80, 12, "Z:" + String(accelZ, 1));
    
    // Гироскоп (середина)
    display.drawString(0, 24, "GYRO:");
    display.drawString(0, 36, "X:" + String(gyroX, 1));
    display.drawString(40, 36, "Y:" + String(gyroY, 1));
    display.drawString(80, 36, "Z:" + String(gyroZ, 1));
    
    // Температура (низ)
    display.drawString(0, 48, "Temp: " + String(temp, 1) + "C");
    
    // Мигающий индикатор
    static bool blink = false;
    blink = !blink;
    if (blink) {
      display.fillRect(120, 56, 4, 4);
    }
    
    display.display();
  } else {
    Serial.println("MPU6050 read error");
  }
  
  delay(100); // 10 FPS
}
