#include "oled.h"

#ifdef kUseTinyGL
ZBuffer* frame_buffer;
#endif  // kUseTinyGL

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);  // U8g2 display setup
// U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE,
// kIoPinOledScl, kIoPinOledSda);  // Somehow this doesent work

void OledInit(void) {
  u8g2.begin();          // Initialize U8g2
  u8g2.clearBuffer();    // Clear the display buffer
  u8g2.setDrawColor(1);  // Set color to white
  OledDrawTest();
}

void OledDrawTest(void) {
  u8g2.clearBuffer();  // Clear the display buffer
  u8g2.sendBuffer();   // Send the buffer to the display
}