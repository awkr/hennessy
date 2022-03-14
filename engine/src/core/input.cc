#include "input.h"
#include "event.h"
#include "log.h"
#include "memory.h"

namespace hn::input {

struct KeyboardState {
  bool keys[KeyMax];
};

struct MouseState {
  f32  x;
  f32  y;
  bool buttons[ButtonMax];
};

struct InputState {
  KeyboardState keyboard_current;
  KeyboardState keyboard_previous;
  MouseState    mouse_current;
  MouseState    mouse_previous;
};

static bool       initialized = false;
static InputState state{};

bool initialize() {
  if (initialized) {
    return false;
  }
  hn::mem::zero(&state, sizeof(state));
  initialized = true;
  HN_debug("Input subsystem initialized.");
  return true;
}

void terminate() { initialized = false; }

void update(f64 delta_time) {
  if (!initialized) {
    return;
  }

  // Copy current states to previous states.
  hn::mem::copy(&state.keyboard_previous, &state.keyboard_current, sizeof(KeyboardState));
  hn::mem::copy(&state.mouse_previous, &state.mouse_current, sizeof(MouseState));
}

bool is_key_down(Key key) {
  if (!initialized) {
    return false;
  }
  return state.keyboard_current.keys[key];
}

bool is_key_up(Key key) {
  if (!initialized) {
    return true;
  }
  return !state.keyboard_current.keys[key];
}

bool was_key_down(Key key) {
  if (!initialized) {
    return false;
  }
  return state.keyboard_previous.keys[key];
}

bool was_key_up(Key key) {
  if (!initialized) {
    return true;
  }
  return !state.keyboard_previous.keys[key];
}

void process_key(Key key, bool pressed) {
  // Only handle this if the state actually changed.
  if (state.keyboard_current.keys[key] == pressed) {
    return;
  }

  // Update internal state.
  state.keyboard_current.keys[key] = pressed;

  // Fire off an event for immediate processing.
  event::Context context{};
  context.data.u16[0] = key;
  event::fire(pressed ? event::SystemEventCode::KeyPressed
                      : event::SystemEventCode::KeyReleased,
              nullptr, context);
}

bool is_button_down(Button button) {
  if (!initialized) {
    return false;
  }
  return state.mouse_current.buttons[button];
}

bool is_button_up(Button button) {
  if (!initialized) {
    return true;
  }
  return !state.mouse_current.buttons[button];
}

bool was_button_down(Button button) {
  if (!initialized) {
    return false;
  }
  return state.mouse_previous.buttons[button];
}

bool was_button_up(Button button) {
  if (!initialized) {
    return true;
  }
  return !state.mouse_previous.buttons[button];
}

void get_mouse_position(i32 &x, i32 &y) {
  if (!initialized) {
    x = 0;
    y = 0;
    return;
  }
  x = state.mouse_current.x;
  y = state.mouse_current.y;
}

void get_previous_mouse_position(i32 &x, i32 &y) {
  if (!initialized) {
    x = 0;
    y = 0;
    return;
  }
  x = state.mouse_previous.x;
  y = state.mouse_previous.y;
}

void process_button(Button button, bool pressed) {
  if (state.mouse_current.buttons[button] == pressed) {
    return;
  }

  state.mouse_current.buttons[button] = pressed;

  event::Context context{};
  context.data.u16[0] = button;
  event::fire(pressed ? event::SystemEventCode::ButtonPressed
                      : event::SystemEventCode::ButtonReleased,
              nullptr, context);
}

void process_mouse_move(f32 x, f32 y) {
  if (state.mouse_current.x == x && state.mouse_current.y == y) {
    return;
  }

  state.mouse_current.x = x;
  state.mouse_current.y = y;

  event::Context context{};
  context.data.f32[0] = x;
  context.data.f32[1] = y;
  event::fire(event::SystemEventCode::MouseMoved, nullptr, context);
}

void process_mouse_wheel(f32 delta_x, f32 delta_y) {
  // Note: No internal state to update.

  event::Context context{};
  context.data.f32[0] = delta_x;
  context.data.f32[1] = delta_y;
  event::fire(event::SystemEventCode::MouseWheel, nullptr, context);
}

} // namespace hn::input
