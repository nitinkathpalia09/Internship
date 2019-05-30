#include "pch.h"
#include<iostream>
#include<Windows.h>
#include<string>
#include "ExlplicitDLL.h"
typedef std:: string(*Func)(const int);

int main()
{
	Func Chec;
	HINSTANCE hDLL = LoadLibrary(L"ExplicitDLL.dll");
	if (hDLL = NULL)
	{
		std::cout << "Failed to load library.\n";
		return EXIT_FAILURE;
	}
	else {
		Chec = (Func)GetProcAddress(hDLL, "Hello");
		if (!Chec)
		{
			std::cout << "\nCould not locate function";
			return EXIT_FAILURE;
		}
		int iNum = 0;
		std:: cout << "Enter a number\n";
		std::cin >> iNum;
		std:: string strMsg = Chec(iNum);
		std::cout << strMsg;
		return EXIT_SUCCESS;
	}

	FreeLibrary(hDLL);
}
