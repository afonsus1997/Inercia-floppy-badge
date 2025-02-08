#ifndef __GFX_CONFIG__H__
#define __GFX_CONFIG__H__

#include "animations/rotating_logo/anim_rotating_logo.h"
#include "animations/hyperspace/anim_hyperspace.h"

typedef void (*AnimationFunction)(); // Define a type for the function pointers

extern AnimationFunction animations[]; // Array of function pointers


#endif  //!__GFX_CONFIG__H__