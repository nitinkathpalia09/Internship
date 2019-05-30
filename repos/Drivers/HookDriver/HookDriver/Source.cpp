#include "kmexts.h"

#include "Windowsx.h"

BOOLEAN KmExtsPsLoadImageNotifyRoutineRegistered;

#pragma warning(disable: 4100)
#pragma warning(disable: 4101)

VOID

KmExtsPsLoadImageNotifyRoutine(

	PUNICODE_STRING  FullImageName,

	HANDLE ProcessId,

	PIMAGE_INFO  ImageInfo)
{

	PIMAGE_INFO_EX imageInfoEx = NULL;

	PFILE_OBJECT   backingFileObject;




	LPCSTR DllPath = "C:\\Users\\WysePC\\source\\repos\\testlib\\x64\\Debug";



	// Open a handle to target process

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);



	// Allocate memory for the dllpath in the target process

	// length of the path string + null terminator

	LPVOID pDllPath = VirtualAllocEx(hProcess, 0, strlen(DllPath) + 1,

		MEM_COMMIT, PAGE_READWRITE);



	// Write the path to the address of the memory we just allocated

	// in the target process

	WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPath,

		strlen(DllPath) + 1, 0);



	// Create a Remote Thread in the target process which

	// calls LoadLibraryA as our dllpath as an argument -> program loads our dll

	HANDLE hLoadThread = CreateRemoteThread(hProcess, 0, 0,

		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"),

			"LoadLibraryA"), pDllPath, 0, 0);



	// Wait for the execution of our loader thread to finish

	WaitForSingleObject(hLoadThread, INFINITE);



	std::cout << "Dll path allocated at: " << std::hex << pDllPath << std::endl;

	std::cin.get();



	// Free the memory allocated for our dll path

	VirtualFreeEx(hProcess, pDllPath, strlen(DllPath) + 1, MEM_RELEASE);



	return 0;





	if (ImageInfo->ExtendedInfoPresent) {
		imageInfoEx = CONTAINING_RECORD(ImageInfo, IMAGE_INFO_EX, ImageInfo);

		backingFileObject = imageInfoEx->FileObject;

		if (backingFileObject != NULL) {
			DbgPrint("%wZ being loaded into "\
				"Process 0x%p. \n Backing File Object %s (0x%p)\n",
				FullImageName,
				ProcessId,
				backingFileObject != NULL ? "Available" : "Unavailable",

				backingFileObject);
			return;
		}

	}
	else {
		backingFileObject = NULL;
	}
	DbgPrint("KmExtsPsLoadImageNotifyRoutine: %wZ being loaded into "\
		"Process 0x%p. Backing File Object %s (0x%p)\n",
		FullImageName,
		ProcessId,
		backingFileObject != NULL ? "Available" : "Unavailable",
		backingFileObject);
	return;
}

extern "C"

NTSTATUS
DriverEntry(

	PDRIVER_OBJECT DriverObject,

	PUNICODE_STRING RegistryPath)

{

	NTSTATUS       status;
	UNICODE_STRING callbackName;


	UNICODE_STRING altitude;
	
	UNREFERENCED_PARAMETER(RegistryPath);
	status = PsSetLoadImageNotifyRoutine(KmExtsPsLoadImageNotifyRoutine);


	if (!NT_SUCCESS(status)) {

		DbgPrint("PsSetLoadImageNotifyRoutine failed! Status 0x%x\n",

			status);

	}

	KmExtsPsLoadImageNotifyRoutineRegistered = TRUE;

	DriverObject->DriverUnload = KmExtsUnload;

	status = STATUS_SUCCESS;
	return status;

}
VOID
KmExtsUnload(
	PDRIVER_OBJECT DriverObject)

{

	UNREFERENCED_PARAMETER(DriverObject);

	KmExtsUnregisterAllCallbacks();

	return;

}
VOID

KmExtsUnregisterAllCallbacks(

	VOID)

{

	NTSTATUS status;
	if (KmExtsPsLoadImageNotifyRoutineRegistered == TRUE) {

		status = PsRemoveLoadImageNotifyRoutine(KmExtsPsLoadImageNotifyRoutine);


		ASSERT(NT_SUCCESS(status));


	}

}

