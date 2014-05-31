#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hashtable.h"

typedef  struct _HashTableEl {
	struct _HashTableEl *next;
	void *key;
	void *val;
} HashTableEl;

struct _HashTable {
	HashTableEl **arr;
	int len;
	void *ud;
	ht_cmp f_cmp;
	ht_free f_free;
};

typedef struct _HashTable HashTable;
typedef struct _HashTable *lpHashTable;

// Jenkins hash function
unsigned long hash(void *key) {
	char *str = (char*) key;
	unsigned long code = 0x3F230156;
	int i;
	for(i=0; str[i]!=0; i++){
		code += (unsigned char) str[i];
		code += code << 10;
		code ^= code >> 6;
	}
	code += code << 3;
	code ^= code >> 11;
	code += code << 15;
	return code;
}

int cmp(void *key1, void *key2) {
	return strcmp((char*)key1, (char*)key2) == 0;
}

lpHashTable ht_create(int size, void *ud, ht_cmp f_cmp, ht_free f_free) {
	lpHashTable tab = (lpHashTable) malloc(sizeof(HashTable));
	int i;
	tab->arr = (HashTableEl **) malloc(size*sizeof(void*));
	for(i=0; i<size; i++)
		tab->arr[i] = 0;
	tab->len	= size;
	tab->ud		= ud;
	tab->f_cmp  = f_cmp;
	tab->f_free = f_free;
	return tab;
}

void ht_destroy(lpHashTable tab) {
	int i;
	for(i=0; i<tab->len; i++){
		while(tab->arr[i]!=0)
			ht_delete(tab, tab->arr[i]->key);
	}
	free(tab->arr);
	free(tab);
}

void *ht_get(lpHashTable tab, void *key) {
	int code = hash(key) % tab->len;
	HashTableEl *el = tab->arr[code];
	while(el!=0){
		if( tab->f_cmp(tab->ud,el->key,key)==0 )
			return el->val;
		el = el->next;
	}
	return 0;
}

void ht_put(lpHashTable tab, void *key, void *val) {
	int code = hash(key) % tab->len;
	HashTableEl *el;
	if( ht_get(tab,key) != 0 )
		//return;
		ht_delete(tab,key);
	el = (HashTableEl *) malloc(sizeof(HashTableEl));
	el->next = tab->arr[code];
	el->key = key;
	el->val = val;
	tab->arr[code] = el;
}

void ht_delete(lpHashTable tab, void *key) {
	int code = hash(key) % tab->len;
	HashTableEl *el = tab->arr[code];
	HashTableEl *prev = 0, *next = 0;
	while(el!=0){
		if( cmp(el->key,key) ){
			if( prev==0 )
				tab->arr[code] = el->next;
			else
				prev->next = el->next;
			if(tab->f_free!=0)
				tab->f_free(tab->ud,el->key,el->val);
			free(el);
			return;
		}
		prev = el;
		el = el->next;
	}
}