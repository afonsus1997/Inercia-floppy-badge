#include "main.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting up...");

  IoInit();
  OledInit();
  PlayerInit();
  multicore_launch_core1(core1_displayTask);
}

void loop() { // core0_audioTask
  IoReadButtons();
  IoReadAnalogInputs();
  IoReadVolume();
  PlayerHandlePlayback();
  IoResetButtonActivations();
  
}

void core1_displayTask() {
  while(1){
    u8g2.clearBuffer();
    GfxHandler();
    UiHandleUiElements();
    u8g2.sendBuffer();
    delay(10);  
  }
}