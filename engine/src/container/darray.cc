#include "darray.h"
#include "core/log.h"
#include "core/memory.h"

namespace hn::darray {

void *_create(u64 length, u64 stride) {
  u64  header_size = FieldCount * sizeof(u64);
  u64  array_size  = length * stride;
  u64  total_size  = header_size + array_size;
  u64 *array       = (u64 *)hn::mem::allocate(total_size, hn::mem::TagDArray);
  hn::mem::set(array, 0, total_size);
  array[Capacity] = length;
  array[Length]   = 0;
  array[Stride]   = stride;
  return (void *)(array + FieldCount);
}

void _destroy(void *array) {
  u64 *header      = (u64 *)array - FieldCount;
  u64  header_size = FieldCount * sizeof(u64);
  u64  total_size  = header_size + header[Capacity] * header[Stride];
  hn::mem::free(header, total_size, hn::mem::TagDArray);
  array = nullptr;
}

u64 _field_get(void *array, u64 field) {
  u64 *header = (u64 *)array - FieldCount;
  return header[field];
}

void _field_set(void *array, u64 field, u64 value) {
  u64 *header   = (u64 *)array - FieldCount;
  header[field] = value;
}

void *_resize(void *array) {
  u64   length = darray_length(array);
  u64   stride = darray_stride(array);
  void *temp   = _create((DARRAY_RESIZE_FACTOR * darray_capacity(array)), stride);
  hn::mem::copy(temp, array, length * stride);
  _field_set(temp, Length, length);
  _destroy(array);
  return temp;
}

void *_push(void *array, const void *value_ptr) {
  u64 length = darray_length(array);
  u64 stride = darray_stride(array);
  if (length >= darray_capacity(array)) {
    array = _resize(array);
  }

  u64 addr = (u64)array;
  addr += (length * stride);
  hn::mem::copy((void *)addr, value_ptr, stride);
  _field_set(array, Length, length + 1);
  return array;
}

void _pop(void *array, void *dest) {
  u64 length = darray_length(array);
  u64 stride = darray_stride(array);
  u64 addr   = (u64)array;
  addr += ((length - 1) * stride);
  hn::mem::copy(dest, (void *)addr, stride);
  _field_set(array, Length, length - 1);
}

void *_pop_at(void *array, u64 index, void *dest) {
  u64 length = darray_length(array);
  u64 stride = darray_stride(array);
  if (index >= length) {
    HN_error("Index outside the array bounds: length: %llu, index: %llu.", length, index);
    return array;
  }

  u64 addr = (u64)array;
  hn::mem::copy(dest, (void *)(addr + (index * stride)), stride);

  // If not on the last element, snip out the entry and copy the rest inward.
  if (index != length - 1) {
    hn::mem::copy((void *)(addr + (index * stride)), (void *)(addr + ((index + 1) * stride)),
                  stride * (length - index));
  }

  _field_set(array, Length, length - 1);
  return array;
}

void *_insert_at(void *array, u64 index, void *value_ptr) {
  u64 length = darray_length(array);
  u64 stride = darray_stride(array);
  if (index >= length) {
    HN_error("Index outside the array bounds: length: %llu, index: %llu", length, index);
    return array;
  }
  if (length >= darray_capacity(array)) {
    array = _resize(array);
  }

  u64 addr = (u64)array;

  // If not on the last element, copy the rest outward.
  if (index != length - 1) {
    hn::mem::copy((void *)(addr + ((index + 1) * stride)), (void *)(addr + (index * stride)),
                  stride * (length - index));
  }

  // Set the value at the index.
  hn::mem::copy((void *)(addr + (index * stride)), value_ptr, stride);

  _field_set(array, Length, length + 1);
  return array;
}

} // namespace hn::darray
