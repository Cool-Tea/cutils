#ifndef STRBDR_H
#define STRBDR_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef STRBDR_DISASSERT
#define sb_assert(cond)
#else
#include <assert.h>
#define sb_assert(cond) assert(cond)
#endif

typedef struct string_builder {
  uint32_t size;
  uint32_t capacity;
  char* data;
} strbdr_t;

#define sb_grow_capacity(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define sb_realloc(ptr, old_size, new_size) \
  ({                                        \
    void* _ptr = NULL;                      \
    if ((new_size) == 0) {                  \
      free(ptr);                            \
      _ptr = NULL;                          \
    } else if ((old_size) == 0) {           \
      _ptr = malloc(new_size);              \
    } else {                                \
      _ptr = realloc((ptr), (new_size));    \
    }                                       \
    _ptr;                                   \
  })

#define sb_free(sb) sb_realloc((sb)->data, (sb)->capacity, 0)

#define sb_reserve(sb, rsvsz)                                      \
  do {                                                             \
    if ((rsvsz) > (sb)->capacity) {                                \
      uint32_t _newsz = (sb)->capacity;                            \
      while ((rsvsz) > _newsz) {                                   \
        _newsz = sb_grow_capacity(_newsz);                         \
      }                                                            \
      (sb)->data = sb_realloc((sb)->data, (sb)->capacity, _newsz); \
      sb_assert((sb)->data && "Failed to allocate memory");        \
      (sb)->capacity = _newsz;                                     \
    }                                                              \
  } while (0)

#define sb_append(sb, ch)             \
  do {                                \
    sb_reserve((sb), (sb)->size + 2); \
    (sb)->data[(sb)->size++] = (ch);  \
    (sb)->data[(sb)->size] = 0;       \
  } while (0)

#define sb_concat(sb, str)                           \
  do {                                               \
    uint32_t len = strlen(str);                      \
    sb_reserve((sb), (sb)->size + len + 1);          \
    memcpy((sb)->data + (sb)->size, (str), len + 1); \
    (sb)->size += len;                               \
  } while (0)

#define sb_nconcat(sb, str, len)                       \
  do {                                                 \
    sb_reserve((sb), (sb)->size + (len) + 1);          \
    memcpy((sb)->data + (sb)->size, (str), (len) + 1); \
    (sb)->size += (len);                               \
  } while (0)

#define sb_concatf(sb, fmt, ...)                                      \
  do {                                                                \
    uint32_t len = snprintf(NULL, 0, (fmt), ##__VA_ARGS__);           \
    sb_reserve((sb), (sb)->size + len + 1);                           \
    snprintf((sb)->data + (sb)->size, len + 1, (fmt), ##__VA_ARGS__); \
    (sb)->size += len;                                                \
  } while (0)

#define sb_resize(sb, newsz)       \
  do {                             \
    sb_reserve((sb), (newsz) + 1); \
    (sb)->size = (newsz);          \
    (sb)->data[(sb)->size] = 0;    \
  } while (0)

#define sb_clear(sb) sb_resize((sb), 0)

#define sb_insert(sb, i, str, len)                                         \
  do {                                                                     \
    sb_reserve((sb), (sb)->size + (len) + 1);                              \
    memmove((sb)->data + (i) + (len), (sb)->data + (i), (sb)->size - (i)); \
    memcpy((sb)->data + (i), (str), (len));                                \
    (sb)->size += (len);                                                   \
  } while (0)

#define sb_first(sb) ((sb)->data[sb_assert((sb)->size), 0])

#define sb_last(sb) ((sb)->data[sb_assert((sb)->size), (sb)->size - 1])

#define sb_printfmt "%.*s"

#define sb_printarg(sb) (sb)->size, (sb)->data

#ifdef STRVIEW_H
#define sb_toview(sb) ((strview_t){.size = (sb)->size, .data = (sb)->data})
#define sv_frombdr(sb) ((strview_t){.size = (sb)->size, .data = (sb)->data})
#endif

#endif  // STRBDR