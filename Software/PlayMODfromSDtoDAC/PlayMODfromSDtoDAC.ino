// Airrr's i2s random MOD player with "skipnext" button and OLED
//main library: https://github.com/earlephilhower/ESP8266Audio
//DAC (PCM5102):
//3.3V -> VCC, 33V, XMT, (SCK)
//GND  -> GND, FLT, DMP, FMT, SCL
//BCLK->BCK      to Pin D26  ESP32
//I2SO->DIN      to Pin D22
//LRCLK(WS)->LCK to Pin D25
//OLED SSD1306:
//SDA -> D21, SCL -> D4
//SD card:
//CS -> D5, MOSI -> D23, CLK -> D18, MISO -> D19
//Next track -> VP pin to VCC (pull down req.)

#include <Arduino.h>
#include "AudioGeneratorMOD.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceSD.h"
// #include "AudioOutputI2SNoDAC.h"
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <math.h>



    // .miso_gpio = 12,  // GPIO number (not Pico pin number)
    // .mosi_gpio = 15,
    // .sck_gpio = 14,
    // .ss_gpio = 13,

AudioGeneratorMOD *mod;
AudioFileSourceSD *fileO;
AudioOutputI2S *out;
// AudioOutputI2SNoDAC *out2;
uint16_t FileCount; //in root
unsigned long seed;

#define TOP_BUTTON 5
#define MID_BUTTON 6
#define BOT_BUTTON 4

int buttonStateTop;  // HIGH means not pressed, LOW means pressed
int buttonStateMid;  // HIGH means not pressed, LOW means pressed
int buttonStateBot;  // HIGH means not pressed, LOW means pressed

int lastButtonStateTop = HIGH;  // HIGH means not pressed, LOW means pressed
int lastButtonStateMid = HIGH;  // HIGH means not pressed, LOW means pressed
int lastButtonStateBot = HIGH;  // HIGH means not pressed, LOW means pressed

unsigned long lastDebounceTimeTop = 0;  // the last time the output pin was toggled
unsigned long lastDebounceTimeMid = 0;  // the last time the output pin was toggled
unsigned long lastDebounceTimeBot = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers

int activatedTop = 0;  // HIGH means not pressed, LOW means pressed
int activatedMid = 0;  // HIGH means not pressed, LOW means pressed
int activatedBot = 0;  // HIGH means not pressed, LOW means pressed

int file2open;
float currVol;

void setup() {  ////////////////////////////setup start
  currVol = 0.1;
  Serial.begin(115200);

  pinMode(TOP_BUTTON, INPUT);
  pinMode(MID_BUTTON, INPUT);
  pinMode(BOT_BUTTON, INPUT);

  // fileO = new AudioFileSourceSD(entry.name());  
  out = new AudioOutputI2S(22000, 20, 22);
  out->SetGain(0.05);
  mod = new AudioGeneratorMOD();
  mod->SetBufferSize(1 * 1024);  ///3
  mod->SetSampleRate(22000);
  mod->SetStereoSeparation(64);  // upto64 64-no separation! 0-max and ugly!

  Serial.printf("MOD start\n");
  delay(500);
  if (!SD.begin(13,1000000UL * (10), SPI1)) {
    Serial.println("SD initialization failed!");
    while (1);
  }
  File root = SD.open("/mods");
  // printDirectory(root);
  while (true) {
    File entry =  root.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    Serial.print(entry.name());

    if (entry.isDirectory()) {
    } else {

      if (!strcasecmp(entry.name() + strlen(entry.name()) - 4, ".mod")) {
        FileCount++;
      }
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
  Serial.print("MODs found:");
  Serial.println(FileCount);

  PlayFile(1);
}            //////////////////////////////setup end


void readButton(){
  int readingTop = digitalRead(TOP_BUTTON);
  int readingMid = digitalRead(MID_BUTTON);
  int readingBot = digitalRead(BOT_BUTTON);

  // Check if the button state has changed
  if (readingTop != lastButtonStateTop) {
    // Reset the debouncing timer
    lastDebounceTimeTop = millis();
  }
  if (readingMid != lastButtonStateMid) {
    // Reset the debouncing timer
    lastDebounceTimeMid = millis();
  }
  if (readingBot != lastButtonStateBot) {
    // Reset the debouncing timer
    lastDebounceTimeBot = millis();
  }

  // Check if enough time has passed to consider it a valid button press
  if ((millis() - lastDebounceTimeTop) > debounceDelay) {
    // Update the button state only if it has changed
    // Serial.printf("readingTop:%d buttonStateTop:%d\n", readingTop, buttonStateTop);
    if (readingTop != buttonStateTop) {
      buttonStateTop = readingTop;

      // If the button is pressed, do something (replace this with your desired action)
      if (buttonStateTop == LOW) {
        Serial.println("Top Button pressed!");
        activatedTop = 1;
      }
    }
  }

  // Check if enough time has passed to consider it a valid button press
  if ((millis() - lastDebounceTimeMid) > debounceDelay) {
    // Update the button state only if it has changed
    // Serial.printf("readingTop:%d buttonStateTop:%d\n", readingTop, buttonStateTop);
    if (readingMid != buttonStateMid) {
      buttonStateMid = readingMid;
      // If the button is pressed, do something (replace this with your desired action)
      if (buttonStateMid == LOW) {
        Serial.println("Mid Button pressed!");
        activatedMid = 1;
      }
    }
  }

  // Check if enough time has passed to consider it a valid button press
  if ((millis() - lastDebounceTimeBot) > debounceDelay) {
    // Update the button state only if it has changed
    // Serial.printf("readingTop:%d buttonStateTop:%d\n", readingTop, buttonStateTop);
    if (readingBot != buttonStateBot) {
      buttonStateBot = readingBot;
      // If the button is pressed, do something (replace this with your desired action)
      if (buttonStateBot == LOW) {
        Serial.println("Bot Button pressed!");
        activatedBot = 1;
      }
    }
  }

  lastButtonStateTop = readingTop;
  lastButtonStateMid = readingMid;
  lastButtonStateBot = readingBot;
}

double linearToLogScale(double linearValue) {
    // Ensure that the input value is in the range [0, 1]
    linearValue = fmax(0.0, fmin(1.0, linearValue));

    // Convert linear value to log scale (in dB)
    double logScaleValue = 20.0 * log10(linearValue);

    return logScaleValue;
}

void loop()  {           /////////////loop start
  readButton();
  if (mod->isRunning()) {
    if (!mod->loop()) mod->stop();
    
    if(activatedTop || activatedMid){
      mod->stop();
    }
    else if(activatedBot){
      
      currVol = fmod(currVol + 0.1, 1.1);
      if(currVol == 0 ) currVol = 0.1;
      // out->SetGain(linearToLogScale(currVol));
      Serial.println(currVol);
      out->SetGain(currVol);
    }
  } else {
    if(activatedTop || activatedMid){
      if(activatedTop == LOW){
        file2open = random(0, FileCount);
        PlayFile(file2open);
      }
      else if(activatedMid){
        file2open = (file2open + 1) % FileCount;
      PlayFile(file2open);
      }
    }
    else {
      Serial.printf("MOD done\n");
      file2open = (file2open + 1) % FileCount;
      PlayFile(file2open);
    }
    
  }

  if(activatedTop) activatedTop = 0;
  if(activatedMid) activatedMid = 0;
  if(activatedBot) activatedBot = 0;


}                  //////////////////loop end

void PlayFile(int file2open) { /////////////////PLAY
  // file2open = random(0, FileCount); //-1
  File root = SD.open("/");
  root.rewindDirectory();

  while (true) {
    File entry =  root.openNextFile();
    if (! entry) {
      Serial.println("no more files");// no more files
      break;
    }
    if (!strcasecmp(entry.name() + strlen(entry.name()) - 4, ".mod")) {
      if (file2open <= 0) {
        Serial.printf("Vibrali:");
        Serial.println(entry.name());

        fileO = new AudioFileSourceSD(entry.name());  
        out->SetGain(0.8);
        mod->SetBufferSize(1 * 1024);  ///3
        mod->SetSampleRate(22000);
        mod->SetStereoSeparation(64);  // upto64 64-no separation! 0-max and ugly!
        mod->begin(fileO, out);  //out

        break;
      }
      file2open--;
    }
  }
}               ///////////////////////PLAY

