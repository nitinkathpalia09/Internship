#include<ntddk.h>

#pragma warning(disable: 4100)
#pragma warning(disable: 4189)


NTSTATUS DriverEntry(
	PDRIVER_OBJECT DriverObject,
	PUNICODE_STRING RegistryPath
);
void LoadImageNotifyRoutine(IN PUNICODE_STRING FullImageName,
	IN HANDLE ProcessId, IN PIMAGE_INFO ImageInfo)
{
	if (!FullImageName)
		return;
	if (!ProcessId || ImageInfo->SystemModeImage)
		return;
	return;
}
VOID ProcessCallback(
	HANDLE hParentId,
	HANDLE hProcessId,
	BOOLEAN bCreate 
);
void GetProcName(
	HANDLE PID,
	PUNICODE_STRING ProcName
);
/*
VOID ProcessCallback(
	HANDLE hParentId,
	HANDLE hProcessId,
	BOOLEAN bCreate
) {
	if (bCreate)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL , "Process %u is created", PsSetLoadImageNotifyRoutine(&LoadImageNotifyRoutine));
	}
	else {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Process %u is terminated", PsSetLoadImageNotifyRoutine(&LoadImageNotifyRoutine));
	}
}
*/
char *GetProcessNameFromPid(HANDLE pid);
extern UCHAR *PsGetProcessImageFileName(IN PEPROCESS Process);

extern NTSTATUS PsLookupProcessByProcessId(
	HANDLE ProcessId,
	PEPROCESS *Process
);
typedef PCHAR(*GET_PROCESS_IMAGE_NAME)(PEPROCESS Process);
GET_PROCESS_IMAGE_NAME gGetProcessImageFileName;

char *GetProcessNameFromPid(HANDLE pid)
{
	PEPROCESS Process;
	if (PsLookupProcessByProcessId(pid, &Process) == STATUS_INVALID_PARAMETER)
	{
		return "pid??";
	}
	return (CHAR*)PsGetProcessImageFileName(Process);
}
VOID ProcessCallback(
	HANDLE hParentId,
	HANDLE hProcessId,
	BOOLEAN bCreate
)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	
	PUNICODE_STRING ProcName = NULL;
	
	if (bCreate)
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Process [%s] is loaded\n", PsSetLoadImageNotifyRoutine(&LoadImageNotifyRoutine)); 
	
	}
	else {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Process [%s] is terminated\n", GetProcessNameFromPid(hProcessId));
	}
}
void DriverUnload(PDRIVER_OBJECT pDriverObject) {
	PsSetCreateProcessNotifyRoutine(ProcessCallback, TRUE);
	//PsRemoveLoadImageNotifyroutine(LoadImageNotifyRoutine);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Driver Unloading\n");

}
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Driver Load\n");
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "----------------------\n");
	PsSetCreateProcessNotifyRoutine(&ProcessCallback, FALSE);
	return STATUS_SUCCESS;
}