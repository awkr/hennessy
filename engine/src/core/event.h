#pragma once

#include "defines.h"

namespace hn::event {

struct Context {
  // 128 bytes
  union {
    i64 i64[2];
    u64 u64[2];
    f64 f64[2];

    i32 i32[2];
    u32 u32[2];
    f32 f32[2];

    i16 i16[2];
    u16 u16[2];

    i8 i8[2];
    u8 u8[2];

    char c[16];
  } data;
};

// Should return true if handled.
typedef bool (*PFN_on_event)(u16 code, void *sender, void *listener, const Context &ctx);

bool initialize();
void terminate();

/**
 * Register to listen for when events are sent with the provided code. Events with duplicate
 * listener/callback combos will not be registered again and will cause this to return false.
 * @param code The event code to listen for.
 * @param listener A pointer to a listener instance. Can be 0/null.
 * @param on_event The callback function pointer to be invoked when the event code is fired.
 * @return True if the event is successfully registered; otherwise false.
 */
bool register_to_listen(u16 code, void *listener, PFN_on_event on_event);
bool unregister_from_listen(u16 code, void *listener, PFN_on_event on_event);
bool fire(u16 code, void *sender, const Context &ctx);

// System internal event code. Application should use codes beyond 255.
enum SystemEventCode {
  // Shuts the application down on the next frame.
  EventCodeApplicationQuit = 0x01,

  /** Keyboard key pressed.
   * Context usage:
   * u16 key_code = ctx.data.u16[0];
   */
  EventCodeKeyPressed = 0x02,

  /** Keyboard key released.
   * Context usage:
   * u16 key_code = ctx.data.u16[0];
   */
  EventCodeKeyReleased = 0x03,

  /** Mouse button pressed.
   * Context usage:
   * u16 button = ctx.data.u16[0];
   */
  EventCodeButtonPressed = 0x04,

  /** Mouse button released.
   * Context usage:
   * u16 button = ctx.data.u16[0];
   */
  EventCodeButtonReleased = 0x05,

  /** Mouse moved.
   * Context usage:
   * u16 x = ctx.data.u16[0];
   * u16 y = ctx.data.u16[1];
   */
  EventCodeMouseMoved = 0x06,

  /** Mouse moved.
   * Context usage:
   * u8 z_delta = ctx.data.u8[0];
   */
  EventCodeMouseWheel = 0x07,

  /** Resized/resolution changed from the OS.
   * Context usage:
   * u16 width = ctx.data.u16[0];
   * u16 height = ctx.data.u16[1];
   */
  EventCodeResized = 0x08,

  // Special-purpose debugging event. Context will vary over time.
  EventCodeDebug0 = 0x10,
  // Special-purpose debugging event. Context will vary over time.
  EventCodeDebug1 = 0x11,
  // Special-purpose debugging event. Context will vary over time.
  EventCodeDebug2 = 0x12,
  // Special-purpose debugging event. Context will vary over time.
  EventCodeDebug3 = 0x13,
  // Special-purpose debugging event. Context will vary over time.
  EventCodeDebug4 = 0x14,

  // The maximum event code that can be used internally.
  EventCodeMax = 0xFF
};

} // namespace hn::event
