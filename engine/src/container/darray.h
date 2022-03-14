#pragma once

#include "defines.h"

/**
 * Memory layout:
 * - u64 capacity: Number elements that can be held.
 * - u64 length: Number of elements currently contained.
 * - u64 stride: Size of each element in bytes.
 * - void* elements.
 */

namespace hn::darray {

enum { Capacity, Length, Stride, FieldCount };

/**
 * Creates a new darray of the given length and stride.
 * Note that this performs a dynamic memory allocation.
 * @note Avoid using this directly; use the darray_create macro instead.
 * @param length The default number of elements in the array.
 * @param stride The size of each array element.
 * @returns A pointer representing the block of memory containing the array.
 */
void *_create(u64 length, u64 stride);
void  _destroy(void *array);

/**
 * Returns the value for the given field.
 * @note This is an internal feature, use public macros instead such as darray_length.
 * @param array The array to retrieve the field for.
 * @param field The index of the field to be retrieved.
 * @returns The value for the given field.
 */
u64  _field_get(void *array, u64 field);
void _field_set(void *array, u64 field, u64 value);

/**
 * Resizes the given array using internal resizing amounts. Causes a new allocation.
 * @note This is an internal implementation detail and should not be called directly.
 * @param array The array to be resized.
 * @returns A pointer to the resized array block.
 */
void *_resize(void *array);

/**
 * Pushes a new entry to the given array. Resizes if necessary.
 * @note Avoid using this directly; call the darray_push macro instead.
 * @param array The array to be pushed to.
 * @param value_ptr A pointer to the value to be pushed. A copy of this value is taken.
 * @returns A pointer to the array block.
 */
void *_push(void *array, const void *value_ptr);

/**
 * Pops an entry out of the array and places it into `dst`.
 * @note Avoid using this directly; call the darray_pop macro instead.
 * @param array The array to pop from.
 * @param dst A pointer to hold the popped value.
 */
void _pop(void *array, void *dst);

/**
 * Pops an entry out of the array at the given index and places it into `dst`.
 * Brings in all entries after the popped index in by one.
 * @note Avoid using this directly; call the darray_pop_at macro instead.
 * @param array The array to pop from.
 * @param index The index to pop from.
 * @param dst A pointer to hold the popped value.
 * @returns The array block.
 */
void *_pop_at(void *array, u64 index, void *dst);

/**
 * Inserts a copy of the given value into the supplied array at the given index.
 * Triggers an array resize if required.
 * @note Avoid using this directly; call the darray_insert_at macro instead.
 * @param array The array to insert into.
 * @param index The index to insert at.
 * @param value_ptr A pointer holding the value to be inserted.
 * @returns The array block.
 */
void *_insert_at(void *array, u64 index, void *value_ptr);

} // namespace hn::darray

// The default darray capacity.
#define DARRAY_DEFAULT_CAPACITY 1

// The default resize factor (doubles on resize).
#define DARRAY_RESIZE_FACTOR 2

/**
 * Creates a new darray of the given type with the default capacity.
 * Performs a dynamic memory allocation.
 * @param type The type to be used to create the darray.
 * @returns A pointer to the array's memory block.
 */
#define darray_create(type) hn::darray::_create(DARRAY_DEFAULT_CAPACITY, sizeof(type))

/**
 * Creates a new darray of the given type with the provided capacity.
 * Performs a dynamic memory allocation.
 * @param type The type to be used to create the darray.
 * @param capacity The number of elements the darray can initially hold (can be resized).
 * @returns A pointer to the array's memory block.
 */
#define darray_reserve(type, capacity) hn::darray::_create(capacity, sizeof(type))

/**
 * Destroys the provided array, freeing any memory allocated by it.
 * @param array The array to be destroyed.
 */
#define darray_destroy(array) hn::darray::_destroy(array);

/**
 * Pushes a new entry to the given array. Resizes if necessary.
 * @param array The array to be pushed to.
 * @param value The value to be pushed. A copy of this value is taken.
 * @returns A pointer to the array block.
 */
#define darray_push(array, value)                                                                  \
  {                                                                                                \
    typeof(value) temp = value;                                                                    \
    array              = (typeof(array))hn::darray::_push(array, &temp);                           \
  }

/**
 * Pops an entry out of the array and places it into `dst`.
 * @param array The array to pop from.
 * @param dst A pointer to hold the popped value.
 */
#define darray_pop(array, dst) hn::darray::_pop(array, dst)

/**
 * Inserts a copy of the given value into the supplied array at the given index.
 * Triggers an array resize if required.
 * @param array The array to insert into.
 * @param index The index to insert at.
 * @param value_ptr A pointer holding the value to be inserted.
 * @returns The array block.
 */
#define darray_insert_at(array, index, value)                                                      \
  {                                                                                                \
    typeof(value) temp = value;                                                                    \
    array              = (typeof(array))hn::darray::_insert_at(array, index, &temp);               \
  }

/**
 * Pops an entry out of the array at the given index and places it into `dst`.
 * Brings in all entries after the popped index in by one.
 * @param array The array to pop from.
 * @param index The index to pop from.
 * @param dst A pointer to hold the popped value.
 * @returns The array block.
 */
#define darray_pop_at(array, index, dst) hn::darray::_pop_at(array, index, dst)

/**
 * Clears all entries from the array. Does not release any internally-allocated memory.
 * @param array The array to be cleared.
 */
#define darray_clear(array) hn::darray::_field_set(array, hn::darray::Length, 0)

/**
 * Gets the given array's capacity.
 * @param array The array whose capacity to retrieve.
 * @returns The capacity of the given array.
 */
#define darray_capacity(array) hn::darray::_field_get(array, hn::darray::Capacity)

/**
 * Gets the length (number of elements) in the given array.
 * @param array The array to obtain the length of.
 * @returns The length of the given array.
 */
#define darray_length(array) hn::darray::_field_get(array, hn::darray::Length)

/**
 * Gets the stride (element size) of the given array.
 * @param array The array to obtain the stride of.
 * @returns The stride of the given array.
 */
#define darray_stride(array) hn::darray::_field_get(array, hn::darray::Stride)

/**
 * Sets the length of the given array. This ensures the array has the required capacity to be able
 * to set entries directly, for instance. Can trigger an internal reallocation.
 * @param array The array to set the length of.
 * @param value The length to set the array to.
 */
#define darray_length_set(array, value) hn::darray::_field_set(array, hn::darray::Length, value)
