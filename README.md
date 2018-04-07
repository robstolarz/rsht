# rsht (really simple hash table)

rsht is a really basic [LPMult](http://www.vldb.org/pvldb/vol9/p96-richter.pdf) hash table with a simple Bloom-like optimization that sometimes prevents unsuccessful reads from causing O(n) scans. Its small code size and lack of use of linked lists make it ideal for usecases with small _n_ and low load factors.

rsht avoids copying data, allowing the caller to manage memory as they wish. This leads to just slightly more complex calling code, with the benefit of (probably?) better performance.

## limitations

* Deleting keys is not possible without recreating the table. The design of the API makes it hard to return the key that was deleted without copying or overwriting something, and it would very handily defeat the Bloom-like optimization. It would be possible to fix, but not without a cost to runtime performance (O(n) scan over hash buckets). A caller should invalidate values rather than keys.
* Shrinking the backing store is not possible without recreating the table.
* Increasing the bucket count is not possible without recreating the table. Doing so would require rehashing everything, which a caller could do anyway.
* Bloom-like only prevents full scans in the absence of collisions. This is mostly acceptable because items that are not captured in the buckets will be towards the beginning of the item list, making the O(n) scan shorter on average.

## building

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
rsht_entry *e = malloc(sizeof(rsht_entry));
e->key = strdup("my key");
e->val = strdup("my val");

// if a caller knows the item has never been entered before
if (!rsht_search(ht, e, RSHT_PUT)) {
  // handle out of memory error
}

// if a caller suspects the item may already exist
rsht_entry *ret = rsht_search(ht, e, RSHT_PUT);
if (!ret) {
  // handle out of memory error
} else if (ret != e) {
  // it was simply updated, time to tear down the object
  free(e->key);
  free(e->val); // this is now the previously stored value
  free(e);
} else {
  // it was properly stored
}
```
retrieval:
```c
rsht_entry f = {.key = "my key"}; // stack allocating this is ok
rsht_entry *ret = rsht_search(ht, &f, RSHT_GET);
if (ret) {
  // use the item that was found
} else {
  // handle the lack of an item
}
// `f` will go out of scope after this; it can be reused for multiple searches
```
cleanup:
```c
bool entry_destroyer(rsht_entry *e, void *userdata) {
  // however one chooses to destroy entries.
  // this example assumes entries are heap allocated and `val` might be `NULL`.
  if (e) {
    if (e->val)
      free(e->val);
    free(e->key)
    free(e)
  }
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
