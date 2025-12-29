// Heltec V3.1 - MPU6050 на GPIO4 и GPIO5
#include <Wire.h>
#include "SH1106Wire.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// === ДИСПЛЕЙ (не трогаем) ===
#define DISPLAY_SDA     17    // SDA дисплея
#define DISPLAY_SCL     18    // SCL дисплея
#define OLED_RST        21    // Reset дисплея
#define OLED_VEXT       10    // Питание дисплея (LOW = ON)
#define OLED_ADDR       0x3C  // Адрес дисплея

// === MPU6050 (подключаем СЮДА) ===
// ВАРИАНТ 1: GPIO4 и GPIO5 (рекомендую - обычно свободны)
#define MPU_SDA         4     // ← ПОДКЛЮЧИ СЮДА
#define MPU_SCL         5     // ← ПОДКЛЮЧИ СЮДА

// ВАРИАНТ 2: GPIO33 и GPIO34 (если 4,5 заняты)
// #define MPU_SDA         33    // ← Альтернатива
// #define MPU_SCL         34    // ← Альтернатива

// ВАРИАНТ 3: GPIO12 и GPIO13 (если другие заняты)
// #define MPU_SDA         12    // ← Альтернатива
// #define MPU_SCL         13    // ← Альтернатива

#define MPU_ADDR        0x68  // Адрес MPU6050

// Объекты
SH1106Wire display(OLED_ADDR, DISPLAY_SDA, DISPLAY_SCL);
Adafruit_MPU6050 mpu;

// Данные
float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float temp;
bool mpuInitialized = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.println("==========================================");
  Serial.println("Heltec V3.1 - MPU6050 Test");
  Serial.print("Display: SDA="); Serial.print(DISPLAY_SDA);
  Serial.print(" SCL="); Serial.println(DISPLAY_SCL);
  Serial.print("MPU6050: SDA="); Serial.print(MPU_SDA);
  Serial.print(" SCL="); Serial.println(MPU_SCL);
  Serial.println("==========================================");
  
  // === 1. ВКЛЮЧАЕМ ДИСПЛЕЙ ===
  Serial.println("[1] Powering display...");
  pinMode(OLED_VEXT, OUTPUT);
  digitalWrite(OLED_VEXT, LOW);
  delay(100);
  
  // === 2. ИНИЦИАЛИЗАЦИЯ ДИСПЛЕЯ ===
  Serial.println("[2] Initializing display...");
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  
  Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  
  display.drawString(0, 0, "Heltec V3.1");
  display.drawString(0, 12, "Init MPU6050...");
  display.drawString(0, 24, "SDA:" + String(MPU_SDA));
  display.drawString(0, 36, "SCL:" + String(MPU_SCL));
  display.display();
  
  Serial.println("Display: OK");
  
  // === 3. ИНИЦИАЛИЗАЦИЯ MPU6050 ===
  Serial.println("[3] Initializing MPU6050...");
  
  // Временная смена пинов для MPU6050
  Wire.end();
  delay(50);
  Wire.begin(MPU_SDA, MPU_SCL);
  Wire.setClock(100000); // Медленнее для надежности
  
  // Пробуем найти MPU6050
  if (!mpu.begin(MPU_ADDR)) {
    Serial.println("MPU6050 not found!");
    
    // Возвращаем пины для дисплея
    Wire.end();
    Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
    
    display.clear();
    display.drawString(0, 0, "MPU6050 ERROR!");
    display.drawString(0, 12, "Check connection:");
    display.drawString(0, 24, "SDA->GPIO" + String(MPU_SDA));
    display.drawString(0, 36, "SCL->GPIO" + String(MPU_SCL));
    display.drawString(0, 48, "VCC->3.3V, GND->GND");
    display.display();
    
    mpuInitialized = false;
  } else {
    // Настройка MPU6050
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    
    Serial.println("MPU6050: OK");
    mpuInitialized = true;
    
    // Возвращаем пины для дисплея
    Wire.end();
    Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
  }
  
  // === 4. СТАРТОВЫЙ ЭКРАН ===
  display.clear();
  if (mpuInitialized) {
    display.drawString(0, 0, "SYSTEM READY!");
    display.drawString(0, 12, "Display: GPIO17,18");
    display.drawString(0, 24, "MPU6050: OK");
    display.drawString(0, 36, "Pins: " + String(MPU_SDA) + "," + String(MPU_SCL));
    display.drawString(0, 48, "Reading data...");
  } else {
    display.drawString(0, 0, "MPU6050 MISSING");
    display.drawString(0, 12, "Display working");
    display.drawString(0, 24, "Connect MPU6050:");
    display.drawString(0, 36, "SDA->GPIO" + String(MPU_SDA));
    display.drawString(0, 48, "SCL->GPIO" + String(MPU_SCL));
  }
  display.display();
  
  delay(2000);
  
  if (mpuInitialized) {
    Serial.println("==========================================");
    Serial.println("AX\tAY\tAZ\tGX\tGY\tGZ\tTemp");
    Serial.println("==========================================");
  }
}

void readMPU6050() {
  if (!mpuInitialized) return;
  
  // Переключаемся на пины MPU6050
  Wire.end();
  Wire.begin(MPU_SDA, MPU_SCL);
  
  sensors_event_t a, g, temp_event;
  if (mpu.getEvent(&a, &g, &temp_event)) {
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
  }
  
  // Возвращаемся к дисплею
  Wire.end();
  Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
}

void updateDisplay() {
  display.clear();
  
  if (mpuInitialized) {
    // Акселерометр
    display.drawString(0, 0, "ACCEL (m/s²):");
    display.drawString(0, 12, "X:" + String(accelX, 1));
    display.drawString(45, 12, "Y:" + String(accelY, 1));
    display.drawString(90, 12, "Z:" + String(accelZ, 1));
    
    // Гироскоп
    display.drawString(0, 24, "GYRO (°/s):");
    display.drawString(0, 36, "X:" + String(gyroX, 1));
    display.drawString(45, 36, "Y:" + String(gyroY, 1));
    display.drawString(90, 36, "Z:" + String(gyroZ, 1));
    
    // Температура
    display.drawString(0, 48, "T:" + String(temp, 1) + "°C");
  } else {
    // Только дисплей
    static int counter = 0;
    display.drawString(0, 0, "Heltec V3.1");
    display.drawString(0, 12, "No MPU6050");
    display.drawString(0, 24, "Connect to:");
    display.drawString(0, 36, "GPIO" + String(MPU_SDA) + " (SDA)");
    display.drawString(0, 48, "GPIO" + String(MPU_SCL) + " (SCL)");
    
    if (counter % 10 == 0) {
      Serial.println("Waiting for MPU6050 on GPIO" + String(MPU_SDA) + "," + String(MPU_SCL));
    }
    counter++;
  }
  
  display.display();
}

void loop() {
  readMPU6050();
  updateDisplay();
  delay(mpuInitialized ? 100 : 1000);
}
