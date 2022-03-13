#include "application.h"
#include "game_types.h"
#include "log.h"
#include "memory.h"
#include "platform/platform.h"

namespace hn::application {

struct State {
  Game           *game;
  bool            is_running;
  bool            is_suspended;
  platform::State platform;
  u16             width;
  u16             height;
  f32             last_time;
};

static State app_state;

bool create(Game &game) {
  static bool initialized = false;
  if (initialized) {
    return false;
  }

  app_state.game = &game;

  // Initialize subsystems.
  log::initialize();

  app_state.is_running   = true;
  app_state.is_suspended = false;

  if (!platform::initialize(&app_state.platform, game.config.name, game.config.x, game.config.y,
                            game.config.width, game.config.height)) {
    return false;
  }

  // Initialize the game.
  if (!game.initialize(&game)) {
    HN_error("Game failed to initialize.");
    return false;
  }

  initialized = true;

  return true;
}

bool run() {
  HN_debug("%s", hn::mem::get_memory_usage());

  while (app_state.is_running) {
    if (!platform::poll_events(&app_state.platform)) {
      app_state.is_running = false;
    }
    if (!app_state.is_suspended) {
      if (!app_state.game->update(app_state.game, 0)) {
        HN_error("Game failed to update. Terminating.");
        app_state.is_running = false;
        break;
      }
      if (!app_state.game->render(app_state.game, 0)) {
        HN_error("Game failed to render. Terminating.");
        app_state.is_running = false;
        break;
      }
    }
  }

  platform::terminate(&app_state.platform);

  // Do some cleaning.
  platform::free(app_state.game->state);

  return true;
}

} // namespace hn::application
