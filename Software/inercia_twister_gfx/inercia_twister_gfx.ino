/**
   floppy badge animation for Inercia

   2024 - Luis Correia (luisfcorreia on github)

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "inercia_bitmap.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C
#define MINSWING 1
#define MAXSWING 64

int rotation;
uint8_t swing;
uint8_t counter;
uint8_t once_every;
bool swing_direction;
float scale;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(9600);

  /**
     We're using this on a Pico with alternative pinout
     setting them up here, before calling display.begin
  */
  Wire.setSDA(16);
  Wire.setSCL(17);

  // SSD1306_SWITCHCAPVCC init
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 startup failed"));
    for (;;);
  }
  /**
     set display brightness to dim, for dark environments...
  */
  display.dim(1);

  counter = 0;
  once_every = 4;
  rotation = 0;
  swing = MINSWING;
  swing_direction = true;
}

void loop() {

  display.clearDisplay();
  drawRotatedBitmap(swing, swing / 3, inercia_bmp, INERCIA_W, INERCIA_H, rotation);
  display.display();

  rotation++;
  if (rotation >= 359) rotation = 0;

  counter++;
  if (counter >= once_every) {
    counter = 0;
    // manage direction
    if (swing_direction) {
      swing++;
      if (swing >= MAXSWING) swing_direction = false;
    } else {
      swing--;
      if (swing <= MINSWING) swing_direction = true;
    }
  }
}

void drawRotatedBitmap(uint8_t x, uint8_t y, const uint8_t * bitmap, uint8_t width, uint8_t height, int angle) {
  int8_t old_x, old_y, new_x, new_y;                 // old and new (rotated) Pixel-Coordinates
  uint8_t pivot_x = width / 2;                       // Calculate the (rotation) center of the image (x fraction)
  uint8_t pivot_y = height / 2;                      // Calculate the (rotation) center of the image (y fraction)
  float angle_rad = angle / 57.3;
  float sin_angle = sin(angle_rad);                  // Lookup the sine
  float cos_angle = cos(angle_rad);                  // Lookup the cosine
  for (int row = 0; row < height; row++) {
    uint8_t displayData, mask = 0;
    for (int col = 0; col < width; col++) {
      if (mask == 0) {
        displayData = pgm_read_byte(bitmap++);       // Read the image data from PROGMEM
        mask = 0x80;                                 //MSB first
      }
      if (displayData & mask) {                      // i.e. a pixel
        old_x = col - pivot_x;                       // Calculate the x-position of the Pixel to be rotated
        old_y = row - pivot_y;                       // Calculate the y-position of the Pixel to be rotated
        new_x = (int) (old_x * cos_angle - old_y * sin_angle); // Calculate the x-position of the rotated Pixel
        new_y = (int) (old_y * cos_angle + old_x * sin_angle); // Calculate the y-position of the rotated Pixel
        display.drawPixel(x + pivot_x + new_x, y + pivot_y + new_y, WHITE);
      }
      mask >>= 1;                       //ready for next bit
    }
  }
}
