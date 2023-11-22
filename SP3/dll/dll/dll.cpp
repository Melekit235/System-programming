#include "pch.h"
#include "dll.h"

void ReplaceStr(DWORD processId, const char* strForSearch, const char* strReplace)
{
	HANDLE handle;
	handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, processId);

	if (handle != NULL)
	{
		MEMORY_BASIC_INFORMATION memoryInformation;

		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);

		char* memoryData = nullptr;


		bool successExtract = true;
		char* memoryPointer = 0;
		while (memoryPointer < systemInfo.lpMaximumApplicationAddress && successExtract)
		{
			if (VirtualQueryEx(handle, memoryPointer, &memoryInformation, sizeof(memoryInformation)))
			{
				if (memoryInformation.State == MEM_COMMIT && memoryInformation.AllocationProtect == PAGE_READWRITE && memoryInformation.Protect != PAGE_GUARD)
				{
					memoryPointer = (char*)memoryInformation.BaseAddress;
					memoryData = new char[memoryInformation.RegionSize];
					SIZE_T nbR;
					SIZE_T nbW;
					//try
					//{
					if (ReadProcessMemory(handle, memoryPointer, &memoryData[0], memoryInformation.RegionSize, &nbR))
					{
						for (size_t i = 0; i < (nbR - strlen(strForSearch)); ++i)
						{
							if (memcmp(strForSearch, &memoryData[i], strlen(strForSearch)) == 0)
							{
								WriteProcessMemory(handle, (char*)memoryPointer + i, strReplace, strlen(strReplace) + 1, &nbW);
							}
						}
					}
					//}
					//catch (std::bad_alloc& e)
					//{

					//}
				}
				memoryPointer += memoryInformation.RegionSize;
			}
			else
			{
				successExtract = false;
			}
		}
	}
}

void ReplaceStrInject(Params* param) {
	DWORD processId = GetCurrentProcessId();
	const char* strForSearch = param->left;
	const char* strReplace = param->right;

	ReplaceStr(processId, strForSearch, strReplace);
};