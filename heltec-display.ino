// Heltec V3.1 - Оптимизированный вывод MPU6050 (исправленная версия)
#include <Wire.h>
#include "SH1106Wire.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// === ПИНЫ ===
#define DISPLAY_SDA     17    // Дисплей SDA
#define DISPLAY_SCL     18    // Дисплей SCL
#define OLED_RST        21    // Reset дисплея
#define OLED_VEXT       10    // Питание дисплея (LOW = ON)
#define OLED_ADDR       0x3C  // Адрес дисплея

#define MPU_SDA         4     // MPU6050 SDA
#define MPU_SCL         5     // MPU6050 SCL
#define MPU_ADDR        0x68  // Адрес MPU6050

// Объекты
SH1106Wire display(OLED_ADDR, DISPLAY_SDA, DISPLAY_SCL);
Adafruit_MPU6050 mpu;

// Данные
float accelX = 0, accelY = 0, accelZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;
float temp = 0;
bool mpuInitialized = false;
unsigned long lastDisplayUpdate = 0;
unsigned long lastDataRead = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.println("Heltec V3.1 - Optimized MPU6050");
  Serial.println("Display: GPIO17,18 | MPU: GPIO4,5");
  
  // === ВКЛЮЧАЕМ ПИТАНИЕ ДИСПЛЕЯ ===
  pinMode(OLED_VEXT, OUTPUT);
  digitalWrite(OLED_VEXT, LOW);
  delay(100);
  
  // === СБРОС ДИСПЛЕЯ ===
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  
  // === ИНИЦИАЛИЗАЦИЯ ДИСПЛЕЯ ===
  Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  
  // Стартовый экран
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "HELTEC V3.1");
  display.drawString(64, 15, "Initializing...");
  display.drawString(64, 30, "MPU6050 Test");
  display.display();
  
  Serial.println("Display initialized");
  
  // === ИНИЦИАЛИЗАЦИЯ MPU6050 ===
  delay(1000);
  
  // Переключаемся на пины MPU6050
  Wire.end();
  Wire.begin(MPU_SDA, MPU_SCL);
  Wire.setClock(100000);
  
  if (mpu.begin(MPU_ADDR)) {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    
    Serial.println("MPU6050: OK");
    mpuInitialized = true;
  } else {
    Serial.println("MPU6050: Not found");
    mpuInitialized = false;
  }
  
  // Возвращаем пины для дисплея
  Wire.end();
  Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
  
  // Финальный стартовый экран
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  if (mpuInitialized) {
    display.drawString(0, 0, "MPU6050 READY");
    display.drawString(0, 15, "Accel: 8G");
    display.drawString(0, 30, "Gyro: 500°/s");
    display.drawString(0, 45, "Display: 128x64");
  } else {
    display.drawString(0, 0, "MPU6050 ERROR");
    display.drawString(0, 15, "Check GPIO4,5");
    display.drawString(0, 30, "SDA:4, SCL:5");
    display.drawString(0, 45, "VCC:3.3V, GND:GND");
  }
  display.display();
  
  delay(2000);
  
  // Очищаем для основного режима
  display.clear();
  display.display();
  
  if (mpuInitialized) {
    Serial.println("AX\tAY\tAZ\tGX\tGY\tGZ\tTemp");
  }
}

void readSensorData() {
  if (!mpuInitialized) return;
  
  unsigned long currentTime = millis();
  if (currentTime - lastDataRead < 50) return;
  
  // Переключаемся на MPU6050
  Wire.end();
  Wire.begin(MPU_SDA, MPU_SCL);
  
  sensors_event_t a, g, temp_event;
  if (mpu.getEvent(&a, &g, &temp_event)) {
    // Фильтрация
    accelX = (accelX * 0.8) + (a.acceleration.x * 0.2);
    accelY = (accelY * 0.8) + (a.acceleration.y * 0.2);
    accelZ = (accelZ * 0.8) + (a.acceleration.z * 0.2);
    
    gyroX = (gyroX * 0.8) + (g.gyro.x * 0.2);
    gyroY = (gyroY * 0.8) + (g.gyro.y * 0.2);
    gyroZ = (gyroZ * 0.8) + (g.gyro.z * 0.2);
    
    temp = temp_event.temperature;
    
    // Serial вывод
    Serial.printf("%.2f\t%.2f\t%.2f\t", accelX, accelY, accelZ);
    Serial.printf("%.2f\t%.2f\t%.2f\t", gyroX, gyroY, gyroZ);
    Serial.printf("%.1fC\n", temp);
  }
  
  // Возвращаемся к дисплею
  Wire.end();
  Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
  
  lastDataRead = currentTime;
}

void updateDisplay() {
  unsigned long currentTime = millis();
  if (currentTime - lastDisplayUpdate < 200) return;
  
  display.clear();
  
  if (mpuInitialized) {
    // === АКСЕЛЕРОМЕТР (слева) ===
    display.drawString(0, 0, "ACCEL (m/s²):");
    
    // X
    display.drawString(0, 12, "X:");
    int barX = map(constrain(accelX, -10, 10), -10, 10, 0, 40);
    display.fillRect(20, 12, barX, 8);
    display.drawString(70, 12, String(accelX, 1));
    
    // Y
    display.drawString(0, 24, "Y:");
    int barY = map(constrain(accelY, -10, 10), -10, 10, 0, 40);
    display.fillRect(20, 24, barY, 8);
    display.drawString(70, 24, String(accelY, 1));
    
    // Z
    display.drawString(0, 36, "Z:");
    int barZ = map(constrain(accelZ, -10, 10), -10, 10, 0, 40);
    display.fillRect(20, 36, barZ, 8);
    display.drawString(70, 36, String(accelZ, 1));
    
    // === ГИРОСКОП (справа) ===
    display.drawString(85, 0, "GYRO (°/s):");
    display.drawString(85, 12, "X:" + String(gyroX, 1));
    display.drawString(85, 24, "Y:" + String(gyroY, 1));
    display.drawString(85, 36, "Z:" + String(gyroZ, 1));
    
    // === ТЕМПЕРАТУРА И ИНДИКАТОР ===
    display.drawString(0, 48, "Temp: " + String(temp, 1) + "C");
    
    // Индикатор активности (мигающий квадратик)
    static bool blink = false;
    blink = !blink;
    if (blink) {
      display.fillRect(120, 53, 6, 6);
    } else {
      display.drawRect(120, 53, 6, 6);
    }
    
  } else {
    // Режим без датчика
    static int counter = 0;
    display.drawString(0, 0, "NO MPU6050");
    display.drawString(0, 15, "Connect to:");
    display.drawString(0, 30, "GPIO4 (SDA)");
    display.drawString(0, 45, "GPIO5 (SCL)");
    
    // Мигающий индикатор
    if (counter % 10 == 0) {
      display.fillRect(120, 53, 6, 6);
    } else {
      display.drawRect(120, 53, 6, 6);
    }
    counter++;
  }
  
  display.display();
  lastDisplayUpdate = currentTime;
}

void loop() {
  readSensorData();
  updateDisplay();
  
  // Проверяем питание дисплея каждые 30 секунд
  static unsigned long lastPowerCheck = 0;
  if (millis() - lastPowerCheck > 30000) {
    digitalWrite(OLED_VEXT, LOW);
    lastPowerCheck = millis();
  }
  
  delay(10);
}
