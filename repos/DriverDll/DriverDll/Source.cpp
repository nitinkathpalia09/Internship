
#include <windows.h>

#include "DllParams.h"



INT main(PDLL_PARAMS DllParams)

{

	if (DllParams == NULL ||

		DllParams->Kernel32Address == NULL ||

		DllParams->Kernel32Size == 0)

		MessageBox(NULL, L"Hello World!", L"Hello World!", NULL);

	return 1;

}