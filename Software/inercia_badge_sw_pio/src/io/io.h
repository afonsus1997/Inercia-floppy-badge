
#ifndef __IO__H__
#define __IO__H__

#include <Wire.h>
#include "io_config.h"
#include "ui.h"

typedef enum {
  kIoErr = 0,
  kIoErrNone = 1
} IoStatus_t;

extern uint16_t io_analog_readings[kIoAnalogInAmmount];
extern float io_current_volume;
extern unsigned long io_last_volume_debounce_time;

IoStatus_t IoInit(void);

IoStatus_t IoSetPinModes(void);

IoStatus_t IoInitI2C(void);

IoStatus_t IoCheckButton(io_button_t &button);

IoStatus_t IoReadButtons(void);

IoStatus_t IoReadVolume(void);

float IoGetCurrentVolume(void);


#endif  //!__IO__H__
