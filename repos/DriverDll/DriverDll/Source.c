
#include <windows.h>



#include <stdio.h>
#pragma warning(disable: 4007)
#pragma warning(disable: 4047)
#pragma warning(disable: 4024)
#pragma warning(disable: 4101)
#pragma warning(disable: 4100)
#pragma warning(disable: 4018)
#pragma warning(disable: 4189)


void HookFunction(char* funcName, LPDWORD function);

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



BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)

{

	if (dwReason == DLL_PROCESS_ATTACH)

	{
		
		
		HookFunction("CreateProcessA", (LPDWORD)&CreateProcessA);

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
	OutputDebugString(L"Process is Hooked!");
	return 1;

}



LPDWORD FoundIAT(char* funcName)

{

	DWORD test = 0;



	LPVOID pMapping = GetModuleHandle(NULL);

	if (pMapping == NULL)

		return 0;



	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)pMapping;



	if (DosHeader->e_magic != IMAGE_DOS_SIGNATURE)

		return 0;



	PIMAGE_NT_HEADERS NtHeaders = (PIMAGE_NT_HEADERS)((char*)DosHeader + DosHeader->e_lfanew);



	if (NtHeaders->Signature != IMAGE_NT_SIGNATURE)

		return 0;



	PIMAGE_DATA_DIRECTORY DataDirectory = &NtHeaders->OptionalHeader.DataDirectory[1]; // Import symbols

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