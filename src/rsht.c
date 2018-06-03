/*
 * src/rsht.c
 *
 * rsht - really simple hash table
 * Copyright (c) 2018 Robert Stolarz
 *
 * rsht is available free of charge under the terms of the MIT
 * License. You are free to redistribute and/or modify it under those
 * terms. It is distributed in the hopes that it will be useful, but
 * WITHOUT ANY WARRANTY. See the LICENSE file for more details.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../rsht.h"

#ifndef RSHT_CAPACITY_SCALE
#define RSHT_CAPACITY_SCALE 1.5
#endif

// dan bernstein's djb2 from comp.lang.c, dumbed down
static unsigned long djb2(const unsigned char *str) {
  unsigned long hash = 5381;
  int c;

  while (c = *str++)
    hash = hash * 33 + c;

  return hash;
}

// bool: did it work?
static bool setcapacity(rsht_ht *ht, const size_t capacity) {
  rsht_entry *items = realloc(ht->items, capacity * sizeof(rsht_entry));
  if (!items) {
    return false;
  }
  ht->capacity = capacity;
  ht->items = items;
  return true;
}

// hint: only rsht_create should call this, and only once.
// bool: did it work?
static bool makebuckets(rsht_ht *ht, const size_t num_buckets) {
  size_t *buckets = calloc(num_buckets, sizeof(size_t));
  if (!buckets) {
    return false;
  }
  ht->num_buckets = num_buckets;
  ht->buckets = buckets;
  return true;
}

rsht_ht *rsht_create(rsht_ht *ht, const size_t num_buckets, const size_t initial_capacity) {
  ht->items = NULL;
  ht->num_slots_used = 0;
  if (!makebuckets(ht, num_buckets))
    goto fail;
  if (!setcapacity(ht, initial_capacity))
    goto unmake_buckets;
  return ht;

unmake_buckets:
  free(ht->buckets);
fail:
  return NULL;
}

rsht_entry *rsht_get_hash(const rsht_ht *ht, const char *key, const unsigned long hash) {
  rsht_entry *entry = NULL;

  /* this part of the code asks three questions:
   * 1) do we know about the item?
   * 2) if we do, does the suggested one match the key we're looking for?
   * 3) if not, is there one somewhere in the bucket set?
   *
   * #1 acts as a sort of Bloom-like optimization to keep this LPMult fast.
   * to delete items, write caller code that invalidates the values.
   * this will not shrink the table; you must also periodically rebuild the hash table yourself.
   *
   * near offsets, you will see `- 1` or `+ 1`; this is for testing for the
   * lack of an item. if an offset is 0, we've never seen the item
   */

  // first, try looking up the hash
  const size_t offset = ht->buckets[hash % ht->num_buckets];
  if (offset)
    entry = &(ht->items[offset - 1]);

  if (entry) { // if we found something
    // make sure it's the right thing
    if (0 != strcmp(entry->key, key)) { // if it's the wrong thing
      size_t i = offset; // find the right thing
      while (i < ht->num_slots_used && // # items?
          0 != strcmp(ht->items[i].key, key)) { // matching item?
        i++;
      }

      if (i == ht->num_slots_used) // if we didn't find it
        entry = NULL; // let the action know
      else
        entry = &(ht->items[i]);
    }
  }
  return entry;
}

rsht_entry *rsht_get(const rsht_ht *ht, const char *key) {
  return rsht_get_hash(ht, key, djb2(key));
}

bool rsht_put(rsht_ht *ht, char *key, void *val, void **old_val_ref) {
  const unsigned long hash = djb2(key);
  rsht_entry *entry = rsht_get_hash(ht, key, hash);
  if (entry) { // if we found it, swap the value into the entry
    if (old_val_ref)
      *old_val_ref = entry->val;
    entry->val = val;
    return true;
  } else if (ht->capacity > ht->num_slots_used ||
      setcapacity(ht, ht->capacity * RSHT_CAPACITY_SCALE)) {
    // make sure there's room, or make room
    ht->items[ht->num_slots_used].key = key;
    ht->items[ht->num_slots_used].val = val;
    ht->buckets[hash % ht->num_buckets] = ht->num_slots_used + 1;
    ht->num_slots_used++;
    return true;
  } else { // we could not make room; blow up
    return false;
  }
}

void rsht_destroy(rsht_ht *ht) {
  if (!ht)
    return;
  free(ht->buckets);
  free(ht->items);
}

size_t rsht_foreach(rsht_ht *ht, rsht_callback fn, void *userdata) {
  size_t num_iterated = 0;
  for (size_t i = 0; i < ht->num_slots_used; i++) {
    rsht_entry *entry = &(ht->items[i]);
    if (entry->key == NULL)
      continue;
    if (fn(entry, userdata)) {
      num_iterated++;
    } else {
      break;
    }
  }
  return num_iterated;
}
