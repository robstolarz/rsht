# rsht (really simple hash table)

rsht is a really basic [LPMult](http://www.vldb.org/pvldb/vol9/p96-richter.pdf) hash table with a simple Bloom-like optimization that sometimes prevents unsuccessful reads from causing O(n) scans. Its small code size and lack of use of linked lists make it ideal for usecases with small _n_ and low load factors.

rsht avoids copying data, allowing the caller to manage memory as they wish. This leads to just slightly more complex calling code, with the benefit of (probably?) better performance.

## limitations

* Deleting keys is not possible without recreating the table. The design of the API makes it hard to return the key that was deleted without copying or overwriting something, and it would very handily defeat the Bloom-like optimization. It would be possible to fix, but not without a cost to runtime performance (O(n) scan over hash buckets). A caller should invalidate values rather than keys.
* Shrinking the backing store is not possible without recreating the table.
* Increasing the bucket count is not possible without recreating the table. Doing so would require rehashing everything, which a caller could do anyway.
* Bloom-like only prevents full scans in the absence of collisions. This is mostly acceptable because items that are not captured in the buckets will be towards the beginning of the item list, making the O(n) scan shorter on average.

## building

rsht conforms to the C99 standard. To build it,

```sh
cd build
cmake ..
make
make test # to test, if you want
```

or use it as part of an existing CMake build with `add_subdirectory`

TODO: allow people to build shared, somehow (CMake `CACHE` var?)

## usage

initialization:
```c
rsht_ht *ht = malloc(sizeof(rsht_ht)); // can also be stack allocated
rsht_create(ht, num_buckets, initial_capacity);
```
insertion:
```c
char *key = strdup("my key");
char *val = strdup("my val");

// if a caller knows the value doesn't already exist
// BEWARE: using it this way can cause memory leaks!
if (!rsht_put(ht, key, val, NULL)) {
  // handle out of memory error
}

// if a caller wants to know the previous value and free it
void *old_val;
if (!rsht_put(ht, key, val, &old_val)) {
  // handle out of memory error
  free(key);
}
if (old_val) {
  free(key); // if the value existed, we can free the key we just used
  free(old_val);
}
```
retrieval:
```c
rsht_entry *ret = rsht_get(ht, "key");
if (ret) {
  // use the item that was found
} else {
  // handle the lack of an item
}
```
cleanup:
```c
bool entry_destroyer(rsht_entry *e, void *userdata) {
  // however one chooses to destroy entries.
  // this example assumes `val` might be `NULL` and everything heap allocated.
  free(e->key);
  if (e->val)
    free(e->val);
  return true; // if this were false, the loop would stop
}

rsht_foreach(ht, entry_destroyer, NULL);
rsht_destroy(ht);
```

## testing
The current tests can be run via `make test` when rsht is configured as a standalone project. They're not very comprehensive and need some work.

## alternative names
The "RS" of rsht could really stand for any number of things. Consider:

* Rob's Silly Hash Table
* Really Super Hash Table
* and many more, including those laced with profanity and muttered under one's breath

## license
`rsht.h` and `src/rsht.c` are provided under the terms of the MIT license.

Everything else is dual-licensed MIT/Public Domain.
