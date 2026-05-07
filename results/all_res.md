## -O3:

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 428 |
| 2 | 430 |
| 3 | 451 |
| 4 | 456 |
| 5 | 464 |
| 6 | 465 |
| 7 | 474 |

Результаты без минимума и максимума: $430$, $451$, $456$, $464$, $465$

Среднее количество тиков на <i>TableFind</i>: $453 
													\pm 16$

Относительная погрешность: $3.61\%$

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
| 1 | 383 |
| 2 | 393 |
| 3 | 395 |
| 4 | 396 |
| 5 | 399 |
| 6 | 400 |
| 7 | 410 |

Результаты без минимума и максимума: $393$, $395$, $396$, $399$, $400$

Среднее количество тиков на <i>TableFind</i>: $397 
													\pm 8$

Относительная погрешность: $1.90\%$

Получили ускорение на $\frac{453 - 397}{453} * 100 =
			12.36\% \pm 0.50$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{3.61^2+1.90^2} =
			4.08\%$

Абсолютная погрешность: $12.36\% * 0.0408 = 
			0.50\%$

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
| 1 | 352 |
| 2 | 358 |
| 3 | 359 |
| 4 | 360 |
| 5 | 360 |
| 6 | 360 |
| 7 | 363 |

Результаты без минимума и максимума: $358$, $359$, $360$, $360$, $360$

Среднее количество тиков на <i>TableFind</i>: $359 
													\pm 3$

Относительная погрешность: $0.89\%$

Полученное ускорение:

$\cdot$ на $\frac{397 - 359}{397} * 100 =
			9.57\% \pm 0.20$ $\%$ относительно предыдущей версии.

Относительная погрешность ускорения: 
			$\sqrt{1.90^2+0.89^2} =
			2.10\%$

Абсолютная погрешность: $9.57\% * 0.0210 = 
			0.20\%$


$\cdot$ на $\frac{453 - 359}{453} * 100 =
			20.75\% \pm 0.77$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{3.61^2+0.89^2} =
			3.72\%$

Абсолютная погрешность: $20.75\% * 0.0372 = 
			0.77\%$

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

## Ассемблерная вставка, проверяющая первые символы строк

```c
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
```

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 338 |
| 2 | 340 |
| 3 | 343 |
| 4 | 344 |
| 5 | 344 |
| 6 | 345 |
| 7 | 347 |

Результаты без минимума и максимума: $340$, $343$, $344$, $344$, $345$

Среднее количество тиков на <i>TableFind</i>: $343 
													\pm 3$

Относительная погрешность: $0.83\%$

Полученное ускорение:

$\cdot$ на $\frac{359 - 343}{359} * 100 =
			4.46\% \pm 0.05$ $\%$ относительно предыдущей версии.

Относительная погрешность ускорения: 
			$\sqrt{0.89^2+0.83^2} =
			1.22\%$

Абсолютная погрешность: $4.46\% * 0.0122 = 
			0.05\%$


$\cdot$ на $\frac{453 - 343}{453} * 100 =
			24.28\% \pm 0.90$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{3.61^2+0.83^2} =
			3.70\%$

Абсолютная погрешность: $24.28\% * 0.0370 = 
			0.90\%$

![3-я оптимизация](../images/opt_assem.png)
