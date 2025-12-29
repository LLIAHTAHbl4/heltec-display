// Heltec V3.1 - MPU6050 на пинах 8,9 через основной Wire
#include <Wire.h>
#include "SH1106Wire.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// === ПИНЫ ПО ДОКУМЕНТАЦИИ HELTEC ===
#define DISPLAY_SDA     17    // Дисплей SDA (не трогаем!)
#define DISPLAY_SCL     18    // Дисплей SCL (не трогаем!)
#define OLED_RST        21    // Reset дисплея
#define OLED_VEXT       10    // Питание дисплея (LOW = ON)
#define OLED_ADDR       0x3C  // Адрес дисплея

// === ПИНЫ ДЛЯ MPU6050 (подключаем сюда) ===
#define MPU_SDA         8     // MPU6050 SDA
#define MPU_SCL         9     // MPU6050 SCL
#define MPU_ADDR        0x68  // Адрес MPU6050

// Объекты (используем один Wire, но переназначаем пины)
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
  Serial.println("Heltec V3.1 - MPU6050 on GPIO8, GPIO9");
  Serial.println("Display: SDA:17, SCL:18 (I2C0)");
  Serial.println("MPU6050: SDA:8, SCL:9");
  Serial.println("==========================================");
  
  // === 1. ИНИЦИАЛИЗАЦИЯ ДИСПЛЕЯ ===
  Serial.println("[1] Initializing display...");
  
  // Включаем питание дисплея
  pinMode(OLED_VEXT, OUTPUT);
  digitalWrite(OLED_VEXT, LOW);
  delay(100);
  
  // Сброс дисплея
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  
  // Инициализация I2C для дисплея
  Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
  
  // Инициализация дисплея
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  
  display.drawString(0, 0, "Heltec V3.1");
  display.drawString(0, 12, "Init Display...");
  display.display();
  
  Serial.println("Display: OK");
  
  // === 2. ИНИЦИАЛИЗАЦИЯ MPU6050 ===
  Serial.println("[2] Initializing MPU6050...");
  
  // ПЕРЕИНИЦИАЛИЗИРУЕМ Wire с пинами для MPU6050
  Wire.end(); // Останавливаем текущий I2C
  delay(50);
  Wire.begin(MPU_SDA, MPU_SCL); // Запускаем с новыми пинами
  Wire.setClock(100000); // 100kHz для надежности
  
  // Инициализация MPU6050
  if (!mpu.begin(MPU_ADDR)) {
    Serial.println("MPU6050 not found!");
    
    // Возвращаем Wire для дисплея
    Wire.end();
    Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
    
    display.clear();
    display.drawString(0, 0, "MPU6050 ERROR!");
    display.drawString(0, 12, "Check wiring:");
    display.drawString(0, 24, "SDA: GPIO" + String(MPU_SDA));
    display.drawString(0, 36, "SCL: GPIO" + String(MPU_SCL));
    display.drawString(0, 48, "Addr: 0x68");
    display.display();
    
    mpuInitialized = false;
  } else {
    // Настройка MPU6050
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    
    Serial.println("MPU6050: OK");
    mpuInitialized = true;
    
    // Возвращаем Wire для дисплея (после настройки MPU)
    Wire.end();
    Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
  }
  
  // Успешная инициализация
  display.clear();
  if (mpuInitialized) {
    display.drawString(0, 0, "SYSTEM READY");
    display.drawString(0, 12, "Display: GPIO17,18");
    display.drawString(0, 24, "MPU6050: GPIO8,9");
    display.drawString(0, 36, "Addr: 0x68");
    display.drawString(0, 48, "Status: OK");
  } else {
    display.drawString(0, 0, "SYSTEM PARTIAL");
    display.drawString(0, 12, "Display: OK");
    display.drawString(0, 24, "MPU6050: FAILED");
    display.drawString(0, 36, "Check GPIO8,9");
    display.drawString(0, 48, "Using display only");
  }
  display.display();
  
  delay(2000);
  
  if (mpuInitialized) {
    Serial.println("==========================================");
    Serial.println("AX\tAY\tAZ\tGX\tGY\tGZ\tTemp");
    Serial.println("==========================================");
  }
}

void loop() {
  if (mpuInitialized) {
    // === ЧТЕНИЕ С MPU6050 ===
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
    
    // Возвращаемся к пинам дисплея
    Wire.end();
    Wire.begin(DISPLAY_SDA, DISPLAY_SCL);
  }
  
  // === ОБНОВЛЕНИЕ ДИСПЛЕЯ ===
  display.clear();
  
  if (mpuInitialized) {
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
  } else {
    // Только дисплей
    static int counter = 0;
    display.drawString(0, 0, "Heltec V3.1");
    display.drawString(0, 12, "MPU6050 not found");
    display.drawString(0, 24, "Using GPIO8,9");
    display.drawString(0, 36, "Check connection");
    display.drawString(0, 48, "Count: " + String(counter++));
    
    Serial.println("MPU6050 not connected");
  }
  
  display.display();
  
  delay(mpuInitialized ? 100 : 1000);
}
