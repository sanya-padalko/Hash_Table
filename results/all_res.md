## -O3:

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 445 |
| 2 | 446 |
| 3 | 447 |
| 4 | 447 |
| 5 | 448 |
| 6 | 449 |
| 7 | 450 |

Результаты без минимума и максимума: $446$, $447$, $447$, $448$, $449$

Среднее количество тиков на <i>TableFind</i>: $447 
													\pm 2$

Относительная погрешность: $0.36\%$

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
| 1 | 377 |
| 2 | 380 |
| 3 | 383 |
| 4 | 387 |
| 5 | 392 |
| 6 | 393 |
| 7 | 400 |

Результаты без минимума и максимума: $380$, $383$, $387$, $392$, $393$

Среднее количество тиков на <i>TableFind</i>: $387 
													\pm 8$

Относительная погрешность: $1.94\%$

Получили ускорение на $\frac{447 - 387}{447} * 100 =
			13.42\% \pm 0.26$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{0.36^2+1.94^2} =
			1.97\%$

Абсолютная погрешность: $13.42\% * 0.0197 = 
			0.26\%$

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
| 1 | 369 |
| 2 | 369 |
| 3 | 371 |
| 4 | 372 |
| 5 | 374 |
| 6 | 376 |
| 7 | 387 |

Результаты без минимума и максимума: $369$, $371$, $372$, $374$, $376$

Среднее количество тиков на <i>TableFind</i>: $372 
													\pm 6$

Относительная погрешность: $1.62\%$

Полученное ускорение:

$\cdot$ на $\frac{387 - 372}{387} * 100 =
			3.88\% \pm 0.10$ $\%$ относительно предыдущей версии.

Относительная погрешность ускорения: 
			$\sqrt{1.94^2+1.62^2} =
			2.53\%$

Абсолютная погрешность: $3.88\% * 0.0253 = 
			0.10\%$


$\cdot$ на $\frac{447 - 372}{447} * 100 =
			16.78\% \pm 0.28$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{0.36^2+1.62^2} =
			1.66\%$

Абсолютная погрешность: $16.78\% * 0.0166 = 
			0.28\%$

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
```

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 99 |
| 2 | 101 |
| 3 | 102 |
| 4 | 102 |
| 5 | 102 |
| 6 | 102 |
| 7 | 105 |

Результаты без минимума и максимума: $101$, $102$, $102$, $102$, $102$

Среднее количество тиков на <i>TableFind</i>: $102 
													\pm 2$

Относительная погрешность: $1.61\%$

Полученное ускорение:

$\cdot$ на $\frac{372 - 102}{372} * 100 =
			72.58\% \pm 1.65$ $\%$ относительно предыдущей версии.

Относительная погрешность ускорения: 
			$\sqrt{1.62^2+1.61^2} =
			2.28\%$

Абсолютная погрешность: $72.58\% * 0.0228 = 
			1.65\%$


$\cdot$ на $\frac{447 - 102}{447} * 100 =
			77.18\% \pm 1.27$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{0.36^2+1.61^2} =
			1.65\%$

Абсолютная погрешность: $77.18\% * 0.0165 = 
			1.27\%$

![3-я оптимизация](../images/opt_assem.png)
