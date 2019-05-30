// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Windows.h>
#include<string.h>
#include <stdio.h>

void HookFunction(const char* funcName, LPDWORD function);
LPDWORD FoundIAT(char* funcName);

int WINAPI HookCreateProcessA(LPCSTR                lpApplicationName,
	LPSTR                 lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCSTR                lpCurrentDirectory,
	LPSTARTUPINFOA        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		
		MessageBox(NULL, L"Injected with success!", L"Hello", NULL);
		HookFunction("A", (LPDWORD)&CreateProcessA);

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void HookFunction(char* funcName, LPDWORD function)
{
	LPDWORD pOldFunction = FoundIAT(funcName);
	DWORD accessProtectionValue, accessProtec;
	int vProtect = VirtualProtect(pOldFunction, sizeof(LPDWORD), PAGE_EXECUTE_READWRITE, &accessProtectionValue);
	*pOldFunction = (DWORD)function;
	vProtect = VirtualProtect(pOldFunction, sizeof(LPDWORD), accessProtectionValue, &accessProtec);

}

int WINAPI HookCreateProcessA(LPCSTR                lpApplicationName,
	LPSTR                 lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCSTR                lpCurrentDirectory,
	LPSTARTUPINFOA        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation)
{
	return MessageBoxA(NULL, "CreateProcessHooked", "Hooked", NULL);

}
LPDWORD FoundIAT(char* funcName)
{
	DWORD test = 0;
	LPVOID pMapping = GetModuleHandle(NULL);
	if (pMapping == NULL)
	{
		return;


	}
	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)pMapping;
	if (DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return;
	}
	PIMAGE_NT_HEADERS NtHeaders = (PIMAGE_NT_HEADERS)((char*)DosHeader + DosHeader->e_lfanew);
	if (NtHeaders->Signature != IMAGE_NT_SIGNATURE)
		return;
	PIMAGE_DATA_DIRECTORY DataDirectory = &NtHeaders->OptionalHeader.DataDirectory[1];

	PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((char*)DosHeader + DataDirectory->VirtualAddress);
	PIMAGE_THUNK_DATA32 OriginalFirstThunk = (PIMAGE_THUNK_DATA32)((char*)DosHeader + ImportDescriptor->OriginalFirstThunk);
	while (OriginalFirstThunk != 0)
	{
		DWORD name = (DWORD)((char*)pMapping + ImportDescriptor->Name);
		OriginalFirstThunk = (PIMAGE_THUNK_DATA32)((char*)DosHeader + ImportDescriptor->OriginalFirstThunk);

		PIMAGE_THUNK_DATA32 FirstThunk = (PIMAGE_THUNK_DATA32)((char*)DosHeader + ImportDescriptor->FirstThunk);

		while (OriginalFirstThunk->u1.AddressOfData != 0)
		{
			PIMAGE_IMPORT_BY_NAME NameImg = (PIMAGE_IMPORT_BY_NAME)((char*)DosHeader + (DWORD)OriginalFirstThunk->u1.AddressOfData);

			test = (DWORD)OriginalFirstThunk->u1.Function & (DWORD)IMAGE_ORDINAL_FLAG32;

			if (test == 0)
			{
				if (strcmp(funcName, (const char*)NameImg->Name) == 0)
				{
					MessageBox(NULL, (LPCTSTR)NameImg->Name, L"", NULL);
					return (LPDWORD)&(FirstThunk->u1.Function);
				}
			}
			OriginalFirstThunk++;
			FirstThunk++;
		}
		ImportDescriptor++;
	}
	return 0;
}