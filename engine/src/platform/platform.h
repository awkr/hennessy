#pragma once

#include "defines.h"

namespace hn::platform {

struct State {
  void *pState = nullptr; // internal state
  bool  quit   = false;
};

bool initialize(State *state, const char *application_name, i32 x, i32 y, u32 width, u32 height);
void terminate(State *state);

bool poll_events(State *state);

void *allocate(u64 size, bool aligned = false);
void  free(void *block, bool aligned = false);
void *memory_zero(void *block, u64 size);
void *memory_copy(void *dst, const void *src, u64 size);
void *memory_set(void *dst, i32 value, u64 size);

void console_write(const char *message, u8 color);
void console_write_error(const char *message, u8 color);

f64 get_system_time();

void sleep(u64 ms);

} // namespace hn::platform
