// ExplicitDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#define EXPORTING_DLL
#include "ExlplicitDLL.h"

extern  "C"
{
	__declspec(dllexport) std::string Hello(const int iNum)
	{
		std::string strRes = "NULL";
		if (iNum % 2 == 0)
		{
			strRes = "number is Even";

		}
		else {
			strRes = "number is Odd";
		}
		return strRes;
	}
}