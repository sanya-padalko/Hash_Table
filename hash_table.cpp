#include "hash_table.h"

Table* TableCtor(int bit_size) {
	Table* table = CALLOC(Table);

	table->cnt      = 0;
	table->bit_size = bit_size;
	table->buckets  = CALLOC_SIZE((1 << bit_size), Node*);
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
	free(table->buckets);
	free(table);
}

void EasyInsert(Node* bucket, char* key) {
	if (!bucket) return;

	while (bucket->next) {
		if (!strcmp(bucket->key, key))	return;
		bucket = bucket->next;
	}
	
	if (!strcmp(bucket->key, key))	return;

	bucket->next = NodeCtor(key);
}

void TableRehash(Table* table) {
	Node** new_buckets = CALLOC_SIZE(1 << (table->bit_size + 1), Node*);

	for (int i = 0; i < (1 << table->bit_size); ++i) {
		Node* node = table->buckets[i];
		while (node) {
			int new_hash = get_hash(node->key, table->bit_size + 1);
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

void TableInsert(Table* table, char* key) {
	if (!table)	return;

	if ((float)(table->cnt + 1) / (1 << table->bit_size) > LOAD_FACTOR)
		TableRehash(table);

	int hash = get_hash(key, table->bit_size);
	Node* node = table->buckets[hash];
	
	if (!node)
		table->buckets[hash] = NodeCtor(key);
	else
		EasyInsert(table->buckets[hash], key);

	++table->cnt;
}

void TablePrint(Table* table) {
	fprintf(stderr, "hash | keys\n");
	fprintf(stderr, "----------------------------------------------\n");
	for (int i = 0; i < (1 << table->bit_size); ++i) {
		fprintf(stderr, "%4d | ", i);
		Node* node = table->buckets[i];
		while (node) {
			fprintf(stderr, "%s", node->key);
			if (node->next)
				fprintf(stderr, " -> ");
			
			node = node->next;
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "----------------------------------------------\n");
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
	return crc32(0, (const uchar*)key, strlen(key)) % (1 << size);
}
