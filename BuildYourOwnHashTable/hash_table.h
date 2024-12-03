#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct {
	char* key;
	char* value;
} ht_entry;

typedef struct {
	int size;
	int base_size;
	int count;
	ht_entry** entries;
} ht_hash_table;

void ht_insert(ht_hash_table* ht, const char* key, const char* value);

char* ht_search(ht_hash_table* ht, const char* key);

void ht_delete(ht_hash_table* h, const char* key);

#endif