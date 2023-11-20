#include <Windows.h>
#include <iostream>
#include "dll.h"

typedef void __stdcall TDynamicReplace(DWORD, const char*, const char*);

void ReplaceStrDynamic(DWORD processId, const char* strForSearch, const char* strReplace);

int main()
{
    DWORD processId = GetCurrentProcessId();

    const char strForSearch[] = "why am i doing this";
    const char strReplace[] = "for this";

    
    char method = '1';
    //std::cin >> method;
    std::cout << "Befor: " << strForSearch << std::endl;
    switch (method)
    {
    case '1':
        ReplaceStr(processId, strForSearch, strReplace);
        break;
    case '2':
        ReplaceStrDynamic(processId, strForSearch, strReplace);
        break;
    case '3':
        ReplaceStrDynamic(processId, strForSearch, strReplace);
        break;
    default:
        break;
    }    
    std::cout << "After: " << strForSearch << std::endl;
}

void ReplaceStrDynamic(DWORD processId, const char* strForSearch, const char* strReplace)
{
    HINSTANCE hLibrary = LoadLibrary(TEXT("DllReplace.dll"));

    if (hLibrary != NULL)
    {
        TDynamicReplace* DynamicReplace = (TDynamicReplace*)GetProcAddress(hLibrary, "ReplaceStr");
        if (DynamicReplace != NULL)
        {
            DynamicReplace(processId, strForSearch, strReplace);
        }
        FreeLibrary(hLibrary);
    }
}

