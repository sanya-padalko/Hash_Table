int TableFind(Table* table, char* key) {
	if (!table)	return 0;

	int hash = table->hash_func(key, table->bit_size);
	Node* node = table->buckets[hash];

	while (node) {
		int comp_result = 0;

		asm volatile (
			".intel_syntax noprefix			        \n\t"
			"mov	rsi,	%1						\n\t"
			"mov	rdi,	%2						\n\t"
			"mov	al,		BYTE PTR [rsi]			\n\t"
			"mov	dl,		BYTE PTR [rdi]			\n\t"

			"cmp	al,		dl						\n\t"
			"jne	.neq							\n\t"
			"inc	rsi								\n\t"
			"inc	rdi								\n\t"
			"call	my_strcmp						\n\t"
			"jmp	.end							\n\t"

			".neq:									\n\t"
			"mov	eax, 	1						\n\t"

			".end:									\n\t"
			"mov	%0,		eax						\n\t"
			".att_syntax prefix						\n\t"
			: "=r" (comp_result)
			: "r" (node->key), "r" (key)
			: "rsi", "rdi", "rax", "rdx", "cc"
		);

		if (comp_result == 0) return 1;

		node = node->next;
	}

	return 0;
}