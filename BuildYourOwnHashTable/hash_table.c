#include <stdlib.h>
#include <string.h>

#include "hash_table.h"
#include "prime.h"

const int HT_INITIAL_BASE_SIZE = 53;
const int HT_PRIME_1 = 337;
const int HT_PRIME_2 = 89;
static ht_entry HT_DELETED_ITEM = { NULL, NULL };

static ht_entry* ht_entry_new(const char* k, const char* v) {
	ht_entry* e = malloc(sizeof(ht_entry));
	e->key = _strdup(k);
	e->value = _strdup(v);
	return e;
}

ht_hash_table* ht_new_sized(const int base_size) {
	ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
	ht->base_size = base_size;

	ht->size = next_prime(ht->base_size);

	ht->count = 0;
	ht->entries = xcalloc((size_t) ht->size, sizeof(ht_entry*));
	return ht;
}

ht_hash_table* ht_new() {
	return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static void ht_resize(ht_hash_table* ht, const int base_size) {
	if (base_size < HT_INITIAL_BASE_SIZE) {
		return;
	}
	ht_hash_table* new_ht = ht_new_sized(base_size);
	for (int i = 0; i < ht->size; i++)
	{
		ht_entry* entry = ht->entries[i];
		if (entry != NULL && entry != &HT_DELETED_ITEM) {
			ht_insert(new_ht, entry->key, entry->value);
		}
	}

	ht->base_size = new_ht->base_size;
	ht->count = new_ht->count;

	// To delete new_ht, we give it ht's size and items 
	const int tmp_size = ht->size;
	ht->size = new_ht->size;
	new_ht->size = tmp_size;

	ht_entry** tmp_entries = ht->entries;
	ht->entries = new_ht->entries;
	new_ht->entries = tmp_entries;

	ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) {
	const int new_size = ht->base_size * 2;
	ht_resize(ht, new_size);
}


static void ht_resize_down(ht_hash_table* ht) {
	const int new_size = ht->base_size / 2;
	ht_resize(ht, new_size);
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

static int ht_get_hash(const char* s, const int num_buckets, const int attempt) {
	const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
	const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
	return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

void ht_insert(ht_hash_table* ht, const char* key, const char* value) {

	//Load higher than 70% implies resizing by a factor of 2
	const int load = ht->count * 100 / ht->size;
	if (load > 70) {
		ht_resize_up(ht);
	}

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

	//Load lower than 10% implies resizing by a factor of 0.5
	const int load = ht->count * 100 / ht->size;
	if (load < 10) {
		ht_resize_down(ht);
	}

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