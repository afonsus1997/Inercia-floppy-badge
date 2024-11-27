#ifndef __OLED__H__
#define __OLED__H__

#include "oled_config.h"

#include "io.h"
#include <U8g2lib.h>  // Include the U8g2 library

#ifdef kUseTinyGL
  #include "gfx_config.h"
  #include "zbuffer.h"
  #include "GL/gl.h"
#endif  //kUseTinyGL

#ifdef kUseTinyGL
  extern ZBuffer* frame_buffer;
#endif  //kUseTinyGL


extern U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2;

void OledDrawTest(void);

void OledInit(void);

#endif  //!__OLED__H__