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

// Function Prototypes
void core1_displayTask();
void readButtons();
void checkButton(Button &button);
void handleButtonActions();
void resetButtonActivations();
void playFile(int index);
void drawRotatedBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height, int angle);
void core0_audioTask();

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
  // Audio setup
  out = new AudioOutputI2S(22000, 20, 22);
  out->SetGain(volume);
  mod = new AudioGeneratorMOD();
  mod->SetBufferSize(1024);
  mod->SetSampleRate(22000);
  mod->SetStereoSeparation(64);

  // Initialize SD card and count MOD files
  if (!SD.begin(13, 1000000UL * 1000, SPI1)) {
    Serial.println("SD initialization failed!");
    while (1)
      ;
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
  volume = map(analogRead(POT_PIN), 4, 1023, 0, 100) / 100.0;
  out->SetGain(volume);

  if (mod->isRunning()) {
    if (!mod->loop()) {
      Serial.println("Audio loop finished.");
      mod->stop();
    }
    isLoading = false;
    handleButtonActions();
  } else if (!isLoading && !midButton.activated) {
    Serial.println("MOD done");
    // Automatically play the next file if not mid button activated
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
        if (!displayingNowPlaying) {
            drawRotatedBitmap(swing, swing / 3, inercia_bmp, INERCIA_W, INERCIA_H, rotation);
        }

        // Safely read nowPlaying using critical section
        noInterrupts();
        u8g2.setFont(u8g2_font_5x8_tr); // Set a font that is 8px tall
        u8g2.setDrawColor(1); // Set color to white
        
        if (displayingNowPlaying) {
            // Draw sound emotes using icons from the font
            u8g2.setFont(u8g2_font_open_iconic_all_1x_t); // Switch to an icon font
            u8g2.drawGlyph(0, 8, 0xE1); // Example icon glyph for speaker (adjust the code as needed)
            u8g2.setCursor(12, 10); // Position to draw the text
            u8g2.setFont(u8g2_font_5x8_tr); // Switch back to text font
            u8g2.print("Now Playing:"); // Title
            
            u8g2.setCursor(0, 20); // Move cursor to the next line
            u8g2.print(nowPlaying); // Display the current file name
            
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
      Serial.printf("%s Button pressed!\n", button.pin == TOP_BUTTON ? "Top" : button.pin == MID_BUTTON ? "Mid"
                                                                                                        : "Bot");
    }
  }
  button.lastState = reading;
}

void handleButtonActions() {
  // Stop current playback if a button is activated
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
