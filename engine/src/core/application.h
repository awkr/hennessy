#pragma once

#include "defines.h"

namespace hn {

struct Game;

}

namespace hn::application {

// Application configuration.
struct Config {
  const char *name;   // The application name used in windowing.
  u16         x;      // Window starting position x-axis.
  u16         y;      // Window starting position y-axis.
  u16         width;  // Window starting width.
  u16         height; // Window starting height.
};

bool create(Game &game);

bool run();

} // namespace hn::application
