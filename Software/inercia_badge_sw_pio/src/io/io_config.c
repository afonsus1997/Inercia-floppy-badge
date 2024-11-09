#include "io_config.h"
#include <Arduino.h>

/* I2C0 Config */
 const uint8_t kIoPinI2c0Sda = 0;
 const uint8_t kIoPinI2c0Scl = 1;

/* I2C1 Config */
 const uint8_t kIoPinI2c1Sda = 18;
 const uint8_t kIoPinI2c1Scl = 19;

/* OLED Config */
 const uint8_t kIoPinOledSda = kIoPinI2c0Sda;
 const uint8_t kIoPinOledScl = kIoPinI2c0Scl;

/* Potentiometer*/
 const uint8_t kIoPotPin = A0;

/* Buttons */
 const uint8_t kIoButtonTopPin = 5;
 const uint8_t kIoButtonMidPin = 6;
 const uint8_t kIoButtonBotPin = 4;

/* RNG Pin*/
 const uint8_t kIoRngPin = A1;

/* I2S Pins */
const uint8_t kIoI2sSckPin = 20;
 const uint8_t kIoRI2sDataPin = 22;



 io_pin_config_t io_pin_config[kIoPinAmmountSize] = {
  /* EXP Header */
  {17, INPUT},
  {18, INPUT}, /* ! This is shared with SAO! */
  {19, INPUT}, /* ! This is shared with SAO! */
  {23, INPUT},
  {24, INPUT},
  {25, INPUT},
  {27, INPUT},
  {28, INPUT},
  {29, INPUT},
  /* EXP Header */
  {2, OUTPUT},
  {3, OUTPUT},
  /* Potentiometer */
  {kIoPotPin, INPUT},
  /* Buttons */
  {kIoButtonTopPin, INPUT},
  {kIoButtonMidPin, INPUT},
  {kIoButtonBotPin, INPUT}
};





 io_button_t io_buttons[kIoButtonAmmount] = {
  {kIoButtonTopPin, HIGH, HIGH, 0, false},
  {kIoButtonMidPin, HIGH, HIGH, 0, false},
  {kIoButtonBotPin, HIGH, HIGH, 0, false}
};




 const uint8_t io_analog_pins[kIoAnalogInAmmount] = { kIoPotPin };


 const unsigned long kIoDebounceDelay = 50;
 const unsigned long KIoVolumeDebounceDelay = 20;
 const float kIoPotDeadZone = 0.02;