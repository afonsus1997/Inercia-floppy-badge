#ifndef __UI__H__
#define __UI__H__

#include <U8g2lib.h>  // Include the U8g2 library
#include <String.h>
#include "oled.h"

extern char ui_now_playing[30];  // Buffer for the currently playing file name
extern bool ui_displaying_now_playing;  // Flag to control now playing visibility
extern unsigned long ui_display_popup_time;  // Time when the now playing message was displayed
extern bool ui_displaying_volume_popup;  // Single flag for volume popup
extern unsigned long ui_volume_popup_time;   // Timer for the popup duration


void UiDrawVolumePopup(int volume);
void UiDrawNowPlaying(const String& nowPlaying);
void UiEnableVolumePopup(void);
void UiHandleUiElements(void);



#endif  //!__UI__H__