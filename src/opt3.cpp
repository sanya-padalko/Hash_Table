#include "../hash_table.h"

uint32_t crc32_table[256];

extern "C" { int my_strcmp(const char* s1, const char* s2); }

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
	if (!table || !key)	return 0;

	int hash = table->hash_func(key, table->bit_size);
	Node* node = table->buckets[hash];

	while (node) {
		asm volatile goto (
			".intel_syntax noprefix			\n\t"
			"mov	rsi,	%0				\n\t"
			"mov	rdi,	%1				\n\t"
			"mov	al,		BYTE PTR [rsi]	\n\t"
			"mov	dl,		BYTE PTR [rdi]	\n\t"

			"cmp	al,		dl				\n\t"
			"je		.eq						\n\t"
			"jmp	%l[next_elem]			\n\t"

			".eq:							\n\t"
			".att_syntax prefix				\n\t"
			:
			: "r" (node->key), "r" (key)
			: "rsi", "rdi", "rax", "rdx", "cc"
			: next_elem
		);

		if (my_strcmp(node->key, key) == 0) return 1;

	next_elem:
		node = node->next;
	}

	return 0;
}

int get_hash(char* key, int size) {
	size_t key_len = strlen(key);
	
	return opt_crc32((const char*)key, key_len) % (1 << size);
}

inline uint32_t crc32(const char* data, int len) {
    uint32_t crc = 0xffffffff;
    for (int i = 0; i < len; ++i)
        crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ data[i]) & 255];

    return crc;
}

void gen_crc32_table() {
    for (int i = 0; i < 256; ++i) {
        uint32_t ch = i;
        for (size_t j = 0; j < 8; ++j) {
            if (ch & 1) ch = (ch >> 1) ^ CRC32_MUL;
            else ch >>= 1;
        }

        crc32_table[i] = ch;
    }
}

inline unsigned int opt_crc32(const char* data, int len) {
	unsigned int crc = 0xFFFFFFFF;

	while (len >= 8) {
		crc = (unsigned int)_mm_crc32_u64(crc, *(const uint64_t*)data);
		data += 8;
		len -= 8;
	}

	while (len--) 
		crc = _mm_crc32_u8(crc, *data++);

	return crc ^ 0xFFFFFFFF;
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

