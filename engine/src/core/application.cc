#include "application.h"
#include "event.h"
#include "game_types.h"
#include "input.h"
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

// Event handlers.

bool on_event(u16 code, void *sender, void *listener, const event::Context &ctx);
bool on_key(u16 code, void *sender, void *listener, const event::Context &ctx);

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
  if (!input::initialize()) {
    HN_error("Input system failed to initialize. Application cannot continue.");
    return false;
  }

  event::register_to_listen(event::SystemEventCode::ApplicationQuit, nullptr, on_event);
  event::register_to_listen(event::SystemEventCode::KeyPressed, nullptr, on_key);
  event::register_to_listen(event::SystemEventCode::KeyReleased, nullptr, on_key);

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
      // Note: Input update/state copying should always be handled after any input should be
      // recorded; I.E. before this line. As a safety, input is the last thing to be updated before
      // this frame ends.
      input::update(0);
    }
  }

  event::unregister_from_listen(event::SystemEventCode::ApplicationQuit, nullptr, on_event);
  event::unregister_from_listen(event::SystemEventCode::KeyPressed, nullptr, on_key);
  event::unregister_from_listen(event::SystemEventCode::KeyReleased, nullptr, on_key);

  input::terminate();
  event::terminate();
  platform::terminate(&app_state.platform);

  // Do some cleaning.
  platform::free(app_state.game->state);

  return true;
}

bool on_event(u16 code, void *sender, void *listener, const event::Context &ctx) {
  switch (code) {
  case event::SystemEventCode::ApplicationQuit: {
    HN_info("event::SystemEventCode::ApplicationQuit received, shutting down.");
    app_state.is_running = false;
    return true;
  }
  }
  return false;
}

bool on_key(u16 code, void *sender, void *listener, const event::Context &ctx) {
  if (code == event::SystemEventCode::KeyPressed) {
    auto key_code = ctx.data.u16[0];
    if (key_code == input::KeyA) {
      HN_debug("Explicit - 'A' key pressed.");
    } else {
      HN_debug("'%c' key pressed.", key_code);
    }
  } else if (code == event::SystemEventCode::KeyReleased) {
    auto key_code = ctx.data.u16[0];
    if (key_code == input::KeyESC) {
      // NOTE: Technically firing an event to itself, but there may be other listeners.
      event::fire(event::SystemEventCode::ApplicationQuit, nullptr, {});

      // Block anything else from processing this.
      return true;
    } else if (key_code == input::KeyB) {
      HN_debug("Explicit - 'B' key released.");
    } else {
      HN_debug("'%c' key released.", key_code);
    }
  }
  return false;
}

} // namespace hn::application
