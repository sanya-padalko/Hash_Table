int TableFind(Table* table, char* key) {
	__asm__ __volatile__ (
		".intel_syntax noprefix					\n\t"

		"test    rdi, rdi						\n\t"
        "je      .null_table					\n\t"
        "push    rbp							\n\t"
        "mov     rbp, rsi						\n\t"
        "push    rbx							\n\t"
        "mov     rbx, rdi						\n\t"
        "sub     rsp, 8							\n\t"
        "mov     esi, DWORD PTR [rdi+4]			\n\t"
        "mov     rdi, rbp						\n\t"
        "call    [QWORD PTR [rbx+8]]			\n\t"
        "mov     rdx, QWORD PTR [rbx+16]		\n\t"
        "cdqe									\n\t"
        "mov     rbx, QWORD PTR [rdx+rax*8]		\n\t"
        "test    rbx, rbx						\n\t"
        "jne     .check							\n\t"
        "jmp     .not_find						\n\t"

	".next_check:								\n\t"
        "mov     rbx, QWORD PTR [rbx+8]			\n\t"
        "test    rbx, rbx						\n\t"
        "je      .not_find						\n\t"

	".check:									\n\t"
        "mov     rdi, QWORD PTR [rbx]			\n\t"
        "mov     rsi, rbp						\n\t"
        "mov     al, BYTE PTR [rsi]				\n\t"	//	всё ради этих строк
		"mov	 cl, BYTE PTR [rdi]				\n\t"	//	перед тем как вызвать my_strcmp
		"cmp	 al, cl							\n\t"	//	сравниваем первые символы
		"jne	 .neq							\n\t"	//	наших строк
		"inc	 rsi							\n\t"
		"inc	 rdi							\n\t"
        "call    my_strcmp						\n\t"
	
	".neq:										\n\t"
        "test    eax, eax						\n\t"
        "jne     .next_check					\n\t"
        "add     rsp, 8							\n\t"
        "mov     eax, 1							\n\t"
        "pop     rbx							\n\t"
        "pop     rbp							\n\t"
        "ret									\n\t"

	".not_find:									\n\t"
        "add     rsp, 8							\n\t"
        "xor     eax, eax						\n\t"
        "pop     rbx							\n\t"
        "pop     rbp							\n\t"
        "ret									\n\t"

	".null_table:								\n\t"

		".att_syntax prefix						\n\t"
		: 
		:
		: "rax", "rcx", "rdx", "rsi", "rdi", "memory"
	);

	return 0;
}