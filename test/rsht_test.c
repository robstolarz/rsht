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
  assert(ent == rsht_get(userdata, ent->key));
  return true;
}


bool destructor(rsht_entry *ent, void *userdata) {
  free(ent->key);
  free(ent->val);
  return true;
}

int main(char **argv, int argc) {
  rsht_ht *ht = calloc(1, sizeof(rsht_ht));
  rsht_create(ht, 5, 5);
  for (int i = 0; i < 10; i++) {
    char *str = calloc(2, sizeof(char));
    str[0] = i + 65;
    printf("%s\n", str);
    rsht_put(ht, str, strdup(str), NULL);
  }

  assert(rsht_get(ht, "key that definitely doesn't exist") == NULL);
  rsht_foreach(ht, lookerupper, ht);
  rsht_foreach(ht, printer, NULL);
  void *old_val;
  rsht_put(ht, "J", strdup("?"), &old_val);
  free(old_val);
  rsht_foreach(ht, printer, NULL);
  rsht_foreach(ht, destructor, NULL);
  rsht_destroy(ht);
  free(ht);
  return 0;
}
