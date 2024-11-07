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
static uint8_t kIoPinI2c0Sda = 0;
static uint8_t kIoPinI2c0Scl = 1;

/* I2C1 Config */
static uint8_t kIoPinI2c1Sda = 18;
static uint8_t kIoPinI2c1Scl = 19;

/* OLED Config */
static uint8_t kIoPinOledSda = kIoPinI2c0Sda;
static uint8_t kIoPinOledScl = kIoPinI2c0Scl;

/* Potentiometer*/
static uint8_t kIoPinPot = A0;

/* Buttons */
static const uint8_t kIoButtonTopPin = 5;
static const uint8_t kIoButtonMidPin = 6;
static const uint8_t kIoButtonBotPin = 4;


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
  {kIoPinPot, INPUT},
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

static const unsigned long debounceDelay = 50;



#endif  //!__IO_CONFIG__H__