#pragma once

#include <vector>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcId(const wchar_t* procName);

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

template <class val>
val readMem(HANDLE handle, DWORD addr)
{
	val x;
	ReadProcessMemory(handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
	return x;
}

template <class val>
val writeMem(HANDLE handle, DWORD addr, val x)
{
	WriteProcessMemory(handle, (LPBYTE*)addr, &x, sizeof(x), NULL);
	return 0;
}