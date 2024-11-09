
#include "player.h"

AudioGeneratorMOD *player_mod;
AudioFileSourceSD *player_fileO;
AudioOutputI2S *player_out;
uint16_t player_file_count = 0;
File player_root;
File player_entry;
char player_now_playing[40];  
uint16_t player_file_index = 0;

void PlayerSetGain(float gain) {
  player_out->SetGain(gain);
}

void PlayerInit(void) {
  player_out = new AudioOutputI2S(kPlayerSampleRate, kIoI2sSckPin, kIoRI2sDataPin);
  player_mod = new AudioGeneratorMOD();
  PlayerSetGain(IoGetCurrentVolume());
  player_mod->SetBufferSize(kPlayerBufferSize);
  player_mod->SetSampleRate(kPlayerSampleRate);
  player_mod->SetStereoSeparation(kPlayerStereoSeparation);

  PlayerInitSdCard();

  Serial.println("Available MOD files:");
  while (player_entry = player_root.openNextFile()) {
    if (!player_entry.isDirectory() && strstr(player_entry.name(), ".mod")) {
      Serial.printf("Index %d: %s\n", player_file_count, player_entry.name());
      player_file_count++;
    }
    player_entry.close();
  }

  if (player_file_count == 0) {
    Serial.println("No MOD files found.");
    return;  // Exit if no files were found
  }

  Serial.print("MODs found: ");
  Serial.println(player_file_count);

}

void PlayerInitSdCard(){
  if (!SD.begin(13, 1000000UL * 1000, SPI1)) {
    Serial.println("SD initialization failed!");
    while (1);
  }
  Serial.println("SD initialized successfully.");

  player_root = SD.open("/");
  if (!player_root) {
    Serial.println("Failed to open directory.");
    return;  // Exit if the directory cannot be opened
  }
}

void PlayerPlayFile(int index) {
  File root = SD.open("/mods");
  root.rewindDirectory();

  while (File entry = root.openNextFile()) {
    if (index <= 0 && strstr(entry.name(), ".mod")) {
      Serial.print("Playing: ");
      Serial.println(entry.name());

      // Safely update nowPlaying using critical section
      noInterrupts();
      strncpy(player_now_playing, entry.name(), sizeof(player_now_playing) - 1);
      player_now_playing[sizeof(player_now_playing) - 1] = '\0';  // Ensure null termination

      // Show now playing message
      UiEnableNowPlayingPopup();  // Store the current time for display duration

      interrupts();

      player_fileO = new AudioFileSourceSD(entry.name());
      player_mod->begin(player_fileO, player_out);
      entry.close();
      break;
    }
    entry.close();
    index--;
  }
}

void PlayerSetVolume(float volume){
  player_out->SetGain(volume/100);
}

void PlayerHandleButtons() {
    if (IoGetButtonState(io_buttons[kIoButtonTop])) {
        player_mod->stop();
        player_file_index = (player_file_index + 1) % player_file_count;  // Play the next file in the list
        PlayerPlayFile(player_file_index);
    }
    if (IoGetButtonState(io_buttons[kIoButtonMid])) {
        Serial.println("Button 2 pressed");
        UiEnableNowPlayingPopup();  // Store the current time for display duration
    }
    if (IoGetButtonState(io_buttons[kIoButtonBot])) {
        player_mod->stop();
        // Handle wrap-around for decrementing
        player_file_index = (player_file_index - 1 + player_file_count) % player_file_count;
        PlayerPlayFile(player_file_index);
    }
}

void PlayerHandlePlayback(){

  if (player_mod->isRunning()) {
    if (!player_mod->loop()) {
      Serial.println("Audio loop finished.");
      player_mod->stop();
    }
    PlayerHandleButtons();
  } else if (1) { // check if no buttons are pressed
    Serial.println("MOD done");
    player_file_index = (player_file_index + 1) % player_file_count;  // Play the next file in the list
    PlayerPlayFile(14);
  }

}