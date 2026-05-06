int TableFind(Table* table, char* key) {
	if (!table)	return 0;

	int hash = table->hash_func(key, table->bit_size);
	Node* node = table->buckets[hash];

	while (node) {
		asm volatile (
			".intel_syntax noprefix			\n\t"
			"mov	rsi,	%0				\n\t"
			"mov	rdi,	%1				\n\t"
			"mov	al,		BYTE PTR [rsi]	\n\t"
			"mov	dl,		BYTE PTR [rdi]	\n\t"

			"cmp	al,		dl				\n\t"
			"je		.eq						\n\t"
			"mov	eax, 	1				\n\t"
			"ret							\n\t"

			".eq:							\n\t"
			".att_syntax prefix				\n\t"
			:
			: "r" (node->key), "r" (key)
			: "rsi", "rdi", "rax", "rdx", "cc"
		);

		if (my_strcmp(node->key, key) == 0) return 1;

		node = node->next;
	}

	return 0;
}