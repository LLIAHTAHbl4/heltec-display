#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Пины для Heltec S3 V3.1
#define OLED_SDA   17
#define OLED_SCL   18
#define OLED_RST   16    // Пин сброса дисплея
#define VEXT_PIN   21    // Пин ВКЛЮЧЕНИЯ питания дисплея (LOW = включен)

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RST);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Heltec S3 V3.1 with OLED");
  
  // 1. ВКЛЮЧАЕМ питание дисплея (LOW = включено!)
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW);
  Serial.println("Display power ON");
  delay(100);
  
  // 2. Сброс дисплея
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  Serial.println("Display reset done");
  
  // 3. Инициализация I2C
  Wire.begin(OLED_SDA, OLED_SCL);
  Serial.print("I2C on SDA:");
  Serial.print(OLED_SDA);
  Serial.print(" SCL:");
  Serial.println(OLED_SCL);
  
  // 4. Инициализация дисплея
  Serial.println("Initializing display...");
  
  // Пробуем разные адреса и настройки
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false)) {
    Serial.println("0x3C failed, trying 0x3D...");
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D, false)) {
      Serial.println("SSD1306 allocation failed");
      Serial.println("Check VEXT_PIN and connections");
      return;
    }
  }
  
  Serial.println("Display initialized!");
  
  // 5. Настройка дисплея
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("HELLO!");
  display.println("Heltec");
  display.println("V3.1");
  display.display();
  
  Serial.println("Text displayed!");
}

void loop() {
  // Мигание для проверки
  display.invertDisplay(true);
  delay(300);
  display.invertDisplay(false);
  delay(1000);
  
  Serial.println("Still running...");
}
