#ifndef AUDIO_H
#define AUDIO_H

typedef struct Audio Audio;

typedef struct Music Music;

Music* audio_load_music(Audio* audio, const char* path);
void audio_destroy_music(Audio* audio, Music* music);

void audio_play_music(Audio* audio, Music* music);
void audio_pause_music(Audio* audio);
void audio_resume_music(Audio* audio);
void audio_restart_music(Audio* audio);
void audio_set_music_volume(Audio* audio, float volume);

#endif