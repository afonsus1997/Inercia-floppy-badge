
#include "io.h"



IoStatus_t IoSetPinModes(void){
  IoStatus_t status = kIoErrNone;

  for (int i = 0; i < kIoPinAmmountSize; i++) {
    pinMode(io_pin_config[i].pin_number, io_pin_config[i].pin_mode);
  }
  
  return kIoErrNone;
}

IoStatus_t IoInitI2C(void){
  IoStatus_t status = kIoErrNone;
  
  Wire.setSDA(kIoPinI2c0Sda);
  Wire.setSCL(kIoPinI2c0Scl);
  Wire.begin();

  Wire1.setSDA(kIoPinI2c1Sda);
  Wire1.setSCL(kIoPinI2c1Scl);
  Wire1.begin();

  return status;
}

IoStatus_t IoInit(void) {
  IoStatus_t status = kIoErrNone;
  status = IoSetPinModes();
  if (status != kIoErrNone) {
    return status;
  }
  status = IoInitI2C();
  if (status != kIoErrNone) {
    return status;
  }
  return status;
  
}

IoStatus_t IoCheckButton(io_button_t &button) {
  int reading = digitalRead(button.pin);

  if (reading != button.lastState) {
    button.lastDebounceTime = millis();
  }

  if ((millis() - button.lastDebounceTime) > debounceDelay && reading != button.state) {
    button.state = reading;
    if (button.state == LOW) {
      button.activated = true;
      Serial.printf("%s Button pressed!\n", button.pin == kIoButtonTopPin ? "Top" : button.pin == kIoButtonMidPin ? "Mid" : "Bot");
    }
  }
  button.lastState = reading;
}

IoStatus_t IoReadButtons(void){
  for (int i = 0; i < kIoButtonAmmount; i++) {
    IoCheckButton(io_buttons[i]);
  }
}


