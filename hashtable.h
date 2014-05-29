#pragma once

typedef struct _HashTable *lpHashTable;

typedef int (*ht_cmp)(void *ud, const void *key1, const void *key2);
typedef void (*ht_free)(void *ud, void *key, void *val);

lpHashTable ht_create(int size, void *ud, ht_cmp f_cmp, ht_free f_free);
void ht_destroy(lpHashTable tab);
void *ht_get(lpHashTable tab, const void *key);
void ht_put(lpHashTable tab, const void *key, void *val);
void ht_delete(lpHashTable tab, const void *key);