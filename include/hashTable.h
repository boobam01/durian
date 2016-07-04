#pragma once

#include "uthash.h"

namespace durian {  
  struct Map {
    char key[100];                    /* key */
    char val[8000];
    UT_hash_handle hh;         /* makes this structure hashable */
  };

  class HashTable {
  public:
    struct Map *find(const char* key) {
      struct Map *s;

      HASH_FIND_STR(map, key, s);  /* s: output pointer */
      return s;
    }

    void add(const char* key, char *val) {
      struct Map *s;

      HASH_FIND_STR(map, key, s);  /* id already in the hash? */
      if (s == NULL) {
        s = (struct Map*)malloc(sizeof(struct Map));
        strcpy(s->key, key);
        HASH_ADD_STR(map, key, s);  /* id: name of key field */
      }
      strcpy(s->val, val);
    }

    void deleteOne(struct Map *ht) {
      HASH_DEL(map, ht);  /* user: pointer to deletee */
      free(ht);
    }

    void deleteAll() {
      struct Map *current_k, *tmp;

      HASH_ITER(hh, map, current_k, tmp) {
        HASH_DEL(map, current_k);  /* delete it (users advances to next) */
        free(current_k);            /* free it */
      }
    }

    struct Map* get() {
      return map;
    }

  private:
    struct Map *map = NULL;
  };
}
