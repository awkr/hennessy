#include <platform/platform.h>

int main() {
  platform::State state{};
  if (platform::initialize(&state, "Test", 100, 100, 120, 120)) {
    while (!state.quit) { platform::poll_events(&state); }
  }
  platform::terminate(&state);
  return 0;
}
