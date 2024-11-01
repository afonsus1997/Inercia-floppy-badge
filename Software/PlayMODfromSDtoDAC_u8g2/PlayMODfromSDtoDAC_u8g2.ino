#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>  // Include the U8g2 library
#include "inercia_bitmap.h"
#include "AudioGeneratorMOD.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceSD.h"
#include <pico/multicore.h>

// u8g2 fonts
// https://raw.githubusercontent.com/olikraus/u8g2/master/doc/u8g2fntlistall.pdf


// OLED Display Configuration
#define MINSWING 1
#define MAXSWING 64

// Audio and Button Configuration
#define TOP_BUTTON 5
#define MID_BUTTON 6
#define BOT_BUTTON 4
#define POT_PIN A0

bool fileChanged = false;  // Flag to signal file change
int fileIndex = 0;
float volume = 0.1;
float previousVolume = 0.1;
bool isLoading = false;

// Variables for Animation
int rotation;
uint8_t swing;
uint8_t counter;
uint8_t once_every;
bool swing_direction;

// Display Setup
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);  // U8g2 display setup

// Audio Setup
AudioGeneratorMOD *mod;
AudioFileSourceSD *fileO;
AudioOutputI2S *out;
uint16_t fileCount = 0;

// Button states and debounce
struct Button {
  int pin;
  int state;
  int lastState = HIGH;
  unsigned long lastDebounceTime = 0;
  bool activated = false;
};

Button topButton = { TOP_BUTTON, HIGH };
Button midButton = { MID_BUTTON, HIGH };
Button botButton = { BOT_BUTTON, HIGH };
unsigned long debounceDelay = 50;

// Shared variables for now playing message
char nowPlaying[30];                // Buffer for the currently playing file name
bool displayingNowPlaying = false;  // Flag to control now playing visibility
unsigned long displayTime = 0;      // Time when the now playing message was displayed

// Shared variables for volume popup
bool displayingVolumePopup = false; // Single flag for volume popup
unsigned long volumePopupTime = 0;  // Timer for the popup duration

// Function Prototypes
void core1_displayTask();
void readButtons();
void checkButton(Button &button);
void handleButtonActions();
void resetButtonActivations();
void playFile(int index);
void drawRotatedBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height, int angle);
void core0_audioTask();
void drawVolumePopup(int volume);
void drawNowPlaying(const String& nowPlaying);

void setup() {
  Serial.begin(115200);
  delay(1000);
  // Initialize button pins
  pinMode(topButton.pin, INPUT);
  pinMode(midButton.pin, INPUT);
  pinMode(botButton.pin, INPUT);
  pinMode(POT_PIN, INPUT);

  // Start core1 for display handling
  multicore_launch_core1(core1_displayTask);

  // Start audio handling on the main core
  out = new AudioOutputI2S(22000, 20, 22);
  out->SetGain(volume);
  mod = new AudioGeneratorMOD();
  mod->SetBufferSize(1024);
  mod->SetSampleRate(22000);
  mod->SetStereoSeparation(64);

  // Initialize SD card and count MOD files
  if (!SD.begin(13, 1000000UL * 1000, SPI1)) {
    Serial.println("SD initialization failed!");
    while (1);
  }
  Serial.println("SD initialized successfully.");

  File root = SD.open("/");
  if (!root) {
    Serial.println("Failed to open /mods directory.");
    return;  // Exit if the directory cannot be opened
  }

  // List all MOD files found
  Serial.println("Available MOD files:");
  while (File entry = root.openNextFile()) {
    if (!entry.isDirectory() && strstr(entry.name(), ".mod")) {
      Serial.printf("Index %d: %s\n", fileCount, entry.name());
      fileCount++;
    }
    entry.close();
  }

  if (fileCount == 0) {
    Serial.println("No MOD files found.");
    return;  // Exit if no files were found
  }

  Serial.print("MODs found: ");
  Serial.println(fileCount);

  // Seed and play initial file
  randomSeed(analogRead(A1));
  fileIndex = random(0, fileCount);
  playFile(fileIndex);
  Serial.println("Playing file...");
  isLoading = true;
}

// Main loop function
void loop() {
  if (!isLoading) {
    readButtons();
  }

  // Adjust volume
  float newVolume = map(analogRead(POT_PIN), 4, 1023, 0, 100) / 100.0;

  // Check if volume change exceeds 2%
  if (abs(newVolume - previousVolume) > 0.03) {
      displayingVolumePopup = true; // Show the volume popup
      volumePopupTime = millis();   // Reset the timer for popup
      previousVolume = newVolume;    // Update the previous volume
  }
  
  // Update the audio output gain
  out->SetGain(newVolume);

  // Hide the volume popup after 1 second
  if (displayingVolumePopup && (millis() - volumePopupTime >= 1000)) {
      displayingVolumePopup = false; // Hide the popup after 1 second
  }

  if (mod->isRunning()) {
    if (!mod->loop()) {
      Serial.println("Audio loop finished.");
      mod->stop();
    }
    isLoading = false;
    handleButtonActions();
  } else if (!isLoading && !midButton.activated) {
    Serial.println("MOD done");
    fileIndex = (fileIndex + 1) % fileCount;  // Play the next file in the list
    playFile(fileIndex);
  }

  resetButtonActivations();  // Reset button states after processing
}

void core1_displayTask() {
    // Initialize display
    Wire.setSDA(0);
    Wire.setSCL(1);
    u8g2.begin(); // Initialize U8g2

    // Initialize animation variables
    counter = 0;
    once_every = 4;
    rotation = 0;
    swing = MINSWING;
    swing_direction = true;

    while (true) {
        // Animation logic
        u8g2.clearBuffer();
        drawRotatedBitmap(swing, swing / 3, inercia_bmp, INERCIA_W, INERCIA_H, rotation);

        // Safely read nowPlaying using critical section
        noInterrupts();
        u8g2.setFont(u8g2_font_5x8_tr); // Set a font that is 8px tall
        u8g2.setDrawColor(1); // Set color to white
        
        if (displayingNowPlaying) {
            drawNowPlaying(nowPlaying);
        }

        // Draw volume popup if necessary
        if (displayingVolumePopup) {
            drawVolumePopup((int)(previousVolume * 100)); // Convert previous volume to percentage
        }
        
        interrupts();

        // Send buffer to display
        u8g2.sendBuffer();
        
        rotation = (rotation + 1) % 360; // Increment the rotation angle
        if (++counter >= once_every) {
            counter = 0;
            swing += (swing_direction ? 1 : -1);
            if (swing >= MAXSWING || swing <= MINSWING) swing_direction = !swing_direction;
        }

        // Check if we need to hide the now playing message
        if (displayingNowPlaying && (millis() - displayTime >= 3000)) {
            displayingNowPlaying = false; // Hide the now playing message
        }

        // Add a short delay to prevent busy-waiting
        delay(10);
    }
}

void readButtons() {
  checkButton(topButton);
  checkButton(midButton);
  checkButton(botButton);
}

void checkButton(Button &button) {
  int reading = digitalRead(button.pin);

  if (reading != button.lastState) {
    button.lastDebounceTime = millis();
  }

  if ((millis() - button.lastDebounceTime) > debounceDelay && reading != button.state) {
    button.state = reading;
    if (button.state == LOW) {
      button.activated = true;
      Serial.printf("%s Button pressed!\n", button.pin == TOP_BUTTON ? "Top" : button.pin == MID_BUTTON ? "Mid" : "Bot");
    }
  }
  button.lastState = reading;
}

void handleButtonActions() {
  if (topButton.activated || botButton.activated) {
    mod->stop();
    if (topButton.activated) {
      // Change to the next file, looping back to the start
      fileIndex = (fileIndex + 1) % fileCount;
      Serial.printf("Next file: %d\n", fileIndex);
    } else if (botButton.activated) {
      // Change to a random file
      fileIndex = random(0, fileCount);
      Serial.printf("Random file: %d\n", fileIndex);
    }
    playFile(fileIndex);  // Play the selected file
    isLoading = true;     // Set loading flag
  }
  if (midButton.activated) {
    // Handle mid button action
    displayingNowPlaying = true; // Show now playing message
    displayTime = millis(); // Reset display time
    strcpy(nowPlaying, "Sample File Name"); // Update with the actual file name
  }
  
}

void resetButtonActivations() {
  topButton.activated = false;
  midButton.activated = false;
  botButton.activated = false;
}

void playFile(int index) {
  File root = SD.open("/mods");
  root.rewindDirectory();

  while (File entry = root.openNextFile()) {
    if (index <= 0 && strstr(entry.name(), ".mod")) {
      Serial.print("Playing: ");
      Serial.println(entry.name());

      // Safely update nowPlaying using critical section
      noInterrupts();
      strncpy(nowPlaying, entry.name(), sizeof(nowPlaying) - 1);
      nowPlaying[sizeof(nowPlaying) - 1] = '\0';  // Ensure null termination

      // Show now playing message
      displayingNowPlaying = true;  // Set flag to show the now playing message
      displayTime = millis();       // Store the current time for display duration

      interrupts();

      fileO = new AudioFileSourceSD(entry.name());
      mod->begin(fileO, out);
      entry.close();
      break;
    }
    entry.close();
    index--;
  }
}

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


void drawVolumePopup(int volume) {
    // Prepare text for display
    String volumeText = "Vol: " + String(volume) + "%";

    // Calculate text width and height
    int textWidth = u8g2.getStrWidth(volumeText.c_str());
    int textHeight = u8g2.getFontAscent() - u8g2.getFontDescent(); // Height of the text box

    // Calculate dimensions and position for the popup box
    int padding = 4; // Padding around the text
    int boxWidth = textWidth + 2 * padding; // Box width based on text width and padding
    int boxHeight = textHeight + 2 * padding; // Box height based on text height and padding
    int x = (128 - boxWidth) / 2; // Center the box horizontally
    int y = (32 - boxHeight) / 2; // Center the box vertically

    // Draw the outline (white)
    u8g2.setDrawColor(1); // Set color to white for outline
    u8g2.drawFrame(x, y, boxWidth, boxHeight); // Draw the outline frame

    // Fill the box (black)
    u8g2.setDrawColor(0); // Set color to black for filling
    u8g2.drawBox(x + 1, y + 1, boxWidth - 2, boxHeight - 2); // Draw filled box with padding for outline

    // Set text properties
    u8g2.setDrawColor(1); // Set color to white for text
    u8g2.setFont(u8g2_font_5x8_tr); // Set font for popup
    u8g2.setCursor(x + padding, y + padding + textHeight - u8g2.getFontDescent()); // Position the text inside the box, adjusted for descent
    u8g2.print(volumeText); // Display the volume text
}

int scrollOffset = 0; // Offset for scrolling text
int scrollSpeed = 1;  // Speed of scrolling

void drawNowPlaying(const String& nowPlaying) {
    // Prepare text for display
    String nowPlayingText = "Now Playing:";
    
    // Set the maximum box width based on the screen size
    const int maxBoxWidth = 128;

    // Define padding
    int padding = 4; // Padding around the text

    // Calculate text height
    int textHeight = u8g2.getFontAscent() - u8g2.getFontDescent(); // Height of the text

    // Calculate the width for the music glyph
    int musicGlyphWidth = 10; // Adjust this value based on your glyph size

    // Calculate the width for the title text
    int titleTextWidth = u8g2.getStrWidth(nowPlayingText.c_str());

    // Start with the full nowPlaying text
    String truncatedNowPlaying = nowPlaying;
    int fileNameWidth = u8g2.getStrWidth(truncatedNowPlaying.c_str());
    
    // Calculate total width required for title and glyph
    int totalWidth = titleTextWidth + musicGlyphWidth + 2 * padding; // Title + glyph + padding
    
    // Calculate box width and ensure it doesn't exceed the max width
    int boxWidth = max(totalWidth, fileNameWidth + 2 * padding); // Box width based on text widths
    if (boxWidth > maxBoxWidth) boxWidth = maxBoxWidth; // Ensure box width does not exceed screen width

    // Adjust the box width based on the title and glyph size
    int availableWidth = boxWidth - totalWidth;
    fileNameWidth = availableWidth;

    // Truncate the nowPlaying text if it exceeds the available width
    while (fileNameWidth > availableWidth && truncatedNowPlaying.length() > 0) {
        truncatedNowPlaying.remove(truncatedNowPlaying.length() - 1); // Remove last character
    }

    // Calculate the final box height and position
    int boxHeight = textHeight * 2 + 3 * padding; // Box height for two lines of text and padding
    int x = (maxBoxWidth - boxWidth) / 2; // Center the box horizontally
    int y = (32 - boxHeight) / 2; // Center the box vertically

    // Draw the outline (white)
    u8g2.setDrawColor(1); // Set color to white for outline
    u8g2.drawFrame(x, y, boxWidth, boxHeight); // Draw the outline frame

    // Fill the box (black)
    u8g2.setDrawColor(0); // Set color to black for filling
    u8g2.drawBox(x + 1, y + 1, boxWidth - 2, boxHeight - 2); // Draw filled box with padding for outline

    // Draw music glyph
    u8g2.setFont(u8g2_font_open_iconic_all_1x_t); // Switch to an icon font
    u8g2.setDrawColor(1); // Set color to white for the glyph
    u8g2.drawGlyph(x + padding, y + padding + textHeight - u8g2.getFontDescent(), 0xE1); // Music icon position

    // Set text properties for the title
    u8g2.setDrawColor(1); // Set color to white for text
    u8g2.setFont(u8g2_font_5x8_tr); // Set font for title
    u8g2.setCursor(x + padding + musicGlyphWidth, y + padding + textHeight - u8g2.getFontDescent()); // Position title text inside the box
    u8g2.print(nowPlayingText); // Display the title text

    // Set text properties for the truncated file name
    u8g2.setCursor(x + padding, y + padding + textHeight + padding + (textHeight - u8g2.getFontDescent())); // Position file name text inside the box
    u8g2.print(truncatedNowPlaying); // Display the truncated current file name
}
