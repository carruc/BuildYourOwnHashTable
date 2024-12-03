#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

const int HT_DEFAULT_SIZE = 53;
const int HT_PRIME_1 = 1;
const int HT_PRIME_2 = 2;
static ht_entry HT_DELETED_ITEM = { NULL, NULL };

static ht_entry* ht_entry_new(const char* k, const char* v) {
	ht_entry* e = malloc(sizeof(ht_entry));
	e->key = _strdup(k);
	e->value = _strdup(v);
	return e;
}

ht_hash_table* ht_new() {
	ht_hash_table* ht = malloc(sizeof(ht_hash_table));
	ht->size = (HT_DEFAULT_SIZE);
	ht->count = 0;
	ht->entries = calloc(ht->size, sizeof(ht_entry));
	return ht;
}

static void ht_entry_delete(ht_entry* e) {
	free(e->key);
	free(e->value);
	free(e);
}

void ht_delete(ht_hash_table* ht) {
	for (int i = 0; i < ht->size; i++){
		ht_entry* e = ht->entries[i];
		if (e != NULL) {
			ht_entry_delete(e);
		}
	}
	free(ht->entries);
	free(ht);
}


//input: string, output: number between 0 and m, the desired bucket array len
//focus: minimize collisions
static int ht_hash(const char* v, const int a, const int m){
	long hash = 0;
	const int len_v = strlen(v);
	for (size_t i = 0; i < len_v; i++) {
		hash += (long) pow(a, len_v - (i + 1)) * v[i];
		hash = hash % m;
	}
	return (int)hash;
}

static int ht_get_hash(
	const char* s, const int num_buckets, const int attempt
) {
	const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
	const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
	return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
	ht_entry* entry = ht_entry_new(key, value);
	int index = ht_get_hash(entry->key, ht->size, 0);
	ht_entry* cur_entry = ht->entries[index];
	int i = 1;
	while (cur_entry != NULL) {
		if (cur_entry != &HT_DELETED_ITEM)
		{
			if (strcmp(cur_entry->key, key)) {
				ht_entry_delete(cur_entry);
				ht->entries[index] = entry;
				return;
			}
		}
		index = ht_get_hash(entry->key, ht->size, i);
		cur_entry = ht->entries[i];
		++i;
	}
	ht->entries[index] = entry;
	ht->count++;
}

char* ht_search(ht_hash_table* ht, const char* key) {
	int index = ht_get_hash(key, ht->size, 0);
	ht_entry* entry = ht->entries[index];
	int i = 1;
	while (entry != NULL) {
		if (entry != &HT_DELETED_ITEM) {
			if (strcmp(entry->key, key) == 0) {
				return entry->value;
			}
		}
		index = ht_get_hash(key, ht->size, i);
		entry = ht->entries[index];
		i++;
	}
	return NULL;
}

void ht_delete(ht_hash_table* ht, const char* key) {
	int index = ht_get_hash(key, ht->size, 0);
	ht_entry* entry = ht->entries[index];
	int i = 1;
	while (entry != NULL) {
		if (entry != &HT_DELETED_ITEM) {
			if (strcmp(entry->key, key) == 0) {
				ht_del_item(entry);
				ht->entries[index] = &HT_DELETED_ITEM;
			}
		}
		index = ht_get_hash(key, ht->size, i);
		entry = ht->entries[index];
		i++;
	}
	ht->count--;
}

