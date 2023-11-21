#include <Windows.h>
#include <cstdlib>
#include "ReplaceFunction.h"
#include <iostream>

struct InjectParams {
	DWORD pid;
	char srcStr[10];
	char OutStr[10];
};


void ForDllInject(struct InjectParams* params) {
	std::cout << "Here injection" << std::endl;
	ReplaceStringInMemory(params->pid, params->srcStr, params->OutStr);
}

void ReplaceStringInMemory(DWORD PID, const char* srcString, const char* destString)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess)
	{
		size_t srcStrLen = strlen(srcString);
		SYSTEM_INFO systemInfo;
		MEMORY_BASIC_INFORMATION memoryInfo;
		GetSystemInfo(&systemInfo);
		char* pointer = 0;
		SIZE_T lpRead = 0;
		while (pointer < systemInfo.lpMaximumApplicationAddress)
		{
			int sz = VirtualQueryEx(hProcess, pointer, &memoryInfo, sizeof(MEMORY_BASIC_INFORMATION));
			if (sz == sizeof(MEMORY_BASIC_INFORMATION))
			{
				if ((memoryInfo.State == MEM_COMMIT) && memoryInfo.AllocationProtect == PAGE_READWRITE)
				{
					pointer = (char*)memoryInfo.BaseAddress;
					try {
						BYTE* lpData = (BYTE*)malloc(memoryInfo.RegionSize);
						if (ReadProcessMemory(hProcess, pointer, lpData, memoryInfo.RegionSize, &lpRead))
						{
							for (size_t i = 0; i < (lpRead - srcStrLen); ++i)
							{
								if (memcmp(srcString, &lpData[i], srcStrLen) == 0)
								{
									char* replaceMemory = pointer + i;
									for (int j = 0; (j < (strlen(destString)) && j < (strlen(srcString))); j++)
									{
										replaceMemory[j] = destString[j];
									}
									replaceMemory[strlen(destString)] = 0;
								}
							}
						}
						free(lpData);
					}
					catch (std::bad_alloc& e) { printf("%s\n", e.what()); }

				}
			}
			pointer += memoryInfo.RegionSize;
		}

		CloseHandle(hProcess);
	}
}