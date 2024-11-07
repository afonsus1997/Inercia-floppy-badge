#include "main.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  IoInit();
  OledInit();
  PlayerInit();
  u8g2.sendBuffer();            // Send the buffer to the display
    // Reset display time
}

void loop() {
  IoReadButtons();
  IoReadVolume();
  // u8g2.clearBuffer();
  UihandleUiElements();
  // delay(20);
}