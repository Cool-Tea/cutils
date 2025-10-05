#ifndef DYNARR_H
#define DYNARR_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef DYNARR_DISASSERT
#define da_assert(cond)
#else
#include <assert.h>
#define da_assert(cond) assert(cond)
#endif

#define da_define(type, name) \
  typedef struct name {       \
    uint32_t size;            \
    uint32_t capacity;        \
    type* data;               \
  } name

#define da_grow_capacity(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define da_free(da) \
  reallocate((da)->data, (da)->capacity * sizeof((da)->data[0]), 0)

#define da_reserve(da, rsvsz)                                            \
  do {                                                                   \
    if ((rsvsz) > (da)->capacity) {                                      \
      uint32_t newsz = (da)->capacity;                                   \
      while ((rsvsz) > newsz) {                                          \
        newsz = da_grow_capacity(newsz);                                 \
      }                                                                  \
      (da)->data =                                                       \
          reallocate((da)->data, (da)->capacity * sizeof((da)->data[0]), \
                     newsz * sizeof((da)->data[0]));                     \
      da_assert((da)->data && "Failed to allocate memory");              \
      (da)->capacity = newsz;                                            \
    }                                                                    \
  } while (0)

#define da_append(da, item)            \
  do {                                 \
    da_reserve((da), (da)->size + 1);  \
    (da)->data[(da)->size++] = (item); \
  } while (0)

#define da_append_many(da, items, itemsz)     \
  do {                                        \
    da_reserve((da), (da)->size + (itemsz));  \
    memcpy((da)->data + (da)->size, (items),  \
           (itemsz) * sizeof((da)->data[0])); \
    (da)->size += (itemsz);                   \
  } while (0)

#define da_resize(da, newsz)   \
  do {                         \
    da_reserve((da), (newsz)); \
    (da)->size = (newsz);      \
  } while (0)

#define da_first(da) ((da)->data[da_assert((da)->size), 0])

#define da_last(da) ((da)->data[da_assert((da)->size), (da)->size - 1])

#define da_remove_unordered(da, i) \
  do {                             \
    da_assert((i) < (da)->size);   \
    (da)->data[i] = da_last(da);   \
    (da)->size--;                  \
  } while (0)

#define da_foreach(type, it, da) \
  for (type* it = (da)->data; it < (da)->data + (da)->size; it++)

#ifdef DYNARR_NOIMPL
void* reallocate(void* ptr, uint32_t old_size, uint32_t new_size);
#else
static inline void* reallocate(void* ptr, uint32_t old_size,
                               uint32_t new_size) {
  if (new_size == 0) {
    free(ptr);
    return NULL;
  } else if (old_size == 0) {
    return malloc(new_size);
  } else {
    return realloc(ptr, new_size);
  }
}
#endif

#endif
