#pragma once

#include <defines.h>
#include <game_types.h>

struct GameState {
  f32 delta_time;
};

bool game_initialize(hn::Game *game);
bool game_update(hn::Game *game, f32 delta_time);
bool game_render(hn::Game *game, f32 delta_time);
void game_on_resize(hn::Game *game, u16 width, u16 height);
