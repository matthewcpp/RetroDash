#include "xbox_audio.h"

#include "../util.h"

#include "stb_vorbis.c"

#include <hal/audio.h>
#include <windows.h>

#include <threads.h>

typedef enum {
    MUSIC_STATE_STOPPED,
    MUSIC_STATE_PLAYING,
    MUSIC_STATE_PAUSED
} MusicState;

struct BufferNode {
    unsigned char* buffer;
    int size;
    struct BufferNode* next;
};

struct Music {
    stb_vorbis* handle;
};

struct Audio {
    char* _asset_dir;
    Music* current_music;

    AudioErrorFunc error_callback;
    MusicState music_state;

    thrd_t streaming_thread;
    mtx_t streaming_mutex;

    struct BufferNode* queue;
    struct BufferNode* available_buffers;
};

void xbox_audio_callback(void* device, void* data);
int _xbox_audio_stream_thread(void* data);

const unsigned short buffer_size = 48*1024;
const size_t buffer_count = 4;
#define MAXRAM 0x03FFAFFF

Audio* xbox_audio_create(const char* asset_dir) {
    Audio* audio = calloc(1, sizeof(Audio));

    audio->music_state = MUSIC_STATE_STOPPED;

    audio->_asset_dir = malloc(strlen(asset_dir) + 1);
    strcpy(audio->_asset_dir, asset_dir);

    for (size_t i  = 0; i < buffer_count; i++) {
        struct BufferNode* buffer_node = calloc(1, sizeof(struct BufferNode));
        buffer_node->buffer = MmAllocateContiguousMemoryEx(buffer_size, 0, MAXRAM, 0, (PAGE_READWRITE | PAGE_WRITECOMBINE));

        buffer_node->next = audio->available_buffers;
        audio->available_buffers = buffer_node;
    }

    mtx_init(&audio->streaming_mutex, mtx_plain);

    XAudioInit(16, 2, &xbox_audio_callback, audio);
    thrd_create(&audio->streaming_thread, _xbox_audio_stream_thread, audio);

    return audio;
}


/** Precondition: audio is currently playing music. */
void _xbox_audio_do_fill_buffers(Audio* audio) {
    while (audio->available_buffers) {
        struct BufferNode* buffer_node = audio->available_buffers;

        int samples_per_channel = stb_vorbis_get_samples_short_interleaved(audio->current_music->handle, 2, (short*)buffer_node->buffer, buffer_size / sizeof(short));

        buffer_node->size = samples_per_channel * sizeof(short) * 2;

        if (!audio->queue) {
            audio->queue = buffer_node;
        }
        else {
            struct BufferNode* tail = audio->queue;

            while (tail->next)
                tail = tail->next;

            tail->next = buffer_node;
        }

        audio->available_buffers = buffer_node->next;
        buffer_node->next = NULL;
    }
}

void _xbox_audio_fill_buffers(Audio* audio) {
    mtx_lock(&audio->streaming_mutex);

    if (!audio->current_music || !audio->available_buffers) {
        mtx_unlock(&audio->streaming_mutex);
        return;
    }

    _xbox_audio_do_fill_buffers(audio);
    mtx_unlock(&audio->streaming_mutex);
}

int _xbox_audio_stream_thread(void* data) {
    Audio* audio = (Audio*)data;

    for (;;) {
        _xbox_audio_fill_buffers(audio);
        Sleep(16);
    }

    return 1;
}

void xbox_audio_callback(void* device, void* data) {
    Audio* audio = (Audio*)data;
    mtx_lock(&audio->streaming_mutex);
    
    struct BufferNode* buffer_node = audio->queue;

    audio->queue = buffer_node->next;

    int is_final = buffer_node->size == 0;
    if (is_final){
        memset(buffer_node->buffer, 0, buffer_size);
        buffer_node->size = buffer_size;
    }

    XAudioProvideSamples(buffer_node->buffer, buffer_node->size, is_final);

    buffer_node->next = audio->available_buffers;
    audio->available_buffers = buffer_node;
    
    mtx_unlock(&audio->streaming_mutex);

    (void)device;
}

Music* audio_load_music(Audio* audio, const char* path){
    size_t path_len = strlen(audio->_asset_dir) + strlen(path) + 6;
    char* full_path = malloc(path_len);
    sprintf(full_path, "%s/%s.ogg", audio->_asset_dir, path);
    massage_path(full_path, path_len - 1);

    int error;
    stb_vorbis* handle = stb_vorbis_open_filename(full_path, &error, NULL);

    if (!handle) {
        if (audio->error_callback)
            audio->error_callback(full_path);

        return NULL;
    }

    Music* music = malloc(sizeof(Music));
    music->handle = handle;

    return music;
}

/** places all buffers in the queue back in the available list. */
void _audio_reset_buffers(Audio* audio) {
    while (audio->queue) {
        struct BufferNode* buffer_node = audio->queue;
        audio->queue = buffer_node->next;

        buffer_node->next = audio->available_buffers;
        audio->available_buffers = buffer_node;
    }
}

void audio_destroy_music(Audio* audio, Music* music) {
    mtx_lock(&audio->streaming_mutex);

    if (music == audio->current_music) {
        XAudioPause();

        _audio_reset_buffers(audio);
        audio->current_music = NULL;
        audio->music_state = MUSIC_STATE_PAUSED;
    }

    mtx_unlock(&audio->streaming_mutex);

    stb_vorbis_close(music->handle);
    free(music);
}

void _xbox_audio_initial_music_load(Audio* audio) {
    _audio_reset_buffers(audio);
    _xbox_audio_do_fill_buffers(audio);
    xbox_audio_callback(NULL, audio);
    xbox_audio_callback(NULL, audio);
}

void audio_play_music(Audio* audio, Music* music) {
    mtx_lock(&audio->streaming_mutex);
    if (music == NULL || music == audio->current_music) {
        mtx_unlock(&audio->streaming_mutex);
        return;
    }

    if (audio->current_music != NULL) {
        stb_vorbis_seek_start(audio->current_music->handle);
    }

    audio->current_music = music;
    _xbox_audio_initial_music_load(audio);


    if (audio->music_state != MUSIC_STATE_PLAYING) {
        XAudioPlay();
        audio->music_state = MUSIC_STATE_PLAYING;
    }

    mtx_unlock(&audio->streaming_mutex);
}

void audio_pause_music(Audio* audio) {
    mtx_lock(&audio->streaming_mutex);

    if (audio->music_state == MUSIC_STATE_PLAYING){
        XAudioPause();
        audio->music_state = MUSIC_STATE_PAUSED;
    }
    
    mtx_unlock(&audio->streaming_mutex);
}

void audio_resume_music(Audio* audio) {
    mtx_lock(&audio->streaming_mutex);

    if (audio->music_state == MUSIC_STATE_PAUSED && audio->current_music){
        XAudioPlay();
        audio->music_state = MUSIC_STATE_PLAYING;
    }

    mtx_unlock(&audio->streaming_mutex);
}

void audio_restart_music(Audio* audio) {
    mtx_lock(&audio->streaming_mutex);
    if (!audio->current_music) {
        mtx_unlock(&audio->streaming_mutex);
        return;
    }

    stb_vorbis_seek_start(audio->current_music->handle);

    _xbox_audio_initial_music_load(audio);
    if (audio->music_state != MUSIC_STATE_PLAYING) {
        XAudioPlay();
        audio->music_state = MUSIC_STATE_PLAYING;
    }

    mtx_unlock(&audio->streaming_mutex);
}

void audio_set_music_volume(Audio* audio, float volume) {
    (void) audio;
    (void) volume;
}

int audio_is_playing_music(Audio* audio) {
    return audio->music_state == MUSIC_STATE_PLAYING;
}

void audio_set_error_callback(Audio* audio, AudioErrorFunc func) {
    audio->error_callback = func;
}