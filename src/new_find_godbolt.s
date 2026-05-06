TableFind(Table*, char*):
        test    rdi, rdi
        je      .L14
        push    rbp
        mov     rbp, rdi
        push    rbx
        mov     rbx, rsi
        sub     rsp, 8
        mov     esi, DWORD PTR [rdi+4]
        mov     rdi, rbx
        call    [QWORD PTR [rbp+8]]
        mov     rdx, QWORD PTR [rbp+16]
        cdqe
        mov     rcx, QWORD PTR [rdx+rax*8]
        test    rcx, rcx
        jne     .L4
        jmp     .L2
.L18:
        mov     rcx, QWORD PTR [rcx+8]
        test    rcx, rcx
        je      .L2
.L4:
        mov     r8, QWORD PTR [rcx]
        mov     rsi,      r8                      
        mov     rdi,      rbx               
        mov     al,             BYTE PTR [rsi]          
        mov     dl,             BYTE PTR [rdi]          
        cmp     al,             dl              
        jne     .neq                                  
        inc     rsi                                 
        inc     rdi                                 
        call    my_strcmp                         
        jmp     .end                                  
.neq:                                     
        mov     eax,      1               
.end:                                     
        mov     r8d,          eax           

        test    r8d, r8d
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