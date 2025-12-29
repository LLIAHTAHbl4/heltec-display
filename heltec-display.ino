#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA   17
#define OLED_SCL   18
#define OLED_RST   16
#define VEXT_PIN   21

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RST);

void setup() {
  Serial.begin(115200);
  delay(3000); // Долгая задержка для стабилизации
  
  Serial.println("=== ULTIMATE OLED TEST ===");
  
  // Включение питания
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW);
  Serial.println("POWER: ON");
  delay(500); // Даем время на прогрев
  
  // Жесткий сброс
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(200);
  digitalWrite(OLED_RST, HIGH);
  delay(200);
  Serial.println("RESET: DONE");
  
  // I2C
  Wire.begin(OLED_SDA, OLED_SCL);
  
  // Инициализация с EXTERNALVCC (работал TEST 2)
  if (!display.begin(SSD1306_EXTERNALVCC, 0x3C, false)) {
    Serial.println("SSD1306 failed!");
    return;
  }
  
  Serial.println("DISPLAY: INIT OK");
  
  // === ЭКСТРЕМАЛЬНЫЕ НАСТРОЙКИ ===
  
  // 1. Максимальный контраст
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(255); // 255 - максимум
  Serial.println("CONTRAST: MAX (255)");
  delay(1000);
  
  // 2. Включаем дисплей
  display.ssd1306_command(SSD1306_DISPLAYON);
  Serial.println("DISPLAY: ON");
  delay(1000);
  
  // 3. ЗАЛИВКА ВСЕГО ЭКРАНА БЕЛЫМ
  Serial.println("FILL: WHITE SCREEN");
  display.clearDisplay();
  display.fillRect(0, 0, 128, 64, SSD1306_WHITE);
  display.display();
  delay(3000);
  
  // 4. ЧЕРНЫЙ экран
  Serial.println("FILL: BLACK SCREEN");
  display.clearDisplay();
  display.display();
  delay(2000);
  
  // 5. ШАХМАТНАЯ ДОСКА (максимально контрастно)
  Serial.println("PATTERN: CHESS BOARD");
  display.clearDisplay();
  for (int y = 0; y < 64; y += 16) {
    for (int x = 0; x < 128; x += 16) {
      if ((x + y) % 32 == 0) {
        display.fillRect(x, y, 16, 16, SSD1306_WHITE);
      }
    }
  }
  display.display();
  delay(3000);
  
  // 6. ГОРИЗОНТАЛЬНЫЕ полосы
  Serial.println("PATTERN: HORIZONTAL LINES");
  display.clearDisplay();
  for (int y = 0; y < 64; y += 4) {
    display.drawFastHLine(0, y, 128, SSD1306_WHITE);
  }
  display.display();
  delay(3000);
  
  // 7. ВЕРТИКАЛЬНЫЕ полосы
  Serial.println("PATTERN: VERTICAL LINES");
  display.clearDisplay();
  for (int x = 0; x < 128; x += 4) {
    display.drawFastVLine(x, 0, 64, SSD1306_WHITE);
  }
  display.display();
  delay(3000);
  
  Serial.println("=== TEST COMPLETE ===");
  Serial.println("Look at screen CAREFULLY!");
  Serial.println("Even faint glow means it works");
}

void loop() {
  // Мигание всей площадью
  display.clearDisplay();
  display.fillRect(0, 0, 128, 64, SSD1306_WHITE);
  display.display();
  Serial.println("SCREEN: WHITE");
  delay(1000);
  
  display.clearDisplay();
  display.display();
  Serial.println("SCREEN: BLACK");
  delay(1000);
  
  // Бегущая точка
  static int x = 0;
  display.clearDisplay();
  display.drawPixel(x, 32, SSD1306_WHITE);
  display.display();
  x = (x + 1) % 128;
  delay(50);
}
