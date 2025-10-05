#define LOG_LEVEL WARNING
#include <log.h>
#include <typedef.h>
#include <dynarr.h>

da_define(usize, vecu);

int main() {
  LOG(DEBUG, "Start logging ...");
  vecu v = {0};

  da_append(&v, 42);
  da_append(&v, 0x20);
  da_append(&v, 0x30);
  da_foreach(usize, i, &v) { LOG(INFO, "Item %lu", *i); }
  da_remove_unordered(&v, 0);
  da_foreach(usize, i, &v) { LOG(WARNING, "Item %lu", *i); }

  LOG(ERROR, "End logging and free stuffs ...");
  da_free(&v);
  return 0;
}