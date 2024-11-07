#include "main.h"

extern bool displayingNowPlaying;  // Flag to control now playing visibility
extern unsigned long
    displayPopupTime;  // Time when the now playing message was displayed

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  IoInit();
  OledInit();
  u8g2.sendBuffer();            // Send the buffer to the display
  displayingNowPlaying = true;  // Show now playing message
  displayPopupTime = millis();  // Reset display time
}

void loop() {
  IoReadButtons();
  // put your main code here, to run repeatedly:
  // u8g2.clearBuffer();
  // handleUiElements();
  // delay(20);
}