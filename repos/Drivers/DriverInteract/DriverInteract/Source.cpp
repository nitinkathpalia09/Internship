#include "kmexts.h"
#define FILE_DEVICE_NOTIFYDEVICE 0x00008337
#define IOCTL_SAYHELLO (ULONG) CTL_CODE( FILE_DEVICE_NOTIFYDEVICE, 0x00, METHOD_BUFFERED, FILE_ANY_ACCESS)
#pragma warning(disable: 4273)
#pragma warning(disable: 4706)
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

NTSTATUS DriverDispatch(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
);
extern "C"

NTSTATUS
DriverEntry(

	PDRIVER_OBJECT DriverObject,

	PUNICODE_STRING RegistryPath)

{

	NTSTATUS       status;
	UNICODE_STRING callbackName;
	WCHAR deviceNameBuffer[] = L"\\Device\\NotifyDevice";
	UNICODE_STRING deviceNameUnicodeString;
	WCHAR deviceLinkBuffer[] = L"DOSDevices\\NotifyDevice";
	UNICODE_STRING deviceLinkUnicodeString;
	PDEVICE_OBJECT interfaceDevice = NULL;
	UNICODE_STRING altitude;


	RtlInitUnicodeString(&deviceNameUnicodeString, deviceNameBuffer);
	status = IoCreateDevice(DriverObject,
		0,
		&deviceNameUnicodeString,
		FILE_DEVICE_NOTIFYDEVICE,
		0, 
		TRUE,
		&interfaceDevice
	);
	if (NT_SUCCESS(status)) {
		RtlInitUnicodeString(&deviceLinkUnicodeString, deviceLinkBuffer);
		status = IoCreateSymbolicLink(
			&deviceLinkUnicodeString,
			&deviceNameUnicodeString
		);

		DriverObject->MajorFunction[IRP_MJ_CREATE] =
			DriverObject->MajorFunction[IRP_MJ_CLOSE] =
			DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] =
			DriverDispatch;
	}


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
NTSTATUS DriverDispatch(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp
)
{
	PIO_STACK_LOCATION iosp;
	ULONG ioControlCode;
	NTSTATUS status;

	iosp = IoGetCurrentIrpStackLocation(Irp);
	switch (iosp->MajorFunction) {
	case IRP_MJ_CREATE:
		status = STATUS_SUCCESS;
		break;
	case IRP_MJ_CLOSE:
		status = STATUS_SUCCESS;
		break;
	case IRP_MJ_DEVICE_CONTROL:
		ioControlCode = 
			iosp->Parameters.DeviceIoControl.IoControlCode;
		if (ioControlCode = IOCTL_SAYHELLO) {
			DbgPrint("Hello World\n");
		}
		status = STATUS_SUCCESS;
		break;
	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DriverDispatch(IN PDRIVER_OBJECT DriverObject,
	IN PIRP Irp)
{

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

