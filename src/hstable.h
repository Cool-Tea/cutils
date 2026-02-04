#ifndef HSTABLE_H
#define HSTABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**************************
 * Hash Table Util Macros *
 **************************/

#ifdef HSTABLE_DISASSERT
#define ht_assert(cond)
#else
#include <assert.h>
#define ht_assert(cond) assert(cond)
#endif

#define ht_empty 0x0
#define ht_used 0x1
#define ht_tombstone 0x2

#define ht_load_threshold 0.75
#define ht_grow_capacity(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define ht_realloc(ptr, old_size, new_size)             \
  ({                                                    \
    void* _ptr = NULL;                                  \
    if ((new_size) == 0) {                              \
      free(ptr);                                        \
      _ptr = NULL;                                      \
    } else if ((old_size) == 0) {                       \
      _ptr = malloc((new_size) * sizeof(*ptr));         \
    } else {                                            \
      _ptr = realloc((ptr), (new_size) * sizeof(*ptr)); \
    }                                                   \
    _ptr;                                               \
  })

// fnv1a hash function
#define ht_hash(key, keylen)                     \
  ({                                             \
    uint32_t _hs = 2166136261u;                  \
    for (uint32_t _i = 0; _i < (keylen); _i++) { \
      _hs ^= *((uint8_t*)(key) + _i);            \
      _hs *= 16777619;                           \
    }                                            \
    _hs;                                         \
  })

/*****************************
 * General Hash Table Macros *
 *****************************/

#define ht_define(ktype, vtype, name) \
  typedef struct name {               \
    uint32_t size;                    \
    uint32_t capacity;                \
    uint8_t* flags;                   \
    ktype* keys;                      \
    vtype* vals;                      \
  } name

#define ht_put_no_rsv(ht, key, value)                                         \
  do {                                                                        \
    for (uint32_t _i = ht_hash((key), sizeof(*(ht)->keys)) % (ht)->capacity;; \
         _i = (_i + 1) % (ht)->capacity) {                                    \
      if ((ht)->flags[_i] == ht_empty || (ht)->flags[_i] == ht_tombstone) {   \
        (ht)->keys[_i] = *(key);                                              \
        (ht)->vals[_i] = (value);                                             \
        (ht)->flags[_i] = ht_used;                                            \
        (ht)->size++;                                                         \
        break;                                                                \
      }                                                                       \
    }                                                                         \
  } while (0)

#define ht_reserve(ht, rsvsz)                                            \
  do {                                                                   \
    if ((rsvsz) > (ht)->capacity ||                                      \
        (ht)->capacity * ht_load_threshold < (rsvsz)) {                  \
      uint32_t _newsz = (ht)->capacity;                                  \
      while ((rsvsz) > _newsz || _newsz * ht_load_threshold < (rsvsz)) { \
        _newsz = ht_grow_capacity(_newsz);                               \
      }                                                                  \
      typeof(*(ht)) _oldht = *(ht);                                      \
      (ht)->flags = ht_realloc((ht)->flags, 0, _newsz);                  \
      (ht)->keys = ht_realloc((ht)->keys, 0, _newsz);                    \
      (ht)->vals = ht_realloc((ht)->vals, 0, _newsz);                    \
      (ht)->capacity = _newsz;                                           \
      ht_assert((ht)->flags && (ht)->keys && (ht)->vals &&               \
                "Failed to allocate memory");                            \
      memset((ht)->flags, ht_empty, _newsz);                             \
      /* Use __i to prevent name conflict in ht_put_no_rsv */            \
      for (uint32_t __i = 0; __i < _oldht.capacity; __i++) {             \
        if (_oldht.flags[__i] == ht_used) {                              \
          ht_put_no_rsv((ht), _oldht.keys + __i, _oldht.vals[__i]);      \
        }                                                                \
      }                                                                  \
      ht_free(&_oldht);                                                  \
    }                                                                    \
  } while (0)

#define ht_has(ht, key)                                                     \
  ({                                                                        \
    uint8_t _found = 0;                                                     \
    for (int32_t _i = ht_hash((key), sizeof(*(ht)->keys)) % (ht)->capacity; \
         (ht)->flags[_i] != ht_empty; _i = (_i + 1) % (ht)->capacity) {     \
      if ((ht)->flags[_i] == ht_used &&                                     \
          memcmp((ht)->keys + _i, (key), sizeof(*(ht)->keys)) == 0) {       \
        _found = 1;                                                         \
        break;                                                              \
      }                                                                     \
    }                                                                       \
    !!_found;                                                               \
  })

#define ht_get(ht, key)                                                     \
  ({                                                                        \
    int32_t _idx = -1;                                                      \
    for (int32_t _i = ht_hash((key), sizeof(*(ht)->keys)) % (ht)->capacity; \
         (ht)->flags[_i] != ht_empty; _i = (_i + 1) % (ht)->capacity) {     \
      if ((ht)->flags[_i] == ht_used &&                                     \
          memcmp((ht)->keys + _i, (key), sizeof(*(ht)->keys)) == 0) {       \
        _idx = _i;                                                          \
        break;                                                              \
      }                                                                     \
    }                                                                       \
    ht_assert(_idx != -1 && "Key not found");                               \
    (ht)->vals[_idx];                                                       \
  })

#define ht_put(ht, key, value)      \
  do {                              \
    ht_reserve(ht, (ht)->size + 1); \
    ht_put_no_rsv(ht, key, value);  \
  } while (0)

#define ht_remove(ht, key)                                                   \
  do {                                                                       \
    for (uint32_t _i = ht_hash((key), sizeof(*(ht)->keys)) % (ht)->capacity; \
         (ht)->flags[_i] != ht_empty; _i = (_i + 1) % (ht)->capacity) {      \
      if ((ht)->flags[_i] == ht_used &&                                      \
          memcmp((ht)->keys + _i, (key), sizeof(*(ht)->keys)) == 0) {        \
        (ht)->flags[_i] = ht_tombstone;                                      \
        (ht)->size--;                                                        \
        break;                                                               \
      }                                                                      \
    }                                                                        \
  } while (0)

#define ht_free(ht)                                           \
  do {                                                        \
    (ht)->flags = ht_realloc((ht)->flags, (ht)->capacity, 0); \
    (ht)->keys = ht_realloc((ht)->keys, (ht)->capacity, 0);   \
    (ht)->vals = ht_realloc((ht)->vals, (ht)->capacity, 0);   \
  } while (0)

#define ht_foreach(key, val, ht)                                             \
  for (uint32_t _i = 0; _i < (ht)->capacity; _i++)                           \
    if ((ht)->flags[_i] == ht_used)                                          \
      for (typeof((ht)->keys) key = &(ht)->keys[_i], _k = key; _k != NULL;   \
           _k = NULL)                                                        \
        for (typeof((ht)->vals) val = &(ht)->vals[_i], _v = val; _v != NULL; \
             _v = NULL)

/****************************
 * String Hash Table Macros *
 ****************************/

#define sht_define(vtype, name) \
  typedef struct name {         \
    uint32_t size;              \
    uint32_t capacity;          \
    uint8_t* flags;             \
    char** keys;                \
    vtype* vals;                \
  } name

#define sht_put_no_dup(sht, key, value)                                       \
  do {                                                                        \
    for (uint32_t _i = ht_hash((key), strlen(key)) % (sht)->capacity;;        \
         _i = (_i + 1) % (sht)->capacity) {                                   \
      if ((sht)->flags[_i] == ht_empty || (sht)->flags[_i] == ht_tombstone) { \
        (sht)->keys[_i] = (key);                                              \
        (sht)->vals[_i] = (value);                                            \
        (sht)->flags[_i] = ht_used;                                           \
        (sht)->size++;                                                        \
        break;                                                                \
      }                                                                       \
    }                                                                         \
  } while (0)

#define sht_reserve(sht, rsvsz)                                              \
  do {                                                                       \
    if ((rsvsz) > (sht)->capacity ||                                         \
        (sht)->capacity * ht_load_threshold < (sht)->size) {                 \
      uint32_t _newsz = (sht)->capacity;                                     \
      while ((rsvsz) > _newsz || _newsz * ht_load_threshold < (sht)->size) { \
        _newsz = ht_grow_capacity(_newsz);                                   \
      }                                                                      \
      typeof(*(sht)) _oldsht = *(sht);                                       \
      (sht)->flags = ht_realloc((sht)->flags, 0, _newsz);                    \
      (sht)->keys = ht_realloc((sht)->keys, 0, _newsz);                      \
      (sht)->vals = ht_realloc((sht)->vals, 0, _newsz);                      \
      (sht)->capacity = _newsz;                                              \
      ht_assert((sht)->flags && (sht)->keys && (sht)->vals &&                \
                "Failed to allocate memory");                                \
      memset((sht)->flags, ht_empty, _newsz);                                \
      for (uint32_t __i = 0; __i < _oldsht.capacity; __i++) {                \
        if (_oldsht.flags[__i] == ht_used) {                                 \
          /* Put but don't duplicate keys */                                 \
          sht_put_no_dup((sht), _oldsht.keys[__i], _oldsht.vals[__i]);       \
        }                                                                    \
      }                                                                      \
      /* Here we use ht_free to prevent freeing key str */                   \
      ht_free(&_oldsht);                                                     \
    }                                                                        \
  } while (0)

#define sht_has(sht, key) sht_nhas(sht, key, strlen(key))

#define sht_nhas(sht, key, keylen)                                        \
  ({                                                                      \
    uint8_t _found = 0;                                                   \
    for (int32_t _i = ht_hash((key), (keylen)) % (sht)->capacity;         \
         (sht)->flags[_i] != ht_empty; _i = (_i + 1) % (sht)->capacity) { \
      if ((sht)->flags[_i] == ht_used &&                                  \
          strncmp((sht)->keys[_i], (key), (keylen)) == 0) {               \
        _found = 1;                                                       \
        break;                                                            \
      }                                                                   \
    }                                                                     \
    !!_found;                                                             \
  })

#define sht_get(sht, key) sht_nget(sht, key, strlen(key))

#define sht_nget(sht, key, keylen)                                        \
  ({                                                                      \
    int32_t _idx = -1;                                                    \
    for (int32_t _i = ht_hash((key), (keylen)) % (sht)->capacity;         \
         (sht)->flags[_i] != ht_empty; _i = (_i + 1) % (sht)->capacity) { \
      if ((sht)->flags[_i] == ht_used &&                                  \
          strncmp((sht)->keys[_i], (key), (keylen)) == 0) {               \
        _idx = _i;                                                        \
        break;                                                            \
      }                                                                   \
    }                                                                     \
    ht_assert(_idx != -1 && "Key not found");                             \
    (sht)->vals[_idx];                                                    \
  })

#define sht_put(sht, key, value) sht_nput(sht, key, strlen(key), value)

#define sht_nput(sht, key, keylen, value)                                     \
  do {                                                                        \
    sht_reserve(sht, (sht)->size + 1);                                        \
    for (uint32_t _i = ht_hash((key), (keylen)) % (sht)->capacity;;           \
         _i = (_i + 1) % (sht)->capacity) {                                   \
      if ((sht)->flags[_i] == ht_empty || (sht)->flags[_i] == ht_tombstone) { \
        (sht)->keys[_i] = strndup((key), (keylen));                           \
        (sht)->vals[_i] = (value);                                            \
        (sht)->flags[_i] = ht_used;                                           \
        (sht)->size++;                                                        \
        break;                                                                \
      }                                                                       \
    }                                                                         \
  } while (0)

#define sht_remove(sht, key) sht_nremove(sht, key, strlen(key))

#define sht_nremove(sht, key, keylen)                                     \
  do {                                                                    \
    for (uint32_t _i = ht_hash((key), (keylen)) % (sht)->capacity;        \
         (sht)->flags[_i] != ht_empty; _i = (_i + 1) % (sht)->capacity) { \
      if ((sht)->flags[_i] == ht_used &&                                  \
          strncmp((sht)->keys[_i], (key), (keylen)) == 0) {               \
        (sht)->keys[_i] = ht_realloc((sht)->keys[_i], 0, 0);              \
        (sht)->flags[_i] = ht_tombstone;                                  \
        (sht)->size--;                                                    \
        break;                                                            \
      }                                                                   \
    }                                                                     \
  } while (0)

#define sht_free(sht)                                            \
  do {                                                           \
    for (uint32_t _i = 0; _i < (sht)->capacity; _i++) {          \
      if ((sht)->flags[_i] == ht_used) {                         \
        (sht)->keys[_i] = ht_realloc((sht)->keys[_i], 0, 0);     \
      }                                                          \
    }                                                            \
    (sht)->flags = ht_realloc((sht)->flags, (sht)->capacity, 0); \
    (sht)->keys = ht_realloc((sht)->keys, (sht)->capacity, 0);   \
    (sht)->vals = ht_realloc((sht)->vals, (sht)->capacity, 0);   \
  } while (0)

#define sht_foreach(key, val, sht)                                             \
  for (uint32_t _i = 0; _i < (sht)->capacity; _i++)                            \
    if ((sht)->flags[_i] == ht_used)                                           \
      for (const char *key = (sht)->keys[_i], *_k = key; _k != NULL;           \
           _k = NULL)                                                          \
        for (typeof((sht)->vals) val = &(sht)->vals[_i], _v = val; _v != NULL; \
             _v = NULL)

#endif  // HSTABLE_H