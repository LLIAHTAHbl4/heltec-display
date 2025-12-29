#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Пины для Heltec S3 V3.1
#define OLED_SDA   17
#define OLED_SCL   18
#define OLED_RST   16
#define VEXT_PIN   21

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RST);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== OLED CONFIG TEST ===");
  
  // Включаем питание дисплея
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW);
  Serial.println("Power: ON (VEXT=LOW)");
  delay(200);
  
  // Сброс дисплея
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(100);
  digitalWrite(OLED_RST, HIGH);
  delay(100);
  Serial.println("Reset done");
  
  // Инициализация I2C
  Wire.begin(OLED_SDA, OLED_SCL);
  
  // === ТЕСТ 1: Обычная инициализация ===
  Serial.println("\n[TEST 1] Normal init...");
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false)) {
    Serial.println("Success! Trying to display...");
    testDisplay("TEST 1 OK");
    delay(3000);
  } else {
    Serial.println("Failed");
  }
  
  // === ТЕСТ 2: С внутренним генератором ===
  Serial.println("\n[TEST 2] Internal charge pump...");
  if (display.begin(SSD1306_EXTERNALVCC, 0x3C, false)) {
    Serial.println("Success! Trying to display...");
    testDisplay("TEST 2 OK");
    delay(3000);
  } else {
    Serial.println("Failed");
  }
  
  // === ТЕСТ 3: Адрес 0x3D ===
  Serial.println("\n[TEST 3] Address 0x3D...");
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3D, false)) {
    Serial.println("Success! Trying to display...");
    testDisplay("TEST 3 OK");
    delay(3000);
  } else {
    Serial.println("Failed");
  }
  
  // === ТЕСТ 4: С максимальной яркостью ===
  Serial.println("\n[TEST 4] Max brightness...");
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false)) {
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(0xFF); // Макс контраст
    Serial.println("Max contrast set");
    testDisplay("TEST 4 MAX");
    delay(3000);
  }
  
  // === ТЕСТ 5: Инверсия цветов ===
  Serial.println("\n[TEST 5] Inverted colors...");
  display.invertDisplay(true);
  testDisplay("INVERTED");
  delay(2000);
  display.invertDisplay(false);
  
  // === ТЕСТ 6: Разные размеры текста ===
  Serial.println("\n[TEST 6] Different text sizes...");
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Size 1");
  display.setTextSize(2);
  display.println("Size 2");
  display.setTextSize(3);
  display.println("Size 3");
  display.display();
  delay(3000);
  
  Serial.println("\n=== ALL TESTS COMPLETE ===");
  Serial.println("If display shows anything, note which test worked!");
}

void testDisplay(String text) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(text);
  display.println("HELLO!");
  display.display();
  Serial.println("Display updated: " + text);
}

void loop() {
  // Простая анимация
  static int counter = 0;
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Counter:");
  display.println(counter++);
  display.display();
  
  Serial.print("Counter: ");
  Serial.println(counter);
  delay(1000);
}
