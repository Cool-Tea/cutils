# cutils

A collection of header-only C utility libraries designed to be lightweight, pragmatic, and easy to use.

## Features

*   **Macro-only**: Most utilities are implemented in macros, enabling type information capture in place.
*   **Typedefs**: Rust-like primitive type definitions (`i32`, `u64`, `usize`, etc.).
*   **Logging**: Simple, colorful logging macros with severity levels.
*   **Dynamic Array**: Type-safe dynamic array implementation using macros.
*   **String View**: Non-owning string reference utilities for efficient string parsing.
*   **String Builder**: Efficient string construction and manipulation.
*   **Hash Table**: Open-addressing hash table with generic key support and specialized string-key helpers.

## Usage

Simply copy the relevant header files from the `src/` directory into your project's include path.

### 1. Typedefs

Use `typedef.h` for succinct, fixed-width integer types similar to Rust.

```c
#include "typedef.h"

i32 main() {
    u64 big_number = 1234567890;
    f32 float_val = 3.14f;
    usize index = 0;
    return 0;
}
```

### 2. Logging

Use `log.h` for leveled logging with color support.

```c
// Optional: Define log level (default is ALL)
#define LOG_LEVEL DEBUG 
#include "log.h"

void some_func() {
    // Levels: DEBUG, INFO, WARNING, ERROR
    LOG(INFO, "Application started");
    LOG(DEBUG, "Variable x = %d", 42);
}
```

### 3. Dynamic Array

Type-safe dynamic arrays with `dynarr.h`, inspired by `nob.h`.

```c
#include "dynarr.h"
#include <stdio.h>

// Define a dynamic array type for int
da_define(int, int_arr_t);

int main() {
    int_arr_t arr = {0}; // Initialize with zero

    // Append items
    da_append(&arr, 10);
    da_append(&arr, 20);

    // Iteration
    da_foreach(it, &arr) {
        printf("%d\n", *it);
    }

    // Direct access
    if (arr.size > 0) {
        printf("First: %d, Last: %d\n", da_first(&arr), da_last(&arr));
    }

    da_free(&arr);
    return 0;
}
```

### 4. String View
Efficient non-owning string parsing with `strview.h`.

```c
#include "strview.h"
#include <stdio.h>

int main() {
    strview_t sv = sv_fromstr("Hello World");

    // Check prefix
    if (sv_startwithstr(sv, "Hello")) {
        printf("Starts with Hello\n");
    }

    // Slicing (does not allocate)
    strview_t sub = sv_subview(sv, 6, 5); 
    
    // Printing using convenience macros
    printf("Substring: " sv_printfmt "\n", sv_printarg(sub)); // "World"

    // Tokenization
    strview_t token;
    strview_t rem = sv;
    while ((token = sv_tokstr(&rem, " ")).data != NULL) {
         // Process token...
    }

    return 0;
}
```

### 5. String Builder
Dynamic string construction with `strbdr.h`.

```c
#include "strbdr.h"
#include <stdio.h>

int main() {
    strbdr_t sb = {0};

    sb_append(&sb, 'H');
    sb_concat(&sb, "ello");
    sb_concatf(&sb, " %s", "World"); // Formatted append

    printf("Result: %s\n", sb.data);

    // Convert to view
    strview_t sv = sb_toview(&sb);

    sb_free(&sb);
    return 0;
}
```

### 6. Hash Table
Generic hash table macros in `hstable.h`. It supports both generic keys and specialized string keys.

#### Generic Map (Key -> Value)
```c
#include "hstable.h"
#include <stdio.h>

// Define map: int (key) -> int (value)
ht_define(int, int, map_t);

int main() {
    map_t map = {0};
    int key = 1, val = 100;

    // Insert (key is passed by pointer)
    ht_put(&map, &key, val);
    
    // Check and Get
    if (ht_has(&map, &key)) {
        int v = ht_get(&map, &key);
        printf("Value: %d\n", v);
    }

    ht_free(&map);
    return 0;
}
```

#### String Map (String Key -> Value)
Use `sht_*` macros for string keys. This implementation handles string duplication for keys automatically.

```c
#include "hstable.h"

// Define map: string (key) -> int (value)
sht_define(int, str_map_t);

int main() {
    str_map_t map = {0};

    sht_put(&map, "apple", 10);
    sht_put(&map, "banana", 20);

    // Iteration
    sht_foreach(key, val, &map) {
        printf("%s: %d\n", key, *val);
    }

    sht_free(&map); // Automatically frees duplicated key strings
    return 0;
}
```
