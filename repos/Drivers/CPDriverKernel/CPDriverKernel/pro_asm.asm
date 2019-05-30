.code 
pro_asm PROC
cli
		lea ebx, storagearray
		start : mov ecx, dword ptr[ebx]
				cmp ecx, 100
				jne fin
				add ebx, 16
				jmp start
				fin : mov dword ptr[ebx], 100
					  mov storptr, ebx
					  sti
					  pro_asm ENDP
					  END