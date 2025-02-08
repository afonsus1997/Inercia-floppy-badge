#include "anim_hyperspace.h"

static const uint8_t starSpeed = 3;
static const uint8_t numStars = 69;
static const uint8_t midWidth = kOledWidth / 2;
static const uint8_t midHeight = kOledHeight / 2;

typedef struct
{
    int16_t x, y, z;
} Star;

Star stars[numStars];

/**
 * this is supposed to be a better random number generator on the RP2040
 */
uint32_t rnd(void)
{
    int k, random = 0;
    volatile uint32_t *rnd_reg = (uint32_t *)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);

    for (k = 0; k < 32; k++)
    {
        random = random << 1;
        random = random + (0x00000001 & (*rnd_reg));
    }
    return random;
}

void updateStars()
{
    for (int i = 0; i < numStars; i++)
    {
        // Update Z position
        stars[i].z -= starSpeed;
        if (stars[i].z <= 0)
        {
            /**
             * this will create a random new star
             */
/*
            stars[i].x = (float)(rnd() % kOledWidth - midWidth);
            stars[i].y = (float)(random() % kOledHeight - midHeight);
            stars[i].z = (float)(rnd() % kOledWidth * 2);
*/            
            stars[i].x = (rnd() % kOledWidth - midWidth);
            stars[i].y = (rnd() % kOledHeight - midHeight);
            stars[i].z = (rnd() % kOledWidth);
        }

        // move star
        float scale = 75.0f / stars[i].z;
        int x = (int)(stars[i].x * scale + midWidth);
        int y = (int)(stars[i].y * scale + midHeight);

        // if not off screen, draw star
        if (x >= 0 && x < kOledWidth && y >= 0 && y < kOledHeight)
        {
            u8g2.drawPixel(x, y);
        }
    }
}

void AnimHyperspace(void)
{
    updateStars();
}
