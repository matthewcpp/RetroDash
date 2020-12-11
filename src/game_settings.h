#ifndef GAME_SETTINGS
#define GAME_SETTINGS

typedef struct {
    float player_speed_modifier;
    int enable_checkpoints;
} GameSettings;


void settings_init(GameSettings* settings);

#endif