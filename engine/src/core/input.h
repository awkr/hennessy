#pragma once

#include "defines.h"

namespace hn::input {

enum Button {
  ButtonUnknown,
  ButtonLeft,
  ButtonRight,
  ButtonMiddle,
  ButtonMax,
};

#define DEFINE_KEY(name, code) Key##name = code

enum Key {
  DEFINE_KEY(BackSpace, 0x08),
  DEFINE_KEY(Enter, 0x0D),
  DEFINE_KEY(Shift, 0x10),
  DEFINE_KEY(Control, 0x11),
  DEFINE_KEY(ESC, 0x1B),
  DEFINE_KEY(Space, 0x20),
  DEFINE_KEY(Left, 0x25),
  DEFINE_KEY(Up, 0x26),
  DEFINE_KEY(Right, 0x27),
  DEFINE_KEY(Down, 0x28),
  DEFINE_KEY(Delete, 0x2E),
  DEFINE_KEY(A, 0x41),
  DEFINE_KEY(B, 0x42),
  DEFINE_KEY(C, 0x43),
  DEFINE_KEY(D, 0x44),
  DEFINE_KEY(S, 0x53),
  DEFINE_KEY(W, 0x57),
  KeyMax,
};

bool initialize();
void terminate();
void update(f64 delta_time);

// Keyboard input.

/**
 * Indicates if the given key is currently pressed down.
 * @param key They key to be checked.
 * @returns True if currently pressed; otherwise false.
 */
bool is_key_down(Key key);
bool is_key_up(Key key);

/**
 * Indicates if the given key was previously pressed down on the last frame.
 * @param key They key to be checked.
 * @returns True if was previously pressed; otherwise false.
 */
bool was_key_down(Key key);
bool was_key_up(Key key);

/**
 * Sets the state for the given key.
 * @param key The key to be processed.
 * @param pressed Indicates whether the key is currently pressed.
 */
void process_key(Key key, bool pressed);

// Mouse input.

bool is_button_down(Button button);
bool is_button_up(Button button);
bool was_button_down(Button button);
bool was_button_up(Button button);
void get_mouse_position(i32 &x, i32 &y);
void get_previous_mouse_position(i32 &x, i32 &y);
void process_button(Button button, bool pressed);
void process_mouse_move(f32 x, f32 y);
void process_mouse_wheel(f32 delta_x, f32 delta_y);

} // namespace hn::input
