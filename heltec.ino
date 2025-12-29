// Простой код для Heltec V3.1
#include <Wire.h>
#include "SH1106Wire.h"

SH1106Wire display(0x3C, 15, 4);

void setup() {
  display.init();
  display.flipScreenVertically();
  display.clear();
  
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Heltec V3.1");
  display.drawString(0, 20, "Привет!");
  display.drawString(0, 40, "GitHub OK");
  
  display.display();
}

void loop() {
  delay(1000);
}
