#include "ui.h"

char ui_now_playing[30];  // Buffer for the currently playing file name
bool ui_displaying_now_playing_popup = false;  // Flag to control now playing visibility
unsigned long ui_now_playing_popup_time =0;  // Time when the now playing message was displayed

bool ui_displaying_volume_popup = false;  // Single flag for volume popup
unsigned long ui_volume_popup_time = 0;   // Timer for the popup duration


void UiEnableVolumePopup(void) {
  ui_displaying_volume_popup = true;  // Enable the volume popup
  ui_volume_popup_time = millis();    // Reset the timer
}

void UiDrawVolumePopup(int volume) {
  // Prepare text for display
  u8g2.setFont(u8g2_font_5x8_tr);  // Set font for popup
  String volumeText = "Vol: " + String(volume) + "%";

  // Calculate text width and height
  int textWidth = u8g2.getStrWidth(volumeText.c_str());
  int textHeight =
      u8g2.getFontAscent() - u8g2.getFontDescent();  // Height of the text box

  // Calculate dimensions and position for the popup box
  int padding = 4;  // Padding around the text
  int boxWidth =
      textWidth + 2 * padding;  // Box width based on text width and padding
  int boxHeight =
      textHeight + 2 * padding;  // Box height based on text height and padding
  int x = (128 - boxWidth) / 2;  // Center the box horizontally
  int y = (32 - boxHeight) / 2;  // Center the box vertically

  // Draw the outline (white)
  u8g2.setDrawColor(1);                       // Set color to white for outline
  u8g2.drawFrame(x, y, boxWidth, boxHeight);  // Draw the outline frame

  // Fill the box (black)
  u8g2.setDrawColor(0);  // Set color to black for filling
  u8g2.drawBox(x + 1, y + 1, boxWidth - 2,
               boxHeight - 2);  // Draw filled box with padding for outline

  // Set text properties
  u8g2.setDrawColor(1);  // Set color to white for text
  u8g2.setCursor(x + padding,
                 y + padding + textHeight -
                     u8g2.getFontDescent());  // Position the text inside the
                                              // box, adjusted for descent
  u8g2.print(volumeText);                     // Display the volume text
}

void UiDrawNowPlaying(const String& nowPlaying) {
  u8g2.setFont(u8g2_font_5x8_tr);  // Set font for popup
  // Prepare text for display
  String nowPlayingText = "Now Playing:";

  // Set the maximum box width based on the screen size
  const int maxBoxWidth = 128;

  // Define padding
  int padding = 4;  // Padding around the text

  // Calculate text height
  int textHeight =
      u8g2.getFontAscent() - u8g2.getFontDescent();  // Height of the text

  // Calculate the width for the music glyph
  int musicGlyphWidth = 10;  // Adjust this value based on your glyph size

  // Calculate the width for the title text
  int titleTextWidth = u8g2.getStrWidth(nowPlayingText.c_str());

  // Start with the full nowPlaying text
  String truncatedNowPlaying = nowPlaying;
  int fileNameWidth = u8g2.getStrWidth(truncatedNowPlaying.c_str());

  // Calculate total width required for title and glyph
  int totalWidth = titleTextWidth + musicGlyphWidth +
                   2 * padding;  // Title + glyph + padding

  // Calculate box width and ensure it doesn't exceed the max width
  int boxWidth =
      max(totalWidth,
          fileNameWidth + 2 * padding);  // Box width based on text widths
  if (boxWidth > maxBoxWidth)
    boxWidth = maxBoxWidth;  // Ensure box width does not exceed screen width

  // Adjust the box width based on the title and glyph size
  int availableWidth = boxWidth - totalWidth;
  fileNameWidth = availableWidth;

  // Truncate the nowPlaying text if it exceeds the available width
  while (fileNameWidth > availableWidth && truncatedNowPlaying.length() > 0) {
    truncatedNowPlaying.remove(truncatedNowPlaying.length() -
                               1);  // Remove last character
  }

  // Calculate the final box height and position
  int boxHeight = textHeight * 2 +
                  3 * padding;  // Box height for two lines of text and padding
  int x = (maxBoxWidth - boxWidth) / 2;  // Center the box horizontally
  int y = (32 - boxHeight) / 2;          // Center the box vertically

  // Draw the outline (white)
  u8g2.setDrawColor(1);                       // Set color to white for outline
  u8g2.drawFrame(x, y, boxWidth, boxHeight);  // Draw the outline frame

  // Fill the box (black)
  u8g2.setDrawColor(0);  // Set color to black for filling
  u8g2.drawBox(x + 1, y + 1, boxWidth - 2,
               boxHeight - 2);  // Draw filled box with padding for outline

  // Draw music glyph
  u8g2.setFont(u8g2_font_open_iconic_all_1x_t);  // Switch to an icon font
  u8g2.setDrawColor(1);  // Set color to white for the glyph
  u8g2.drawGlyph(x + padding, y + padding + textHeight - u8g2.getFontDescent(),
                 0xE1);  // Music icon position

  // Set text properties for the title
  u8g2.setDrawColor(1);            // Set color to white for text
  u8g2.setFont(u8g2_font_5x8_tr);  // Set font for title
  u8g2.setCursor(
      x + padding + musicGlyphWidth,
      y + padding + textHeight -
          u8g2.getFontDescent());  // Position title text inside the box
  u8g2.print(nowPlayingText);      // Display the title text

  // Set text properties for the truncated file name
  u8g2.setCursor(
      x + padding,
      y + padding + textHeight + padding +
          (textHeight -
           u8g2.getFontDescent()));  // Position file name text inside the box
  u8g2.print(truncatedNowPlaying);   // Display the truncated current file name
}

void UiHandleUiElements(void) {
  noInterrupts();
  u8g2.setFont(u8g2_font_5x8_tr);  // Set a font that is 8px tall
  u8g2.setDrawColor(1);            // Set color to white

  if (ui_displaying_now_playing_popup) {
    UiDrawNowPlaying("test.mod");
  }

  // Draw volume popup if necessary
  if (ui_displaying_volume_popup) {
    UiDrawVolumePopup((int)(IoGetCurrentVolume() * 100));  // Convert previous volume to percentage
  }

  

  interrupts();

  // Send buffer to display
  u8g2.sendBuffer();

  // Check if we need to hide the now playing message
  if (ui_displaying_now_playing_popup && (millis() - ui_now_playing_popup_time >= kNowPlayingPopupTimeout)) {
    ui_displaying_now_playing_popup = false;  // Hide the now playing message
  }

  if (ui_displaying_volume_popup && (millis() - ui_volume_popup_time >= kVolumePopupTimeout)) {
    ui_displaying_volume_popup = false;  // Hide the now playing message
  }

}