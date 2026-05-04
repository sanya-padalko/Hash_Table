## -O2:

| Номер замера | Среднее количество тиков |
| :---: | :---: |
| 1 | 440 |
| 2 | 446 |
| 3 | 447 |
| 4 | 460 |
| 5 | 461 |
| 6 | 468 |
| 7 | 472 |

Результаты без минимума и максимума: $446$, $447$, $460$, $461$, $468$

Среднее количество тиков на <i>TableFind</i>: $456 
													\pm 11$

Относительная погрешность: $2.45\%$

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
| 1 | 405 |
| 2 | 412 |
| 3 | 413 |
| 4 | 414 |
| 5 | 415 |
| 6 | 418 |
| 7 | 422 |

Результаты без минимума и максимума: $412$, $413$, $414$, $415$, $418$

Среднее количество тиков на <i>TableFind</i>: $414 
													\pm 5$

Относительная погрешность: $1.18\%$

Получили ускорение на $\frac{456 - 414}{456} * 100 =
			9.21\% \pm 0.25$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{2.45^2+1.18^2} =
			2.72\%$

Абсолютная погрешность: $9.21\% * 0.0272 = 
			0.25\%$

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
| 1 | 384 |
| 2 | 385 |
| 3 | 389 |
| 4 | 389 |
| 5 | 392 |
| 6 | 392 |
| 7 | 398 |

Результаты без минимума и максимума: $385$, $389$, $389$, $392$, $392$

Среднее количество тиков на <i>TableFind</i>: $389 
													\pm 4$

Относительная погрешность: $1.13\%$

Полученное ускорение:

$\cdot$ на $\frac{414 - 389}{414} * 100 =
			6.04\% \pm 0.10$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{1.18^2+1.13^2} =
			1.63\%$

Абсолютная погрешность: $6.04\% * 0.0163 = 
			0.10\%$


$\cdot$ на $\frac{456 - 389}{456} * 100 =
			14.69\% \pm 0.40$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{2.45^2+1.13^2} =
			2.70\%$

Абсолютная погрешность: $14.69\% * 0.0270 = 
			0.40\%$

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
| 1 | 351 |
| 2 | 356 |
| 3 | 357 |
| 4 | 357 |
| 5 | 358 |
| 6 | 360 |
| 7 | 361 |

Результаты без минимума и максимума: $356$, $357$, $357$, $358$, $360$

Среднее количество тиков на <i>TableFind</i>: $358 
													\pm 3$

Относительная погрешность: $0.85\%$

Полученное ускорение:

$\cdot$ на $\frac{389 - 358}{389} * 100 =
			7.97\% \pm 0.11$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{1.13^2+0.85^2} =
			1.41\%$

Абсолютная погрешность: $7.97\% * 0.0141 = 
			0.11\%$


$\cdot$ на $\frac{456 - 358}{456} * 100 =
			21.49\% \pm 0.56$ $\%$ относительно -O3.

Относительная погрешность ускорения: 
			$\sqrt{2.45^2+0.85^2} =
			2.59\%$

Абсолютная погрешность: $21.49\% * 0.0259 = 
			0.56\%$

![3-я оптимизация](../images/opt_assem.png)
