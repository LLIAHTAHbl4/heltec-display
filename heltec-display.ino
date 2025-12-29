// Heltec V3.1 с MPU6050 и OLED дисплеем
#include <Wire.h>
#include "SH1106Wire.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Пины Heltec V3.1
#define I2C_SDA     17    // SDA для дисплея и MPU6050
#define I2C_SCL     18    // SCL для дисплея и MPU6050
#define OLED_RST    21    // Reset дисплея
#define OLED_VEXT   10    // Питание дисплея (LOW = ON)
#define OLED_ADDR   0x3C  // Адрес дисплея

// Объекты
SH1106Wire display(OLED_ADDR, I2C_SDA, I2C_SCL);
Adafruit_MPU6050 mpu;

// Переменные для фильтрации (уменьшение дребезга)
float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float temp;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.println("==========================================");
  Serial.println("Heltec V3.1 with MPU6050 Accelerometer");
  Serial.println("==========================================");
  
  // 1. Включаем питание дисплея
  Serial.println("[1] Powering OLED display...");
  pinMode(OLED_VEXT, OUTPUT);
  digitalWrite(OLED_VEXT, LOW);
  delay(100);
  
  // 2. Сброс дисплея
  Serial.println("[2] Resetting display...");
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  
  // 3. Инициализация I2C
  Serial.println("[3] Starting I2C bus...");
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // 4. Инициализация дисплея
  Serial.println("[4] Initializing OLED display...");
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10); // Меньший шрифт для больше данных
  display.clear();
  
  // Вывод стартового сообщения
  display.drawString(0, 0, "Heltec V3.1");
  display.drawString(0, 12, "MPU6050 Test");
  display.drawString(0, 24, "Initializing...");
  display.display();
  
  // 5. Инициализация MPU6050
  Serial.println("[5] Initializing MPU6050...");
  if (!mpu.begin(0x68, &Wire, 0)) { // Адрес 0x68
    Serial.println("Failed to find MPU6050 chip!");
    display.clear();
    display.drawString(0, 0, "MPU6050 ERROR!");
    display.drawString(0, 20, "Check connection");
    display.display();
    while (1) {
      delay(10);
    }
  }
  
  // Настройка MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);     // ±8G
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);          // ±500°/с
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);       // Фильтр 21Hz
  
  Serial.println("MPU6050 initialized successfully!");
  
  // Стартовый экран
  display.clear();
  display.drawString(0, 0, "Heltec V3.1 Ready");
  display.drawString(0, 12, "MPU6050: OK");
  display.drawString(0, 24, "Reading data...");
  display.display();
  
  delay(2000);
  
  Serial.println("==========================================");
  Serial.println("AX\tAY\tAZ\tGX\tGY\tGZ\tTemp");
  Serial.println("==========================================");
}

void loop() {
  // Чтение данных с MPU6050
  sensors_event_t a, g, temp_event;
  mpu.getEvent(&a, &g, &temp_event);
  
  // Фильтрация (простой усредняющий фильтр)
  accelX = (accelX * 0.7) + (a.acceleration.x * 0.3);
  accelY = (accelY * 0.7) + (a.acceleration.y * 0.3);
  accelZ = (accelZ * 0.7) + (a.acceleration.z * 0.3);
  
  gyroX = (gyroX * 0.7) + (g.gyro.x * 0.3);
  gyroY = (gyroY * 0.7) + (g.gyro.y * 0.3);
  gyroZ = (gyroZ * 0.7) + (g.gyro.z * 0.3);
  
  temp = temp_event.temperature;
  
  // Вывод в Serial монитор
  Serial.printf("%.2f\t%.2f\t%.2f\t", accelX, accelY, accelZ);
  Serial.printf("%.2f\t%.2f\t%.2f\t", gyroX, gyroY, gyroZ);
  Serial.printf("%.1fC\n", temp);
  
  // Обновление дисплея
  display.clear();
  
  // Акселерометр (верхняя часть)
  display.drawString(0, 0, "Accelerometer:");
  display.drawString(0, 12, "X:" + String(accelX, 1) + " m/s²");
  display.drawString(0, 24, "Y:" + String(accelY, 1) + " m/s²");
  display.drawString(0, 36, "Z:" + String(accelZ, 1) + " m/s²");
  
  // Гироскоп и температура (нижняя часть)
  display.drawString(64, 0, "Gyro:");
  display.drawString(64, 12, "X:" + String(gyroX, 1) + "°/s");
  display.drawString(64, 24, "Y:" + String(gyroY, 1) + "°/s");
  display.drawString(64, 36, "Z:" + String(gyroZ, 1) + "°/s");
  
  // Температура в самом низу
  display.drawString(0, 48, "Temp: " + String(temp, 1) + " C");
  
  // Индикация активности
  static bool blink = false;
  blink = !blink;
  display.drawString(110, 48, blink ? "*" : " ");
  
  display.display();
  
  // Задержка для стабильного отображения
  delay(100);
}
