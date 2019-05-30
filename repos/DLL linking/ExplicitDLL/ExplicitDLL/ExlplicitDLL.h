#include<string>
#ifndef EVENODD_H_
#define EVENODD_H_

	#ifdef EXPORTING_DLL
		extern "C"  __declspec(dllexport) std::string Hello(const int);

	#else
		extern "C" __declspec(dllimport) std::string Hello(const int);
#endif

#endif