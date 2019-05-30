#ifndef __KMEXTS_H__
#define __KMEXTS_H__

extern "C" {
#include <ntifs.h>
}

////////////////
// KMEXTS.CPP //
////////////////
extern "C" DRIVER_INITIALIZE DriverEntry;

DRIVER_UNLOAD KmExtsUnload;

VOID
KmExtsUnregisterAllCallbacks(
	VOID
);


///////////////////// 
// CMCALLBACKS.CPP //
/////////////////////

typedef struct _CM_CALLBACK_CONTEXT {
	ULONG          MagicNumber;
	LARGE_INTEGER  CallbackRegistrationCookie;
	BOOLEAN        CallbackRegistered;
}CM_CALLBACK_CONTEXT, *PCM_CALLBACK_CONTEXT;

#define CM_CALLBACK_CONTEXT_MAGIC   0xCCCC0508

extern CM_CALLBACK_CONTEXT KmExtsCmCallbackContext;

NTSTATUS
KmExtsCmRegistryCallback(
	_In_ PVOID CallbackContext,
	_In_ PVOID Argument1,
	_In_ PVOID Argument2
);


///////////////////// 
// EXCALLBACKS.CPP //
/////////////////////

typedef struct _EX_CALLBACK_CONTEXT {
	ULONG            MagicNumber;
	PCALLBACK_OBJECT ExCallbackObject;
	PVOID            RegistrationHandle;
	BOOLEAN          CallbackRegistered;
}EX_CALLBACK_CONTEXT, *PEX_CALLBACK_CONTEXT;

#define EX_CALLBACK_CONTEXT_MAGIC   0x03051980

extern EX_CALLBACK_CONTEXT KmExtsExSetTimeCbContext;

CALLBACK_FUNCTION KmExtsExSetSystemTimeCallback;

extern EX_CALLBACK_CONTEXT KmExtsExPowerStateCbContext;

CALLBACK_FUNCTION KmExtsExPowerStateCallback;

extern EX_CALLBACK_CONTEXT KmExtsExProcAddCbContext;

CALLBACK_FUNCTION KmExtsExProcessorAddCallback;

NTSTATUS
KmExtsCreateAndRegisterCallback(
	_In_ PUNICODE_STRING CallbackName,
	_In_ PCALLBACK_FUNCTION CallbackFunction,
	_In_ PEX_CALLBACK_CONTEXT CallbackContext
);


///////////////////// 
// OBCALLBACKS.CPP //
/////////////////////

#if (NTDDI_VERSION >= NTDDI_VISTA)


// 
// We'll register for notification of every possible type of
// object. Currently only threads and processes
// 
#define KMEXTS_PROCESS_OBJECT_TYPE                 0
#define KMEXTS_THREAD_OBJECT_TYPE                  1
#define KMEXTS_MAX_OBJECT_TYPE                     2

typedef struct _OB_CALLBACK_CONTEXT {
	ULONG            MagicNumber;
	PVOID            RegistrationHandle;
	BOOLEAN          CallbackRegistered;
}OB_CALLBACK_CONTEXT, *POB_CALLBACK_CONTEXT;

#define OB_CALLBACK_CONTEXT_MAGIC   0x12311999

extern OB_CALLBACK_CONTEXT KmExtsObCbContext;

OB_PREOP_CALLBACK_STATUS
KmExtsObPreCallback(
	_In_ PVOID RegistrationContext,
	_In_ POB_PRE_OPERATION_INFORMATION OperationInformation
);

VOID
KmExtsObPostCallback(
	_In_ PVOID RegistrationContext,
	_In_ POB_POST_OPERATION_INFORMATION OperationInformation
);
#endif

///////////////////// 
// PSCALLBACKS.CPP //
/////////////////////


extern BOOLEAN KmExtsPsCreateProcessNotifyRoutineRegistered;

VOID
KmExtsPsCreateProcessNotifyRoutine(
	_In_ HANDLE ParentId,
	_In_ HANDLE ProcessId,
	_In_ BOOLEAN Create
);

#if (NTDDI_VERSION >= NTDDI_VISTA)

extern BOOLEAN KmExtsPsCreateProcessExNotifyRoutineRegistered;

VOID
KmExtsPsCreateProcessNotifyRoutineEx(
	_Inout_ PEPROCESS Process,
	_In_ HANDLE ProcessId,
	_In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN10_RS2)

extern BOOLEAN KmExtsPsCreateProcessEx2NotifyRoutineRegistered;

VOID
KmExtsPsCreateProcessNotifyRoutineEx2(
	_Inout_ PEPROCESS Process,
	_In_ HANDLE ProcessId,
	_In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
);
#endif

extern BOOLEAN KmExtsPsCreateThreadNotifyRoutineRegistered;

VOID
KmExtsPsCreateThreadNotifyRoutine(
	_In_ HANDLE ProcessId,
	_In_ HANDLE ThreadId,
	_In_ BOOLEAN Create
);

#if (NTDDI_VERSION >= NTDDI_WINTHRESHOLD)
extern BOOLEAN KmExtsPsCreateThreadNotifyRoutineExRegistered;

VOID
KmExtsPsCreateThreadNotifyRoutineEx(
	_In_ HANDLE ProcessId,
	_In_ HANDLE ThreadId,
	_In_ BOOLEAN Create
);
#endif

extern BOOLEAN KmExtsPsLoadImageNotifyRoutineRegistered;

VOID
KmExtsPsLoadImageNotifyRoutine(
	_In_ PUNICODE_STRING FullImageName,
	_In_ HANDLE ProcessId,
	_In_ PIMAGE_INFO ImageInfo
);




#endif __KMEXTS_H__