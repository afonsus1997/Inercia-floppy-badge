
#include "player.h"

AudioGeneratorMOD *player_mod;
AudioFileSourceSD *player_fileO;
AudioOutputI2S *player_out;
uint16_t player_file_count = 0;
File player_root;
File player_entry;


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