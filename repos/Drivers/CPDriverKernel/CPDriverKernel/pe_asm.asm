.code
pe_asm PROC

push eax
push ebx
push ecx
push edx

mov ebx, esp
pushf
add ebx, 12
push ebx
call Epilog


popf
pop edx
pop ecx
pop ebx
pop eax
ret
pe_asm ENDP
END