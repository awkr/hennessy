#pragma once

#include "defines.h"

namespace hn ::mem {

enum Tag {
  TagUnknown,
  TagArray,
  TagDArray,
  TagMap,
  TagBST,
  TagString,
  TagTexture,
  TagMaterial,
  TagRenderer,
  TagGame,
  TagTransform,
  TagEntity,
  TagScene,
  TagResource,
  TagMax,
};

void initialize();
void terminate();

void *allocate(u64 size, Tag tag);
void  free(void *block, u64 size, Tag tag);
void *memory_zero(void *block, u64 size);
void *memory_copy(void *dst, const void *src, u64 size);
void *memory_set(void *dst, i32 value, u64 size);

const char *get_memory_usage();

} // namespace hn::mem
