
#include <windows.h>
#include "DllParams.h"

#pragma warning(disable: 4086)

INT main(PDLL_PARAMS DllParams)
{
    if (DllParams == NULL ||
        DllParams->Kernel32Address == NULL ||
        DllParams->Kernel32Size == 0)
        return 0;
    return 1;
}
