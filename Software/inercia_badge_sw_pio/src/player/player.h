#ifndef __PLAYER__H__
#define __PLAYER__H__

#include "player_config.h"

#include <SPI.h>
#include "AudioGeneratorMOD.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceSD.h"
#include "io.h"

extern AudioGeneratorMOD *player_mod;
extern AudioFileSourceSD *player_fileO;
extern AudioOutputI2S *player_out;
extern uint16_t player_file_count;

extern File player_root;
extern File player_entry;

void PlayerSetGain(float gain);

void PlayerInit(void);

void PlayerInitSdCard();


#endif  //!__PLAYER__H__