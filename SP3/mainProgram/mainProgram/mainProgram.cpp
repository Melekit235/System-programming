#include <Windows.h>
#include <iostream>
#include "dll.h"

int main()
{
    std::cout << "Hello World!\n";
    DWORD processId = GetCurrentProcessId();

    const char strForSearch[] = "why am i doing this";
    const char strReplace[] = "for this";

    std::cout << "Befor: " << strForSearch << std::endl;
    ReplaceStr(processId, strForSearch, strReplace);
    std::cout << "After: " << strForSearch << std::endl;
}