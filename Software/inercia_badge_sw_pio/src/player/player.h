#ifndef __PLAYER__H__
#define __PLAYER__H__

#include "player_config.h"

#include <SPI.h>
#include "AudioGeneratorMOD.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceSD.h"
#include "io.h"

AudioGeneratorMOD *player_mod;
AudioFileSourceSD *player_fileO;
AudioOutputI2S *player_out;
uint16_t player_file_count = 0;

File player_root;
File player_entry;

#endif  //!__PLAYER__H__