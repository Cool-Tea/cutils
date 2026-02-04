#include <stdio.h>

#include <typedef.h>
#include <dynarr.h>

// typedef struct {
//   u32 size;
//   u32 capacity;
//   i32 data;
// } veci32;
da_define(i32, veci32);

int main() {
  veci32 da = {};
  da_append(&da, 42);
  da_append(&da, 0x20);
  da_append(&da, 0x30);
  da_foreach(i, &da) { printf("%d ", *i); }
  printf("\n");
  da_remove_unordered(&da, 0);
  da_foreach(i, &da) { printf("%d ", *i); }
  printf("\n");

  da_free(&da);
  return 0;
}