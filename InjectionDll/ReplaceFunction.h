#pragma once
#include <Windows.h>

extern "C" _declspec(dllexport) void __stdcall ReplaceStringInMemory(DWORD PID, const char* srcString, const char* destString);
extern "C" __declspec(dllexport) void __stdcall ForDllInject(struct InjectParams* params);