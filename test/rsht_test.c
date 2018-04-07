#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <rsht.h>

bool printer(rsht_entry *ent, void *userdata) {
  printf("%s\n", ent->val);
  return true;
}

bool lookerupper(rsht_entry *ent, void *userdata) {
  assert(ent == rsht_search(userdata, ent, RSHT_GET));
  return true;
}


bool destructor(rsht_entry *ent, void *userdata) {
  free(ent->key);
  free(ent->val);
  free(ent);
  return true;
}

int main(char **argv, int argc) {
  rsht_ht *ht = calloc(1, sizeof(rsht_ht));
  rsht_create(ht, 5, 5);
  for (int i = 0; i < 10; i++) {
    char *str = calloc(2, sizeof(char));
    str[0] = i + 65;
    printf("%s\n", str);
    rsht_entry *ent = calloc(1, sizeof(rsht_entry));
    ent->key = str;
    ent->val = strdup(str);
    rsht_search(ht, ent, RSHT_PUT);
  }

  rsht_entry e = {.key = "key that definitely doesn't exist"};
  assert(rsht_search(ht, &e, RSHT_GET) == NULL);
  rsht_foreach(ht, lookerupper, ht);
  rsht_foreach(ht, printer, NULL);
  rsht_foreach(ht, destructor, NULL);
  rsht_destroy(ht);
  free(ht);
  return 0;
}
