/*
 * rsht.h
 *
 * rsht - really simple hash table
 * Copyright (c) 2018 Robert Stolarz
 *
 * rsht is available free of charge under the terms of the MIT
 * License. You are free to redistribute and/or modify it under those
 * terms. It is distributed in the hopes that it will be useful, but
 * WITHOUT ANY WARRANTY. See the LICENSE file for more details.
 */

#ifndef RSHT_H
#define RSHT_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct rsht_entry {
  char *key;
  void *val;
} rsht_entry;

typedef struct rsht_ht {
  size_t capacity;
  size_t num_slots_used;
  rsht_entry *items;

  size_t num_buckets;
  size_t *buckets;
} rsht_ht;

// returns NULL if we were unable to allocate space for buckets or entries
rsht_ht *rsht_create(rsht_ht *ht, const size_t num_buckets, const size_t initial_capacity);

rsht_entry *rsht_get(const rsht_ht *ht, const char *key);
// returns false if we needed to increase capacity but failed to do so
bool rsht_put(rsht_ht *ht, char *key, void *val, void **old_val_ref);

void rsht_destroy(rsht_ht *ht);

// should return whether to continue iterating or not
typedef bool (*rsht_callback)(rsht_entry *, void *);

/* 
 * the userdata argument will be passed as the second parameter to fn
 * this routine will return the number of items iterated over (exclusive of an item that fails)
 */
size_t rsht_foreach(rsht_ht *ht, rsht_callback fn, void *userdata);


#ifdef __cplusplus
}
#endif

#endif
