#pragma once

#include <string>
#include "uthash.h"

#define MAX_KEY_LEN 100
#define MAX_VAL_LEN 32000

namespace durian {  
  struct Map {
    char key[MAX_KEY_LEN];                    /* key */
    char val[MAX_VAL_LEN];
    // std::string key;
    // std::string val;
    UT_hash_handle hh;         /* makes this structure hashable */
  };

  class HashTable {
  public:
    struct Map *find(const char* key) {
      struct Map *s;

      HASH_FIND_STR(_map, key, s);  /* s: output pointer */
      return s;
    }

    // void add(const std::string key, const std::string val) {
    void add(const char* key, const char* val) {
      struct Map *s;

      HASH_FIND_STR(_map, key, s);  /* id already in the hash? */
      if (s == NULL) {
        s = (struct Map*)malloc(sizeof(struct Map));
        strncpy(s->key, key, MAX_KEY_LEN);
        //s->key = key;
        HASH_ADD_STR(_map, key, s);  /* id: name of key field */
      }
      strncpy(s->val, val, MAX_VAL_LEN);
      // s->val = val;
    }

    void deleteOne(struct Map *ht) {
      HASH_DEL(_map, ht);  /* user: pointer to deletee */
      free(ht);
    }

    void deleteAll() {
      struct Map *current_k, *tmp;

      HASH_ITER(hh, _map, current_k, tmp) {
        HASH_DEL(_map, current_k);  /* delete it (users advances to next) */
        free(current_k);            /* free it */
      }
    }

    struct Map* get() {
      return _map;
    }

  private:
    struct Map* _map = NULL;
  };
}
