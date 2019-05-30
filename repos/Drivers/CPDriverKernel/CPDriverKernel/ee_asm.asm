.code 
ee_asm PROC
cli
		mov ebx, userstructptr
		mov ecx, dword ptr[ebx]
		mov a, ecx
		add ecx, 32
		cmp ecx, 4096
		jl skip
		sub ecx, 4096
		skip: mov pos, ecx
			  mov dword ptr[ebx], ecx

			  mov ebx, userbuffptr
			  add ebx, ecx
			  add ebx, 6

			  mov edx, userbuff
			  add edx, ecx
			  mov dword ptr[ebx], edx
			  sti
			   ee_asm ENDP
			   END