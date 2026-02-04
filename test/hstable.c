#include <log.h>
#include <hstable.h>

ht_define(int, int, sprs_t);

void ht_test() {
  sprs_t ht = {0};

  // Insert key-value pairs
  for (int i = 0; i < 10; i++) {
    ht_put(&ht, &i, i * 10);
  }

  // Retrieve and verify values
  for (int i = 0; i < 10; i++) {
    int val = ht_get(&ht, &i);
    ht_assert(val == i * 10);
    LOG(INFO, "Key: %d, Value: %d", i, val);
  }

  // Remove a key and verify
  ht_remove(&ht, &(int){5});
  ht_assert(!ht_has(&ht, &(int){5}));
  LOG(INFO, "Removed key: %d", 5);

  // Retrieve and verify values
  ht_foreach(k, v, &ht) { LOG(INFO, "Key: %d, Value: %d", *k, *v); }

  ht_free(&ht);

  LOG(INFO, "ht_test completed successfully.");
}

sht_define(int, shtbl_t);

void sht_test() {
  shtbl_t sht = {0};

  const char* keys[] = {"apple", "banana", "cherry", "date", "elderberry"};
  int values[] = {1, 2, 3, 4, 5};
  const int num_items = sizeof(keys) / sizeof(keys[0]);
  // Insert key-value pairs
  for (int i = 0; i < num_items; i++) {
    sht_put(&sht, keys[i], values[i]);
  }

  // Retrieve and verify values
  for (int i = 0; i < num_items; i++) {
    int val = sht_get(&sht, keys[i]);
    ht_assert(val == values[i]);
    LOG(INFO, "Key: %s, Value: %d", keys[i], val);
  }

  // Remove a key and verify
  sht_remove(&sht, keys[2]);  // Remove "cherry"
  ht_assert(!sht_has(&sht, keys[2]));
  LOG(INFO, "Removed key: %s", keys[2]);

  // Remove a literal key and verify
  sht_remove(&sht, "banana");
  LOG(INFO, "Removed key: %s", "banana");

  // Retrieve and verify values
  sht_foreach(k, v, &sht) { LOG(INFO, "Key: %s, Value: %d", k, *v); }

  sht_free(&sht);

  LOG(INFO, "sht_test completed successfully.");
}

int main() {
  ht_test();
  sht_test();
  return 0;
}