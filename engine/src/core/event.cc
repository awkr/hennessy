#include "event.h"
#include "container/darray.h"
#include "log.h"
#include "memory.h"

namespace hn::event {

struct RegisteredEvent {
  void        *listener;
  PFN_on_event callback;
};

struct EventCodeEntry {
  RegisteredEvent *events = nullptr;
};

// This should be more than enough codes...
const int max_message_codes = 100;

// State structure.
struct EventSystemState {
  // Lookup table for event codes.
  EventCodeEntry registered[max_message_codes];
};

// Event system internal state.
static EventSystemState state{};

bool initialize() {
  static bool initialized = false;
  if (initialized) {
    return false;
  }

  hn::mem::zero(&state, sizeof(state));

  initialized = true;
  HN_debug("Event subsystem initialized.");
  return true;
}

void terminate() {
  // Free the events arrays. Objects pointed to should be destroyed on their own.
  for (auto &entry : state.registered) {
    if (entry.events) {
      darray_destroy(entry.events);
    }
  }
}

bool register_to_listen(u16 code, void *listener, PFN_on_event on_event) {
  if (!state.registered[code].events) {
    state.registered[code].events = (RegisteredEvent *)darray_create(RegisteredEvent);
  }

  u64 registered_count = darray_length(state.registered[code].events);
  for (u64 i = 0; i < registered_count; ++i) {
    if (state.registered[code].events[i].listener == listener) {
      // TODO warn
      return false;
    }
  }

  // At this point, no duplicate was found. Proceed with registrations.
  RegisteredEvent event{listener, on_event};
  darray_push(state.registered[code].events, event);

  return true;
}

bool unregister_from_listen(u16 code, void *listener, PFN_on_event on_event) {
  if (!state.registered[code].events) {
    // TODO warn
    return false;
  }

  u64 registered_count = darray_length(state.registered[code].events);
  for (u64 i = 0; i < registered_count; ++i) {
    auto e = state.registered[code].events[i];
    if (e.listener == listener && e.callback == on_event) {
      // Found one, remove it.
      RegisteredEvent popped_event{};
      darray_pop_at(state.registered[code].events, i, &popped_event);
      return false;
    }
  }

  // Not found.
  return false;
}

bool fire(u16 code, void *sender, const Context &ctx) {
  // If nothing is registered for the code, boot out.
  if (!state.registered[code].events) {
    // TODO warn
    return false;
  }

  u64 registered_count = darray_length(state.registered[code].events);
  for (u64 i = 0; i < registered_count; ++i) {
    auto e = state.registered[code].events[i];
    if (e.callback(code, sender, e.listener, ctx)) {
      // Message has been consumed, do not send to other listeners.
      return false;
    }
  }

  // Not found.
  return false;
}

} // namespace hn::event
