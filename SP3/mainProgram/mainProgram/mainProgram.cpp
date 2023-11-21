#include <Windows.h>
#include <iostream>
#include "dll.h"

#include <tlhelp32.h>

struct ParamsForInject {
    char left[255];
    char right[255];
};

typedef void __stdcall TDynamicReplace(DWORD, const char*, const char*);

void ReplaceStrDynamic(DWORD processId, const char* strForSearch, const char* strReplace);
void ReplaceStrInject(const char* strForSearch, const char* strReplace);
DWORD GetProcessId();

int main()
{
    DWORD processId = GetCurrentProcessId();

    const char strForSearch[] = "why";
    const char strReplace[] = "for this";

    
    char method = '3';
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
        ReplaceStrInject(strForSearch, strReplace);
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

void ReplaceStrInject(const char* strForSearch, const char* strReplace)
{
    DWORD processId = GetProcessId();

    Params ParamsForInject;
    strcpy_s(ParamsForInject.left, strForSearch);
    strcpy_s(ParamsForInject.right, strReplace);

    HANDLE hProcessForInject = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

    LPVOID lpProcess = VirtualAllocEx(hProcessForInject, NULL, lstrlenA("DllReplace.dll") + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    LPVOID lpParams = VirtualAllocEx(hProcessForInject, NULL, sizeof(Params), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    WriteProcessMemory(hProcessForInject, lpProcess, "DllReplace.dll", lstrlenA("DllReplace.dll") + 1, NULL);
    WriteProcessMemory(hProcessForInject, lpParams, &ParamsForInject, sizeof(Params), NULL);
    
    ///////////////////////////////////////////////////////////////////////////////////
    HANDLE hKernel32 = (HANDLE)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
    HANDLE hLoaderWorker = CreateRemoteThread(hProcessForInject, NULL, 0, (LPTHREAD_START_ROUTINE)hKernel32, lpProcess, 0, NULL);
    
    WaitForSingleObject(hLoaderWorker, INFINITE);
    DWORD remote_thread_result;
    GetExitCodeThread(hLoaderWorker, &remote_thread_result);
    if (remote_thread_result == NULL) {
        //cout << "Failed to inject library. Returned handle is null" << endl;
        return;
    }
    HMODULE hInjectedDll = (HMODULE)remote_thread_result;

    HMODULE hLocalDll = (HMODULE)LoadLibrary(TEXT("DllReplace.dll"));//let's inject lib to self
    FARPROC localProcOffset = GetProcAddress(hLocalDll, "ReplaceStrInject");
    if (localProcOffset == NULL) {
        //cout << "Failed to find str_change function in dll.dll library..." << endl;
        return;
    }
    ptrdiff_t cbLocalOffset = (ptrdiff_t)localProcOffset - (ptrdiff_t)hLocalDll;
    FARPROC remoteProcOffset = (FARPROC)((ptrdiff_t)hInjectedDll + cbLocalOffset);
   
    HANDLE hRemoteWorker = CreateRemoteThread(hProcessForInject, NULL, 0, (LPTHREAD_START_ROUTINE)remoteProcOffset, (LPVOID)lpParams, 0, NULL);
   ////////////////////////////////////////////////////////////////////////////////////////////////


    WaitForSingleObject(hRemoteWorker, INFINITE);
    VirtualFreeEx(hProcessForInject, lpParams, sizeof(ParamsForInject), NULL);
    VirtualFreeEx(hProcessForInject, lpProcess, strlen("DllReplace.dll") + 1, NULL);
    CloseHandle(hLoaderWorker);
    CloseHandle(hRemoteWorker);
    CloseHandle(hProcessForInject);
    FreeLibrary(hLocalDll);
}

DWORD GetProcessId() {
    DWORD processID = 0;
    HANDLE snapHandle;
    PROCESSENTRY32 processEntry = { 0 };

    if ((snapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE) {
        return 0;
    }

    processEntry.dwSize = sizeof(PROCESSENTRY32);
    Process32First(snapHandle, &processEntry);
    do {
        if (wcscmp(processEntry.szExeFile, L"inject.exe") == 0) {
            return processEntry.th32ProcessID;
        }
    } while (Process32Next(snapHandle, &processEntry));

    if (snapHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(snapHandle);
    }

    return 0;
}