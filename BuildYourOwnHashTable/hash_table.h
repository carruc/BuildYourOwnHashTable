#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct {
	char* key;
	char* value;
} ht_entry;

typedef struct {
	int size;
	int count;
	ht_entry** entries;
} ht_hash_table;

#endif