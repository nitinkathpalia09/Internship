#include <ntddk.h>
#include<ntifs.h>
#include<Windowsx.h>

NTSTATUS HookedMjCreate(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION      irpStack;
	ULONG                   ioTransferType;

	// Get a pointer to the current location in the IRP. This is where
	// the function codes and parameters are located.

	irpStack = IoGetCurrentIrpStackLocation(Irp);
	switch (irpStack->MajorFunction)
	{
	case IRP_MJ_CREATE:

		ioTransferType = irpStack->Parameters.DeviceIoControl.IoControlCode;
		ioTransferType &= 3;

		// Filter only files containing _root_
		if (irpStack->FileObject != NULL && irpStack->FileObject->FileName.Length > 0 && wcsstr(irpStack->FileObject->FileName.Buffer, L"_root_") != NULL)
		{
			DbgPrint("[HOOK] File: %ws\n", irpStack->FileObject->FileName.Buffer);

			// Need to know the method to find input buffer
			if (ioTransferType == METHOD_BUFFERED)
			{
				// Call our completion routine if IRP succeeds.
				// To do this, change the Control flags in the IRP.
				irpStack->Control = 0;
				irpStack->Control |= SL_INVOKE_ON_SUCCESS;

				// Save old completion routine if present
				irpStack->Context = (PIO_COMPLETION_ROUTINE)ExAllocatePool(NonPagedPool, sizeof(REQINFO));
				((PREQINFO)irpStack->Context)->OldCompletion = irpStack->CompletionRoutine;

				// Setup our function to be called
				// upon completion of the IRP
				irpStack->CompletionRoutine = (PIO_COMPLETION_ROUTINE)IoCompletionRoutine;
			}
		}
		break;

	default:
		break;
	}

	// Call the original function
	return OldIrpMj(DeviceObject, Irp);
}

// The CompletionRoutine is called only if we found a file to deny access
NTSTATUS IoCompletionRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context)
{
	PVOID OutputBuffer;
	DWORD NumOutputBuffers;
	PIO_COMPLETION_ROUTINE p_compRoutine;
	DWORD i;

	OutputBuffer = Irp->UserBuffer;
	p_compRoutine = ((PREQINFO)Context)->OldCompletion;
	DbgPrint("Completion routine reached! Deny access.\n");
	ExFreePool(Context);

	// We deny access by returning an ERROR code
	Irp->IoStatus.Status = STATUS_NOT_FOUND;

	// Call next completion routine (if any)
	if ((Irp->StackCount > (ULONG)1) && (p_compRoutine != NULL))
		return (p_compRoutine)(DeviceObject, Irp, NULL);
	else
		return Irp->IoStatus.Status;
}
	
NTSTATUS status = STATUS_SUCCESS;