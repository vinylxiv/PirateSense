// PirateSense.

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <ThirdParty/XORSTR.h>
#include <Windows.h>

#include "Utility/Logging.h"

void GetProcessID(const char* WindowTitle, DWORD& ProcessID)
{
	GetWindowThreadProcessId(FindWindow(nullptr, WindowTitle), &ProcessID);
}

bool DoesFileExist(const std::string& FileName)
{
	struct stat Buffer;
	return (stat(FileName.c_str(), &Buffer) == 0);
}

int main()
{
	PIRATESENSE_LOG(LOG_Core, ELogType::Info, xorstr_(L"Injecting..."));

	const char* DLLName = xorstr_("PirateSense.dll");
	const char* WindowTitle = xorstr_("Sea of Thieves");

	DWORD ProcessID = 0;
	char DLLPath[MAX_PATH];

	if (!DoesFileExist(DLLName))
	{
		PIRATESENSE_LOG(LOG_Core, ELogType::Error, L"DLL not found!");

		std::cin.get();

		return 0;
	}

	if (!GetFullPathName(DLLName, MAX_PATH, DLLPath, nullptr))
	{
		PIRATESENSE_LOG(LOG_Core, ELogType::Error, L"Failed to get full path!");

		std::cin.get();

		return 0;
	}

	GetProcessID(WindowTitle, ProcessID);
	if (ProcessID == 0)
	{
		PIRATESENSE_LOG(LOG_Core, ELogType::Error, L"Failed to get process ID!");

		std::cin.get();

		return 0;
	}

	const HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
	if (!ProcessHandle)
	{
		PIRATESENSE_LOG(LOG_Core, ELogType::Error, L"Failed to open handle to process!");

		std::cin.get();

		return 0;
	}

	void* AllocatedMemory = VirtualAllocEx(ProcessHandle, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!AllocatedMemory)
	{
		PIRATESENSE_LOG(LOG_Core, ELogType::Error, L"Failed to allocate memory!");

		std::cin.get();

		return 0;
	}

	if (!WriteProcessMemory(ProcessHandle, AllocatedMemory, DLLPath, MAX_PATH, nullptr))
	{
		PIRATESENSE_LOG(LOG_Core, ELogType::Error, L"Failed to write process memory!");

		std::cin.get();

		return 0;
	}

	const HANDLE ThreadHandle = CreateRemoteThread(ProcessHandle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA), AllocatedMemory, 0, nullptr);
	if (!ThreadHandle)
	{
		PIRATESENSE_LOG(LOG_Core, ELogType::Error, L"Failed to create thread!");

		std::cin.get();

		return 0;
	}

	CloseHandle(ProcessHandle);
	CloseHandle(ThreadHandle);

	PIRATESENSE_LOG(LOG_Core, ELogType::Success, xorstr_(L"Injected!"));

	// Wrapped in sub scope because I hate having "using namespace" outside of a contained scope.
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(2000ms);
	}
	
	return 0;
}
