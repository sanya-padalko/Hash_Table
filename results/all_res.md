## -O2:

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 449 |
| 2 | 451 |
| 3 | 455 |
| 4 | 456 |
| 5 | 457 |
| 6 | 457 |
| 7 | 458 |

Результаты без минимума и максимума: $451$, $455$, $456$, $457$, $457$

Среднее количество тиков на <i>TableFind</i>: $455$

Относительная погрешность: $0.70\%$

![До оптимизаций](../images/base.png)

## Замена strcmp на my_strcmp из ассемблерного файла

```
.intel_syntax noprefix
.global my_strcmp
.text

my_strcmp:

.loop:
	mov   al, [rdi]
	mov   dl, [rsi]

	cmp   al, dl
	jne   .end_loop

	test  al, al
	jz    .end_loop

	inc   rdi
	inc   rsi
	jmp   .loop

.end_loop:
	movzx eax, al
	movzx edx, dl
	sub   eax, edx
	ret

.section .note.GNU-stack,"",@progbits

```

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 398 |
| 2 | 399 |
| 3 | 402 |
| 4 | 402 |
| 5 | 406 |
| 6 | 409 |
| 7 | 410 |

Результаты без минимума и максимума: $399$, $402$, $402$, $406$, $409$

Среднее количество тиков на <i>TableFind</i>: $404$

Относительная погрешность: $1.08\%$

Получили ускорение на $\frac{455 - 404}{455} * 100 = 11.21\% \pm 0.14$ $\%$ относительно -O3.

Абсолютная погрешность: $11.21\% * \frac{\sqrt{0.70^2+1.08^2}}{100} = 11.21\% * 0.0129 = 0.14\%$

Относительная погрешность ускорения: $1.29\%$

![1-я оптимизация](../images/opt_strcmp.png)

## Замена crc32 на intrinsic-и:

```c
inline unsigned int opt_crc32(const uchar* data, int len) {
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
```

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 375 |
| 2 | 378 |
| 3 | 379 |
| 4 | 379 |
| 5 | 382 |
| 6 | 384 |
| 7 | 385 |

Результаты без минимума и максимума: $378$, $379$, $379$, $382$, $384$

Среднее количество тиков на <i>TableFind</i>: $380$

Относительная погрешность: $0.86\%$

Полученное ускорение:

$\cdot$ на $\frac{404 - 380}{404} * 100 = 5.94\% \pm 0.08$ $\%$ относительно предыдущей оптимизации.

Абсолютная погрешность: $5.94\% * \frac{\sqrt{1.08^2+0.86^2}}{100} = 5.94\% * 0.0138 = 0.08\%$

Относительная погрешность ускорения: $1.38\%$

$\cdot$ на $\frac{455 - 380}{455} * 100 = 16.48\% \pm 0.18$ $\%$ относительно -O3.

Абсолютная погрешность: $16.48\% * \frac{\sqrt{0.70^2+0.86^2}}{100} = 16.48\% * 0.0111 = 0.18\%$

Относительная погрешность ускорения: $1.11\%$

![2-я оптимизация](../images/opt_crc32.png)

Рассмотрим ассемблерный вид <i>TableFind</i> c godbolt:

```
TableFind(Table*, char*):
        test    rdi, rdi
        je      .L14
        push    rbp
        mov     rbp, rsi
        push    rbx
        mov     rbx, rdi
        sub     rsp, 8
        mov     esi, DWORD PTR [rdi+4]
        mov     rdi, rbp
        call    [QWORD PTR [rbx+8]]
        mov     rdx, QWORD PTR [rbx+16]
        cdqe
        mov     rbx, QWORD PTR [rdx+rax*8]
        test    rbx, rbx
        jne     .L4
        jmp     .L2
.L18:
        mov     rbx, QWORD PTR [rbx+8]
        test    rbx, rbx
        je      .L2
.L4:
        mov     rdi, QWORD PTR [rbx]
        mov     rsi, rbp
        call    strcmp
        test    eax, eax
        jne     .L18
        add     rsp, 8
        mov     eax, 1
        pop     rbx
        pop     rbp
        ret
.L2:
        add     rsp, 8
        xor     eax, eax
        pop     rbx
        pop     rbp
        ret
.L14:
        xor     eax, eax
        ret
```

## Замена TableFind на ассемблерную вставку:

```c
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
```

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 338 |
| 2 | 339 |
| 3 | 340 |
| 4 | 342 |
| 5 | 344 |
| 6 | 344 |
| 7 | 350 |

Результаты без минимума и максимума: $339$, $340$, $342$, $344$, $344$

Среднее количество тиков на <i>TableFind</i>: $342$

Относительная погрешность: $1.12\%$

Полученное ускорение:

$\cdot$ на $\frac{380 - 342}{380} * 100 = 10.00\% \pm 0.14$ $\%$ относительно предыдущей оптимизации.

Абсолютная погрешность: $10.00\% * \frac{\sqrt{0.86^2+1.12^2}}{100} = 10.00\% * 0.0141 = 0.14\%$

Относительная погрешность ускорения: $1.41\%$

$\cdot$ на $\frac{455 - 342}{455} * 100 = 24.84\% \pm 0.33$ $\%$ относительно -O3.

Абсолютная погрешность: $24.84\% * \frac{\sqrt{0.70^2+1.12^2}}{100} = 24.84\% * 0.0132 = 0.33\%$

Относительная погрешность ускорения: $1.32\%$

![3-я оптимизация](../images/opt_assem.png)
