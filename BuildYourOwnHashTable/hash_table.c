#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

static ht_entry* ht_new_entry(const char* k, const char* v) {
	ht_entry* e = malloc(sizeof(ht_entry));
	e->key = strdup(k);
	e->value = strdup(v);
	return e;
}

ht_hash_table* ht_new() {
	ht_hash_table* ht = malloc(sizeof(ht_hash_table));

}