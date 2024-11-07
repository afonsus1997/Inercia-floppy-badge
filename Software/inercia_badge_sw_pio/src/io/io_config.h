#ifndef __IO_CONFIG__H__
#define __IO_CONFIG__H__

#include "io.h"

typedef struct io_pin_config {
  uint8_t pin_number;
  uint8_t pin_mode;
} io_pin_config_t;

typedef struct  {
  uint8_t pin;
  uint8_t state;
  uint8_t lastState = HIGH;
  unsigned long lastDebounceTime = 0;
  bool activated = false;
} io_button_t;


/* I2C0 Config */
static const uint8_t kIoPinI2c0Sda = 0;
static const uint8_t kIoPinI2c0Scl = 1;

/* I2C1 Config */
static const uint8_t kIoPinI2c1Sda = 18;
static const uint8_t kIoPinI2c1Scl = 19;

/* OLED Config */
static const uint8_t kIoPinOledSda = kIoPinI2c0Sda;
static const uint8_t kIoPinOledScl = kIoPinI2c0Scl;

/* Potentiometer*/
static const uint8_t kIoPotPin = A0;

/* Buttons */
static const uint8_t kIoButtonTopPin = 5;
static const uint8_t kIoButtonMidPin = 6;
static const uint8_t kIoButtonBotPin = 4;

/* Analog Pins*/
static const uint8_t kIoPotPin = A0;

static const uint8_t kIoRngPin = A1;

/* I2S Pins */
static const uint8_t kIoI2sSckPin = 20;
static const uint8_t kIoRI2sDataPin = 22;


static const uint8_t kIoPinAmmountSize = 15;

static io_pin_config_t io_pin_config[kIoPinAmmountSize] = {
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

static const uint8_t kIoButtonAmmount = 3;

typedef enum {
  kIoButtonTop = 0,
  kIoButtonMid = 1,
  kIoButtonBot = 2
} io_button_e;

static io_button_t io_buttons[kIoButtonAmmount] = {
  {kIoButtonTopPin, HIGH, HIGH, 0, false},
  {kIoButtonMidPin, HIGH, HIGH, 0, false},
  {kIoButtonBotPin, HIGH, HIGH, 0, false}
};

static const uint8_t kIoAnalogInAmmount = 1;

typedef enum {
  kIoPot = 0,
} io_analog_in_e;

static const uint8_t io_analog_pins[kIoAnalogInAmmount] = { kIoPotPin };


static const unsigned long kIoDebounceDelay = 50;
static const unsigned long KIoVolumeDebounceDelay = 50;



#endif  //!__IO_CONFIG__H__