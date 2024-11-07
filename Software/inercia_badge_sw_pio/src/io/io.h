
#ifndef __IO__H__
#define __IO__H__

#include <Wire.h>
#include "io_config.h"

typedef enum {
  kIoErr = 0,
  kIoErrNone = 1
} IoStatus_t;

static uint16_t io_analog_readings[kIoAnalogInAmmount];

float io_current_volume = 0;
float io_last_volume = 0;

unsigned long io_last_volume_debounce_time = 0;


IoStatus_t IoInit(void);

IoStatus_t IoSetPinModes(void);

IoStatus_t IoInitI2C(void);

IoStatus_t IoCheckButton(io_button_t &button);

IoStatus_t IoReadButtons(void);

IoStatus_t IoReadVolume(void)



#endif  //!__IO__H__
