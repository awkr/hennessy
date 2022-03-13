#pragma once

#include <cstdio>

#define HN_log(level, message, ...)                                                                \
  fprintf(stdout, level " %s:%s:%d " message "\n", __FILE_NAME__, __FUNCTION__, __LINE__,          \
          ##__VA_ARGS__);

#define HN_debug(message, ...) HN_log("[D]", message, ##__VA_ARGS__)
#define HN_warn(message, ...)  HN_log("\033[1;33m[W]\033[0m", message, ##__VA_ARGS__)
#define HN_error(message, ...) HN_log("\033[1;31m[E]\033[0m", message, ##__VA_ARGS__)
#define HN_fatal(message, ...) HN_log("\033[1;41m[F]\033[0m", message, ##__VA_ARGS__)
