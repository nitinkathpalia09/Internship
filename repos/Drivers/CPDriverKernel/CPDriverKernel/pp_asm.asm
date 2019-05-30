.code
pp_asm PROC

push eax
		push ebx
		push ecx
		push edx

		mov ebx, esp
		pushf
		add ebx, 16
		push ebx
		call Prolog

		popf
		pop edx
		pop ecx
		pop ebx
		pop eax
		ret
		pp_asm ENDP
		END