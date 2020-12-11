#include "game_settings.h"

void settings_init(GameSettings* settings) {
    settings->player_speed_modifier = 1.0f;
    settings->enable_checkpoints = 0;
}