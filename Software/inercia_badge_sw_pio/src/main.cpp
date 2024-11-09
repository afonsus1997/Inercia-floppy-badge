#include "main.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting up...");

  IoInit();
  OledInit();
  PlayerInit();
  UiEnableVolumePopup();

  multicore_launch_core1(core1_displayTask);
  // u8g2.sendBuffer();            // Send the buffer to the display
    // Reset display time
}

void loop() {
  IoReadButtons();
  IoReadAnalogInputs();
  IoReadVolume();
  PlayerHandlePlayback();
  
}

void core1_displayTask() {
  u8g2.clearBuffer();
  UiHandleUiElements();
}