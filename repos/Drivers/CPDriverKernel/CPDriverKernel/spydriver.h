


#include "ntddk.h"
#include <stdio.h>
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;


typedef struct tagRelocatedFunction {
	LONG address;
	LONG function;
} RelocatedFunction, *PRelocatedFunction;


typedef struct tagStorage {

	DWORD isfree;
	DWORD retaddress;
	DWORD prevEBP;
	RelocatedFunction* ptr;

}Storage, *PStorage;
