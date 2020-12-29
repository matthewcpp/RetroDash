#include "xbox_audio.h"

#include "../util.h"

#include "stb_vorbis.c"

#include <hal/audio.h>
#include <hal/debug.h>

#include <windows.h>

void xbox_audio_callback(void* device, void* data);

Audio* xbox_audio_create(const char* asset_dir) {
    Audio* audio = calloc(1, sizeof(Audio));
    audio->_asset_dir = malloc(strlen(asset_dir) + 1);
    strcpy(audio->_asset_dir, asset_dir);

    return audio;
}

void xbox_audio_destroy(Audio* audio) {
    if (audio->is_playing) {
        XAudioPause();
    }

    free(audio);
}

const unsigned short buffer_size = 48*1024;
#define MAXRAM 0x03FFAFFF

void xbox_audio_init(Audio* audio) {
    for (int i = 0; i < XBOX_AUDIO_NUM_BUFFERS; i++) {
        audio->playback_buffers[i] = MmAllocateContiguousMemoryEx(buffer_size, 0, MAXRAM, 0, (PAGE_READWRITE | PAGE_WRITECOMBINE));
        memset(audio->playback_buffers[i], 0, buffer_size);
    }

    /* Initialize audio subsystem to begin playback */
    XAudioInit(16, 2, &xbox_audio_callback, audio);
}

#define MIN(x,y) ((x)<(y)?(x):(y))

void xbox_audio_callback(void* device, void* data) {
    Audio* audio = (Audio*)data;

    int is_final = (audio->voice_pos + buffer_size) >= audio->voice_len;
    int chunk_size = MIN(audio->voice_len - audio->voice_pos, buffer_size);

    memcpy(audio->playback_buffers[audio->current_buf], audio->voice_data + audio->voice_pos, chunk_size);
    XAudioProvideSamples(audio->playback_buffers[audio->current_buf], chunk_size, is_final);

    if (is_final) {
        audio->voice_pos = 0;
    } else {
        audio->voice_pos = audio->voice_pos + chunk_size;
    }

    audio->current_buf = (audio->current_buf + 1) % XBOX_AUDIO_NUM_BUFFERS;

    (void)device;
}

Music* audio_load_music(Audio* audio, const char* path){
    size_t path_len = strlen(audio->_asset_dir) + strlen(path) + 2;
    char* full_path = malloc(path_len);
    sprintf(full_path, "%s/%s", audio->_asset_dir, path);
    massage_path(full_path, path_len);

    short* decoded;
    int channels, len,  sample_rate;
    len = stb_vorbis_decode_filename(full_path, &channels, &sample_rate, &decoded);

    if (len < 0 && audio->error_callback != NULL)
        audio->error_callback(full_path);

    free(full_path);

    if (len < 0)
        return NULL;

    Music* music = malloc(sizeof(Music));
    music->samples = decoded;
    music->sample_count = len;

    return music;
}

void audio_destroy_music(Audio* audio, Music* music) {
    free(music->samples);
    free(music);
}

void audio_play_music(Audio* audio, Music* music) {
    if (music == NULL || music == audio->current_music)
        return;

    audio->current_music = music;

    audio->voice_data = (unsigned char*)music->samples;
    audio->voice_len = music->sample_count * sizeof (int16_t);
    audio->voice_pos = 0;
    audio->current_buf = 0;

    if (!audio->is_playing) {
        audio->is_playing = 1;

        /* Provide initial samples to begin playing */
        for (int i = 0; i < XBOX_AUDIO_NUM_BUFFERS; i++) {
            xbox_audio_callback(NULL, audio);
        }

        debugPrint("Initial Samples provided, begin playback\n");
        XAudioPlay();
    }
}

void audio_pause_music(Audio* audio) {
    if (audio->is_playing){
        XAudioPause();
        audio->is_playing = 0;
    }
}

void audio_resume_music(Audio* audio) {
    if (!audio->is_playing){
        XAudioPlay();
        audio->is_playing = 1;
    }
}

void audio_restart_music(Audio* audio) {
    audio->voice_pos = 0;

    if (!audio->is_playing) {
        audio->is_playing = 1;
        XAudioPlay();
    }
}

void audio_set_music_volume(Audio* audio, float volume) {
    (void) audio;
    (void) volume;
}

int audio_is_playing_music(Audio* audio) {
    return audio->is_playing;
}

void audio_set_error_callback(Audio* audio, AudioErrorFunc func) {
    audio->error_callback = func;
}