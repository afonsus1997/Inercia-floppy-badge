#ifndef __UI__H__
#define __UI__H__

#include <U8g2lib.h>  // Include the U8g2 library
#include <String.h>
#include "oled.h"

extern U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2;



void drawVolumePopup(int volume);
void drawNowPlaying(const String& nowPlaying);

void handleUiElements(void);



#endif  //!__UI__H__