
#include "io.h"

// extern void UiEnableVolumePopup(void);

uint16_t io_analog_readings[kIoAnalogInAmmount];
float io_current_volume = 0;
unsigned long io_last_volume_debounce_time = 0;

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

  /* Start by reading an analog input for RNG*/
  pinMode(kIoRngPin, INPUT);
  randomSeed(analogRead(kIoRngPin));

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

  if ((millis() - button.lastDebounceTime) > kIoDebounceDelay && reading != button.state) {
    button.state = reading;
    if (button.state == LOW) {
      button.activated = true;
      Serial.printf("%s Button pressed!\n", button.pin == kIoButtonTopPin ? "Top" : button.pin == kIoButtonMidPin ? "Mid" : "Bot");
    }
  }
  button.lastState = reading;
  return kIoErrNone;
}

IoStatus_t IoReadButtons(void){
  for (int i = 0; i < kIoButtonAmmount; i++) {
    IoCheckButton(io_buttons[i]);
  }
  return kIoErrNone;
}

IoStatus_t IoReadAnalogInputs(void){
  for (int i = 0; i < kIoAnalogInAmmount; i++) {
    io_analog_readings[i] = (uint16_t)analogRead(io_analog_pins[i]);
  }
  return kIoErrNone;
}

IoStatus_t IoReadVolume(void){
  float reading = map(io_analog_readings[kIoPot], 4, 1023, 0, 100) / 100.0;

  if ((millis() - io_last_volume_debounce_time) > KIoVolumeDebounceDelay && abs(reading - io_current_volume) > 2) {
    io_current_volume = reading;
    io_last_volume_debounce_time = millis();
    // UiEnableVolumePopup();
    // Add hook to set mod player output gain
    Serial.printf("Volume %f\n", io_current_volume);
  }

  return kIoErrNone;
}


float IoGetCurrentVolume(void){
  return io_current_volume;
}
