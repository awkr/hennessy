#include "game.h"
#include <core/memory.h>
#include <entry.h>

// Define the function to create a game.
bool create_game(hn::Game *out_game) {
  out_game->config.name   = "Test";
  out_game->config.x      = 100;
  out_game->config.y      = 100;
  out_game->config.width  = 120;
  out_game->config.height = 120;
  out_game->initialize    = game_initialize;
  out_game->update        = game_update;
  out_game->render        = game_render;
  out_game->on_resize     = game_on_resize;
  // Create the game state.
  out_game->state = hn::mem::allocate(sizeof(GameState), hn::mem::TagGame);
  return true;
}
