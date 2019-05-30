#include<stdio.h> 

#include<windows.h>



HANDLE RtlCreateUserThread(

	HANDLE hProcess,

	LPVOID lpBaseAddress,

	LPVOID lpSpace

)

{

	//The prototype of RtlCreateUserThread from undocumented.ntinternals.com

	typedef DWORD(WINAPI * functypeRtlCreateUserThread)(

		HANDLE      ProcessHandle,

		PSECURITY_DESCRIPTOR  SecurityDescriptor,

		BOOL      CreateSuspended,

		ULONG     StackZeroBits,

		PULONG     StackReserved,

		PULONG     StackCommit,

		LPVOID     StartAddress,

		LPVOID     StartParameter,

		HANDLE      ThreadHandle,

		LPVOID     ClientID

		);

	//Get handle for ntdll which contains RtlCreateUserThread
	HANDLE hRemoteThread = NULL;
	
	HMODULE hNtDllModule = GetModuleHandle(L"ntdll.dll");
	
	if (hNtDllModule == NULL)

	{

		return NULL;

	}

	functypeRtlCreateUserThread funcRtlCreateUserThread = (functypeRtlCreateUserThread)GetProcAddress(hNtDllModule, "RtlCreateUserThread");
	if (!funcRtlCreateUserThread)

	{

		return NULL;

	}

	funcRtlCreateUserThread(hProcess, NULL, 0, 0, 0, 0, lpBaseAddress, lpSpace,

		&hRemoteThread, NULL);

	DWORD lastError = GetLastError();

	return hRemoteThread;

}

HANDLE NtCreateThreadEx(

	HANDLE hProcess,

	LPVOID lpBaseAddress,

	LPVOID lpSpace

)

{

	//The prototype of NtCreateThreadEx from undocumented.ntinternals.com

	typedef DWORD(WINAPI * functypeNtCreateThreadEx)(

		PHANDLE                 ThreadHandle,

		ACCESS_MASK             DesiredAccess,

		LPVOID                  ObjectAttributes,

		HANDLE                  ProcessHandle,

		LPTHREAD_START_ROUTINE  lpStartAddress,

		LPVOID                  lpParameter,

		BOOL                    CreateSuspended,

		DWORD                   dwStackSize,

		DWORD                   Unknown1,

		DWORD                   Unknown2,

		LPVOID                  Unknown3

		);

	HANDLE                      hRemoteThread = NULL;

	HMODULE                     hNtDllModule = NULL;

	functypeNtCreateThreadEx    funcNtCreateThreadEx = NULL;

	//Get handle for ntdll which contains NtCreateThreadEx

	hNtDllModule = GetModuleHandle(L"ntdll.dll");

	if (hNtDllModule == NULL)

	{

		return NULL;

	}

	funcNtCreateThreadEx = (functypeNtCreateThreadEx)GetProcAddress(hNtDllModule, "NtCreateThreadEx");

	if (!funcNtCreateThreadEx)

	{

		return NULL;

	}

	funcNtCreateThreadEx(&hRemoteThread, GENERIC_ALL, NULL, hProcess, (LPTHREAD_START_ROUTINE)lpBaseAddress, lpSpace, FALSE, NULL, NULL, NULL, NULL);

	return hRemoteThread;

}

int injectIntoPID(int process, int method)

{

	DWORD pid = (DWORD)process;

	const char* dll2 = "C:\\DriverTest\\dllinject.dll";

	//Gets the process handle for the target process

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (OpenProcess == NULL)

	{

		puts("Could not find process\n");

	}

	//Retrieves kernel32.dll module handle for getting loadlibrary base address

	HMODULE hModule = GetModuleHandle(L"kernel32.dll");

	//Gets address for LoadLibraryA in kernel32.dll

	LPVOID lpBaseAddress = (LPVOID)GetProcAddress(hModule, "LoadLibraryA");

	if (lpBaseAddress == NULL)

	{

		puts("Unable to locate LoadLibraryA\n");

		return -1;

	}

	//Allocates space inside for inject.dll to our target process

	LPVOID lpSpace = (LPVOID)VirtualAllocEx(hProcess, NULL, strlen(dll2), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (lpSpace == NULL)

	{

		printf("Could not allocate memory in process %u\n", (int)process);

		return -1;

	}

	//Write inject.dll to memory of process

	int n = WriteProcessMemory(hProcess, lpSpace, dll2, strlen(dll2), NULL);

	if (n == 0)

	{

		puts("Could not write to process's address space\n");

		return -1;

	}


	HANDLE hThread;

	switch (method)

	{

	case 1:

		hThread = NtCreateThreadEx(hProcess, lpBaseAddress, lpSpace);

		break;

	case 2:

		hThread = RtlCreateUserThread(hProcess, lpBaseAddress, lpSpace);

		break;
	
	default:

		hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpBaseAddress, lpSpace, NULL, NULL);
		puts("loaded the Inject dll into process ");
	}

	if (hThread == NULL)

	{

		return -1;

	}

	else

	{

		DWORD threadId = GetThreadId(hThread);

		DWORD processId = GetProcessIdOfThread(hThread);

		printf("Injected thread id: %u for pid: %u", threadId, processId);

		getchar();

		getchar();

		getchar();

		CloseHandle(hProcess);

		return 0;

	}

}



int main(int argc, char* argv)

{

	int pid;

	int method;

	puts("Inject into which PID?");

	scanf_s("%u", &pid);

	puts("Which method? (0 (default): CRT, 1: NtCreateThread, 2: RtlCreateUserThread)");

	scanf_s("%u", &method);

	int result = injectIntoPID(pid, method);

	if (result == -1)

	{

		puts("Could not inject into PID");

	}

	system("pause");

}