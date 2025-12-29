#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Для I2C дисплея (пробуем еще раз)
#define I2C_SDA 15
#define I2C_SCL 4
#define I2C_RST 16

// Для SPI дисплея (если вдруг SPI)
#define SPI_MOSI 13
#define SPI_CLK  14
#define SPI_DC   27
#define SPI_CS   12
#define SPI_RST  33

// Пробуем I2C сначала
Adafruit_SH1106 display(128, 64, &Wire, I2C_RST);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Testing display...");
  
  // Пробуем I2C
  Serial.println("Trying I2C...");
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Сброс дисплея
  pinMode(I2C_RST, OUTPUT);
  digitalWrite(I2C_RST, LOW);
  delay(50);
  digitalWrite(I2C_RST, HIGH);
  delay(50);
  
  // Инициализация I2C дисплея
  if (display.begin(0x3C, true)) {
    Serial.println("I2C display OK!");
    showText("I2C WORKS!");
  } else {
    Serial.println("I2C failed, trying 0x3D...");
    if (display.begin(0x3D, true)) {
      Serial.println("I2C 0x3D OK!");
      showText("I2C 0x3D");
    } else {
      Serial.println("I2C completely failed");
    }
  }
}

void showText(String msg) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println(msg);
  display.println("HELLO!");
  display.display();
}

void loop() {
  delay(1000);
  Serial.println("Running...");
}
