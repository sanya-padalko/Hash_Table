## -O3:

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 423 |
| 2 | 425 |
| 3 | 441 |
| 4 | 446 |
| 5 | 453 |
| 6 | 454 |
| 7 | 454 |

Результаты без минимума и максимума: $425$, $441$, $446$, $453$, $454$

Среднее количество тиков на <i>TableFind</i>: $444 
													\pm 12$

Относительная погрешность: $2.81\%$

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

.section .note.GNU-stack, "", @progbits

```

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 392 |
| 2 | 392 |
| 3 | 393 |
| 4 | 395 |
| 5 | 398 |
| 6 | 404 |
| 7 | 404 |

Результаты без минимума и максимума: $392$, $393$, $395$, $398$, $404$

Среднее количество тиков на <i>TableFind</i>: $396 
													\pm 5$

Относительная погрешность: $1.24\%$

Получили ускорение на $\frac{444 - 396}{444} * 100 =
			10.81\% \pm 0.33$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{2.81^2+1.24^2} =
			3.07\%$

Абсолютная погрешность: $10.81\% * 0.0307 = 
			0.33\%$

![1-я оптимизация](../images/opt_strcmp.png)

## Замена crc32 на intrinsic-и

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
| 1 | 354 |
| 2 | 356 |
| 3 | 363 |
| 4 | 369 |
| 5 | 372 |
| 6 | 373 |
| 7 | 407 |

Результаты без минимума и максимума: $356$, $363$, $369$, $372$, $373$

Среднее количество тиков на <i>TableFind</i>: $367 
													\pm 17$

Относительная погрешность: $4.60\%$

Полученное ускорение:

$\cdot$ на $\frac{396 - 367}{396} * 100 =
			7.32\% \pm 0.35$ $\%$ относительно предыдущей версии.

Относительная погрешность ускорения: 
			$\sqrt{1.24^2+4.60^2} =
			4.76\%$

Абсолютная погрешность: $7.32\% * 0.0476 = 
			0.35\%$


$\cdot$ на $\frac{444 - 367}{444} * 100 =
			17.34\% \pm 0.93$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{2.81^2+4.60^2} =
			5.39\%$

Абсолютная погрешность: $17.34\% * 0.0539 = 
			0.93\%$

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
        call    my_strcmp
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

## Ассемблерная вставка, проверяющая первые два символа

```c
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
```

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 338 |
| 2 | 338 |
| 3 | 338 |
| 4 | 339 |
| 5 | 340 |
| 6 | 341 |
| 7 | 341 |

Результаты без минимума и максимума: $338$, $338$, $339$, $340$, $341$

Среднее количество тиков на <i>TableFind</i>: $339 
													\pm 1$

Относительная погрешность: $0.38\%$

Полученное ускорение:

$\cdot$ на $\frac{367 - 339}{367} * 100 =
			7.63\% \pm 0.35$ $\%$ относительно предыдущей версии.

Относительная погрешность ускорения: 
			$\sqrt{4.60^2+0.38^2} =
			4.62\%$

Абсолютная погрешность: $7.63\% * 0.0462 = 
			0.35\%$


$\cdot$ на $\frac{444 - 339}{444} * 100 =
			23.65\% \pm 0.67$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{2.81^2+0.38^2} =
			2.84\%$

Абсолютная погрешность: $23.65\% * 0.0284 = 
			0.67\%$

![3-я оптимизация](../images/opt_assem.png)
