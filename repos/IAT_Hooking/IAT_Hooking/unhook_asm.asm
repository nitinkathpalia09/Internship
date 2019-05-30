
.code
PROC: unhook_asm 
unhook_asm PROC
push rax
mov rax, CR0
mov rbx, NOT 0FFFEFFFFh
or rax,  rbx
mov CR0, rax
pop rax
ret 
unhook_asm ENDP
END 