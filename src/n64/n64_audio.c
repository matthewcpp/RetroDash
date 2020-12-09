#include "n64_audio.h"

#include <libdragon.h>
#include <mikmod.h>

#include <string.h>

struct Audio {
    Music* current_music;
};

struct Music {
    MODULE* mikmod_module;
};

// Copied from libdragon sample
MIKMODAPI extern UWORD md_mode __attribute__((section (".data")));
MIKMODAPI extern UWORD md_mixfreq __attribute__((section (".data")));

static void init_mikmod() {
    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();

    md_mode |= DMODE_16BITS;
    md_mode |= DMODE_SOFT_MUSIC;
    md_mode |= DMODE_SOFT_SNDFX;
    //md_mode |= DMODE_STEREO;

    md_mixfreq = audio_get_frequency();
    MikMod_Init("");
}

Audio* n64_audio_create() {
    audio_init(44100,2);
    init_mikmod();

    Audio* audio = calloc(1, sizeof(Audio));
    return audio;
}

void n64_audio_destroy(Audio* audio){
    free(audio);
    MikMod_Exit();
}

void n64_audio_update(Audio* audio) {
    MikMod_Update();
    (void)audio;
}

Music* audio_load_music(Audio* audio, const char* path) {
    size_t rom_path_size = 11 + strlen(path); //rom:// + ".mod\0"
    char* rom_path = malloc(rom_path_size);
    sprintf(rom_path, "rom://%s.mod", path);
    MODULE* mikmod_module = Player_Load(rom_path, 256, 0);
    free(rom_path);

    if (!mikmod_module)
        return NULL;

    Music* music = malloc(sizeof(Music));
    music->mikmod_module = mikmod_module;

    (void)audio;

    return music;
}

void audio_destroy_music(Audio* audio, Music* music){
    if (audio->current_music == music) {
        Player_Stop();
        audio->current_music = NULL;
    }

    Player_Free(music->mikmod_module);
    free(music);
}

void audio_play_music(Audio* audio, Music* music){
    audio_write_silence();
    audio_write_silence();
    Player_Start(music->mikmod_module);

    audio->current_music = music;
}

void audio_pause_music(Audio* audio) {
    if (!Player_Paused())
        Player_TogglePause();

    (void)audio;
}

void audio_resume_music(Audio* audio) {
    if (Player_Paused())
        Player_TogglePause();

    (void)audio;
}

void audio_restart_music(Audio* audio) {
    Player_SetPosition(0);
    (void)audio;
}

void audio_set_music_volume(Audio* audio, float volume) {
    Player_SetVolume((int)(volume * 128.0f));
    (void)audio;
}