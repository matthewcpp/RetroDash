#ifndef XBOX_AUDIO_H
#define XBOX_AUDIO_H

#include "../audio.h"

struct Music {
    short* samples;
    int sample_count;
};


#define XBOX_AUDIO_NUM_BUFFERS 2

struct Audio {
    char* _asset_dir;
    Music* current_music;

    unsigned char* playback_buffers[XBOX_AUDIO_NUM_BUFFERS];
    unsigned int current_buf;

    unsigned char* voice_data;
    unsigned int voice_len;
    unsigned int voice_pos;

    int is_playing;
};



Audio* xbox_audio_create(const char* asset_dir);
void xbox_audio_init(Audio* audio);
void xbox_audio_destroy(Audio* audio);
#endif