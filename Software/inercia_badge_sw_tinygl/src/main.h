#ifndef __MAIN__H__
#define __MAIN__H__

#include <Arduino.h>
#include "io.h"
#include "oled.h"
#include "ui.h"
#include "player.h"
#include <pico/multicore.h>
#include "animations/rotating_logo/anim_rotating_logo.h"
#include "gfx.h"  

void core0_audioTask();
void core1_displayTask();

#endif  //!__MAIN__H__