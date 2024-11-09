
#include "anim_rotating_logo.h"


static const uint8_t kMinSwing = 1;
static const uint8_t kMaxSwing = 64;

static int rotation = 0;
static uint8_t swing = 4;
static uint8_t counter = 0;
static uint8_t once_every = kMinSwing;
static bool swing_direction = true;


void drawRotatedBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height, int angle) {
  int8_t old_x, old_y, new_x, new_y;
  uint8_t pivot_x = width / 2;
  uint8_t pivot_y = height / 2;
  float angle_rad = angle / 57.3;  // Convert to radians
  float sin_angle = sin(angle_rad);
  float cos_angle = cos(angle_rad);

  for (int row = 0; row < height; row++) {
    uint8_t displayData, mask = 0;
    for (int col = 0; col < width; col++) {
      if (mask == 0) {
        displayData = pgm_read_byte(bitmap++);
        mask = 0x80;
      }
      if (displayData & mask) {
        old_x = col - pivot_x;
        old_y = row - pivot_y;
        new_x = (int)(old_x * cos_angle - old_y * sin_angle) + pivot_x + x;
        new_y = (int)(old_x * sin_angle + old_y * cos_angle) + pivot_y + y;
        u8g2.drawPixel(new_x, new_y);  // Use U8g2 to draw the pixel
      }
      mask >>= 1;
    }
  }
}

void AnimRotatingLogo(void) {
  drawRotatedBitmap(swing, swing / 3, inercia_bmp, kInerciaBmpWidth, kInerciaBmpHeight, rotation);


  rotation = (rotation + 1) % 360; // Increment the rotation angle
  if (++counter >= once_every) {
    counter = 0;
    swing += (swing_direction ? 1 : -1);
    if (swing >= kMaxSwing || swing <= kMinSwing) swing_direction = !swing_direction;
  }



}