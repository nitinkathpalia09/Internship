// GetPID.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include<Windows.h>
#include<tlhelp32.h>
#include <stdio.h>


#include "pch.h"

DWORD GetParentProcessID(DWORD dwProcessID)
{
	DWORD dwParentProcessID = -1;
	HANDLE			hProcessSnapshot;
	PROCESSENTRY32	processEntry32;

	hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnapshot != INVALID_HANDLE_VALUE)
	{
		processEntry32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnapshot, &processEntry32))
		{
			do
			{
				if (dwProcessID == processEntry32.th32ProcessID)
				{
					dwParentProcessID = processEntry32.th32ParentProcessID;
					break;
				}
			} while (Process32Next(hProcessSnapshot, &processEntry32));

			CloseHandle(hProcessSnapshot);
		}
	}

	return dwParentProcessID;
}