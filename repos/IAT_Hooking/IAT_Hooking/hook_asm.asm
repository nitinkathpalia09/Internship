.code

hook_asm PROC
push rax
mov rax, CR0
and rax, 0FFFEFFFFh
mov CR0, rax
pop rax
ret 
hook_asm ENDP
END 