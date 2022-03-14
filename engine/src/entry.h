#pragma once

#include "core/application.h"
#include "core/log.h"
#include "core/memory.h"
#include "game_types.h"

// External-defined function to create a game.
extern bool create_game(hn::Game &out_game);

// The main entry point of the application.
int main() {
  hn::mem::initialize();

  // Request the game instance from the application.
  hn::Game game{};
  if (!create_game(game)) {
    return 1;
  } else if (!game.validate()) {
    return 2;
  }

  // Initialization.
  if (!hn::application::create(game)) {
    HN_error("Application failed to create.");
    return 3;
  }

  // Begin the game loop.
  if (!hn::application::run()) {
    HN_error("Application did not terminate gracefully.");
    return 4;
  }

  hn::mem::terminate();

  return 0;
}
