// Heltec V3.1 - Два устройства на разных портах I2C
#include <Wire.h>
#include "SH1106Wire.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// === ПОРТ I2C0: ДЛЯ ДИСПЛЕЯ (не трогаем!) ===
#define DISPLAY_SDA     17    // I2C0 SDA - дисплей
#define DISPLAY_SCL     18    // I2C0 SCL - дисплей
#define OLED_RST        21    // Reset дисплея
#define OLED_VEXT       10    // Питание дисплея (LOW = ON)
#define OLED_ADDR       0x3C  // Адрес дисплея

// === ПОРТ I2C1: ДЛЯ MPU6050 ===
#define MPU_SDA         8     // I2C1 SDA - MPU6050
#define MPU_SCL         9     // I2C1 SCL - MPU6050
#define MPU_ADDR        0x68  // Адрес MPU6050

// Два отдельных объекта I2C
TwoWire I2C_DISPLAY = TwoWire(0); // Пор I2C0
TwoWire I2C_MPU = TwoWire(1);     // Пор I2C1

// Объекты устройств на разных портах
SH1106Wire display(OLED_ADDR, DISPLAY_SDA, DISPLAY_SCL);
Adafruit_MPU6050 mpu(&I2C_MPU); // Указываем использовать I2C1

// Данные
float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float temp;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.println("==========================================");
  Serial.println("Heltec V3.1 - Dual I2C Ports");
  Serial.println("Display: I2C0 (SDA:17, SCL:18)");
  Serial.println("MPU6050: I2C1 (SDA:8, SCL:9)");
  Serial.println("==========================================");
  
  // === 1. ИНИЦИАЛИЗАЦИЯ ДИСПЛЕЯ (I2C0) ===
  Serial.println("[1] Initializing display on I2C0...");
  
  // Включаем питание
  pinMode(OLED_VEXT, OUTPUT);
  digitalWrite(OLED_VEXT, LOW);
  delay(100);
  
  // Сброс
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  
  // Инициализация I2C0 для дисплея
  I2C_DISPLAY.begin(DISPLAY_SDA, DISPLAY_SCL);
  
  // Инициализация дисплея
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  
  display.drawString(0, 0, "Heltec V3.1");
  display.drawString(0, 12, "Dual I2C Test");
  display.drawString(0, 24, "Display: I2C0");
  display.drawString(0, 36, "MPU: I2C1");
  display.display();
  
  Serial.println("Display: OK");
  
  // === 2. ИНИЦИАЛИЗАЦИЯ MPU6050 (I2C1) ===
  Serial.println("[2] Initializing MPU6050 on I2C1...");
  delay(1000);
  
  // Инициализация I2C1 для MPU6050
  I2C_MPU.begin(MPU_SDA, MPU_SCL);
  
  // Инициализация MPU6050
  if (!mpu.begin(MPU_ADDR, &I2C_MPU, 0)) {
    Serial.println("MPU6050 not found on I2C1!");
    
    display.clear();
    display.drawString(0, 0, "MPU6050 ERROR!");
    display.drawString(0, 12, "Check I2C1 wiring");
    display.drawString(0, 24, "SDA: GPIO" + String(MPU_SDA));
    display.drawString(0, 36, "SCL: GPIO" + String(MPU_SCL));
    display.drawString(0, 48, "Addr: 0x" + String(MPU_ADDR, HEX));
    display.display();
    
    while (1) {
      delay(1000);
    }
  }
  
  // Настройка MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  Serial.println("MPU6050: OK");
  
  // Успешная инициализация
  display.clear();
  display.drawString(0, 0, "SYSTEM READY");
  display.drawString(0, 12, "Display: I2C0 OK");
  display.drawString(0, 24, "MPU6050: I2C1 OK");
  display.drawString(0, 36, "SDA:8 SCL:9");
  display.drawString(0, 48, "Addr: 0x68");
  display.display();
  
  delay(2000);
  
  Serial.println("==========================================");
  Serial.println("AX\tAY\tAZ\tGX\tGY\tGZ\tTemp");
  Serial.println("==========================================");
}

void loop() {
  // Чтение с MPU6050 через I2C1
  sensors_event_t a, g, temp_event;
  mpu.getEvent(&a, &g, &temp_event);
  
  // Фильтрация
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
  
  // Обновление дисплея через I2C0
  display.clear();
  
  // Акселерометр
  display.drawString(0, 0, "ACCEL (m/s²):");
  display.drawString(0, 12, "X:" + String(accelX, 1));
  display.drawString(40, 12, "Y:" + String(accelY, 1));
  display.drawString(80, 12, "Z:" + String(accelZ, 1));
  
  // Гироскоп
  display.drawString(0, 24, "GYRO (°/s):");
  display.drawString(0, 36, "X:" + String(gyroX, 1));
  display.drawString(40, 36, "Y:" + String(gyroY, 1));
  display.drawString(80, 36, "Z:" + String(gyroZ, 1));
  
  // Температура
  display.drawString(0, 48, "Temp: " + String(temp, 1) + "°C");
  
  display.display();
  
  delay(100);
}
