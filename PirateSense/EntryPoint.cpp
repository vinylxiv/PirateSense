// PirateSense.

#include <filesystem>
#include <fstream>
#include <iostream>
#include <kiero.h>

#include "PirateSense.h"
#include "Utility/Logging.h"
// #include "Utility/CrashHandler.h"

void Entrypoint(const HMODULE ModuleHandle)
{
	PIRATESENSE_DEVLOG(LOG_Core, LogType::Info, xorstr_(L"Hit EntryPoint..."));

	// CrashHandler::Init();
	
    PirateSense::Init();
	PirateSense::MainLoop();
	PirateSense::Shutdown();

	FreeLibraryAndExitThread(ModuleHandle, 0);
}

BOOL APIENTRY DllMain(HMODULE ModuleHandle, DWORD CallReason, LPVOID Reserved)
{
    switch (CallReason)
    {
        case DLL_PROCESS_ATTACH:
        {
			DisableThreadLibraryCalls(ModuleHandle);
   
            PirateSense::CoreModuleHandle = ModuleHandle;
   
			if (const auto ThreadHandle = CreateThread(nullptr, 0ull, reinterpret_cast<LPTHREAD_START_ROUTINE>(Entrypoint), ModuleHandle, 0ul, nullptr); ThreadHandle != nullptr)
			{
				CloseHandle(ThreadHandle);
			}

            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
        default:
        {
            break;
        }
    }

    return TRUE;
}
