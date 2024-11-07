#ifndef __UI__H__
#define __UI__H__

#include <U8g2lib.h>  // Include the U8g2 library
#include <String.h>
#include "oled.h"

extern U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2;

char ui_now_playing[30];  // Buffer for the currently playing file name
bool ui_displaying_now_playing = false;  // Flag to control now playing visibility
unsigned long ui_display_popup_time =0;  // Time when the now playing message was displayed

bool ui_displaying_volume_popup = false;  // Single flag for volume popup
unsigned long ui_volume_popup_time = 0;   // Timer for the popup duration

void UiDrawVolumePopup(int volume);
void UiDrawNowPlaying(const String& nowPlaying);
void UiEnableVolumePopup(void);
void UiHandleUiElements(void);



#endif  //!__UI__H__