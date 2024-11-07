
#ifndef __IO__H__
#define __IO__H__

#include <Wire.h>
#include "io_config.h"

typedef enum {
  kIoErr = 0,
  kIoErrNone = 1
} IoStatus_t;



#ifdef __cplusplus
extern "C" {
#endif

  IoStatus_t IoInit(void);

  IoStatus_t IoSetPinModes(void);

  IoStatus_t IoInitI2C(void);

#ifdef __cplusplus
}
#endif

#endif  //!__IO__H__
