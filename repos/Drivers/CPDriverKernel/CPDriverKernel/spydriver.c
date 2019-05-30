


#include "spydriver.h"
#define IOCTL_START_SPYING 1000
#pragma warning(disable: 4189)
#pragma warning(disable: 4101)
#pragma warning(disable: 4100)
#pragma warning(disable: 4389)
#pragma warning(disable: 4018)

const WCHAR devicename[] = L"\\Device\\Spydevice";
const WCHAR devicelink[] = L"\\DosDevices\\SPY1";

///////////////////
char savebuff[64]; KEVENT event1, event2;
long totalcount = 0, base, userbuff, userstruct;
unsigned char *replacementchunks; DWORD *functionarray;
Storage storagearray[256];
BYTE retbuff[16]; BOOLEAN ishooking;
DWORD * userstructptr; BYTE * userbuffptr;

void __stdcall Prolog(DWORD * relocptr)
{


	DWORD x; DWORD *ebpptr; int a = 0;

	RelocatedFunction * reloc = (RelocatedFunction*)relocptr[0];
	DWORD *retaddessptr = relocptr + 1;

	Storage*storptr;
	KIRQL irql = KeGetCurrentIrql();

	//find the first available Storage structure
	if (irql < DISPATCH_LEVEL)KeWaitForSingleObject(&event1, Executive, KernelMode, 0, 0);

	
	_asm {
		cli
		lea ebx, storagearray
		start : mov ecx, dword ptr[ebx]
				cmp ecx, 100
				jne fin
				add ebx, 16
				jmp start
				fin : mov dword ptr[ebx], 100
					  mov storptr, ebx
					  sti
	}
	
	//void pro_asm();

	if (irql < DISPATCH_LEVEL)KeSetEvent(&event1, 0, 0);
	//store all relevant information in it	
	_asm mov ebpptr, ebp
	storptr->retaddress = (*retaddessptr);
	storptr->ptr = reloc;
	storptr->prevEBP = ebpptr[0];

	//modify the CPU stack
	relocptr[0] = functionarray[reloc->function];
	retaddessptr[0] = (DWORD)&retbuff;

	//save the pointer to the Storage structure above the return address
	ebpptr[0] = (DWORD)storptr;

}


void  __stdcall Epilog(DWORD*retvalptr)
{

	DWORD *ebpptr;
	DWORD*retaddessptr = retvalptr + 1; DWORD retval = retvalptr[0];
	Storage*storptr; RelocatedFunction * reloc;
	DWORD i, a, b, pos, n;	KIRQL irql;

	// get the pointer to the Storage structure
	_asm mov ebpptr, ebp
	storptr = (Storage*)ebpptr[0];

	reloc = (RelocatedFunction*)storptr->ptr;

	//modify the CPU stack
	retaddessptr[0] = storptr->retaddress;
	ebpptr[0] = storptr->prevEBP;

	// mark the Storage structure as free
	storptr->isfree = 0;



	if (!ishooking)return;

	// now we are going to send data to the controller application:
	irql = KeGetCurrentIrql();
	if (irql < DISPATCH_LEVEL)KeWaitForSingleObject(&event2, Executive, KernelMode, 0, 0);

	//the assembly equivalent of the folowing lines:

	a=userstructptr[0];
	pos=(a+32)%4096;
	userstructptr[0]=pos;
	i=userbuff+pos;
	memmove(&userbuffptr[pos+6],&i,4);

	//
	
	_asm {
		cli
		mov ebx, userstructptr
		mov ecx, dword ptr[ebx]
		mov a, ecx
		add ecx, 32
		cmp ecx, 4096
		jl skip
		sub ecx, 4096
		skip: mov pos, ecx
			  mov dword ptr[ebx], ecx

			  mov ebx, userbuffptr
			  add ebx, ecx
			  add ebx, 6

			  mov edx, userbuff
			  add edx, ecx
			  mov dword ptr[ebx], edx
			  sti
	}
	


	if (irql < DISPATCH_LEVEL)KeSetEvent(&event2, 0, 0);

	// keep on filling the array with machine codes

	// instruction to spin
	userbuffptr[pos] = 0xFF; userbuffptr[pos + 1] = 0x25; i = userbuff + pos + 6;
	memmove(&userbuffptr[pos + 2], &i, 4);

	// instruction to push arguments
	userbuffptr[pos + 10] = 0x68;
	memmove(&userbuffptr[pos + 11], &reloc->function, 4);
	userbuffptr[pos + 15] = 0x68;
	memmove(&userbuffptr[pos + 16], &retval, 4);
	userbuffptr[pos + 20] = 0x68; i = userbuff + pos;
	memmove(&userbuffptr[pos + 21], &i, 4);

	//instruction to jump to the target function
	userbuffptr[pos + 25] = 0xFF; userbuffptr[pos + 26] = 0x25; i = userstruct + 4;
	memmove(&userbuffptr[pos + 27], &i, 4);

	//finally, schedule the target function for execution
	i = userbuff + pos + 10;
	memmove(&userbuffptr[a + 6], &i, 4);

}



__declspec(naked)void ProxyProlog()
{
	
	_asm {
		push eax
		push ebx
		push ecx
		push edx

		mov ebx, esp
		pushf
		add ebx, 16
		push ebx
		call Prolog

		popf
		pop edx
		pop ecx
		pop ebx
		pop eax
		ret
	}
	
	//void pp_asm();
}

__declspec(naked)void ProxyEpilog()
{
	
	_asm {
		push eax
		push ebx
		push ecx
		push edx

		mov ebx, esp
		pushf
		add ebx, 12
		push ebx
		call Epilog


		popf
		pop edx
		pop ecx
		pop ebx
		pop eax
		ret
	}
	
	//void pe_asm();
}






VOID DrvUnload(IN PDRIVER_OBJECT  DriverObject)
{


	UNICODE_STRING devlink;


	RtlInitUnicodeString(&devlink, devicelink);
	IoDeleteSymbolicLink(&devlink);

	IoDeleteDevice(DriverObject->DeviceObject);
	if (totalcount)
	{
		MmUnmapIoSpace(userbuffptr, 4096);
		MmUnmapIoSpace(userstructptr, 8);
		ExFreePool(replacementchunks); ExFreePool(functionarray);
	}
}



NTSTATUS DrvCreate(IN PDEVICE_OBJECT  devobject, IN PIRP irp)
{


	irp->IoStatus.Information = 0;
	irp->IoStatus.Status = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return 0;
}


NTSTATUS DrvClose(IN PDEVICE_OBJECT  devobject, IN PIRP irp)
{


	ULONG count; DWORD*ptr; DWORD num, thunk, x;

	count = 0; ptr = (DWORD*)savebuff;
	while (1)
	{
		if (count == totalcount)break;



		memmove(&thunk, ptr, 4); ptr++;
		memmove(&num, ptr, 4); ptr++;

		for (x = 0; x < num; x++)
		{

			DWORD*IATentryaddress = (DWORD*)(base + thunk) + x;
			memmove(IATentryaddress, &functionarray[count], 4);
			count++;

		}
	}


	ishooking = FALSE;


	irp->IoStatus.Information = 0;
	irp->IoStatus.Status = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return 0;
}
NTSTATUS DrvDispatch(IN PDEVICE_OBJECT devobject, IN PIRP irp)
{
	char*buff = NULL;
	PIO_STACK_LOCATION location;
	DWORD thunk, count, x, a;
	long num, addr;
	DWORD * ptr;
	BYTE*byteptr;
	BYTE *array = 0;
	RelocatedFunction * reloc;

	location = IoGetCurrentIrpStackLocation(irp);
	if (location->Parameters.DeviceIoControl.IoControlCode == IOCTL_START_SPYING)
	{
		buff = (char*)irp->AssociatedIrp.SystemBuffer;
		if (totalcount)
		{
			MmUnmapIoSpace(userbuffptr, 4096);
			MmUnmapIoSpace(userstructptr, 8);
			ExFreePool(replacementchunks);
			ExFreePool(functionarray);
			totalcount = 0;
		}
	}
}

NTSTATUS DrvDispatch(IN PDEVICE_OBJECT  devobject, IN PIRP irp)
{

	char*buff = NULL; PIO_STACK_LOCATION loc;
	OBJECT_HANDLE_INFORMATION  info1, info2; HANDLE evhandle1, evhandle2;
	DWORD thunk, count, x, a; long num, addr; DWORD * ptr;
	BYTE*byteptr; BYTE *array = 0; RelocatedFunction * reloc;


	loc = IoGetCurrentIrpStackLocation(irp);

	
	if (loc->Parameters.DeviceIoControl.IoControlCode == IOCTL_START_SPYING)
	{
		buff = (char*)irp->AssociatedIrp.SystemBuffer;

		//free resources that might be allocated by our previous call to DrvDispatch
		if (totalcount)
		{
			MmUnmapIoSpace(userbuffptr, 4096);
			MmUnmapIoSpace(userstructptr, 8);
			ExFreePool(replacementchunks); ExFreePool(functionarray);
			totalcount = 0;
		}

		//map the addresses of user -mode writebuff and structbuff arrays into the
		//kernel address space
		memmove(&userbuff, &buff[0], 4);
		memmove(&userstruct, &buff[4], 4);
		userbuffptr = (BYTE *)MmMapIoSpace(MmGetPhysicalAddress((void*)userbuff), 4096, FALSE);
		userstructptr = (DWORD *)MmMapIoSpace(MmGetPhysicalAddress((void*)userstruct), 8, FALSE);

		//save the remaining relevant data
		memmove(&base, &buff[8], 4);
		memmove(&totalcount, &buff[12], 4);
		memmove(&savebuff, &buff[16], 64);


		//allocate function replacement chunks
		replacementchunks = (unsigned char*)ExAllocatePool(NonPagedPool, totalcount * 16);

		//allocate the array that holds addresses of actual functions
		functionarray = (DWORD*)ExAllocatePool(NonPagedPool, totalcount * 4);



		// overwrite IAT entries
		count = 0; ptr = (DWORD*)savebuff;
		while (1)
		{
			if (count == totalcount)break;



			memmove(&thunk, ptr, 4); ptr++;
			memmove(&num, ptr, 4); ptr++;

			for (x = 0; x < num; x++)
			{

				DWORD*IATentryaddress = (DWORD*)(base + thunk) + x;
				memmove(&functionarray[count], IATentryaddress, 4);
				byteptr = &replacementchunks[count * 16];

				reloc = (RelocatedFunction *)&byteptr[6];
				byteptr[0] = 255; byteptr[1] = 21; memmove(&byteptr[2], &reloc, 4);
				reloc->function = count;

				a = (DWORD)&ProxyProlog;
				memmove(&reloc->address, &a, 4);

				a = (DWORD)byteptr; memmove(IATentryaddress, &a, 4);
				count++;

			}
		}
		ishooking = TRUE;
	}



	irp->IoStatus.Information = strlen(buff);
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


NTSTATUS
DriverEntry(
	IN PDRIVER_OBJECT  DriverObject,
	IN PUNICODE_STRING RegistryPath
)

{
	int i = 0;


	NTSTATUS status; DWORD addr = 0;
	PDEVICE_OBJECT  devobject;

	UNICODE_STRING devname; UNICODE_STRING devlink;



	DriverObject->MajorFunction[IRP_MJ_CREATE] = DrvCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DrvClose;

	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DrvDispatch;
	DriverObject->DriverUnload = DrvUnload;

	RtlInitUnicodeString(&devname, devicename);
	RtlInitUnicodeString(&devlink, devicelink);

	status = IoCreateDevice(DriverObject, 256, &devname, FILE_DEVICE_UNKNOWN, 0, 0, &devobject);
	IoCreateUnprotectedSymbolicLink(&devlink, &devname);





	KeInitializeEvent(&event1, SynchronizationEvent, TRUE);
	KeInitializeEvent(&event2, SynchronizationEvent, TRUE);

	addr = (DWORD)&retbuff[6];
	retbuff[0] = 255; retbuff[1] = 21; memmove(&retbuff[2], &addr, 4);
	addr = (DWORD)&ProxyEpilog;
	memmove(&retbuff[6], &addr, 4);

	return STATUS_SUCCESS;




}

