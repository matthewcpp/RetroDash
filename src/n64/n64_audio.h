#ifndef N64_AUDIO
#define N64_AUDIO

#include "../audio.h"

Audio* n64_audio_create();
void n64_audio_destroy(Audio* audio);
void n64_audio_update(Audio* audio);

#endif