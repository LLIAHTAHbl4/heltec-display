// Heltec V3.1 с MPU6050 - ЛЕГКОДОСТУПНЫЕ пины
#include <Wire.h>
#include "SH1106Wire.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// === ВЫБЕРИ ЛЮБОЙ ВАРИАНТ ПИНОВ ===

// Вариант A: GPIO8 и GPIO9 (стандартные альтернативные)
// #define I2C_SDA     8
// #define I2C_SCL     9

// Вариант B: GPIO33 и GPIO34 (свободные пины на краю платы)
#define I2C_SDA     33
#define I2C_SCL     34

// Вариант C: GPIO1 и GPIO2 (TX/RX, осторожно - могут мешать Serial)
// #define I2C_SDA     1
// #define I2C_SCL     2

// Вариант D: GPIO4 и GPIO5 (часто свободные)
// #define I2C_SDA     4
// #define I2C_SCL     5

// Остальные пины (не меняются)
#define OLED_RST    21    // Reset дисплея
#define OLED_VEXT   10    // Питание дисплея (LOW = ON)
#define OLED_ADDR   0x3C  // Адрес дисплея

// Объекты
SH1106Wire display(OLED_ADDR, I2C_SDA, I2C_SCL);
Adafruit_MPU6050 mpu;

// Переменные
float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float temp;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println();
  Serial.println("==========================================");
  Serial.println("Heltec V3.1 - Легкодоступные пины I2C");
  Serial.print("SDA: GPIO"); Serial.println(I2C_SDA);
  Serial.print("SCL: GPIO"); Serial.println(I2C_SCL);
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
  
  // 3. Инициализация I2C на новых пинах
  Serial.println("[3] Starting I2C on new pins...");
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // 4. Инициализация дисплея
  Serial.println("[4] Initializing OLED display...");
  if (!display.init()) {
    Serial.println("OLED init failed! Check I2C pins.");
    while(1);
  }
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  
  // Стартовый экран
  display.drawString(0, 0, "Heltec V3.1");
  display.drawString(0, 12, "SDA:" + String(I2C_SDA));
  display.drawString(0, 24, "SCL:" + String(I2C_SCL));
  display.drawString(0, 36, "Init MPU6050...");
  display.display();
  
  // 5. Инициализация MPU6050
  Serial.println("[5] Initializing MPU6050...");
  delay(1000); // Даем время дисплею
  
  if (!mpu.begin(0x68, &Wire, 0)) {
    Serial.println("MPU6050 not found!");
    display.clear();
    display.drawString(0, 0, "MPU6050 ERROR!");
    display.drawString(0, 12, "Check connection");
    display.drawString(0, 24, "SDA:" + String(I2C_SDA));
    display.drawString(0, 36, "SCL:" + String(I2C_SCL));
    display.display();
    while (1) {
      delay(1000);
    }
  }
  
  // Настройка MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  Serial.println("MPU6050 OK!");
  
  // Успешная инициализация
  display.clear();
  display.drawString(0, 0, "SYSTEM READY");
  display.drawString(0, 12, "MPU6050: OK");
  display.drawString(0, 24, "Pins SDA:" + String(I2C_SDA));
  display.drawString(0, 36, "Pins SCL:" + String(I2C_SCL));
  display.display();
  
  delay(2000);
  
  Serial.println("==========================================");
  Serial.println("AX\tAY\tAZ\tGX\tGY\tGZ\tTemp");
  Serial.println("==========================================");
}

void loop() {
  // Чтение данных
  sensors_event_t a, g, temp_event;
  if (!mpu.getEvent(&a, &g, &temp_event)) {
    Serial.println("MPU6050 read error!");
    delay(1000);
    return;
  }
  
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
  
  // OLED вывод (оптимизированный)
  display.clear();
  
  // Акселерометр
  display.drawString(0, 0, "ACCEL:");
  display.drawString(0, 12, "X:" + String(accelX, 1));
  display.drawString(40, 12, "Y:" + String(accelY, 1));
  display.drawString(80, 12, "Z:" + String(accelZ, 1));
  
  // Гироскоп
  display.drawString(0, 24, "GYRO:");
  display.drawString(0, 36, "X:" + String(gyroX, 1));
  display.drawString(40, 36, "Y:" + String(gyroY, 1));
  display.drawString(80, 36, "Z:" + String(gyroZ, 1));
  
  // Температура и пины
  display.drawString(0, 48, "T:" + String(temp, 1) + "C");
  display.drawString(40, 48, "SDA:" + String(I2C_SDA));
  display.drawString(80, 48, "SCL:" + String(I2C_SCL));
  
  display.display();
  
  delay(150); // Частота обновления
}
