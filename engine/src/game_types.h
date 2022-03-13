#pragma once

#include "core/application.h"

namespace hn {

// Represents the basic game state in a game.
// Called for creation by the application.
struct Game {
  application::Config config{};
  // Function pointer to game's initialize function.
  bool (*initialize)(struct Game *game)                       = nullptr;
  bool (*update)(struct Game *game, f32 delta_time)           = nullptr;
  bool (*render)(struct Game *game, f32 delta_time)           = nullptr;
  void (*on_resize)(struct Game *game, u16 width, u16 height) = nullptr;
  // Game-specific game state. Created and managed by the game.
  void *state = nullptr;

  bool validate() const { return initialize && update && render && on_resize; }
};

} // namespace hn
