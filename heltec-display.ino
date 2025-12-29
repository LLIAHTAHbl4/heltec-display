/* 
 *  Проект для Heltec WiFi Kit 32 V3.1 с датчиками AHT10 и SHT31
 *  Показания выводятся на OLED-дисплей и в Serial Monitor
 */

// --- БИБЛИОТЕКИ ---
#include "SH1106Wire.h"           // Для дисплея SH1106
#include <Wire.h>                 // Для работы с I2C
#include <Adafruit_AHTX0.h>       // Для AHT10/AHT20
#include <Adafruit_SHT31.h>       // Для SHT31-D

// --- КОНСТАНТЫ ДИСПЛЕЯ (FIXED!) ---
#define OLED_VEXT     10          // Управление питанием дисплея (LOW = ВКЛ)
#define OLED_RST      21          // Сброс дисплея
#define OLED_SDA      17          // SDA дисплея - НЕ МЕНЯТЬ!
#define OLED_SCL      18          // SCL дисплея - НЕ МЕНЯТЬ!
#define OLED_ADDR     0x3C        // Адрес I2C дисплея

// --- КОНСТАНТЫ I2C ДЛЯ ДАТЧИКОВ ---
#define I2C_SDA       4           // SDA для датчиков (GPIO4)
#define I2C_SCL       5           // SCL для датчиков (GPIO5)

// --- АДРЕСА I2C ДАТЧИКОВ ---
#define AHT10_ADDR    0x38        // Адрес AHT10 (AC5600)
#define SHT31_ADDR    0x44        // Адрес SHT31-D (может быть 0x45)

// --- ОБЪЕКТЫ ---
SH1106Wire display(OLED_ADDR, OLED_SDA, OLED_SCL);  // Объект дисплея
Adafruit_AHTX0 aht;                                 // Объект AHT10
Adafruit_SHT31 sht31 = Adafruit_SHT31();            // Объект SHT31

// --- ПЕРЕМЕННЫЕ ---
bool ahtFound = false;   // Флаг обнаружения AHT10
bool shtFound = false;   // Флаг обнаружения SHT31
unsigned long lastUpdate = 0;  // Для интервала обновления
const int UPDATE_INTERVAL = 2000;  // Обновление каждые 2 сек

void setup() {
  Serial.begin(115200);
  delay(100);
  
  // --- ИНИЦИАЛИЗАЦИЯ ДИСПЛЕЯ (ОБЯЗАТЕЛЬНАЯ ПОСЛЕДОВАТЕЛЬНОСТЬ!) ---
  pinMode(OLED_VEXT, OUTPUT);
  digitalWrite(OLED_VEXT, LOW);    // ВКЛЮЧАЕМ ПИТАНИЕ ДИСПЛЕЯ!
  delay(100);
  
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  
  // Инициализация Wire для дисплея (пины 17,18)
  Wire.begin(OLED_SDA, OLED_SCL);
  
  // Инициализация дисплея
  display.init();
  display.flipScreenVertically();  // КРИТИЧЕСКИ ВАЖНО!
  
  // Отображаем начальный экран
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Heltec V3.1");
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 20, "Инициализация...");
  display.display();
  
  Serial.println("\n\n=== Heltec WiFi Kit 32 V3.1 ===");
  Serial.println("Инициализация системы...");
  
  // --- ИНИЦИАЛИЗАЦИЯ I2C ДЛЯ ДАТЧИКОВ (пины 4,5) ---
  Wire1.begin(I2C_SDA, I2C_SCL);  // Используем второй I2C порт
  
  // --- ПОИСК И ИНИЦИАЛИЗАЦИЯ AHT10 (AC5600) ---
  Serial.print("Поиск AHT10 (адрес 0x38)... ");
  
  if (aht.begin(&Wire1, AHT10_ADDR)) {
    ahtFound = true;
    Serial.println("НАЙДЕН");
  } else {
    Serial.println("НЕ НАЙДЕН!");
    display.drawString(0, 35, "AHT10: НЕТ");
  }
  
  // --- ПОИСК И ИНИЦИАЛИЗАЦИЯ SHT31-D ---
  Serial.print("Поиск SHT31 (адрес 0x44)... ");
  
  if (sht31.begin(SHT31_ADDR, &Wire1)) {
    shtFound = true;
    Serial.println("НАЙДЕН");
  } else {
    // Пробуем альтернативный адрес
    if (sht31.begin(0x45, &Wire1)) {
      shtFound = true;
      Serial.println("НАЙДЕН по адресу 0x45");
    } else {
      Serial.println("НЕ НАЙДЕН!");
      display.drawString(0, 45, "SHT31: НЕТ");
    }
  }
  
  // --- ЗАВЕРШЕНИЕ НАСТРОЙКИ ---
  if (ahtFound || shtFound) {
    Serial.println("Датчики готовы к работе!");
    
    // Обновляем экран
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Датчики:");
    display.setFont(ArialMT_Plain_10);
    
    int yPos = 20;
    if (ahtFound) {
      display.drawString(0, yPos, "✓ AHT10: OK");
      yPos += 12;
    }
    if (shtFound) {
      display.drawString(0, yPos, "✓ SHT31: OK");
      yPos += 12;
    }
    
    display.drawString(0, yPos, "Чтение...");
    display.display();
  } else {
    Serial.println("ОШИБКА: Датчики не найдены!");
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "ОШИБКА!");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 20, "Датчики не найдены");
    display.drawString(0, 35, "Проверьте подключение");
    display.display();
    
    // Бесконечный цикл при ошибке
    while (true) {
      delay(1000);
    }
  }
  
  delay(2000);  // Пауза перед началом измерений
}

void loop() {
  // Обновляем данные каждые UPDATE_INTERVAL мс
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = millis();
    
    // Переменные для данных
    float tempAHT = NAN, humidityAHT = NAN;
    float tempSHT = NAN, humiditySHT = NAN;
    
    // --- ЧТЕНИЕ ДАННЫХ С AHT10 ---
    if (ahtFound) {
      sensors_event_t humidity, temp;
      if (aht.getEvent(&humidity, &temp)) {
        tempAHT = temp.temperature;
        humidityAHT = humidity.relative_humidity;
      }
    }
    
    // --- ЧТЕНИЕ ДАННЫХ С SHT31 ---
    if (shtFound) {
      tempSHT = sht31.readTemperature();
      humiditySHT = sht31.readHumidity();
    }
    
    // --- ВЫВОД В SERIAL MONITOR ---
    Serial.println("\n=== ПОКАЗАНИЯ ДАТЧИКОВ ===");
    
    if (ahtFound && !isnan(tempAHT)) {
      Serial.printf("AHT10:  Температура: %.1f°C, Влажность: %.1f%%\n", 
                    tempAHT, humidityAHT);
    } else {
      Serial.println("AHT10:  Нет данных");
    }
    
    if (shtFound && !isnan(tempSHT)) {
      Serial.printf("SHT31:  Температура: %.1f°C, Влажность: %.1f%%\n", 
                    tempSHT, humiditySHT);
    } else {
      Serial.println("SHT31:  Нет данных");
    }
    
    // --- ВЫВОД НА ДИСПЛЕЙ ---
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Датчики:");
    
    display.setFont(ArialMT_Plain_10);
    int yPos = 20;
    
    // Отображение данных AHT10
    if (ahtFound && !isnan(tempAHT)) {
      display.drawString(0, yPos, "AHT10:");
      display.drawString(40, yPos, String(tempAHT, 1) + "C");
      display.drawString(80, yPos, String(humidityAHT, 1) + "%");
      yPos += 12;
    }
    
    // Отображение данных SHT31
    if (shtFound && !isnan(tempSHT)) {
      display.drawString(0, yPos, "SHT31:");
      display.drawString(40, yPos, String(tempSHT, 1) + "C");
      display.drawString(80, yPos, String(humiditySHT, 1) + "%");
      yPos += 12;
    }
    
    // Средние значения, если оба датчика работают
    if (ahtFound && shtFound && !isnan(tempAHT) && !isnan(tempSHT)) {
      float avgTemp = (tempAHT + tempSHT) / 2;
      float avgHum = (humidityAHT + humiditySHT) / 2;
      
      display.drawString(0, yPos, "Среднее:");
      display.drawString(50, yPos, String(avgTemp, 1) + "C");
      display.drawString(90, yPos, String(avgHum, 1) + "%");
    }
    
    // Время обновления
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 55, "Обновлено: " + String(millis()/1000) + "с");
    
    display.display();
  }
  
  // Короткая пауза
  delay(100);
}

// --- ДОПОЛНИТЕЛЬНЫЕ ФУНКЦИИ ---
void scanI2C() {
  // Функция для сканирования I2C шины (можете вызвать из setup для диагностики)
  Serial.println("\nСканирование I2C шины...");
  
  byte error, address;
  int nDevices = 0;
  
  for(address = 1; address < 127; address++) {
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();
    
    if (error == 0) {
      Serial.printf("Устройство найден по адресу 0x%02X\n", address);
      nDevices++;
    }
  }
  
  if (nDevices == 0) {
    Serial.println("Устройства I2C не найдены!");
  }
}
