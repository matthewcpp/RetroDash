#ifndef SDL_AUDIO_H
#define SDL_AUDIO_H

#include "../audio.h"

#include <SDL2/SDL_mixer.h>

struct Audio {
    Music* current_music;
};

struct Music {
    Mix_Music* sdl_music;
};

Audio* sdl_audio_create();
void sdl_audio_destroy();

#endif