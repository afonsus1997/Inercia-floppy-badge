#include "main.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting up...");

  IoInit();
  OledInit();
  PlayerInit();
  // PlayerSetVolume(50);
  multicore_launch_core1(core1_displayTask);
  UiEnableVolumePopup();
  // u8g2.sendBuffer();            // Send the buffer to the display
    // Reset display time
}

void loop() {
  // IoReadButtons();
  IoReadAnalogInputs();
  IoReadVolume();
  PlayerHandlePlayback();
  
}

void core1_displayTask() {
  while(1){
    u8g2.clearBuffer();
    UiHandleUiElements();
  }
}