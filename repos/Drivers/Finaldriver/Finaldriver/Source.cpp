#include "kmexts.h"

#pragma warning(disable: 4273)
BOOLEAN KmExtsPsLoadImageNotifyRoutineRegistered;

#pragma warning(disable: 4100)
#pragma warning(disable: 4101)
extern "C" {
	void GetProcName(
		HANDLE PID,
		PUNICODE_STRING ProcessName
	);
	char *GetProcessNameFromPid(HANDLE pid);
	extern UCHAR *PsGetProcessImageFileName(IN PEPROCESS Process);
	extern NTSTATUS PsLookupProcessByProcessId(
		HANDLE ProcessId,
		PEPROCESS *Process);
	typedef PCHAR(*GET_PROCESS_IMAGE_NAME)(PEPROCESS Process);
	GET_PROCESS_IMAGE_NAME gGetProcessImageFileName;
	char *GetProcessNameFromPid(HANDLE pid)
	{
		PEPROCESS Process;
		if (PsLookupProcessByProcessId(pid, &Process) == STATUS_INVALID_PARAMETER)
		{
			return "pid??";
		}
		return  (CHAR*)PsGetProcessImageFileName(Process);
	}
}

VOID

KmExtsPsLoadImageNotifyRoutine(

	PUNICODE_STRING  FullImageName,

	HANDLE ProcessId,

	PIMAGE_INFO  ImageInfo)
{

	PIMAGE_INFO_EX imageInfoEx = NULL;

	PFILE_OBJECT   backingFileObject;

	if (ImageInfo->ExtendedInfoPresent) {
		imageInfoEx = CONTAINING_RECORD(ImageInfo, IMAGE_INFO_EX, ImageInfo);

		backingFileObject = imageInfoEx->FileObject;

		if (backingFileObject != NULL) {
			DbgPrint("%wZ being loaded into "\
				"Process [%s] whose Id is (0x%p).\n Backing File Object %s (0x%p)\n", 
				FullImageName,
				GetProcessNameFromPid(ProcessId),
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

