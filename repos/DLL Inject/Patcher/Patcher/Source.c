#include <windows.h>
#include "stdafx.h"

#include "DllParams.h"

#pragma warning (disable: 4007)
#pragma warning (disable: 4047)
#pragma warning (disable: 4024)
#pragma warning (disable: 4211)
#pragma warning (disable: 4013)

static void CustomFunction()
{
	MessageBox(NULL, L"HelloWorld", L"CreateProcess called!", NULL);
}
static FARPROC MyCreateProcess(HMODULE hModule, LPCSTR lpProcName)
{
	if (strcmp(lpProcName, "Init") == 0)
		return (FARPROC)CustomFunction;
	return MyCreateProcess(hModule, lpProcName);
}
static BOOL HackGetProcAddress()
{
	HMODULE module = GetModuleHandle(L"kernel32.dll");
	if (module == NULL)
		return FALSE;
	ULONG size;
	PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToDataEx(module, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size, NULL);
	BOOL Found = FALSE;
	while (importDescriptor->Characteristics && importDescriptor->Name)
	{
		PSTR importName = (PSTR)((PBYTE)module + importDescriptor->Name);
		if (_stricmp(importName, "kernel32.dll") == 0) {
			Found = TRUE;
			break;
		}
		importDescriptor++;
	}
	if (!Found)
		return FALSE;
	PROC baseGetProcAddress = (PROC)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "CreateProcessA");
	PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)((PBYTE)module + importDescriptor->FirstThunk);

	while (thunk->u1.Function)
	{
		PROC* funcStorage = (PROC*)&thunk->u1.Function;

		if (*funcStorage == baseGetProcAddress) {
			MEMORY_BASIC_INFORMATION mbi;
			VirtualQuery(funcStorage, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
			if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &mbi.Protect))
				return FALSE;
			*funcStorage = (PROC)MyCreateProcess;

			DWORD dwOldProtect;
			VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwOldProtect);
			//VirtualAllocEx(HANDLE hProcess, LPVOID lpAddress,SIZE_T dwSize, DWORD  fwAllocationType, DWORD flProtect);
			return TRUE;

		}
		thunk++;

	}
	return FALSE;
}
INT main(PDLL_PARAMS DllParams)

{

	if (DllParams == NULL ||

		DllParams->Kernel32Address == NULL ||

		DllParams->Kernel32Size == 0)
	{

		return 0;
	}
	CustomFunction();

}