#include "application.h"
#include "event.h"
#include "game_types.h"
#include "log.h"
#include "memory.h"
#include "platform/platform.h"

namespace hn::application {

struct State {
  Game           *game         = nullptr;
  bool            is_running   = false;
  bool            is_suspended = false;
  platform::State platform{};
  u16             width     = 120;
  u16             height    = 120;
  f32             last_time = 0;
};

static State app_state{};

bool create(Game &game) {
  static bool initialized = false;
  if (initialized) {
    return false;
  }

  app_state.game = &game;

  // Initialize subsystems.
  log::initialize();
  if (!event::initialize()) {
    HN_error("Event system failed to initialize. Application cannot continue.");
    return false;
  }

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

  event::terminate();
  platform::terminate(&app_state.platform);

  // Do some cleaning.
  platform::free(app_state.game->state);

  return true;
}

} // namespace hn::application
