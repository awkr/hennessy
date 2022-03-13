#include "memory.h"
#include "core/log.h"
#include "platform/platform.h"
#include <cstring>

namespace hn::mem {

struct Stats {
  u64 allocated;
  u64 tagged_allocations[TagMax];
};

static const char *tag_names[TagMax] = {"Unknown",   "Array",   "DArray",   "Map",      "BST",
                                        "String",    "Texture", "Material", "Renderer", "Game",
                                        "Transform", "Entity",  "Scene",    "Resource"};

static Stats stats{};

void initialize() { platform::memory_zero(&stats, sizeof(Stats)); }

void terminate() {}

void *allocate(u64 size, Tag tag) {
  if (tag == TagUnknown) {
    HN_warn("allocation called using TagUnknown. Re-class this allocation.")
  }
  stats.allocated += size;
  stats.tagged_allocations[tag] += size;

  // TODO memory alignment
  void *block = platform::allocate(size);
  platform::memory_zero(block, size);
  return block;
}

void free(void *block, u64 size, Tag tag) {
  if (tag == TagUnknown) {
    HN_warn("free called using TagUnknown. Re-class this allocation.")
  }
  stats.allocated -= size;
  stats.tagged_allocations[tag] -= size;

  // TODO memory alignment
  platform::free(block);
}

void *memory_zero(void *block, u64 size) { return platform::memory_zero(block, size); }

void *memory_copy(void *dst, const void *src, u64 size) {
  return platform::memory_copy(dst, src, size);
}

void *memory_set(void *dst, i32 value, u64 size) { return platform::memory_set(dst, value, size); }

const char *get_memory_usage() {
  const u64 gib = 1024 * 1024 * 1024;
  const u64 mib = 1024 * 1024;
  const u64 kib = 1024;

  char buffer[2048] = "System memory usage:\n";
  u64  offset       = strlen(buffer);
  for (u16 i = 0; i < TagMax; ++i) {
    char  unit[4] = "Xib";
    float amount  = 1.0f;
    if (stats.tagged_allocations[i] >= gib) {
      unit[0] = 'G';
      amount  = stats.tagged_allocations[i] / (float)gib;
    } else if (stats.tagged_allocations[i] >= mib) {
      unit[0] = 'M';
      amount  = stats.tagged_allocations[i] / (float)mib;
    } else if (stats.tagged_allocations[i] >= kib) {
      unit[0] = 'K';
      amount  = stats.tagged_allocations[i] / (float)kib;
    } else {
      unit[0] = 'B';
      unit[1] = 0;
      amount  = stats.tagged_allocations[i];
    }
    i32 length = snprintf(buffer + offset, 2048, "  %-10s: %.2f %s\n", tag_names[i], amount, unit);
    offset += length;
  }
  return strdup(buffer);
}

} // namespace hn::mem
