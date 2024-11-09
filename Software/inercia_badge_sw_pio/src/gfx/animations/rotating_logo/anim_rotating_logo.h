#ifndef __ANIM_ROTATING_LOGO__H__
#define __ANIM_ROTATING_LOGO__H__

#include <stdint.h> 
#include <U8g2lib.h>  // Include the U8g2 library
#include "oled.h"
#include "bitmaps/inercia_bitmap.h"


static void drawRotatedBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height, int angle);

void AnimRotatingLogo(void);

#endif  //!__ANIM_ROTATING_LOGO__H__