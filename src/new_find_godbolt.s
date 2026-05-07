TableFind(Table*, char*):
        test    rdi, rdi
        je      .L12
        push    rbp
        mov     rbp, rsi
        push    rbx
        sub     rsp, 8
        test    rsi, rsi
        je      .L2
        mov     rbx, rdi
        mov     esi, DWORD PTR [rdi+4]
        mov     rdi, rbp
        call    [QWORD PTR [rbx+8]]
        mov     rdx, QWORD PTR [rbx+16]
        cdqe
        mov     rbx, QWORD PTR [rdx+rax*8]
        test    rbx, rbx
        je      .L2
.L5:
        mov     rcx, QWORD PTR [rbx]
        .intel_syntax noprefix              
        mov     rsi,      rcx               
        mov     rdi,      rbp               
        mov     al,             BYTE PTR [rsi]      
        mov     dl,             BYTE PTR [rdi]      
        cmp     al,             dl                      
        je          .eq                                   
        jmp     .L3                 
.eq:                                                    

        mov     rsi, rbp
        mov     rdi, rcx
        call    strcmp
        test    eax, eax
        je      .L6
.L3:
        mov     rbx, QWORD PTR [rbx+8]
        test    rbx, rbx
        jne     .L5
.L2:
        add     rsp, 8
        xor     eax, eax
        pop     rbx
        pop     rbp
        ret
.L6:
        add     rsp, 8
        mov     eax, 1
        pop     rbx
        pop     rbp
        ret
.L12:
        xor     eax, eax
        ret