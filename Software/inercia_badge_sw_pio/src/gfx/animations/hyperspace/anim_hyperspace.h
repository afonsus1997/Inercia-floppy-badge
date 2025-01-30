#ifndef __ANIM_HYPERSPACE__H__
#define __ANIM_HYPERSPACE__H__

#include <stdint.h>
#include <U8g2lib.h> // Include the U8g2 library
#include "oled.h"
#include "oled_config.h"

static void initializeStars(void);
uint32_t rnd(void);

void AnimHyperspace(void);

#endif //!__ANIM_HYPERSPACE__H__