#include<stdio.h>
#include<Windows.h>

PVOID WINAPI GetFunctionAddress(LPCSTR DllName, LPCSTR FunctionName, BOOL LoadDll)
{
	PIMAGE_DOS_HEADER pIDH;
	PIMAGE_NT_HEADERS pINH;
	PIMAGE_EXPORT_DIRECTORY pIED;

	HMODULE hModule;
	PDWORD StartAddress;
	PWORD Ordinal;
	DWORD i;

	if (LoadDll)
	{
		hModule = LoadLibrary(DllName);

	}
	else {
		hModule = GetModuleHandle(DllName);
	}

	if (!hModule)
	{
		return NULL;
	}

	pIDH = (PIMAGE_DOS_HEADER)hModule;
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return NULL;
	}
	pINH = (PIMAGE_NT_HEADERS)((LPBYTE)hModule + pIDH->e_lfanew);
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
	{
		return NULL;
	}
	if (pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0)
	{
		return NULL;
	}
	//pIED =(PIMAGE_EXPORT_DIRECTORY)((LPBYTE)hModule+ pINH-> OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	pIED = (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)hModule + pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	Address = (PWORD)((LPBYTE)hModule + pIED->AddressOfFunctions);
	Name = (PDWORD)((LPBYTE)hModule + pIED->AddressOfNames);
	Ordinal = (PWORD)((LPBYTE)hModule + pIED->AddressOfNameOrdinals);

	for (i = 0; i < pIED->AddressOfFunctions; i++)
	{
		if (!strcmp(FunctionName, (char*)hModule + Name[i]))
		{
			return (PVOID)((LPBYTE)hModule + Address[Ordinals[i]]);
		}
	}
	return NULL;


}
int main()
{
	printf("\nExitProcess Address : %#x\n", GetFunctionAddress("kernel32.dll", "ExitProcess", FALSE));
	printf("\nNtCreateFile Address : %#x\n", GetFunctionAddress("ntdll.dll", "NtCreateFile", FALSE));
}
