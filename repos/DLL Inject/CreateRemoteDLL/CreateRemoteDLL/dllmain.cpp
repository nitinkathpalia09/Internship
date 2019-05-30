// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include<stdio.h>
#include<Windows.h>


BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
	MessageBox(NULL, "Attached", NULL, NULL);
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		MessageBox(NULL, "Attached", NULL, NULL);
		break;
	case DLL_PROCESS_DETACH:
		MessageBox(NULL, "Attached", NULL, NULL);
		break;
	case DLL_THREAD_ATTACH:
		MessageBox(NULL, "Attached", NULL, NULL);
		break;
	case DLL_THREAD_DETACH:
		MessageBox(NULL, "Attached", NULL, NULL);
		break;
	}
	MessageBox(NULL, "Attached", NULL, NULL);
	return TRUE;
}
