#ifndef __IO_CONFIG__H__
#define __IO_CONFIG__H__

typedef struct io_pin_config {
  uint8_t pin_number;
  uint8_t pin_mode;
} io_pin_config_t;

static const uint8_t kExpConnIoSize = 11;

static io_pin_config_t io_pin_config[kExpConnIoSize] = {
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
  {3, OUTPUT}
};

/* I2C0 Config */
static uint8_t kIoPinI2c0Sda = 0;
static uint8_t kIoPinI2c0Scl = 1;

/* I2C1 Config */
static uint8_t kIoPinI2c1Sda = 18;
static uint8_t kIoPinI2c1Scl = 19;

/* OLED Config */
static uint8_t kIoPinOledSda = kIoPinI2c0Sda;
static uint8_t kIoPinOledScl = kIoPinI2c0Scl;


#endif  //!__IO_CONFIG__H__