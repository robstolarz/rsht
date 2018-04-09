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

#ifndef __RSHT
#define __RSHT

#include <stdbool.h>
#include <stddef.h>

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

rsht_ht *rsht_create(rsht_ht *ht, size_t num_buckets, size_t initial_capacity);

rsht_entry *rsht_get(rsht_ht *ht, char *key);
bool rsht_put(rsht_ht *ht, char *key, void *val, void **old_val_ref);

void rsht_destroy(rsht_ht *ht);

typedef bool (*rsht_callback)(rsht_entry *, void *);

// bool: false ends iteration early
void rsht_foreach(rsht_ht *ht, rsht_callback fn, void *userdata);

#endif
