#include "hash_table.h"
#include <nmmintrin.h>
#include <stdint.h>

Table* TableCtor(int bit_size, int (*func)(char*, int)) {
	Table* table = CALLOC(Table);

	table->cnt			= 0;
	table->bit_size		= bit_size;
	table->buckets		= CALLOC_SIZE((1 << bit_size), Node*);
	table->hash_func	= func;
	for (int i = 0; i < (1 << bit_size); ++i)
		table->buckets[i] = NULL;

	return table;
}

void TableDtor(Table* table) {

	for (int i = 0; i < (1 << table->bit_size); ++i) {
		Node* node = table->buckets[i];
		while (node) {
			Node* next_node = node->next;
			NodeDtor(node);
			node = next_node;
		}
	}

	table->cnt = table->bit_size = 0;
	table->hash_func = NULL;

	free(table->buckets);
	free(table);
}

void TableInsert(Table* table, char* key) {
	if (!table)	return;

	if ((float)(table->cnt + 1) / (1 << table->bit_size) > LOAD_FACTOR)
		TableRehash(table);

	int hash = table->hash_func(key, table->bit_size);
	Node* node = table->buckets[hash];
	
	if (!node) {
		table->buckets[hash] = NodeCtor(key);
		++table->cnt;
	}
	else
		table->cnt += EasyInsert(table->buckets[hash], key);
}

void TableRehash(Table* table) {
	Node** new_buckets = CALLOC_SIZE(1 << (table->bit_size + 1), Node*);

	for (int i = 0; i < (1 << table->bit_size); ++i) {
		Node* node = table->buckets[i];
		while (node) {
			int new_hash = table->hash_func(node->key, table->bit_size + 1);
			if (!new_buckets[new_hash])
				new_buckets[new_hash] = NodeCtor(node->key);
			else
				EasyInsert(new_buckets[new_hash], node->key);
			
			Node* new_node = node->next;
			NodeDtor(node);
			node = new_node;
		}
	}

	free(table->buckets);
	table->buckets = new_buckets;
	++table->bit_size;
}

int EasyInsert(Node* bucket, char* key) {
	if (!bucket) return 0;

	while (bucket->next) {
		if (!strcmp(bucket->key, key))	return 0;
		bucket = bucket->next;
	}
	
	if (!strcmp(bucket->key, key))	return 0;

	bucket->next = NodeCtor(key);
	return 1;
}

void TablePrint(Table* table) {
	FILE* table_view = fopen("table_view.txt", "w");

	fprintf(table_view, "  hash  | keys\n");
	fprintf(table_view, "----------------------------------------------\n");
	for (int i = 0; i < (1 << table->bit_size); ++i) {
		fprintf(table_view, "%7d | ", i);
		Node* node = table->buckets[i];
		while (node) {
			fprintf(table_view, "%s", node->key);
			if (node->next)
				fprintf(table_view, " -> ");
			
			node = node->next;
		}
		fprintf(table_view, "\n");
	}
	fprintf(table_view, "----------------------------------------------\n");

	fprintf(table_view, "\nWord's count: %d\n", table->cnt);

	fclose(table_view);
}

int TableFind(Table* table, char* key) {
	if (!table)	return 0;

	int hash = table->hash_func(key, table->bit_size);
	Node* node = table->buckets[hash];

	char first_char = key[0];

	while (node) {
		if (node->key[0] == first_char && !strcmp(node->key, key)) return 1;

		node = node->next;
	}

	return 0;
}

Node* NodeCtor(char* key) {
	Node* node = CALLOC(Node);
	node->key  = strdup(key);
	node->next = NULL;

	return node;
}

void NodeDtor(Node* node) {
	free(node->key);
	free(node);
}

int get_hash(char* key, int size) {
	size_t key_len = strlen(key);
	
	return crc32(0, (const uchar*)key, key_len) % (1 << size);
}
