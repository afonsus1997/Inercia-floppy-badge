#ifndef __OLED__H__
#define __OLED__H__

#include "io.h"
#include <U8g2lib.h>  // Include the U8g2 library
#include "oled_config.h"

#define U8G2_USE_DYNAMIC_ALLOC
#define __ARM_LINUX__

extern U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2;

void OledDrawTest(void);

void OledInit(void);

#endif  //!__OLED__H__